#include "manager.hh"
#include "compiler/compiler.hh"
#include "compiler/compilationresult.hh"

#include <iostream>
#include <filesystem>

Manager::Manager()
    : demo_window_(emulator_),
      load_project_window_(emulator_, properties_),
      message_box_(emulator_),
      send_keypress_window_(emulator_),
      ram_window_(emulator_),
      cpu_window_(emulator_),
      terminal_window_(emulator_, send_keypress_window_),
      storage_window_(emulator_),
      symbol_select_window_(emulator_),
      code_window_(emulator_, symbol_select_window_),
      menu_window_(emulator_, { code_window_, cpu_window_, ram_window_, terminal_window_, demo_window_, storage_window_ })
{
    properties_.initialize(gui_.context());
    
    symbol_select_window_.set_code_window(&code_window_);
    
    load_project_window_.on_start_executing([this](std::string const& filename) {
        if (load_project(filename))
            open_windows_from_last_time();
    });
    load_project_window_.set_visible(true);
    
    code_window_.on_recompile_project([this]() {
        load_project(last_project_loaded_);
    });
    
    for (auto const& window: menu_windows_)
        gui_.add_window(*window);
    for (auto const& window: other_windows_)
        gui_.add_window(*window);
}

Manager::~Manager()
{
    for (auto const& mwindow: menu_windows_)
        properties_.set_property_bool(mwindow->name(), mwindow->visible());
}

void Manager::run(std::optional<std::string> const& project_to_load)
{
    try {
        gui_.step(true);
        if (project_to_load) {
            load_project_window_.load_project(*project_to_load);
            open_windows_from_last_time();
        }
        
        while (gui_.continue_executing()) {
            gui_.step(emulator_.stopped());
            emulator_.execute();
            if (!emulator_.stopped()) {
                auto key = gui_.last_keypress();
                if (key)
                    emulator_.keypress((uint8_t) *key);
            }
        }
    } catch (std::exception& e) {
        gui_.force_end_frame();
        std::cerr << e.what() << "\n";
        message_box_.set_message(MessageBox::Error, e.what());
        run({});
    }
}

bool Manager::load_project(std::string const& project_name)
{
    CompilationResult result = Compiler().compile_from_project_file(project_name);
    std::cout << result.message;
    
    auto sources_path = std::filesystem::path(project_name).parent_path();
    
    if (result.error.has_value()) {
        message_box_.set_message(MessageBox::Error, result.error.value());
        return false;
    } else {
        // initialize emulator
        auto& rom = result.binaries.at(result.project_file.rom).data;
        image_file.emplace(64, result, true);
        emulator_.initialize(rom, image_file);
    
        // open windows
        load_project_window_.set_visible(false);
        menu_window_.set_visible(true);
        
        // setup window objects
        code_model_.emplace(emulator_, result.debug);
        code_window_.set_code_model(*code_model_);
        symbol_select_window_.set_code_model(*code_model_);
        if (image_file)
            storage_window_.set_image_file(*image_file);
        
        last_project_loaded_ = project_name;
        return true;
    }
}

void Manager::open_windows_from_last_time()
{
    for (auto const& mwindow: menu_windows_)
        if (properties_.property_bool(mwindow->name()))
            mwindow->set_visible(true);
}

