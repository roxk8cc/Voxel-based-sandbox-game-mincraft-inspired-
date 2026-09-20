#ifndef SAVEMANAGER_H
#define SAVEMANAGER_H

#include "world.h"
#include <string>
#include <fstream> 
#include <vector>


class Savemanager
{
private:
    
    /**
     * @struct RLESegment
     * Represents a run of consecutive identical blocks
     */
    struct RLESegment 
    {
        BlockType type;
        uint32_t count;
    };

    /**
     * Compresses world data using Run-Length Encoding
     * @param world Reference to the world to compress
     * @return Vector of RLE segments
     */
    static std::vector<RLESegment> compress_world(World& world);

    /**
     * Decompresses RLE data back into world blocks
     * @param segments Vector of RLE segments
     * @param world Reference to the world to fill
     * @return true if successful, false otherwise
     */
    static bool decompress_world(const std::vector<RLESegment>& segments, World& world);

public:
    /**
     * Saves the world to a binary file using RLE compression
     * @param world Reference to the world to save
     * @param filename Optional custom filename
     * @return true if save was successful, false otherwise
     */
    static bool save_world(World& world, const std::string& filename = "world_save.dat");

    /**
     * Loads the world from a binary file with RLE decompression
     * @param world Reference to the world to load into
     * @param filename Optional custom filename
     * @return true if load was successful, false otherwise
     */
    static bool load_world(World& world, const std::string& filename = "world_save.dat");

    /**
     * Checks if a save file exists
     * @param filename The filename to check
     * @return true if file exists, false otherwise
     */
    static bool save_exists(const std::string& filename = "world_save.dat");

    /**
     * Gets the size of the save file in bytes
     * @param filename The filename to check
     * @return Size in bytes, or 0 if file doesn't exist
     */
    static size_t get_save_size(const std::string& filename = "world_save.dat");

    /**
     * Gets the default save file path
     * @return Path to the save file
     */
    static std::string get_save_path();

};

#endif