#ifndef ENTITY_H
#define ENTITY_H
#include <glad/glad.h>

class Entity {
public:
    void update(float delta, float g, float elast);
    void display(GLuint v_count) const;
};

#endif
