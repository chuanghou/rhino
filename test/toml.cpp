
#include "gtest/gtest.h"
#include "toml++/toml.hpp"

TEST(Toml_Test, parse) {
    std::ifstream config_file("config/dog_roster.conf");

    if (!config_file.is_open()) {
        std::cerr << "Failed to open config file!" << std::endl;
    }                     
}