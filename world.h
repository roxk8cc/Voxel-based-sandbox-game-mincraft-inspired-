#ifndef WORLD_H
#define WORLD_H

#include "block.h"
#include "vector3d.h"

/**
 * @class World
 * A container for the game enviroment
 */
class World 
{
    private:
        static const int RENDER_DISTANCE = 40;
        static const int GROUND_BUFFER = 5;
        static const int SKY_BUFFER = 5;
        static const int SIZE = 80;
        static const int HEIGHT = 50;
        Block* blocks;

        /**
         * Allocate memory for the world blocks.
         */
        void allocate_world();
        
        /**
         * Calculate the index in the 1D array for 3D coordinates.
         * @param x The X-coordinate.
         * @param y The Y-coordinate.
         * @param z The Z-coordinate.
         * @return The index in the 1D array.
         */
        int get_index(int x, int y, int z);

        /**
         * Check if a face of a block is visible (adjacent block is air).
         * @param x The X-coordinate.
         * @param y The Y-coordinate.
         * @param z The Z-coordinate.
         * @return True if the face is visible.
         */
        bool is_exposed(int x, int y, int z);

        /**
         * Recalculates and updates the visibilility of block.
         * @param x, y, z The coordinates of the block to update.
         */
        void update_block_visible(int x, int y, int z);

        /**
         * Calculates the final vertical height (Y-axis) for a given (X, Z) coordinate.
         */
        int get_terrain_height(int x, int z);
    
        float hashrandom(int x, int z, int seed);

        float value_noise(float x, float z);

    public:

        /**
         * A default constructor that initializes the world.
         */
        World();

        /**
         * Free memory for the world blocks.
         */
        void free_world();
    
        /**
         * Return the block at the location.
         * @param x The X-coordinate (width).
         * @param y The Y-coordinate (height).
         * @param z The Z-coordinate (depth).
         * @return The Block at the location or a default AIR block if out of bounds.
         */
        Block get_block(int x, int y, int z);
        
        /**
         * Set a block to a new type.
         * @param x The X-coordinate.
         * @param y The Y-coordinate.
         * @param z The Z-coordinate.
         * @param type The new BlockType
         */
        void set_block(int x, int y, int z, BlockType type);
        
        /**
         * Draw the Block.
         * @param cameraPos The camera position to determine render distance.
         */
        void render(Vector player_pos);
    
        /**
         * Gets the size of the world.
         * @return The value of the SIZE.
         */
        int get_size();

        /**
         * Gets the height of the world.
         * @return The value of the HEIGHT.
         */
        int get_height();
        
        /**
         * Generate the world.
         */
        void generate_world();

        void render_trees();

        float value_noise_3d(float x, float y, float z);
};

#endif