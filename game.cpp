#include <iostream>

#include <cmath>
#include <algorithm>
#include <chrono>

#include <raylib.h>
#include <rlgl.h>

#include "print_rl_colour.cpp"

/*
 * a: start value
 * b: target value
 * r: value remaining after 1s , 0<=r<=1,
 *  (ie. controls steepness of lerp curve where r=0 is linear and r=1 is stationary)
 * dt: deltatime
 */
float lerp_dt(float a, float b, float r, float dt){
    r = std::ranges::clamp(r, 0.f, 1.f);
    return std::lerp(a, b, 1-pow(r, dt));
    // lerp(a,b,t) = (1-t)a + bt -> frame dependant
}

float lerp_dt_margin(float a, float b, float r, float dt, float margin_of_error = 1e-3){
    int lerped = lerp_dt(a, b, r, dt);
    if(std::abs(lerped - b) < margin_of_error){
        return b;
    }else{
        return lerped;
    }
}

class PlayerObject{
    public:
        PlayerObject(Vector2 _pos, Vector2 _vel, Vector3 _accel_lerp_xyr, Vector2 _size, Vector2 _default_speed, Color _colour){
            pos = _pos;
            vel = _vel;
            accel_lerp_xyr = _accel_lerp_xyr;
            size = _size;
            default_speed = _default_speed;
            //render_colour = _colour;
            hitbox = Rectangle{.x = _pos.x, .y = _pos.y, .width = _size.x, .height = _size.y};
        }

        ~PlayerObject(){
            UnloadTexture(sprite);
        }

        void set_key_binding(int key_up, int key_down, int key_left, int key_right){
            PLAYER_KEY_UP = key_up;
            PLAYER_KEY_DOWN = key_down;
            PLAYER_KEY_LEFT = key_left;
            PLAYER_KEY_RIGHT = key_right;
        }

        void set_sprite(const char* filename){
            sprite = LoadTexture(filename);
            std::cout << "Loaded texture from '" << filename << "'" << std::endl;
        }

        void update(double deltatime){
            // check input
            respond_to_input(deltatime);

            // collision
            if((pos.x <= 0) || (pos.x + size.x >= GetScreenWidth())){ 
                vel.x = 0; 
                pos.x = std::ranges::clamp(pos.x, 1.f, GetScreenWidth()-size.x-1.f);
            }
            if((pos.y <= 0) || (pos.y + size.y >= GetScreenHeight())){ 
                vel.y = 0; 
                pos.y = std::ranges::clamp(pos.y, 1.f, GetScreenHeight()-size.y-1.f);
            }

            // update position
            pos.x += vel.x * deltatime;
            pos.y += vel.y * deltatime;
            hitbox.x = pos.x;
            hitbox.y = pos.y;
        }

        void respond_to_input(double deltatime){
            // input
            bool up_key_down = IsKeyDown(PLAYER_KEY_UP);
            bool down_key_down = IsKeyDown(PLAYER_KEY_DOWN);
            bool left_key_down = IsKeyDown(PLAYER_KEY_LEFT);
            bool right_key_down = IsKeyDown(PLAYER_KEY_RIGHT);

            // up/down movement
            if(!(up_key_down != down_key_down)){ // both or neither
                vel.y = lerp_dt_margin(vel.y, 0, accel_lerp_xyr.z, deltatime);
            }else if(up_key_down){
                vel.y = lerp_dt_margin(vel.y, -default_speed.y, accel_lerp_xyr.z, deltatime);
            }else if(down_key_down){
                vel.y = lerp_dt_margin(vel.y, default_speed.y, accel_lerp_xyr.z, deltatime);
            }

            // left/right movement
            if(!(left_key_down != right_key_down)){
                vel.x = lerp_dt_margin(vel.x, 0, accel_lerp_xyr.z, deltatime);
            }else if(right_key_down){
                vel.x = lerp_dt_margin(vel.x, default_speed.x, accel_lerp_xyr.z, deltatime);
            }else if(left_key_down){
                vel.x = lerp_dt_margin(vel.x, -default_speed.x, accel_lerp_xyr.z, deltatime);
            }
        }

        Vector2 pos;
        Vector2 vel;
        Vector3 accel_lerp_xyr;
        Vector2 size;
        Vector2 default_speed;
        //Color render_colour;
        Rectangle hitbox;

        int PLAYER_KEY_UP;
        int PLAYER_KEY_DOWN;
        int PLAYER_KEY_LEFT;
        int PLAYER_KEY_RIGHT;

        Texture2D sprite;
};


int main(){
    const int DEFAULT_WINDOW_WIDTH = 1000;
    const int DEFAULT_WINDOW_HEIGHT = 1000;
    const char WINDOW_TITLE[] = "Game Test";
    const Color BACKGROUND_COLOR = Color{30, 30, 30, 255};
    const int TARGET_FPS = 60;

    // window init should occur before loading assets as per RayLib requirements
    InitWindow(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, WINDOW_TITLE);

    { // new context for when the window is open

        SetTargetFPS(TARGET_FPS);

        // object init
        PlayerObject player = PlayerObject(Vector2{0,0}, Vector2{0,0}, Vector3{.1f,.1f,.005f}, Vector2{100,100}, Vector2{500, 500}, MAGENTA);
        player.set_key_binding(KEY_W, KEY_S, KEY_A, KEY_D);
        player.set_sprite("sprites/steve_face_100_100.png");

        PlayerObject player2 = PlayerObject(Vector2{500,500}, Vector2{0,0}, Vector3{.1f,.1f,.005f}, Vector2{80, 80}, Vector2{500, 500}, GREEN);
        player2.set_key_binding(KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT);
        player2.set_sprite("sprites/steve_face_100_100.png");

        //std::cout << "player.color = " << rl_col_to_str(player.render_colour) << std::endl;
        //std::cout << "player2.color = " << rl_col_to_str(player2.render_colour) << std::endl;

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

            /*
               DrawRectangleRec(player.hitbox, player.render_colour);
               DrawRectangleRec(player2.hitbox, player2.render_colour);
               */
            DrawTexture(player.sprite, player.pos.x, player.pos.y, WHITE);
            DrawTexture(player2.sprite, player2.pos.x, player2.pos.y, WHITE);

            DrawFPS(10, 10);
            EndDrawing();
        }

    } // close context for when window is open (destroys textures before CloseWindow called)

    CloseWindow();

    return 0;
}
