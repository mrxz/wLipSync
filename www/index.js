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
  profileJson.mfccDataCount
);

const mfccs =  profileJson.mfccs;
const mfccData = new DataView(memory.buffer, mfccPtr, profileJson.mfccNum * profileJson.mfccDataCount * 4);
for(let i = 0; i < mfccs.length; i++) {
  const mfcc = mfccs[i];
  for(let j = 0; j < profileJson.mfccDataCount; j++) {
    let sum = 0;
    for(let x = 0; x < mfcc.mfccCalibrationDataList[j].array.length; x++) {
      sum += mfcc.mfccCalibrationDataList[j].array[x];
    }

    mfccData.setFloat32((i * profileJson.mfccDataCount + j) * 4, sum / mfcc.mfccCalibrationDataList[j].array.length, true);
  }
}
console.log(mfccPtr, mfccData);

console.log(wasmProgram.instance.exports.execute())
