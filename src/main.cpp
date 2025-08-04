#include "main.hpp"

#include "PluginConfig.hpp"
#include "UI/ImageFactoryFlowCoordinator.hpp"
#include "logging.hpp"
#include "bsml/shared/BSML.hpp"
#include "logging.hpp"
#include "Hooks.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-functions.hpp"

using namespace ImageFactory;

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
extern "C" __attribute__((visibility("default"))) void setup(CModInfo& info)  {
    info.id = MOD_ID;
    info.version = VERSION;
    info.version_long = 0;
	
    getPluginConfig().Init(modInfo);
    INFO("Completed setup!");
}

// Called later on in the game loading - a good time to install function hooks
extern "C" __attribute__((visibility("default"))) void late_load()  {
    il2cpp_functions::Init();
    custom_types::Register::AutoRegister();

    BSML::Init();

    makeFolders();

    BSML::Register::RegisterSettingsMenu<ImageFactory::UI::ImageFactoryFlowCoordinator*>(MOD_ID);
    BSML::Register::RegisterMainMenu<ImageFactory::UI::ImageFactoryFlowCoordinator*>(MOD_ID, "Place images around the game!");

    INFO("Installing hooks...");
    InstallHooks();
    INFO("Installed all hooks!");
}