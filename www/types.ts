/**
 * Singular MFCC entry of a wLipSync profile.
 */
export interface ProfileMFCC {
    /**
     * Name associated with this MFCC. Commonly the corresponding viseme.
     * This name does not have to be unique, allowing multiple MFCC for different
     * phonemes to map to the same viseme.
     */
    name: string;
    /**
     * List of data points created during calibration.
     * The length of the list is the `mfccDataCount` of the profile.
     */
    mfccCalibrationDataList: Array<{ array: number[] }>;

    /**
     * Precomputed MFCC coefficients. Present in case of loading from binary format.
     */
    values?: Array<number>;
}

/**
 * wLipSync profile
 */
export type Profile = {
    /**
     * The sample rate to which the incoming audio is downsampled.
     */
    targetSampleRate: number;
    /**
     * The number of samples (after downsampling) to process.
     */
    sampleCount: number;
    /**
     * The number of Mel Filter Bank channels.
     */
    melFilterBankChannels: number;
    /**
     * Which scoring metric to use for comparing between MFCC.
     * Either L1_NORM (0), L2_NORM (1) or COSINE_SIMILARITY (2).
     */
    compareMethod: 0 | 1 | 2;
    /**
     * The number of MFCC in the profile.
     */
    mfccNum: number;
    /**
     * The number of data records per MFCC.
     */
    mfccDataCount: number;
    /**
     * Whether or not to standardize the coefficients of each MFCC.
     */
    useStandardization: boolean;
    /**
     * The MFCC entries of this profile.
     */
    mfccs: Array<ProfileMFCC>;

    /**
     * Precomputed means per MFCC entry. Present in case of loading from binary format.
     */
    means?: Array<number>;
    /**
     * Precomputed standard deviations per MFCC entry. Present in case of loading from binary format.
     */
    stdDevs?: Array<number>;
};

export interface WorkletMessage {
    timestamp: number;
    index: number;
    name: string;
    volume: number;
};
