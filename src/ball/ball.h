#ifndef BALL_H
#define BALL_H
#include <glad/glad.h>

class Ball
{
public:
    float pos_x, pos_y, vel_x, vel_y, rad;
    GLuint VAO, VBO;
    Ball(float pos_x, float pos_y, float rad);
    ~Ball();
    void update(float delta, float g, float elast);
    void display(GLint color_loc) const;
};

#endif
