#include <string>

#include <QApplication>

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
    return app.exec();
}
