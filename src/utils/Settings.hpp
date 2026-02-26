#pragma once

#include <string>

namespace settings {

struct Settings {
    std::string origin;
    std::string destination;
};

std::string settings_file_path();
Settings load();
bool save(const Settings& settings);

}  // namespace settings
