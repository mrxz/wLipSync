import type { Profile } from "./types.js";
import { WLipSyncAudioNode } from "./audio-node.js";

// NOTE: This convenience method is only needed for the single-file approach to
//       lazy-load the processor in the audio worklet. For consistency it exists here
//       as well.

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
    return new WLipSyncAudioNode(audioContext, profile);
}

export type * from './types.js';
export * from './parse.js';
export { configuration, WLipSyncAudioNode } from './audio-node.js';