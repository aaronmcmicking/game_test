#pragma once

#include <iostream>
#include <optional>
#include <raylib.h>
#include <array>
#include <vector>
#include "game_math.cpp"

/*
enum DirectionUDLR{
    UP = 1,
    DOWN,
    LEFT,
    RIGHT
};
*/

class Object{
    public:
        Object(Vector2 _pos, bool _solid = false, Rectangle _hitbox = {}){
            pos = _pos;
            solid = _solid;

            if(!_solid || (_hitbox.width == 0 && _hitbox.height == 0)){
                hitbox.reset();
            }else{
                hitbox.emplace(_hitbox);
            }

            id = id_counter++;
        }

        //std::array<DirectionUDLR, 4> check_collision(std::vector<Object*> objects, Vector2 future_pos){
        DirectionUDLR check_collision_solids(std::vector<Object*> objects, Vector2 future_pos){
            //if(!solid){ return {false, false, false, false}; }
            if(!hitbox.has_value()){ return {false, false, false, false}; }

            DirectionUDLR collisions = {false, false, false, false};
            for(const auto& object: objects){
                //std::cout << "object (" << id << ") checking collision against object (" << object->id << ")" << std::endl;
                if(object->id == id){ continue; } // guard against self-comparison
                                                  
                if(object->solid && object->hitbox.has_value()){
                    //std::cout << "object (" << id << ") calculating collision against object (" << object->id << ")" << std::endl;
                    Rectangle other_hitbox = object->hitbox.value();
                    Rectangle my_hitbox = hitbox.value();
                    my_hitbox.x = future_pos.x;
                    my_hitbox.y = future_pos.y;
                    if(CheckCollisionRecs(my_hitbox, other_hitbox)){
                        // up
                        if((my_hitbox.y < other_hitbox.y + other_hitbox.height) && (my_hitbox.y > other_hitbox.y)){
                            collisions.up = true;
                        }

                        // down
                        if((my_hitbox.y + my_hitbox.height > other_hitbox.y) && (my_hitbox.y + my_hitbox.height < other_hitbox.y + other_hitbox.height)){
                            collisions.down = true;
                        }

                        // left
                        if((my_hitbox.x < other_hitbox.x + other_hitbox.width) && (my_hitbox.x > other_hitbox.x)){
                            collisions.left = true;
                        }

                        // right
                        if((my_hitbox.x + my_hitbox.width > other_hitbox.x) && (my_hitbox.x + my_hitbox.width < other_hitbox.x + other_hitbox.width)){
                            collisions.right = true;
                        }
                    } // colliding with current object
                } // current object is solid
            } // for object in object
            return collisions;
        }

        Vector2 pos;
        bool solid = false;
        std::optional<Rectangle> hitbox;
        int id;

    private:
        inline static int id_counter = 0;
};
