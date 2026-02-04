#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include "../Utils/Logger.h"

namespace Config {

    struct PetConfig {
        int idleTimeoutMs = 8000;
        float walkSpeed = 50.0f;
        float walkChance = 0.6f;
        float sleepChance = 0.1f;
        int followDesktopDelayMs = 150;
        int normalFps = 30;
        int idleFps = 10;
        int sleepFps = 2;
    };

    class ConfigLoader {
    public:
        static bool LoadPetConfig(const wchar_t* path, PetConfig& config) {
            std::ifstream file(path);
            if (!file.is_open()) {
                Utils::Logger::Log(L"Config: Could not open %s, using defaults", path);
                return false;
            }

            std::stringstream buffer;
            buffer << file.rdbuf();
            std::string json = buffer.str();

            config.idleTimeoutMs = ParseInt(json, "idleTimeoutMs", 8000);
            config.walkSpeed = ParseFloat(json, "walkSpeed", 50.0f);
            config.walkChance = ParseFloat(json, "walkChance", 0.6f);
            config.sleepChance = ParseFloat(json, "sleepChance", 0.1f);
            config.followDesktopDelayMs = ParseInt(json, "followDesktopDelayMs", 150);
            config.normalFps = ParseInt(json, "normalFps", 30);
            config.idleFps = ParseInt(json, "idleFps", 10);
            config.sleepFps = ParseInt(json, "sleepFps", 2);

            Utils::Logger::Log(L"Config: Loaded successfully");
            return true;
        }

    private:
        static int ParseInt(const std::string& json, const std::string& key, int defaultVal) {
            size_t pos = json.find("\"" + key + "\"");
            if (pos == std::string::npos) return defaultVal;
            
            pos = json.find(":", pos);
            if (pos == std::string::npos) return defaultVal;
            
            pos++; // Skip ':'
            while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\t')) pos++;
            
            try {
                return std::stoi(json.substr(pos));
            } catch (...) {
                return defaultVal;
            }
        }

        static float ParseFloat(const std::string& json, const std::string& key, float defaultVal) {
            size_t pos = json.find("\"" + key + "\"");
            if (pos == std::string::npos) return defaultVal;
            
            pos = json.find(":", pos);
            if (pos == std::string::npos) return defaultVal;
            
            pos++; // Skip ':'
            while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\t')) pos++;
            
            try {
                return std::stof(json.substr(pos));
            } catch (...) {
                return defaultVal;
            }
        }
    };
}
