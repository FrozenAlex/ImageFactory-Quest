#include "UI/ImageFactoryFlowCoordinator.hpp"

#include "Presenters/PresenterManager.hpp"
#include "UI/EditImageView.hpp"
#include "HMUI/ViewController.hpp"
#include "bsml/shared/Helpers/creation.hpp"

DEFINE_TYPE(ImageFactory::UI, ImageFactoryFlowCoordinator);

using namespace HMUI;


namespace ImageFactory::UI {

    void ImageFactoryFlowCoordinator::DidActivate(bool firstActivation,
                                              bool addedToHierarchy,
                                              bool screenSystemEnabling) {
        if (firstActivation) {
            SetTitle("IMAGEFACTORY", ViewController::AnimationType::In);
            showBackButton = true;

            imageCreationViewController = reinterpret_cast<NewImageView*>(BSML::Helpers::CreateViewController<NewImageView*>());
            imageEditingViewController = reinterpret_cast<SavedImageView*>(BSML::Helpers::CreateViewController<SavedImageView*>());
            imageFactoryViewController = reinterpret_cast<InfoView*>(BSML::Helpers::CreateViewController<InfoView*>());
        
            ProvideInitialViewControllers(imageFactoryViewController, imageCreationViewController, imageEditingViewController, nullptr, nullptr);
        }             
    }

    void ImageFactoryFlowCoordinator::CreateImage(StringW path) {
        EditImageView* viewController = reinterpret_cast<EditImageView*>(BSML::Helpers::CreateViewController<EditImageView*>());

        if (!viewController) return;

        viewController->Initialize(path);

        PresenterManager::DespawnAll();
        
        SetLeftScreenViewController(BSML::Helpers::CreateViewController<HMUI::ViewController*>(), HMUI::ViewController::AnimationType::In);
        SetRightScreenViewController(BSML::Helpers::CreateViewController<HMUI::ViewController*>(), HMUI::ViewController::AnimationType::In);
        ReplaceTopViewController(viewController, this, this, nullptr, HMUI::ViewController::AnimationType::In, HMUI::ViewController::AnimationDirection::Horizontal);
    }

    void ImageFactoryFlowCoordinator::BackButtonWasPressed(HMUI::ViewController* topView) {
        _parentFlowCoordinator->DismissFlowCoordinator(this, HMUI::ViewController::AnimationDirection::Horizontal, nullptr, false);
    }

    void ImageFactoryFlowCoordinator::ResetViews() {
        ReplaceTopViewController(imageFactoryViewController, this, this, nullptr, HMUI::ViewController::AnimationType::In, HMUI::ViewController::AnimationDirection::Horizontal);
        SetLeftScreenViewController(imageCreationViewController, HMUI::ViewController::AnimationType::In);
        SetRightScreenViewController(imageEditingViewController, HMUI::ViewController::AnimationType::In);
    }

    void ImageFactoryFlowCoordinator::EditImage(IFImage* image, TMPro::TextMeshProUGUI* text) {
        EditImageView* viewController = reinterpret_cast<EditImageView*>(BSML::Helpers::CreateViewController<EditImageView*>());

        if (!viewController) return;

        viewController->InitializeEditor(image, text);

        PresenterManager::DespawnAll();
        
        SetLeftScreenViewController(BSML::Helpers::CreateViewController<HMUI::ViewController*>(), HMUI::ViewController::AnimationType::In);
        SetRightScreenViewController(BSML::Helpers::CreateViewController<HMUI::ViewController*>(), HMUI::ViewController::AnimationType::In);
        ReplaceTopViewController(viewController, this, this, nullptr, HMUI::ViewController::AnimationType::In, HMUI::ViewController::AnimationDirection::Horizontal);
    }
}

