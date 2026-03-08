#pragma once

#include <filesystem>
#include <string>

class Settings {
public:
    static Settings& instance();

    const std::string& origin() const;
    const std::string& destination() const;

    void set_origin(const std::string& origin);
    void set_destination(const std::string& destination);

    std::string file_path() const;
    bool load();
    bool save() const;

private:
    Settings() = default;

    static std::filesystem::path user_home_dir();
    static std::filesystem::path config_dir();
    static std::filesystem::path settings_path();

    std::string origin_;
    std::string destination_;
};
