#include "menu.h"
#include "savemanager.h"
#include <ctime>
#include <sstream>
#include <iomanip>
#include <cstdio>

Menu::Button::Button(float x, float y, float width, float height, const std::string& text)
{
    bounds = {x, y, width, height};
    this->text = text;
    normal_color = (Color){70, 70, 80, 255};
    hover_color = (Color){90, 90, 100, 255};
    text_color = WHITE;
    is_hovered = false;
    font_size = 30;
}

void Menu::Button::update()
{
    Vector2 mouse_position = GetMousePosition();
    is_hovered = CheckCollisionPointRec(mouse_position, bounds);
}

void Menu::Button::draw()
{
    Color current_color;
    if(is_hovered)
    {
        current_color = hover_color;
    }
    else
    {
        current_color = normal_color;
    }
    
    // Draw button background
    DrawRectangleRec(bounds, current_color);
    DrawRectangleLinesEx(bounds, 2, is_hovered ? YELLOW : GRAY);
    
    // Draw text centered
    int text_width = MeasureText(text.c_str(), font_size);
    float text_x = bounds.x + (bounds.width - text_width) / 2;
    float text_y = bounds.y + (bounds.height - font_size) / 2;
    DrawText(text.c_str(), text_x, text_y, font_size, text_color);
}

bool Menu::Button::is_clicked()
{
    return is_hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

void Menu::Button::set_position(float x, float y)
{
    bounds.x = x;
    bounds.y = y;
}

Rectangle Menu::Button::get_bounds()
{
    return bounds;
}

Menu::Menu() 
{
    current_state = MAIN_MENU;
    previous_state = MAIN_MENU;
    selected_slot = -1;
    highlighted_slot = 0;
    
    bg_color = (Color){20, 30, 40, 255};
    panel_color = (Color){40, 50, 60, 200};
    title_color = YELLOW;
    text_color = WHITE;
    initialize_save_slots();
    
    // Create main menu buttons
    float botten_width = 300;
    float botten_height = 60;
    float center_x = (SCREEN_WIDTH - botten_width) / 2;
    float start_y = 300;
    float spacing = 80;
    
    main_menu_buttons.push_back(Button(center_x, start_y, botten_width, botten_height, "New Game"));
    main_menu_buttons.push_back(Button(center_x, start_y + spacing, botten_width, botten_height, "Load Game"));
    main_menu_buttons.push_back(Button(center_x, start_y + spacing * 2, botten_width, botten_height, "Exit"));
    
    // Create pause menu buttons
    pause_menu_buttons.push_back(Button(center_x, start_y, botten_width, botten_height, "Resume"));
    pause_menu_buttons.push_back(Button(center_x, start_y + spacing, botten_width, botten_height, "Save Game"));
    pause_menu_buttons.push_back(Button(center_x, start_y + spacing * 2, botten_width, botten_height, "Load Game"));
    pause_menu_buttons.push_back(Button(center_x, start_y + spacing * 3, botten_width, botten_height, "Main Menu"));
    
    // Create save/load menu buttons
    save_load_buttons.push_back(Button(50, SCREEN_HEIGHT - 100, 200, 50, "Back"));
    
    // Create confirm/cancel buttons for delete dialog
    float confirm_botten_width = 150;
    float confirm_botten_height = 50;
    float confirm_center_x = SCREEN_WIDTH / 2;
    float confirm_y = SCREEN_HEIGHT / 2 + 50;
    
    confirm_buttons.push_back(Button(confirm_center_x - confirm_botten_width - 10, confirm_y, confirm_botten_width, confirm_botten_height, "Yes"));
    confirm_buttons.push_back(Button(confirm_center_x + 10, confirm_y, confirm_botten_width, confirm_botten_height, "No"));
    
    // Create delete buttons for each slot
    float slot_y = 200;
    float slot_spacing = 100;
    for (int i = 0; i < MAX_SAVE_SLOTS; i++)
    {
        float delete_botten_x = SCREEN_WIDTH - 230;
        float delete_botten_y = slot_y + i * slot_spacing + 15;
        slot_delete_buttons.push_back(Button(delete_botten_x, delete_botten_y, 100, 50, "Delete"));
    }
}

void Menu::initialize_save_slots()
{
    save_slots.clear();
    for (int i = 1; i <= MAX_SAVE_SLOTS; i++)
    {
        save_slots.push_back(SaveSlot(i));
    }
    refresh_save_slots();
}

void Menu::refresh_save_slots()
{
    for (auto& slot : save_slots)
    {
        slot.exists = Savemanager::save_exists(slot.filename);
        
        if (slot.exists)
        {
            slot.file_size = Savemanager::get_save_size(slot.filename);

            slot.last_saved = "Saved";
        } 
        else
        {
            slot.last_saved = "Empty";
            slot.file_size = 0;
        }
    }
}

GameState Menu::update()
{
    if(delete_confirm)
    {
        update_delete();
        return current_state;
    }
    switch (current_state)
    {
        case MAIN_MENU:
            update_main_menu();
            break;
        case PAUSE_MENU:
            update_pause_menu();
            break;
        case SAVE_MENU:
            update_save_menu();
            break;
        case LOAD_MENU:
            update_load_menu();
            break;
        default:
            break;
    }
    return current_state;
}

void Menu::draw()
{
    switch (current_state)
    {
        case MAIN_MENU:
            draw_main_menu();
            break;
        case PAUSE_MENU:
            draw_pause_menu();
            break;
        case SAVE_MENU:
            draw_save_menu();
            break;
        case LOAD_MENU:
            draw_load_menu();
            break;
        default:
            break;
    }
    if(delete_confirm)
    {
        draw_delete();
    }
}

void Menu::set_state(GameState state)
{
    clear_input_buffer();
    previous_state = current_state;
    current_state = state;
    
    if (state == SAVE_MENU || state == LOAD_MENU)
    {
        refresh_save_slots();
        highlighted_slot = 0;
        selected_slot = -1;
    }
}

std::string Menu::get_slot_filename(int slot)
{
    if (slot >= 0 && slot < save_slots.size())
    {
        return save_slots[slot].filename;
    }
    return "save_slot_1.dat";
}

void Menu::clear_input_buffer()
{
    while(GetKeyPressed() != 0)
    {}
}

void Menu::draw_main_menu()
{
    ClearBackground(bg_color);
    // Draw title
    const char* title = "BLOCKVERSE";
    int title_size = 80;
    int title_width = MeasureText(title, title_size);
    DrawText(title, (SCREEN_WIDTH - title_width) / 2, 100, title_size, title_color);
    // Draw subtitle
    const char* subtitle = "Adventure Awaits";
    int subtitle_size = 30;
    int subtitle_width = MeasureText(subtitle, subtitle_size);
    DrawText(subtitle, (SCREEN_WIDTH - subtitle_width) / 2, 200, subtitle_size, GRAY);
    
    // Update and draw buttons
    for (Button& button : main_menu_buttons)
    {
        button.update();
        button.draw();
    }
    
    DrawText("Press control to open the menu", 10, SCREEN_HEIGHT - 30, 20, GRAY);
}

void Menu::draw_pause_menu()
{
    // Draw semi-transparent overlay
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){0, 0, 0, 150});
    
    // Draw panel
    Rectangle panel = {SCREEN_WIDTH / 2 - 250, 100, 500, 600};
    DrawRectangleRec(panel, panel_color);
    DrawRectangleLinesEx(panel, 3, title_color);
    
    // Draw title
    const char* title = "PAUSED";
    int title_size = 60;
    int title_width = MeasureText(title, title_size);
    DrawText(title, (SCREEN_WIDTH - title_width) / 2, 150, title_size, title_color);
    
    // Update and draw buttons
    for (Button& button : pause_menu_buttons)
    {
        button.update();
        button.draw();
    }
}

void Menu::draw_save_menu()
{
    ClearBackground(bg_color);
    
    // Draw title
    const char* title = "SAVE GAME";
    int title_size = 60;
    int title_width = MeasureText(title, title_size);
    DrawText(title, (SCREEN_WIDTH - title_width) / 2, 50, title_size, title_color);
    
    // Draw instruction
    const char* instruction = "Select a slot to save your game";
    int instr_width = MeasureText(instruction, 25);
    DrawText(instruction, (SCREEN_WIDTH - instr_width) / 2, 130, 25, GRAY);
    
    // Draw save slots
    float slot_y = 200;
    float slot_spacing = 100;
    
    for (int i = 0; i < save_slots.size(); i++)
    {
        bool is_highlighted = (i == highlighted_slot);
        draw_save_slot(save_slots[i], 100, slot_y + i * slot_spacing, is_highlighted);
        
        // Draw delete button only if slot has save data
        if (save_slots[i].exists)
        {
            slot_delete_buttons[i].update();
            slot_delete_buttons[i].draw();
        }
    }
    for (Button& button : save_load_buttons)
    {
        button.update();
        button.draw();
    }
}

void Menu::draw_load_menu()
{
    ClearBackground(bg_color);
    
    // Draw title
    const char* title = "LOAD GAME";
    int title_size = 60;
    int title_width = MeasureText(title, title_size);
    DrawText(title, (SCREEN_WIDTH - title_width) / 2, 50, title_size, title_color);
    
    // Draw instruction
    const char* instruction = "Select a slot to load your game";
    int instr_width = MeasureText(instruction, 25);
    DrawText(instruction, (SCREEN_WIDTH - instr_width) / 2, 130, 25, GRAY);
    
    // Draw save slots
    float slot_y = 200;
    float slot_spacing = 100;
    
    for (int i = 0; i < save_slots.size(); i++)
    {
        bool is_highlighted = (i == highlighted_slot);
        draw_save_slot(save_slots[i], 100, slot_y + i * slot_spacing, is_highlighted);
        
        if (save_slots[i].exists)
        {
            slot_delete_buttons[i].update();
            slot_delete_buttons[i].draw();
        }
    }
    
    // Update and draw back button
    for (Button& button : save_load_buttons) {
        button.update();
        button.draw();
    }
}

void Menu::draw_save_slot(SaveSlot& slot, float x, float y, bool is_highlighted)
{
    Rectangle bounds = {x, y, SCREEN_WIDTH - 200, 80};
    
    // Draw slot background
    Color bg = is_highlighted ? (Color){80, 90, 100, 255} : (Color){50, 60, 70, 255};
    DrawRectangleRec(bounds, bg);
    DrawRectangleLinesEx(bounds, 2, is_highlighted ? YELLOW : GRAY);
    
    // Draw slot number
    std::string slot_text = "Slot " + std::to_string(slot.slot_number);
    DrawText(slot_text.c_str(), x + 20, y + 15, 30, WHITE);
    
    // Draw status
    if (slot.exists)
    {
        DrawText(slot.last_saved.c_str(), x + 20, y + 50, 20, LIGHTGRAY);
        
        std::string size_text = std::to_string(slot.file_size / 1024) + " KB";
        DrawText(size_text.c_str(), x + 300, y + 50, 20, LIGHTGRAY);
    }
    else
    {
        DrawText("Empty Slot", x + 20, y + 50, 20, GRAY);
    }
    
    if (is_highlighted)
    {
        const char* hint = "Press ENTER to select";
        int hint_width = MeasureText(hint, 20);
        DrawText(hint, x + 500, y + 30, 20, YELLOW);
    }
}

void Menu::update_main_menu()
{
    for (int i = 0; i < main_menu_buttons.size(); i++)
    {
        if (main_menu_buttons[i].is_clicked())
        {
            if (i == 0)
            {
                set_state(PLAYING);
            } 
            else if (i == 1)
            {   
                set_state(LOAD_MENU);
            }
            else if (i == 2)
            {
                CloseWindow();
            }
        }
    }
}

void Menu::update_pause_menu()
{
    for (int i = 0; i < pause_menu_buttons.size(); i++)
    {
        if (pause_menu_buttons[i].is_clicked())
        {
            if (i == 0)
            {
                set_state(PLAYING);
            } 
            else if (i == 1)
            {
                set_state(SAVE_MENU);
            }
            else if (i == 2)
            {
                set_state(LOAD_MENU);
            } 
            else if(i == 3)
            {
                set_state(MAIN_MENU);
            }
        }
    }
}

void Menu::update_save_menu()
{
    if (IsKeyPressed(KEY_UP))
    {
        highlighted_slot = (highlighted_slot - 1 + MAX_SAVE_SLOTS) % MAX_SAVE_SLOTS;
    }
    if (IsKeyPressed(KEY_DOWN))
    {
        highlighted_slot = (highlighted_slot + 1) % MAX_SAVE_SLOTS;
    }
    for (int i = 0; i < save_slots.size(); i++)
    {
        if (save_slots[i].exists && slot_delete_buttons[i].is_clicked())
        {
            delete_slot_candidate = i;
            delete_confirm = true;
            return;
        }
    }
    
    Vector2 mouse_pos = GetMousePosition();
    float slot_y = 200;
    float slot_spacing = 100;
    
    for (int i = 0; i < save_slots.size(); i++)
    {
        Rectangle bounds = {100, slot_y + i * slot_spacing, SCREEN_WIDTH - 200, 80};
        
        if (CheckCollisionPointRec(mouse_pos, bounds))
        {
            highlighted_slot = i;
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                Rectangle delete_bounds = slot_delete_buttons[i].get_bounds();
                if (!CheckCollisionPointRec(mouse_pos, delete_bounds))
                {
                    selected_slot = i;
                    clear_input_buffer();
                }
            }
        }
    }
    if (save_load_buttons[0].is_clicked())
    {
       set_state(previous_state);
    }
}

void Menu::update_load_menu()
{
    if (IsKeyPressed(KEY_UP))
    {
        highlighted_slot = (highlighted_slot - 1 + MAX_SAVE_SLOTS) % MAX_SAVE_SLOTS;
    }
    if (IsKeyPressed(KEY_DOWN))
    {
        highlighted_slot = (highlighted_slot + 1) % MAX_SAVE_SLOTS;
    }
    
    for (int i = 0; i < save_slots.size(); i++)
    {
        if (save_slots[i].exists && slot_delete_buttons[i].is_clicked())
        {
            delete_slot_candidate = i;
            delete_confirm = true;
            return;
        }
    }
    
    Vector2 mouse_pos = GetMousePosition();
    float slot_y = 200;
    float slot_spacing = 100;
    
    for (int i = 0; i < save_slots.size(); i++)
    {
        Rectangle bounds = {100, slot_y + i * slot_spacing, SCREEN_WIDTH - 200, 80};
        if (CheckCollisionPointRec(mouse_pos, bounds))
        {
            highlighted_slot = i;
            
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && save_slots[i].exists)
            {
                Rectangle delete_bounds = slot_delete_buttons[i].get_bounds();
                if (!CheckCollisionPointRec(mouse_pos, delete_bounds))
                {
                    selected_slot = i;
                    clear_input_buffer();
                }
            }
        }
    }

    if (save_load_buttons[0].is_clicked())
    {
        set_state(previous_state);
    }
}

void Menu::draw_delete()
{
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){0, 0, 0, 180});
    Rectangle dialog = {SCREEN_WIDTH / 2 - 250, SCREEN_HEIGHT / 2 - 100, 500, 200};
    DrawRectangleRec(dialog, panel_color);
    DrawRectangleRec(dialog, panel_color);

    const char* title = "DELETE SAVE?";
    int title_size = 40;
    int title_width = MeasureText(title, title_size);
    DrawText(title, (SCREEN_WIDTH - title_width) / 2, SCREEN_HEIGHT / 2 - 80, title_size, RED);

    std::string msg = "Delete Slot " + std::to_string(delete_slot_candidate + 1) + "?";
    const char* message = msg.c_str();
    int msg_width = MeasureText(message, 25);
    DrawText(message, (SCREEN_WIDTH - msg_width) / 2, SCREEN_HEIGHT / 2 - 30, 25, WHITE);
    
    const char* warning = "This action cannot be undone!";
    int warn_width = MeasureText(warning, 20);
    DrawText(warning, (SCREEN_WIDTH - warn_width) / 2, SCREEN_HEIGHT / 2, 20, GRAY);
    
    for (Button& button : confirm_buttons)
    {
        button.update();
        button.draw();
    }
}

void Menu::update_delete()
{
    if(confirm_buttons[0].is_clicked())
    {
        delete_save(delete_slot_candidate);
        delete_confirm = false;
        delete_slot_candidate = -1;
        refresh_save_slots();
    }
    if(confirm_buttons[1].is_clicked())
    {
        delete_confirm = false;
        delete_slot_candidate = -1;
    }
}

void Menu::delete_save(int slot)
{
    if(slot < 0 || slot >= save_slots.size())
    {
        return;
    }
    std::string filepath = "./" + save_slots[slot].filename;
    remove(filepath.c_str());
}