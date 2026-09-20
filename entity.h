#ifndef ENTITY_H
#define ENTITY_H

#include "vector3d.h"
#include "world.h"
#include "raylib.h"

/**
 * enum All states for a vllager
 */
enum villager_state 
{
    WANDER, 
    FOLLOW_PLAYER, 
    FLEE, 
};

/**
 * @class Base class for all entity.
 */
class Entity 
{
    protected:
        Vector position;
        float health;
        bool is_alive;
        float width;
        float height;
        Vector gravity;
        float speed;
        //Fleeing logic
        bool is_fleeing;
        Vector flee_direction;
        float flee_timer;
        
        Vector last_attacker_position;
        bool is_hit;
        float hit_timer;
        static constexpr float HIT_FLASH_DURATION = 0.3f;

        /**
         * Start flee behavior
         * @param danger_position Position to flee from
         */
        void start_fleeing(Vector danger_position);

        /**
        * Applies gravity and checks if the entity is standing on a solid block.
        * @param position Reference to current position.
        * @param velocity Reference to current gravity.
        * @param height Entity height for foot-level detection.
        * @return true if the entity is touching the ground.
        */
        bool on_the_ground(Vector& position, Vector& gravity, float height, World& world, float deltatime);

        /**
         * Handle the whole movement logic for entity.
         */
        void handle_movement(Vector move, World &world);

        /**
         * Check if entity is currently fleeing
         * @return true if fleeing
         */
        bool is_entity_fleeing();

        /**
        * Get color of entity when hit
        */
        Color get_hit_color();

        /**
        * Update hit flash timer
        * @param deltatime Time elapsed
        * */
        void update_hit_flash(float deltatime);
        
    public:
        /**
         * A constructor
         * @param position position of entity.
         * @param health hp.
         */
        Entity(Vector position, float health);
        
        /**
         * Destructor。
         */
        virtual ~Entity();

        /**
        * Pure virtual function: Updates entity logic every frame.
        */
        virtual void update(float deltatime, World& world) = 0;

        /**
        * Pure virtual function: Handles the drawing of the entity.
        */
        virtual void render() = 0;
        
        /**
        * Returns the current 3D position of the entity.
        */
        Vector get_position();

        /**
         * Get the height of entity.
         */
        float get_height();
        
        /**
        * Checks if the entity is still alive.
        * @return true if it is still alive.
        */
        bool is_entity_alive();

        /**
        * Reduces the entity's health points.
        * @param damage The amount of health to subtract.
        */
        void take_damage(float damage, Vector attacker_postion);

        /**
        * Checks if the entity intersects with solid blocks.
        * @param world Reference to the game world.
        * @return true if the entity is colliding with a solid block.
        */
        bool check_collision(World& world);

        /**
        * Attempts to push the entity out of solid blocks if it becomes trapped.
        * @param position The position to be corrected.
        * @param width Entity's width.
        * @param height Entity's height.
        */
        void fix_entity_stuck(World& world);

        /**
         * 
         * @param ray_origin
         * @param ray_direction
         * @param max_distance
         * @param hit_distance
         * @return true
         */
        bool ray_intersects(Vector ray_origin, Vector ray_direction, float max_distance, float& hit_distance);

};

/**
 * @class Sheep
 * A sheep entity
 */
class Sheep : public Entity 
{
    private:
        static constexpr float FLEE_DURATION = 5.0f;
        static constexpr float FLEE_SPEED = 3.5f;
        static constexpr float FLEE_DISTANCE = 10.0f;
        static constexpr float NORMAL_SPEED = 1.8f;

        float wander_timer;
        Vector wander_direction;

        /**
        * Handles flee logic.
        * @param deltatime Frame delta time (seconds)
        * @param world Reference to game world for environment queries
        */
        void handle_flee(float deltatime, World& wolrd);

        /**
        * Handles random wandering behavior.
        * @param deltatime Frame delta time
        * @param world Reference to world for collision and terrain checks
        */
        void handle_wander(float deltatime, World& world);
        
    public:
        Sheep(Vector position);
        void update(float deltatime, World& world) override;
        void render() override;
};

/**
 * @class Villager
 * Villager entity.
 */
class Villager : public Entity 
{
    private:
        static constexpr float FOLLOW_DISTANCE_MIN = 3.0f;
        static constexpr float FOLLOW_DISTANCE_MAX = 15.0f;
        static constexpr float WANDER_SPEED = 1.0f;
        static constexpr float FOLLOW_SPEED = 1.5f;
        static constexpr float FLEE_SPEED = 2.5f;

        Vector player_position;
        villager_state current_state;
        villager_state previous_state;
        float state_timer;

        float wander_timer;
        Vector wander_direction;

        /**
        * Evaluates environment and updates state transitions.
        */
        void update_state(float deltatime);

        /**
        * @brief Switches villager to a new state.
        * 
        * Resets state timers and preserves previous state.
        * 
        * @param new_state Target FSM state
        */
        void change_state(villager_state new_state);

        /**
        * Executes behavior logic for current state.
        * @param deltatime Frame delta time
        * @param world World reference
        */
        void execute_current_state(float deltatime, World &wolrd);

        /**
         * Handles wander behavious.
         */
        void handle_wander_state(float deltatime, World& world);

        /**
         * Handles follow player behavious.
         */
        void handle_follow_state(float deltatime, World& world);

        /**
         * Handles fleeing behavious after being attacked.
         */
        void handle_flee_state(float deltatime, World& world);
        
    public:
        Villager(Vector pos);
        void update(float deltatime, World& world) override;
        void render() override;

        /**
        * Updates target position
        * Used for follow and flee direction calculation.
        */
        void set_target(Vector target);
};

#endif