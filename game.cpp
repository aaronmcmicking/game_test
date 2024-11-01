#include <iostream>

#include <cmath>
#include <chrono>

#include <raylib.h>
#include <rlgl.h>

double clampg(double val, double lo, double hi){
    if(val < lo){ return lo; }
    if(val > hi){ return hi; }
    return val;
}

/*
 * a: start value
 * b: target value
 * r: value remaining after 1s , 0<=r<=1,
 *  (ie. controls steepness of lerp curve where r=0 is linear and r=1 is stationary)
 * dt: deltatime
 */
float lerp_dt(float a, float b, float r, float dt){
    r = clampg(r, 0, 1);
    return std::lerp(a, b, 1-pow(r, dt));
    // lerp(a,b,t) = (1-t)a + bt -> frame dependant
}

class RectObject{
    public:
        RectObject(Vector2 _pos, Vector2 _vel, Vector3 _accel_lerp_xyr, Vector2 _size, Vector2 _default_speed){
            pos = _pos;
            vel = _vel;
            accel_lerp_xyr = _accel_lerp_xyr;
            size = _size;
            default_speed = _default_speed;
            rect = Rectangle{.x = _pos.x, .y = _pos.y, .width = _size.x, .height = _size.y};
        }

        Vector2 pos;
        Vector2 vel;
        Vector3 accel_lerp_xyr;
        Vector2 size;
        Vector2 default_speed;
        Rectangle rect;
};


int main(){
    const int DEFAULT_WINDOW_WIDTH = 1000;
    const int DEFAULT_WINDOW_HEIGHT = 1000;
    const char WINDOW_TITLE[] = "Game Test";
    const Color BACKGROUND_COLOR = Color{30, 30, 30, 255};
    const int TARGET_FPS = 60;

    // object init
    RectObject rect = RectObject(Vector2{0,0}, Vector2{0,0}, Vector3{.1f,.1f,.005f}, Vector2{100,100}, Vector2{500, 500});

    // window init
    InitWindow(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, WINDOW_TITLE);
    //SetTargetFPS(TARGET_FPS * 5.f);
    SetTargetFPS(TARGET_FPS);

    // deltatime init
    auto previous_time = std::chrono::high_resolution_clock::now();
    while(!WindowShouldClose()){
        // deltatime update
        auto current_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed_time = current_time - previous_time;
        previous_time = std::chrono::high_resolution_clock::now();

        double deltatime = elapsed_time.count() / 1000.l;
        //std::cout << deltatime << "ms" << std::endl;

        // input
        if(IsKeyDown(KEY_W)){
            rect.vel.y = lerp_dt(rect.vel.y, -rect.default_speed.y, rect.accel_lerp_xyr.z, deltatime);
        }else if(IsKeyDown(KEY_S)){
            rect.vel.y = lerp_dt(rect.vel.y, rect.default_speed.y, rect.accel_lerp_xyr.z, deltatime);
        }else{
            rect.vel.y = lerp_dt(rect.vel.y, 0, rect.accel_lerp_xyr.z, deltatime);
        }
        if(IsKeyDown(KEY_D)){
            rect.vel.x = lerp_dt(rect.vel.x, rect.default_speed.x, rect.accel_lerp_xyr.z, deltatime);
        }else if(IsKeyDown(KEY_A)){
            rect.vel.x = lerp_dt(rect.vel.x, -rect.default_speed.x, rect.accel_lerp_xyr.z, deltatime);
        }else{
            rect.vel.x = lerp_dt(rect.vel.x, 0, rect.accel_lerp_xyr.z, deltatime);
        }
        
        // update
        rect.pos.x += rect.vel.x * deltatime;
        rect.pos.y += rect.vel.y * deltatime;
        rect.rect.x = rect.pos.x;
        rect.rect.y = rect.pos.y;

        // draw
        BeginDrawing();
        ClearBackground(BACKGROUND_COLOR);
        DrawFPS(10, 10);

        DrawRectangleRec(rect.rect, RED);

        EndDrawing();
    }
    CloseWindow();

    return 0;
}
