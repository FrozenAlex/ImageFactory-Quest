#include "UI/InfoView.hpp"
#include <memory>
#include <string>

#include "assets.hpp"
#include "ConfigManager.hpp"
#include "IFImage.hpp"
#include "ImageManager.hpp"
#include "PluginConfig.hpp"
#include "Presenters/PresenterManager.hpp"
#include "bsml/shared/BSML.hpp"
#include "logging.hpp"

DEFINE_TYPE(ImageFactory::UI, InfoView);

using namespace UnityEngine::UI;
using namespace UnityEngine;
using namespace HMUI;


namespace ImageFactory::UI {

    void InfoView::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
        if (firstActivation) {
            static auto UnityEngine_Application_OpenURL = il2cpp_utils::resolve_icall<void, StringW>("UnityEngine.Application::OpenURL");

            if (!get_gameObject()) {
                return;
            }
            
            logoSprite = BSML::Lite::ArrayToSprite(Assets::Logo_png);
            optimusSprite = BSML::Lite::ArrayToSprite(Assets::Optimus_png);
            bandootSprite = BSML::Lite::ArrayToSprite(Assets::Bandoot_jpg);
            githubSprite = BSML::Lite::ArrayToSprite(Assets::GitHub_png);
            helpSprite = BSML::Lite::ArrayToSprite(Assets::Help_png);
            settingsSprite = BSML::Lite::ArrayToSprite(Assets::Settings_png);
            resetSprite = BSML::Lite::ArrayToSprite(Assets::Reset_png);

            // All sub icons
            _resetSubIcon = std::make_shared<ClickableSubIcon>(ClickableSubIcon {
                .name = "Reset",
                .text = "Reset all config data",
                .buttonText = "Reset",
                .icon = resetSprite.ptr(),
                .onClick = [](UnityW<ImageFactory::UI::InfoView> self) {
                    auto imageManager = ImageFactory::ImageManager::get_instance();
                    if (imageManager) {
                        imageManager->RemoveAllImages();
                    }
                    ImageFactory::Config::Reset();
                }
            });

            _helpSubIcon = std::make_shared<ClickableSubIcon>(ClickableSubIcon {
                .name = "Help and FAQ",
                .text = "ImageFactory allows you to place custom images throughout your game. Everything has been designed to be easy for the user to use and customize. To learn more about how to use this mod, you can watch this video showcasing how to use it.",
                .buttonText = "<color=#ff3838>Open Youtube</color>",
                .icon = helpSprite.ptr(),
                .onClick = [](UnityW<ImageFactory::UI::InfoView> self) {
                    UnityEngine_Application_OpenURL("https://www.youtube.com/watch?v=biKDA6Fm5lI");
                }
            });

            _githubSubIcon = std::make_shared<ClickableSubIcon>(ClickableSubIcon{
                .name = "Github",
                .text = "ImageFactory is open source! You can view it on GitHub. Have a bug report or a feature request? Submit an issue on GitHub.",
                .buttonText = "Open Github in Browser",
                .icon = githubSprite.ptr(),
                .onClick = [](UnityW<ImageFactory::UI::InfoView> self) {
                    UnityEngine_Application_OpenURL("https://github.com/OptimusChen/ImageFactory-Quest");
                }
            });

            _optimusSubIcon = std::make_shared<ClickableSubIcon>(ClickableSubIcon{
                .name = "Optimus",
                .text = "sum random guy named optimus ported this mod to quest",
                .buttonText = "Github Link",
                .icon = optimusSprite.ptr(),
                .onClick = [](UnityW<ImageFactory::UI::InfoView> self) {
                    UnityEngine_Application_OpenURL("https://github.com/OptimusChen");
                }
            });

            _bandootSubIcon = std::make_shared<ClickableSubIcon>(ClickableSubIcon{
                .name = "Bandoot",
                .text = "Bandoot commissioned for this mod to be made, go check him out!",
                .buttonText = "Opens Twitch in Browser",
                .icon = bandootSprite.ptr(),
                .onClick = [](UnityW<ImageFactory::UI::InfoView> self) {
                    UnityEngine_Application_OpenURL("https://twitch.tv/bandoot");
                }
            });
            
            BSML::parse_and_construct(Assets::info_view_bsml, this->get_transform(), this);
            
            #ifdef HotReload
                fileWatcher = get_gameObject()->AddComponent<HotReloadFileWatcher*>();
                fileWatcher->host = this;
                fileWatcher->filePath = "/sdcard/bsml/ImageFactory/info-view.bsml";
                fileWatcher->checkInterval = 0.5f;
            #endif

            return;
        }
    }

    void InfoView::PostParse() {
        // This method can be used to perform additional setup after the BSML has been parsed.
        // For example, you can set up event listeners or modify UI elements here.
        // Set all images
        DEBUG("Setting up images for the info view");
        if (!logoImage) {
            DEBUG("Logo image is null, cannot set sprite");
            return;
        }
        logoImage->set_sprite(logoSprite);
        settingsImage->set_sprite(settingsSprite);
        resetImage->set_sprite(resetSprite);
        helpImage->set_sprite(helpSprite);
        githubImage->set_sprite(githubSprite);
        optimusImage->set_sprite(optimusSprite);
        bandootImage->set_sprite(bandootSprite);

        DEBUG("Setting up images for the info view completed");
        // Reopen modal if it was open before
        if (_currentModal) {
            ShowModal(_currentModal.value());
        }

        // Restpre settings modal state
        if (isSettingsOpen) {
            settingsModal->Show();
        }
    }

    void InfoView::ShowModal(std::shared_ptr<ClickableSubIcon> icon) {
        _currentModal = icon;

        modalImage->set_sprite(icon->icon.ptr());
        modal->Show();
        modalTitle->set_text(icon->name);
        modalText->set_text(icon->text);

        auto buttonText = modalActionButton->GetComponentInChildren<TMPro::TextMeshProUGUI*>();
        buttonText->set_text(icon->buttonText);
    }

    void InfoView::ClickedModalButton() {
        if (_currentModal) {
            _currentModal.value()->onClick(this);
            DismissModal();
        } else {
            DEBUG("No modal is currently open.");
        }
    }

    void InfoView::DismissModal() {
        // This method can be used to dismiss a modal view.
        // You can hide the modal view or remove it from the hierarchy here.
        // For example, you can hide the settings modal or the info modal.
        modal->Hide();
        _currentModal = std::nullopt;  // Clear the current modal
        DEBUG("Modal dismissed.");
    }



    // Modal buttons
    void InfoView::ClickedBandoot() {
        ShowModal(_bandootSubIcon);
    }
    void InfoView::ClickedOptimus() {
        ShowModal(_optimusSubIcon);
    }
    void InfoView::ClickedGitHub() {
        ShowModal(_githubSubIcon);
    }
    void InfoView::ClickedHelp() {
        ShowModal(_helpSubIcon);
    }
    void InfoView::ClickedReset() {
        ShowModal(_resetSubIcon);
    }
    void InfoView::OpenSettings() {
        settingsModal->Show();
        isSettingsOpen = true;
    }
    void InfoView::CloseSettings() {
        settingsModal->Hide();
        isSettingsOpen = false;
    }


    // Property getters
    bool InfoView::get_settingsEnabled() {
        return getPluginConfig().Enabled.GetValue();
    }
    bool InfoView::get_settingsAnimationsEnabled() {
        return getPluginConfig().AllowAnimations.GetValue();
    }
    bool InfoView::get_settingsIgnoreNoText() {
        return getPluginConfig().IgnoreTextAndHUDs.GetValue();
    }
    
    // Property setters
    void InfoView::set_settingsEnabled(bool value) {
        getPluginConfig().Enabled.SetValue(value);
       
        // If the plugin is disabled, despawn all images
        if (!value) {
            for (std::pair<IFImage*, StringW> pair : *PresenterManager::MAP) {
                pair.first->Despawn(false);
            }
        } else {
            PresenterManager::SpawnInMenu();
        }
    }
    void InfoView::set_settingsAnimationsEnabled(bool value) {
        getPluginConfig().AllowAnimations.SetValue(value);
    }
    void InfoView::set_settingsIgnoreNoText(bool value) {
        getPluginConfig().IgnoreTextAndHUDs.SetValue(value);
    }

}  // namespace ImageFactory::UI
