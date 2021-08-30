#ifndef EMULATOR_WINDOW_HH
#define EMULATOR_WINDOW_HH

#include <string>
#include "../emulator/emulator.hh"

class Window {
public:
    explicit Window(Emulator& emulator) : emulator_(emulator) {}
    
    [[nodiscard]] bool visible() const { return visible_; }
    virtual void set_visible(bool visible) { visible_ = visible; }
    
    // implement this
    virtual void draw() = 0;
    virtual void update() {}
    [[nodiscard]] virtual std::string name() const = 0;

protected:
    Emulator& emulator_;
    bool visible_ = false;
    
    static const int PageUp = 0x10a;
    static const int PageDown = 0x10b;
    static const int F2 = 0x123;
    static const int F7 = 0x128;
    static const int F8 = 0x129;
    static const int F9 = 0x12a;
    static const int F10 = 0x12b;
    static const int F12 = 0x12d;
};

#endif
