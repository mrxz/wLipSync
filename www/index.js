var memoryView;

const memory = new WebAssembly.Memory({ initial:32 });
memoryView = new Uint8Array(memory.buffer);

const textDecoder = new TextDecoder();

const importObject = {
  JS: {
    js_print: function(str, len) {
      console.log(textDecoder.decode(memoryView.slice(str, str + len)));
    },
  },
  env: {
    memory: memory
  }
};

const wasmProgram = await WebAssembly.instantiateStreaming(fetch("wlipsync.wasm"), importObject);
const exports = wasmProgram.instance.exports;

exports.main();

const profile = await fetch('profile.json');
const profileJson = await profile.json();
console.log(profileJson);

const mfccPtr = exports.load_profile(
  profileJson.targetSampleRate,
  profileJson.sampleCount,
  profileJson.melFilterBankChannels,
  profileJson.compareMethod,
  profileJson.mfccNum,
  profileJson.mfccDataCount,
  profileJson.useStandardization ? 1 : 0
);

const mfccs =  profileJson.mfccs;
const mfccData = new DataView(memory.buffer, mfccPtr, profileJson.mfccNum * profileJson.mfccDataCount * 12 * 4);

let index = 0;
for(const phoneme of mfccs) {
  for(const sampleList of phoneme.mfccCalibrationDataList) {
    for(const sample of sampleList.array) {
      mfccData.setFloat32(index, sample, true);
      index += 4;
    }
  }
}
console.log(mfccData);
exports.precompute_profile();

console.log(wasmProgram.instance.exports.execute())
