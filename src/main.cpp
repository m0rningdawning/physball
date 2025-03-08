#include <iostream>
#include <thread>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "shader_util.h"
#include "entities/ball/ball.h"

/*
 * TODO:
 * - Reformat and clean the code
 * - Reformat the constants from the upper case to the proper convention.
 * Current focus:
 * - Add the ground
 * - Enable gravity and simulate collision with the ground
 * Near future:
 * - Add more balls
 * - Add proper collision with other balls
 * - Add directed forces
 * - Add elasticity properties to the ground
 * - Improve rendering to add more shapes
 * Far future:
 * - Switch to 3D
 * - Improve 3D render to support more models
 * - Simulate materials and their influence on one another
 * - Simulate shatter and other events
 */

constexpr int WIDTH = 800;
constexpr int HEIGHT = 600;
constexpr float GRAVITY = -9.8f;
constexpr float ELASTICITY = 0.8f;
constexpr double FRAME_TIME = 1.0 / 144.0;

std::vector<float> verts;

GLuint VAO, VBO;

void regenerate_vertices(const Ball& ball, float win_r) {
    verts.clear();

    for (int i = 0; i <= 360; i += 10) {
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

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);

    float aspect_ratio = static_cast<float>(width) / static_cast<float>(height);
    regenerate_vertices(*static_cast<Ball*>(glfwGetWindowUserPointer(window)), aspect_ratio);
}

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    std::vector bg_color{0.2f, 0.3f, 0.3f, 1.0f};

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "PhysBall", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, WIDTH, HEIGHT);

    // Ball
    Ball ball(0.0f, 0.0f, 0.5f);
    glfwSetWindowUserPointer(window, &ball);

    // Ball color struct
    struct ball_color_struct {
        float color[3]{1.0f, 0.0f, 0.0f};
        bool is_rgb = false;
        float rgb_inc = 0.0001f;
        int curr_rgb = 1;
    } ball_color;

    // Shader build and compilation
    GLuint ball_shader_prog = ShaderUtil::create_shader("../src/entities/ball/ball_vert.glsl",
                                                        "../src/entities/ball/ball_frag.glsl");

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

    // Init time
    GLdouble l_time = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        GLdouble c_time = glfwGetTime();
        GLfloat d_time = static_cast<GLfloat>(c_time - l_time);

        if (d_time < FRAME_TIME) {
            std::this_thread::sleep_for(std::chrono::duration<double>(FRAME_TIME - d_time));
            c_time = glfwGetTime();
            d_time = static_cast<float>(c_time - l_time);
        }

        l_time = c_time;

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

        if (ball_color.is_rgb) {
            if (ball_color.rgb_inc < 0.0f || ball_color.rgb_inc > 0.01f) {
                ball_color.rgb_inc = 0.0f;
            }

            switch (ball_color.curr_rgb) {
            case 0:
                if (ball_color.color[0] <= 1.0f) {
                    ball_color.color[0] += ball_color.rgb_inc;
                }
                else {
                    if (ball_color.color[2] > 0.0f) {
                        ball_color.color[2] -= ball_color.rgb_inc;
                    }
                    else {
                        ball_color.curr_rgb = 1;
                    }
                }
                break;
            case 1:
                if (ball_color.color[1] <= 1.0f) {
                    ball_color.color[1] += ball_color.rgb_inc;
                }
                else {
                    if (ball_color.color[0] > 0.0f) {
                        ball_color.color[0] -= ball_color.rgb_inc;
                    }
                    else
                        ball_color.curr_rgb = 2;
                }
                break;
            case 2:
                if (ball_color.color[2] <= 1.0f) {
                    ball_color.color[2] += ball_color.rgb_inc;
                }
                else {
                    if (ball_color.color[1] > 0.0f) {
                        ball_color.color[1] -= ball_color.rgb_inc;
                    }
                    else
                        ball_color.curr_rgb = 0;
                }
                break;
            default:
                break;
            }
        }

        glUniform3f(ball_col_loc, ball_color.color[0], ball_color.color[1], ball_color.color[2]);

        glUniform1f(win_r_loc, win_r);

        // Ball display and move
        ball.update(d_time, GRAVITY, ELASTICITY);
        regenerate_vertices(ball, win_r);
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
        ImGui::ColorEdit3("Color", ball_color.color);
        ImGui::Checkbox("RGB Color Wave", &ball_color.is_rgb);
        ImGui::InputFloat("Color Wave Speed", &ball_color.rgb_inc, 0.0001f, 0.001f, "%.4f");
        ImGui::End();

        ImGui::Begin("Ball Size");
        if (ImGui::SliderFloat("Radius", &ball.rad, 0.01f, 1.0f)) {
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
