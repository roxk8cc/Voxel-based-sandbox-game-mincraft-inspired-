#include "game.h"
#include "world.h"
#include "player.h"
#include "raylib.h"
#include "vector3d.h"
#include "savemanager.h"

void Game::handle_camera()
{
    Camera3d& camera = player.get_camera_ref();
    Vector2 mouse_delta = GetMouseDelta();
    Vector current_position = camera.get_position();
    if(mouse_delta.x != 0 || mouse_delta.y != 0) 
    {
        camera.rotate(mouse_delta.x * player.get_mouse_sensitivity(), - mouse_delta.y * player.get_mouse_sensitivity());
    }
    
    Vector direction = init_vector();
    if(IsKeyDown(KEY_UP))
    {
        direction = add_vec(direction, camera.get_forward_XZ());
    }
    if(IsKeyDown(KEY_DOWN))
    {
        direction = subtract_vec(direction, camera.get_forward_XZ());
    }
    if(IsKeyDown(KEY_LEFT))
    {
        direction = subtract_vec(direction, camera.get_right());
    }
    if(IsKeyDown(KEY_RIGHT))
    {
        direction = add_vec(direction, camera.get_right());
    }

    if (length_vec(direction) > 0) 
    {
        direction= normalize_vec(direction);
        Vector movement = multiply_vec(direction, player.get_speed());
        
        Vector next_position = current_position;
        next_position.x += movement.x;
        if (!check_collision(next_position) && is_within_boundary(next_position)) 
        {
            camera.move({direction.x, 0, 0}, player.get_speed());
        }

        current_position = camera.get_position();
        Vector next_z = current_position;
        next_z.z += movement.z;
        if (!check_collision(next_z) && is_within_boundary(next_z)) 
        {
            camera.move({0, 0, direction.z}, player.get_speed());
        }
    }

    if(IsKeyDown(KEY_SPACE))
    {
        if(player.is_flying())
        {
            if (camera.get_position().y < world.get_height())
            {
                Vector velocity = player.get_velocity();
                velocity.y = Player::JUMP_SPEED;
                player.set_velocity(velocity);
            }
        }
        else if(player.is_on_ground())
        {
            Vector velocity = player.get_velocity();
            velocity.y = Player::JUMP_SPEED;
            player.set_velocity(velocity);
            player.set_on_ground(false);
        }
    }
    if(IsKeyDown(KEY_LEFT_SHIFT) && player.is_flying())
    {
        Vector velocity = player.get_velocity();
        velocity.y = -Player::JUMP_SPEED;
        player.set_velocity(velocity);
    }
    else if(player.is_flying() && !IsKeyDown(KEY_SPACE))
    {
        Vector velocity = player.get_velocity();
        velocity.y *= 0.9;
        player.set_velocity(velocity);
    }
}

void Game::handle_selection()
{
    if(IsKeyPressed(KEY_F))
    {
        player.switch_flying();
    }

    if(IsKeyPressed(KEY_ONE))
    {
        player.set_selected_block(GRASS);
    }
    if(IsKeyPressed(KEY_TWO))
    {
        player.set_selected_block(STONE);
    }
    if(IsKeyPressed(KEY_THREE))
    {
        player.set_selected_block(WOOD);
    }
    if(IsKeyPressed(KEY_FOUR))
    {
        player.set_selected_block(SAND);
    }
    if(IsKeyPressed(KEY_FIVE))
    {
        player.set_selected_block(WATER);
    }
    if(IsKeyPressed(KEY_SIX))
    {
        player.set_selected_block(DIRT);
    }
    if(IsKeyPressed(KEY_SEVEN))
    {
        player.set_selected_block(BEDROCK);
    }

}

void Game::handle_block_interaction()
{
    if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        Camera3d& camera = player.get_camera_ref();
        Vector ray_origin = camera.get_position();
        Vector ray_direction = camera.get_forward();
        
        float max_distance = 10.0f;
        
        for(size_t i = 0; i < entities.size(); i++)
        {
            if(entities[i]->is_entity_alive())
            {
                float hit_distance;
                if(entities[i]->ray_intersects(ray_origin, ray_direction, max_distance, hit_distance))
                {
                    Vector player_position = camera.get_position();
                    entities[i]->take_damage(10.0f, player_position);
                    return;
                }
            }
        }
    }
    
    Camera3d& camera = player.get_camera_ref();
    Vector ray_position = camera.get_position();
    Vector ray_direction = camera.get_forward();
    
    float max_distance = 10.0f;
    float step = 0.1f;

    
    for(float distance = 0; distance < max_distance; distance += step) 
    {
        Vector offset = multiply_vec(ray_direction, distance);
        Vector current_position = add_vec(ray_position, offset);
        
        int x = (int)round(current_position.x);
        int y = (int)round(current_position.y);
        int z = (int)round(current_position.z);
        
        Block block = world.get_block(x, y, z);
        
        if (block.is_solid()) 
        {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) 
            {
                if(y != 0)
                {
                    if(block.get_type() == WATER)
                    {
                        fluid_simulator->remove_water(x,y,z);
                    }
                    world.set_block(x, y, z, AIR);
                    fluid_simulator->on_block_changed(x,y,z);
                }
            }
            else if(IsKeyPressed(KEY_ENTER)) 
            {
                Vector offset = multiply_vec(ray_direction, distance - step);
                Vector prev_positision = add_vec(ray_position, offset);
                
                int px = (int)round(prev_positision.x);
                int py = (int)round(prev_positision.y);
                int pz = (int)round(prev_positision.z);
                BlockType selected = player.get_selected_block();
                if(selected == WATER)
                {
                    fluid_simulator->add_water(px, py, pz, false);
                }
                world.set_block(px, py, pz, player.get_selected_block());
            }
            break;
        }
    }
}

void Game::handle_input() 
{
    handle_camera();
    handle_block_interaction();
    handle_selection();
}

void Game::display_hotbar()
{
    const int slot_size = 50;
    const int slot_spacing = 5;
    const int hotbar_size = (int)hotbar_items.size();
    int hotbar_width = hotbar_size * (slot_size + slot_spacing) - slot_spacing;
    int hotbar_x = (SCREEN_WIDTH - hotbar_width) / 2;
    int hotbar_y = SCREEN_HEIGHT - 80;
    
    BlockType current_selected = player.get_selected_block();
    
    for(int i = 0; i < hotbar_size; i++)
    {
        int x = hotbar_x + i * (slot_size + slot_spacing);
        int y = hotbar_y;
        
        BlockType block_type = hotbar_items[i];
        bool is_selected = (block_type == current_selected);
        
        Color bg_color;
        if(is_selected)
        {
            bg_color = DARKGRAY;
        }
        else
        {
            bg_color = LIGHTGRAY;
        }
        DrawRectangle(x, y, slot_size, slot_size, bg_color);
        
        if(is_selected)
        {
            DrawRectangleLinesEx((Rectangle){(float)x, (float)y, (float)slot_size, (float)slot_size}, 3, WHITE);
        }
        else
        {
            DrawRectangleLines(x, y, slot_size, slot_size, BLACK);
        }
        
        Block temp_block(block_type);
        int padding = 8;
        DrawRectangle(x + padding, y + padding, slot_size - padding * 2, slot_size - padding * 2, temp_block.get_color());
        DrawText(TextFormat("%d", i + 1), x + 25, y + 30, 15, WHITE);
    }
}

void Game::handle_gravity(float deltatime)
{
    if(!player.is_flying())
    {
        Vector velocity = player.get_velocity();
        velocity.y -= GRAVITY * deltatime;
        if(velocity.y < -30)
        {
            velocity.y = -30;
        }
        player.set_velocity(velocity);
    }
    Camera3d& camera = player.get_camera_ref();
    Vector position = camera.get_position();
    Vector velocity = player.get_velocity();
    position.y += velocity.y * deltatime;

    if(!player.is_flying())
    {
        int x = (int)floor(position.x);
        int y = (int)floor(position.y - Player::PLAYER_HEIGHT); 
        int z = (int)floor(position.z);
        
        player.set_on_ground(false);
        
        Block block_below = world.get_block(x, y, z);
        if(block_below.is_solid())
        {
            position.y = y + Player::PLAYER_HEIGHT + 1;
            velocity.y = 0;
            player.set_velocity(velocity);
            player.set_on_ground(true);
        }
    }
    camera.move(init_vector(), 0);
    Vector current_position = camera.get_position();
    Vector difference = subtract_vec(position, current_position);
    camera.move(difference, 1);
}

bool Game::check_collision(Vector position)
{
    float epsilon = 0.05;
    float half_width = Player::PLAYER_WIDTH / 2;
    
    int min_x = (int)floor(position.x - half_width);
    int max_x = (int)floor(position.x + half_width);
    int min_y = (int)floor(position.y - Player::PLAYER_HEIGHT + epsilon);
    int max_y = (int)floor(position.y - epsilon);
    int min_z = (int)floor(position.z - half_width);
    int max_z = (int)floor(position.z + half_width);

    for(int x = min_x; x <= max_x; x++)
    {
        for(int y = min_y; y <= max_y; y++)
        {
            for(int z = min_z; z <= max_z;z++)
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

bool Game::is_within_boundary(Vector position)
{
    float margin = 0.5;
    if (position.x < margin || position.x >= world.get_size() - margin)
    {
        return false;
    }
    if (position.y < margin || position.y >= world.get_height() - margin)
    {
        return false;
    }
    if (position.z < margin || position.z >= world.get_size() - margin)
    {
        return false;
    }
    return true;
}

bool Game::is_space_empty(int x, int z, float entity_height, Vector& position)
{
    int y = world.get_height(); 
    while(y > 10)
    {
        Block block = world.get_block(x, y, z);
        if (block.get_type() == AIR || block.get_type() == WATER)
        {
            y--;
        }
        else
        {
            break;
        }
    }

    if(!world.get_block(x, y, z).is_solid())
    {
        return false;
    }

    int blocks_above = (int)ceil(entity_height); 
    for(int check_y = y + 1; check_y <= y + blocks_above; check_y++)
    {
        if(world.get_block(x, check_y, z).is_solid())
        {
            return false;
        }
    }

    position = init_vector((float)x + 0.5f, (float)(y + 1) + entity_height, (float)z + 0.5f);
    return true;
}

void Game::generate_entities()
{   
    int sheep_amount = 0;
    for(int i = 0; i < 20 && sheep_amount < 5; i++) 
    {
        int x = 20 + rand() % 40;
        int z = 20 + rand() % 40;
        Vector position;
        
        if(is_space_empty(x, z, 1.0f, position)) 
        {
            Sheep* sheep = new Sheep(position);
            if(sheep->check_collision(world))
            {
                sheep->fix_entity_stuck(world);
            }
            
            if(!sheep->check_collision(world)) 
            {
                entities.push_back(sheep);
                sheep_amount++;
            } 
            else 
            {
                delete sheep;
            }
        }
    }

    int villager_amount = 0;
    for(int i = 0; i < 10 && villager_amount < 3; i++) 
    {
        int x = 20 + rand() % 40;
        int z = 20 + rand() % 40;

        Vector pos;
        if(is_space_empty(x, z, 1.8f, pos)) 
        {
            Villager* villager = new Villager(pos);
            if(villager->check_collision(world))
            {
                villager->fix_entity_stuck(world);
            }
            
            if(!villager->check_collision(world)) 
            {
                entities.push_back(villager);
                villager_amount++;
            }
            else
            {
                delete villager;
            }
        }
    }
}

void Game::update_entities(float deltatime)
{   
    Vector player_pos = player.get_camera_ref().get_position();
    
    for(size_t i = 0; i < entities.size(); i++)
    {
        if(entities[i]->is_entity_alive())
        {
            Villager* villager = dynamic_cast<Villager*>(entities[i]);
            if(villager != nullptr)
            {
                villager->set_target(player_pos);
            }
            entities[i]->update(deltatime, world);
        }
    }
}

void Game::handle_entity_attack()
{
    if(!IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        return;
    }
    
    Camera3d& camera = player.get_camera_ref();
    Vector ray_origin = camera.get_position();
    Vector ray_direction = camera.get_forward();
    
    float max_distance = 10.0f;
    float closest_distance = max_distance;
    Entity* closest_entity = nullptr;
    
    for(size_t i = 0; i < entities.size(); i++)
    {
        if(entities[i]->is_entity_alive())
        {
            float hit_distance;
            if(entities[i]->ray_intersects(ray_origin, ray_direction, max_distance, hit_distance))
            {
                if(hit_distance < closest_distance)
                {
                    closest_distance = hit_distance;
                    closest_entity = entities[i];
                }
            }
        }
    }
    if(closest_entity != nullptr)
    {
        Vector player_position = camera.get_position();
        closest_entity->take_damage(10.0f, player_position);
    }
}

void Game::render_entities()
{
    Vector player_position = player.get_camera_ref().get_position();
    float render_distance = 40;
    
    for(size_t i = 0; i < entities.size(); i++)
    {
        if(entities[i]->is_entity_alive())
        {
            Vector entity_pos = entities[i]->get_position();
            Vector diff = subtract_vec(entity_pos, player_position);
            float distance = length_vec(diff);
            
            if(distance < render_distance)
            {
                entities[i]->render();
            }
        }
    }
}

void Game::cleanup_entities()
{
    for(int i = 0; i < entities.size(); i++)
    {
        delete entities[i];
    }
    entities.clear();
}

void Game::update_day_night(float deltatime)
{
    day_night_timer += deltatime;
    
    if(day_night_timer >= cycle_duration)
    {
        day_night_timer -= cycle_duration;
    }
}

float Game::get_day_progress()
{
    float t = day_night_timer / cycle_duration;
    return 0.5f * (1.0f + std::sin(t * 2.0f * 3.1415926f + 3.1415926f/2.0f));
}

Color Game::get_sky_color()
{
    Color day_sky   = {135, 206, 235, 255};
    Color night_sky = {10, 10, 40, 255};
    float time = get_day_progress();
    unsigned char red = (unsigned char)(night_sky.r * (1 - time) + day_sky.r * time);
    unsigned char green = (unsigned char)(night_sky.g * (1 - time) + day_sky.g * time);
    unsigned char blue = (unsigned char)(night_sky.b * (1 - time) + day_sky.b * time);

    return {red, green, blue, 255};
}

float Game::get_darkness()
{
    const float MAX_DARKNESS = 150.0f;
    float t = get_day_progress(); 
    return (1.0f - t) * MAX_DARKNESS;
}

Game::Game()
{
    day_night_timer = 0;
    cycle_duration = 600;
    hotbar_items = {GRASS, STONE, WOOD, SAND, WATER, DIRT, BEDROCK};
    game_state = MAIN_MENU;
    game_initialized = false;
    is_show_notification = false;
    notification_timer = 0.0f;
    notification_text = "";
    notification_color = YELLOW;
    fluid_simulator = new FluidSimulator(&world);
}

void Game::draw_crosshair()
{
    int crosshair_x = SCREEN_WIDTH / 2;
    int crosshair_y = SCREEN_HEIGHT / 2;
    DrawLine(crosshair_x - 10, crosshair_y, crosshair_x + 10, crosshair_y, WHITE);
    DrawLine(crosshair_x, crosshair_y - 10, crosshair_x, crosshair_y + 10, WHITE);
    DrawCircle(crosshair_x, crosshair_y, 2, WHITE);
}

void Game::handle_menu()
{
    static GameState last_state = MAIN_MENU;
    static int last_selected_slot = -1;

    int current_selected_slot = menu.get_selected_slot();

    if (game_state == SAVE_MENU && current_selected_slot >= 0 && current_selected_slot != last_selected_slot)
    {
        save();
        menu.clear_input_buffer();
        last_selected_slot = current_selected_slot;
    }
    else if (game_state == LOAD_MENU && current_selected_slot >= 0 && current_selected_slot != last_selected_slot)
    {
        menu.clear_input_buffer();
        load();
        last_selected_slot = current_selected_slot;
        menu.set_state(PLAYING);
        game_state = PLAYING;
        DisableCursor();
    }
    if (game_state != last_state)
    {
        if (game_state == SAVE_MENU || game_state == LOAD_MENU)
        {
            last_selected_slot = -1;
        }
    }

    // Handle cursor and world initialization on state change
    if (game_state != last_state)
    {
        if (game_state == PLAYING)
        {
            DisableCursor();
            if (last_state == MAIN_MENU)
            {
                init_game_world();
            }
        }
        else
        {
            EnableCursor();
        }
    }
    last_state = game_state;
}

void Game::save()
{
    int slot = menu.get_selected_slot();
    if(slot < 0)
    {
        show_notification("No slot selected!", RED, 3.0f);
        return;
    }
    std::string filename = menu.get_slot_filename(slot);

    bool success = Savemanager::save_world(world, filename);
    if (success)
    {
        show_notification("Game saved to Slot " + std::to_string(slot + 1) + "!", GREEN, 3.0f);
        menu.refresh_save_slots();
    }
    else
    {
        show_notification("Failed to save game!", RED, 3.0f);
    }
}

void Game::load()
{
    int slot = menu.get_selected_slot();
    if (slot < 0)
    {
        show_notification("No slot selected!", RED,3.0f);
        return;
    }
    
    std::string filename = menu.get_slot_filename(slot);
    
    if (!Savemanager::save_exists(filename))
    {
        show_notification("Save file not found!", RED, 3.0f);
        return;
    }
    
    bool success = Savemanager::load_world(world, filename);
    if (success)
    {
        show_notification("Game loaded from Slot " + std::to_string(slot + 1) + "!", GREEN, 3.0f);
        game_initialized = true;
        menu.refresh_save_slots();
    }
    else
    {
        show_notification("Failed to load game!", RED, 3.0f);
    }
}

void Game::show_notification(const std::string& message, Color color, float duration)
{
    notification_text = message;
    notification_timer = duration;
    notification_color = color;
    is_show_notification = true;
}

void Game::display_notifications(float deltatime)
{
    if (is_show_notification)
    {
        if (deltatime > 0)
        {
            notification_timer -= deltatime;
        }
        if (notification_timer <= 0)
        {
            is_show_notification = false;
        }
        else
        {
            float alpha = notification_timer < 1.0f ? notification_timer : 1.0f;
            int text_width = MeasureText(notification_text.c_str(), 30);
            int x = (SCREEN_WIDTH - text_width) / 2;
            int y = 50;
            DrawText(notification_text.c_str(), x + 2, y + 2, 30, Fade(BLACK, alpha));
            DrawText(notification_text.c_str(), x, y, 30, Fade(notification_color, alpha));
        }
    }
}

void Game::init_game_world()
{
    if(!game_initialized)
    {
        generate_entities();
        game_initialized = true;
    }
}

void Game::load_texture()
{
    const char* base_path = "textures/";
    block_textures[GRASS] = LoadTexture((std::string(base_path) + "grass.png").c_str());
    block_textures[STONE] = LoadTexture((std::string(base_path) + "stone.png").c_str());
    block_textures[LEAVES] = LoadTexture((std::string(base_path) + "leave.png").c_str());
    block_textures[WOOD] = LoadTexture((std::string(base_path) + "wood.png").c_str());
    block_textures[SAND] = LoadTexture((std::string(base_path) + "sand.png").c_str());
    block_textures[DIRT] = LoadTexture((std::string(base_path) + "dirt.png").c_str());
    block_textures[BEDROCK] = LoadTexture((std::string(base_path) + "bedrock.png").c_str());
}

void Game::unload_texture()
{
    for (auto& pair : block_textures) 
    {
        UnloadTexture(pair.second);
    }
    block_textures.clear();
}

void Game::display_game()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "WEN RONGXIN 36522163 HD project");
    SetTargetFPS(60);
    game_state = MAIN_MENU;

   load_texture();

    while(!WindowShouldClose()) 
    {

        float deltatime = GetFrameTime();
        game_state = menu.get_state();
        handle_menu();

        BeginDrawing();

        if(game_state == PLAYING)
        {
            if(IsKeyPressed(KEY_LEFT_CONTROL))
            {
                menu.set_state(PAUSE_MENU);
                game_state = PAUSE_MENU;
            }
            else
            {
                update_day_night(deltatime);
                handle_input();

                if(fluid_simulator != nullptr)
                {
                    fluid_simulator->update(deltatime);
                }
                ClearBackground(get_sky_color());
                
                BeginMode3D(player.get_camera());
                
                Vector pos = player.get_camera_ref().get_position();
                world.render(pos, block_textures);
                render_entities();
                EndMode3D();

                float darkness = get_darkness();
                if(darkness > 0)
                {
                    DrawRectangle(0,0,SCREEN_WIDTH, SCREEN_HEIGHT, (Color){0,0,0,(unsigned char)darkness});
                }

                draw_crosshair();
                
                handle_gravity(deltatime);
                update_entities(deltatime);
                display_hotbar();
                display_notifications(deltatime);
            }
        }
        else
        {
            menu.update();
            menu.draw();
        }
        EndDrawing();
    }
    cleanup_entities(); 
    world.free_world();
    if(fluid_simulator != nullptr)
    {
        delete fluid_simulator;
        fluid_simulator = nullptr;
    }
    unload_texture();
    CloseWindow();
}