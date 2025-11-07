# 1.3.0
* Fixed a bug where NaN values would break reported volume (#6)
* Improved performance by using radix-2 algorithm for FFT
* Reduced bundle size by enabling mangling

# 1.2.1
* Fixed a bug in score calculations for profiles with `useStandardization` enabled
* Fixed L2-norm score calculation to match `uLipSync` behaviour (#4)
* Fixed a bug in parsing binary profile files with >12 phonemes

# 1.2.0
* Improved performance, processing is now ~**77%** faster

# 1.1.2
* Fixed types folder mismatch, which prevented type information from being available

# 1.1.1
* Fixed a bug where profiles with >12 phonemes would produce incorrect results

# 1.1.0
* Improved performance, processing is now ~**7x** faster
* Added support for binary profiles for size efficiency (#1)

# 1.0.0
* Initial release of wLipSync
