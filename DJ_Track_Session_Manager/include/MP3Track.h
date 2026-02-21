#ifndef MP3TRACK_H
#define MP3TRACK_H

#include "AudioTrack.h"

/**
 * MP3Track - Represents an MP3 audio file with lossy compression
 * MP3 uses perceptual coding to reduce file size while maintaining acceptable quality
 * Students must implement all virtual functions from AudioTrack
 * 
 * Phase 4 contracts:
 * - load(): simulate deck preparation (format-specific message); does not start playback.
 * - analyze_beatgrid(): run immediately after load() in this assignment for compatibility checks.
 * - get_quality_score(): derived from bitrate (e.g., normalized by 320kbps).
 * - clone(): return a deep polymorphic copy used by the mixer; source remains unchanged.
 */
class MP3Track : public AudioTrack {
private:
    int bitrate;        // Compression level: 128, 192, 320 kbps (higher = better quality)
    bool has_id3_tags;  // Whether file contains ID3 metadata (artist, album, etc.)

public:
    /**
     * Constructor for MP3Track
     */
    MP3Track(const std::string& title, const std::vector<std::string>& artists, 
             int duration, int bpm, int bitrate, bool has_tags = true);

    // ========== TODO: IMPLEMENT VIRTUAL FUNCTIONS ==========

    /**
     * TODO: Implement load function for MP3 files
     * HINT: Print loading message specific to MP3 format
     */
    void load() override;

    /**
     * TODO: Implement beat grid analysis for MP3
     * HINT: MP3 analysis might be less precise than WAV
     */
    void analyze_beatgrid() override;

    /**
     * TODO: Implement quality score calculation
     * HINT: Use bitrate to determine quality (higher bitrate = better quality)
     */
    double get_quality_score() const override;

    /**
     * TODO: Implement clone function
     * HINT: Return a unique_ptr to a new MP3Track with same properties
     */
    PointerWrapper<AudioTrack> clone() const override;

    // Getters
    int get_bitrate() const { return bitrate; }
    bool has_tags() const { return has_id3_tags; }
};

#endif // MP3TRACK_H