#pragma once
#include "HMUI/FlowCoordinator.hpp"
#include "HMUI/ViewController.hpp"
#include "TMPro/TextMeshProUGUI.hpp"
#include "NewImageView.hpp"
#include "SavedImageView.hpp"
#include "InfoView.hpp"
#include "custom-types/shared/macros.hpp"
#include "IFImage.hpp"

DECLARE_CLASS_CODEGEN(ImageFactory::UI, ImageFactoryFlowCoordinator, HMUI::FlowCoordinator) {
    DECLARE_OVERRIDE_METHOD(void, DidActivate, il2cpp_utils::FindMethodUnsafe("HMUI", "FlowCoordinator", "DidActivate", 3), bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);
    DECLARE_OVERRIDE_METHOD(void, BackButtonWasPressed, il2cpp_utils::FindMethodUnsafe("HMUI", "FlowCoordinator", "BackButtonWasPressed", 1), HMUI::ViewController* topViewController);
    DECLARE_INSTANCE_FIELD_DEFAULT(ImageFactory::UI::NewImageView*, imageCreationViewController, nullptr);
    DECLARE_INSTANCE_FIELD_DEFAULT(ImageFactory::UI::SavedImageView*, imageEditingViewController, nullptr);
    DECLARE_INSTANCE_FIELD_DEFAULT(ImageFactory::UI::InfoView*, imageFactoryViewController, nullptr);
    DECLARE_INSTANCE_METHOD(void, CreateImage, StringW);
    DECLARE_INSTANCE_METHOD(void, ResetViews);
    DECLARE_INSTANCE_METHOD(void, EditImage, IFImage*, TMPro::TextMeshProUGUI*);
};

namespace ImageFactory {
    class Cache {
        public:
            static UI::ImageFactoryFlowCoordinator* flow; 
    };
}