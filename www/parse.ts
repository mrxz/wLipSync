import type { Profile } from "./types";

const textDecoder = new TextDecoder();

export function parseBinaryProfile(buffer: ArrayBuffer): Profile {
    const data = new DataView(buffer);
    let offset = 0;
    const readFloat = () => {
        const value = data.getFloat32(offset);
        offset += 4;
        return value;
    };
    const readUint8 = () => data.getUint8(offset++);
    const readInt = () => {
        const value = data.getInt32(offset);
        offset += 4;
        return value;
    };
    const readChar = () => {
        return String.fromCharCode(data.getUint8(offset++));
    };
    const readString = () => {
        const size = readUint8();
        const value = textDecoder.decode(buffer.slice(offset, offset + size));
        offset += size;
        return value;
    };
    const readFloats = (count: number) => {
        return [...new Array(count)].map(_ => readFloat());
    }

    // Verify signature
    if(readChar() !== 'W' || readChar() !== 'L' || readChar() !== 'I' || readChar() !== 'P') {
        throw new Error('Invalid file signature for binary wLipSync profile');
    }

    // Read base properties
    const profile: Profile = {
        targetSampleRate: readInt(),
        sampleCount: readInt(),
        melFilterBankChannels: readUint8(),
        compareMethod: readUint8() as 0|1|2,
        mfccNum: readUint8(),
        mfccDataCount: readUint8(),
        useStandardization: readUint8() === 1,
        mfccs: []
    };
    // Read phoneme data
    for(let phoneme = 0; phoneme < profile.mfccNum; phoneme++) {
        profile.mfccs.push({
            name: '',
            mfccCalibrationDataList: [],
            values: readFloats(12)
        });
    }
    profile.means = readFloats(12);
    profile.stdDevs = readFloats(12);
    for(let phoneme = 0; phoneme < profile.mfccNum; phoneme++) {
        profile.mfccs[phoneme].name = readString();
    }
    console.log(profile);

    return profile;
}