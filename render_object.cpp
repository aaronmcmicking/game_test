#pragma once

#include <raylib.h>
#include "object.cpp"

class RenderObject: public Object{
    public:
        RenderObject(Vector2 _pos, bool _solid = false, Rectangle _hitbox = {}): Object(_pos, true, _solid, _hitbox) {};

        virtual void render(Vector2) = 0;
};
