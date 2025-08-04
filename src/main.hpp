#pragma once

#include "logging.hpp"

// Include the modloader header, which allows us to tell the modloader which mod this is, and the version etc.
#include "scotland2/shared/loader.hpp"

// beatsaber-hook is a modding framework that lets us call functions and fetch field values from in the game
// It also allows creating objects, configuration, and importantly, hooking methods to modify their values

#include "beatsaber-hook/shared/config/config-utils.hpp"


inline modloader::ModInfo modInfo = {MOD_ID, VERSION, 0}; // Stores the ID and version of our mod, and is sent to the modloader upon startup

// Define these functions here so that we can easily read configuration and log information from other files
Configuration& getConfig();

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