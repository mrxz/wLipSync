import type { Profile } from "./types.js";
import { WLipSyncAudioNode } from "./audio-node.js";

// NOTE: This convenience method is only needed for the single-file approach to
//       lazy-load the processor in the audio worklet. For consistency it exists here
//       as well.
export async function createWLipSyncNode(audioContext: AudioContext, profile: Profile) {
    return new WLipSyncAudioNode(audioContext, profile);
}

export type * from './types.js';
export { configuration, WLipSyncAudioNode } from './audio-node.js';