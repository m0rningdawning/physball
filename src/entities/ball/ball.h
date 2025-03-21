#ifndef BALL_H
#define BALL_H
#include <glad/glad.h>

class Ball {
public:
    float pos_x, pos_y;
    float vel_x, vel_y;
    float rad;
    bool is_dragged;

    Ball(float pos_x, float pos_y, float rad);

    void update(float delta, float g, float elast, float friction);
    void display(GLuint v_count) const;
};

#endif
