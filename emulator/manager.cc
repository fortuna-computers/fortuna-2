#include "manager.hh"
#include "compiler/compiler.hh"
#include "compiler/compilationresult.hh"

#include <iostream>
#include "emulator/emulator.hh"

Manager::Manager()
    : properties_(gui_.context()),
      load_project_window_(properties_)
{
    load_project_window_.on_start_executing([this](std::string const& filename) {
        load_project(filename);
    });
    load_project_window_.set_visible(true);
    
    menu_window_.on_open_code([this]() { code_window_.set_visible(true); });
    menu_window_.on_open_cpu([this]() { cpu_window_.set_visible(true); });
    menu_window_.on_open_memory([this]() { ram_window_.set_visible(true); });
    menu_window_.on_open_demo([this]() { demo_window_.set_visible(true); });
    menu_window_.on_open_terminal([this]() { terminal_window_.set_visible(true); });
    
    code_window_.on_go_to_file([this]() { file_select_window_.set_visible(true); });
    
    terminal_window_.on_send_keypress([this]() { send_keypress_window_.set_visible(true); });
    
    gui_.add_window(load_project_window_);
    gui_.add_window(message_box_);
    gui_.add_window(menu_window_);
    gui_.add_window(send_keypress_window_);
    gui_.add_window(file_select_window_);
    for (auto const& window: menu_windows_)
        gui_.add_window(*window);
}

Manager::~Manager()
{
    for (auto const& mwindow: menu_windows_)
        properties_.set_property_bool(mwindow->name(), mwindow->visible());
}

void Manager::open_windows_from_last_time()
{
    for (auto const& mwindow: menu_windows_)
        if (properties_.property_bool(mwindow->name()))
            mwindow->set_visible(true);
}

void Manager::run()
{
    try {
        gui_.run();
    } catch (std::exception& e) {
        gui_.force_end_frame();
        std::cerr << e.what() << "\n";
        message_box_.set_message(MessageBox::Error, e.what());
        run();
    }
}

void Manager::load_project(std::string const& project_name)
{
    CompilationResult result = Compiler().compile_from_project_file(project_name);
    std::cout << result.message;
    if (result.error.has_value()) {
        message_box_.set_message(MessageBox::Error, result.error.value());
    } else {
        auto& rom = result.binaries.at(result.project_file.debug->rom).data;
        Emulator::get().initialize(rom, result.project_file);
        menu_window_.set_visible(true);
        open_windows_from_last_time();
        load_project_window_.set_visible(false);
    
        code_model_ = CodeModel(result.debug);
        code_window_.set_code_model(*code_model_);
        file_select_window_.set_code_model(*code_model_);
    }
}

