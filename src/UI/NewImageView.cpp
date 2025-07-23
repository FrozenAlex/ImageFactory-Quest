#include "UI/NewImageView.hpp"
#include "UI/ImageFactoryFlowCoordinator.hpp"
#include "assets.hpp"
#include "logging.hpp"
#include "Utils/UIUtils.hpp"
#include "Utils/FileUtils.hpp"
#include "Utils/StringUtils.hpp"
#include "HMUI/Touchable.hpp"
#include "System/IO/FileStream.hpp"
#include "System/IO/FileMode.hpp"
#include "System/Diagnostics/Stopwatch.hpp"
#include "UnityEngine/Rect.hpp"
#include "UnityEngine/Resources.hpp"
#include "UnityEngine/WaitForSeconds.hpp"
#include "UnityEngine/HideFlags.hpp"
#include "bsml/shared/BSML/SharedCoroutineStarter.hpp"
#include "bsml/shared/BSML/MainThreadScheduler.hpp"
#include "bsml/shared/BSML-Lite/Creation/Text.hpp"
#include "bsml/shared/BSML-Lite/Creation/Image.hpp"
#include "bsml/shared/BSML-Lite/Creation/Buttons.hpp"
#include "bsml/shared/BSML-Lite/Creation/Layout.hpp"
#include "custom-types/shared/delegate.hpp"
#include "bsml/shared/Helpers/utilities.hpp"
#include "HMUI/ScrollView.hpp"
#include "UI/Cells/NewImageViewTableData.hpp"

DEFINE_TYPE(ImageFactory::UI, NewImageView);

using namespace GlobalNamespace;
using namespace UnityEngine::UI;
using namespace UnityEngine;
using namespace System;
using namespace HMUI;

#define SetPreferredSize(identifier, width, height)                                         \
    auto layout##identifier = identifier->get_gameObject()->GetComponent<LayoutElement*>(); \
    if (!layout##identifier)                                                                \
        layout##identifier = identifier->get_gameObject()->AddComponent<LayoutElement*>();  \
    layout##identifier->set_preferredWidth(width);                                          \
    layout##identifier->set_preferredHeight(height)

namespace ImageFactory::UI {

    void NewImageView::DidActivate(bool firstActivation,
                                              bool addedToHierarchy,
                                              bool screenSystemEnabling) {
        if (firstActivation) 
        {
            BSML::parse_and_construct(Assets::new_image_view_bsml, this->get_transform(), this);
            #ifdef HotReload
                fileWatcher = get_gameObject()->AddComponent<HotReloadFileWatcher*>();
                fileWatcher->host = this;
                fileWatcher->filePath = "/sdcard/bsml/ImageFactory/new-image-view.bsml";
                fileWatcher->checkInterval = 0.5f;
            #endif

            this->StartCoroutine(
                custom_types::Helpers::CoroutineHelper::New(
                    SetupListElements()
                )
            );
        }
    }

    void NewImageView::PostParse() {
        // This method can be used to perform additional setup after the BSML has been parsed.
        // For example, you can set up event listeners or modify UI elements here.
        if (imageTableData) {
            imageTableData->tableView->SetDataSource(reinterpret_cast<HMUI::TableView::IDataSource*>(this), false);
            imageTableData->tableView->ReloadData();
        }
    }

    float NewImageView::CellSize() {
        DEBUG("NewImageView::CellSize called, returning cell size:");
        return imageTableData->CellSize();
    }

    int NewImageView::NumberOfCells() {
        DEBUG("NewImageView::NumberOfCells called, imagesList size: {}", imagesList.size());
        return imagesList.size();
    }

    HMUI::TableCell* NewImageView::CellForIdx(HMUI::TableView* tableView, int idx) {
        DEBUG("NewImageView::CellForIdx called with idx: {}", idx);
        if (idx < 0 || idx >= imagesList.size()) {
            ERROR("Index out of bounds: {}", idx);
            return nullptr; // Return nullptr to indicate an invalid cell
        }
        return Cells::NewImageViewTableData::GetCell(tableView)->PopulateWithImageData(imagesList[idx]);
    }

    void NewImageView::PageUp() {
        if (imageTableData && imageTableData->tableView) {
            imageTableData->tableView->get_scrollView()->PageUpButtonPressed();
        }
    }
    void NewImageView::PageDown() {
        if (imageTableData && imageTableData->tableView) {
            imageTableData->tableView->get_scrollView()->PageDownButtonPressed();
        }
    }

    custom_types::Helpers::Coroutine NewImageView::SetupListElements(){
        std::vector<std::string> images = FileUtils::getFiles("/sdcard/ModData/com.beatgames.beatsaber/Mods/ImageFactory/Images/");

        if (images.empty()) {
            DEBUG("No images found in folder!");
            loadingRoot->get_gameObject()->SetActive(true);
            loadingText->set_text("No images found in folder!\n/sdCard/ModData/com.beatgames/Mods/ImageFactory/Images/");
            co_return;
        }
        DEBUG("Found {} images in folder!", images.size());
        for (int i = 0; i < images.size(); i++) {
            auto image = images.at(i);
            DEBUG("Loading image: {}", image);

            auto imageData = std::make_shared<ImageFactory::Models::IFSourceImage>();
            System::IO::FileStream* stream = System::IO::FileStream::New_ctor(image, System::IO::FileMode::Open, ::System::IO::FileAccess::Read);
            auto fileSize = stream->get_Length();
            imageData->path = image;
            imageData->fileName = FileUtils::GetFileName(image, false);
            imageData->size = "File Size: " +
                        StringUtils::removeTrailingZeros(round(fileSize / FileUtils::FileSizeDivisor(fileSize))) +
                         " " + FileUtils::FileSizeExtension(fileSize);
            imageData->animated = FileUtils::isGifFile(image);
            imagesList.push_back(imageData);
            stream->Close();
        }


        // // Show loading screen and hide the selection root
        // loadingRoot->get_gameObject()->SetActive(true);
        // selectionRoot->get_gameObject()->SetActive(false);
        // loadingText->set_text("Loading Images...");

        // Diagnostics::Stopwatch* watch = Diagnostics::Stopwatch::New_ctor();

        // // Disable the table
        // imageTableData->tableView->get_gameObject()->SetActive(false);

        // for (int i = 0; i < images.size(); i++) {
        //     watch->Reset();
        //     watch->Start();

        //     auto image = images.at(i);

        //     HorizontalLayoutGroup* levelBarLayout = BSML::Lite::CreateHorizontalLayoutGroup(list->get_transform());

        //     levelBarLayout->set_childControlWidth(false);

        //     LayoutElement* levelBarLayoutElement = levelBarLayout->GetComponent<LayoutElement*>();
        //     levelBarLayoutElement->set_minHeight(10.0f);
        //     levelBarLayoutElement->set_minWidth(20.0f);

        //     SafePtrUnity<Sprite> sprite = nullptr;
        //     if (FileUtils::isGifFile(image)) {
        //         sprite = UIUtils::FirstFrame(image);
        //     } else {
        //         sprite = BSML::Lite::FileToSprite(image);
        //     }
        //     if (!sprite) {
        //         DEBUG("Failed to load sprite from image: {}", image);
        //         continue;
        //     }
        //     sprite->set_hideFlags(HideFlags::None);
        //     Object::DontDestroyOnLoad(sprite.ptr());
        
        //     co_yield reinterpret_cast<Collections::IEnumerator*>(CRASH_UNLESS(WaitForSeconds::New_ctor(0.3f)));

        //     auto img = BSML::Lite::CreateImage(levelBarLayoutElement->get_transform(), sprite.ptr(), Vector2(2.0f, 0.0f), Vector2(10.0f, 2.0f));

        //     SetPreferredSize(img, 10.0f, 2.0f);

        //     System::IO::FileStream* stream = System::IO::FileStream::New_ctor(image, System::IO::FileMode::Open, ::System::IO::FileAccess::Read);

        //     long fileSize = stream->get_Length();
        //     auto loadTime = watch->get_ElapsedMilliseconds();

        //     BSML::Lite::CreateText(levelBarLayoutElement->get_transform(), FileUtils::GetFileName(image, false), true);
           
        //     levelBarLayoutElement->set_minWidth(1.0f);

        //     auto button = BSML::Lite::CreateUIButton(levelBarLayoutElement->get_transform(), "", Vector2(0.0f, 0.0f), Vector2(10.0f, 10.0f),
        //         [this, image, img, fileSize, loadTime]() {
        //             auto modal = BSML::Lite::CreateModal(get_transform(), Vector2(70.0f, 50.0f),
        //                 nullptr, true);

        //             auto container = BSML::Lite::CreateScrollableModalContainer(modal);

        //             auto imgModal = BSML::Lite::CreateImage(modal->get_transform(), img->get_sprite(), Vector2(-18.0f, 8.0f),
        //                 Vector2(30.0f, 30.0f));

        //             auto anim = BSML::Lite::CreateText(modal->get_transform(), "Animated: No",
        //                 Vector2(30.0f, 17.0f));

        //             if (FileUtils::isGifFile(image)) {
        //                 anim->set_text("Animated: Yes");
        //             }

        //             anim->set_fontSize(5.0f);

        //             BSML::Lite::CreateText(modal->get_transform(), "Width: " +
        //                 StringUtils::removeTrailingZeros(ceil(img->get_sprite()->get_textureRect().get_width())) + "px", Vector2(30.0f, 11.0f))->set_fontSize(5.0f);
        //             BSML::Lite::CreateText(modal->get_transform(),  "Height: " +
        //                 StringUtils::removeTrailingZeros(ceil(img->get_sprite()->get_textureRect().get_height())) + "px", Vector2(30.0f, 5.0f))->set_fontSize(5.0f);

        //             BSML::Lite::CreateText(modal->get_transform(), "File Size: " +
        //                 StringUtils::removeTrailingZeros(round(fileSize / FileUtils::FileSizeDivisor(fileSize))) +
        //                 " " + FileUtils::FileSizeExtension(fileSize), Vector2(30.0f, -1.0f))->set_fontSize(5.0f);

        //             BSML::Lite::CreateText(modal->get_transform(), "Load Time: " + std::to_string(loadTime) + " ms", Vector2(30.0f, -7.0f))->set_fontSize(5.0f);
                    
        //             auto create = BSML::Lite::CreateUIButton(modal->get_transform(), "CREATE", Vector2(14.0f, -17.0f),
        //                 Vector2(30.0f, 10.0f), [=]() { 
        //                     ImageFactoryFlowCoordinator* flow = Object::FindObjectsOfType<ImageFactoryFlowCoordinator*>()->First();

        //                     if (flow) {
        //                         flow->CreateImage(image);
        //                     }
        //                 });

        //             BSML::Lite::CreateUIButton(modal->get_transform(), "CANCEL", Vector2(-18.0f, -17.0f),
        //                 Vector2(30.0f, 10.0f), [=]() { 
        //                     modal->Hide();
        //                 });

        //             if (FileUtils::isGifFile(image)) {
        //                 create->set_interactable(false);
        //                 imgModal->set_sprite(UIUtils::FirstFrame(image));
        //                 BSML::Utilities::SetImage(imgModal, "file://" + image, false, BSML::Utilities::ScaleOptions(), [=](){
        //                     create->set_interactable(true);
        //                 });
        //             }

        //             modal->Show();
        //         });

        //     BSML::Lite::CreateText(button->get_transform(), "+")->set_alignment(TMPro::TextAlignmentOptions::Center);

        //     co_yield reinterpret_cast<Collections::IEnumerator*>(CRASH_UNLESS(WaitForSeconds::New_ctor(0.1f)));

        //     loadingText->set_text("Loading Images... (" + std::to_string(i + 1) + "/" + std::to_string(images.size()) + ")");
        // }

        // // watch->Stop();

        // co_yield reinterpret_cast<Collections::IEnumerator*>(CRASH_UNLESS(WaitForSeconds::New_ctor(0.5f)));
        
        // list->get_gameObject()->set_active(true);

        // if (images.empty()) {
        //     loadingRoot->get_gameObject()->SetActive(true);
        //     loadingText->set_text("No images found in folder!\n/sdCard/ModData/com.beatgames/Mods/ImageFactory/Images/");
        // } else {
        //     loadingRoot->get_gameObject()->SetActive(false);
        // }
        // Reload data for the table view
        if (imagesTable()) {
            imagesTable()->get_gameObject()->SetActive(true);
            imagesTable()->ReloadData();
        }
        co_return;
    }
}
