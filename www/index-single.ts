import type { Profile } from "./types.js";
import wasm from "./public/wlipsync.wasm?url";
import { configuration, WLipSyncAudioNode } from "./audio-node.js";

configuration.wasmModule = await WebAssembly.compileStreaming(fetch(wasm));

/**
 * Creates a WLipSyncAudioNode for a given audio context and profile.
 * This method should always be used to construct the nodes, as it ensures
 * the library is properly initialized.
 *
 * @param audioContext The AudioContext to create the node in
 * @param profile The lipsync profile to use for the node
 * @returns {Promise<WLipSyncAudioNode>}
 */
export async function createWLipSyncNode(audioContext: AudioContext, profile: Profile): Promise<WLipSyncAudioNode> {
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



