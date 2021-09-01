#include "terminal.hh"

Terminal::Terminal(size_t lines, size_t columns)
        : lines_(lines), columns_(columns)
{
    reset();
    
    char c = 33;
}

void Terminal::add_char(uint16_t c)
{
    if (c == Return) {
        if (cursor_y_ == (lines_ - 1))
            pull_screen_up();
        else
            ++cursor_y_;
        cursor_x_ = 0;
    
    } else if (c == Backspace) {
        if (cursor_x_ == 0 && cursor_y_ > 0) {
            --cursor_y_;
            cursor_x_ = columns_ - 1;
        } else {
            --cursor_x_;
        }
        text_.at(cursor_y_).at(cursor_x_) = ' ';
        
    } else {
        text_.at(cursor_y_).at(cursor_x_) = (char) c;
        ++cursor_x_;
        if (cursor_x_ == (columns_)) {
            ++cursor_y_;
            cursor_x_ = 0;
        }
        if (cursor_y_ == (lines_))
            pull_screen_up();
    }
}

void Terminal::reset()
{
    text_.clear();
    for (size_t i = 0; i < lines_; ++i) {
        text_.emplace_back(columns_, ' ');
        /*
        for (size_t j = 0; j < columns_; ++j)
            text_.at(i).replace(j, 1, std::string(1, 'A' + ((j + i) % 26)));
        */
    }
    cursor_x_ = cursor_y_ = 0;
}

void Terminal::pull_screen_up()
{
    text_.erase(text_.begin());
    text_.emplace_back(columns_, ' ');
}
