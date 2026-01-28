#ifndef BLOCK_H
#define BLOCK_H

#include "raylib.h" 

/**
 * All the types of block.
 */
enum BlockType 
{
    AIR = 0,
    GRASS,
    STONE,
    WOOD,
    SAND,
    WATER,
    DIRT,
    BEDROCK
};

/**
 * @class Block
 * @brief It represent the a basic block unit in the world.
 */
class Block
{
    private:
        BlockType type;
        bool visible;

    public:
        /**
         * A default constructor.
         * Initialise the block to AIR type.
         */
        Block();

        /**
         * A non-default constructor.
         * Initialise the block of a specific type.
         */
        Block(BlockType type);

        /**
         * Return a BlockType of the block.
         * @return the current type assigned to this block.
         */
        BlockType get_type();

        /**
         * Update the block to a new type.
         * @param type the type should be changed to
         */
        void set_type(BlockType type);

        /**
         * Check if the block is solid or it is air.
         * @return bool True if the block is solid, False if it is air
         */
        bool is_solid();

        /**
         * Check if the block is visible.
         * @return bool True if the block is visible
         */
        bool is_visible();

        /**
         * Sets whether the object should be rendered.
         * @param v: True to show the object, false to hide it.
         */
        void set_visible(bool v);

        /**
         * Return the color of the block.
         * @return color of the block.
         */
        Color get_color();

};

#endif