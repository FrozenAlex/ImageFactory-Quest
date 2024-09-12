#include "main.hpp"

#include "IFImage.hpp"
#include "PluginConfig.hpp"
#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "UI/ImageFactoryFlowCoordinator.hpp"
#include "paper/shared/logger.hpp"
#include "bsml/shared/BSML.hpp"
#include "logging.hpp"
using namespace ImageFactory;

// Loads the config from disk using our modInfo, then returns it for use
Configuration& getConfig() {
    static Configuration config(modInfo);
    config.Load();
    return config;
}

void ImageFactory::InstallHooks() {
    Hooks::LoadImagesHook();
    Hooks::MenuTransitionsHelper();
    Presenters::ComboHooks();
    Presenters::PercentHooks();
    Presenters::PresenterHooks();
    Presenters::LastNoteHooks();
    Presenters::ResultsHooks();
}

void makeFolders() {
    std::string dataFolder = IMAGE_FACTORY_PATH;

    if (!direxists(dataFolder)) {
        int result = mkpath(dataFolder);
        if (result == -1) {
            INFO("Failed creation of the main data folder!");
        }
    }

    if (!direxists(dataFolder + "Images/")) {
        int result = mkpath(dataFolder + "Images/");
        if (result == -1) {
            INFO("Failed creation of the images folder!");
        }
    }
}

// Called at the early stages of game loading
extern "C" __attribute__((visibility("default"))) void setup(CModInfo& info) {
    info.id = MOD_ID;
    info.version = VERSION;
    info.version_long = 0;
	
    getConfig().Load();
    getPluginConfig().Init(modInfo);
    getConfig().Reload();
    getConfig().Write();  // Load the config file
    INFO("Completed setup!");
}

// Called later on in the game loading - a good time to install function hooks
extern "C" __attribute__((visibility("default"))) void late_load() {
    il2cpp_functions::Init();
    custom_types::Register::AutoRegister();

    BSML::Init();

    makeFolders();

    BSML::Register::RegisterSettingsMenu<ImageFactory::UI::ImageFactoryFlowCoordinator*>(MOD_ID);

    INFO("Installing hooks...");
    InstallHooks();
    INFO("Installed all hooks!");
}