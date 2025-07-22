#pragma once

#include "HMUI/ViewController.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/GameObject.hpp"
#include "bsml/shared/BSML/Components/Backgroundable.hpp"
#include "custom-types/shared/macros.hpp"
#include "custom-types/shared/coroutine.hpp"
#include "GlobalNamespace/LoadingControl.hpp"
#include "IFImage.hpp"
#include "PluginConfig.hpp"

DECLARE_CLASS_CODEGEN(ImageFactory::UI, ImageEditingViewController, HMUI::ViewController) {
    DECLARE_OVERRIDE_METHOD(void, DidActivate, il2cpp_utils::FindMethodUnsafe("HMUI", "ViewController", "DidActivate", 3), bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);
    DECLARE_INSTANCE_FIELD(GlobalNamespace::LoadingControl*, loadingControl);
    DECLARE_INSTANCE_FIELD(UnityEngine::Transform*, containerParent);
    DECLARE_INSTANCE_FIELD(UnityEngine::GameObject*, container);
    DECLARE_INSTANCE_METHOD(void, Refresh, IFImage*);
    DECLARE_INSTANCE_METHOD(void, ClearList);

    void CreateListElement(UnityW<UnityEngine::Transform> parent, bool isEditing, UnityW<IFImage> image, std::string filename);
    private:
        custom_types::Helpers::Coroutine SetupListElements(UnityEngine::Transform* parent);
        std::vector<UnityW<UnityEngine::GameObject>> elems;
};