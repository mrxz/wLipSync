import type { Profile } from "./types";
import wasm from "./wlipsync.wasm?url";
import { configuration, WLipSyncAudioNode } from "./audio-node";

configuration.wasmModule = await WebAssembly.compileStreaming(fetch(wasm));

export async function createWLipSyncNode(audioContext: AudioContext, profile: Profile) {
    // Crude check to see if module has already loaded, otherwise lazy loads it.
    try {
        return new WLipSyncAudioNode(audioContext, profile);
    } catch {
        await audioContext.audioWorklet.addModule(new URL('./audio-processor.js', import.meta.url));
        return new WLipSyncAudioNode(audioContext, profile);
    }
}

export { WLipSyncAudioNode } from './audio-node';



