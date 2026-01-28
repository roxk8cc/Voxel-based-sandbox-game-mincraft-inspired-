#include "camera3d.h"
#include "raylib.h"

void Camera3d::update_vectors() 
{
    if(pitch > 89.0f) 
    {
        pitch = 89.0f;
    }
    if(pitch < -89.0f)
    {
        pitch = -89.0f;
    }
    
    forward.x = cosf(pitch * DEGREE_TO_RAD) * cosf(yaw * DEGREE_TO_RAD);
    forward.y = sinf(pitch * DEGREE_TO_RAD);
    forward.z = cosf(pitch * DEGREE_TO_RAD) * sinf(yaw * DEGREE_TO_RAD);
    forward = normalize_vec(forward);
}

Camera3d::Camera3d()
{
    position = {0, 0, 0}; 
    yaw = -90.0f;
    pitch = 0.0f;
}

Camera3d::Camera3d(Vector pos) 
{
    position = pos;
    yaw = -90.0f;
    pitch = 0.0f;
    update_vectors();
}

void Camera3d::rotate(float delta_yaw, float delta_pitch) 
{
    yaw += delta_yaw;
    pitch += delta_pitch;
    update_vectors();
}

void Camera3d::move(Vector direction, float speed) 
{
    Vector movement = multiply_vec(direction, speed);
    position = add_vec(position, movement);
}

Vector Camera3d::get_right() 
{
    Vector worldUp = {0, 1, 0};
    Vector right = cross(forward, worldUp);
    return normalize_vec(right);
}

Vector Camera3d::get_forward_XZ() 
{
    Vector fwd;
    fwd.x = forward.x;
    fwd.y = 0;
    fwd.z = forward.z;
    return normalize_vec(fwd);
}

Camera3D Camera3d::to_raylib_camera() 
{
    Camera3D cam = { 0 };

    cam.position.x = (float)position.x;
    cam.position.y = (float)position.y;
    cam.position.z = (float)position.z;

    Vector target_vector = add_vec(position, forward);
    
    cam.target.x = (float)target_vector.x;
    cam.target.y = (float)target_vector.y;
    cam.target.z = (float)target_vector.z;

    cam.up.x = 0.0f;
    cam.up.y = 1.0f;
    cam.up.z = 0.0f;

    cam.fovy = 70.0f;                       
    cam.projection = CAMERA_PERSPECTIVE;

    return cam;
}

Vector Camera3d::get_position()
{ 
    return position; 
}

Vector Camera3d::get_forward() 
{
    return forward;
}