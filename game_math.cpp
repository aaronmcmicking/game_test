#pragma once

#include <iostream>
#include <cmath>
#include <raylib.h>
#include <raymath.h>

struct DirectionUDLR{
    bool up;
    bool down;
    bool left;
    bool right;
};

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

float lerp_dt_margin(float a, float b, float dt, float r = 5e-3f, float margin_of_error = 1e-3f){
    int lerped = lerp_dt(a, b, r, dt);
    if(std::abs(lerped - b) < margin_of_error){
        //std::cout << "abs(lerped-b) = abs(" << lerped << "-" << b << ") = " << std::abs(lerped-b) << " < margin = " << margin_of_error << std::endl;
        return b;
    }else{
        return lerped;
    }
}

Vector2 Vector2Absolute(Vector2 v){
    return {abs(v.x), abs(v.y)};
}

int sign(float f){
    return f >= 0 ? 1 : -1;
}
