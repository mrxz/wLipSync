import type { Profile } from "./types.js";
import wasm from "./public/wlipsync.wasm?url";
import { configuration, WLipSyncAudioNode } from "./audio-node.js";

configuration.wasmModule = await WebAssembly.compileStreaming(fetch(wasm));

export async function createWLipSyncNode(audioContext: AudioContext, profile: Profile) {
    // Crude check to see if module has already loaded, otherwise lazy loads it.
    try {
        return new WLipSyncAudioNode(audioContext, profile);
    } catch {
        await audioContext.audioWorklet.addModule(new URL('./public/audio-processor.js', import.meta.url));
        return new WLipSyncAudioNode(audioContext, profile);
    }
}

export type * from './types.js';
export * from './parse.js';
export { WLipSyncAudioNode } from './audio-node.js';



