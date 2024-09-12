#include "ImageManager.hpp"
#include "logging.hpp"
#include "Presenters/PresenterManager.hpp"
#include "Utils/StringUtils.hpp"
#include "Utils/FileUtils.hpp"
#include <string>
#include <unordered_map>
#include <vector>
#include "UnityEngine/WaitForSeconds.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/TextureFormat.hpp"
#include <fstream>

DEFINE_TYPE(ImageFactory, ImageManager);


custom_types::Helpers::Coroutine ImageFactory::ImageManager::LoadImages() {
    // Parse images and  Loop through each image
    auto imageConfigs = getPluginConfig().SaveData.GetValue();

    // Remove all invalid config entries.
    int vectorSizeBeforeCleanup = imageConfigs.size();
    imageConfigs.erase(std::remove_if(imageConfigs.begin(), imageConfigs.end(), [](ImageConfig& imageConfig) {
        if (!imageConfig.LocalFilePath.has_value()) {
            return true;
        }
        if (imageConfig.LocalFilePath->empty()) {
            return true;
        }
        auto fullFilePath = IMAGE_FACTORY_IMAGES_PATH + imageConfig.LocalFilePath.value();
        if (!fileexists(fullFilePath)) {
            WARNING("File not found: {}", fullFilePath);
            return true;
        }
        return false;
    }), imageConfigs.end());
    int vectorSizeAfterCleanup = images.size();
    INFO("Removed {} invalid entries from the config.", vectorSizeBeforeCleanup - vectorSizeAfterCleanup);
    if (vectorSizeBeforeCleanup != vectorSizeAfterCleanup) {
        getPluginConfig().SaveData.SetValue(imageConfigs);
    }

    // Loop through each image
    for (auto& imageConfig: imageConfigs ) {
        // Check if the file exists, if not, continue to the next.
        auto fullFilePath = IMAGE_FACTORY_IMAGES_PATH + imageConfig.LocalFilePath.value();

        fstream f(fullFilePath);

        // This should never happen tho...
        if (!f.good()) {
            WARNING("File not found: {}", fullFilePath);
            continue;
        }

        // Create the image object.
        IFImage* image = this->go->AddComponent<IFImage*>();

        // Setup image from the configValue.
        image->path = fullFilePath;
        image->sprite = BSML::Lite::FileToSprite(image->path);
        image->position = imageConfig.Position;
        image->rotation = imageConfig.Rotation;
        image->scale = imageConfig.Size;
        image->name = imageConfig.Name.value_or("");
        image->presentationoption = imageConfig.Presentation.PresentationID.value_or("");
        image->enabled = imageConfig.Enabled;
        image->extraData = new std::unordered_map<std::string, std::string>();
        image->isAnimated = FileUtils::isGifFile(image->path);
        image->canAnimate = false;

        // Setup lookup dictionary for extra data.
        StringW extraData = imageConfig.Presentation.Value.value_or("");
        if (extraData->get_Length() != 0) {
            std::vector<std::string> pairs = StringUtils::split(extraData, '/');

            for (int j = 0; j < pairs.size(); j++) {
                StringW pair = pairs.at(j);

                if (pair->get_Length() != 0) {
                    StringW key = StringUtils::split(pair, ';').at(0);
                    StringW val = StringUtils::split(pair, ';').at(1);

                    image->SetExtraData(key, val);
                }
            }
        }

        image->fileName = FileUtils::GetFileName(image->path, false);
        PresenterManager::Parse(image, image->presentationoption);

        if (FileUtils::isGifFile(image->path)) {
            image->sprite = UIUtils::FirstFrame(image->path);
        }

        image->Create();
        image->Update(false);
        image->Despawn(false);

        // Add image to the ImageManager list.
        this->AddImage(image);

        bool finished = false;

        co_yield custom_types::Helpers::CoroutineHelper::New(
                image->SetImage([&finished](){
                    finished = true;
                })
        );

        while (!finished) {
            co_yield nullptr;
        }

        // Yield 50ms to give other threads a chance to run.
        co_yield reinterpret_cast<System::Collections::IEnumerator*>(CRASH_UNLESS(WaitForSeconds::New_ctor(0.05f)));
    }


    PresenterManager::SpawnInMenu();

    INFO("DONE LOADING");

    co_return;
}

void ImageFactory::ImageManager::AddImage(UnityW<IFImage> image) {
    images->Add(image);
}

void ImageFactory::ImageManager::AddNewImage(UnityW<IFImage> image) {
    // TODO: Add the image to config too
    images->Add(image);
}

void ImageFactory::ImageManager::RemoveImage(UnityW<IFImage> image) {
    auto indexOfImage = images->IndexOf(image);
    if (indexOfImage == -1) {
        WARNING("Image not found in the list to delete it");
        return;
    }

    images->Remove(image);
    image->Destroy();

    auto imageConfigs = getPluginConfig().SaveData.GetValue();
    imageConfigs.erase(imageConfigs.begin() + indexOfImage);
    getPluginConfig().SaveData.SetValue(imageConfigs);
}

void ImageFactory::ImageManager::UpdateImage(UnityW<IFImage> image) {
    int indexOfImage = images->IndexOf(image);

    if (indexOfImage == -1) {
        WARNING("Image not found in the list to update it");
        return;
    }

    auto imageConfigs = getPluginConfig().SaveData.GetValue();
    auto& imageConfig = imageConfigs.at(indexOfImage);

    imageConfig.Position = image->position;
    imageConfig.Rotation = image->rotation;
    imageConfig.Size = image->scale;
    imageConfig.Name = image->name;
    imageConfig.Presentation.PresentationID = image->presentationoption;
    imageConfig.Enabled = image->enabled;

    // TODO: Implement duration
    imageConfig.Presentation.Duration = 0.0f;

    // Setup lookup dictionary for extra data.
    std::string extraData;
    for (std::pair<StringW, StringW> pair : *image->extraData) {
        extraData += static_cast<std::string>(pair.first) + "|" + static_cast<std::string>(pair.second) + "/";
    }
    imageConfig.Presentation.Value = extraData;

    // Save the updated config.
    getPluginConfig().SaveData.SetValue(imageConfigs);
}

void ImageFactory::ImageManager::RemoveAllImages() {
    for (auto& image : images) {
        image->Destroy();
    }

    images->Clear();
    getPluginConfig().SaveData.SetValue(std::vector<ImageConfig>());
}

UnityW<ImageFactory::ImageManager> ImageFactory::ImageManager::get_instance(){
    static UnityW<ImageManager> instance = nullptr;

    // If the instance is null or the pointer is null, create a new instance of the ImageManager.
    if (!instance) {
        auto go = GameObject::New_ctor("ImageFactory:ImageManager");
        Object::DontDestroyOnLoad(go);
        instance = go->AddComponent<ImageFactory::ImageManager*>();
        // Save a reference to the game object.
        instance->go = go;
    }

    return instance;
}