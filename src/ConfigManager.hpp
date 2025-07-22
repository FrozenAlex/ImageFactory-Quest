#pragma once
#include "PluginConfig.hpp"
#include <vector>
#include <string>

namespace ImageFactory::Config {
    std::list<ImageConfig*>* getImageConfigsList();
    void ClearImages();
    void Reset();
    void Load();
    void Save();
    void RemoveImage(ImageConfig* imageConfig);
    void AddImage(ImageConfig& imageConfig);
    void AttachImageToConfig(UnityW<ImageFactory::IFImage> image, ImageConfig* imageConfig);
    void ReorderImages(ImageConfig* from, ImageConfig* to);
    ImageConfig* GetConfigByImage(UnityW<ImageFactory::IFImage> image);
    UnityW<ImageFactory::IFImage> GetImageByConfig(ImageConfig* imageConfig);
};