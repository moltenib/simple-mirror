#include <string>
#include <vector>

#include <QApplication>
#include <QIcon>

#include "MainWindow.hpp"

namespace {

const std::vector<std::string> kIconCandidates = {
    "folder-sync",
    "document-save",
};

std::string pick_icon_name() {
    for (const auto& candidate : kIconCandidates) {
        if (!QIcon::fromTheme(QString::fromStdString(candidate)).isNull()) {
            return candidate;
        }
    }
    return "";
}

}  // namespace

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setStyle("Fusion");

    const std::string icon_name = pick_icon_name();
    if (!icon_name.empty()) {
        app.setWindowIcon(QIcon::fromTheme(QString::fromStdString(icon_name)));
    }

    MainWindow window(icon_name);
    window.show();
    return app.exec();
}
