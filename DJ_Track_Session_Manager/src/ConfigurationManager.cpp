#include "ConfigurationManager.h"
#include <fstream>
#include <sstream>
#include <iostream>

ConfigurationManager::ConfigurationManager() :
    cache_size(8),
    bpm_tolerance(10),
    auto_sync(true), additional_settings() {
}

bool ConfigurationManager::loadFromFile(const std::string& config_path) {
    std::ifstream file(config_path);
    if (!file.is_open()) {
        std::cerr << "[ERROR] Could not open config file: " << config_path << std::endl;
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Skip comments and empty lines
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        // Parse key=value pairs
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            continue;
        }
        
        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);
        
        // Parse known configuration keys
        if (key == "cache_size") {
            cache_size = std::stoi(value);
        } else if (key == "bpm_tolerance") {
            bpm_tolerance = std::stoi(value);
        } else if (key == "auto_sync") {
            auto_sync = (value == "true");
        } else {
            // Store unknown settings for extensibility
            additional_settings[key] = value;
        }
    }
    
    return true;
}

std::string ConfigurationManager::getSetting(const std::string& key) const {
    auto it = additional_settings.find(key);
    return (it != additional_settings.end()) ? it->second : "";
}

void ConfigurationManager::displayConfiguration() const {
    std::cout << "=== DJ System Configuration ===" << std::endl;
    std::cout << "Cache Size: " << cache_size << " slots" << std::endl;
    std::cout << "BPM Tolerance: " << bpm_tolerance << " BPM" << std::endl;
    std::cout << "Auto Sync: " << (auto_sync ? "enabled" : "disabled") << std::endl;
    
    if (!additional_settings.empty()) {
        std::cout << "Additional Settings:" << std::endl;
        for (const auto& setting : additional_settings) {
            std::cout << "  " << setting.first << ": " << setting.second << std::endl;
        }
    }
    std::cout << std::endl;
}