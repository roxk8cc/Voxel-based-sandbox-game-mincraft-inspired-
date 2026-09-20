#ifndef FLUID_SIMULATOR_H
#define FLUID_SIMULATOR_H

#include "world.h"
#include <vector>
#include <queue>
#include <set>

/**
 * @class Fluid simulator
 * Try to make the static water blcok flow like real water
 */
class FluidSimulator
{
    private:
        static constexpr float REFILL_DELAY = 0.15f;
        static constexpr int EMPTY = -1;
        static constexpr int SOURCE = 0;
    
        World* world;
        int world_size;
        int world_height;
        std::vector<int> water_level;
        std::vector<bool> is_source;
        std::set<int> pending_updates;
        std::vector<float> refill_timer;

        inline int idx(int x, int y, int z) const 
        { 
            return x + z * world_size + y * world_size * world_size; 
        }
     
        /**
         * Checks whether the given coordinates are within world bounds
         * @return bool true if in bounds
         */
        bool in_bounds(int x, int y, int z);

        /**
         * Checks the block at these coordinates can act as a container for fluid
         * @return bool true if can
         */
        bool can_hold_water(int x, int y, int z);

        /**
         * Checks if the block is a physical obstacle 
         * @return true if yes
         */
        bool is_solid_block(int x, int y, int z);
        
        /**
         * Initializes water level and source arrays from the world data
         */
        void initialize_from_world();

        /**
         * Schedules a cell to be updated in the next simulation step
         */
        void schedule_update(int x, int y, int z);

        /**
         * Updates a single water cell
         * @param x X-coordinate
         * @param y Y-coordinate
         * @param z Z-coordinate
         * @param deltatime Delta time
         */
        void update_cell(int x, int y, int z, float deltatime);
        
        /**
         * Calculates the water level for a given cell based on neighbors
         * @return New water level
         */
        int calculate_flow_level(int x, int y, int z);
        
        /**
         * Syncs water level for a cell to the actual world block
         */
        void sync_to_world(int x, int y, int z);
        
    public:

        /**
         * Default Constructor.
         * #param world a pointer to world data,
         */
        FluidSimulator(World* world);

        /**
         * Destructor
         */
        ~FluidSimulator();

        /**
        * Updates the water simulation.
        * @param deltatime Time elapsed since the last update
        */
        void update(float dletatime);

        /**
        * Adds water at a specific position.
        */
        void add_water(int x, int y, int z, bool is_source = false);

        /**
         * remove water at a specific position.
         */
        void remove_water(int x, int y, int z);

        /**
         * Notifies the simulator that a block in the world has changed.
         */
        void on_block_changed(int x, int y, int z);
};

#endif