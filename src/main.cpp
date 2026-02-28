#include <string>

#include <QApplication>
#include <QGuiApplication>
#include <QScreen>
#include <QTimer>

#include "utils/AppSetup.hpp"
#include "views/MainWindow.hpp"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setStyle("Fusion");
    app_setup::install_app_translator(app);

    const std::string icon_name = app_setup::pick_icon_name();
    app_setup::apply_app_icon(app, icon_name);

    MainWindow window(icon_name);
    window.show();

    QScreen* target_screen = window.screen();
    if (!target_screen) {
        target_screen = QGuiApplication::primaryScreen();
    }
    if (target_screen) {
        const QRect available = target_screen->availableGeometry();
        const QRect frame = window.frameGeometry();
        const int x = available.left() + ((available.width() - frame.width()) / 2);
        const int y = available.top() + ((available.height() - frame.height()) / 2);
        window.move(x, y);
    }

    QTimer::singleShot(0, &window, [&window]() {
        window.raise();
        window.activateWindow();
    });

    return app.exec();
}
