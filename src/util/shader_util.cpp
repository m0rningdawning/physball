#include <iostream>
#include <fstream>
#include <sstream>
#include "shader_util.h"

std::string ShaderUtil::load_shader(const char* path)
{
    const std::ifstream file(path);
    if (!file.is_open())
    {
        std::cerr << "Failed to open shader file: " << path << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();
    std::cout << "Loaded shader from " << path << std::endl;
    return source;
}

GLuint ShaderUtil::compile_shader(const GLenum type, const char* source)
{
    const GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char info[512];
        glGetShaderInfoLog(shader, 512, nullptr, info);
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << info << std::endl;
    }
    return shader;
}

GLuint ShaderUtil::create_shader(const char* vert_path, const char* frag_path)
{
    const std::string vert_c = load_shader(vert_path);
    const std::string frag_c = load_shader(frag_path);

    const GLuint vert = compile_shader(GL_VERTEX_SHADER, vert_c.c_str());
    const GLuint frag = compile_shader(GL_FRAGMENT_SHADER, frag_c.c_str());

    const GLuint shader_prog = glCreateProgram();
    glAttachShader(shader_prog, vert);
    glAttachShader(shader_prog, frag);
    glLinkProgram(shader_prog);

    GLint success;
    glGetProgramiv(shader_prog, GL_LINK_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetProgramInfoLog(shader_prog, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vert);
    glDeleteShader(frag);

    return shader_prog;
}
