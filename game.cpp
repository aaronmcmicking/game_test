#include <exception>
#include <iostream>

#include <cmath>
#include <algorithm>
#include <chrono>

#include <raylib.h>
#include <rlgl.h>
#include <raymath.h>

#include "print_rl_colour.cpp"

#include "texture_manager.cpp"
#include "game_math.cpp"
#include "render_object.cpp"
#include "player.cpp"
#include "follower.cpp"

double get_deltatime(){
    static std::chrono::high_resolution_clock::time_point previous_time = std::chrono::high_resolution_clock::now();

    auto current_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed_time = current_time - previous_time;
    previous_time = std::chrono::high_resolution_clock::now();
    double deltatime = elapsed_time.count() / 1000.l;

    return deltatime;
}

int main(){
    const int DEFAULT_WINDOW_WIDTH = 1000;
    const int DEFAULT_WINDOW_HEIGHT = 1000;
    const char WINDOW_TITLE[] = "Game Test";
    const Color BACKGROUND_COLOR = Color{30, 30, 30, 255};
    const int TARGET_FPS = 60;

    // window init should occur before loading assets as per RayLib requirements
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, WINDOW_TITLE);
    SetTargetFPS(TARGET_FPS);

    { // new context for when the window is open so that textures can be destroyed before closing the window


        // object init
        PlayerObject player = PlayerObject(Vector2{0,0}, Vector2{0,0}, .0005f, Vector2{100,100}, Vector2{350, 350});
        player.set_key_binding(KEY_W, KEY_S, KEY_A, KEY_D);
        player.set_sprite("sprites/steve_face_100_100.png");

        PlayerObject player2 = PlayerObject(Vector2{500,500}, Vector2{0,0}, .0005f, Vector2{80, 80}, Vector2{350, 350});
        player2.set_key_binding(KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT);
        player2.set_sprite("sprites/steve_face_100_100.png");

        Follower follower = Follower(.0003f, Vector2{100, 100}, Vector2{200, 200}, 1);
        follower.set_sprite("sprites/steve_face_100_100.png");

        std::vector<RenderObject*> render_objects {};
        render_objects.push_back(&player);
        render_objects.push_back(&player2);
        render_objects.push_back(&follower);

        // deltatime init
        while(!WindowShouldClose()){
            // deltatime update
            double deltatime = get_deltatime();
            //std::cout << deltatime << "ms" << std::endl;

            player.update(deltatime);
            player2.update(deltatime);
            follower.target_pos = player.pos;
            follower.update(deltatime);

            // draw
            BeginDrawing();
            ClearBackground(BACKGROUND_COLOR);

            for(const auto& object: render_objects){
                object->render();
            }

            DrawFPS(10, 10);
            EndDrawing();
        } // while(!WindowShouldClose)

    } // close context for when window is open (destroys textures before CloseWindow called)

    //texture_manager.unload();
    CloseWindow();


    return 0;
}
