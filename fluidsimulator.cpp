#include "fluidsimulator.h"
#include <algorithm>
#include <cmath>
#include <cstdio>

const int LEVEL_SOURCE = 6;
const int LEVEL_MAX = 5;
const int LEVEL_MIN = 1;
const int LEVEL_EMPTY = 0;

FluidSimulator::FluidSimulator(World* world)
{
    this->world = world;
    world_size = world->get_size();
    world_height = world->get_height();
    int total = world_size * world_size * world_height;
    water_level.assign(total, LEVEL_EMPTY);
    is_source.assign(total, false);
    initialize_from_world();
}

FluidSimulator::~FluidSimulator()
{

}

bool FluidSimulator::in_bounds(int x, int y, int z)
{
    return x >= 0 && x < world_size && y >= 0 && y < world_height && z >= 0 && z < world_size;
}


bool FluidSimulator::is_solid_block(int x, int y, int z)
{
    if (!in_bounds(x, y, z))
    {
        return true;
    }
    BlockType type = world->get_block(x, y, z).get_type();
    return type != AIR && type != WATER;
}


bool FluidSimulator::can_hold_water(int x, int y, int z)
{
    if (!in_bounds(x, y, z))
    {
        return false;
    }
    BlockType type = world->get_block(x, y, z).get_type();
    return type == AIR || type == WATER;
}

void FluidSimulator::initialize_from_world()
{
    for (int y = 0; y < world_height; y++)
    {
        for (int z = 0; z < world_size; z++)
        {
            for (int x = 0; x < world_size; x++)
            {
                Block block = world->get_block(x, y, z);
                if (block.get_type() == WATER)
                {
                    int i = idx(x, y, z);
                    water_level[i] = LEVEL_MAX - 1;
                    is_source[i] = true; 
                    schedule_update(x, y, z);
                }
            }
        }
    }
}

void FluidSimulator::schedule_update(int x, int y, int z)
{
    if (in_bounds(x,y,z))
        pending_updates.insert(idx(x,y,z));
}

int FluidSimulator::calculate_flow_level(int x, int y, int z) 
{
    int i = idx(x, y, z);

    if (is_source[i]) return LEVEL_SOURCE;

    //check whteher it flow downward
    bool falling = false;
    if (water_level[i] >= LEVEL_MAX && in_bounds(x, y - 1, z) && can_hold_water(x, y - 1, z))
    {
        int down = idx(x, y - 1, z);
        if (water_level[down] < LEVEL_MAX)
        {
            falling = true;
        }
    }
    //check whether any water block above th block
    if (in_bounds(x, y + 1, z)) 
    {
        int above = idx(x, y + 1, z);
        if (water_level[above] > 0)
        {
            return LEVEL_MAX;
        }
    }
    if (falling)
    {
        return LEVEL_MAX;
    }

    int max_neighbor = 0;
    if (in_bounds(x, y + 1, z))
    {
        int above_idx = idx(x, y + 1, z);
        if (water_level[above_idx] > 0)
        {
            return LEVEL_MAX;
        }
    }
    // check the nighboring blocks
    const int delta_x[] = {1,-1,0,0};
    const int delta_z[] = {0,0,1,-1};
    for (int d = 0; d < 4; d++) 
    {
        int next_x = x + delta_x[d];
        int next_z = z + delta_z[d];

        if (!in_bounds(next_x, y, next_z))
        {
            continue;
        }
        int neighbor_idx = idx(next_x, y, next_z);
        int level = water_level[neighbor_idx];

        if (level > max_neighbor)
        {
            max_neighbor = level;
        }
    }

    if (max_neighbor == 0)
    {
        if (water_level[i] > 0)
        {
            return water_level[i];
        }
        return LEVEL_EMPTY;
    }
   // cauculate the new level of water
   int new_level = std::max(max_neighbor - 1, water_level[i]);
    if (new_level < LEVEL_MIN)
    {
        return LEVEL_EMPTY;
    }
    return new_level;
}

void FluidSimulator::update_cell(int x, int y, int z, float dt) 
{
    int i = idx(x, y,z);
    if (is_source[i])
    {
        sync_to_world(x, y, z);
        return;
    }

    if (is_solid_block(x, y, z))
    {
        int i = idx(x, y, z);
        if (water_level[i] != LEVEL_EMPTY)
        {
            water_level[i] = LEVEL_EMPTY;
            is_source[i] = false;
        }
        return; 
    }
    
    int old_level = water_level[i];
    int new_level = calculate_flow_level(x, y, z);

    if (!is_source[i] && new_level == LEVEL_SOURCE)
    {
        int count_x = 0;
        int count_z = 0;

        if (in_bounds(x + 1, y, z) && is_source[idx(x + 1, y, z)])
        {
            count_x++;
        }
        if (in_bounds(x - 1, y, z) && is_source[idx(x - 1, y, z)])
        {
            count_x++;
        }

        if (in_bounds(x, y, z + 1) && is_source[idx(x, y, z + 1)])
        {
            count_z++;
        }
        if (in_bounds(x, y, z - 1) && is_source[idx(x, y, z - 1)])
        {
            count_z++;
        }

        bool supported = in_bounds(x, y - 1, z) && is_solid_block(x, y - 1, z);
        if (count_x >= 1 && count_z >= 1 && supported)
        {
            is_source[i] = true;
            new_level = LEVEL_SOURCE;
        }
    }

    if (new_level != old_level) 
    {
        water_level[i] = new_level;
        sync_to_world(x, y, z);
        on_block_changed(x,y,z);
    }
}

void FluidSimulator::sync_to_world(int x, int y, int z) 
{
    int i = idx(x, y, z);
    int level = water_level[i];
    BlockType current = world->get_block(x, y, z).get_type();
    
    if (level <= LEVEL_EMPTY) 
    {
        if (current == WATER)
        {
            world->set_block(x, y, z, AIR);
        }
    }
    else 
    {
        if (current != WATER)
        {
            world->set_block(x, y, z, WATER);
        }
    }
}

void FluidSimulator::update(float dt) 
{
    std::vector<int> to_update(pending_updates.begin(), pending_updates.end());
    pending_updates.clear();
    
    for (int i : to_update)
    {
        int z = (i / world_size);   
        int y = z / world_size; 
        z = z % world_size;  
        int x = i % world_size; 
        
        int idx_y = i / (world_size * world_size);
        int rest = i % (world_size * world_size);
        int idx_z = rest / world_size;
        int idx_x = rest % world_size;
        update_cell(idx_x, idx_y, idx_z, dt);
    }
}

void FluidSimulator::add_water(int x, int y, int z, bool source) 
{
    if (!can_hold_water(x, y, z))
    {
        return;
    }

    int i = idx(x, y, z);
    
    if (source)
    {
        water_level[i] = LEVEL_SOURCE;
        is_source[i] = true;
    }
    else
    {
        water_level[i] = LEVEL_MAX; 
        is_source[i] = false;
    }
    
    sync_to_world(x, y, z);
    on_block_changed(x,y,z);
}

void FluidSimulator::on_block_changed(int x,int y,int z)
{
    schedule_update(x,y,z);
    schedule_update(x+1,y,z);
    schedule_update(x-1,y,z);
    schedule_update(x,y+1,z);
    schedule_update(x,y-1,z);
    schedule_update(x,y,z+1);
    schedule_update(x,y,z-1);
}

void FluidSimulator::remove_water(int x, int y, int z) 
{
    if (!in_bounds(x, y, z))
    {
        return;
    }
    int i = idx(x, y, z);
    water_level[i] = LEVEL_EMPTY;
    is_source[i] = false;
    
    sync_to_world(x, y, z);
    on_block_changed(x,y,z);
}