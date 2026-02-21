#ifndef WAVTRACK_H
#define WAVTRACK_H

#include "AudioTrack.h"

/**
 * WAVTrack - Represents a WAV audio file with high-quality uncompressed audio
 * WAV files store raw audio data without compression, providing maximum quality
 * Students must implement all virtual functions from AudioTrack
 * 
 * Phase 4 contracts:
 * - load(): simulate deck preparation for WAV (often faster due to no decompression).
 * - analyze_beatgrid(): run immediately after load() in this assignment; can be more precise.
 * - get_quality_score(): derived from sample_rate and bit_depth (higher => better).
 * - clone(): return a deep polymorphic copy used by the mixer; source remains unchanged.
 * - get_quality_score(): function of sample_rate and bit_depth (both higher -> better).
 */
class WAVTrack : public AudioTrack {
private:
    int sample_rate;    // Samples per second: 44100 (CD), 48000 (pro), 96000+ (hi-res)
    int bit_depth;      // Bits per sample: 16 (CD), 24 (pro), 32 (float)

public:
    /**
     * Constructor for WAVTrack
     */
    WAVTrack(const std::string& title, const std::vector<std::string>& artists, 
             int duration, int bpm, int sample_rate, int bit_depth);

    // ========== TODO: IMPLEMENT VIRTUAL FUNCTIONS ==========

    /**
     * TODO: Implement load function for WAV files
     * HINT: WAV files are uncompressed, so loading might be faster
     */
    void load() override;

    /**
     * TODO: Implement beat grid analysis for WAV
     * HINT: Uncompressed audio allows more precise beat detection
     */
    void analyze_beatgrid() override;

    /**
     * TODO: Implement quality score calculation
     * HINT: Use sample rate and bit depth for quality (both higher = better)
     */
    double get_quality_score() const override;

    /**
     * TODO: Implement clone function
     * HINT: Return a unique_ptr to a new WAVTrack with same properties
     */
    PointerWrapper<AudioTrack> clone() const override;

    // Getters
    int get_sample_rate() const { return sample_rate; }
    int get_bit_depth() const { return bit_depth; }
};

#endif // WAVTRACK_H