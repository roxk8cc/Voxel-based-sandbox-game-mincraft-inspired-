#include "entity.h"
#include "raylib.h"
#include <cmath>
#include <cstdlib>
#include <ctime>

Entity::Entity(Vector position, float health)
{
    this->position = position;
    this->health = health;
    is_alive = true;
    width = 0.8f;
    height = 1.8f;
    gravity = {0,0,0};
    is_fleeing = false;
    flee_direction = init_vector();
    flee_timer = 0.0f;
    last_attacker_position = init_vector();
    is_hit = false;
    hit_timer = 0;
}

Entity::~Entity()
{}

void Entity::take_damage(float damage, Vector attacker_position)
{
    if(!is_alive)
    {
        return;
    }
    health -= damage;
    if(health <= 0)
    {
        health = 0;
        is_alive = false;
    }
    last_attacker_position = attacker_position;
    start_fleeing(attacker_position);
    is_hit = true;
    hit_timer = HIT_FLASH_DURATION;
}

void Entity::start_fleeing(Vector danger_position)
{
    is_fleeing = true;
    
    Vector to_safety = subtract_vec(position, danger_position);
    to_safety.y = 0;
    
    if(length_vec(to_safety) > 0.1f)
    {
        flee_direction = normalize_vec(to_safety);
    }
    else
    {
        float random_angle = (float)(rand() % 360) * 3.14159f / 180.0f;
        flee_direction.x = cos(random_angle);
        flee_direction.z = sin(random_angle);
        flee_direction.y = 0;
    }
}

bool Entity::is_entity_fleeing()
{
    return is_fleeing;
}

void Entity::update_hit_flash(float deltatime)
{
    if(is_hit)
    {
        hit_timer -= deltatime;
        if(hit_timer <= 0.0f)
        {
            is_hit = false;
            hit_timer = 0.0f;
        }
    }
}

bool Entity::check_collision(World& world)
{
    float epsilon = 0.01;
    
    int min_x = (int)floor(position.x - width);
    int max_x = (int)floor(position.x + width);
    int min_y = (int)floor(position.y - height + epsilon);
    int max_y = (int)floor(position.y - epsilon);
    int min_z = (int)floor(position.z - width);
    int max_z = (int)floor(position.z + width);
    
    for(int x = min_x; x <= max_x; x++)
    {
        for(int y = min_y; y <= max_y; y++)
        {
            for(int z = min_z; z <= max_z; z++)
            {
                if(world.get_block(x, y, z).is_solid())
                {
                    return true;
                }
            }
        }
    }
    return false;
}

Color Entity::get_hit_color()
{
    if(!is_hit || hit_timer <= 0.0f)
    {
        return WHITE;
    }
    float blink_speed = 10.0f;
    float blink_value = sinf(hit_timer * blink_speed * 3.1415926f);

    if(blink_value > 0.0f)
    {
        float intensity = hit_timer / HIT_FLASH_DURATION;
        unsigned char red = 225;
        unsigned char green = (unsigned char)(255 * (1.0f - intensity));
        unsigned char blue = (unsigned char)(255 * (1.0f - intensity));
        return (Color){red, green, blue, 255};
    }
    else
    {
        return WHITE;
    }
}

Vector Entity::get_position()
{
    return position;
}

float Entity::get_height()
{
    return height;
}

bool Entity::is_entity_alive()
{
    return is_alive;
}

void Entity::fix_entity_stuck(World& world)
{
    const float margin = 0.5f;
    const float lift_step = 0.5f;
    const int max_attempts = 10;
    Vector original = position;

    for(int attempt = 0; attempt < max_attempts; attempt++)
    {
        if(!check_collision(world))
        {
            return;
        }

        Vector candidates[] = {
            {original.x + margin, original.y, original.z},
            {original.x - margin, original.y, original.z},
            {original.x, original.y, original.z + margin},
            {original.x, original.y, original.z - margin},
            {original.x, original.y + lift_step, original.z}
        };

        bool fixed = false;
        for(Vector candidate : candidates)
        {
            position = candidate;
            if(!check_collision(world))
            {
                fixed = true;
                break;
            }
        }
        if(fixed)
        {
            return;
        }
        original.y += lift_step;
        position = original;
    }

    int base_x = (int)floor(position.x);
    int base_z = (int)floor(position.z);
    int y = (int)floor(position.y);

    for(int i = 0; i < 6; i++)
    {
        if(world.get_block(base_x, y - i, base_z).is_solid())
        {
            position.y = (float)(y - i + 1) + height;
            gravity = init_vector();
            return;
        }
    }
}

bool Entity::on_the_ground(Vector& position,Vector& gravity, float height, World& world, float deltatime)
{
    gravity.y -= 9.8f * deltatime;
    if (gravity.y < -20.0f)
    {
        gravity.y = -20.0f;
    }

    float next_y = position.y + gravity.y * deltatime;

    if (gravity.y > 0.0f)
    {
        position.y = next_y;
        return false;
    }

    float foot_y = next_y - height;

    int min_x = (int)floor(position.x - width);
    int max_x = (int)floor(position.x + width);
    int min_z = (int)floor(position.z - width);
    int max_z = (int)floor(position.z + width);
    int foot_block_y = (int)floor(foot_y);

    for (int x = min_x; x <= max_x; x++)
    {
        for (int z = min_z; z <= max_z; z++)
        {
            if (world.get_block(x, foot_block_y, z).is_solid())
            {
                position.y = (float)(foot_block_y + 1) + height;
                gravity.y = 0.0f;
                return true;
            }
        }
    }
    position.y = next_y;
    return false;
}


void Entity::handle_movement(Vector move, World &world)
{
    const float step_height = 0.6f;
    Vector original = position;
    position.x += move.x;
    if (check_collision(world))
    {
        position = original;
        position.y += step_height;
        position.x += move.x;
        if (check_collision(world))
        {
            position = original;
        }
    }
    original = position;

    position.z += move.z;
    if (check_collision(world))
    {
        position = original;
        position.y += step_height;
        position.z += move.z;
        if (check_collision(world))
        {
            position = original;
        }
    }
}


bool Entity::ray_intersects(Vector ray_origin, Vector ray_direction, float max_distance, float& hit_distance)
{
    if(!is_alive)
    {
        return false;
    }
    
    float half_width = width / 2.0f;
    
    Vector box_min = {position.x - half_width, position.y - height, position.z - half_width};
    Vector box_max = {position.x + half_width, position.y, position.z + half_width};
    
    float tmin = 0.0f;
    float tmax = max_distance;
    
    if(fabs(ray_direction.x) > 0.0001f)
    {
        float t1 = (box_min.x - ray_origin.x) / ray_direction.x;
        float t2 = (box_max.x - ray_origin.x) / ray_direction.x;
        
        if(t1 > t2) { float temp = t1; t1 = t2; t2 = temp; }
        
        tmin = fmax(tmin, t1);
        tmax = fmin(tmax, t2);
        
        if(tmin > tmax) return false;
    }
    else if(ray_origin.x < box_min.x || ray_origin.x > box_max.x)
    {
        return false;
    }
    
    if(fabs(ray_direction.y) > 0.0001f)
    {
        float t1 = (box_min.y - ray_origin.y) / ray_direction.y;
        float t2 = (box_max.y - ray_origin.y) / ray_direction.y;
        
        if(t1 > t2) { float temp = t1; t1 = t2; t2 = temp; }
        
        tmin = fmax(tmin, t1);
        tmax = fmin(tmax, t2);
        
        if(tmin > tmax) return false;
    }
    else if(ray_origin.y < box_min.y || ray_origin.y > box_max.y)
    {
        return false;
    }

    if(fabs(ray_direction.z) > 0.0001f)
    {
        float t1 = (box_min.z - ray_origin.z) / ray_direction.z;
        float t2 = (box_max.z - ray_origin.z) / ray_direction.z;
        
        if(t1 > t2) { float temp = t1; t1 = t2; t2 = temp; }
        
        tmin = fmax(tmin, t1);
        tmax = fmin(tmax, t2);
        
        if(tmin > tmax) return false;
    }
    else if(ray_origin.z < box_min.z || ray_origin.z > box_max.z)
    {
        return false;
    }
    
    hit_distance = tmin;
    return true;
}

Sheep::Sheep(Vector position) : Entity(position, 20)
{
    wander_timer = 0.0f;
    wander_direction = init_vector();
    width = 0.6f;
    height = 1.0f;
}

void Sheep::handle_flee(float deltatime, World& world)
{
    flee_timer -= deltatime;
    
    if(flee_timer <= 0)
    {
        is_fleeing = false;
        flee_direction = init_vector();
        return;
    }
    
    if(length_vec(flee_direction) > 0)
    {
        Vector move_step = {flee_direction.x * FLEE_SPEED * deltatime, 0,flee_direction.z * FLEE_SPEED * deltatime};
        handle_movement(move_step, world);
    }
}

void Sheep::handle_wander(float deltatime, World& world)
{
    wander_timer -= deltatime;
    
    if(wander_timer <= 0)
    {
        wander_timer = (float)(rand() % 4 + 2);
        
        if(rand() % 100 < 30)
        {
            wander_direction = init_vector();
        }
        else
        {
            float direction_x = (float)(rand() % 200 - 100) / 100.0f; 
            float direction_z = (float)(rand() % 200 - 100) / 100.0f;
            
            wander_direction = {direction_x, 0, direction_z};
            wander_direction = normalize_vec(wander_direction);
        }
    }
    
    if(length_vec(wander_direction) > 0)
    {
        Vector move_step = {
            wander_direction.x * NORMAL_SPEED * deltatime,
            0,
            wander_direction.z * NORMAL_SPEED * deltatime
        };
        handle_movement(move_step, world);
    }
}

void Sheep::update(float deltatime, World& world)
{
    if(!is_alive)
    {
        return;
    }
    update_hit_flash(deltatime);
    
    if(check_collision(world))
    {
        fix_entity_stuck(world);
    }
    
    bool on_ground = on_the_ground(position, gravity, height, world, deltatime);
    
    if(on_ground)
    {
        if(is_fleeing)
        {
            handle_flee(deltatime, world);
        }
        else
        {
            handle_wander(deltatime, world);
        }
    }

    float margin = width / 2;
    if(position.x < margin)
    {
        position.x = margin;
    }
    if(position.x >= world.get_size() - margin)
    {
        position.x = world.get_size() - margin;
    }
    if(position.z < margin)
    {
        position.z = margin;
    }
    if(position.z >= world.get_size() - margin)
    {
        position.z = world.get_size() - margin;
    }
}

void Sheep::render()
{
    if(!is_alive)
    {
        return;
    }

    Color hit_color = get_hit_color();
    Color body_color;

    if(is_hit && hit_timer > 0)
    {
        body_color = hit_color;
    }
    else if(is_fleeing)
    {
        body_color = (Color){255, 200, 200, 255};
    }
    else
    {
        body_color = WHITE;
    }
    
    DrawCube({(float)position.x, (float)position.y - 0.5f, (float)position.z}, 0.7f, 0.5f, 0.5f, body_color);
    DrawCubeWires({(float)position.x, (float)position.y - 0.5f, (float)position.z}, 0.7f, 0.5f, 0.5f, GRAY);
    
    DrawCube({(float)position.x, (float)position.y - 0.25f, (float)position.z + 0.4f}, 0.4f, 0.4f, 0.4f, body_color);
    DrawCubeWires({(float)position.x, (float)position.y - 0.25f, (float)position.z + 0.4f}, 0.4f, 0.4f, 0.4f, GRAY);
    
    float leg_size = 0.12f;
    float leg_height = 0.4f;
    float leg_y = position.y - 0.7f - leg_height / 2;
    
    DrawCube({(float)position.x - 0.25f, leg_y, (float)position.z - 0.15f}, leg_size, leg_height, leg_size, BLACK);
    DrawCube({(float)position.x + 0.25f, leg_y, (float)position.z - 0.15f}, leg_size, leg_height, leg_size, BLACK);
    DrawCube({(float)position.x - 0.25f, leg_y, (float)position.z + 0.15f}, leg_size, leg_height, leg_size, BLACK);
    DrawCube({(float)position.x + 0.25f, leg_y, (float)position.z + 0.15f}, leg_size, leg_height, leg_size, BLACK);
}

Villager::Villager(Vector position) : Entity(position, 40)
{
    player_position = init_vector(0, 0, 0);
    width = 0.6f;
    height = 1.8f;

    current_state = WANDER;
    previous_state = WANDER;
    state_timer = 0.0f;
    wander_timer = 0.0f;
    wander_direction = init_vector();
}

void Villager::set_target(Vector target)
{
    player_position = target;
}

void Villager::change_state(villager_state new_state)
{
    if(new_state == current_state)
    {
        return;
    }
    
    previous_state = current_state;
    current_state = new_state;
    state_timer = 0.0f;
    
    switch(new_state)
    {
        case WANDER:
            wander_timer = 3.0f + (float)(rand() % 3);
            break;
        case FLEE:
            flee_timer = 4.0f;
            break;
        default:
            break;
    }
}

void Villager::update_state(float deltatime)
{
    state_timer += deltatime;
    
    Vector to_player = subtract_vec(player_position, position);
    to_player.y = 0;
    float distance_to_player = length_vec(to_player);
    
    switch(current_state)
    {
        case WANDER:
            if(is_fleeing)
            {
                change_state(FLEE);
            }
            else if(distance_to_player > FOLLOW_DISTANCE_MIN && 
                    distance_to_player < FOLLOW_DISTANCE_MAX)
            {
                change_state(FOLLOW_PLAYER);
            }
            else if(state_timer > wander_timer)
            {
                change_state(WANDER);
            }
            break;
            
        case FOLLOW_PLAYER:
            if(is_fleeing)
            {
                change_state(FLEE);
            }
            else if(distance_to_player > FOLLOW_DISTANCE_MAX)
            {
                change_state(WANDER);
            }
            break;
            
        case FLEE:
            if(!is_fleeing || flee_timer <= 0)
            {
                is_fleeing = false;
                change_state(WANDER);
            }
            break;
    }
}

void Villager::handle_wander_state(float deltatime, World& world)
{
    wander_timer -= deltatime;
    
    if(wander_timer <= 0 || length_vec(wander_direction) < 0.1f)
    {
        float random_angle = (float)(rand() % 360) * 3.14159f / 180.0f;
        wander_direction.x = cos(random_angle);
        wander_direction.z = sin(random_angle);
        wander_direction.y = 0;
        wander_direction = normalize_vec(wander_direction);
        wander_timer = 2.0f;
    }
    
    Vector move_step = {
        wander_direction.x * WANDER_SPEED * deltatime,
        0.0f,
        wander_direction.z * WANDER_SPEED * deltatime
    };
    handle_movement(move_step, world);
}

void Villager::handle_follow_state(float deltatime, World& world)
{
    Vector to_player = subtract_vec(player_position, position);
    to_player.y = 0;
    
    float distance = length_vec(to_player);
    
    if(distance > FOLLOW_DISTANCE_MIN)
    {
        Vector move_direction = normalize_vec(to_player);
        Vector move_step = {
            move_direction.x * FOLLOW_SPEED * deltatime,
            0.0f,
            move_direction.z * FOLLOW_SPEED * deltatime
        };
        handle_movement(move_step, world);
    }
}

void Villager::handle_flee_state(float deltatime, World& world)
{
    flee_timer -= deltatime;
    
    if(length_vec(flee_direction) > 0)
    {
        Vector move_step = {
            flee_direction.x * FLEE_SPEED * deltatime,
            0.0f,
            flee_direction.z * FLEE_SPEED * deltatime
        };
        handle_movement(move_step, world);
    }
}

void Villager::execute_current_state(float deltatime, World& world)
{
    switch(current_state)
    {
        case WANDER:
            handle_wander_state(deltatime, world);
            break;
        case FOLLOW_PLAYER:
            handle_follow_state(deltatime, world);
            break;
        case FLEE:
            handle_flee_state(deltatime, world);
            break;
    }
}


void Villager::update(float deltatime, World& world)
{
    if(!is_alive)
    {
        return;
    }

    update_hit_flash(deltatime);

    if(check_collision(world))
    {
        fix_entity_stuck(world);
    }

    bool on_ground = on_the_ground(position, gravity, height, world, deltatime);
    
    if(on_ground)
    {
        update_state(deltatime);
        execute_current_state(deltatime, world);
    }
    
    float margin = width / 2 ;
    if(position.x < margin)
    {
        position.x = margin;
    }
    if(position.x >= world.get_size() - margin)
    {
        position.x = world.get_size() - margin;
    }
    if(position.z < margin)
    {
        position.z = margin;
    }
    if(position.z >= world.get_size() - margin)
    {
        position.z = world.get_size() - margin;
    }
}

void Villager::render()
{
    if(!is_alive)
    {
        return;
    }

    Color hit_color = get_hit_color();
    Color body_color = BROWN;
    Color head_color = BEIGE;
    Color arm_color = BROWN;
    Color leg_color = DARKBROWN;

    if(is_hit && hit_timer > 0)
    {
        body_color = hit_color;
        head_color = hit_color;
        arm_color = hit_color;
        leg_color = hit_color;
    }

    DrawCube({(float)position.x, (float)position.y - 0.9f, (float)position.z}, 0.5f, 1.0f, 0.35f, body_color);
    DrawCubeWires({(float)position.x, (float)position.y - 0.9f, (float)position.z}, 0.5f, 1.0f, 0.35f, DARKBROWN);
    
    DrawCube({(float)position.x, (float)position.y - 0.2f, (float)position.z}, 0.6f, 0.6f, 0.6f, head_color);
    DrawCubeWires({(float)position.x, (float)position.y - 0.2f, (float)position.z}, 0.6f, 0.6f, 0.6f, DARKGRAY);
    
    DrawCube({(float)position.x, (float)position.y - 0.2f, (float)position.z + 0.4f}, 0.2f, 0.3f, 0.15f, (Color){210, 170, 130, 255});
    
    DrawCube({(float)position.x - 0.4f, (float)position.y - 0.9f, (float)position.z}, 0.2f, 0.8f, 0.2f, arm_color);
    DrawCube({(float)position.x + 0.4f, (float)position.y - 0.9f, (float)position.z}, 0.2f, 0.8f, 0.2f, arm_color);
    
    DrawCube({(float)position.x - 0.15f, (float)position.y - 1.5f, (float)position.z}, 0.25f, 0.7f, 0.25f, leg_color);
    DrawCube({(float)position.x + 0.15f, (float)position.y - 1.5f, (float)position.z}, 0.25f, 0.7f, 0.25f, leg_color);
}