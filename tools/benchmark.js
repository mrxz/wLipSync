import { run, bench } from 'mitata';
import { readFileSync } from 'fs';
import profile from '../example/profile.json' with { type: "json" };

// Load wlipsync.wasm
const memory = new WebAssembly.Memory({ initial: 4 });
const importObject = { env: { memory: memory } };
const wasmBuffer = readFileSync(new URL('../www/public/wlipsync.wasm', import.meta.url));
const { instance } = await WebAssembly.instantiate(wasmBuffer, importObject);
const exports = instance.exports;

// Initialize wLipSync
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
const mfccData = new DataView(memory.buffer, mfccPtr, profile.mfccNum * profile.mfccDataCount * 12 * 4);

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
exports.set_input(48_000);

// Randomize input buffer
const inputBuffer = new DataView(memory.buffer, exports.get_input_buffer(), exports.get_input_buffer_size() * 4);
for(let i = 0; i < inputBuffer.byteLength; i += 4) {
  inputBuffer.setFloat32(i, (Math.random() - 0.5) * 5.0);
}

// Benchmark
bench('execute', () => exports.execute(0));

await run();