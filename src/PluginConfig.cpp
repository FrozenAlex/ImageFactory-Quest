#include "../include/PluginConfig.hpp"
#include "Utils/FileUtils.hpp"

using namespace UnityEngine;
#define RESET_CONFIG_VALUE(value) \
    value.SetValue(value.GetDefaultValue())


namespace ImageFactory {
    void Config::Reset() {
        PluginConfig_t& pluginConfig = getPluginConfig();
        RESET_CONFIG_VALUE(pluginConfig.Enabled);
        RESET_CONFIG_VALUE(pluginConfig.IgnoreTextAndHUDs);
        RESET_CONFIG_VALUE(pluginConfig.AllowAnimations);
        RESET_CONFIG_VALUE(pluginConfig.SaveData);
    }
}