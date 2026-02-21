#pragma once

#include "CacheSlot.h"
#include "AudioTrack.h"
#include "PointerWrapper.h"
#include <vector>
#include <cstddef>
#include <cstdint>
#include <string>

/**
 * @brief LRU Cache Implementation
 * 
 * Manages limited-capacity cache with Least Recently Used eviction policy.
 * This class has one responsibility: implementing efficient LRU caching logic.
 * It's decoupled from file I/O, UI concerns, and mixing operations.
 * 
 * Phase 4 usage contract:
 * - Used by DJControllerService with fixed capacity in this assignment.
 * - get() marks entries MRU by updating their access time.
 * - put() inserts as MRU and evicts true LRU when full.
 */
class LRUCache {
private:
    std::vector<CacheSlot> slots;
    size_t max_size;
    uint64_t access_counter;

public:
    /**
     * @brief Construct LRU cache with specified capacity
     * @param capacity Maximum number of tracks to cache
     */
    explicit LRUCache(size_t capacity);
    
    /**
     * @brief Check if cache contains a track
     * @param track_id Track identifier to search for
     * @return true if track is in cache
     */
    bool contains(const std::string& track_id) const;
    
    /**
     * @brief Get a track from cache (updates LRU order)
     * @param track_id Track identifier
     * @return Raw pointer to track, or nullptr if not found
     * 
     * This method updates access time, moving the track to
     * "most recently used" position in LRU algorithm.
     */
    AudioTrack* get(const std::string& track_id);
    
    /**
     * @brief Put a track into cache (handles eviction if full)
     * @param track Track to cache (transfers ownership).
     * @return true if an eviction occurred, false otherwise.
     * 
     * If cache is full, automatically evicts the least recently
     * used track before storing the new one.
     */
    bool put(PointerWrapper<AudioTrack> track);
    
    /**
     * @brief Manually evict the least recently used track
     * @return true if a track was evicted
     */
    bool evictLRU();
    
    /**
     * @brief Get current cache usage
     * @return Number of occupied slots
     */
    size_t size() const;
    
    /**
     * @brief Get maximum cache capacity
     */
    size_t capacity() const { return max_size; }
    
    /**
     * @brief Check if cache is full
     */
    bool isFull() const { return size() >= max_size; }
    
    /**
     * @brief Clear all cache entries
     */
    void clear();
    
    /**
     * @brief Display cache status with LRU information
     */
    void displayStatus() const;
    /**
     * @brief Update LRU Cache capacity
     * This method should be used only once.
     */
    void set_capacity(size_t capacity);
private:
    /**
     * @brief Find slot containing specific track
     * @param track_id Track identifier
     * @return Slot index, or max_size if not found
     */
    size_t findSlot(const std::string& track_id) const;
    
    /**
     * @brief Find the least recently used slot
     * @return Slot index of LRU entry
     */
    size_t findLRUSlot() const;
    
    /**
     * @brief Find first empty slot
     * @return Slot index, or max_size if cache is full
     */
    size_t findEmptySlot() const;
};