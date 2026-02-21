#include "SessionFileParser.h"
#include <iostream>
#include <sstream>
#include <algorithm>

// ========== PUBLIC METHODS (PROVIDED FOR STUDENTS) ==========

bool SessionFileParser::parse_config_file(const std::string& config_path, SessionConfig& config) {
    std::ifstream file(config_path);
    
    if (!file.is_open()) {
        std::cout << "[ERROR] Cannot open config file: " << config_path << std::endl;
        return false;
    }
    
    std::string line;
    int line_number = 0;
    
    while (std::getline(file, line)) {
        line_number++;
        line = trim_string(line);
        
        // Skip empty lines and comments
        if (line.empty() || is_comment_line(line)) {
            continue;
        }
        
        // Parse configuration entries
        std::string key, value;
        if (parse_key_value(line, key, value)) {
            
            if (key == "app_name") {
                config.app_name = value;
                
            } else if (key == "version") {
                config.version = value;
                
            } else if (key.find("library_track_") == 0) {
                // Handle library_track_1, library_track_2, etc.
                SessionConfig::TrackInfo track_info;
                if (parse_library_track(value, track_info)) {
                    config.library_tracks.push_back(track_info);
                } else {
                    std::cout << "[WARNING] Invalid track format at line " << line_number << std::endl;
                }
                
            } else if (key == "controller_cache_size") {
                try {
                    config.controller_cache_size = std::stoi(value);
                } catch (const std::exception& e) {
                    std::cout << "[WARNING] Invalid cache size at line " << line_number << std::endl;
                }
                
            } else if (key == "bpm_tolerance") {
                try {
                    config.bpm_tolerance = std::stoi(value);
                } catch (const std::exception& e) {
                    std::cout << "[WARNING] Invalid BPM tolerance at line " << line_number << std::endl;
                }
                
            } else if (key == "auto_sync") {
                config.auto_sync = parse_bool(value);
                
            } else {
                // Check if it's a playlist definition (any other key=value where value contains numbers/commas)
                std::string playlist_name;
                std::vector<int> track_indices;
                if (parse_playlist_line(line, playlist_name, track_indices)) {
                    config.playlists[playlist_name] = track_indices;
                } else {
                    std::cout << "[WARNING] Unknown config key '" << key << "' at line " << line_number << std::endl;
                }
            }
            
        } else {
            std::cout << "[WARNING] Cannot parse line " << line_number << ": " << line << std::endl;
        }
    }
    
    file.close();
    
    std::cout << "Parsed config file: " << config.library_tracks.size() << " tracks found, " 
              << config.playlists.size() << " playlists found" << std::endl;
    return true;
}


std::string SessionFileParser::extract_playlist_name(const std::string& playlist_path) {
    // TODO: Students implement name extraction
    
    // Find last slash or backslash
    size_t last_slash = playlist_path.find_last_of("/\\");
    std::string filename = (last_slash != std::string::npos) ? 
                          playlist_path.substr(last_slash + 1) : playlist_path;
    
    // Remove .playlist extension
    size_t dot_pos = filename.find_last_of('.');
    if (dot_pos != std::string::npos && filename.substr(dot_pos) == ".playlist") {
        filename = filename.substr(0, dot_pos);
    }
    
    // TODO: Convert underscores to spaces and clean up name
    std::replace(filename.begin(), filename.end(), '_', ' ');
    
    return filename;
}

bool SessionFileParser::validate_track_format(const std::string& line) {
    // TODO: Students implement format validation
    
    // Basic validation: should start with MP3 or WAV
    if (line.length() < 4) {
        return false;
    }
    
    if (line.substr(0, 3) == "MP3" || line.substr(0, 3) == "WAV") {
        // Check if followed by comma
        if (line.length() > 3 && line[3] == ',') {
            return true;
        }
    }
    
    return false;
}

// ========== PRIVATE HELPER METHODS ==========

std::vector<std::string> SessionFileParser::split_string(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(trim_string(token));
    }
    
    return tokens;
}

std::string SessionFileParser::trim_string(const std::string& str) {
    const std::string whitespace = " \t\n\r";
    
    size_t start = str.find_first_not_of(whitespace);
    if (start == std::string::npos) {
        return "";
    }
    
    size_t end = str.find_last_not_of(whitespace);
    return str.substr(start, end - start + 1);
}

bool SessionFileParser::parse_bool(const std::string& str) {
    std::string lower_str = str;
    std::transform(lower_str.begin(), lower_str.end(), lower_str.begin(), ::tolower);
    
    return (lower_str == "true" || lower_str == "1" || lower_str == "yes");
}

bool SessionFileParser::is_comment_line(const std::string& line) {
    return !line.empty() && line[0] == '#';
}

bool SessionFileParser::parse_key_value(const std::string& line, std::string& key, std::string& value) {
    size_t equals_pos = line.find('=');
    if (equals_pos == std::string::npos) {
        return false;
    }
    
    key = trim_string(line.substr(0, equals_pos));
    value = trim_string(line.substr(equals_pos + 1));
    
    return !key.empty();
}

bool SessionFileParser::parse_library_track(const std::string& line, SessionConfig::TrackInfo& track_info) {
    // Expected format: MP3,title,{artist1;artist2;},duration,bpm,bitrate,has_tags
    // or: WAV,title,{artist1;artist2;},duration,bpm,sample_rate,bit_depth
    
    std::vector<std::string> parts = split_string(line, ',');
    
    if (parts.size() < 7) {
        return false;
    }
    
    try {
        track_info.type = parts[0];
        track_info.title = parts[1];
        
        // Parse artist list from {artist1;artist2;...} format
        track_info.artists = parse_artist_list(parts[2]);
        
        track_info.duration_seconds = std::stoi(parts[3]);
        track_info.bpm = std::stoi(parts[4]);
        track_info.extra_param1 = std::stoi(parts[5]);  // bitrate or sample_rate
        track_info.extra_param2 = std::stoi(parts[6]);  // has_tags or bit_depth
        
        // Validate track type is MP3 or WAV
        if (track_info.type != "MP3" && track_info.type != "WAV") {
            return false;
        }
        
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

std::vector<std::string> SessionFileParser::parse_artist_list(const std::string& artist_str) {
    std::vector<std::string> artists;
    std::string cleaned = trim_string(artist_str);
    
    // Remove curly braces
    if (cleaned.length() >= 2 && cleaned.front() == '{' && cleaned.back() == '}') {
        cleaned = cleaned.substr(1, cleaned.length() - 2);
    }
    
    // Split by semicolon
    std::vector<std::string> artist_parts = split_string(cleaned, ';');
    for (const auto& artist : artist_parts) {
        std::string trimmed = trim_string(artist);
        if (!trimmed.empty()) {
            artists.push_back(trimmed);
        }
    }
    
    // If no artists found, add a default
    if (artists.empty()) {
        artists.push_back("Unknown Artist");
    }
    
    return artists;
}

bool SessionFileParser::parse_playlist_line(const std::string& line, std::string& playlist_name, std::vector<int>& track_indices) {
    // Expected format: playlist_name=1,2,3,4
    std::string key, value;
    if (!parse_key_value(line, key, value)) {
        return false;
    }
    
    playlist_name = key;
    track_indices.clear();
    
    // Parse comma-separated indices
    std::vector<std::string> index_strs = split_string(value, ',');
    for (const auto& idx_str : index_strs) {
        try {
            int idx = std::stoi(trim_string(idx_str));
            track_indices.push_back(idx);
        } catch (const std::exception& e) {
            // Skip invalid indices
            std::cout << "[WARNING] Invalid track index in playlist '" << playlist_name << "': " << idx_str << std::endl;
        }
    }
    
    return !track_indices.empty();
}