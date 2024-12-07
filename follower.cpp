#pragma once

#include <exception>
#include <raylib.h>
#include <iostream>
#include <algorithm>
#include "game_math.cpp"
#include "player.cpp"
//#include "texture_manager.cpp"

class Follower{
    public:
        Follower(float _accel_lerp_constant, Vector2 _size, Vector2 _default_speed, int _target_id, std::vector<PlayerObject>& _players, Vector2 _target_pos = {500, 500}): players {_players}, sprite {nullptr} {
            target_pos = _target_pos;
            pos = {50, 50};
            vel = {0, 0};
            accel_lerp_constant = _accel_lerp_constant;
            size = _size;
            default_speed = _default_speed;
            max_speed = _default_speed;
            //render_colour = _colour;
            hitbox = Rectangle{.x = pos.x, .y = pos.y, .width = _size.x, .height = _size.y};

            id = id_counter++;
            target_id = _target_id;
            //players = _players; 
        }

        /*
        ~Follower(){
            UnloadTexture(*sprite);
        }
        */

        void set_sprite(const char* filename){
            sprite = std::make_shared<Texture2D>(LoadTexture(filename));
            //texture_manager.add(sprite);
            std::cout << "(Follower " << id << ") Loaded texture from '" << filename << "'" << std::endl;
        }

        void update(double deltatime){
            // check input
            //respond_to_input(deltatime);
            update_velocity(deltatime);

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
        std::vector<PlayerObject>& players;

        //Texture2D sprite;
        std::shared_ptr<Texture2D> sprite;

    private:
        inline static int id_counter = 0;
};
