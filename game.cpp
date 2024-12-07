#include <exception>
#include <iostream>

#include <cmath>
#include <algorithm>
#include <chrono>

#include <raylib.h>
#include <rlgl.h>
#include <raymath.h>

#include "print_rl_colour.cpp"

#include "game_math.cpp"
#include "player.cpp"

int main(){
    const int DEFAULT_WINDOW_WIDTH = 1000;
    const int DEFAULT_WINDOW_HEIGHT = 1000;
    const char WINDOW_TITLE[] = "Game Test";
    const Color BACKGROUND_COLOR = Color{30, 30, 30, 255};
    const int TARGET_FPS = 60;

    // window init should occur before loading assets as per RayLib requirements
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, WINDOW_TITLE);

    { // new context for when the window is open so that textures can be destroyed before closing the window

        SetTargetFPS(TARGET_FPS);

        // object init
        PlayerObject player = PlayerObject(Vector2{0,0}, Vector2{0,0}, Vector3{.1f,.1f,.0005f}, Vector2{100,100}, Vector2{350, 350});
        player.set_key_binding(KEY_W, KEY_S, KEY_A, KEY_D);
        player.set_sprite("sprites/steve_face_100_100.png");

        PlayerObject player2 = PlayerObject(Vector2{500,500}, Vector2{0,0}, Vector3{.1f,.1f,.0005f}, Vector2{80, 80}, Vector2{350, 350});
        player2.set_key_binding(KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT);
        player2.set_sprite("sprites/steve_face_100_100.png");

        // deltatime init
        auto previous_time = std::chrono::high_resolution_clock::now();
        while(!WindowShouldClose()){
            // deltatime update
            auto current_time = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> elapsed_time = current_time - previous_time;
            previous_time = std::chrono::high_resolution_clock::now();

            double deltatime = elapsed_time.count() / 1000.l;
            //std::cout << deltatime << "ms" << std::endl;

            player.update(deltatime);
            player2.update(deltatime);

            // draw
            BeginDrawing();
            ClearBackground(BACKGROUND_COLOR);

            DrawTexture(player.sprite, player.pos.x, player.pos.y, WHITE);
            DrawTexture(player2.sprite, player2.pos.x, player2.pos.y, WHITE);

            DrawFPS(10, 10);
            EndDrawing();
        } // while(!WindowShouldClose)

    } // close context for when window is open (destroys textures before CloseWindow called)

    CloseWindow();

    return 0;
}
