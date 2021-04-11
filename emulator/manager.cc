#include "manager.hh"
#include "compiler/compiler.hh"
#include "compiler/compilationresult.hh"

#include <iostream>

Manager::Manager()
    : window_(MainWindow::get())
{
    load_project_window_.on_start_executing([this](std::string const& filename) {
        load_project(filename);
    });
    load_project_window_.set_visible(true);
    
    window_.add_window(demo_window_);
    window_.add_window(load_project_window_);
    window_.add_window(message_box_);
}

void Manager::run()
{
    try {
        window_.run();
    } catch (std::exception& e) {
        window_.force_end_frame();
        message_box_.set_message(MessageBox::Error, e.what());
        run();
    }
}

void Manager::load_project(std::string const& project_name)
{
    CompilationResult result = Compiler().compile_from_project_file(project_name);
    if (result.error.has_value()) {
        message_box_.set_message(MessageBox::Error, result.error.value());
    } else {
        debug = result.debug;
        // TODO - load binaries
        // TODO - show windows
        load_project_window_.set_visible(false);
    }
}
