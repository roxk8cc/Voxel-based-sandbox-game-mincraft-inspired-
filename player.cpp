#include "player.h"
#include <cmath>

Player::Player()
{
    Vector start_position = init_vector(16, 25, 16);
    camera = Camera3d(start_position);
    selected_block = GRASS;
    speed = 0.1f;
    mouse_sensitivity = 0.15f;
    velocity = init_vector();
    on_ground = false;
    flying = false;
}

float Player::get_speed()
{
    return speed;
}

float Player::get_mouse_sensitivity()
{
    return mouse_sensitivity;
}

BlockType Player::get_selected_block()
{ 
    return selected_block; 
}

void Player::set_selected_block(BlockType type)
{ 
    selected_block = type;
}

Camera3D Player::get_camera() 
{
    return camera.to_raylib_camera();
}

Camera3d& Player::get_camera_ref()
{
    return camera;
}

Vector Player::get_velocity()
{
    return velocity;
}

void Player::set_velocity(Vector v)
{
    velocity = v;
}

bool Player::is_on_ground()
{
    return on_ground;
}

void Player::set_on_ground(bool state)
{
    on_ground = state;
}

bool Player::is_flying()
{
    return flying;
}

void Player::switch_flying()
{
    flying = !flying;
    velocity.y = 0;
}