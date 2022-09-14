#include "UI/ImageEditingViewController.hpp"

#include "Helpers/utilities.hpp"
#include "BSML/Animations/AnimationStateUpdater.hpp"
#include "UI/ImageFactoryFlowCoordinator.hpp"
#include "Utils/UIUtils.hpp"
#include "Utils/FileUtils.hpp"
#include "Utils/StringUtils.hpp"
#include "HMUI/Touchable.hpp"
#include "HMUI/Touchable.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "questui/shared/ArrayUtil.hpp"
#include "Presenters/PresenterManager.hpp"
#include "UnityEngine/Rect.hpp"
#include "UnityEngine/Resources.hpp"
#include "UnityEngine/WaitForSeconds.hpp"
#include "GlobalNamespace/SharedCoroutineStarter.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-functions.hpp"
#include "beatsaber-hook/shared/utils/typedefs-string.hpp"
#include "HMUI/ViewController_AnimationDirection.hpp"
#include "HMUI/ViewController_AnimationType.hpp"
#include "PluginConfig.hpp"
#include "Sprites.hpp"
#include "main.hpp"

DEFINE_TYPE(ImageFactory::UI, ImageEditingViewController);

using namespace GlobalNamespace;
using namespace UnityEngine::UI;
using namespace UnityEngine;
using namespace QuestUI;
using namespace HMUI;

#define StartCoroutine(method) GlobalNamespace::SharedCoroutineStarter::get_instance()->StartCoroutine(custom_types::Helpers::CoroutineHelper::New(method))

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

            auto bg = UIUtils::CreateHeader(BeatSaberUI::CreateHorizontalLayoutGroup(get_transform())->get_transform(), "New Image");

            auto listBar = BeatSaberUI::CreateHorizontalLayoutGroup(get_transform());
        
            auto listBarElement = listBar->GetComponent<LayoutElement*>();
            listBarElement->set_preferredWidth(20);
            listBarElement->set_preferredHeight(60);

            containerParent = listBar->get_transform();
    
            container = BeatSaberUI::CreateScrollableSettingsContainer(containerParent);

            StartCoroutine(SetupListElements(container->get_transform()));
        }
    }

    custom_types::Helpers::Coroutine ImageEditingViewController::SetupListElements(Transform* list){
        if (loadingControl) {
            loadingControl->refreshButton->get_gameObject()->SetActive(false);
            loadingControl->refreshText->get_gameObject()->SetActive(false);
        }
        
        GameObject::Destroy(loadingControl);
        
        GameObject* existingLoadinControl = Resources::FindObjectsOfTypeAll<LoadingControl*>()->values[0]->get_gameObject();
        GameObject* loadinControlGameObject = UnityEngine::GameObject::Instantiate(existingLoadinControl, get_transform());

        auto loadingControlTransform = loadinControlGameObject->get_transform();
        loadingControl = loadinControlGameObject->GetComponent<LoadingControl*>();
        loadingControl->get_gameObject()->SetActive(true);
        loadingControl->set_enabled(true);

        ConstString load = "Loading Images...";

        loadingControl->refreshText->set_text(load);
        loadingControl->loadingText->set_text(load);
        loadingControl->ShowLoading(load);

        list->get_gameObject()->set_active(false);

        StringW s = getPluginConfig().Images.GetValue();
        ConfigDocument& config = getPluginConfig().config->config;
        int i = 0;

        vector<std::string> split = StringUtils::split(static_cast<std::string>(s), '/');

        loadingControl->loadingText->set_text("Loading Images... (0/" + std::to_string(split.size()) + ")");

        while (!(i == split.size())) {
            StringW fileName = split.at(i);

            if (fileName->get_Length() != 0) {
                CreateListElement(list, false, nullptr, fileName);
            }

            i++;

            loadingControl->loadingText->set_text("Loading Images... (" + std::to_string(i - 1) + "/" + std::to_string(split.size() - 1) + ")");

            co_yield reinterpret_cast<System::Collections::IEnumerator*>(CRASH_UNLESS(WaitForSeconds::New_ctor(0.4f)));
        }

        co_yield reinterpret_cast<System::Collections::IEnumerator*>(CRASH_UNLESS(WaitForSeconds::New_ctor(0.15f)));

        if (i == 0) {   
            loadingControl->refreshText->get_gameObject()->SetActive(true);
            loadingControl->ShowText("No Images Found in Config!", false);
        } else {
            loadingControl->Hide();
        }

        list->get_gameObject()->SetActive(true);
    }

    void ImageEditingViewController::Refresh(IFImage* image) {
        CreateListElement(container->get_transform(), true, image, image->internalName);
    }

    void ImageEditingViewController::CreateListElement(UnityEngine::Transform* list, bool refresh, IFImage* ifimage, StringW fileName) {
        std::string path;
        std::string name;
        bool enabled;

        if (ifimage && refresh) {
            path = ifimage->path;
            enabled = ifimage->enabled;
            name = ifimage->name;
        } else {
            ConfigDocument& config = getPluginConfig().config->config;
            rapidjson::Value& configValue = config[static_cast<std::string>(fileName)];

            path = static_cast<std::string>(configValue["path"].GetString());
            name = static_cast<std::string>(configValue["name"].GetString());
            enabled = configValue["enabled"].GetBool();
        }

        HorizontalLayoutGroup* levelBarLayout = BeatSaberUI::CreateHorizontalLayoutGroup(list);
        levelBarLayout->set_childControlWidth(false);

        LayoutElement* levelBarLayoutElement = levelBarLayout->GetComponent<LayoutElement*>();
        levelBarLayoutElement->set_minHeight(10.0f);
        levelBarLayoutElement->set_minWidth(20.0f);

        Sprite* sprite = BeatSaberUI::FileToSprite(path);

        auto image = BeatSaberUI::CreateImage(levelBarLayoutElement->get_transform(), sprite, {2.0f, 0.0f}, {10.0f, 2.0f});

        SetPreferredSize(image, 10.0f, 2.0f);

        if (FileUtils::isGifFile(path)) {
            image->set_sprite(BeatSaberUI::Base64ToSprite(Blank));
            BSML::Utilities::SetImage(image, "file://" + path);
        }

        LayoutElement* imgElem = image->GetComponent<LayoutElement*>();
        imgElem->set_preferredHeight(2.0f);
        imgElem->set_preferredWidth(10.0f);

        TMPro::TextMeshProUGUI* text = BeatSaberUI::CreateText(levelBarLayoutElement->get_transform(), name, true);
        if (!enabled) {
            text->set_color(Color::get_red());
        } else {
            text->set_color(Color::get_green());
        }

        levelBarLayoutElement->set_minWidth(1.0f);
        levelBarLayoutElement->set_minHeight(1.0f);

        Button* deleteButton = BeatSaberUI::CreateUIButton(levelBarLayoutElement->get_transform(), "", {0.0f, 0.0f}, {12.0f, 9.0f}, 
            [=]() {
                for (std::pair<IFImage*, std::string> pair : *PresenterManager::MAP) {
                    if (pair.first->internalName.starts_with(fileName)) {
                        Config::Delete(pair.first);
                        levelBarLayout->get_gameObject()->set_active(false);
                        break;
                    }
                }
            });

        auto deleteText = BeatSaberUI::CreateText(
            deleteButton->get_transform(), "X");
        deleteText->set_alignment(TMPro::TextAlignmentOptions::Center);
        deleteText->set_color(Color(1.0f, 0.0f, 0.0f, 1.0f));

        Button* editButton = BeatSaberUI::CreateUIButton(levelBarLayoutElement->get_transform(), "", {0.0f, 0.0f}, {12.0f, 9.0f}, 
            [=]() {
                for (std::pair<IFImage*, std::string> pair : *PresenterManager::MAP) {
                    if (pair.first->internalName.starts_with(fileName)) {
                        ArrayUtil::First(Resources::FindObjectsOfTypeAll<ImageFactoryFlowCoordinator*>())->EditImage(pair.first);
                        break;
                    }
                }
            });

        auto editText = BeatSaberUI::CreateText(editButton->get_transform(), "<-");
        editText->set_alignment(TMPro::TextAlignmentOptions::Center);

        levelBarLayout->get_gameObject()->set_active(true);
    }
}