#include "UI/ImageCreationViewController.hpp"
#include "UI/ImageFactoryFlowCoordinator.hpp"
#include "logging.hpp"
#include "Utils/UIUtils.hpp"
#include "Utils/FileUtils.hpp"
#include "Utils/StringUtils.hpp"
#include "HMUI/Touchable.hpp"
#include "System/IO/FileStream.hpp"
#include "System/IO/FileMode.hpp"
#include "System/Diagnostics/Stopwatch.hpp"
#include "UnityEngine/Rect.hpp"
#include "UnityEngine/Resources.hpp"
#include "UnityEngine/WaitForSeconds.hpp"
#include "UnityEngine/HideFlags.hpp"
#include "bsml/shared/BSML/SharedCoroutineStarter.hpp"
#include "bsml/shared/BSML/MainThreadScheduler.hpp"
#include "bsml/shared/BSML-Lite/Creation/Text.hpp"
#include "bsml/shared/BSML-Lite/Creation/Image.hpp"
#include "bsml/shared/BSML-Lite/Creation/Buttons.hpp"
#include "bsml/shared/BSML-Lite/Creation/Layout.hpp"
#include "custom-types/shared/delegate.hpp"
#include "bsml/shared/Helpers/utilities.hpp"
DEFINE_TYPE(ImageFactory::UI, ImageCreationViewController);

using namespace GlobalNamespace;
using namespace UnityEngine::UI;
using namespace UnityEngine;
using namespace System;
using namespace HMUI;

#define SetPreferredSize(identifier, width, height)                                         \
    auto layout##identifier = identifier->get_gameObject()->GetComponent<LayoutElement*>(); \
    if (!layout##identifier)                                                                \
        layout##identifier = identifier->get_gameObject()->AddComponent<LayoutElement*>();  \
    layout##identifier->set_preferredWidth(width);                                          \
    layout##identifier->set_preferredHeight(height)

namespace ImageFactory::UI {

    void ImageCreationViewController::DidActivate(bool firstActivation,
                                              bool addedToHierarchy,
                                              bool screenSystemEnabling) {
        if (firstActivation) 
        {
            if (!get_gameObject()) return;

            get_gameObject()->AddComponent<Touchable*>();

            auto bg = UIUtils::CreateHeader(BSML::Lite::CreateHorizontalLayoutGroup(get_transform())->get_transform(), "New Image");

            auto listBar = BSML::Lite::CreateHorizontalLayoutGroup(get_transform());
           
            auto listBarElement = listBar->GetComponent<LayoutElement*>();
            listBarElement->set_preferredWidth(20);
            listBarElement->set_preferredHeight(60);

            GameObject* container = BSML::Lite::CreateScrollableSettingsContainer(listBar->get_transform());

            this->StartCoroutine(
                custom_types::Helpers::CoroutineHelper::New(
                    SetupListElements(container->get_transform())
                )
            );
        }
    }

    custom_types::Helpers::Coroutine ImageCreationViewController::SetupListElements(Transform* list){
        std::vector<std::string> images = FileUtils::getFiles("/sdcard/ModData/com.beatgames.beatsaber/Mods/ImageFactory/Images/");

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

        Diagnostics::Stopwatch* watch = Diagnostics::Stopwatch::New_ctor();

        list->get_gameObject()->set_active(false);

        for (int i = 0; i < images.size(); i++) {
            watch->Reset();
            watch->Start();

            auto image = images.at(i);

            HorizontalLayoutGroup* levelBarLayout = BSML::Lite::CreateHorizontalLayoutGroup(list->get_transform());

            levelBarLayout->set_childControlWidth(false);

            LayoutElement* levelBarLayoutElement = levelBarLayout->GetComponent<LayoutElement*>();
            levelBarLayoutElement->set_minHeight(10.0f);
            levelBarLayoutElement->set_minWidth(20.0f);



            SafePtrUnity<Sprite> sprite = nullptr;
            if (FileUtils::isGifFile(image)) {
                sprite = UIUtils::FirstFrame(image);
            } else {
                sprite = BSML::Lite::FileToSprite(image);
            }
            if (!sprite) {
                DEBUG("Failed to load sprite from image: {}", image);
                continue;
            }
            sprite->set_hideFlags(HideFlags::None);
            Object::DontDestroyOnLoad(sprite.ptr());
        
            co_yield reinterpret_cast<Collections::IEnumerator*>(CRASH_UNLESS(WaitForSeconds::New_ctor(0.3f)));

            auto img = BSML::Lite::CreateImage(levelBarLayoutElement->get_transform(), sprite.ptr(), Vector2(2.0f, 0.0f), Vector2(10.0f, 2.0f));

            SetPreferredSize(img, 10.0f, 2.0f);

            System::IO::FileStream* stream = System::IO::FileStream::New_ctor(image, System::IO::FileMode::Open, ::System::IO::FileAccess::Read);

            long fileSize = stream->get_Length();
            auto loadTime = watch->get_ElapsedMilliseconds();

            BSML::Lite::CreateText(levelBarLayoutElement->get_transform(), FileUtils::GetFileName(image, false), true);
           
            levelBarLayoutElement->set_minWidth(1.0f);

            auto button = BSML::Lite::CreateUIButton(levelBarLayoutElement->get_transform(), "", Vector2(0.0f, 0.0f), Vector2(10.0f, 10.0f),
                [this, image, img, fileSize, loadTime]() {
                    auto modal = BSML::Lite::CreateModal(get_transform(), Vector2(70.0f, 50.0f),
                        nullptr, true);

                    auto container = BSML::Lite::CreateScrollableModalContainer(modal);

                    auto imgModal = BSML::Lite::CreateImage(modal->get_transform(), img->get_sprite(), Vector2(-18.0f, 8.0f),
                        Vector2(30.0f, 30.0f));

                    auto anim = BSML::Lite::CreateText(modal->get_transform(), "Animated: No",
                        Vector2(30.0f, 17.0f));

                    if (FileUtils::isGifFile(image)) {
                        anim->set_text("Animated: Yes");
                    }

                    anim->set_fontSize(5.0f);

                    BSML::Lite::CreateText(modal->get_transform(), "Width: " +
                        StringUtils::removeTrailingZeros(ceil(img->get_sprite()->get_textureRect().get_width())) + "px", Vector2(30.0f, 11.0f))->set_fontSize(5.0f);
                    BSML::Lite::CreateText(modal->get_transform(),  "Height: " +
                        StringUtils::removeTrailingZeros(ceil(img->get_sprite()->get_textureRect().get_height())) + "px", Vector2(30.0f, 5.0f))->set_fontSize(5.0f);

                    BSML::Lite::CreateText(modal->get_transform(), "File Size: " +
                        StringUtils::removeTrailingZeros(round(fileSize / FileUtils::FileSizeDivisor(fileSize))) +
                        " " + FileUtils::FileSizeExtension(fileSize), Vector2(30.0f, -1.0f))->set_fontSize(5.0f);

                    BSML::Lite::CreateText(modal->get_transform(), "Load Time: " + std::to_string(loadTime) + " ms", Vector2(30.0f, -7.0f))->set_fontSize(5.0f);
                    
                    auto create = BSML::Lite::CreateUIButton(modal->get_transform(), "CREATE", Vector2(14.0f, -17.0f),
                        Vector2(30.0f, 10.0f), [=]() { 
                            ImageFactoryFlowCoordinator* flow = Object::FindObjectsOfType<ImageFactoryFlowCoordinator*>()->First();

                            if (flow) {
                                flow->CreateImage(image);
                            }
                        });

                    BSML::Lite::CreateUIButton(modal->get_transform(), "CANCEL", Vector2(-18.0f, -17.0f),
                        Vector2(30.0f, 10.0f), [=]() { 
                            modal->Hide();
                        });

                    if (FileUtils::isGifFile(image)) {
                        create->set_interactable(false);
                        imgModal->set_sprite(UIUtils::FirstFrame(image));
                        BSML::Utilities::SetImage(imgModal, "file://" + image, false, BSML::Utilities::ScaleOptions(), [=](){
                            create->set_interactable(true);
                        });
                    }

                    modal->Show();
                });

            BSML::Lite::CreateText(button->get_transform(), "+")->set_alignment(TMPro::TextAlignmentOptions::Center);

            co_yield reinterpret_cast<Collections::IEnumerator*>(CRASH_UNLESS(WaitForSeconds::New_ctor(0.1f)));

            loadingControl->____loadingText->set_text("Loading Images... (" + std::to_string(i + 1) + "/" + std::to_string(images.size()) + ")");
        }

        // watch->Stop();

        co_yield reinterpret_cast<Collections::IEnumerator*>(CRASH_UNLESS(WaitForSeconds::New_ctor(0.5f)));
        
        list->get_gameObject()->set_active(true);

        if (images.empty()) {
            loadingControl->____refreshText->get_gameObject()->SetActive(true);
            loadingControl->ShowText("No images found in folder!\n/sdCard/ModData/com.beatgames/Mods/ImageFactory/Images/", true);
        } else {
            loadingControl->Hide();
        }
        co_return;
    }
}
