#include "mainwindow.hh"

#include <stdexcept>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"

//
// INITIALIZATION
//

MainWindow& MainWindow::get()
{
    static MainWindow main_window;
    return main_window;
}

MainWindow::MainWindow()
{
    // initialize
    if (!glfwInit())
        throw std::runtime_error("Could not initialize GLFW.");
    glfwSetErrorCallback([](int error, const char* description) { MainWindow::get().on_error(error, description); });
    
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
    context_ = ImGui::CreateContext();
    io_ = &ImGui::GetIO();
    ImGui_ImplGlfw_InitForOpenGL(window_, true);
    ImGui_ImplOpenGL3_Init(glsl_version.c_str());
    ImGui::StyleColorsDark();
    
    // properties
    initialize_properties(context_);
    ImGui::LoadIniSettingsFromDisk("imgui.ini");
}

MainWindow::~MainWindow()
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

void MainWindow::run()
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

void MainWindow::force_end_frame()
{
    while (context_->CurrentWindowStack.Size > 1)
        ImGui::End();
    ImGui::EndFrame();
}

//
// CALLBACKS
//

void MainWindow::on_error(int error, char const* description)
{
    throw std::runtime_error("Error " + std::to_string(error) + ": " + description);
}

//
// PROPERTIES
//

std::string MainWindow::property(std::string const& name) const
{
    auto it = properties_.find(name);
    if (it == properties_.end())
        return "";
    return it->second;
}

void MainWindow::set_property(std::string const& name, std::string const& value)
{
    properties_[name] = value;
}

void MainWindow::initialize_properties(ImGuiContext* p_context)
{
    ImGuiSettingsHandler ini_handler;
    ini_handler.TypeName = "UserData";
    ini_handler.TypeHash = ImHashStr("UserData");
    ini_handler.ReadOpenFn = [](ImGuiContext*, ImGuiSettingsHandler* h, const char* name) -> void* {
        if (strcmp(name, "Config") == 0)
            return h->UserData;
        return nullptr;
    };
    ini_handler.ReadLineFn = [](ImGuiContext*, ImGuiSettingsHandler*, void* entry, const char* line) {
        auto& props = *reinterpret_cast<std::unordered_map<std::string, std::string>*>(entry);
        std::string sline = line;
        if (sline.find('=') != std::string::npos) {
            std::string key = sline.substr(0, sline.find('='));
            std::string value = sline.substr(sline.find('=') + 1);
            props[key] = value;
        }
    };
    ini_handler.WriteAllFn = [](ImGuiContext*, ImGuiSettingsHandler* h, ImGuiTextBuffer* buf) {
        auto& props = *reinterpret_cast<std::unordered_map<std::string, std::string>*>(h->UserData);
        buf->appendf("[UserData][Config]\n");
        for (auto& [k,v] : props)
            buf->appendf((k + "=%s").c_str(), v.c_str());
    };
    ini_handler.UserData = &properties_;
    p_context->SettingsHandlers.push_back(ini_handler);
}

ImGuiIO& MainWindow::io() const
{
    return *io_;
}


