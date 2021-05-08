#include "catch.hpp"

#include "../emulator/terminal.hh"

TEST_CASE("Live overflow", "[Terminal]") {
    Terminal terminal(5, 5);
    REQUIRE(terminal.text().at(0) == "     ");
    REQUIRE(terminal.cursor_x() == 0);
    
    terminal.add_char('H');
    REQUIRE(terminal.text().at(0) == "H    ");
    REQUIRE(terminal.cursor_x() == 1);
    
    terminal.add_char('e');
    REQUIRE(terminal.text().at(0) == "He   ");
    REQUIRE(terminal.cursor_x() == 2);
    
    terminal.add_char('l');
    terminal.add_char('l');
    terminal.add_char('o');
    terminal.add_char(' ');
    terminal.add_char('W');
    REQUIRE(terminal.text().at(0) == "Hello");
    REQUIRE(terminal.text().at(1) == " W   ");
    REQUIRE(terminal.cursor_x() == 2);
    REQUIRE(terminal.cursor_y() == 1);
}

TEST_CASE("Screen overflow", "[Terminal]") {
    Terminal terminal(5, 5);
    
    terminal.add_char('H');
    terminal.add_char(Terminal::Return);
    terminal.add_char('W');
    REQUIRE(terminal.text().at(0) == "H    ");
    REQUIRE(terminal.text().at(1) == "W    ");
    REQUIRE(terminal.cursor_x() == 1);
    REQUIRE(terminal.cursor_y() == 1);
    
    terminal.add_char(Terminal::Return);
    terminal.add_char(Terminal::Return);
    terminal.add_char(Terminal::Return);
    terminal.add_char(Terminal::Return);
    REQUIRE(terminal.text().at(0) == "W    ");
    REQUIRE(terminal.text().at(1) == "     ");
}
