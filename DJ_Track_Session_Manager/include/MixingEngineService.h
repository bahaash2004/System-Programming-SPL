#ifndef MIXINGENGINESERVICE_H
#define MIXINGENGINESERVICE_H

#include "AudioTrack.h"
#include <string>

// Service responsible for deck operations and track analysis
// Phase 4 binding:
// - Enforces instant transitions and deck alternation policy.
// - After loading to a deck: call track.load(); then analyze_beatgrid(); then switch active deck.
// - The previously active deck becomes finished and is unloaded immediately.
class MixingEngineService {
private:
    AudioTrack* decks[2];
    size_t active_deck;
    bool auto_sync;
    int bpm_tolerance;
public:
    MixingEngineService();
    ~MixingEngineService();

    MixingEngineService(const MixingEngineService&) = delete;
    MixingEngineService& operator=(const MixingEngineService&) = delete;

    /** Contract: Load a track to the next deck per instant-transition policy
     * - @param track: reference to a cached track to be cloned for the mixer
     * - @return: index of the deck the track was loaded to (0 or 1), or -1 on failure.
     * - @brief: This function clones the track, unloads the target deck if needed, loads the new track, analyzes the beatgrid, switches the active deck, and unloads the previous deck.
     * - @attention: on clone failure, log an error and return
     */
    int loadTrackToDeck(const AudioTrack& track);

    // Display deck status
    void displayDeckStatus() const;

    /**
     * Contract: Determine if decks A and the given track can be mixed
     * @return true if mixable by BPM/key criteria; false otherwise
     */
    bool can_mix_tracks(const PointerWrapper<AudioTrack>& track) const;

    /**
     * Contract: Synchronize BPM between active deck and given track.
     * - @param track: Pointer to the track to sync with the currently active deck
     * - @brief This function calculates average BPM between active deck and given track, then sets the given track's BPM to the average
     * - @attention What should be the preconditions of this function? What this method modifies?
     */
    void sync_bpm(const PointerWrapper<AudioTrack>& track) const;

    /**
     * @brief set auto sync mode
     * 
     */
    void set_auto_sync(bool enabled) {
        auto_sync = enabled;
    }

    /**
     * @brief Set BPM tolerance for mixing compatibility
     * 
     */
    void set_bpm_tolerance(int tolerance) {
        bpm_tolerance = tolerance;
    }

};

#endif // MIXINGENGINESERVICE_H
