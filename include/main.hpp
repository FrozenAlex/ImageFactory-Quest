#pragma once

#include "paper/shared/logger.hpp"

// Include the modloader header, which allows us to tell the modloader which mod this is, and the version etc.
#include "modloader/shared/modloader.hpp"

// beatsaber-hook is a modding framework that lets us call functions and fetch field values from in the game
// It also allows creating objects, configuration, and importantly, hooking methods to modify their values
#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/config/config-utils.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-functions.hpp"
#include "beatsaber-hook/shared/utils/hooking.hpp"

// Define these functions here so that we can easily read configuration and log information from other files
Configuration& getConfig();
Logger& getLogger();  

#define INFO(...) Paper::Logger::fmtLog<Paper::LogLevel::INF>(__VA_ARGS__)
#define ERROR(...) Paper::Logger::fmtLog<Paper::LogLevel::ERR>(__VA_ARGS__)
#define CRITICAL(...) Paper::Logger::fmtLog<Paper::LogLevel::ERR>(__VA_ARGS__)
#define DEBUG(...) Paper::Logger::fmtLog<Paper::LogLevel::DBG>(__VA_ARGS__)
#define WARNING(...) Paper::Logger::fmtLog<Paper::LogLevel::WRN>(__VA_ARGS__)

namespace ImageFactory {    
    namespace Hooks {
        void MenuTransitionsHelper();
        void LoadImagesHook();
    }

    namespace Presenters {
        void ComboHooks();
        void PercentHooks();
        void LastNoteHooks();
        void PresenterHooks();
        void ResultsHooks();
    }

    void InstallHooks();
}