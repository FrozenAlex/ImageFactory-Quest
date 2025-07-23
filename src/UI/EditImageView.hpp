#pragma once

#include "IFImage.hpp"
#include "Presenters/Presenter.hpp"
#include "HMUI/ViewController.hpp"
#include "TMPro/TextMeshProUGUI.hpp"
#include "UnityEngine/Coroutine.hpp"
#include "custom-types/shared/coroutine.hpp"
#include "custom-types/shared/macros.hpp"
#include "bsml/shared/BSML/Components/HotReloadFileWatcher.hpp"

DECLARE_CLASS_CODEGEN(ImageFactory::UI, EditImageView, HMUI::ViewController) {
    #ifdef HotReload
    DECLARE_INSTANCE_FIELD(BSML::HotReloadFileWatcher*, fileWatcher);
    #endif
    
    DECLARE_INSTANCE_METHOD(void, PostParse);
    DECLARE_OVERRIDE_METHOD_MATCH(
        void, DidActivate, &HMUI::ViewController::DidActivate, bool firstActivation, bool addedToHeirarchy, bool screenSystemDisabling
    );
    DECLARE_OVERRIDE_METHOD_MATCH(
        void, DidDeactivate, &HMUI::ViewController::DidDeactivate, bool removedFromHierarchy, bool screenSystemEnabling
    );
    DECLARE_INSTANCE_FIELD(bool, hasSaved); 
    DECLARE_INSTANCE_FIELD(bool, editing); 
    DECLARE_INSTANCE_FIELD(IFImage*, image);
    DECLARE_INSTANCE_FIELD(IFImage*, backUpImage);
    DECLARE_INSTANCE_FIELD(TMPro::TextMeshProUGUI*, text);
    DECLARE_INSTANCE_METHOD(void, InitializeEditor, IFImage*, TMPro::TextMeshProUGUI*);

    public: 
        StringW path;
        std::vector<UnityEngine::GameObject*> options;
        Presenters::Presenter* presenter;
        
        void Initialize(StringW str);
        void ResetOptions(UnityEngine::Transform* parent);
        custom_types::Helpers::Coroutine UpdateImage();
};