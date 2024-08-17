#include "UI/ImageFactoryFlowCoordinator.hpp"

#include "Presenters/PresenterManager.hpp"
#include "UI/ImageCreatorViewController.hpp"
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

            imageCreationViewController = reinterpret_cast<ImageCreationViewController*>(BSML::Helpers::CreateViewController<ImageCreationViewController*>());
            imageEditingViewController = reinterpret_cast<ImageEditingViewController*>(BSML::Helpers::CreateViewController<ImageEditingViewController*>());
            imageFactoryViewController = reinterpret_cast<ImageFactoryViewController*>(BSML::Helpers::CreateViewController<ImageFactoryViewController*>());
        
            ProvideInitialViewControllers(imageFactoryViewController, imageCreationViewController, imageEditingViewController, nullptr, nullptr);
        }             
    }

    void ImageFactoryFlowCoordinator::CreateImage(StringW path) {
        ImageCreatorViewController* viewController = reinterpret_cast<ImageCreatorViewController*>(BSML::Helpers::CreateViewController<ImageCreatorViewController*>());

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
        ImageCreatorViewController* viewController = reinterpret_cast<ImageCreatorViewController*>(BSML::Helpers::CreateViewController<ImageCreatorViewController*>());

        if (!viewController) return;

        viewController->InitializeEditor(image, text);

        PresenterManager::DespawnAll();
        
        SetLeftScreenViewController(BSML::Helpers::CreateViewController<HMUI::ViewController*>(), HMUI::ViewController::AnimationType::In);
        SetRightScreenViewController(BSML::Helpers::CreateViewController<HMUI::ViewController*>(), HMUI::ViewController::AnimationType::In);
        ReplaceTopViewController(viewController, this, this, nullptr, HMUI::ViewController::AnimationType::In, HMUI::ViewController::AnimationDirection::Horizontal);
    }
}

