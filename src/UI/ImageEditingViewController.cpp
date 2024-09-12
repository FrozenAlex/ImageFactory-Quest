#include "UI/ImageEditingViewController.hpp"

#include "bsml/shared/BSML/Animations/AnimationStateUpdater.hpp"
#include "UI/ImageFactoryFlowCoordinator.hpp"
#include "Utils/UIUtils.hpp"
#include "Utils/FileUtils.hpp"
#include "HMUI/Touchable.hpp"
#include "Presenters/PresenterManager.hpp"
#include "UnityEngine/Resources.hpp"
#include "UnityEngine/WaitForSeconds.hpp"
#include "bsml/shared/BSML/SharedCoroutineStarter.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-functions.hpp"
#include "beatsaber-hook/shared/utils/typedefs-string.hpp"
#include "HMUI/ViewController.hpp"
#include "PluginConfig.hpp"
#include "bsml/shared/BSML-Lite/Creation/Text.hpp"
#include "bsml/shared/BSML-Lite/Creation/Image.hpp"
#include "bsml/shared/BSML-Lite/Creation/Layout.hpp"
#include "ImageManager.hpp"
DEFINE_TYPE(ImageFactory::UI, ImageEditingViewController);

using namespace GlobalNamespace;
using namespace UnityEngine::UI;
using namespace UnityEngine;
using namespace HMUI;

#define SetPreferredSize(identifier, width, height)                                         \
    auto layout##identifier = identifier->get_gameObject()->GetComponent<LayoutElement*>(); \
    if (!layout##identifier)                                                                \
        layout##identifier = identifier->get_gameObject()->AddComponent<LayoutElement*>();  \
    layout##identifier->set_preferredWidth(width);                                          \
    layout##identifier->set_preferredHeight(height)

namespace ImageFactory::UI {

    void ImageEditingViewController::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
        if (firstActivation) 
        {
            if (!get_gameObject()) return;

            get_gameObject()->AddComponent<Touchable*>();

            auto bg = UIUtils::CreateHeader(BSML::Lite::CreateHorizontalLayoutGroup(get_transform())->get_transform(), "New Image");

            auto listBar = BSML::Lite::CreateHorizontalLayoutGroup(get_transform());
        
            auto listBarElement = listBar->GetComponent<LayoutElement*>();
            listBarElement->set_preferredWidth(20);
            listBarElement->set_preferredHeight(60);

            containerParent = listBar->get_transform();
    
            container = BSML::Lite::CreateScrollableSettingsContainer(containerParent);

            StartCoroutine(
                custom_types::Helpers::CoroutineHelper::New(
                    SetupListElements(container->get_transform())
                )
            );
        } else {
            if (loadingControl) {
                loadingControl->get_gameObject()->set_active(false);
            }
        }
    }

    custom_types::Helpers::Coroutine ImageEditingViewController::SetupListElements(Transform* list){
        if (loadingControl) {
            loadingControl->____refreshButton->get_gameObject()->SetActive(false);
            loadingControl->____refreshText->get_gameObject()->SetActive(false);
        }
        
        GameObject::Destroy(loadingControl);
        
        GameObject* existingLoadinControl = Resources::FindObjectsOfTypeAll<LoadingControl*>()->_values[0]->get_gameObject();
        GameObject* loadinControlGameObject = UnityEngine::GameObject::Instantiate(existingLoadinControl, get_transform());

        auto loadingControlTransform = loadinControlGameObject->get_transform();
        loadingControl = loadinControlGameObject->GetComponent<LoadingControl*>();
        loadingControl->get_gameObject()->SetActive(true);
        loadingControl->set_enabled(true);

        ConstString load = "Loading Images...";

        loadingControl->____refreshText->set_text(load);
        loadingControl->____loadingText->set_text(load);
        loadingControl->ShowLoading(load);

        list->get_gameObject()->set_active(false);

        auto imageConfigs = getPluginConfig().SaveData.GetValue();

        for (int i = 0; i < imageConfigs.size(); i++) {
            auto& imageConfig = imageConfigs.at(i);

            std::string fileName = imageConfig.LocalFilePath.value_or("");
            if (!fileName.empty()) {
                CreateListElement(list, false, nullptr, fileName);
            }

            loadingControl->_loadingText->set_text("Loading Images... (" + std::to_string(i) + "/" + std::to_string(imageConfigs.size() - 1) + ")");

            co_yield reinterpret_cast<System::Collections::IEnumerator*>(CRASH_UNLESS(WaitForSeconds::New_ctor(0.4f)));
        }

        co_yield reinterpret_cast<System::Collections::IEnumerator*>(CRASH_UNLESS(WaitForSeconds::New_ctor(0.15f)));

        if (elems.empty()) {
            loadingControl->_refreshText->get_gameObject()->SetActive(true);
            loadingControl->ShowText("No Images Found in Config!", false);
        } else {
            loadingControl->Hide();
        }

        list->get_gameObject()->SetActive(true);
    }

    void ImageEditingViewController::Refresh(IFImage* image) {
        CreateListElement(container->get_transform(), true, image, image->internalName);
    }

    void ImageEditingViewController::CreateListElement(UnityW<UnityEngine::Transform> list, bool refresh, UnityW<IFImage> ifimage, std::string fileName) {
        std::string path;
        std::string name;
        bool enabled;

        // If image exists, get its params for the item
        if (ifimage && refresh) {
            path = ifimage->path;
            enabled = ifimage->enabled;
            name = ifimage->name;
        } else {
            // TODO: Figure out what it does
            //ConfigDocument& config = getPluginConfig().config->config;
            //rapidjson::Value& configValue = config[static_cast<std::string>(fileName)];
            //
            //path = static_cast<std::string>(configValue["path"].GetString());
            //name = static_cast<std::string>(configValue["name"].GetString());
            //enabled = configValue["enabled"].GetBool();
        }

        HorizontalLayoutGroup* levelBarLayout = BSML::Lite::CreateHorizontalLayoutGroup(list);
        levelBarLayout->set_childControlWidth(false);

        LayoutElement* levelBarLayoutElement = levelBarLayout->GetComponent<LayoutElement*>();
        levelBarLayoutElement->set_minHeight(10.0f);
        levelBarLayoutElement->set_minWidth(20.0f);

        Sprite* sprite = BSML::Lite::FileToSprite(path);

        auto image = BSML::Lite::CreateImage(levelBarLayoutElement->get_transform(), sprite, {2.0f, 0.0f}, {10.0f, 2.0f});

        SetPreferredSize(image, 10.0f, 2.0f);

        if (FileUtils::isGifFile(path)) {
            image->set_sprite(UIUtils::FirstFrame(path));
        }

        LayoutElement* imgElem = image->GetComponent<LayoutElement*>();
        imgElem->set_preferredHeight(2.0f);
        imgElem->set_preferredWidth(10.0f);

        TMPro::TextMeshProUGUI* text = BSML::Lite::CreateText(levelBarLayoutElement->get_transform(), name, true);
        if (!enabled) {
            text->set_color(Color::get_red());
        } else {
            text->set_color(Color::get_green());
        }

        levelBarLayoutElement->set_minWidth(1.0f);
        levelBarLayoutElement->set_minHeight(1.0f);

        Button* deleteButton = BSML::Lite::CreateUIButton(levelBarLayoutElement->get_transform(), "", {0.0f, 0.0f}, {12.0f, 9.0f}, 
            [=]() {
                auto manager = ImageManager::get_instance();
                for (std::pair<IFImage*, std::string> pair : *PresenterManager::MAP) {
                    if (pair.first->internalName.starts_with(fileName)) {
                        manager->RemoveImage(pair.first);
                        levelBarLayout->get_gameObject()->set_active(false);
                        break;
                    }
                }
            });

        auto deleteText = BSML::Lite::CreateText(
            deleteButton->get_transform(), "X");
        deleteText->set_alignment(TMPro::TextAlignmentOptions::Center);
        deleteText->set_color(Color(1.0f, 0.0f, 0.0f, 1.0f));

        Button* editButton = BSML::Lite::CreateUIButton(levelBarLayoutElement->get_transform(), "", {0.0f, 0.0f}, {12.0f, 9.0f}, 
            [=]() {
                for (std::pair<IFImage*, std::string> pair : *PresenterManager::MAP) {
                    if (pair.first->internalName.starts_with(fileName)) {
                        Resources::FindObjectsOfTypeAll<ImageFactoryFlowCoordinator*>()->First()->EditImage(pair.first, text);
                        break;
                    }
                }
            });

        auto editText = BSML::Lite::CreateText(editButton->get_transform(), "<-");
        editText->set_alignment(TMPro::TextAlignmentOptions::Center);

        levelBarLayout->get_gameObject()->set_active(true);

        elems.push_back(levelBarLayout->get_gameObject());
    }

    void ImageEditingViewController::ClearList() {
        for (int i = 0; i < elems.size(); i++) {
            elems.at(i)->set_active(false);
        }
    }
}

