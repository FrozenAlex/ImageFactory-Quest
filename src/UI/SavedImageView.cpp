#include "UI/SavedImageView.hpp"

#include "ConfigManager.hpp"
#include "bsml/shared/BSML/Animations/AnimationStateUpdater.hpp"
#include "UI/ImageFactoryFlowCoordinator.hpp"
#include "Utils/UIUtils.hpp"
#include "Utils/FileUtils.hpp"
#include "HMUI/Touchable.hpp"
#include "Presenters/PresenterManager.hpp"
#include "UnityEngine/Resources.hpp"
#include "UnityEngine/WaitForSeconds.hpp"
#include "bsml/shared/BSML/SharedCoroutineStarter.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-functions.hpp"
#include "beatsaber-hook/shared/utils/typedefs-string.hpp"
#include "HMUI/ViewController.hpp"
#include "PluginConfig.hpp"
#include "bsml/shared/BSML-Lite/Creation/Text.hpp"
#include "bsml/shared/BSML-Lite/Creation/Image.hpp"
#include "bsml/shared/BSML-Lite/Creation/Layout.hpp"
#include "ImageManager.hpp"
#include "logging.hpp"
#include "assets.hpp"
#include "UnityEngine/WaitForEndOfFrame.hpp"
#include "HMUI/ScrollView.hpp"
#include "UI/Cells/SavedImageViewTableData.hpp"
DEFINE_TYPE(ImageFactory::UI, SavedImageView);

using namespace GlobalNamespace;
using namespace UnityEngine::UI;
using namespace UnityEngine;
using namespace HMUI;

#define SetPreferredSize(identifier, width, height)                                         \
    auto layout##identifier = identifier->get_gameObject()->GetComponent<LayoutElement*>(); \
    if (!layout##identifier)                                                                \
        layout##identifier = identifier->get_gameObject()->AddComponent<LayoutElement*>();  \
    layout##identifier->set_preferredWidth(width);                                          \
    layout##identifier->set_preferredHeight(height)

namespace ImageFactory::UI {

    void SavedImageView::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
        if (firstActivation) 
        {
            DEBUG("SavedImageView::DidActivate called");

            BSML::parse_and_construct(Assets::saved_image_view_bsml, this->get_transform(), this);
            DEBUG("SavedImageView::DidActivate called");
            #ifdef HotReload
                fileWatcher = get_gameObject()->AddComponent<HotReloadFileWatcher*>();
                fileWatcher->host = this;
                fileWatcher->filePath = "/sdcard/bsml/ImageFactory/saved-image-view.bsml";
                fileWatcher->checkInterval = 0.5f;
            #endif

            DEBUG("SavedImageView::DidActivate called");
            StartCoroutine(
                custom_types::Helpers::new_coro(
                    SetupListElements()
                )
            );
        } else {
            if (loadingRoot->get_gameObject()->get_active()) {
                loadingRoot->get_gameObject()->SetActive(false);
            }
        }
    }

    void SavedImageView::PostParse() {
        // This method can be used to perform additional setup after the BSML has been parsed.
        // For example, you can set up event listeners or modify UI elements here.
        if (imageTableData) {
            imageTableData->tableView->SetDataSource(reinterpret_cast<HMUI::TableView::IDataSource*>(this), false);
            imageTableData->tableView->ReloadData();
        }
    }

    float SavedImageView::CellSize() {
        return imageTableData->CellSize(); // This method should return the size of each cell in the table view.
    }

    int SavedImageView::NumberOfCells() {
        return 12; // TODO: return the number of images
    }

    HMUI::TableCell* SavedImageView::CellForIdx(HMUI::TableView* tableView, int idx) {
        // TODO get image data from image list here
        return Cells::SavedImageViewTableData::GetCell(tableView); 
    }

    void SavedImageView::PageUp() {
        if (imageTableData && imageTableData->tableView) {
            imageTableData->tableView->get_scrollView()->PageUpButtonPressed();
        }
    }
    void SavedImageView::PageDown() {
        if (imageTableData && imageTableData->tableView) {
            imageTableData->tableView->get_scrollView()->PageDownButtonPressed();
        }
    }

    custom_types::Helpers::Coroutine SavedImageView::SetupListElements(){
        DEBUG("SavedImageView::DidActivate called");
        
        // Wait for table to be created
        co_yield reinterpret_cast<System::Collections::IEnumerator*>(UnityEngine::WaitForEndOfFrame::New_ctor());

        auto table = imagesTable();
        if (!table) {
            DEBUG("SavedImageView::DidActivate called - table is null");
            co_return;
        }

        // loadingRoot->get_gameObject()->SetActive(false);
        // loadingRoot->get_gameObject()->SetActive(false);
        // DEBUG("SavedImageView::DidActivate called");
        

        // imageTableData->tableView->get_gameObject()->set_active(false);

        // auto imageConfigs = getPluginConfig().SaveData.GetValue();

        // for (int i = 0; i < imageConfigs.size(); i++) {
        //     auto& imageConfig = imageConfigs.at(i);

        //     std::string fileName = imageConfig.LocalFilePath.value_or("");
        //     if (!fileName.empty()) {
        //         CreateListElement(list, false, nullptr, fileName);
        //     }

        //     loadingText->set_text("Loading Images... (" + std::to_string(i) + "/" + std::to_string(imageConfigs.size() - 1) + ")");

        //     co_yield reinterpret_cast<System::Collections::IEnumerator*>(CRASH_UNLESS(WaitForSeconds::New_ctor(0.4f)));
        // }

        // co_yield reinterpret_cast<System::Collections::IEnumerator*>(CRASH_UNLESS(WaitForSeconds::New_ctor(0.15f)));

        // if (elems.empty()) {
        //     // loadingControl->_refreshText->get_gameObject()->SetActive(true);
        //     loadingText->set_text("No Images Found in Config!");
        // } else {
        //     loadingRoot->get_gameObject()->SetActive(false);
        //     selectionRoot->get_gameObject()->SetActive(true);
        // }

        imagesTable()->get_gameObject()->SetActive(true);

        co_return;
    }

    void SavedImageView::Refresh(IFImage* image) {
        CreateListElement(container->get_transform(), true, image, image->internalName);
    }

    void SavedImageView::CreateListElement(UnityW<UnityEngine::Transform> list, bool refresh, UnityW<IFImage> ifimage, std::string fileName) {
        std::string path;
        std::string name;
        bool enabled;

        // If image exists, get its params for the item
        if (ifimage && refresh) {
            path = ifimage->path;
            enabled = ifimage->enabled;
            name = ifimage->name;
        } else {
            auto config = ImageFactory::Config::GetConfigByImage(ifimage);
            if (config) {
                
            }
        }



        HorizontalLayoutGroup* levelBarLayout = BSML::Lite::CreateHorizontalLayoutGroup(list);
        levelBarLayout->set_childControlWidth(false);

        LayoutElement* levelBarLayoutElement = levelBarLayout->GetComponent<LayoutElement*>();
        levelBarLayoutElement->set_minHeight(10.0f);
        levelBarLayoutElement->set_minWidth(20.0f);
        DEBUG("Creating list element for image: {}", fileName);
        
        Sprite* sprite = BSML::Lite::FileToSprite(path);

        auto image = BSML::Lite::CreateImage(levelBarLayoutElement->get_transform(), sprite, {2.0f, 0.0f}, {10.0f, 2.0f});

        SetPreferredSize(image, 10.0f, 2.0f);

        if (FileUtils::isGifFile(path)) {
            image->set_sprite(UIUtils::FirstFrame(path));
        }

        LayoutElement* imgElem = image->GetComponent<LayoutElement*>();
        imgElem->set_preferredHeight(2.0f);
        imgElem->set_preferredWidth(10.0f);

        TMPro::TextMeshProUGUI* text = BSML::Lite::CreateText(levelBarLayoutElement->get_transform(), name, true);
        if (!enabled) {
            text->set_color(Color::get_red());
        } else {
            text->set_color(Color::get_green());
        }

        levelBarLayoutElement->set_minWidth(1.0f);
        levelBarLayoutElement->set_minHeight(1.0f);

        Button* deleteButton = BSML::Lite::CreateUIButton(levelBarLayoutElement->get_transform(), "", {0.0f, 0.0f}, {12.0f, 9.0f}, 
            [=]() {
                auto manager = ImageManager::get_instance();
                for (std::pair<IFImage*, std::string> pair : *PresenterManager::MAP) {
                    if (pair.first->internalName.starts_with(fileName)) {
                        manager->RemoveImage(pair.first);
                        levelBarLayout->get_gameObject()->set_active(false);
                        break;
                    }
                }
            });

        auto deleteText = BSML::Lite::CreateText(
            deleteButton->get_transform(), "X");
        deleteText->set_alignment(TMPro::TextAlignmentOptions::Center);
        deleteText->set_color(Color(1.0f, 0.0f, 0.0f, 1.0f));

        Button* editButton = BSML::Lite::CreateUIButton(levelBarLayoutElement->get_transform(), "", {0.0f, 0.0f}, {12.0f, 9.0f}, 
            [=]() {
                for (std::pair<IFImage*, std::string> pair : *PresenterManager::MAP) {
                    if (pair.first->fileName.starts_with(fileName)) {
                        Resources::FindObjectsOfTypeAll<ImageFactoryFlowCoordinator*>()->First()->EditImage(pair.first, text);
                        break;
                    }
                }
            });

        auto editText = BSML::Lite::CreateText(editButton->get_transform(), "<-");
        editText->set_alignment(TMPro::TextAlignmentOptions::Center);

        levelBarLayout->get_gameObject()->set_active(true);

        elems.push_back(levelBarLayout->get_gameObject());
    }

    void SavedImageView::ClearList() {
        for (int i = 0; i < elems.size(); i++) {
            elems.at(i)->set_active(false);
        }
    }
}

