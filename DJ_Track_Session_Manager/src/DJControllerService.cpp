#include "DJControllerService.h"
#include "MP3Track.h"
#include "WAVTrack.h"
#include <iostream>
#include <memory>

DJControllerService::DJControllerService(size_t cache_size)
    : cache(cache_size) {}
/**
 * TODO: Implement loadTrackToCache method
 */
int DJControllerService::loadTrackToCache(AudioTrack &track)
{

    const std::string &title = track.get_title();

    // (a)
    if (cache.contains(title))
    {

        // (b) HIT case
        if (cache.get(title) != nullptr)
        {
            return 1;
        }
    }

    // (c) MISS case
    PointerWrapper<AudioTrack> cloned_wrapper = track.clone();
    if (!cloned_wrapper)
    {
        std::cout << "[ERROR] Track: \"" << title << "\" failed to clone for cache insertion." << std::endl;
        return 0;
    }

    // - Simulate loading on the cloned track, and do a beatgrid analysis.
    cloned_wrapper->load();
    cloned_wrapper->analyze_beatgrid();

    // - Wrap the prepared clone in a new PointerWrapper and insert into cache
    // The wrapper is passed to cache.put(), which takes ownership via move.
    bool eviction_occurred = cache.put(std::move(cloned_wrapper));

    if (eviction_occurred)
    {
        return -1;
    }

    return 0; // Placeholder
}

void DJControllerService::set_cache_size(size_t new_size)
{
    cache.set_capacity(new_size);
}
// implemented
void DJControllerService::displayCacheStatus() const
{
    std::cout << "\n=== Cache Status ===\n";
    cache.displayStatus();
    std::cout << "====================\n";
}

/**
 * TODO: Implement getTrackFromCache method
 */
AudioTrack *DJControllerService::getTrackFromCache(const std::string &track_title)
{

    //  Raw pointer to track, or nullptr if not found
    AudioTrack *track_ptr = cache.get(track_title);
    return track_ptr;
}
