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
wasmProgram.instance.exports.main();

