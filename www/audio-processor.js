const memory = new WebAssembly.Memory({ initial: 4 });
const importObject = {
    env: {
        memory: memory
    }
};

class Processor extends AudioWorkletProcessor {
    inputBufferIndex = -1;
    inputBufferPtr = -1;
    inputBufferSize = -1;
    /** @type {Float32Array|null} */
    inputBuffer = null;
    lastIndex = 0;
    volumePtr = -1;
    volumeView = null;
    mfccs = [];
    exports = null;

    constructor(options) {
        super();

        const { wasmModule, profile } = options.processorOptions;
        WebAssembly.instantiate(wasmModule, importObject).then(instance => {
            const exports = this.exports = instance.exports;

            const phonemeCount = profile.mfccs.length;
            const mfccPtr = exports.load_profile(
                profile.targetSampleRate,
                profile.sampleCount,
                profile.melFilterBankChannels,
                profile.compareMethod,
                phonemeCount,
                profile.mfccDataCount,
                profile.useStandardization ? 1 : 0
            );

            const mfccs = this.mfccs = profile.mfccs;
            if(!profile.means) {
                // Upload raw mfcc calibration data and precompute profile
                const mfccData = new DataView(memory.buffer, mfccPtr, phonemeCount * profile.mfccDataCount * 12 * 4);
                let index = 0;
                for(const phoneme of mfccs) {
                    for(const sampleList of phoneme.mfccCalibrationDataList) {
                        for(const sample of sampleList.array) {
                            mfccData.setFloat32(index, sample, true);
                            index += 4;
                        }
                    }
                }
                exports.precompute_profile();
            } else {
                // Profile has already been precomputed, upload values directly
                const profilePtrs = exports.get_profile_ptrs();
                const profilePtrsView = new DataView(memory.buffer, profilePtrs, 3 * 4);
                const mfccsView = new DataView(memory.buffer, profilePtrsView.getInt32(0, true), phonemeCount * 12 * 4);
                for(let phoneme = 0; phoneme < phonemeCount; phoneme++) {
                    for(let i = 0; i < 12; i++) {
                        mfccsView.setFloat32((phoneme * 12 + i) * 4, mfccs[phoneme].values[i], true);
                    }
                }
                const meansView = new DataView(memory.buffer, profilePtrsView.getInt32(4, true), 12 * 4);
                const stdDevsView = new DataView(memory.buffer, profilePtrsView.getInt32(8, true), 12 * 4);
                for(let i = 0; i < 12; i++) {
                    meansView.setFloat32(i * 4, profile.means[i], true);
                    stdDevsView.setFloat32(i * 4, profile.stdDevs[i], true);
                }
            }
            exports.set_input(sampleRate);

            this.inputBufferPtr = exports.get_input_buffer();
            this.inputBufferSize = exports.get_input_buffer_size();
            this.inputBuffer = new Float32Array(memory.buffer, this.inputBufferPtr, this.inputBufferSize);
            this.inputBufferIndex = 0;
            this.lastIndex = 0;
            this.volumePtr = exports.get_volume_ptr();
            this.volumeView = new DataView(memory.buffer, this.volumePtr, 4);
        });
    }

    static get parameterDescriptors() {
        return [{ name: "blockSize", defaultValue: 512, minValue: 128, automationRate: "k-rate" }];
    }

    process(input, output, parameters) {
        const monoInput = input[0][0];
        if(!monoInput) {
            return true;
        }

        // Populate ring buffer
        if(this.inputBufferIndex === -1) {
            return true;
        }

        // Process quantum
        for(let i = 0; i < monoInput.length; i++) {
            this.inputBuffer[this.inputBufferIndex] = monoInput[i];
            this.inputBufferIndex = (this.inputBufferIndex + 1) % this.inputBufferSize;
        }

        if((this.inputBufferIndex + this.inputBufferSize - this.lastIndex) % this.inputBufferSize === parameters.blockSize[0]) {
            const index = this.exports.execute(this.inputBufferIndex);
            const volume = this.volumeView.getFloat32(0, true);

            this.port.postMessage({ timestamp: currentTime, index, name: this.mfccs[index].name, volume });
            this.lastIndex = this.inputBufferIndex;
        }

        return true;
    }
}

registerProcessor("wlipsync-processor", Processor);
