import { readFileSync } from 'fs';
import wavefile from 'wavefile';

const BLOCK_SIZE = 1024;

const args = process.argv.slice(2);
if(args.length !== 2) {
    console.log('Usage: bake <profile.json> <input.wav>')
    process.exit(-1);
}

// Load wlipsync.wasm
const memory = new WebAssembly.Memory({ initial: 4 });
const importObject = { env: { memory: memory } };
const wasmBuffer = readFileSync(new URL('../dist/wlipsync.wasm', import.meta.url));
const { instance } = await WebAssembly.instantiate(wasmBuffer, importObject);
const exports = instance.exports;

// Initialize wLipSync
const profile = JSON.parse(readFileSync(args[0], 'utf-8'));
const mfccPtr = exports.load_profile(
    profile.targetSampleRate,
    profile.sampleCount,
    profile.melFilterBankChannels,
    profile.compareMethod,
    profile.mfccNum,
    profile.mfccDataCount,
    profile.useStandardization ? 1 : 0
);

const mfccs = profile.mfccs;
const mfccData = new DataView(memory.buffer, mfccPtr, profile.mfccs.length * profile.mfccDataCount * 12 * 4);

let index = 0;
for(const phoneme of mfccs) {
    for(const sampleList of phoneme.mfccCalibrationDataList) {
        for(const sample of sampleList.array) {
            mfccData.setFloat32(index, sample, true);
            index += 4;
        }
    }
}
exports.precompute_profile();

// Read .wav file and convert it to required format
const buffer = readFileSync(args[1]);
const wav = new wavefile.WaveFile(buffer);
wav.toBitDepth('32f');
const audioData = wav.getSamples();
const { numChannels, sampleRate } = wav.fmt;
const monoSamples = numChannels > 1 ? audioData[0] : audioData;

exports.set_input(sampleRate);

// Process
const volumeView = new DataView(memory.buffer, exports.get_volume_ptr(), 4);
const inputBufferSize = exports.get_input_buffer_size();
const inputBuffer = new DataView(memory.buffer, exports.get_input_buffer(), inputBufferSize * 4);
let inputBufferIndex = 0;
let lastIndex = 0;
for(let i = 0; i < monoSamples.length; i++) {
  inputBuffer.setFloat32(inputBufferIndex*4, monoSamples[i], true);
  inputBufferIndex = (inputBufferIndex + 1) % inputBufferSize;

  if((inputBufferIndex + inputBufferSize - lastIndex) % inputBufferSize === BLOCK_SIZE) {
    const index = exports.execute(inputBufferIndex);
    const volume = volumeView.getFloat32(0, true);

    console.log(mfccs[index].name, volume);
    lastIndex = inputBufferIndex;
  }
}

