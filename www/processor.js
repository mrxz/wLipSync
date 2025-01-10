const memory = new WebAssembly.Memory({ initial: 32 });
const importObject = {
    env: {
        memory: memory
    }
};

class Processor extends AudioWorkletProcessor {
    constructor(options) {
        super();

        this.inputBufferIndex = -1;

        const { wasmModule, profile } = options.processorOptions;
        WebAssembly.instantiate(wasmModule, importObject).then(instance => {
            const exports = this.exports = instance.exports;

            const mfccPtr = exports.load_profile(
                profile.targetSampleRate,
                profile.sampleCount,
                profile.melFilterBankChannels,
                profile.compareMethod,
                profile.mfccNum,
                profile.mfccDataCount,
                profile.useStandardization ? 1 : 0
            );

            const mfccs = this.mfccs = profile.mfccs;
            const mfccData = new DataView(memory.buffer, mfccPtr, profile.mfccNum * profile.mfccDataCount * 12 * 4);

            let index = 0;
            for (const phoneme of mfccs) {
                for (const sampleList of phoneme.mfccCalibrationDataList) {
                    for (const sample of sampleList.array) {
                        mfccData.setFloat32(index, sample, true);
                        index += 4;
                    }
                }
            }
            console.log(mfccData, sampleRate);
            exports.precompute_profile();
            exports.set_input(sampleRate);

            this.inputBufferPtr = exports.get_input_buffer();
            this.inputBufferSize = exports.get_input_buffer_size();
            this.inputBuffer = new DataView(memory.buffer, this.inputBufferPtr, this.inputBufferSize * 4);
            this.inputBufferIndex = 0;
            this.lastIndex = 0;
        });
    }

    process([input], [output]) {
        const monoInput = input[0];
        if (!monoInput) {
            return true;
        }

        // Pass-through
        for (let channel = 0; channel < output.length; channel++) {
            output[channel].set(input[channel]);
        }

        // Populate ring buffer
        if(this.inputBufferIndex === -1) {
            return true;
        }

        // Process quantum
        for(let i = 0; i < monoInput.length; i++) {
            this.inputBuffer.setFloat32(this.inputBufferIndex * 4, monoInput[i], true);
            this.inputBufferIndex = (this.inputBufferIndex + 1) % this.inputBufferSize;

            if((this.inputBufferIndex + this.inputBufferSize - this.lastIndex) % this.inputBufferSize === 735 /*3072*/) {
                const index = this.exports.execute(this.inputBufferIndex);
                this.port.postMessage({ index, name: this.mfccs[index].name });
                this.lastIndex = this.inputBufferIndex;
            }
        }

        return true;
    }
}

registerProcessor("processor", Processor);
