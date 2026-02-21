#pragma once

#include "AudioTrack.h"
#include "PointerWrapper.h"
#include <cstddef>
#include <cstdint>

/**
 * @brief Single Cache Entry with LRU Metadata (Single Responsibility)
 * 
 * Represents one slot in the DJ controller's limited memory.
 * Separates cache slot management from the larger cache algorithm,
 * following SRP and making the design easier to test and maintain.
 *
 * Phase 4 usage:
 * - Each slot holds exactly one cached track instance owned by the controller.
 * - access() updates last_access_time to reflect MRU/LRU policy.
 * - clear() releases ownership; callers log evictions as needed.
 */
class CacheSlot {
private:
    PointerWrapper<AudioTrack> track;    // The cached track
    uint64_t last_access_time;           // For LRU algorithm
    bool occupied;                       // Is this slot in use?

public:
    /**
     * @brief Construct empty cache slot
     */
    CacheSlot();
    
    /**
     * @brief Store a track in this slot
     * @param track_ptr Track to store (transfers ownership)
     * @param access_time Current access timestamp
     */
    void store(PointerWrapper<AudioTrack> track_ptr, uint64_t access_time);
    
    /**
     * @brief Access the track (updates LRU timestamp)
     * @param access_time Current access timestamp
     * @return Raw pointer to track (does not transfer ownership)
     */
    AudioTrack* access(uint64_t access_time);
    
    /**
     * @brief Clear this slot (removes track)
     */
    void clear();
    
    /**
     * @brief Check if slot is occupied
     */
    bool isOccupied() const { return occupied; }
    
    /**
     * @brief Get last access time for LRU comparison
     */
    uint64_t getLastAccessTime() const { return last_access_time; }
    
    /**
     * @brief Get track without updating access time
     */
    AudioTrack* getTrack() const { return track.get(); }
};