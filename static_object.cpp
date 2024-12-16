#pragma once

#include "object.cpp"
#include <raylib.h>

class StaticObject: public Object{
    public:
        StaticObject(Vector2 _pos, bool _renderable = false, bool _solid = true, Rectangle _hitbox = {}): Object(_pos, _renderable, _solid, _hitbox) {
        }
};
