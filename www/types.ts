export interface ProfileMFCC {
    name: string;
    mfccCalibrationDataList: Array<{ array: number[] }>;

    // Precomputed values
    values?: Array<number>;
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

    // Precomputed values
    means?: Array<number>;
    stdDevs?: Array<number>;
};

export interface WorkletMessage {
    timestamp: number;
    index: number;
    name: string;
    volume: number;
};
