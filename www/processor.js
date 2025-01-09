const memory = new WebAssembly.Memory({ initial: 32 });
const importObject = {
    JS: {
        js_print: function (str, len) {},
    },
    env: {
        memory: memory
    }
};

class Processor extends AudioWorkletProcessor {
    constructor() {
        super();

        this.inputBufferIndex = -1;

        // Listen for initialization message
        this.port.onmessage = async (event) => {
            const { wasmModule, profile } = event.data;
            const instance = await WebAssembly.instantiate(wasmModule, importObject);
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

            const mfccs = profile.mfccs;
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
            console.log(mfccData);
            exports.precompute_profile();
            exports.set_input(48_000); // FIXME: Fetch this from the AudioContext somehow

            this.inputBufferPtr = exports.get_input_buffer();
            this.inputBufferSize = exports.get_input_buffer_size();
            this.inputBuffer = new DataView(memory.buffer, this.inputBufferPtr, this.inputBufferSize * 4);
            this.inputBufferIndex = 0;
        }
    }

    process([input], [output]) {
        const monoInput = input[0];

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
            this.inputBufferIndex = (this.inputBufferIndex + 1) % this.inputBufferSize;
            this.inputBuffer.setFloat32(this.inputBufferIndex * 4, monoInput[i], true);
        }

        const index = this.exports.execute(this.inputBufferIndex);
        console.log(index, this.inputBufferIndex);

        return true;
    }
}

registerProcessor("processor", Processor);
