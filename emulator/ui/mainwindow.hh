#ifndef EMULATOR_MAINWINDOW_HH
#define EMULATOR_MAINWINDOW_HH

#include <memory>

class MainWindow {
public:
    ~MainWindow();
    
    void run();

    static MainWindow& get();
    
private:
    MainWindow();
    void on_error(int error, const char* description);
    
    struct GLFWwindow* window;
    
    static constexpr float background_color[] = { 0.45f, 0.55f, 0.60f };
};

#endif
