#include "ball.h"
#include <cmath>
#include <vector>

Ball::Ball(float pos_x, float pos_y, float rad)
    : pos_x(pos_x), pos_y(pos_y), vel_x(0.0f), vel_y(0.0f), rad(rad)
{
    std::vector<float> vertices;
    for (int i = 0; i <= 360; i += 10)
    {
        const float angle = i * 3.14159f / 180.0f;
        vertices.push_back(pos_x + rad * cos(angle));
        vertices.push_back(pos_y + rad * sin(angle));
    }

    // VAO and VBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

Ball::~Ball()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
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

void Ball::display(GLint color_loc) const
{
    glUniform3f(color_loc, 1.0f, 0.0f, 0.0f);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 37);
    glBindVertexArray(0);
}