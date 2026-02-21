#pragma once

#include <string>
#include <unordered_map>

/**
 * @brief Configuration Management (Single Responsibility)
 * 
 * Handles application configuration settings from config files.
 * Phase 4 note: For this assignment, the playlists directory is always "./playlists"
 * and cache capacity is fixed to 8 by policy; ConfigurationManager may still parse
 * bpm_tolerance and auto_sync.
 */
class ConfigurationManager {
private:
    int cache_size;
    int bpm_tolerance;
    bool auto_sync;
    
    // Additional config parameters
    std::unordered_map<std::string, std::string> additional_settings;

public:
    /**
     * @brief Construct with default configuration
     */
    ConfigurationManager();
    
    /**
     * @brief Load configuration from file
     * @param config_path Path to dj_config.txt file
     * @return true if loaded successfully
     */
    bool loadFromFile(const std::string& config_path);
    
    // Configuration getters
    int getCacheSize() const { return cache_size; }
    int getBPMTolerance() const { return bpm_tolerance; }
    bool getAutoSync() const { return auto_sync; }
    
    /**
     * @brief Get additional configuration value by key
     */
    std::string getSetting(const std::string& key) const;
    
    /**
     * @brief Display current configuration
     */
    void displayConfiguration() const;
};