#include "ball.h"
#include <cmath>
#include <vector>

Ball::Ball(float pos_x, float pos_y, float rad)
    : pos_x(pos_x), pos_y(pos_y), vel_x(0.0f), vel_y(0.0f), rad(rad)
{
}

void Ball::update(float delta, float g, float elast)
{
    vel_y += g * delta;
    pos_x += vel_x * delta;
    pos_y += vel_y * delta;

    if (pos_y - rad < -1.0f)
    {
        pos_y = -1.0f + rad;
        vel_y = -vel_y * elast;
    }
}

void Ball::display(GLuint v_count) const
{
    glDrawArrays(GL_TRIANGLE_FAN, 0, v_count);
}
