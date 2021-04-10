#include "glfwwindow.hh"

#include <stdexcept>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>

#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"

//
// INITIALIZATION
//

GLFWWindow& GLFWWindow::get()
{
    static GLFWWindow main_window;
    return main_window;
}

GLFWWindow::GLFWWindow()
{
    // initialize
    if (!glfwInit())
        throw std::runtime_error("Could not initialize GLFW.");
    glfwSetErrorCallback([](int error, const char* description) { GLFWWindow::get().on_error(error, description); });
    
    // window hints
    glfwWindowHint(GLFW_DOUBLEBUFFER, 1);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);
    glfwWindowHint(GLFW_STENCIL_BITS, 8);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    std::string glsl_version = "#version 150";
    
    // create window
    window_ = glfwCreateWindow(800, 600, "Mini-Z80 emulator", nullptr, nullptr);
    
    // setup OpenGL
    glfwMakeContextCurrent(window_);
    if (!gladLoadGL(glfwGetProcAddress))
        throw std::runtime_error("Could not initialize GLAD.");
    glClearColor(background_color[0], background_color[1], background_color[2], 1.0f);
    
    // configure GLFW
    glfwSwapInterval(1);
    glfwSetFramebufferSizeCallback(window_, [](GLFWwindow*, int width, int height) { glViewport(0, 0, width, height); });
    
    // configure ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplGlfw_InitForOpenGL(window_, true);
    ImGui_ImplOpenGL3_Init(glsl_version.c_str());
    ImGui::StyleColorsDark();
}

GLFWWindow::~GLFWWindow()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    
    if (window_)
        glfwDestroyWindow(window_);
    glfwTerminate();
}

//
// MAIN LOOP
//

void GLFWWindow::run()
{
    while (!glfwWindowShouldClose(window_)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        for (auto& child: children_)
            if (child->visible())
                child->draw();
        
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    
        glfwSwapBuffers(window_);
        
        glfwPollEvents();
    }
}

//
// CALLBACKS
//

void GLFWWindow::on_error(int error, char const* description)
{
    throw std::runtime_error("Error " + std::to_string(error) + ": " + description);
}
