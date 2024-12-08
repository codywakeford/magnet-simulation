#ifndef CONFIG_HPP
#define CONFIG_HPP 

#include <unordered_map>
#include <string>
#include <nlohmann/json.hpp>
#include <any>
#include <fstream>
#include <string>
#include <iostream>
#include <mutex>
#include <filesystem>

using json = nlohmann::json;

/*
Allows live control of globals in the liveconfig.json

*/
class Config {
public:
    const std::string filePath = "liveconfig.json";
    const int FPS = 165;
    const float FRAME_TIME = 1.0f / FPS;
    const float vacuumPermeativity = 8.8e-12;
    const float gravitational_constant = 5.0f;
    
    // for collision dampening
    const float minDistance = 50.0f;
    const float dampingFactor = 0.2f;
    //////////////////////////////////


    // int* testValue = std::any_cast<int>(&liveMap["testValue"]);

    std::unordered_map<std::string, std::any> liveMap = {
        { "testValue", 10 },
    };
    

    void loadConfig() {
        std::ifstream configFile(filePath);
        if (!configFile) {
            std::cerr << "[Config] Could not open configuration file!" << std::endl;
            return;
        }

        try {
            json jsonData;
            configFile >> jsonData;

            // Log the JSON configuration
            std::cout << "[Config] JSON Configuration Contents: " << std::endl;
            std::cout << jsonData.dump(4) << std::endl;

            {
                std::lock_guard<std::mutex> lock(configMutex);
                for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
                    const std::string& key = it.key();
                    const auto& value = it.value();

                    if (value.is_number()) {
                        liveMap[key] = std::any(value.get<int>());
                        std::cout << "[Config] Updated key: " << key << " with int value: " << value.get<int>() << std::endl;
                    } else if (value.is_boolean()) {
                        liveMap[key] = std::any(value.get<bool>());
                        std::cout << "[Config] Updated key: " << key << " with bool value: " << value.get<bool>() << std::endl;
                    } else if (value.is_string()) {
                        liveMap[key] = std::any(value.get<std::string>());
                        std::cout << "[Config] Updated key: " << key << " with string value: " << value.get<std::string>() << std::endl;
                    }
                }
                lastModifiedTime = std::filesystem::last_write_time(filePath);
            }
        } catch (const std::exception &e) {
            std::cerr << "[Config] Error reading config file: " << e.what() << std::endl;
        }
    }

    void checkForUpdates() {
        auto currentModifiedTime = std::filesystem::last_write_time(filePath);

        {
            std::lock_guard<std::mutex> lock(configMutex);
            if (currentModifiedTime != lastModifiedTime) {
                std::cout << "[Config] Detected change in configuration file. Reloading..." << std::endl;
                lastModifiedTime = currentModifiedTime;
            } else {
                return;
            }
        }

        // Call loadConfig() safely outside the mutex
        loadConfig();
    }


private:
    std::mutex configMutex;
    nlohmann::json jsonData;
    std::filesystem::file_time_type lastModifiedTime;


};

extern Config config;

#endif