#include "LRUCache.h"
#include <iostream>
LRUCache::LRUCache(size_t capacity)
    : slots(capacity), max_size(capacity), access_counter(0) {}

bool LRUCache::contains(const std::string &track_id) const
{
    return findSlot(track_id) != max_size;
}

AudioTrack *LRUCache::get(const std::string &track_id)
{
    size_t idx = findSlot(track_id);
    if (idx == max_size)
        return nullptr;
    return slots[idx].access(++access_counter);
}

/**
 * TODO: Implement the put() method for LRUCache
 */
bool LRUCache::put(PointerWrapper<AudioTrack> track)
{
    //(A)
    if (!track)
    {
        return false;
    }

    bool eviction_occured = false;
    const std ::string &track_title = track->get_title();
    size_t target_slot_index = max_size;
    target_slot_index = findSlot(track_title);

    //(b) if the track exists [HIT]
    if (target_slot_index != max_size)
    {
        slots[target_slot_index].access(++access_counter);
        return false;
    }

    // (D)
    target_slot_index = findEmptySlot();

    // (C) if cache is full DO evictLRU()
    if (target_slot_index == max_size)
    {

        if (evictLRU())
        {
            eviction_occured = true;
            // (D)
            target_slot_index = findEmptySlot();
        }
        else
        {
            return false;
        }
    }

    /// (e)
    if (target_slot_index != max_size)
    {
        slots[target_slot_index].store(std::move(track), ++access_counter);
    }

    // (f)
    return eviction_occured;
}

bool LRUCache::evictLRU()
{
    size_t lru = findLRUSlot();
    if (lru == max_size || !slots[lru].isOccupied())
        return false;
    slots[lru].clear();
    return true;
}

size_t LRUCache::size() const
{
    size_t count = 0;
    for (const auto &slot : slots)
        if (slot.isOccupied())
            ++count;
    return count;
}

void LRUCache::clear()
{
    for (auto &slot : slots)
    {
        slot.clear();
    }
}

void LRUCache::displayStatus() const
{
    std::cout << "[LRUCache] Status: " << size() << "/" << max_size << " slots used\n";
    for (size_t i = 0; i < max_size; ++i)
    {
        if (slots[i].isOccupied())
        {
            std::cout << "  Slot " << i << ": " << slots[i].getTrack()->get_title()
                      << " (last access: " << slots[i].getLastAccessTime() << ")\n";
        }
        else
        {
            std::cout << "  Slot " << i << ": [EMPTY]\n";
        }
    }
}

size_t LRUCache::findSlot(const std::string &track_id) const
{
    for (size_t i = 0; i < max_size; ++i)
    {
        if (slots[i].isOccupied() && slots[i].getTrack()->get_title() == track_id)
            return i;
    }
    return max_size;
}

/**
 * TODO: Implement the findLRUSlot() method for LRUCache
 */
size_t LRUCache::findLRUSlot() const
{
    if (size() == 0)
    {
        return max_size;
    }

    size_t lru_index = max_size;
    size_t min_access_time = (size_t)-1;

    for (size_t i = 0; i < max_size; ++i)
    {
        if (slots[i].isOccupied())
        {
            size_t current_access_time = slots[i].getLastAccessTime();

            if (current_access_time < min_access_time)
            {
                min_access_time = current_access_time;
                lru_index = i;
            }
        }
    }
    return lru_index;
}

size_t LRUCache::findEmptySlot() const
{
    for (size_t i = 0; i < max_size; ++i)
    {
        if (!slots[i].isOccupied())
            return i;
    }
    return max_size;
}

void LRUCache::set_capacity(size_t capacity)
{
    if (max_size == capacity)
        return;
    // udpate max size
    max_size = capacity;
    // update the slots vector
    slots.resize(capacity);
}
