#pragma once

#include "HMUI/ViewController.hpp"
#include "UnityEngine/Transform.hpp"
#include "custom-types/shared/macros.hpp"
#include "custom-types/shared/coroutine.hpp"

#include "GlobalNamespace/LoadingControl.hpp"
#include "HMUI/ImageView.hpp"
#include <list>
#include <map>
#include <memory>
#include <optional>
#include <vector>
#include "bsml/shared/BSML/Components/HotReloadFileWatcher.hpp"
#include "bsml/shared/BSML/Components/CustomListTableData.hpp"
#include "UnityEngine/UI/HorizontalOrVerticalLayoutGroup.hpp"
#include "HMUI/CurvedTextMeshPro.hpp"
#include "Models/IFSourceImage.hpp"
#include "bsml/shared/BSML/Components/ClickableImage.hpp"
#include "bsml/shared/BSML/Components/ModalView.hpp"

DECLARE_CLASS_CODEGEN_INTERFACES(ImageFactory::UI, NewImageView, HMUI::ViewController, HMUI::TableView::IDataSource*) {

    DECLARE_OVERRIDE_METHOD_MATCH(void, DidActivate, &HMUI::ViewController::DidActivate, bool firstActivation, bool addedToHeirarchy, bool screenSystemDisabling);
    DECLARE_INSTANCE_METHOD(void, PostParse);
    DECLARE_INSTANCE_FIELD(UnityW<UnityEngine::UI::HorizontalOrVerticalLayoutGroup>, selectionRoot);
    DECLARE_INSTANCE_FIELD(UnityW<UnityEngine::UI::HorizontalOrVerticalLayoutGroup>, loadingRoot);
    DECLARE_INSTANCE_FIELD(UnityW<HMUI::CurvedTextMeshPro>, loadingText);

    // Table stuff
    DECLARE_INSTANCE_FIELD(UnityW<BSML::CustomListTableData>, imageTableData);
    DECLARE_INSTANCE_FIELD(float, cellSize);
    DECLARE_OVERRIDE_METHOD_MATCH(HMUI::TableCell*, CellForIdx, &HMUI::TableView::IDataSource::CellForIdx, HMUI::TableView* tableView, int idx);
    DECLARE_OVERRIDE_METHOD_MATCH(float, CellSize, &HMUI::TableView::IDataSource::CellSize);
    DECLARE_OVERRIDE_METHOD_MATCH(int, NumberOfCells, &HMUI::TableView::IDataSource::NumberOfCells);
    DECLARE_INSTANCE_METHOD(void, PageUp);
    DECLARE_INSTANCE_METHOD(void, PageDown);
    DECLARE_INSTANCE_METHOD(void, CancelCreationClicked);
    DECLARE_INSTANCE_METHOD(void, CreateClicked);

    DECLARE_INSTANCE_FIELD(UnityW<BSML::ModalView>, imageDetailsModal);
    DECLARE_INSTANCE_FIELD(UnityW<HMUI::ImageView>, preview);

    DECLARE_INSTANCE_FIELD(UnityW<HMUI::CurvedTextMeshPro>, animText);
    DECLARE_INSTANCE_FIELD(UnityW<HMUI::CurvedTextMeshPro>, widthText);
    DECLARE_INSTANCE_FIELD(UnityW<HMUI::CurvedTextMeshPro>, heightText);
    DECLARE_INSTANCE_FIELD(UnityW<HMUI::CurvedTextMeshPro>, fileSizeText);
    DECLARE_INSTANCE_FIELD(UnityW<HMUI::CurvedTextMeshPro>, loadTimeText);

    #ifdef HotReload
    DECLARE_INSTANCE_FIELD(BSML::HotReloadFileWatcher*, fileWatcher);
    #endif
    public: 
        void SelectImage(std::shared_ptr<ImageFactory::Models::IFSourceImage> image);
    private:
        UnityW<HMUI::TableView> imagesTable() {if(imageTableData) {return imageTableData->tableView;} else return nullptr;}
        custom_types::Helpers::Coroutine SetupListElements();
        
        void ToggleModal(bool enable);
        std::vector<std::shared_ptr<ImageFactory::Models::IFSourceImage>> imagesList;
        std::optional<std::shared_ptr<ImageFactory::Models::IFSourceImage>> selectedImage;
};
