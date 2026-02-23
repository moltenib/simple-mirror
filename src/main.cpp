#include <string>
#include <vector>

#include <gtkmm.h>

#include "MainWindow.hpp"

namespace {

const std::vector<std::string> kIconCandidates = {
    "folder-sync",
    "document-save",
};

std::string pick_icon_name() {
    auto icon_theme = Gtk::IconTheme::get_default();
    if (!icon_theme) {
        return "";
    }

    for (const auto& candidate : kIconCandidates) {
        if (icon_theme->has_icon(candidate)) {
            return candidate;
        }
    }

    return "";
}

}  // namespace

int main(int argc, char* argv[]) {
    auto app = Gtk::Application::create(argc, argv, "org.quickbackup.app");

    const std::string icon_name = pick_icon_name();
    if (!icon_name.empty()) {
        Gtk::Window::set_default_icon_name(icon_name);
    }

    MainWindow window(icon_name);
    return app->run(window);
}
