#include "utils/AppSetup.hpp"

#include <string>
#include <vector>

#include <QApplication>
#include <QCoreApplication>
#include <QDir>
#include <QIcon>
#include <QLocale>
#include <QTranslator>
#include <QWidget>

namespace app_setup {

namespace {

const std::vector<std::string> kIconCandidates = {
    "folder-sync",
    "document-save",
};

}  // namespace

std::string pick_icon_name() {
    for (const auto& candidate : kIconCandidates) {
        if (!QIcon::fromTheme(QString::fromStdString(candidate)).isNull()) {
            return candidate;
        }
    }
    return "";
}

void apply_app_icon(QApplication& app, const std::string& icon_name) {
    if (icon_name.empty()) {
        return;
    }
    app.setWindowIcon(QIcon::fromTheme(QString::fromStdString(icon_name)));
}

void apply_window_icon(QWidget& window, const std::string& icon_name) {
    if (icon_name.empty()) {
        return;
    }
    window.setWindowIcon(QIcon::fromTheme(QString::fromStdString(icon_name)));
}

void install_app_translator(QApplication& app) {
    const QLocale locale = QLocale::system();
    const QString language = locale.name().section('_', 0, 0).toLower();
    if (language == "en") {
        return;
    }

    const QString locale_name = locale.name();
    const QString app_dir = QCoreApplication::applicationDirPath();
    const QString cwd = QDir::currentPath();
    const QStringList search_dirs = {
        QDir(app_dir).filePath("resources/locales/qm"),
        QDir(cwd).filePath("resources/locales/qm"),
    };
    const QStringList suffixes = {
        locale_name,
        language,
    };

    auto* translator = new QTranslator(&app);
    for (const QString& search_dir : search_dirs) {
        for (const QString& suffix : suffixes) {
            if (translator->load("simple-mirror_" + suffix, search_dir)) {
                app.installTranslator(translator);
                return;
            }
        }
    }

    delete translator;
}

}  // namespace app_setup
