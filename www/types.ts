export interface ProfileMFCC {
    name: string;
    mfccCalibrationDataList: Array<{ array: number[] }>;
}

export type Profile = {
    targetSampleRate: number;
    sampleCount: number;
    melFilterBankChannels: number;
    compareMethod: 0 | 1 | 2;
    mfccNum: number;
    mfccDataCount: number;
    useStandardization: boolean;
    mfccs: Array<ProfileMFCC>;
};

export interface WorkletMessage {
    timestamp: number;
    index: number;
    name: string;
    volume: number;
};
