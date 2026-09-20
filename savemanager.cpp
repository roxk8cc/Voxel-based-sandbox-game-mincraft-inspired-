#include "savemanager.h"
#include <iostream>
#include <cstring>
#include <sys/stat.h>

const uint32_t FILE_MAGIC = 0x564F5845; 
const uint32_t FILE_VERSION = 1;

std::vector<Savemanager::RLESegment> Savemanager::compress_world(World& world) 
{
    std::vector<RLESegment> segments;
    int size = world.get_size();
    int height = world.get_height();
    
    BlockType current_type = world.get_block(0, 0, 0).get_type();
    uint32_t count = 1;
    
    for (int x = 0; x < size; x++) 
    {
        for (int y = 0; y < height; y++) 
        {
            for (int z = 0; z < size; z++) 
            {
                if (x == 0 && y == 0 && z == 0) 
                {
                    continue;
                }
                BlockType block_type = world.get_block(x, y, z).get_type();
                if (block_type == current_type) 
                {
                    count++;
                } 
                else 
                {
                    segments.push_back({current_type, count});
                    current_type = block_type;
                    count = 1;
                }
            }
        }
    }
    segments.push_back({current_type, count});
    return segments;
}

bool Savemanager::decompress_world(const std::vector<RLESegment>& segments, World& world) 
{
    int size = world.get_size();
    int height = world.get_height();
    
    int x = 0;
    int y = 0;
    int z = 0;
    
    for (const RLESegment& segment : segments) 
    {
        for (uint32_t i = 0; i < segment.count; i++) 
        {
            if (x >= size || y >= height || z >= size) 
            {
                return false;
            }
            world.set_block(x, y, z, segment.type);
            z++;

            if (z >= size) 
            {
                z = 0;
                y++;
                if (y >= height) 
                {
                    y = 0;
                    x++;
                }
            }
        }
    }
    return true;
}

std::string Savemanager::get_save_path() 
{
    return "./";
}

bool Savemanager::save_world(World& world, const std::string& filename) 
{
    
    std::string filepath = get_save_path() + filename;
    std::vector<RLESegment> segments = compress_world(world);
    
    std::ofstream file(filepath, std::ios::binary);
    if (!file.is_open()) 
    {
        return false;
    }
    
    file.write(reinterpret_cast<const char*>(&FILE_MAGIC), sizeof(FILE_MAGIC));
    file.write(reinterpret_cast<const char*>(&FILE_VERSION), sizeof(FILE_VERSION));
    
    int size = world.get_size();
    int height = world.get_height();
    file.write(reinterpret_cast<const char*>(&size), sizeof(size));
    file.write(reinterpret_cast<const char*>(&height), sizeof(height));
    
    uint32_t segment_count = segments.size();
    file.write(reinterpret_cast<const char*>(&segment_count), sizeof(segment_count));
    
    for (const RLESegment& segment : segments) 
    {
        file.write(reinterpret_cast<const char*>(&segment.type), sizeof(segment.type));
        file.write(reinterpret_cast<const char*>(&segment.count), sizeof(segment.count));
    }
    
    file.close();
    return true;
}

bool Savemanager::load_world(World& world, const std::string& filename) 
{
    std::string filepath = get_save_path() + filename;
    
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) 
    {
        return false;
    }
    
    uint32_t magic, version;
    file.read(reinterpret_cast<char*>(&magic), sizeof(magic));
    file.read(reinterpret_cast<char*>(&version), sizeof(version));
    
    if (magic != FILE_MAGIC) 
    {
        return false;
    }
    
    if (version != FILE_VERSION) 
    {
        return false;
    }
    
    int saved_size, saved_height;
    file.read(reinterpret_cast<char*>(&saved_size), sizeof(saved_size));
    file.read(reinterpret_cast<char*>(&saved_height), sizeof(saved_height));
    
    if (saved_size != world.get_size() || saved_height != world.get_height()) 
    {
        return false;
    }
    
    uint32_t segment_count;
    file.read(reinterpret_cast<char*>(&segment_count), sizeof(segment_count));
    std::vector<RLESegment> segments;
    segments.reserve(segment_count);
    
    for (uint32_t i = 0; i < segment_count; i++) 
    {
        RLESegment segment;
        file.read(reinterpret_cast<char*>(&segment.type), sizeof(segment.type));
        file.read(reinterpret_cast<char*>(&segment.count), sizeof(segment.count));
        segments.push_back(segment);
    }
    
    file.close();
    
    if (!decompress_world(segments, world)) 
    {
        return false;
    }
    return true;
}

bool Savemanager::save_exists(const std::string& filename) 
{
    std::string filepath = get_save_path() + filename;
    std::ifstream file(filepath);
    return file.good();
}

size_t Savemanager::get_save_size(const std::string& filename) 
{
    std::string filepath = get_save_path() + filename;
    struct stat file_info;
    int return_code = stat(filepath.c_str(), &file_info);
    if (return_code == 0) 
    {
        return file_info.st_size;
    } 
    else 
    {
        return 0;
    }
}