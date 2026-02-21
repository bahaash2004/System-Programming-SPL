#include "DJLibraryService.h"
#include "SessionFileParser.h"
#include "MP3Track.h"
#include "WAVTrack.h"
#include <iostream>
#include <memory>
#include <filesystem>



// implementation of the destructor
DJLibraryService::~DJLibraryService()
{
    // Iterate through the vector and delete each allocated AudioTrack
    for (AudioTrack *track : library)
    {
        delete track;
    }
    // Clear the vector to remove the dangling pointers
    library.clear();
}

/**
 * @brief Load a playlist from track indices referencing the library
 * @param library_tracks Vector of track info from config
 */
void DJLibraryService::buildLibrary(const std::vector<SessionConfig::TrackInfo> &library_tracks)
{
   // std::cout << "[INFO] " << library_tracks.size() << " tracks to be loaded into library." << std::endl;

    // Iterate through the configuration data
    for (const auto &info : library_tracks)
    {
        AudioTrack *new_track = nullptr;

        // (a) + (b)
        if (info.type == "MP3")
        {
            int bitrate = info.extra_param1;
            bool has_tags = (info.extra_param2 != 0);

            new_track = new MP3Track(info.title, info.artists, info.duration_seconds, info.bpm, bitrate, has_tags);

            //std::cout << "MP3Track created: " << bitrate << " kbps" << std::endl;
        }
        else if (info.type == "WAV")
        {
            int sample_rate = info.extra_param1;
            int bit_depth = info.extra_param2;

            new_track = new WAVTrack(info.title, info.artists, info.duration_seconds, info.bpm, sample_rate, bit_depth);

           // std::cout << "WAVTrack created: " << sample_rate << "Hz/" << bit_depth << "bit" << std::endl;
        }

        // (c) Store the track in the member vector named "library"
        if (new_track)
        {
            library.push_back(new_track);
        }
        else
        {
            std::cout << "[ERROR] Could not create track: Unknown format (" << info.type << ")" << std::endl;
        }
    }

    // (d)
    std::cout << "[INFO] Track library built: " << library.size() << " tracks loaded" << std::endl;
}

/**
 * @brief Display the current state of the DJ library playlist
 *
 */
void DJLibraryService::displayLibrary() const
{
    std::cout << "=== DJ Library Playlist: "
              << playlist.get_name() << " ===" << std::endl;

    if (playlist.is_empty())
    {
        std::cout << "[INFO] Playlist is empty.\n";
        return;
    }

    // Let Playlist handle printing all track info
    playlist.display();

    std::cout << "Total duration: " << playlist.get_total_duration() << " seconds" << std::endl;
}

/**
 * @brief Get a reference to the current playlist
 *
 * @return Playlist&
 */
Playlist &DJLibraryService::getPlaylist()
{
    // Your implementation here
    return playlist;
}

/**
 * TODO: Implement findTrack method
 *
 * HINT: Leverage Playlist's find_track method
 */
AudioTrack *DJLibraryService::findTrack(const std::string &track_title)
{
    // Your implementation here

    return playlist.find_track(track_title);
}

void DJLibraryService::loadPlaylistFromIndices(const std::string &playlist_name,
                                               const std::vector<int> &track_indices)
{
    // Your implementation here

    // (a)
    std::cout << "[INFO] Loading playlist: " << playlist_name << std::endl;

    //(b)
    playlist = Playlist(playlist_name);

    //(c)
    for (int index : track_indices)
    {

        // convert 1-based index into 0_based
        size_t library_idx = static_cast<size_t>(index - 1);

        // validate index is within library bounds
        if (index < 1 || library_idx >= library.size())
        {

            // If invalid, log warning and skip
            std::cout << "[WARNING] Invalid track index: " << index << " and skip" << std::endl;
            continue;
        }

        AudioTrack *source_track = library[library_idx];
        // Clone the track polymorphically and unwrap the PointerWrapper
        // We use the PointerWrapper to manage the clone temporarily
        PointerWrapper<AudioTrack> cloned_wrapper = source_track->clone();

        // If clone is nullptr, log error and skip
        if (!cloned_wrapper)
        {
            std::cout << "[ERROR] Failed to clone track: " << source_track->get_title() << std::endl;
            continue;
        }

        // Call load() and analyze_beatgrid() on cloned track
        cloned_wrapper->load();
        cloned_wrapper->analyze_beatgrid();

        // Add cloned track to playlist using playlist.add_track()
        // We call release() to transfer ownership from the wrapper to the Playlist
        playlist.add_track(cloned_wrapper.release());

        // Log successful addition
        //std::cout << "Added '" << source_track->get_title() << "' to playlist '" << playlist_name << "'" << std::endl;
    }

    // Log summary
    std::cout << "[INFO] Playlist loaded: " << playlist_name
              << " (" << playlist.get_track_count() << " tracks)" << std::endl;
}
/**
 * TODO: Implement getTrackTitles method
 * @return Vector of track titles in the playlist
 */
std::vector<std::string> DJLibraryService::getTrackTitles() const
{
    // Your implementation here
    std::vector<std::string> titles;

    // Iterate through playlist tracks
    std::vector<AudioTrack *> tracks = playlist.getTracks();

    for (const auto *track : tracks)
    {
        if (track)
        {
            // Collect titles using track->get_title()
            titles.push_back(track->get_title());
        }
    }
    return titles;
}
