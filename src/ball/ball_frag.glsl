#version 330 core

out vec4 FragColor;

uniform vec3 ball_col;

void main()
{
    FragColor = vec4(ball_col, 1.0);
}