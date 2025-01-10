const wasmModule = await WebAssembly.compileStreaming(fetch("wlipsync.wasm"));
const profile = await fetch('profile.json');
const profileJson = await profile.json();

const audioContext = new AudioContext();
await audioContext.audioWorklet.addModule('processor.js');
const processor = new AudioWorkletNode(audioContext, "processor", { processorOptions: { wasmModule, profile: profileJson } });
processor.port.onmessage = (e) => {
  outputEl.innerHTML = e.data.name;
}

const outputEl = document.getElementById('out');

window.addEventListener('click', async _ => {
  if (audioContext.state === "suspended") {
    audioContext.resume();
  }

  const stream = await navigator.mediaDevices.getUserMedia({
    audio: true,
  });
  const source = audioContext.createMediaStreamSource(stream);

  source.connect(processor)//.connect(audioContext.destination);
}, { passive: true, once: true })

