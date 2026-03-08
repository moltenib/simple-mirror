#include "utils/Settings.hpp"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>

#include <QJsonDocument>
#include <QJsonObject>
#include <QString>

std::filesystem::path Settings::user_home_dir() {
    const char* home = std::getenv("HOME");
#ifdef _WIN32
    if (!home || !*home) {
        home = std::getenv("USERPROFILE");
    }
#endif
    if (home && *home) {
        return std::filesystem::path(home);
    }
    return std::filesystem::current_path();
}

std::filesystem::path Settings::config_dir() {
#ifdef _WIN32
    const char* local_app_data = std::getenv("LOCALAPPDATA");
    if (local_app_data && *local_app_data) {
        return std::filesystem::path(local_app_data) / "simple-mirror";
    }

    const char* app_data = std::getenv("APPDATA");
    if (app_data && *app_data) {
        return std::filesystem::path(app_data) / "simple-mirror";
    }

    return user_home_dir() / "AppData" / "Local" / "simple-mirror";
#else
    const char* xdg_config_home = std::getenv("XDG_CONFIG_HOME");
    if (xdg_config_home && *xdg_config_home) {
        return std::filesystem::path(xdg_config_home) / "simple-mirror";
    }
    return user_home_dir() / ".config" / "simple-mirror";
#endif
}

std::filesystem::path Settings::settings_path() {
    return config_dir() / "settings.json";
}

Settings& Settings::instance() {
    static Settings settings;
    return settings;
}

const std::string& Settings::origin() const {
    return origin_;
}

const std::string& Settings::destination() const {
    return destination_;
}

void Settings::set_origin(const std::string& origin) {
    origin_ = origin;
}

void Settings::set_destination(const std::string& destination) {
    destination_ = destination;
}

std::string Settings::file_path() const {
    return settings_path().string();
}

bool Settings::load() {
    origin_.clear();
    destination_.clear();

    const std::filesystem::path path = settings_path();
    if (!std::filesystem::exists(path)) {
        std::error_code ec;
        std::filesystem::create_directories(path.parent_path(), ec);
        return false;
    }

    std::ifstream input(path);
    if (!input) {
        return false;
    }

    const std::string json_text{
        std::istreambuf_iterator<char>(input),
        std::istreambuf_iterator<char>()};

    QJsonParseError parse_error;
    const QJsonDocument document = QJsonDocument::fromJson(
        QByteArray::fromStdString(json_text),
        &parse_error);
    if (parse_error.error != QJsonParseError::NoError || !document.isObject()) {
        return false;
    }

    const QJsonObject object = document.object();
    origin_ = object.value("origin").toString().toStdString();
    destination_ = object.value("destination").toString().toStdString();
    return true;
}

bool Settings::save() const {
    const std::filesystem::path path = settings_path();
    std::error_code ec;
    std::filesystem::create_directories(path.parent_path(), ec);
    if (ec) {
        return false;
    }

    QJsonObject object;
    object.insert("origin", QString::fromStdString(origin_));
    object.insert("destination", QString::fromStdString(destination_));
    const QJsonDocument document(object);

    std::ofstream output(path, std::ios::trunc);
    if (!output) {
        return false;
    }

    output << document.toJson(QJsonDocument::Indented).toStdString();
    return output.good();
}
