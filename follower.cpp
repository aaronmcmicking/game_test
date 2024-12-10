#pragma once

#include <exception>
#include <raylib.h>
#include <iostream>
#include <algorithm>
#include <utility>
#include "game_math.cpp"
#include "player.cpp"
#include "render_object.cpp"

//#include "texture_manager.cpp"

class Follower: public RenderObject{
    public:
        Follower(float _accel_lerp_constant, Vector2 _size, Vector2 _default_speed, int _target_id, Vector2 _target_pos = {500, 500}): sprite {nullptr} {
            target_pos = _target_pos;
            pos = {50, 50};
            vel = {0, 0};
            accel_lerp_constant = _accel_lerp_constant;
            size = _size;
            default_speed = _default_speed;
            max_speed = _default_speed;
            hitbox = Rectangle{.x = pos.x, .y = pos.y, .width = _size.x, .height = _size.y};

            id = id_counter++;
            target_id = _target_id;
        }

        void set_sprite(const char* filename){
            sprite = std::make_shared<Texture2D>(LoadTexture(filename));
            //texture_manager.add(sprite);
            std::cout << "(Follower " << id << ") Loaded texture from '" << filename << "'" << std::endl;
        }

        std::pair<bool, bool> collide(Vector2 future_pos){
            std::pair<bool, bool> colliding {};
            if((future_pos.x <= 0) || (future_pos.x + size.x >= GetScreenWidth())){ 
                colliding.first = true;
            }
            if((future_pos.y <= 0) || (future_pos.y + size.y >= GetScreenHeight())){ 
                colliding.second = true;
            }
            return colliding;
        }

        void update(double deltatime){
            // check input
            update_velocity(deltatime);

            Vector2 future_pos = {
                pos.x + (vel.x * (float)deltatime), 
                pos.y + (vel.y * (float)deltatime)
            };

            // collision
            std::pair<bool, bool> colliding = collide(future_pos);
            if(colliding.first){ 
                vel.x = 0; 
                future_pos.x = std::ranges::clamp(future_pos.x, 0.f, GetScreenWidth()-size.x);
            }
            if(colliding.second){ 
                vel.y = 0; 
                future_pos.y = std::ranges::clamp(future_pos.y, 0.f, GetScreenHeight()-size.y);
            }

            // update position
            pos.x = future_pos.x;
            pos.y = future_pos.y;
            hitbox.x = pos.x;
            hitbox.y = pos.y;

            //printf("player %d: speed = %g\n", id, sqrtf(vel.x*vel.x + vel.y*vel.y));
        }

        void update_velocity(double deltatime){
            /*
            auto predicate = [this](PlayerObject p){ return p.id == target_id; };
            auto target = std::find_if(players.begin(), players.end(), predicate);
            //float target_x = target->pos.x;
            //float target_y = target->pos.y;
            
            if(target == players.end()){
                throw std::exception();
            }else{
                std::cout << "target is not the end of the list" << std::endl;
            }
            */

            //std::cout << "(follower " << id << ") target has pos (x, y) = (" << target->pos.x << ", " << target->pos.y << ")" << std::endl;
            //bool above_target = (pos.y <= target->pos.y);
            //bool left_of_target = (pos.x <= target->pos.x);
            bool above_target = (pos.y <= target_pos.y);
            bool left_of_target = (pos.x <= target_pos.x);

            auto lerp_velocity = [](float old_vel, bool lesser_than_target, float speed, float lerp_constant, double deltatime){
                if(lesser_than_target){
                    return lerp_dt_margin(old_vel, speed, deltatime, lerp_constant, 20.f);
                }else if(!lesser_than_target){
                    return lerp_dt_margin(old_vel, -speed, deltatime, lerp_constant, 20.f);
                }else{
                    throw std::exception();
                }
            };

            // left/right movement
            vel.x = lerp_velocity(vel.x, left_of_target, max_speed.x, accel_lerp_constant, deltatime);
            // up/down movement
            vel.y = lerp_velocity(vel.y, above_target, max_speed.y, accel_lerp_constant, deltatime);
        }

        void render() override {
            DrawTexture(*sprite, pos.x, pos.y, WHITE);
        };

        Vector2 pos;
        Vector2 vel;
        float accel_lerp_constant; // a constant used to 'slow down' or 'speed up' velocity lerping, expected to be ~0.005
        Vector2 size;
        Vector2 default_speed;
        Vector2 max_speed; // may change when moving diagonally, etc
        Rectangle hitbox;

        int id;
        Vector2 target_pos = {500, 500};
        int target_id;
        //std::vector<PlayerObject>& players;

        //Texture2D sprite;
        std::shared_ptr<Texture2D> sprite;

    private:
        inline static int id_counter = 0;
};
