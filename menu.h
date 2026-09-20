#ifndef MENU_H
#define MENU_H

#include "raylib.h"
#include "gamestate.h"
#include <vector>
#include <string>
#include <functional>


/**
 * @class Menu game menu manager
 */
class Menu
{
    private:
        /**
         * @class Button A clickable button
         */
        class Button
        {
            private:
                Rectangle bounds;
                std::string text;
                Color normal_color;
                Color hover_color;
                Color text_color;
                bool is_hovered;
                int font_size;

            public:
                /**
                 * Constructor.
                 */
                Button(float x, float y, float width, float height, const std::string& text);
                
                /**
                 * Update button state
                 */
                void update();

                /**
                 * Draw the button
                 */
                void draw();

                /**
                 * Check if the buttion was clicked.
                 */
                bool is_clicked();

                /**
                 * Set buttion posiiton.
                 * @param x, y the coordinate fo button position
                 */
                void set_position(float x, float y);

                /**
                 * Get buttion
                 */
                Rectangle get_bounds();
        };
        
        static const int SCREEN_WIDTH = 1280;
        static const int SCREEN_HEIGHT = 720;
        static const int MAX_SAVE_SLOTS = 5;
        
        GameState current_state;
        GameState previous_state;
        std::vector<SaveSlot> save_slots;
        int selected_slot;
        int highlighted_slot;

        bool delete_confirm = false;
        int delete_slot_candidate = -1;
        
        // Menu buttons
        std::vector<Button> main_menu_buttons;
        std::vector<Button> pause_menu_buttons;
        std::vector<Button> save_load_buttons;
        std::vector<Button> confirm_buttons;
        std::vector<Button> slot_delete_buttons;
        
        // UI Colors
        Color bg_color;
        Color panel_color;
        Color title_color;
        Color text_color;
        
        /**
         * Initialize save slots and check which ones exist
         */
        void initialize_save_slots();

        /**
         * Draw the main menu
         */
        void draw_main_menu();
        
        /**
         * Draw the pause menu (in-game)
         */
        void draw_pause_menu();
        
        /**
         * Draw the save slot selection menu
         */
        void draw_save_menu();
        
        /**
         * Draw the load slot selection menu
         */
        void draw_load_menu();
        
        /**
         * Update main menu input
         */
        void update_main_menu();
        
        /**
         * Update pause menu input
         */
        void update_pause_menu();
        
        /**
         * Update save menu input
         */
        void update_save_menu();
        
        /**
         * Update load menu input
         */
        void update_load_menu();
        
        /**
         * Draw a save slot card
         */
        void draw_save_slot(SaveSlot& slot, float x, float y, bool is_highlighted);

        /**
         * Draw a delete button
         */
        void draw_delete();

        /**
         * Update delete confirmation input logic
         */
        void update_delete();

        /**
         * Delete a specific save slot.
         */
        void delete_save(int slot); 
        
    public:
        /**
         * Constructor.
         */
        Menu();
        
        /**
         * Update save slot information from file system
         */
        void refresh_save_slots();
        
        /**
         * Update the current menu (handle input, etc.)
         * @return The current game state
         */
        GameState update();
        
        /**
         * Draw the current menu screen
         */
        void draw();
        
        /**
         * Set the current game state
         */
        void set_state(GameState state);
        
        /**
         * Get the current game state
         */
        GameState get_state() { return current_state; }
        
        /**
         * Get the currently selected save slot number
         */
        int get_selected_slot() { return selected_slot; }
        
        /**
         * Get the filename for a specific slot
         */
        std::string get_slot_filename(int slot);  
        
        /**
         * clear the input buffer
         */
        void clear_input_buffer();
};

#endif