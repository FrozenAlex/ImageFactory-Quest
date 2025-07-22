#pragma once
#include "PluginConfig.hpp"
#include <vector>
#include <string>

namespace ImageFactory::Config {
    std::list<shared_ptr<ImageConfig>>* getImageConfigsList();
    void ClearImages();
    void Reset();
    void Load();
    void Save();
    void RemoveImage(shared_ptr<ImageConfig> imageConfig);
    shared_ptr<ImageConfig> AddImage(ImageConfig imageConfig);
    void AttachImageToConfig(UnityW<ImageFactory::IFImage> image, shared_ptr<ImageConfig> imageConfig);
    void ReorderImages(shared_ptr<ImageConfig> from, shared_ptr<ImageConfig> to);
    std::optional<shared_ptr<ImageConfig>> GetConfigByImage(UnityW<ImageFactory::IFImage> image);
    UnityW<ImageFactory::IFImage> GetImageByConfig(shared_ptr<ImageConfig> imageConfig);
};