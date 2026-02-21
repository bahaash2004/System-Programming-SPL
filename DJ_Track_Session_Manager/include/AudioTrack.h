#pragma once

#include <string>
#include "PointerWrapper.h"
#include <memory>
#include <vector>
/**
 * Base class for all audio track types in the DJ library system.
 * This class demonstrates virtual functions, Rule of 5, and dynamic memory management.
 * 
 * Phase 4 contracts:
 * - load(): lightweight, format-specific preparation when a track is assigned to a deck;
 *   sets readiness state and may log; does not start playback.
 * - analyze_beatgrid(): runs immediately after load() in this assignment to make BPM
 *   available for compatibility checks; results may be cached per instance.
 * - clone(): used at the cache→mixer boundary; mixer always receives a polymorphic clone
 *   and owns it; the cache retains its own copy.
 * 
 */
class AudioTrack {
protected:
    std::string title;
    std::vector<std::string> artists;
    int duration_seconds;
    int bpm;  // beats per minute for mixing
    double* waveform_data;  // Dynamic array for audio analysis
    size_t waveform_size;   // Size of the waveform array

public:
    /**
     * Constructor - initializes basic track information
     * Students should call this from derived class constructors
     */
    AudioTrack(const std::string& title, const std::vector<std::string>& artists, 
               int duration, int bpm, size_t waveform_samples = 1000);

    // ========== RULE OF 5 - STUDENTS MUST IMPLEMENT ALL OF THESE ==========

    /**
     * TODO: Implement destructor
     * HINT: Don't forget to clean up waveform_data!
     */
    virtual ~AudioTrack();

    /**
     * TODO: Implement copy constructor
     * HINT: Deep copy the waveform_data array
     */
    AudioTrack(const AudioTrack& other);

    /**
     * TODO: Implement copy assignment operator
     * HINT: Check for self-assignment, clean up existing data, then deep copy
     */
    AudioTrack& operator=(const AudioTrack& other);

    /**
     * TODO: Implement move constructor
     * HINT: Transfer ownership without copying, leave other in valid state
     */
    AudioTrack(AudioTrack&& other) noexcept;

    /**
     * TODO: Implement move assignment operator
     * HINT: Clean up current data, steal from other, reset other
     */
    AudioTrack& operator=(AudioTrack&& other) noexcept;

    // ========== VIRTUAL FUNCTIONS FOR POLYMORPHISM ==========

    /**
     * Pure virtual function - load track from file
     * Each format implements this differently
     * E.g., MP3 might decode headers, WAV might read chunks
     * This method sets up the track for playback
     * Practically, it will read metadata and print information
     */
    virtual void load() = 0;

    /**
     * Pure virtual function - analyze beat grid for mixing
     * Different formats may have different analysis methods
     * mostly for demonstration; in real systems this might be more complex
     * This method simply prints a message for demonstration purposes
     */
    virtual void analyze_beatgrid() = 0;

    /**
     * Pure virtual function - calculate audio quality score
     * MP3 uses bitrate, WAV uses sample rate, etc.
     */
    virtual double get_quality_score() const = 0;

    /**
     * Pure virtual function - create a copy of this track
     * Demonstrates the prototype pattern with polymorphism
     */
    virtual PointerWrapper<AudioTrack> clone() const = 0;

    /**
     * Function to get a copy of the waveform data
     */
    void get_waveform_copy(double* buffer, size_t buffer_size) const;

    void set_bpm(int new_bpm){bpm=new_bpm;}
    
    // ========== ACCESSOR FUNCTIONS ==========
    std::string get_title() const { return title; }
    int get_bpm() const { return bpm; }
    int get_duration() const { return duration_seconds; }
    std::vector<std::string> get_artists() const { return artists; }
};
