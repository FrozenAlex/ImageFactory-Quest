#include "GlobalNamespace/MainMenuViewController.hpp"
#include "logging.hpp"
#include "ImageManager.hpp"
#include "Hooks.hpp"
#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "custom-types/shared/coroutine.hpp"
#include "UnityEngine/WaitForSeconds.hpp"
#include "bsml/shared/Helpers/getters.hpp"
#include "bsml/shared/Helpers/creation.hpp"
#include "UI/ImageFactoryFlowCoordinator.hpp"

using namespace ImageFactory;
using namespace UnityEngine;
using namespace GlobalNamespace;


custom_types::Helpers::Coroutine Debug() {
    co_yield reinterpret_cast<System::Collections::IEnumerator*>(UnityEngine::WaitForSeconds::New_ctor(3.0f));

    // Open the ImageFactory menu
    auto flow = BSML::Helpers::CreateFlowCoordinator<ImageFactory::UI::ImageFactoryFlowCoordinator*>();
    auto parentFlow = BSML::Helpers::GetMainFlowCoordinator()->YoungestChildFlowCoordinatorOrSelf();
    parentFlow->PresentFlowCoordinator(flow, nullptr, HMUI::ViewController::AnimationDirection::Horizontal, false, false);
};


MAKE_HOOK_MATCH(MainMenuViewController_DidActivate, &GlobalNamespace::MainMenuViewController::DidActivate, void, GlobalNamespace::MainMenuViewController* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
    MainMenuViewController_DidActivate(self, firstActivation, addedToHierarchy, screenSystemEnabling);

    if (firstActivation) {

        auto manager = ImageManager::get_instance();
        self->StartCoroutine(
            custom_types::Helpers::new_coro(
                manager->LoadImages()
            )
        );

        // Debug
        self->StartCoroutine(custom_types::Helpers::new_coro(Debug()));
    }
}




void Hooks::LoadImagesHook() {
    INSTALL_HOOK(Logger, MainMenuViewController_DidActivate);
}