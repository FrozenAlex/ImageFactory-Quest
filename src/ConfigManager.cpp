#include "ConfigManager.hpp"
#include "PluginConfig.hpp"
#include <memory>
#include <optional>
#include <unordered_map>
#include <vector>
#include "IFImage.hpp"
#include "logging.hpp"

using namespace UnityEngine;
#define RESET_CONFIG_VALUE(value) \
    value.SetValue(value.GetDefaultValue())

std::list<std::shared_ptr<ImageConfig>> imageConfigs;
std::unordered_map<std::shared_ptr<ImageConfig>, SafePtrUnity<ImageFactory::IFImage>> imageConfigMap;

bool isLoaded = false;

namespace ImageFactory::Config {
    std::list<std::shared_ptr<ImageConfig>>* getImageConfigsList() {
        return &imageConfigs;
    }

    void ClearImages() {
        if (!isLoaded) return;

        imageConfigs.clear();
        imageConfigMap.clear();
        PluginConfig_t& pluginConfig = getPluginConfig();
        pluginConfig.SaveData.SetValue(std::vector<ImageConfig>());
        pluginConfig.Save();
        INFO("Cleared all images from the config.");
    }

    void Reset() {
        if (!isLoaded) return;
        

        PluginConfig_t& pluginConfig = getPluginConfig();
        RESET_CONFIG_VALUE(pluginConfig.Enabled);
        RESET_CONFIG_VALUE(pluginConfig.IgnoreTextAndHUDs);
        RESET_CONFIG_VALUE(pluginConfig.AllowAnimations);
        RESET_CONFIG_VALUE(pluginConfig.SaveData);

        isLoaded = false;
        imageConfigs.clear();
        imageConfigMap.clear();
        pluginConfig.Save();
    }

    /**
     * @brief Cleans up non existing images from the config.
     */
    void CleanUp() {
        if (!isLoaded) return;
        
        int sizeBefore = imageConfigs.size();
        imageConfigs.remove_if([](std::shared_ptr<ImageConfig> imageConfig) {
            if (!imageConfig->LocalFilePath.has_value()) {
                return true;
            }
            if (imageConfig->LocalFilePath->empty()) {
                return true;
            }
            auto fullFilePath = IMAGE_FACTORY_IMAGES_PATH + imageConfig->LocalFilePath.value();
            if (!fileexists(fullFilePath)) {
                WARNING("File not found: {}", fullFilePath);
                return true;
            }
            return false;
        });
        int sizeAfter = imageConfigs.size();
        if (sizeBefore != sizeAfter) {
            INFO("Removed {} invalid entries from the config.", sizeBefore - sizeAfter);
            Save();
        } else {
            INFO("No invalid entries found in the config.");
        }
    }

    void Load() {
        if (isLoaded) return;

        PluginConfig_t& pluginConfig = getPluginConfig();
        
        auto configValues = pluginConfig.SaveData.GetValue();
        for (const auto& config : configValues) {
            std::shared_ptr<ImageConfig> imageConfig = std::make_shared<ImageConfig>();
            imageConfig->Enabled = config.Enabled;
            imageConfig->Size = config.Size;
            imageConfig->Position = config.Position;
            imageConfig->Rotation = config.Rotation;
            imageConfig->Name = config.Name;
            imageConfig->LocalFilePath = config.LocalFilePath;
            imageConfig->Presentation = config.Presentation;

            imageConfigs.push_back(imageConfig);
        }
        
        CleanUp();
        isLoaded = true;
    }

    void Save() {
        if (!isLoaded) {
            WARNING("ImageFactory Config is not loaded, cannot save.");
            return;
        };

        PluginConfig_t& pluginConfig = getPluginConfig();
        std::vector<ImageConfig> configs;

        for (const auto& imageConfig : imageConfigs) {
            ImageConfig config;
            config.Enabled = imageConfig->Enabled;
            config.Size = imageConfig->Size;
            config.Position = imageConfig->Position;
            config.Rotation = imageConfig->Rotation;
            config.Name = imageConfig->Name;
            config.LocalFilePath = imageConfig->LocalFilePath;
            config.Presentation = imageConfig->Presentation;

            configs.push_back(config);
        }

        pluginConfig.SaveData.SetValue(configs);
        pluginConfig.Save();
    }

    void RemoveImage(std::shared_ptr<ImageConfig> imageConfig) {
        if (!isLoaded) {
            WARNING("ImageFactory Config is not loaded, cannot remove image.");
            return;
        };

        if (imageConfigMap.find(imageConfig) != imageConfigMap.end()) {
            imageConfigMap.erase(imageConfig);
        }

        auto it = std::find(imageConfigs.begin(), imageConfigs.end(), imageConfig);
        if (it != imageConfigs.end()) {
            imageConfigs.erase(it);
            Save();
        } else {
            WARNING("ImageConfig not found in the list to delete it");
        }
    }

    std::shared_ptr<ImageConfig> AddImage(ImageConfig imageConfig) {
        if (!isLoaded) {
            WARNING("ImageFactory Config is not loaded, cannot add image.");
            return nullptr;
        };

        std::shared_ptr<ImageConfig> newImageConfig = std::make_shared<ImageConfig>();
        newImageConfig->Enabled = imageConfig.Enabled;
        newImageConfig->Size = imageConfig.Size;
        newImageConfig->Position = imageConfig.Position;
        newImageConfig->Rotation = imageConfig.Rotation;
        newImageConfig->Name = imageConfig.Name;
        newImageConfig->LocalFilePath = imageConfig.LocalFilePath;
        newImageConfig->Presentation = imageConfig.Presentation;

        imageConfigs.push_back(newImageConfig);
        Save();
        return newImageConfig;
    }

    void AttachImageToConfig(UnityW<ImageFactory::IFImage> image, std::shared_ptr<ImageConfig> imageConfig) {
        if (!isLoaded) {
            WARNING("ImageFactory Config is not loaded, cannot attach image to config.");
            return;
        };

        if (imageConfigMap.find(imageConfig) == imageConfigMap.end()) {
            imageConfigMap[imageConfig] = image;
        } else {
            WARNING("ImageConfig already has an associated IFImage");
        }
    }
    void ReorderImages(std::shared_ptr<ImageConfig> from, std::shared_ptr<ImageConfig> to) {
        if (!isLoaded) return;
        auto itFrom = std::find(imageConfigs.begin(), imageConfigs.end(), from);
        auto itTo = std::find(imageConfigs.begin(), imageConfigs.end(), to);

        if (itFrom != imageConfigs.end() && itTo != imageConfigs.end()) {
            std::iter_swap(itFrom, itTo);
            Save();
        } else {
            WARNING("One or both ImageConfigs not found in the list to reorder, ignoring.");
        }
    }

    std::optional<std::shared_ptr<ImageConfig>> GetConfigByImage(UnityW<ImageFactory::IFImage> image) {
        if (!isLoaded) {
            WARNING("ImageFactory Config is not loaded, cannot get config by image.");
            return nullptr;
        };

        for (auto& pair : imageConfigMap) {
            if (pair.second == image) {
                auto pointer = pair.first; // Return the shared_ptr to the ImageConfig (can be null)
                if (!pointer) {
                    return std::nullopt; // If the weak_ptr is expired, return nullopt
                }
                return pointer; // Return the shared_ptr to the ImageConfig
            }
        }
        return std::nullopt; // Not found
    }

    UnityW<ImageFactory::IFImage> GetImageByConfig(std::shared_ptr<ImageConfig> imageConfig) {
        if (!isLoaded) {
            WARNING("ImageFactory Config is not loaded, cannot get image by config.");
            return nullptr;
        };

        auto it = imageConfigMap.find(imageConfig);
        if (it != imageConfigMap.end()) {
            return it->second.ptr();
        }
        return nullptr; // Not found
    }
};