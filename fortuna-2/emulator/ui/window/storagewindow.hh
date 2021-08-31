#ifndef EMULATOR_STORAGEWINDOW_HH
#define EMULATOR_STORAGEWINDOW_HH

#include "../window.hh"

class StorageWindow : public Window {
public:
    explicit StorageWindow(Emulator& emulator) : Window(emulator) {}
    
    void draw() override;
    std::string name() const override { return "storage_window"; }
    
    void set_image_file(ImageFile& image_file);

private:
    ImageFile* image_file_ = nullptr;
    
    void draw_storage_table() const;
    void go_to_block_number(ssize_t block);
    void load_data();
    
    ssize_t block_number_ = 0;
    uint8_t data_[512] = { 0 };
};

#endif
