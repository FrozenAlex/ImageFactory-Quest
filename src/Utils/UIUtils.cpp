#include "Utils/UIUtils.hpp"

#include "System/Collections/Generic/Dictionary_2.hpp"
#include "GlobalNamespace/PlayerSpecificSettings.hpp"
#include "GlobalNamespace/PlayerDataModel.hpp"
#include "GlobalNamespace/PlayerData.hpp"
#include "TMPro/TextMeshProUGUI.hpp"
#include "UnityEngine/Vector2.hpp"
#include "UnityEngine/RectTransform.hpp"
#include "UnityEngine/UI/VerticalLayoutGroup.hpp"
#include "UnityEngine/UI/LayoutElement.hpp"
#include "UnityEngine/SpriteMeshType.hpp"
#include "UnityEngine/TextureWrapMode.hpp"
#include "UnityEngine/Texture2D.hpp"
#include "HMUI/CurvedCanvasSettingsHelper.hpp"
#include "HMUI/ImageView.hpp"
#include "bsml/shared/Helpers/utilities.hpp"
#include "bsml/shared/BSML-Lite/Creation/Text.hpp"
#include "bsml/shared/BSML/Animations/AnimationInfo.hpp"
#include "EasyGifReader.h"
#include "PluginConfig.hpp"
#include "assets.hpp"
#include "main.hpp"
#include <map>

using namespace HMUI;
using namespace TMPro;
using namespace UnityEngine;
using namespace UnityEngine::UI;

#define SetPreferredSize(identifier, width, height)                                         \
    auto layout##identifier = identifier->get_gameObject()->GetComponent<LayoutElement*>(); \
    if (!layout##identifier)                                                                \
        layout##identifier = identifier->get_gameObject()->AddComponent<LayoutElement*>();  \
    layout##identifier->set_preferredWidth(width);                                          \
    layout##identifier->set_preferredHeight(height)

namespace UIUtils {

    BSML::Backgroundable* CreateHeader(Transform* transform, std::string name, Vector3 localPos, Color color, int width, int height) {
        GameObject* mainLayout = GameObject::New_ctor();
        RectTransform* parent = mainLayout->AddComponent<RectTransform*>();
        parent->SetParent(transform, false);
        parent->set_localPosition(localPos);

        VerticalLayoutGroup* vertical = BSML::Lite::CreateVerticalLayoutGroup(parent);
        vertical->get_rectTransform()->set_anchoredPosition({40.0f, 35.0f});
        HorizontalLayoutGroup* horizontal =
            BSML::Lite::CreateHorizontalLayoutGroup(vertical->get_transform());

        auto layoutElem = horizontal->GetComponent<LayoutElement*>();
        layoutElem->set_preferredHeight(height);
        layoutElem->set_preferredWidth(width);

        UnityW<BSML::Backgroundable> background =
            horizontal->get_gameObject()->AddComponent<BSML::Backgroundable*>();
        background->ApplyBackground("round-rect-panel");
        background->ApplyAlpha(color.a);

        ImageView* imageView =
            background->get_gameObject()->GetComponentInChildren<ImageView*>();
        imageView->gradient = true;
        imageView->_gradientDirection = 0;
        imageView->set_color(UnityEngine::Color::get_white());
        imageView->set_color0(color);
        imageView->set_color1(color);
        imageView->_curvedCanvasSettingsHelper->Reset();

        HorizontalLayoutGroup* header = horizontal;

        TextMeshProUGUI* text = BSML::Lite::CreateText(header->get_transform(), name);
        text->set_fontSize(text->get_fontSize() * 1.5f);
        text->set_alignment(TextAlignmentOptions::Center);

        auto containerGroup = BSML::Lite::CreateHorizontalLayoutGroup(transform);
        containerGroup->get_rectTransform()->set_anchoredPosition({0.0f, 0.0f});
        
        SetPreferredSize(containerGroup, 20, 80);

        auto bg = containerGroup->get_gameObject()->AddComponent<BSML::Backgroundable*>();
        bg->ApplyBackground(il2cpp_utils::newcsstr("round-rect-panel"));
        bg->background->set_color(color);

        return bg;
    }

    BSML::Backgroundable* CreateHeader(Transform* transform, std::string name) {
        return CreateHeader(transform, name, {38.0f, 0.0f, 0.0f}, Color(0.1f, 0.1f, 0.1f, 0.0f), 100, 10);
    }

    HMUI::ModalView* CreateModal(Sprite* sprite, std::string desc, Transform* parent, std::string name, std::string buttonText, std::function<void(HMUI::ModalView*)> onClick) {
        BSML::ModalView* modal = BSML::Lite::CreateModal(parent, {90.0f, 50.0f}, nullptr, true);
        GameObject* scrollableModal = BSML::Lite::CreateScrollableModalContainer(modal);
         

        modal->Show();

        if (name.compare("Help and FAQ") == 0) {
            BSML::Lite::CreateImage(modal->get_transform(), sprite, {-38.0f, 19.0f}, {6.0f, 9.0f});
        } else {
            BSML::Lite::CreateImage(modal->get_transform(), sprite, {-38.0f, 19.0f}, {9.0f, 9.0f});
        }

        BSML::Lite::CreateUIButton(modal->get_transform(), "DISMISS", {-22.0f, -19.0f}, {40.0f, 8.0f}, [modal]() {
            modal->Hide();
        });
        BSML::Lite::CreateUIButton(modal->get_transform(), buttonText, {22.0f, -19.0f}, {40.0f, 8.0f}, [onClick, modal]() { onClick(modal); });
    
        BSML::Lite::CreateImage(modal->get_transform(), BSML::Lite::ArrayToSprite(Assets::Pixel_png), {0.0f, 13.0f}, {85.0f, 0.8f});

        // TODO: Check if it's anchored position or sizeDelta
        BSML::Lite::CreateText(modal->get_transform(), name, 10.0f, {-32.0f, 25.0f});

        // TODO: Tweak font size and check italics
        BSML::Lite::CreateText(modal->get_transform(), desc, {-42.0f, 10.0f});
        return modal;
    }

    using namespace BSML;

    inline uint32_t make_black_transparent(const uint32_t& v) {
        return v >> 8 ? v : 0;
    }

    SafePtr<System::Collections::Generic::Dictionary_2<StringW, UnityEngine::Sprite*>> cache;
    Sprite* FirstFrame(std::string path) {
        if (!cache) cache.emplace(System::Collections::Generic::Dictionary_2<StringW, UnityEngine::Sprite*>::New_ctor());

        UnityEngine::Sprite* sprite = nullptr;
        if (cache->TryGetValue(path, byref(sprite)) && sprite && sprite->m_CachedPtr.m_value) {
            return sprite;
        } else {
            cache->Remove(path);
        }

        try {
            auto gifReader = EasyGifReader::openFile(path.c_str());
            int  width = gifReader.width(), height = gifReader.height(), frameCount = gifReader.frameCount();

            for (const auto& gifFrame : gifReader) {
                auto currentFrame = new FrameInfo(gifFrame.width(), gifFrame.height());

                const uint8_t* pixels = (const uint8_t*)gifFrame.pixels();
                uint8_t* colorData = currentFrame->colors.ptr()-> _values + currentFrame->colors.ptr()->get_Length();

                int height = gifFrame.height();
                int rowSize = sizeof(uint32_t) * gifFrame.width();
                
                for (int y = 0; y < height; y++) {
                    colorData -= rowSize;
                    
                    std::transform((uint32_t*)pixels, (uint32_t*)(pixels + rowSize), (uint32_t*)colorData, make_black_transparent);
                    pixels += rowSize;
                }

                auto frameTexture = Texture2D::New_ctor(gifFrame.width(), gifFrame.height(), UnityEngine::TextureFormat::RGBA32, false);
                frameTexture->set_wrapMode(TextureWrapMode::Clamp);
                frameTexture->LoadRawTextureData(currentFrame->colors.ptr());
                frameTexture->Apply();

                sprite = BSML::Utilities::LoadSpriteFromTexture(frameTexture);
                
                cache->Add(path, sprite);

                return sprite;
            }
        } catch (EasyGifReader::Error gifError) { }

        return nullptr;
    }

    SafePtrUnity<GlobalNamespace::PlayerDataModel> model = nullptr;
    bool NoHud() {
        if (!model.isAlive()) model = Object::FindObjectOfType<GlobalNamespace::PlayerDataModel*>();

        bool ignore = getPluginConfig().IgnoreTextAndHUDs.GetValue();

        if (ignore) {
            return true;
        }

        return ignore == model->get_playerData()->get_playerSpecificSettings()->get_noTextsAndHuds();
    }
}