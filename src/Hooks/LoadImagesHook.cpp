#include "main.hpp"

#include "Presenters/PresenterManager.hpp"
#include "GlobalNamespace/MainMenuViewController.hpp"
#include "logging.hpp"
#include "ImageManager.hpp"

using namespace ImageFactory;
using namespace UnityEngine;
using namespace GlobalNamespace;


MAKE_HOOK_MATCH(MainMenuViewController_DidActivate, &GlobalNamespace::MainMenuViewController::DidActivate, void, GlobalNamespace::MainMenuViewController* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
    MainMenuViewController_DidActivate(self, firstActivation, addedToHierarchy, screenSystemEnabling);

    if (firstActivation) {

        auto manager = ImageManager::get_instance();
        self->StartCoroutine(
            custom_types::Helpers::new_coro(
                manager->LoadImages()
            )
        );
    }
}


void Hooks::LoadImagesHook() {
    INSTALL_HOOK(Logger, MainMenuViewController_DidActivate);
}