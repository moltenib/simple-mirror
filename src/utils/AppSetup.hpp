#pragma once

#include <string>

class QApplication;
class QWidget;

namespace app_setup {

std::string pick_icon_name();
void apply_app_icon(QApplication& app, const std::string& icon_name);
void apply_window_icon(QWidget& window, const std::string& icon_name);
void install_app_translator(QApplication& app);

}  // namespace app_setup
