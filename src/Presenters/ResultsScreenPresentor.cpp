#include "Presenters/PresenterManager.hpp"
#include "Presenters/ResultsScreenPresenter.hpp"
#include "GlobalNamespace/ResultsViewController.hpp"
#include "GlobalNamespace/LevelCompletionResults.hpp"
#include "logging.hpp"
#include "bsml/shared/BSML-Lite/Creation/Layout.hpp"
#include "GlobalNamespace/ResultsViewController.hpp"
#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "Hooks.hpp"

using namespace GlobalNamespace;



namespace ImageFactory::Presenters {

    std::vector<std::string_view> buttonOptions = {
        "Finished", "Passed", "Failed"
    };

    std::vector<GameObject*> ResultsScreenPresenter::GetUIElements(Transform* parent, IFImage* image) {
        std::vector<GameObject*> ret;

        
        auto dropDown = BSML::Lite::CreateDropdown(parent, "When", image->GetExtraData("results_when", "Finished"), buttonOptions, 
            [=](StringW s){
                image->SetExtraData("results_when", s);  
            });

        ret.push_back(dropDown->get_transform()->get_parent()->get_gameObject());

        return ret;
    }

    MAKE_HOOK_MATCH(ResultsViewController_DidActivate, &ResultsViewController::DidActivate, void, ResultsViewController* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
        ResultsViewController_DidActivate(self, firstActivation, addedToHierarchy, screenSystemEnabling);

        if (self->____levelCompletionResults->___energy <= 0) {
            PresenterManager::SpawnExtraData(PresenterManager::RESULTS_SCREEN, "results_when", "Failed");
        } else {
            PresenterManager::SpawnExtraData(PresenterManager::RESULTS_SCREEN, "results_when", "Passed");
        }

        INFO("ENERGY {}", self->____levelCompletionResults->___energy);
        
        PresenterManager::SpawnExtraData(PresenterManager::RESULTS_SCREEN, "results_when", "Finished");
    }

    MAKE_HOOK_MATCH(ResultsViewController_DidDeactivate, &ResultsViewController::DidDeactivate, void, ResultsViewController* self, bool addedToHierarchy, bool screenSystemEnabling) {
        ResultsViewController_DidDeactivate(self, addedToHierarchy, screenSystemEnabling);
        
        PresenterManager::DespawnforAll(PresenterManager::RESULTS_SCREEN);
    }

    void ResultsHooks() {
        INSTALL_HOOK(Logger, ResultsViewController_DidActivate);
        INSTALL_HOOK(Logger, ResultsViewController_DidDeactivate);
    }
}