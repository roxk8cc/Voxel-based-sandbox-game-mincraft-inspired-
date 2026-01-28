#include "vector3d.h"
#include "player.h"
#include "camera3d.h"
#include "block.h"
#include "world.h"
#include "game.h"
#include "raylib.h"
#include <cmath>

// clang++ main.cpp vector3d.cpp player.cpp camera3d.cpp block.cpp world.cpp game.cpp entity.cpp\
//     -o game5 \
//     -I/opt/homebrew/include \
//     -L/opt/homebrew/lib \
//     -lraylib \
//     -framework OpenGL \
//     -framework Cocoa \
//     -framework IOKit \
//     -framework CoreAudio \
//     -framework CoreVideo \
//     -std=c++17

int main() 
{
    
    Game game;
    game.display_game();

    return 0;
}