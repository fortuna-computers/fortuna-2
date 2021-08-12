#include "manager.hh"
#include "compiler/compiler.hh"
#include "compiler/compilationresult.hh"

#include <iostream>
#include "Z80.h"
#include "emulator/emulator.hh"

Manager::Manager()
    : window_(MainWindow::get())
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
    
    terminal_window_.on_send_keypress([this]() { send_keypress_window_.set_visible(true); });
    
    window_.add_window(load_project_window_);
    window_.add_window(message_box_);
    window_.add_window(menu_window_);
    window_.add_window(send_keypress_window_);
    for (auto const& window: menu_windows_)
        window_.add_window(*window);
}

Manager::~Manager()
{
    for (auto const& mwindow: menu_windows_)
        window_.set_property_bool(mwindow->name(), mwindow->visible());
}

void Manager::open_windows_from_last_time()
{
    for (auto const& mwindow: menu_windows_)
        if (window_.property_bool(mwindow->name()))
            mwindow->set_visible(true);
}

void Manager::run()
{
    try {
        window_.run();
    } catch (std::exception& e) {
        window_.force_end_frame();
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
        debug = result.debug;
        auto& rom = result.binaries.at(result.project_file.debug->rom).data;
        Emulator::get().initialize(rom, result.project_file);
        menu_window_.set_visible(true);
        open_windows_from_last_time();
        load_project_window_.set_visible(false);
    }
}

