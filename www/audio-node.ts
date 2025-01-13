import type { Profile, WorkletMessage } from "./types";
import { smoothDamp } from "./utils";

export let configuration: { wasmModule: WebAssembly.Module|undefined } = { wasmModule: undefined };

export class WLipSyncAudioNode extends AudioWorkletNode {
    private openCloseVelocity = 0;
    private lastTimestamp = 0;
    private weightVelocities: Record<string, number> = {};

    public minVolume = -2.5;
    public maxVolume = -1.5;
    public smoothness = 0.05;

    public volume = 0;
    public weights: Record<string, number> = {};

    constructor(context: BaseAudioContext, profile: Profile, wasmModule: WebAssembly.Module | undefined = configuration.wasmModule) {
        super(context, 'wlipsync-processor', { processorOptions: { wasmModule: wasmModule, profile }});

        for(const mfcc of profile.mfccs) {
            this.weights[mfcc.name] = 0;
            this.weightVelocities[mfcc.name] = 0;
        }
        this.port.onmessage = this.onMessage.bind(this);
    }

    private onMessage(event: MessageEvent<WorkletMessage>) {
        const deltaTime = event.data.timestamp - this.lastTimestamp;
        this.lastTimestamp = event.data.timestamp;

        const rawVolume = event.data.volume;
        let normVolume = Math.log10(rawVolume);
        normVolume = (normVolume - this.minVolume) / (this.maxVolume - this.minVolume);
        normVolume = Math.max(Math.min(normVolume, 1), 0);

        // Update volume
        [this.volume, this.openCloseVelocity] = smoothDamp(this.volume, normVolume, this.openCloseVelocity, this.smoothness, deltaTime);

        // Update weights
        for(const key in this.weights) {
            const targetWeight = key === event.data.name ? 1 : 0;
            let weightVel = this.weightVelocities[key];
            [this.weights[key], weightVel] = smoothDamp(this.weights[key], targetWeight, weightVel, this.smoothness, deltaTime);
            this.weightVelocities[key] = weightVel;
        }
    }
}