#include <ball.h>
#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "shader_util.h"

constexpr int WIDTH = 800;
constexpr int HEIGHT = 600;

std::vector<float> verts;

GLuint VAO, VBO;

void regenerate_vertices(const Ball& ball, float win_r)
{
    verts.clear();
    for (int i = 0; i <= 360; i += 10)
    {
        float angle = static_cast<float>(i) * 3.14159f / 180.0f;
        float ndc_x = ball.rad * cos(angle) / win_r;
        float ndc_y = ball.rad * sin(angle);
        verts.push_back(ball.pos_x + ndc_x);
        verts.push_back(ball.pos_y + ndc_y);
    }

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), verts.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);

    float aspect_ratio = static_cast<float>(width) / static_cast<float>(height);
    regenerate_vertices(*static_cast<Ball*>(glfwGetWindowUserPointer(window)), aspect_ratio);
}

int main()
{
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    std::vector bg_color{0.2f, 0.3f, 0.3f, 1.0f};

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "PhysBall", nullptr, nullptr);
    if (!window)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, WIDTH, HEIGHT);

    // Ball
    Ball ball(0.0f, 0.0f, 0.5f);
    glfwSetWindowUserPointer(window, &ball);

    // Color array
    float ball_color[3]{1.0f, 0.0f, 0.0f};

    // Shader build and compilation
    GLuint ball_shader_prog = ShaderUtil::create_shader("../src/ball/ball_vert.glsl", "../src/ball/ball_frag.glsl");

    // VAO and VBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Initial vertices
    regenerate_vertices(ball, static_cast<float>(WIDTH) / HEIGHT);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // ImGui setup
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    ImGui::StyleColorsDark();

    while (!glfwWindowShouldClose(window))
    {
        GLint win_w, win_h;
        glfwGetFramebufferSize(window, &win_w, &win_h);
        float win_r = static_cast<float>(win_w) / static_cast<float>(win_h);

        glClearColor(bg_color[0], bg_color[1], bg_color[2], bg_color[3]);
        glClear(GL_COLOR_BUFFER_BIT);

        // The ball
        const GLint ball_col_loc = glGetUniformLocation(ball_shader_prog, "ball_col");
        const GLint win_r_loc = glGetUniformLocation(ball_shader_prog, "win_r");
        glUseProgram(ball_shader_prog);
        glBindVertexArray(VAO);

        glUniform3f(ball_col_loc, ball_color[0], ball_color[1], ball_color[2]);
        glUniform1f(win_r_loc, win_r);

        ball.display(verts.size() / 2);

        // ImGui shenanigans
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Background Color");
        ImGui::ColorEdit4("Color", bg_color.data());
        ImGui::End();

        ImGui::Begin("Ball Color");
        ImGui::ColorEdit3("Color", ball_color);
        ImGui::End();

        ImGui::Begin("Ball Size");
        if (ImGui::SliderFloat("Radius", &ball.rad, 0.01f, 1.0f))
        {
            regenerate_vertices(ball, win_r);
        }
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Imgui cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // Everything else cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(ball_shader_prog);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
