#pragma once

#include <memory>
#include "bsml/shared/macros.hpp"
#include "HMUI/ViewController.hpp"
#include "custom-types/shared/macros.hpp"
#include "bsml/shared/BSML/Components/HotReloadFileWatcher.hpp"
#include "bsml/shared/BSML/Components/ClickableImage.hpp"
#include "bsml/shared/BSML/Components/ModalView.hpp"
#include "HMUI/CurvedTextMeshPro.hpp"
#include "UnityEngine/UI/Button.hpp"

DECLARE_CLASS_CODEGEN(ImageFactory::UI, InfoView, HMUI::ViewController) {
    #ifdef HotReload
    DECLARE_INSTANCE_FIELD(BSML::HotReloadFileWatcher*, fileWatcher);
    #endif

    DECLARE_INSTANCE_METHOD(void, PostParse);
    
    DECLARE_INSTANCE_METHOD(void, DismissModal);

    // Sprites for the images (to not load them from file every time)
    DECLARE_INSTANCE_FIELD(UnityW<UnityEngine::Sprite>, logoSprite);
    DECLARE_INSTANCE_FIELD(UnityW<UnityEngine::Sprite>, settingsSprite);
    DECLARE_INSTANCE_FIELD(UnityW<UnityEngine::Sprite>, resetSprite);
    DECLARE_INSTANCE_FIELD(UnityW<UnityEngine::Sprite>, helpSprite);
    DECLARE_INSTANCE_FIELD(UnityW<UnityEngine::Sprite>, githubSprite);
    DECLARE_INSTANCE_FIELD(UnityW<UnityEngine::Sprite>, optimusSprite);
    DECLARE_INSTANCE_FIELD(UnityW<UnityEngine::Sprite>, bandootSprite);
    

    // Clickable images for the buttons
    DECLARE_INSTANCE_FIELD(UnityW<HMUI::ImageView>, logoImage);
    DECLARE_INSTANCE_FIELD(UnityW<BSML::ClickableImage>, settingsImage);
    DECLARE_INSTANCE_FIELD(UnityW<BSML::ClickableImage>, resetImage);
    DECLARE_INSTANCE_FIELD(UnityW<BSML::ClickableImage>, helpImage);
    DECLARE_INSTANCE_FIELD(UnityW<BSML::ClickableImage>, githubImage);
    DECLARE_INSTANCE_FIELD(UnityW<BSML::ClickableImage>, optimusImage);
    DECLARE_INSTANCE_FIELD(UnityW<BSML::ClickableImage>, bandootImage);

    DECLARE_INSTANCE_FIELD(bool, isModalOpen);
    DECLARE_INSTANCE_FIELD(bool, isSettingsOpen);

    DECLARE_INSTANCE_METHOD(void, ClickedGitHub);
    DECLARE_INSTANCE_METHOD(void, ClickedOptimus);
    DECLARE_INSTANCE_METHOD(void, ClickedBandoot);
    DECLARE_INSTANCE_METHOD(void, ClickedHelp);
    DECLARE_INSTANCE_METHOD(void, ClickedReset);
    DECLARE_INSTANCE_METHOD(void, ClickedModalButton);
    DECLARE_INSTANCE_METHOD(void, OpenSettings);
    DECLARE_INSTANCE_METHOD(void, CloseSettings);
    

    DECLARE_INSTANCE_FIELD(UnityW<BSML::ModalView>, modal);
    DECLARE_INSTANCE_FIELD(UnityW<HMUI::ImageView>, modalImage);
    DECLARE_INSTANCE_FIELD(UnityW<HMUI::CurvedTextMeshPro>, modalTitle);
    DECLARE_INSTANCE_FIELD(UnityW<HMUI::CurvedTextMeshPro>, modalText);
    DECLARE_INSTANCE_FIELD(UnityW<UnityEngine::UI::Button>, modalActionButton);

    DECLARE_INSTANCE_FIELD(UnityW<BSML::ModalView>, settingsModal);
    DECLARE_BSML_PROPERTY(bool, settingsEnabled);
    DECLARE_BSML_PROPERTY(bool, settingsAnimationsEnabled);
    DECLARE_BSML_PROPERTY(bool, settingsIgnoreNoText);
    
    struct ClickableSubIcon {
        std::string name;
        std::string text;
        std::string buttonText;
        SafePtrUnity<UnityEngine::Sprite> icon;
        std::function<void(UnityW<ImageFactory::UI::InfoView>)> onClick;
    };

    DECLARE_OVERRIDE_METHOD_MATCH(
        void, DidActivate, &HMUI::ViewController::DidActivate, bool firstActivation, bool addedToHeirarchy, bool screenSystemDisabling
    );
    private:
        void ShowModal(std::shared_ptr<ClickableSubIcon> icon);
        std::shared_ptr<ClickableSubIcon> _resetSubIcon;
        std::shared_ptr<ClickableSubIcon> _helpSubIcon;
        std::shared_ptr<ClickableSubIcon> _githubSubIcon;
        std::shared_ptr<ClickableSubIcon> _optimusSubIcon;
        std::shared_ptr<ClickableSubIcon> _bandootSubIcon;
        std::optional<std::shared_ptr<ClickableSubIcon>> _currentModal;
};

