#ifndef SHADER_UTIL_H
#define SHADER_UTIL_H
#include <glad/glad.h>
#include <string>

class ShaderUtil
{
public:
    static std::string load_shader(const char* path);
    static GLuint compile_shader(GLenum type, const char* source);
    static GLuint create_shader(const char* vert_path, const char* frag_path);
};

#endif
