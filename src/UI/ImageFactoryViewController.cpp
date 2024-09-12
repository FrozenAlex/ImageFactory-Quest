#include "UI/ImageFactoryViewController.hpp"

#include "IFImage.hpp"
#include "PluginConfig.hpp"
#include "Utils/UIUtils.hpp"
#include "HMUI/Touchable.hpp"
#include "UnityEngine/Application.hpp"
#include "UnityEngine/Vector2.hpp"
#include "Presenters/PresenterManager.hpp"
#include "assets.hpp"

DEFINE_TYPE(ImageFactory::UI, ImageFactoryViewController);

using namespace UnityEngine::UI;
using namespace UnityEngine;
using namespace UIUtils;
using namespace HMUI;

namespace ImageFactory::UI {

    void ImageFactoryViewController::DidActivate(
        bool firstActivation,
        bool addedToHierarchy,
        bool screenSystemEnabling
    ) {
        if (firstActivation) 
        {
            if (!get_gameObject()) return;

            get_gameObject()->AddComponent<Touchable*>();
            
           UnityEngine::Sprite* logo = BSML::Lite::ArrayToSprite(Assets::Logo_png);
           UnityEngine::Sprite* optimus = BSML::Lite::ArrayToSprite(Assets::Optimus_png);
           UnityEngine::Sprite* bandoot = BSML::Lite::ArrayToSprite(Assets::Bandoot_jpg);
           UnityEngine::Sprite* github = BSML::Lite::ArrayToSprite(Assets::GitHub_png);
           UnityEngine::Sprite* help = BSML::Lite::ArrayToSprite(Assets::Help_png);
           UnityEngine::Sprite* settings = BSML::Lite::ArrayToSprite(Assets::Settings_png);
           UnityEngine::Sprite* reset = BSML::Lite::ArrayToSprite(Assets::Reset_png);

            BSML::Lite::CreateImage(this, logo, {0.0f, 0.0f}, {58.0f, 58.0f});

            auto sButton = BSML::Lite::CreateClickableImage(
        this,
        settings,
        [=](){
                UnityW<BSML::ModalView> modal = BSML::Lite::CreateModal(this, UnityEngine::Vector2(76.0f, 28.0f), nullptr, true);

                GameObject* scrollableModal = BSML::Lite::CreateScrollableModalContainer(modal);
                BSML::Lite::CreateToggle(scrollableModal->get_transform(), "Enabled", getPluginConfig().Enabled.GetValue(),
                    [](bool b) { 
                        getPluginConfig().Enabled.SetValue(b);

                        if (!b) {
                            for (std::pair<IFImage*, StringW> pair : *PresenterManager::MAP) {
                                pair.first->Despawn(false);
                            }
                        } else {
                            PresenterManager::SpawnInMenu();
                        }
                    });

                BSML::Lite::CreateToggle(scrollableModal->get_transform(), "Animate Images", getPluginConfig().AllowAnimations.GetValue(),
                    [](bool b) { getPluginConfig().AllowAnimations.SetValue(b); });

                BSML::Lite::CreateToggle(scrollableModal->get_transform(), "Ignore No Text and HUDs", getPluginConfig().IgnoreTextAndHUDs.GetValue(),
                    [](bool b) { getPluginConfig().IgnoreTextAndHUDs.SetValue(b); });

                modal->Show();
                },
                {-35.0f, 20.0f},
                {18.0f, 18.0f}
            );
            BSML::Lite::AddHoverHint(sButton, "Global Mod Settings");

            auto rButton = BSML::Lite::CreateClickableImage(this, reset, [=](){
                Config::Reset();
            },  {-35.0f, 0.0f}, {18.0f, 18.0f});

            rButton->set_preserveAspect(true);
            BSML::Lite::AddHoverHint(rButton, "Reset All Config Data");
            
            static auto UnityEngine_Application_OpenURL = il2cpp_utils::resolve_icall<void, StringW>("UnityEngine.Application::OpenURL");

            auto hButton = BSML::Lite::CreateClickableImage(this, help,  [=](){
                CreateModal(help, "ImageFactory allows you to place custom images throughout your\ngame. Everything has been designed to be easy for the user to "
                            "use\nand customize. To learn more about how to use this mod, you can\nwatch this for how to use it.", this->get_transform(),
                            "Help and FAQ", "Youtube Link", [=](HMUI::ModalView* modal){
                                

                                UnityEngine_Application_OpenURL("https://www.youtube.com/watch?v=biKDA6Fm5lI");
                            });
            }, {-35.0f, -20.0f}, {18.0f, 18.0f});
            hButton->set_preserveAspect(true);
            BSML::Lite::AddHoverHint(hButton, "Help and Tutorial");


            auto oButton = BSML::Lite::CreateClickableImage(this, optimus, [=](){
                CreateModal(optimus, "sum random guy named optimus ported this mod to quest", this->get_transform(),
                            "Optimus", "Github Link", [=](HMUI::ModalView* modal){
                                UnityEngine_Application_OpenURL("https://github.com/OptimusChen");
                            });
            },  {35.0f, -20.0f}, {18.0f, 18.0f});
            BSML::Lite::AddHoverHint(oButton, "The mod creator!");

            auto bButton = BSML::Lite::CreateClickableImage(this, bandoot, [=](){
                CreateModal(bandoot, "Bandoot commissioned for the original PC ImageFactory mod to\nbe made, go check him out!", this->get_transform(),
                            "Bandoot", "Twitch Link", [=](HMUI::ModalView* modal){
                                UnityEngine_Application_OpenURL("https://www.twitch.tv/bandoot");
                            });
            }, {35.0f, 0.0f}, {18.0f, 18.0f});
            BSML::Lite::AddHoverHint(bButton, "The mod commissioner!");

            auto gButton = BSML::Lite::CreateClickableImage(this, github,  [=](){
                CreateModal(github, "ImageFactory is open source! You can view it on GitHub. Have a\nbug report or a feature request? Submit an issue on GitHub.", this->get_transform(),
                            "Github", "Github Link", [=](HMUI::ModalView* modal){
                                UnityEngine_Application_OpenURL("https://github.com/OptimusChen/ImageFactory-Quest");
                            });
            },{35.0f, 20.0f}, {18.0f, 18.0f});
            BSML::Lite::AddHoverHint(gButton, "View the Github Page!");
        }
    }
}
