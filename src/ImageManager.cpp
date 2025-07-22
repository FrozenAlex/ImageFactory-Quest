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
#include "ConfigManager.hpp"

DEFINE_TYPE(ImageFactory, ImageManager);


custom_types::Helpers::Coroutine ImageFactory::ImageManager::LoadImages() {
    // Parse images and Loop through each image
    INFO("Loading images...");
    ImageFactory::Config::Load();

    auto imageConfigs = ImageFactory::Config::getImageConfigsList();
    if (imageConfigs->empty()) {
        INFO("No images found in the config, skipping loading.");
        co_return;
    }
    INFO("Found {} images in the config.", imageConfigs->size());


    // Loop through each image
    for (auto imageConfig: *imageConfigs ) {
        // Check if the file exists, if not, continue to the next.
        auto fullFilePath = IMAGE_FACTORY_IMAGES_PATH + imageConfig->LocalFilePath.value();

        fstream f(fullFilePath);

        // This should never happen tho...
        if (!f.good()) {
            WARNING("File not found: {}", fullFilePath);
            continue;
        }

        // Create the image object.
        IFImage* image = this->go->AddComponent<IFImage*>();
        
        ImageFactory::Config::AttachImageToConfig(image, imageConfig);

        // Setup image from the configValue.
        image->path = fullFilePath;
        image->sprite = BSML::Lite::FileToSprite(image->path);
        image->position = imageConfig->Position;
        image->rotation = imageConfig->Rotation;
        image->scale = imageConfig->Size;
        image->name = imageConfig->Name.value_or("");
        image->presentationoption = imageConfig->Presentation.PresentationID.value_or("");
        image->enabled = imageConfig->Enabled;
        image->extraData = new std::unordered_map<std::string, std::string>();
        image->isAnimated = FileUtils::isGifFile(image->path);
        image->canAnimate = false;

        // Setup lookup dictionary for extra data.
        StringW extraData = imageConfig->Presentation.Value.value_or("");
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
    auto imageConfig = ImageFactory::Config::GetConfigByImage(image);

    images->Remove(image);
    image->Destroy();

    if (imageConfig.has_value()) {
        ImageFactory::Config::RemoveImage(imageConfig.value());
    }
}

void ImageFactory::ImageManager::UpdateImage(UnityW<IFImage> image) {
    int indexOfImage = images->IndexOf(image);

    if (indexOfImage == -1) {
        WARNING("Image not found in the list to update it");
        return;
    }

    auto imageConfigOptional = ImageFactory::Config::GetConfigByImage(image);
    if (!imageConfigOptional) {
        WARNING("ImageConfig not found for the image");
        return;
    }
    auto imageConfig = imageConfigOptional.value();

    imageConfig->Position = image->position;
    imageConfig->Rotation = image->rotation;
    imageConfig->Size = image->scale;
    imageConfig->Name = image->name;
    imageConfig->Presentation.PresentationID = image->presentationoption;
    imageConfig->Enabled = image->enabled;

    // TODO: Implement duration
    imageConfig->Presentation.Duration = 0.0f;

    // Setup lookup dictionary for extra data.
    std::string extraData;
    for (std::pair<StringW, StringW> pair : *image->extraData) {
        extraData += static_cast<std::string>(pair.first) + "|" + static_cast<std::string>(pair.second) + "/";
    }
    imageConfig->Presentation.Value = extraData;

    // Save the updated config.
    ImageFactory::Config::Save();
}

void ImageFactory::ImageManager::RemoveAllImages() {
    DEBUG("Removing all images from the ImageManager");
    if (!images) {
        DEBUG("No images to remove from the ImageManager");
        return;
    }
    for (auto& image : images) {
        if (image) {
            image->Destroy();
        }
    }
    DEBUG("All images destroyed from the ImageManager");
    images->Clear();
    DEBUG("All images removed from the ImageManager");
    // Clear the config images as well.
    DEBUG("Clearing all images from the ImageFactory Config");
    ImageFactory::Config::ClearImages();
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