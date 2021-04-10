#ifndef EMULATOR_WINDOW_HH
#define EMULATOR_WINDOW_HH

class Window {
public:
    [[nodiscard]] bool visible() const { return visible_; }
    void set_visible(bool visible) { visible_ = visible; }
    virtual void draw() = 0;

protected:
    Window() = default;
    
    bool visible_ = true;
};

#endif
