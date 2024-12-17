#pragma once

#include "game_math.cpp"
//#include "texture_manager.cpp"

#include <memory>
#include <vector>
#include <limits>
#include <raylib.h>
#include <raymath.h>
#include <iostream>

#include "render_object.cpp"

//class PlayerObject: public RenderObject{
class PlayerObject: public Object{
    public:
        PlayerObject(Vector2 _pos, Vector2 _vel, float _accel_lerp_constant, Vector2 _size, Vector2 _default_speed): Object(_pos, true, false, Rectangle{}), sprite {nullptr} {
            pos = _pos;
            vel = _vel;
            accel_lerp_constant = _accel_lerp_constant;
            size = _size;
            default_speed = _default_speed;
            max_speed = _default_speed;
            hitbox = Rectangle{.x = _pos.x, .y = _pos.y, .width = _size.x, .height = _size.y};
        }

        /*
        ~PlayerObject(){
            std::cout << "(player " << id << ") Destructor called" << std::endl;
            UnloadTexture(*sprite);
        }
        */

        void set_key_binding(int key_up, int key_down, int key_left, int key_right){
            PLAYER_KEY_UP = key_up;
            PLAYER_KEY_DOWN = key_down;
            PLAYER_KEY_LEFT = key_left;
            PLAYER_KEY_RIGHT = key_right;
        }

        void set_sprite(const char* filename){
            sprite = std::make_shared<Texture2D>(LoadTexture(filename));
            //texture_manager.add(sprite);
            std::cout << "Loaded texture from '" << filename << "'" << std::endl;
        }

        //void update(std::vector<Object*> objects, double deltatime){
        void update(std::vector<std::shared_ptr<Object>> objects, double deltatime){
            // check input
            respond_to_input(deltatime);

            Vector2 future_pos = {
                pos.x + (vel.x * (float)deltatime), 
                pos.y + (vel.y * (float)deltatime)
            };

            Vector2 previous_pos = {pos.x, pos.y};

            // collision
            // vertical collisions
            DirectionUDLR vert_collisions = check_collision_solids(objects, Vector2{pos.x, future_pos.y});
            if(vert_collisions.up || vert_collisions.down){
                vel.y = 0; 
                future_pos.y = pos.y;
            }
            // horizontal collisions
            DirectionUDLR hori_collisions = check_collision_solids(objects, Vector2{future_pos.x, previous_pos.y});
            if(hori_collisions.left || hori_collisions.right){
                vel.x = 0; 
                future_pos.x = pos.x;
            }

            // update position
            pos.x = future_pos.x;
            pos.y = future_pos.y;
            if(hitbox.has_value()){
                hitbox->x = pos.x;
                hitbox->y = pos.y;
            }
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
            auto lerp_velocity = [this](float old_vel, bool pos_key, bool neg_key, float speed, float lerp_constant, double deltatime){
                if(!(pos_key != neg_key)){
                    return lerp_dt_margin(old_vel, 0, deltatime, lerp_constant, VELOCITY_MARGIN);
                }else if(pos_key){
                    return lerp_dt_margin(old_vel, speed, deltatime, lerp_constant, VELOCITY_MARGIN);
                }else if(neg_key){
                    return lerp_dt_margin(old_vel, -speed, deltatime, lerp_constant, VELOCITY_MARGIN);
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
            vel.x = lerp_velocity(vel.x, right_key_down, left_key_down, max_speed.x, accel_lerp_constant, deltatime);
            // up/down movement
            vel.y = lerp_velocity(vel.y, down_key_down, up_key_down, max_speed.y, accel_lerp_constant, deltatime);

            //if(id==0) std::cout << "player (" << id << ") vel(x, y) = (" << vel.x << ", " << vel.y << ")" << std::endl;
        }

        virtual void render(Vector2 pos_offset) override {
            DrawTexture(*sprite, pos.x + pos_offset.x, pos.y + pos_offset.y, WHITE);
        }

        Vector2 vel;
        float accel_lerp_constant; // a constant used to 'slow down' or 'speed up' velocity lerping, expected to be ~0.005
        Vector2 size;
        Vector2 default_speed;
        Vector2 max_speed; // may change when moving diagonally, etc

        int PLAYER_KEY_UP;
        int PLAYER_KEY_DOWN;
        int PLAYER_KEY_LEFT;
        int PLAYER_KEY_RIGHT;

        const float VELOCITY_MARGIN = 5.f;

        std::shared_ptr<Texture2D> sprite;
};
