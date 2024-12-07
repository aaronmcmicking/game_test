#pragma once

#include "game_math.cpp"

#include <raylib.h>
#include <raymath.h>
#include <iostream>

class PlayerObject{
    public:
        PlayerObject(Vector2 _pos, Vector2 _vel, Vector3 _accel_lerp_xyr, Vector2 _size, Vector2 _default_speed){
            pos = _pos;
            vel = _vel;
            accel_lerp_xyr = _accel_lerp_xyr;
            size = _size;
            default_speed = _default_speed;
            max_speed = _default_speed;
            //render_colour = _colour;
            hitbox = Rectangle{.x = _pos.x, .y = _pos.y, .width = _size.x, .height = _size.y};

            id = id_counter++;
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

            //printf("player %d: speed = %g\n", id, sqrtf(vel.x*vel.x + vel.y*vel.y));
        }

        void respond_to_input(double deltatime){
            // input
            bool up_key_down = IsKeyDown(PLAYER_KEY_UP);
            bool down_key_down = IsKeyDown(PLAYER_KEY_DOWN);
            bool left_key_down = IsKeyDown(PLAYER_KEY_LEFT);
            bool right_key_down = IsKeyDown(PLAYER_KEY_RIGHT);

            update_velocity(left_key_down, right_key_down, up_key_down, down_key_down, deltatime);
        }

        void update_velocity(bool left_key_down, bool right_key_down, bool up_key_down, bool down_key_down, double deltatime){
            auto lerp_velocity = [](float old_vel, bool pos_key, bool neg_key, float speed, float lerp_constant, double deltatime){
                if(!(pos_key != neg_key)){
                    return lerp_dt_margin(old_vel, 0, deltatime, lerp_constant);
                }else if(pos_key){
                    return lerp_dt_margin(old_vel, speed, deltatime, lerp_constant);
                }else if(neg_key){
                    return lerp_dt_margin(old_vel, -speed, deltatime, lerp_constant);
                }else{
                    throw std::exception();
                }
            };

            if((left_key_down || right_key_down) && (up_key_down || down_key_down)){
                max_speed = Vector2Multiply(default_speed, {1/1.414f, 1/1.414f}); 
            }else{
                max_speed = default_speed;
            }

            // left/right movement
            vel.x = lerp_velocity(vel.x, right_key_down, left_key_down, max_speed.x, accel_lerp_xyr.z, deltatime);
            // up/down movement
            vel.y = lerp_velocity(vel.y, down_key_down, up_key_down, max_speed.y, accel_lerp_xyr.z, deltatime);
        }

        Vector2 pos;
        Vector2 vel;
        Vector3 accel_lerp_xyr;
        Vector2 size;
        Vector2 default_speed;
        Vector2 max_speed; // may change when moving diagonally, etc
        Rectangle hitbox;

        int PLAYER_KEY_UP;
        int PLAYER_KEY_DOWN;
        int PLAYER_KEY_LEFT;
        int PLAYER_KEY_RIGHT;

        int id;

        Texture2D sprite;

    private:
        inline static int id_counter = 0;
};
