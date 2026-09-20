#include "world.h"
#include "raylib.h"
#include <ctime>
#include <cstdlib>
#include "rlgl.h"

float World::fade(float t)
{
    return t * t * t * (t * (t * 6 - 15) + 10);
}

float World::lerp(float t, float a, float b)
{
    return a + t * (b - a);
}

float World::grad(int hash, float x, float z)
{
    int h = hash & 15; 
    float u = h < 8 ? x : z;
    float v = h < 4 ? z : (h == 12 || h == 14 ? x : z);
    return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
}

float World::grad3d(int hash, float x, float y, float z)
{
    int h = hash & 15;
    float u = h < 8 ? x : y;
    float v = h < 4 ? y : (h == 12 || h == 14 ? x : z);
    return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
}

float World::perlin_noise(float x, float z)
{
    int X = (int)floor(x) & 255;
    int Z = (int)floor(z) & 255;

    x -= floor(x);
    z -= floor(z);

    float u = fade(x);
    float v = fade(z);

    auto get_hash = [](int xi, int zi) 
    {
        unsigned int h = (unsigned int)xi * 374761393 + (unsigned int)zi * 668265263; 
        h = (h ^ (h >> 13)) * 1274126177;
        return h ^ (h >> 16);
    };
    int aa = get_hash(X, Z);
    int ab = get_hash(X, Z + 1);
    int ba = get_hash(X + 1, Z);
    int bb = get_hash(X + 1, Z + 1);
    return lerp(v, lerp(u, grad(aa, x, z), grad(ba, x - 1, z)), lerp(u, grad(ab, x, z - 1), grad(bb, x - 1, z - 1)));
}

float World::perlin_noise_3d(float x, float y, float z)
{
    int X = (int)floor(x) & 255;
    int Y = (int)floor(y) & 255;
    int Z = (int)floor(z) & 255;

    x -= floor(x);
    y -= floor(y);
    z -= floor(z);

    float u = fade(x);
    float v = fade(y);
    float w = fade(z);

    auto get_hash_3d = [](int xi, int yi, int zi)
    {
        unsigned int h = (unsigned int)xi * 374761393 + (unsigned int)yi * 668265263 + (unsigned int)zi * 1445635377;
        h = (h ^ (h >> 13)) * 1274126177;
        return h ^ (h >> 16);
    };

    int aaa = get_hash_3d(X, Y, Z);
    int aba = get_hash_3d(X, Y + 1, Z);
    int aab = get_hash_3d(X, Y, Z + 1);
    int abb = get_hash_3d(X, Y + 1, Z + 1);
    int baa = get_hash_3d(X + 1, Y, Z);
    int bba = get_hash_3d(X + 1, Y + 1, Z);
    int bab = get_hash_3d(X + 1, Y, Z + 1);
    int bbb = get_hash_3d(X + 1, Y + 1, Z + 1);

    return lerp(w,lerp(v,lerp(u, grad3d(aaa, x, y, z), grad3d(baa, x - 1, y, z)),
        lerp(u, grad3d(aba, x, y - 1, z), grad3d(bba, x - 1, y - 1, z))),
        lerp(v,lerp(u, grad3d(aab, x, y, z - 1), grad3d(bab, x - 1, y, z - 1)),
        lerp(u, grad3d(abb, x, y - 1, z - 1), grad3d(bbb, x - 1, y - 1, z - 1)))
    );
}

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
    float warp_strength = 18.0f;
    float warp_x = perlin_noise(x * 0.01f, z * 0.01f) * warp_strength;
    float warp_z = perlin_noise((x + 1000) * 0.01f, (z + 1000) * 0.01f) * warp_strength;
    
    float warped_x = x + warp_x;
    float warped_z = z + warp_z;
    
    float total = 0.0f;
    float frequency = 0.055f;
    float amplitude = 1.0f;
    float max_value = 0.0f;
    
    int octaves = 5;
    float persistence = 0.50f;

    for(int i = 0; i < octaves; i++)
    {
        total += perlin_noise((warped_x + 12345) * frequency, (warped_z + 12345) * frequency) * amplitude;
        max_value += amplitude;
        amplitude *= persistence;
        frequency *= 2.0f;
    }

    float heightMap = total / max_value; 
    int base_height = 20; 
    int max_variation = 16;
    int height = base_height + (int)(heightMap * max_variation);
    if(height < GROUND_BUFFER)
    {
        height = 5;
    }
    if(height >= HEIGHT - SKY_BUFFER)
    {
        height = HEIGHT - SKY_BUFFER;
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
    for(int i = 0; i < 200; i++)
    {
        int x = 5 + rand() % (SIZE - 10);
        int z = 5 + rand() % (SIZE - 10);
        int ground = get_terrain_height(x, z);
        
        if(ground >= 20 && blocks[get_index(x, ground, z)].get_type() == GRASS)
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
            for(int delta_x = -2; delta_x <= 2; delta_x++)
            {
                for(int delta_z = -2; delta_z <= 2; delta_z++)
                {
                    for(int delta_y = 0; delta_y <= 2; delta_y++)
                    {
                        int leaf_x = x + delta_x;
                        int leaf_z = z + delta_z;
                        int leaf_y = top_y + delta_y;
                        
                        if(leaf_x >= 0 && leaf_x < SIZE && leaf_z >= 0 && leaf_z < SIZE && leaf_y < HEIGHT)
                        {
                            if(blocks[get_index(leaf_x,leaf_y,leaf_z)].get_type() == AIR)
                            {
                                if((abs(delta_x) <= 1 && abs(delta_z) <= 1) || delta_y > 0)
                                    {
                                        blocks[get_index(leaf_x, leaf_y, leaf_z)].set_type(LEAVES);
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

    float cave_scale = 0.2f; 
    float cave_threshold = 0.7f; 
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
                
                if(height >= 19)
                {
                    blocks[get_index(x, height, z)] = Block(GRASS);
                }
                else
                {
                    blocks[get_index(x, height, z)] = Block(SAND);
                }
                
                for(int y = 1; y <= 18; y++)
                {
                    if(blocks[get_index(x,y,z)].get_type() == AIR)
                    {
                       blocks[get_index(x,y,z)].set_type(WATER);
                    }
                }
                 for(int y = 1; y <= height; y++)
                {
                    int idx = get_index(x, y, z);
                    if (blocks[idx].get_type() != WATER && blocks[idx].get_type() != BEDROCK)
                    {
                        float noise3d = value_noise_3d(x * cave_scale, y * cave_scale, z * cave_scale);
                        
                        if (noise3d > cave_threshold)
                        {
                            blocks[idx] = Block(AIR);
                        }
                    }
                }
            }
        }
        render_trees();
    for(int x = 0; x < SIZE; x++)
    {        
        for(int y = 0; y < HEIGHT; y++)
        {
            for(int z = 0; z < SIZE; z++)
            {
                update_block_visible(x, y, z); 
            }
        }
    }
}

void World::DrawBlock(Texture2D texture, Vector3 position, Block& block) 
{
    if (!block.has_texture())
    {
        DrawCube(position, 1.0f, 1.0f, 1.0f, block.get_color());
        return;
    }
    float x = position.x;
    float y = position.y;
    float z = position.z;
    float s = 0.5f;
    float u_step = 1.0f / 4.0f; 
    float v_step = 1.0f / 3.0f; 

    rlSetTexture(texture.id);
    rlBegin(RL_QUADS);
    rlColor4ub(255, 255, 255, 255);

    Vector2 side_UV = block.get_uv_offset(FACE_SIDE);
    float u = side_UV.x * u_step;
    float v = side_UV.y * v_step;

    //offset of top side
    Vector2 top_UV = block.get_uv_offset(FACE_TOP);
    float top_u = top_UV.x * u_step;
    float top_v = top_UV.y * v_step;
    rlTexCoord2f(top_u, top_v);
    rlVertex3f(x - s, y + s, z - s);
    rlTexCoord2f(top_u, top_v + v_step);
    rlVertex3f(x - s, y+s, z+s);
    rlTexCoord2f(top_u + u_step, top_v + v_step); 
    rlVertex3f(x+s, y+s, z+s);
    rlTexCoord2f(top_u + u_step, top_v);
    rlVertex3f(x+s, y+s, z - s);

    //offset of top side
    Vector2 botUV = block.get_uv_offset(FACE_BOTTOM);
    float bu = botUV.x * u_step;
    float bv = botUV.y * v_step;
    rlTexCoord2f(bu, bv);
    rlVertex3f(x - s, y - s, z - s);
    rlTexCoord2f(bu+u_step, bv);
    rlVertex3f(x + s, y - s, z - s);
    rlTexCoord2f(bu + u_step, bv + v_step);
    rlVertex3f(x + s, y - s, z + s);
    rlTexCoord2f(bu, bv + v_step);
    rlVertex3f(x - s, y - s, z + s);

    rlTexCoord2f(u, v + v_step);
    rlVertex3f(x - s, y - s, z+s);
    rlTexCoord2f(u + u_step, v + v_step);
    rlVertex3f(x+s, y - s, z + s);
    rlTexCoord2f(u  +  u_step, v);
    rlVertex3f(x + s, y + s, z + s);
    rlTexCoord2f(u, v);
    rlVertex3f(x - s, y + s, z + s);

    rlTexCoord2f(u, v  +  v_step);
    rlVertex3f(x - s, y - s, z - s);
    rlTexCoord2f(u + u_step, v + v_step);
    rlVertex3f(x - s, y - s, z + s);
    rlTexCoord2f(u + u_step, v);
    rlVertex3f(x - s, y + s, z +s );
    rlTexCoord2f(u, v);
    rlVertex3f(x - s, y+s, z - s);

    rlTexCoord2f(u, v + v_step);
    rlVertex3f(x+s, y - s, z+s);
    rlTexCoord2f(u + u_step, v + v_step);
    rlVertex3f(x+s, y - s, z - s);
    rlTexCoord2f(u + u_step, v);
    rlVertex3f(x+s, y+s, z - s);
    rlTexCoord2f(u, v);
    rlVertex3f(x + s, y + s, z + s);
   
    rlTexCoord2f(u, v  +  v_step);
    rlVertex3f(x + s, y - s, z - s); 
    rlTexCoord2f(u  +  u_step, v  +  v_step);
    rlVertex3f(x - s, y - s, z - s);
    rlTexCoord2f(u  +  u_step, v);
    rlVertex3f(x - s, y + s, z - s);
    rlTexCoord2f(u, v);
    rlVertex3f(x + s, y + s, z - s);
        
    rlEnd();
    rlSetTexture(0);
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

void World::render(Vector player_pos, std::map<BlockType, Texture2D>& block_textures)
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
                    continue;

                Block& block = blocks[get_index(x, y, z)];
                if(block.is_solid() && block.is_visible())
                {
                    auto it = block_textures.find(block.get_type());
                    if(it != block_textures.end() && it-> second.id > 0)
                    {
                        DrawBlock(it->second, {(float)x, (float)y, (float)z}, block);
                    }
                    else
                    {
                        DrawCube({(float)x, (float)y, (float)z}, 1.0f, 1.0f, 1.0f, block.get_color());
                        DrawCubeWires({(float)x, (float)y, (float)z}, 1.0f, 1.0f, 1.0f, BLACK);
                    }
                }
            }
        }
    }
}