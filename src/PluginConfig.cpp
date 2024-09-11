#include "../include/PluginConfig.hpp"

#include "beatsaber-hook/shared/rapidjson/include/rapidjson/writer.h"
#include "UI/ImageFactoryFlowCoordinator.hpp"
#include "UI/ImageEditingViewController.hpp"
#include "Presenters/PresenterManager.hpp"
#include "UnityEngine/WaitForSeconds.hpp"
#include "UnityEngine/GameObject.hpp"
#include "Utils/StringUtils.hpp"
#include "Utils/FileUtils.hpp"
#include "IFImage.hpp"
#include "logging.hpp"
#include <fstream>


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

    void Config::Add(IFImage* image) {
        PluginConfig_t& pluginConfig = getPluginConfig();
        auto imageConfig = ImageConfig();
        imageConfig.Position = image->position;
        imageConfig.Rotation = image->screen->get_transform()->get_rotation();
        imageConfig.Size = image->scale;
        imageConfig.Enabled = image->enabled;
        imageConfig.LocalFilePath = image->fileName;
        imageConfig.Name = image->name;
        imageConfig.Presentation = PresentationOptions();
        imageConfig.Presentation.PresentationID = image->presentationoption;


        std::string extraData;
        for (std::pair<StringW, StringW> pair : *image->extraData) {
            extraData = extraData + static_cast<std::string>(pair.first) + ";" + static_cast<std::string>(pair.second) + "/";
        }
        imageConfig.Presentation.Value = image->presentationoption;

        auto originalVector = pluginConfig.SaveData.GetValue();
        originalVector.push_back(imageConfig);
        pluginConfig.SaveData.SetValue(originalVector);
    }

    void Config::Update(IFImage* image, int index) {
//        PluginConfig_t& pluginConfig = getPluginConfig();
//        ConfigDocument& configDoc = pluginConfig.config->config;
//        if (configDoc.HasMember(image->internalName)) {
//            rapidjson::Document::AllocatorType& allocator = pluginConfig.config->config.GetAllocator();
//            rapidjson::Value& configValue = configDoc[static_cast<std::string>(image->internalName)];
//
//            configValue["x"].SetFloat(image->x);
//            configValue["y"].SetFloat(image->y);
//            configValue["z"].SetFloat(image->z);
//            configValue["angleX"].SetFloat(image->angleX);
//            configValue["angleY"].SetFloat(image->angleY);
//            configValue["angleZ"].SetFloat(image->angleZ);
//            configValue["scaleX"].SetFloat(image->scaleX);
//            configValue["scaleY"].SetFloat(image->scaleY);
//            configValue["width"].SetFloat(image->width);
//            configValue["height"].SetFloat(image->height);
//            configValue["enabled"].SetBool(image->enabled);
//            configValue["path"].SetString(image->path.c_str(), allocator);
//            configValue["name"].SetString(image->name.c_str(), allocator);
//            configValue["presentationOption"].SetString(image->presentationoption.c_str(), allocator);
//
//            std::string extraData;
//            for (std::pair<StringW, StringW> pair : *image->extraData) {
//                extraData += static_cast<std::string>(pair.first) + "|" + static_cast<std::string>(pair.second) + "/";
//            }
//
//            configValue["extraData"].SetString(extraData.c_str(), allocator);
//
//            pluginConfig.config->Write();
//            pluginConfig.config->Reload();
//        }
    }

    void Config::Delete(IFImage* image, bool clearInfo) {
        PluginConfig_t& pluginConfig = getPluginConfig();
//        ConfigDocument& configDoc = pluginConfig.config->config;
//        if (configDoc.HasMember(image->internalName)) {
//            configDoc.RemoveMember(image->internalName);
//            pluginConfig.Amount.SetValue(pluginConfig.Amount.GetValue() - 1);
//
//            StringW images = pluginConfig.Images.GetValue();
//
//            pluginConfig.Images.SetValue(static_cast<std::string>(images->Replace("/" + image->internalName, Il2CppString::_get_Empty())));
//
//            pluginConfig.config->Write();
//            pluginConfig.config->Reload();
//
//            image->Destroy();
//
//            if (clearInfo) {
//                PresenterManager::ClearInfo(image);
//            }
//
//            Object::Destroy(image);
//        }
    }

    custom_types::Helpers::Coroutine Config::LoadImages() {
        // Create game object
        GameObject* obj = GameObject::New_ctor("IFImages");
        GameObject::DontDestroyOnLoad(obj);

        // Load configuration


        // Parse images and  Loop through each image
        auto images = getPluginConfig().SaveData.GetValue();

        for (auto& imageConfig: images ) {
            // Check if the file exists, if not, continue to the next.
            if (!imageConfig.LocalFilePath.has_value()) {
                continue;
            }
            // Check if file name is empty, if so, continue to the next.
            if (imageConfig.LocalFilePath->empty()) {
                continue;
            }
            auto fullFilePath = IMAGE_FACTORY_IMAGES_PATH + imageConfig.LocalFilePath.value();

            // Check if file exists in the file system, if not, continue to the next.
            if (!fileexists(fullFilePath)) {
                WARNING("File not found: {}", fullFilePath);
                continue;
            }

            fstream f(fullFilePath);
            if (!f.good()) {
                WARNING("File not found: {}", fullFilePath);
                continue;
            }

            // Create the image object.
            IFImage* image = obj->AddComponent<IFImage*>();

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
}