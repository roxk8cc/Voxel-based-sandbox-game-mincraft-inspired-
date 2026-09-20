#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <string>
#include <vector>

/**
 * @enum GameState
 * Different states the game can be in
 */
enum GameState
{
    MAIN_MENU, 
    PLAYING,   
    PAUSE_MENU,   
    SAVE_MENU,  
    LOAD_MENU 
};

/**
 * @struct SaveSlot
 * Information about a save file
 */
struct SaveSlot
{
    int slot_number;
    std::string filename;
    bool exists;
    std::string display_name;
    std::string last_saved; 
    size_t file_size;   
    
    SaveSlot(int num)
    {
        slot_number = num;
        exists = false;
        file_size = 0;
        filename = "save_slot_" + std::to_string(num) + ".dat";
        display_name = "Slot " + std::to_string(num);
        last_saved = "Empty";
    }
};

#endif