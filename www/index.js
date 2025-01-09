const wasmModule = await WebAssembly.compileStreaming(fetch("wlipsync.wasm"));
const profile = await fetch('profile.json');
const profileJson = await profile.json();

const audioContext = new AudioContext();
await audioContext.audioWorklet.addModule('processor.js');
const processor = new AudioWorkletNode(audioContext, "processor");
processor.port.postMessage({ wasmModule, profile: profileJson });

window.addEventListener('click', async _ => {
  if (audioContext.state === "suspended") {
    audioContext.resume();
  }

  const stream = await navigator.mediaDevices.getUserMedia({
    audio: true,
  });
  const source = audioContext.createMediaStreamSource(stream);

  source.connect(processor).connect(audioContext.destination);

}, false)