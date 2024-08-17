#include "main.hpp"

#include "Presenters/PresenterManager.hpp"
#include "GlobalNamespace/MainMenuViewController.hpp"
// #include "bsml/shared/BSML/SharedCoroutineStarter.hpp"
#include "PluginConfig.hpp"
#include "logging.hpp"

using namespace ImageFactory;
using namespace UnityEngine;
using namespace GlobalNamespace;


MAKE_HOOK_MATCH(MainMenuViewController_DidActivate, &GlobalNamespace::MainMenuViewController::DidActivate, void, GlobalNamespace::MainMenuViewController* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
    MainMenuViewController_DidActivate(self, firstActivation, addedToHierarchy, screenSystemEnabling);

    if (firstActivation) {
        self->StartCoroutine(
            custom_types::Helpers::new_coro(
                Config::LoadImages()
            )
        );
    }
}


void Hooks::LoadImagesHook() {
    INSTALL_HOOK(Logger, MainMenuViewController_DidActivate);
}