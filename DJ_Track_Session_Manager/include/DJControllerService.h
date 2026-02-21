#ifndef DJCONTROLLERSERVICE_H
#define DJCONTROLLERSERVICE_H

#include "LRUCache.h"
#include "CacheSlot.h"
#include "PointerWrapper.h"
#include <string>

/**
 * Service responsible for managing the controller's memory (cache)
 * Cache capacity is fixed, and the tracks are managed with LRU policy.
 * On HIT: touch MRU (most recently used); on MISS: insert; if full, evict LRU.
 * - Mixer always receives a polymorphic clone; cache retains its copy.
 */
class DJControllerService {
public:
    // Construct with a given cache size
    explicit DJControllerService(size_t cache_size = 8);

    // Contract: Ensure a track is present in cache by key (full playlist line)
    // Input: A reference to an AudioTrack.
    // Output: An integer indicating the result: 1 for HIT, 0 for MISS without eviction, -1 for MISS with eviction.
    int loadTrackToCache(AudioTrack& track);


    // Contract: Display cache status (LRU order and occupancy)
    // - Intended for debugging and interactive inspection
    void displayCacheStatus() const; // TODO: Implement

    /**
     * @brief Set the cache size for the LRUCache.
     * @param new_size The new size for the cache.
     * @note This function is meant for a single usage. don't call it more then once.
     */
    void set_cache_size(size_t new_size);
    /**
     * @brief Get a track from the cache by its title.
     * @param track_title The title of the track to retrieve.
     * @return A raw pointer to the track if found, otherwise nullptr. Does not transfer ownership.
     */
    AudioTrack* getTrackFromCache(const std::string& track_title);

private:
    LRUCache cache;
};

#endif // DJCONTROLLERSERVICE_H
