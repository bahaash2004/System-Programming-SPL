#pragma once

#include "DJLibraryService.h"
#include "DJControllerService.h"
#include "MixingEngineService.h"
#include "SessionFileParser.h"
#include "ConfigurationManager.h"
#include <string>
#include <vector>

/**
 * @brief DJ Controller Memory Statistics
 * Phase 4: Track how memory is used in the DJ controller simulation
 */
struct ControllerStats {
    size_t tracks_in_controller;      // Tracks currently loaded in controller memory
    size_t total_tracks_processed;    // Total tracks that passed through controller
    size_t memory_slots_used;         // How many memory slots are occupied
    size_t tracks_evicted;            // How many tracks were removed due to memory limits
    
    ControllerStats() : tracks_in_controller(0), total_tracks_processed(0), 
                       memory_slots_used(0), tracks_evicted(0) {}
};

/**
 * @brief Professional DJ Session System Orchestrator
 */
class DJSession {
private:
    // Session identification
    std::string session_name;

    // Service-oriented architecture: delegate to services
    DJLibraryService library_service;
    DJControllerService controller_service;
    MixingEngineService mixing_service;
    
    // Configuration and session state
    ConfigurationManager config_manager;
    SessionConfig session_config;
    std::vector<std::string> track_titles;
    //bool play_all = false;
    bool play_all;
    // Session statistics
    struct SessionStats {
        size_t tracks_processed = 0;
        size_t cache_hits = 0;
        size_t cache_misses = 0;
        size_t cache_evictions = 0;
        size_t deck_loads_a = 0;
        size_t deck_loads_b = 0;
        size_t transitions = 0;
        size_t errors = 0;
    } stats;

public:
    // ========== CONSTRUCTORS & DESTRUCTOR ==========

    /**
     * @brief Construct a new DJSession orchestrator
     * @param name Session identifier
     */
    DJSession(const std::string& name = "DJ Session", bool play_all = false);

    /**
     * @brief Destructor
     */
    ~DJSession();

    // ========== ORCHESTRATION METHODS ==========


    /**
     * Contract: Load playlist into the session library
     * - Input: playlist name (key from session config)
     * - Output: true on success; false if playlist not found or empty
     */
    bool load_playlist(const std::string& playlist_name);

    /**
     * Contract: Demand-load a track into the controller cache.
     * - Input: The name of the track to load.
     * - Output: An integer indicating a HIT (1) or MISS (0).
     */
    int load_track_to_controller(const std::string& track_name);

    /**
     * Contract: Load a cached track into a mixer deck (instant-transition model)
     * - Input: track title (or key).
     * - Output: true on success; false if not found in cache or clone fails
     */
    bool load_track_to_mixer_deck(const std::string& track_title);

    /**
     * Contract: Orchestrate the DJ performance simulation
     */
    void simulate_dj_performance();


    // ========== STATUS & DISPLAY METHODS ==========

    const std::string& get_session_name() const { return session_name; }

    // TODO: Add more status and display methods as needed, delegating to services

private:
    // ========== PROVIDED HELPER METHODS (Menu and Config) ==========
    
    /**
     * @brief Load configuration from bin/dj_config.txt
     * @return true if configuration loaded successfully
     */
    bool load_configuration();
    
    /**
     * @brief Display available playlists from config and prompt user to select one
     * @return Selected playlist name, or empty string if cancelled
     */
    std::string display_playlist_menu_from_config();
    /**
     * @brief Print final session summary with statistics
     */
    void print_session_summary() const;
};
