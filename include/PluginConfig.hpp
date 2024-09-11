#pragma once

#include "UnityEngine/Color.hpp"
#include "UnityEngine/Vector2.hpp"
#include "UnityEngine/Vector3.hpp"
#include "UnityEngine/Vector4.hpp"

#include "IFImage.hpp"
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "custom-types/shared/coroutine.hpp"
#include "config-utils/shared/config-utils.hpp"

#define IMAGE_FACTORY_PATH_FORMAT "/sdcard/ModData/{}/Mods/ImageFactory/"
static std::string IMAGE_FACTORY_PATH = fmt::format(IMAGE_FACTORY_PATH_FORMAT, modloader::get_application_id().c_str());
static std::string IMAGE_FACTORY_IMAGES_PATH = IMAGE_FACTORY_PATH + "Images/";


DECLARE_JSON_CLASS(PresentationOptions,
    NAMED_VALUE_OPTIONAL(std::string, PresentationID, "PresentationID");
    NAMED_VALUE_OPTIONAL(std::string, Value, "Value");
    // Zero Duration will be infinite
    NAMED_VALUE_DEFAULT(float, Duration, 0.0f, "Duration");
)

DECLARE_JSON_CLASS(ImageConfig,
    NAMED_VALUE_DEFAULT(bool, Enabled, true,  "Enabled");
    NAMED_VALUE(UnityEngine::Vector2, Size, "Size");
    NAMED_VALUE(UnityEngine::Vector3, Position, "Position");
    NAMED_VALUE(UnityEngine::Quaternion, Rotation, "Rotation");
    NAMED_VALUE_OPTIONAL(std::string, Name, "Name");
    NAMED_VALUE_OPTIONAL(std::string, LocalFilePath, "LocalFilePath");

    NAMED_VALUE(PresentationOptions, Presentation, "Presentation");
)

DECLARE_CONFIG(PluginConfig,
    CONFIG_VALUE(Enabled, bool, "Enabled", true);
    CONFIG_VALUE(AllowAnimations, bool, "AllowAnimations", true);
    CONFIG_VALUE(IgnoreTextAndHUDs, bool, "IgnoreTextAndHUDs", false);
    
    CONFIG_VALUE(SaveData, std::vector<ImageConfig>, "SaveData", std::vector<ImageConfig>());
);

namespace ImageFactory {
    class Config {
        public:
            static void Reset();
            static void Delete(IFImage* image, bool clearInfo);
            static void Add(IFImage* image);
            static void Update(IFImage* image);
            static custom_types::Helpers::Coroutine LoadImages();
    };
}