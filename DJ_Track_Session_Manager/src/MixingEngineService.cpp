#include "MixingEngineService.h"
#include <iostream>
#include <memory>

#include <cmath>

/**
 * TODO: Implement MixingEngineService constructor
 */
MixingEngineService::MixingEngineService()
    : decks(), active_deck(1), auto_sync(false), bpm_tolerance(0)
{
    // Your implementation here
    decks[0]=nullptr; decks[1]=nullptr;
    std::cout << "[MixingEngineService] Initialized with 2 empty decks." << std::endl;
}

/**
 * TODO: Implement MixingEngineService destructor
 */
MixingEngineService::~MixingEngineService() {
    // Your implementation here
    std::cout << "[MixingEngineService] Cleaning up decks...";
    for(int i=0; i<2 ; ++i){
        if(decks[i] != nullptr){
            delete decks[i];
            decks[i] = nullptr;
        }
    }
}

// void MixingEngineService::MixingEngineService(const MixingEngineService& other):
// active_deck(other.active_deck),
// auto_sync(other.auto_sync),
// bpm_tolerance(other.bpm_tolerance),  
// {
//     decks[0]=other.decks[0];
//     decks[1]=other.decks[1];
// }


/**
 * TODO: Implement loadTrackToDeck method
 * @param track: Reference to the track to be loaded
 * @return: Index of the deck where track was loaded, or -1 on failure
 */
int MixingEngineService::loadTrackToDeck(const AudioTrack& track) {
    // Your implementation here
    //(a)
    std::cout << "\n=== Loading Track to Deck ===\n";

    //(b)
    PointerWrapper<AudioTrack> cloned = track.clone();

    //(c)
    if(!cloned){
        std::cout << "[ERROR] Track: \"" << track.get_title() <<"\" failed to clone\n";
        return -1;
    }

    //(d)
    size_t target = 1 - active_deck;
    

    //(e)
    std::cout << "[Deck Switch] Target deck: " << target << "\n";

    //(f)
    if(decks[target] != nullptr){
        delete decks[target];
        decks[target]=nullptr;
    }

    //(g)
    if(cloned){
    cloned->load();
    cloned->analyze_beatgrid();
    }


    //(h)
    //if(auto_sync && decks[active_deck] != nullptr){
    if(auto_sync ){
        if(!can_mix_tracks(cloned)){
            sync_bpm(cloned);
        }
    }

    //(i)
    decks[target] = cloned.release();
    std::cout << "[Load Complete] '" << decks[target]->get_title() 
              << "' is now loaded on deck " << target << "\n";

    //(j)
    // if(decks[active_deck] != nullptr && target != active_deck){
    //     std::cout << "[Unload] Unloading previous deck "
    //     <<active_deck<< " (" 
    //     <<decks[active_deck]->get_title() << ")\n";

    //     delete decks[active_deck];
    //     decks[active_deck]=nullptr;
    // }

    //(k)
    active_deck = target;

    //(l)
    std::cout << "[Active Deck] Switched to deck " << target << "\n";

    //displayDeckStatus();
    //(m)
    return static_cast<int>(target);
}

/**
 * @brief Display current deck status
 */
void MixingEngineService::displayDeckStatus() const {
    std::cout << "\n=== Deck Status ===\n";
    for (size_t i = 0; i < 2; ++i) {
        if (decks[i])
            std::cout << "Deck " << i << ": " << decks[i]->get_title() << "\n";
        else
            std::cout << "Deck " << i << ": [EMPTY]\n";
    }
    std::cout << "Active Deck: " << active_deck << "\n";
    std::cout << "===================\n";
}

/**
 * TODO: Implement can_mix_tracks method
 * 
 * Check if two tracks can be mixed based on BPM difference.
 * 
 * @param track: Track to check for mixing compatibility
 * @return: true if BPM difference <= tolerance, false otherwise
 */
bool MixingEngineService::can_mix_tracks(const PointerWrapper<AudioTrack>& track) const {
    // Your implementation here
    if(decks[active_deck] == nullptr || track.get() == nullptr){
        return false;
    }
    int track_new= track->get_bpm();
    int deck_old= decks[active_deck]->get_bpm();
    int diff = std::abs(deck_old - track_new);
    return diff <= bpm_tolerance; 
}

/**
 * TODO: Implement sync_bpm method
 * @param track: Track to synchronize with active deck
 */
void MixingEngineService::sync_bpm(const PointerWrapper<AudioTrack>& track) const {
    // Your implementation here
    if(decks[active_deck] == nullptr || track.get() == nullptr)
    {
    std::cout << "[Sync BPM] Cannot sync - one of the decks is empty." << std::endl;

    return;
    } 
    int track_bpm = track->get_bpm();
    int deck_bpm = decks[active_deck]->get_bpm();
    int avg = (deck_bpm+track_bpm)/2;
    track->set_bpm(avg);
    std::cout << "[Sync BPM] Syncing BPM from " << track_bpm << " to " << avg << "\n";
}
