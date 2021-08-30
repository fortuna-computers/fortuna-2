#include "demowindow.hh"

#include "imgui.h"

void DemoWindow::draw()
{
    ImGui::ShowDemoWindow(&visible_);
}