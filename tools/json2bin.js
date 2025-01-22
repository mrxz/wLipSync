import { readFileSync, writeFileSync } from 'fs';

const args = process.argv.slice(2);
if(args.length !== 2) {
    console.log('Usage: json2bin <profile.json> <profile.bin>')
    process.exit(-1);
}

// Load wlipsync.wasm
const memory = new WebAssembly.Memory({ initial: 4 });
const importObject = { env: { memory: memory } };
const wasmBuffer = readFileSync(new URL('../dist/wlipsync.wasm', import.meta.url));
const { instance } = await WebAssembly.instantiate(wasmBuffer, importObject);
const exports = instance.exports;

// Load profile
const profile = JSON.parse(readFileSync(args[0], 'utf-8'));

const MAX_NAME_LENGTH = 128;
const MFCC_NUM = 12;
const MAX_PHONEMES = 128;
const MAX_SIZE =
    /* Signature */
    4 +
    /* Properties */
    7 * 4 +
    /* MFCC averages x Max phonemes */
    MFCC_NUM * MAX_PHONEMES * 4 +
    /* Means */
    MFCC_NUM * 4 +
    /* Standard deviations */
    MFCC_NUM * 4 +
    /* Names */
    (4 + MAX_PHONEMES * MAX_NAME_LENGTH);

const buffer = new ArrayBuffer(MAX_SIZE);
const data = new DataView(buffer);

let offset = 0;
const writeFloat = (float) => {
    data.setFloat32(offset, float);
    offset += 4;
};
const writeInt = (int) => {
    data.setInt32(offset, int);
    offset += 4;
};
const writeUint8 = (int) => {
    data.setUint8(offset++, int);
};
const writeString = (str, prefixLength = true) => {
    if(prefixLength) {
        writeUint8(str.length);
    }
    for(let i = 0; i < str.length; i++) {
        data.setUint8(offset++, str.charCodeAt(i));
    }
};

// Signature
writeString('WLIP', false);

writeInt(profile.targetSampleRate);
writeInt(profile.sampleCount);
writeUint8(profile.melFilterBankChannels);
writeUint8(profile.compareMethod);
writeUint8(profile.mfccs.length);
writeUint8(profile.mfccDataCount);
writeUint8(profile.useStandardization ? 1 : 0);

// Compute averages
const mfccPtr = exports.load_profile(
    profile.targetSampleRate,
    profile.sampleCount,
    profile.melFilterBankChannels,
    profile.compareMethod,
    profile.mfccs.length,
    profile.mfccDataCount,
    profile.useStandardization ? 1 : 0
);
const mfccData = new DataView(memory.buffer, mfccPtr, profile.mfccs.length * profile.mfccDataCount * MFCC_NUM * 4);
let index = 0;
for (const phoneme of profile.mfccs) {
    for (const sampleList of phoneme.mfccCalibrationDataList) {
        for (const sample of sampleList.array) {
            mfccData.setFloat32(index, sample, true);
            index += 4;
        }
    }
}

exports.precompute_profile();
const profilePtrs = exports.get_profile_ptrs();
const profilePtrsView = new DataView(memory.buffer, profilePtrs, 3 * 4);

// Averages
{
    const view = new DataView(memory.buffer, profilePtrsView.getInt32(0, true), profile.mfccs.length * MFCC_NUM * 4);
    for(let phoneme = 0; phoneme < profile.mfccs.length; phoneme++) {
        for(let i = 0; i < MFCC_NUM; i++) {
            writeFloat(view.getFloat32((phoneme * MFCC_NUM + i) * 4, true));
        }
    }
}

// Means
{
    const view = new DataView(memory.buffer, profilePtrsView.getInt32(4, true), MFCC_NUM * 4);
    for(let i = 0; i < MFCC_NUM; i++) {
        writeFloat(view.getFloat32(i * 4, true));
    }
}

// Standard deviations
{
    const view = new DataView(memory.buffer, profilePtrsView.getInt32(8, true), MFCC_NUM * 4);
    for(let i = 0; i < MFCC_NUM; i++) {
        writeFloat(view.getFloat32(i * 4, true));
    }
}

// Names
{
    for(let phoneme = 0; phoneme < profile.mfccs.length; phoneme++) {
        const name = profile.mfccs[phoneme].name;
        writeString(name);
    }
}

// Output
writeFileSync(args[1], new Uint8Array(buffer, 0, offset));