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