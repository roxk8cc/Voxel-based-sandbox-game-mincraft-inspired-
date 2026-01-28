#include "block.h" 

Block::Block()
{
    type = AIR;
    visible = false;
}

Block::Block(BlockType type)
{
    this->type = type;
    
}

BlockType Block::get_type()
{
    return type;
}

void Block::set_type(BlockType type)
{
    this->type = type;
}

bool Block::is_solid()
{
    return type != AIR;
}

bool Block::is_visible()
{
    return visible;
}

void Block::set_visible(bool v)
{
    visible = v;
}

Color Block::get_color()
{
    switch(type) 
    {
        case GRASS: 
            return (Color){60, 179, 113, 255};
        case STONE: 
            return (Color){128, 128, 128, 255};
        case WOOD:
            return (Color){101, 67, 33, 255};
        case SAND:
            return (Color){238, 214, 175, 255};
        case WATER:
            return (Color){65, 105, 225, 150};
        case DIRT:
            return (Color){139, 90, 43, 255};
        case BEDROCK:
            return (Color){50, 50, 50, 255};
        default: 
            return WHITE;
    }
}
