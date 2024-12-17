#include <exception>
#include <iostream>

#include <cmath>
#include <algorithm>
#include <chrono>

#include <memory>
#include <raylib.h>
#include <rlgl.h>
#include <raymath.h>
#include <stdexcept>

#include "print_rl_colour.cpp"

#include "game_math.cpp"
#include "static_object.cpp"
#include "render_object.cpp"
#include "player.cpp"
#include "follower.cpp"
#include "camera.cpp"

double get_deltatime(){
    static std::chrono::high_resolution_clock::time_point previous_time = std::chrono::high_resolution_clock::now();

    auto current_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed_time = current_time - previous_time;
    previous_time = std::chrono::high_resolution_clock::now();
    double deltatime = elapsed_time.count() / 1000.l;

    return deltatime;
}

std::vector<std::shared_ptr<Object>> load_world(Texture* bg){
        std::vector<std::shared_ptr<Object>> objects {};

        Image simple_background_img = LoadImage("backgrounds/basic_path.png");
        Texture simple_background = LoadTextureFromImage(simple_background_img);
        UnloadImage(simple_background_img);

        StaticObject background_bounds_top = StaticObject({Vector2{0, -100}, false, true, (Rectangle){0, -100, (float)simple_background.width, 100}});
        StaticObject background_bounds_bottom = StaticObject({Vector2{0, (float)simple_background.height}, false, true, (Rectangle){0, (float)simple_background.height, (float)simple_background.width, 100}});
        StaticObject background_bounds_left = StaticObject({Vector2{-100, 0}, false, true, (Rectangle){-100, 0, 100, (float)simple_background.height}});
        StaticObject background_bounds_right = StaticObject({Vector2{(float)simple_background.width, 0}, false, true, (Rectangle){(float)simple_background.width, 0, 100, (float)simple_background.height}});
        StaticObject fence = StaticObject({Vector2{0,1300}, false, true, Rectangle{0, 1300, 2657, 200}});

        objects.push_back(std::make_shared<StaticObject>(background_bounds_top));
        objects.push_back(std::make_shared<StaticObject>(background_bounds_bottom));
        objects.push_back(std::make_shared<StaticObject>(background_bounds_left));
        objects.push_back(std::make_shared<StaticObject>(background_bounds_right));
        objects.push_back(std::make_shared<StaticObject>(fence));

        *bg = simple_background;

        return objects;
}

template <typename T>
T& get_object_by_id(std::vector<std::shared_ptr<Object>>& objects, int target_id) {
    for (auto& obj: objects) {
        if (obj->id == target_id) {
            // attempt to dynamic_cast to the desired subclass (T)
            if (auto* derived = dynamic_cast<T*>(obj.get())) {
                return *derived;  // return a reference to the derived type
            } else {
                throw std::runtime_error("Object with the given ID is not of the requested type");
            }
        }
    }
    throw std::runtime_error("Object with the given ID not found");
}

int main(){
    const int DEFAULT_WINDOW_WIDTH = 1000;
    const int DEFAULT_WINDOW_HEIGHT = 1000;
    const char WINDOW_TITLE[] = "Game Test";
    const Color BACKGROUND_COLOR = Color{30, 30, 30, 255};
    const int TARGET_FPS = 60;

    const double MAX_DELTATIME = (1.f / TARGET_FPS) * 1.2f; // 20% over the expected dt

    // window init should occur before loading assets as per RayLib requirements
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, WINDOW_TITLE);
    SetTargetFPS(TARGET_FPS);

    { // new context for when the window is open so that textures can be destroyed before closing the window

        // object init
        std::vector<std::shared_ptr<Object>> objects {};
        Texture simple_background {};
        objects = load_world(&simple_background);

        PlayerObject _player = PlayerObject(Vector2{250, 250}, Vector2{0,0}, .0005f, Vector2{100,100}, Vector2{350, 350});
        int player_id = _player.id;
        _player.set_key_binding(KEY_W, KEY_S, KEY_A, KEY_D);
        _player.set_sprite("sprites/steve_face_100_100.png");

        PlayerObject _player2 = PlayerObject(Vector2{500,500}, Vector2{0,0}, .0005f, Vector2{80, 80}, Vector2{350, 350});
        int player2_id = _player2.id;
        _player2.set_key_binding(KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT);
        _player2.set_sprite("sprites/steve_face_100_100.png");

        Follower _follower = Follower(Vector2{50, 50}, .0003f, Vector2{100, 100}, Vector2{300, 300});
        int follower_id = _follower.id;
        _follower.set_sprite("sprites/steve_face_100_100.png");

        objects.push_back(std::make_shared<PlayerObject>(_player));
        objects.push_back(std::make_shared<PlayerObject>(_player2));
        objects.push_back(std::make_shared<Follower>(_follower));

        PlayerObject& player = get_object_by_id<PlayerObject>(objects, player_id);
        PlayerObject& player2 = get_object_by_id<PlayerObject>(objects, player2_id);
        Follower& follower = get_object_by_id<Follower>(objects, follower_id);
    
        /*
        std::cout << "All object IDs in objects list are: ";
        for(const auto& obj: objects){ std::cout << obj->id << ", "; }
        std::cout << std::endl;
        */

        GameCamera camera = {{0, 0}, {(float)GetScreenWidth(), (float)GetScreenHeight()}};

        // deltatime init
        while(!WindowShouldClose()){
            // deltatime update
            double deltatime = get_deltatime();
            //std::cout << deltatime*1000.f << "ms" << std::endl; 
            if(deltatime > MAX_DELTATIME){
                //std::cout << "skipping frame with long deltatime (" << deltatime*1000.f << "ms)" << std::endl; 
                //continue;
                std::cout << "adjusting frame with long deltatime (" << deltatime*1000.f << "ms)" << std::endl; 
                deltatime = MAX_DELTATIME;
            }

            if(IsKeyDown(KEY_R)){
                follower.recall();
            }else if(IsKeyDown(KEY_T)){
                follower.release();
            }

            player.update(objects, deltatime);
            player2.update(objects, deltatime);
            follower.set_target_entity_pos(player.pos);
            follower.update(objects, Vector2Length(player.vel) > 100.f, deltatime);

            if(IsWindowResized()){
                camera.size = {(float)GetScreenWidth(), (float)GetScreenHeight()};
            }
            camera.pos = {(player.pos.x + (player.size.x)/2.f) - camera.size.x/2.f, (player.pos.y + (player.size.y)/2.f) - camera.size.y/2.f};

            // draw
            BeginDrawing();

            // background
            ClearBackground(BACKGROUND_COLOR);
            DrawTexture(simple_background, 0 - camera.pos.x, 0 - camera.pos.y, WHITE);

             // draw one of the backgrounds bounding boxes
             /*
            DrawRectangle(background_bounds_right.hitbox.value().x - camera.pos.x, background_bounds_right.hitbox.value().y - camera.pos.y, background_bounds_right.hitbox.value().width, background_bounds_right.hitbox.value().height, RED);
            DrawRectangle(background_bounds_top.hitbox.value().x - camera.pos.x, background_bounds_top.hitbox.value().y - camera.pos.y, background_bounds_top.hitbox.value().width, background_bounds_top.hitbox.value().height, RED);
            DrawRectangle(background_bounds_left.hitbox.value().x - camera.pos.x, background_bounds_left.hitbox.value().y - camera.pos.y, background_bounds_left.hitbox.value().width, background_bounds_left.hitbox.value().height, RED);
            DrawRectangle(background_bounds_bottom.hitbox.value().x - camera.pos.x, background_bounds_bottom.hitbox.value().y - camera.pos.y, background_bounds_bottom.hitbox.value().width, background_bounds_bottom.hitbox.value().height, RED);
            */
            //DrawRectangle(fence.hitbox.value().x - camera.pos.x, fence.hitbox.value().y - camera.pos.y, fence.hitbox.value().width, fence.hitbox.value().height, RED);

            for(const auto& object: objects){
                if(object->renderable){
                    object->render(Vector2Multiply({-1, -1}, camera.pos));
                }
            }
            //DrawRectangle(player.hitbox.value().x - camera.pos.x, player.hitbox.value().y - camera.pos.y, player.hitbox.value().width, player.hitbox.value().height, BLUE);

            DrawFPS(10, 10);
            EndDrawing();
        } // while(!WindowShouldClose)

    } // close context for when window is open (destroys textures before CloseWindow called)

    CloseWindow();


    return 0;
}
