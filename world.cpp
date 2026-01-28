#include "world.h"
#include "raylib.h"
#include <ctime>
#include <cstdlib>

void World::allocate_world()
{
    blocks = new Block[SIZE * HEIGHT * SIZE];
}

void World::free_world()
{
    if(blocks != nullptr)
    {
        delete[] blocks;
        blocks = nullptr;
    }
}

int World::get_index(int x, int y, int z)
{
    if ( x < 0 || x >= SIZE || y < 0 || y >= HEIGHT || z< 0 || z >= SIZE)
    {
        return -1;
    }
    return x * HEIGHT * SIZE + y * SIZE + z;
}

bool World::is_exposed(int x, int y, int z)
{
    if (get_block(x, y, z).get_type() == AIR)
    {
        return false;
    }

    if (x == 0 || get_block(x - 1, y, z).get_type() == AIR)
    {
        return true;
    }
    if (x == SIZE - 1 || get_block(x + 1, y, z).get_type() == AIR)
    {
        return true;
    }
    if (y == 0 || get_block(x, y - 1, z).get_type() == AIR)
    {
        return true;
    }
    if (y == HEIGHT - 1 || get_block(x, y + 1, z).get_type() == AIR)
    {
        return true;
    }
    if (z == 0 || get_block(x, y, z - 1).get_type() == AIR)
    {
        return true;
    }
    if (z == SIZE - 1 || get_block(x, y, z + 1).get_type() == AIR)
    {
        return true;
    }

    return false;
}

void World::update_block_visible(int x, int y, int z)
{
    if (x < 0 || x >= SIZE || y < 0 || y >= HEIGHT || z < 0 || z >= SIZE)
    {
        return;
    }

    Block& block = blocks[get_index(x, y, z)];
    if (block.get_type() == AIR) 
    {
        block.set_visible(false);
        return;
    }
    else
    {
        bool visible = is_exposed(x,y,z);
        blocks[get_index(x, y, z)].set_visible(visible);
    }
}


float World::hashrandom(int x, int z, int seed)
{
    // 1. Combine inputs into a 1D index using large primes
    unsigned int n = (unsigned int)x;
    n += (unsigned int)z * 19349663; 
    n += (unsigned int)seed * 83492791;

    // 2. Bitwise scramble to create "Avalanche Effect" (chaos)
    n ^= (n << 13);
    n *= 0x5bd1e995; 
    n ^= (n >> 15);
    n *= 0x27d4eb2d;
    n ^= (n << 13);
    
    // 3. Normalize result to [0.0, 1.0] range
    return (float)n / (float)0xffffffffU;
}

float World::value_noise(float x, float z)
{
    int intX = (int)x;
    int intZ = (int)z;
    float fracX = x - intX;
    float fracZ = z - intZ;
    
    float v1 = hashrandom(intX, intZ, 12345);
    float v2 = hashrandom(intX + 1, intZ, 12345);
    float v3 = hashrandom(intX, intZ + 1, 12345);
    float v4 = hashrandom(intX + 1, intZ + 1, 12345);
    
    float i1 = v1 * (1 - fracX) + v2 * fracX;
    float i2 = v3 * (1 - fracX) + v4 * fracX;
    
    return i1 * (1 - fracZ) + i2 * fracZ;
}

float World::value_noise_3d(float x, float y, float z)
{
    int iX = (int)floor(x);
    int iY = (int)floor(y);
    int iZ = (int)floor(z);
    
    float fX = x - iX;
    float fY = y - iY;
    float fZ = z - iZ;

    float v000 = hashrandom(iX,     iZ,     iY);
    float v100 = hashrandom(iX + 1, iZ,     iY);
    float v010 = hashrandom(iX,     iZ + 1, iY);
    float v110 = hashrandom(iX + 1, iZ + 1, iY);
    float v001 = hashrandom(iX,     iZ,     iY + 1);
    float v101 = hashrandom(iX + 1, iZ,     iY + 1);
    float v011 = hashrandom(iX,     iZ + 1, iY + 1);
    float v111 = hashrandom(iX + 1, iZ + 1, iY + 1);

    float x1 = v000 * (1 - fX) + v100 * fX;
    float x2 = v010 * (1 - fX) + v110 * fX;
    float x3 = v001 * (1 - fX) + v101 * fX;
    float x4 = v011 * (1 - fX) + v111 * fX;

    float y1 = x1 * (1 - fZ) + x2 * fZ;
    float y2 = x3 * (1 - fZ) + x4 * fZ;

    return y1 * (1 - fY) + y2 * fY;
}

int World::get_terrain_height(int x, int z)
{
    float scale = 0.08f; //zooming the noise map
    float noise = value_noise(x * scale, z * scale);
    
    //range remapping
    float remapped_noise = noise * 2.0f - 1.0f; 
    
    int baseheight = HEIGHT / 2;
    //Amplitude Control
    int variation = (int)(remapped_noise * 8.0f); 
    int height = baseheight + variation;
    
    if(height < GROUND_BUFFER)
    {
        height = 5;
    }
    if(height >= HEIGHT - SKY_BUFFER)
    {
        height = HEIGHT - SKY_BUFFER + 1;
    }
    return height;
}

World::World() 
{
    blocks = nullptr;
    allocate_world();
    generate_world();
}

        
Block World::get_block(int x, int y, int z)
{
    int idx = get_index(x, y, z);
    if(idx != -1)
    {
        return blocks[get_index(x, y, z)];
    }
    return Block(AIR);
}

void World::set_block(int x, int y, int z, BlockType type) 
{
    int idx = get_index(x, y, z);
    if(idx != -1)
    {
        blocks[get_index(x, y, z)].set_type(type);
    }
    update_block_visible(x, y, z);
    update_block_visible(x + 1, y, z);
    update_block_visible(x - 1, y, z);
    update_block_visible(x, y + 1, z);
    update_block_visible(x, y - 1, z);
    update_block_visible(x, y, z + 1);
    update_block_visible(x, y, z - 1);
}
    
int World::get_size()
{ 
    return SIZE; 
}

int World::get_height()
{ 
    return HEIGHT; 
}

void World::render_trees()
{
    srand(time(NULL));
    for(int i = 0; i < 120; i++)
    {
        int x = 5 + rand() % (SIZE - 10);
        int z = 5 + rand() % (SIZE - 10);
        int ground = get_terrain_height(x, z);
        
        if(ground >= 23 && blocks[get_index(x, ground, z)].get_type() == GRASS)
        {
            int tree_height = 4 + rand() % 3;
            
            for(int h = 1; h <= tree_height; h++)
            {
                if(ground + h < HEIGHT)
                {
                    blocks[get_index(x,ground + h,z)].set_type(WOOD);
                }
            }
            
            int top_y = ground + tree_height;
            for(int dx = -2; dx <= 2; dx++)
            {
                for(int dz = -2; dz <= 2; dz++)
                {
                    for(int dy = 0; dy <= 2; dy++)
                    {
                        int nx = x + dx;
                        int nz = z + dz;
                        int ny = top_y + dy;
                        
                        if(nx >= 0 && nx < SIZE && nz >= 0 && nz < SIZE && ny < HEIGHT)
                        {
                            if(blocks[get_index(nx,ny,nz)].get_type() == AIR)
                            {
                                if((abs(dx) <= 1 && abs(dz) <= 1) || dy > 0)
                                    {
                                        blocks[get_index(nx, ny, nz)].set_type(GRASS);
                                    }
                            }
                        }
                    }
                }
            }
        }
    }
}

void World::generate_world()
{
    // Initialize the world to AIR
    for(int x = 0; x < SIZE; x++)
    {
        for(int y = 0; y < HEIGHT; y++)
        {
            for(int z = 0; z < SIZE; z++)
            {
                blocks[get_index(x, y, z)] = Block(AIR);
            }
        }
    }
    //Deal with terrain

    float cave_scale = 0.1f;    // 缩放：值越大，洞穴越细碎
    float cave_threshold = 0.73f; // 阈值：值越大，洞穴越少 (建议 0.7 - 0.8)

    for(int x = 0; x < SIZE; x++)
        {
            for(int z = 0; z < SIZE; z++)
            {
                int height = get_terrain_height(x, z);
            
                blocks[get_index(x,0,z)] = Block(BEDROCK);

                for(int y = 1; y <= height - 4; y++)
                {
                    blocks[get_index(x, y, z)] = Block(STONE);
                }
                
                for(int y = height - 3; y < height; y++)
                {
                    if(y > 0)
                    {
                        blocks[get_index(x, y, z)] = Block(DIRT);
                    }
                }
                
                if(height >= 23)
                {
                    blocks[get_index(x, height, z)] = Block(GRASS);
                }
                else
                {
                    blocks[get_index(x, height, z)] = Block(SAND);
                }
                
                for(int y = 1; y <= 22; y++)
                {
                    if(blocks[get_index(x,y,z)].get_type() == AIR)
                    {
                       blocks[get_index(x,y,z)].set_type(WATER);
                    }
                }
                 for(int y = 1; y <= height; y++)
            {
                int idx = get_index(x, y, z);
                
                // 只有当前的方块不是水也不是基岩时，才考虑挖洞
                if (blocks[idx].get_type() != WATER && blocks[idx].get_type() != BEDROCK)
                {
                    float noise3d = value_noise_3d(x * cave_scale, y * cave_scale, z * cave_scale);
                    
                    if (noise3d > cave_threshold) {
                        blocks[idx] = Block(AIR); // 挖成空气
                    }
                }
            }
            }
        }
        render_trees();

    for(int x = 0; x < SIZE; x++)
            for(int y = 0; y < HEIGHT; y++)
                for(int z = 0; z < SIZE; z++)
                    update_block_visible(x, y, z); 
}

void World::render(Vector player_pos)
{
    int min_x = player_pos.x - RENDER_DISTANCE;
    int max_x = player_pos.x + RENDER_DISTANCE;
    int min_z = player_pos.z - RENDER_DISTANCE;
    int max_z = player_pos.z + RENDER_DISTANCE;
    if(min_x < 0)
    {
        min_x = 0;
    }
    if(max_x >= SIZE)
    {
        max_x = SIZE - 1;
    }
    if(min_z < 0)
    {
        min_z = 0;
    }
    if(max_z >= SIZE)
    {
        max_z = SIZE - 1;
    }

    for(int x = min_x; x <= max_x; x++) 
    {
        for(int z = min_z; z <= max_z; z++) 
        {
            for(int y = 0; y < HEIGHT; y++)
            {
                if(y - player_pos.y > 25 || y - player_pos.y < -25)
                {
                    continue;
                }

                Block block = blocks[get_index(x, y, z)];
                if(block.is_solid() && block.is_visible())
                {
                    DrawCube({(float)x, (float)y, (float)z}, 1.0f, 1.0f, 1.0f, block.get_color());
                    DrawCubeWires({(float)x, (float)y, (float)z}, 1.0f, 1.0f, 1.0f, BLACK);
                }
            }
        }
    }
}
