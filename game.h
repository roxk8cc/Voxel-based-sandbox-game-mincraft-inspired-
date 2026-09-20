#ifndef GAME_H
#define GAME_H

#include "world.h"
#include <map>
#include "player.h"
#include "entity.h"
#include "savemanager.h"
#include "menu.h"
#include "gamestate.h"
#include "fluidsimulator.h"
#include <vector>

/**
 * @class Game
 * Manages the main game logic.
 */
class Game 
{
    private:
        static const int SCREEN_WIDTH = 1280;
        static const int SCREEN_HEIGHT = 720;
        static const int TARGET_FPS = 60;
        static const int GRAVITY = 20;
        //day and night
        float day_night_timer;
        float cycle_duration;

        //menu game states
        GameState game_state;
        Menu menu;
        bool game_initialized;
        bool is_show_notification;
        float notification_timer;
        std::string notification_text;
        Color notification_color;

        World world;
        Player player;
        FluidSimulator* fluid_simulator;
        std::vector<BlockType> hotbar_items;
        std::vector<Entity*> entities;
        std::map<BlockType, Texture2D> block_textures;

        /**
         * Handle the movement of player.
         */
        void handle_camera();
        
        /**
         * Handle the block selection, which types of block to place.
         */
        void handle_selection();

        /**
         * Handles block removement and placement.
         */
        void handle_block_interaction();

        /**
         * Integrate all input handling together.
         */
        void handle_input();

        /** 
         * Calculate the progress of the day/night cycle. 
         */
        float get_day_progress();

        /**
         * Display the block that player choose in the hotbar.
         */
        void display_hotbar();

        /**
         * Add the gravity to player movement.
         * @param deltatime The time elapsed since the last frame, or not it runs all the times. 
         */
        void handle_gravity(float deltatime);

        /**
         * check whether the collision happens between player and blocks
         * @param position player's location
         * @return true if there is a collision with solid block
         */
        bool check_collision(Vector position);

        /**
         * Checks if a position is out of the world map bounds.
         * * @param position player's position
         * @return true if the position is within the map
         */
        bool is_within_boundary(Vector position);

        /**
         * Draw the crosshair.
         */
        void draw_crosshair();

        /**
         * Checks if there is enough space to spawn an entity.
         * @param x The X-coordinate.
         * @param z The Z-coordinate.
         * @param entity_height The height of the entity.
         * @param position stores the calculated 3D Vector if space is found.
         * @return true if it is empty.
         */
        bool is_space_empty(int x, int z, float entity_height, Vector& position);
        
        /**
         * Initializes and populates the world with entities
         */
        void generate_entities();
        
        /**
         * Updates the logic for all active entities in the world.
         * * @param deltatime Time elapsed since the last frame in seconds.
         */
        void update_entities(float deltatime);
        
        /**
         * Render all the entities.
         */
        void render_entities();
        
        /**
         * Safely deletes all entity objects and clears the entity vector.
         */
        void cleanup_entities();

        /**
         * Manages the passage of time for the day and night cycle.
         * * @param deltatime Time elapsed since the last frame.
         */
        void update_day_night(float deltatime);

        /**
         * Get the sky color based on the current time of day.
         * @return Color representing the current sky state.
         */
        Color get_sky_color();

        /**
         * Calculates a darkness overlay value based on the day/night cycle.
         * @return A float value representing the current intensity of darkness (0.0 to 1.0).
         */
        float get_darkness();

        /** 
         * Handle attacks or interactions between entities. 
         */
        void handle_entity_attack();

        /** 
         * Initialize the game world for a new session.
         */
        void init_game_world();

        /** 
         * Handle menu input and transitions between game states
         */
        void handle_menu();

        /** 
         * Save the current game state to a save slot.
         */
        void save();

        /** 
         * Load the game state from a save slot. 
         */
        void load();

        /**
         * Show a notification on screen.
         * @param message Text to display
         * @param color Notification color
         * @param duration Time to display
         */
        void show_notification(const std::string& message,Color color, float duration = 3.0f);

        /** 
         * Update notification timers and display messages on screen.
         */
        void display_notifications(float deltatime);

        /**
         * Load all block and entity textures into memory
         */
        void load_texture();
        
        /**
         * Unload all textures from memory to free resources. 
         * */
        void unload_texture();

    public:

        /**
         * Default construtor.
         */
        Game();

        /**
         * Render the game world and UI.
         */
        void display_game();

};

#endif
