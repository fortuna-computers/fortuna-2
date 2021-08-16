#ifndef EMULATOR_MESSAGEBOX_HH
#define EMULATOR_MESSAGEBOX_HH

#include <string>

#include "../window.hh"

class MessageBox : public Window {
public:
    explicit MessageBox(Emulator& emulator) : Window(emulator) {}
    
    enum Type { Info, Error, FatalError };
    
    void set_message(Type type, std::string const& message, std::string const& title = "");
    void draw() override;
    std::string name() const override { return "message_box"; }

private:
    std::string message_;
    std::string title_;
    Type        type_ = Type::Info;
};

#endif
