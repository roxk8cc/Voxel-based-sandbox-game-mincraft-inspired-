#ifndef PLAYER_H
#define PLAYER_H

#include "world.h"
#include "camera3d.h"

class Player 
{
    private:

        Camera3d camera;
        BlockType selected_block;
        float speed;
        float mouse_sensitivity;
        Vector velocity;
        bool flying;
        bool on_ground;
        
    public:

        static constexpr float PLAYER_WIDTH = 0.5;
        static constexpr float PLAYER_HEIGHT = 1.8;
        static constexpr int JUMP_SPEED = 7;

        /**
         * A default constructor.
         * Initializes the player's position, speed and sensitivity.
         */
        Player();

        /**
         * Gets the speed of the player.
         * @return the speed of the player.
         */
        float get_speed();

        /**
         * Gets the mouse_sensitivity of the player.
         * @return the mouse_sensitivity of the player.
         */
        float get_mouse_sensitivity();

        /**
         * Gets the type of block the player currently selecte.
         * @return BlockType The type of the seleted block.
         */
        BlockType get_selected_block();

        /**
         * Sets the type of block the player currently selecte.
         * @return BlockType The type of the seleted block.
         */
        void set_selected_block(BlockType type);

        /**
         * Returns the player's view.
         * @return Camera3D in Raylib camera structure.
         */
        Camera3D get_camera();

        /**
         * Returns a reference to Camera3d object, allowing direct modification.
         */
        Camera3d& get_camera_ref();

        /**
         * Gets the current velocity vector of the player.
         * @return Vector representing the player's movement speed and direction.
         */
        Vector get_velocity();

        /**
         * Updates the player's velocity vector.
         * @param v The new velocity vector to apply.
         */
        void set_velocity(Vector v);

        /**
         * Checks if the player is currently touching the ground.
         * @return bool True if the player is supported by a solid block.
         */
        bool is_on_ground();

        /**
         * Sets the player's grounded state.
         * @param state True if grounded, false if not.
         */
        void set_on_ground(bool state);

        /**
         * Checks if the player is currently in flying state.
         * @return bool True if flying mode is active.
         */
        bool is_flying();

        /**
         * Siwtch the flying state.
         */
        void switch_flying();

    };

#endif