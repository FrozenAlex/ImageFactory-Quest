#pragma once
#include "PluginConfig.hpp"
#include "IFImage.hpp"

namespace ImageFactory::Config {
    std::list<std::shared_ptr<ImageConfig>>* getImageConfigsList();
    void ClearImages();
    void Reset();
    void Load();
    void Save();
    void RemoveImage(std::shared_ptr<ImageConfig> imageConfig);
    std::shared_ptr<ImageConfig> AddImage(ImageConfig imageConfig);
    void AttachImageToConfig(UnityW<ImageFactory::IFImage> image, std::shared_ptr<ImageConfig> imageConfig);
    void ReorderImages(std::shared_ptr<ImageConfig> from, std::shared_ptr<ImageConfig> to);
    std::optional<std::shared_ptr<ImageConfig>> GetConfigByImage(UnityW<ImageFactory::IFImage> image);
    UnityW<ImageFactory::IFImage> GetImageByConfig(std::shared_ptr<ImageConfig> imageConfig);
};