#pragma once

#include "UnityEngine/Color.hpp"
#include "UnityEngine/Vector2.hpp"
#include "UnityEngine/Vector3.hpp"
#include "UnityEngine/Vector4.hpp"
#include "UnityEngine/Quaternion.hpp"

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "custom-types/shared/coroutine.hpp"
#include "config-utils/shared/config-utils.hpp"

#define IMAGE_FACTORY_PATH_FORMAT "/sdcard/ModData/{}/Mods/ImageFactory/"
static std::string IMAGE_FACTORY_PATH = fmt::format(IMAGE_FACTORY_PATH_FORMAT, modloader::get_application_id().c_str());
static std::string IMAGE_FACTORY_IMAGES_PATH = IMAGE_FACTORY_PATH + "Images/";


// Declare quaternion
#define CONVERSION(clazz, construct, convert) \
clazz(const UnityEngine::clazz& other) { construct; } \
clazz& operator=(const UnityEngine::clazz& other) { construct; return *this; } \
operator UnityEngine::clazz() const { return UnityEngine::clazz convert; }

namespace ConfigUtils {
    DECLARE_JSON_STRUCT(Quaternion) {
        VALUE(float, x);
        VALUE(float, y);
        VALUE(float, z);
        VALUE(float, w);
        CONVERSION(Quaternion, x = other.x; y = other.y; z = other.z; w = other.w, (x, y, z, w));
        Quaternion() = default;
        Quaternion(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
    };
}

#undef CONVERSION


DECLARE_JSON_STRUCT(PresentationOptions) {
    NAMED_VALUE_OPTIONAL(std::string, PresentationID, "PresentationID");
    NAMED_VALUE_OPTIONAL(std::string, Value, "Value");
    // Zero Duration will be infinite
    NAMED_VALUE_DEFAULT(float, Duration, 0.1f, "Duration");
};
  

DECLARE_JSON_STRUCT(ImageConfig) {
    NAMED_VALUE_DEFAULT(bool, Enabled, true,  "Enabled");
    NAMED_VALUE(ConfigUtils::Vector2, Size, "Size");
    NAMED_VALUE(ConfigUtils::Vector3, Position, "Position");
    NAMED_VALUE(ConfigUtils::Quaternion, Rotation, "Rotation");
    NAMED_VALUE_OPTIONAL(std::string, Name, "Name");
    // *** LocalFilePath is the relative path to the image file in the Images folder.
    NAMED_VALUE_OPTIONAL(std::string, LocalFilePath, "LocalFilePath");

    NAMED_VALUE(PresentationOptions, Presentation, "Presentation");
};

DECLARE_CONFIG(PluginConfig) {
    CONFIG_VALUE(Enabled, bool, "Enabled", true);
    CONFIG_VALUE(AllowAnimations, bool, "AllowAnimations", true);
    CONFIG_VALUE(IgnoreTextAndHUDs, bool, "IgnoreTextAndHUDs", false);
    CONFIG_VALUE(SaveData, std::vector<ImageConfig>, "SaveData", std::vector<ImageConfig>());
};