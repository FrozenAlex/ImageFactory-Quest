#include "UI/EditImageView.hpp"

#include "UI/ImageFactoryFlowCoordinator.hpp"
#include "Utils/UIUtils.hpp"
#include "Presenters/PresenterManager.hpp"
#include "HMUI/CurvedCanvasSettingsHelper.hpp"
#include "HMUI/Touchable.hpp"
#include "UnityEngine/Rect.hpp"
#include "UnityEngine/RectOffset.hpp"
#include "UnityEngine/Graphics.hpp"
#include "UnityEngine/UI/ContentSizeFitter.hpp"
#include "ImageManager.hpp"
#include "logging.hpp"
#include "assets.hpp"


using namespace UnityEngine::UI;
using namespace UnityEngine;
using namespace HMUI;

#define SetPreferredSize(identifier, width, height)                                         \
    auto layout##identifier = identifier->get_gameObject()->GetComponent<LayoutElement*>(); \
    if (!layout##identifier)                                                                \
        layout##identifier = identifier->get_gameObject()->AddComponent<LayoutElement*>();  \
    layout##identifier->set_preferredWidth(width);                                          \
    layout##identifier->set_preferredHeight(height)

#define SetWidth(identifier, w)                                         \
    auto width##identifier = identifier->get_gameObject()->GetComponent<LayoutElement*>(); \
    if (!width##identifier)                                                                \
        width##identifier = identifier->get_gameObject()->AddComponent<LayoutElement*>();  \
    width##identifier->set_preferredWidth(w);   

#define SetFits(identifier, vert, horiz) \
    auto fitter##identifier = identifier->get_gameObject()->GetComponent<ContentSizeFitter*>(); \
    if (!fitter##identifier)                                                                    \
        fitter##identifier = identifier->get_gameObject()->AddComponent<ContentSizeFitter*>();  \
    fitter##identifier->set_verticalFit(vert);                                                  \
    fitter##identifier->set_horizontalFit(horiz);

DEFINE_TYPE(ImageFactory::UI, EditImageView);

namespace ImageFactory::UI {
    void EditImageView::DidActivate(bool firstActivation,
                                         bool addedToHierarchy,
                                         bool screenSystemEnabling) {
        if (firstActivation) {
            BSML::parse_and_construct(Assets::edit_image_view_bsml, this->get_transform(), this);
            
            #ifdef HotReload
                fileWatcher = get_gameObject()->AddComponent<HotReloadFileWatcher*>();
                fileWatcher->host = this;
                fileWatcher->filePath = "/sdcard/bsml/ImageFactory/edit-image-view.bsml";
                fileWatcher->checkInterval = 0.5f;
            #endif
            return;
            if (!get_gameObject()) return;


            get_gameObject()->AddComponent<Touchable*>();

            if (editing) {
                image->Spawn(false);
            }
            
            image->UpdateImage(true);

            VerticalLayoutGroup* vertical = BSML::Lite::CreateVerticalLayoutGroup(get_transform());
            SetFits(vertical, ContentSizeFitter::FitMode::PreferredSize, ContentSizeFitter::FitMode::PreferredSize);

            HorizontalLayoutGroup* horiz = BSML::Lite::CreateHorizontalLayoutGroup(vertical->get_transform());
            SetFits(horiz, ContentSizeFitter::FitMode::PreferredSize, ContentSizeFitter::FitMode::PreferredSize);
            SetPreferredSize(horiz, 94, 8);

            BSML::Lite::CreateStringSetting(horiz->get_transform(), "Name", image->name,
                [=](StringW s) { 
                    image->name = static_cast<std::string>(s);
            });

            
            VerticalLayoutGroup* settingsVert = BSML::Lite::CreateVerticalLayoutGroup(vertical->get_transform());
            SetFits(settingsVert, ContentSizeFitter::FitMode::PreferredSize, ContentSizeFitter::FitMode::PreferredSize);
            SetWidth(horiz, 90);
            
            BSML::Lite::CreateToggle(settingsVert->get_transform(), "Enabled", image->enabled, 
                [=](bool b) {
                    image->enabled = b;
                    image->UpdateImage(true);
            });

            BSML::Lite::CreateIncrementSetting(settingsVert->get_transform(), "Scale X", 2, 0.1f, image->scale.x,
                [=](float f) {
                    image->scale.x = f;
                    image->UpdateImage(true);
            });

            BSML::Lite::CreateIncrementSetting(settingsVert->get_transform(), "Scale Y", 2, 0.1f, image->scale.y,
                [=](float f) {
                    image->scale.y = f;
                    image->UpdateImage(true);
            });

            VerticalLayoutGroup* optionsVert = BSML::Lite::CreateVerticalLayoutGroup(vertical->get_transform());
            optionsVert->set_padding(RectOffset::New_ctor(-2, -20, -2, -2));
            optionsVert->set_childAlignment(UnityEngine::TextAnchor::MiddleCenter);

            SetFits(optionsVert, ContentSizeFitter::FitMode::PreferredSize, ContentSizeFitter::FitMode::PreferredSize);
            SetPreferredSize(optionsVert, 90, 34);

            Backgroundable* bg = optionsVert->get_gameObject()->AddComponent<Backgroundable*>();
            bg->ApplyBackground("round-rect-panel");
            bg->ApplyAlpha(0.8f);

            std::vector<string_view> presOptions;

            for (int i = 0; i < PresenterManager::PRESENTERS.size(); i++) {
                presOptions.push_back(PresenterManager::PRESENTERS.at(i));
            }

            VerticalLayoutGroup* list = BSML::Lite::CreateVerticalLayoutGroup(get_transform());
            list->get_rectTransform()->set_anchoredPosition({0.0f, -20.0f});
            list->set_spacing(2.0f);
            list->set_childForceExpandHeight(false);
            SetFits(list, ContentSizeFitter::FitMode::PreferredSize, ContentSizeFitter::FitMode::PreferredSize);
            SetPreferredSize(list, 85, 40);

            auto dropDown = BSML::Lite::CreateDropdown(list->get_transform(), "Presentation Options", image->presentationoption, presOptions,
                [this, list](StringW s) {
                    image->presentationoption = static_cast<std::string>(s);

                    ResetOptions(list->get_transform());

                });

            presenter = PresenterManager::GetPresenter(image->presentationoption);
            options = presenter->GetUIElements(list->get_transform(), image);

            ImageFactoryFlowCoordinator* flow = Object::FindObjectsOfType<ImageFactoryFlowCoordinator*>()->First();

            auto buttonGroup = BSML::Lite::CreateHorizontalLayoutGroup(vertical->get_transform());

            auto cancelButton = BSML::Lite::CreateUIButton(buttonGroup, "", {-0.0f, -0.0f}, {40.0f, 8.0f},
                [=]() {
                    flow->ResetViews();
            });

            BSML::Lite::CreateText(cancelButton->get_transform(), "CANCEL")->set_alignment(TMPro::TextAlignmentOptions::Center);

            auto saveButton = BSML::Lite::CreateUIButton(buttonGroup, "SAVE", {0.0f, -0.0f}, {40.0f, 8.0f},
                [this, flow]() {
                    DEBUG("Saving image: {}", image->name);
                    auto manager = ImageManager::get_instance();
                    GameObject* screen = image->screenGO;
                    auto localPosition = screen->get_transform()->get_localPosition();
                    auto rotation = screen->get_transform()->get_rotation();

                    image->position = localPosition;
                    image->rotation = rotation;

                    if (editing) {
                        manager->UpdateImage(image);
                        Object::Destroy(backUpImage);
                    } else {
                        manager->AddImage(image);
                        // TODO: Add the image to the config
                        manager->AddNewImage(image);
                    }
                    
                    PresenterManager::Parse(image, image->presentationoption);

                    hasSaved = true;

                    image->UpdateImage(false);
                    image->Despawn(false);

                    if (!editing) {
                        flow->imageEditingViewController->Refresh(image);
                    }

                    if (text) {
                        text->set_text(image->name);

                        if (!image->enabled) {
                            text->set_color(Color::get_red());
                        } else {
                            text->set_color(Color::get_green());
                        }
                    }
    
                    flow->ResetViews();
            });
            saveButton->set_interactable(true);
            // BSML::Lite::CreateText(saveButton->get_transform(), "SAVE")->set_alignment(TMPro::TextAlignmentOptions::Center);
        }        
    }

    void EditImageView::PostParse() {
        // if (editing) {
        //     image->UpdateImage(true);
        //     backUpImage->UpdateImage(true);
        // } else {
        //     image->UpdateImage(false);
        // }
    }

    void EditImageView::DidDeactivate(bool removedFromHierarchy, bool screenSystemEnabling) {
        if (image) { 
            if (editing) {
                image->Despawn(false);
                backUpImage->UpdateImage(false);
                backUpImage->Despawn(false);
                backUpImage->Spawn(false);
                PresenterManager::ClearInfo(image);
                PresenterManager::Parse(backUpImage, backUpImage->presentationoption);
                hasSaved = true;
            }

            if (!hasSaved) {
                PresenterManager::ClearInfo(image);
                image->Despawn(false);
                Object::Destroy(image);
            } else {

            }
        }

        PresenterManager::SpawnInMenu();
    }

    void EditImageView::ResetOptions(Transform* list) {
        for (int i = 0; i < options.size(); i++) {
            Object::Destroy(options.at(i));
        }
        
        delete presenter;

        presenter = PresenterManager::GetPresenter(image->presentationoption);
        options = presenter->GetUIElements(list, image);
    }

    void EditImageView::Initialize(StringW s) {
        path = s;

        GameObject* obj = GameObject::New_ctor(s);
        IFImage* image = obj->AddComponent<IFImage*>();
        image->ctor(BSML::Lite::FileToSprite(static_cast<std::string>(s)), s);

        this->image = image;

        editing = false;
        hasSaved = false;
    }

    void EditImageView::InitializeEditor(IFImage* image, TMPro::TextMeshProUGUI* text) {
        this->image = image;
        this->backUpImage = image;
        this->text = text;

        image->Spawn(false);

        editing = true;
        hasSaved = false;
    }
}