#pragma once

#include <exception>
#include <raylib.h>
#include <iostream>
#include <algorithm>
#include <raymath.h>
#include <utility>
#include "game_math.cpp"
#include "player.cpp"
#include "render_object.cpp"

//#include "texture_manager.cpp"

typedef enum FOLLOWER_STATE{
    IDLE = 1,
    FOLLOWING,
    WANDERING,
}FOLLOWER_STATE;

class Follower: public RenderObject{
    public:
        Follower(Vector2 _pos, float _accel_lerp_constant, Vector2 _size, Vector2 _default_speed, Vector2 _target_pos = {500, 500}): RenderObject(_pos, false, Rectangle{}), sprite {nullptr} {
            target_pos = _target_pos;
            pos = _pos;
            vel = {0, 0};
            accel_lerp_constant = _accel_lerp_constant;
            size = _size;
            default_speed = _default_speed;
            max_speed = _default_speed;
            hitbox = Rectangle{.x = pos.x, .y = pos.y, .width = _size.x, .height = _size.y};

            state = FOLLOWING;
        }

        void set_sprite(const char* filename){
            sprite = std::make_shared<Texture2D>(LoadTexture(filename));
            //texture_manager.add(sprite);
            std::cout << "(Follower " << id << ") Loaded texture from '" << filename << "'" << std::endl;
        }

        void recall(){
            state = FOLLOWING;
        }

        void release(){
            state = WANDERING;
            target_pos = Vector2Random(0, 4000);
        }

        void update(std::vector<Object*> objects, double deltatime){
            static Vector2 idle_pos = pos;
            /*
            if(Vector2Distance(target_pos, pos) > 300){
                state = FOLLOWING;
            }else{
                if(state == FOLLOWING){
                    idle_pos = pos;
                }
                state = IDLE;
                target_pos = idle_pos;
            }
            */
            //std::cout << "follower (" << id << ") target_pos is (x,y) = (" << target_pos.x << ", " << target_pos.y << ")" << std::endl;

            float dist_to_target = Vector2Distance(pos, target_pos);
            switch (state){
                case IDLE:
                    if(dist_to_target > 300){
                        state = FOLLOWING;
                    }else{
                        update_velocity(deltatime, {0, 0});
                    }
                    break;
                case FOLLOWING:
                    if(dist_to_target <= 300){
                        state = IDLE;
                    }else{
                        update_velocity(deltatime, max_speed);  
                    }
                    break;
                case WANDERING:
                    if(dist_to_target < 100){
                        //state = IDLE;
                        target_pos = Vector2Random(0, 4000);
                    }else{
                        update_velocity(deltatime, max_speed);  
                    }
                    break;
                default:
                    throw std::exception(); // unhandled state
            }


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
            }else{
                std::cout << "follower (" << id << ") has no hitbox" << std::endl;
            }

            //printf("player %d: speed = %g\n", id, sqrtf(vel.x*vel.x + vel.y*vel.y));
        }

        void update_velocity(double deltatime, Vector2 target_speed){
            bool above_target = (pos.y <= target_pos.y);
            bool left_of_target = (pos.x <= target_pos.x);

            auto lerp_velocity = [](float old_vel, bool lesser_than_target, float speed, float lerp_constant, double deltatime){
                if(lesser_than_target){
                    return lerp_dt_margin(old_vel, speed, deltatime, lerp_constant, 5.f);
                }else if(!lesser_than_target){
                    return lerp_dt_margin(old_vel, -speed, deltatime, lerp_constant, 5.f);
                }else{
                    throw std::exception();
                }
            };

            
            float dist_to_target_x = std::abs(pos.x - target_pos.x);
            float dist_to_target_y = std::abs(pos.y - target_pos.y);

            float distance_margin = 5.f;
            if(dist_to_target_x > distance_margin && dist_to_target_y > distance_margin){
                target_speed = Vector2Multiply(target_speed, {1/1.414f, 1/1.414f});
            }

            // left/right movement
            if(dist_to_target_x > distance_margin){
                vel.x = lerp_velocity(vel.x, left_of_target, target_speed.x, accel_lerp_constant, deltatime);
            }else{
                vel.x = lerp_velocity(vel.x, left_of_target, 0, accel_lerp_constant, deltatime);
            }
            // up/down movement
            if(dist_to_target_y > distance_margin){
                vel.y = lerp_velocity(vel.y, above_target, target_speed.y, accel_lerp_constant, deltatime);
            }else{
                vel.y = lerp_velocity(vel.y, above_target, 0, accel_lerp_constant, deltatime);
            }
        }

        void set_target_entity_pos(Vector2 _pos){
            if(state == FOLLOWING || state == IDLE){
                target_pos = _pos;
            }
        }

        void render(Vector2 pos_offset) override {
            DrawTexture(*sprite, pos.x + pos_offset.x, pos.y + pos_offset.y, WHITE);
        };

        Vector2 vel;
        float accel_lerp_constant; // a constant used to 'slow down' or 'speed up' velocity lerping, expected to be ~0.005
        Vector2 size;
        Vector2 default_speed;
        Vector2 max_speed; // may change when moving diagonally, etc


        std::shared_ptr<Texture2D> sprite;

        FOLLOWER_STATE state;

    private:
        Vector2 target_pos;
};
