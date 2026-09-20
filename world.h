#ifndef WORLD_H
#define WORLD_H

#include "block.h"
#include "vector3d.h"
#include <map>

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
        static const int SIZE = 100;
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
    
        /**
         * Generates a pseudo-random number based on coordinates and a seed
         */
        float hashrandom(int x, int z, int seed);

        /**
         * 2D value noise, produces smooth random values for (x, z)
         */
        float value_noise(float x, float z);

        /**
         * 3D value noise produces smooth random values for (x, y, z)
         */
        float value_noise_3d(float x, float y, float z);

        /**
         * Smooth fade function used in Perlin noise for smooth interpolation
         */
        float fade(float t);

        /**
         * Linear interpolation function, computes value between a and b by t
         */
        float lerp(float t, float a, float b);

        /**
         * 2D Perlin noise gradient function, generates directional contribution
         */
        float grad(int hash, float x, float z);

        /**
         * 3D Perlin noise gradient function, generates directional contribution
         */
        float grad3d(int hash, float x, float y, float z);

        /**
         * 2D Perlin noise function, produces smooth random height or variation
         */
        float perlin_noise(float x, float z);

        /**
         * 3D Perlin noise function, produces smooth random values for volume or fluid
         */
        float perlin_noise_3d(float x, float y, float z);

        /**
         * Generate the terrain of the wold.
         */
        void generate_world();

        /**
         * Render trees within the world
         */
        void render_trees();

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
    
        void render(Vector player_pos, std::map<BlockType, Texture2D>& block_textures);
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
         * Draw a single block at a specific position with a texture.
         * @param texture The texture to apply to the block when rendering.
         * @param position The 3D position of the block in the world.
         * @param block The block object containing type and other properties.
         */
        void DrawBlock(Texture2D texture, Vector3 position, Block& block);
};

#endif