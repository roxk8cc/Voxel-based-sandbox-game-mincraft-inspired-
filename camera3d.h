#ifndef CAMERA_H
#define CAMERA_H

#include "vector3d.h"
#include "raylib.h"
#include <cmath>

/**
 * @class Camera3d a frist-person perspective camera
 */
class Camera3d 
{
    private:
        Vector position;
        Vector forward;
        float yaw; //horizental
        float pitch; //vertical

        static constexpr float DEGREE_TO_RAD = M_PI / 180.0f;
        
        /**
         * Calculates the forward vector based on current yaw and pitch.
         */
        void update_vectors();
        
    public:

        /**
         * A default constructor.
         * Initializes camera at (0, 0, 0) looking toward the negative Z-axis.
         */
        Camera3d();
        
        /**
         * Constructs a camera at a specific starting position.
         * @param pos The initial Vector position for the camera.
         */
        Camera3d(Vector pos);
        
        /**
         * Rotates the camera view by adding offsets to yaw and pitch.
         * @param delta_yaw Change in horizontal angle.
         * @param delta_pitch Change in vertical angle.
         */
        void rotate(float delta_yaw, float delta_pitch);
        
        /**
         * Translates the camera position in a specific direction.
         * @param direction A normalized vector indicating the direction of movement.
         * @param speed A scalar multiplier for the distance moved.
         */
        void move(Vector direction, float speed);
        
        /**
         * Calculates the right vector relative to the camera's current view.
         * @return Vector pointing to the right of the forward direction.
         */
        Vector get_right();
        
        /**
         * Gets the forward direction projected onto the XZ.
         * @return Vector pointing forward but parallel to the ground.
         */
        Vector get_forward_XZ();

        /**
         * Converts this class into a raylib-compatible Camera3D struct.
         * @return Camera3D The struct required by BeginMode3D().
         */
        Camera3D to_raylib_camera();
        
        /**
         * Gets the current world position.
         * @return Vector The camera's current location.
         */
        Vector get_position();
        
        /**
         * Gets the current look-direction.
         * @return Vector The normalized forward vector.
         */
        Vector get_forward();
    
};

#endif