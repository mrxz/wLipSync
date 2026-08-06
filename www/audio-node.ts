import type { Profile, WorkletMessage } from "./types";
import { smoothDamp } from "./utils";

export let configuration: { wasmModule: WebAssembly.Module|undefined } = { wasmModule: undefined };

/**
 * The AudioNode that performs the lip sync logic. The volume and viseme weights
 * can be read through the `volume` and `weights` properties.
 *
 * NOTE: This node itself does not generate any output. For simultaneous
 * lipsync processing and audio playback, connect the source node to this node
 * _and_ to the desired sink node(s).
 */
export class WLipSyncAudioNode extends AudioWorkletNode {
    private openCloseVelocity = 0;
    private lastTimestamp = 0;
    private weightVelocities: Record<string, number> = {};

    /**
     * Configurable lower bound for the volume. Anything at or below this
     * value will be treated as silence.
     *
     * NOTE: This is in log10(RMS)
     * @default -2.5
     */
    public minVolume = -2.5;
    /**
     * Configurable upper bound for the volume. Anything at or above this
     * value won't result in increased volume or viseme weight scores.
     *
     * NOTE: This is in log10(RMS)
     * @default -1.5
     */
    public maxVolume = -1.5;
    /**
     * Smoothing term used to configure the smooth damping that is applied
     * to both the output volume and viseme weights. Higher values result
     * in smoother changes, whereas lower values result in quicker changes.
     *
     * @default 0.05
     */
    public smoothness = 0.05;

    /**
     * The latest recorded volume in the range [0-1].
     * Both bounds are determined by `minVolume` and `maxVolume`.
     */
    public volume = 0;
    /**
     * Latest recorded weights per viseme. The values are in the range [0-1]
     * and smooth damping is applied over time, controlled by the `smoothness`
     * property.
     *
     * NOTE: This is purely an output of the node, mutating this property has no
     *       effect and will get overwritten during the next update.
     */
    public readonly weights: Record<string, number> = {};

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

        // Treat NaN values as 0.0 volume, these can occur when input contains NaN values.
        const rawVolume = Number.isNaN(event.data.volume) ? 0.0 : event.data.volume;
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

    public get blockSize() {
        return (this.parameters as Map<string, AudioParam>).get('blockSize')!.value
    }

    public set blockSize(value: number) {
        (this.parameters as Map<string, AudioParam>).get('blockSize')!.setValueAtTime(value, this.context.currentTime);
    }
}