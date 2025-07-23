#pragma once

#include "HMUI/ViewController.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/GameObject.hpp"
#include "bsml/shared/BSML/Components/Backgroundable.hpp"
#include "custom-types/shared/macros.hpp"
#include "custom-types/shared/coroutine.hpp"
#include "GlobalNamespace/LoadingControl.hpp"
#include "IFImage.hpp"
#include "PluginConfig.hpp"
#include "bsml/shared/BSML/Components/HotReloadFileWatcher.hpp"
#include "bsml/shared/BSML/Components/CustomListTableData.hpp"
#include "HMUI/TableView.hpp"
#include "UnityEngine/UI/HorizontalOrVerticalLayoutGroup.hpp"

DECLARE_CLASS_CODEGEN_INTERFACES(ImageFactory::UI, SavedImageView, HMUI::ViewController, HMUI::TableView::IDataSource*) {
    #ifdef HotReload
    DECLARE_INSTANCE_FIELD(BSML::HotReloadFileWatcher*, fileWatcher);
    #endif

    DECLARE_INSTANCE_METHOD(void, PostParse);
    DECLARE_OVERRIDE_METHOD_MATCH(
        void, DidActivate, &HMUI::ViewController::DidActivate, bool firstActivation, bool addedToHeirarchy, bool screenSystemDisabling
    );
    DECLARE_INSTANCE_FIELD(UnityW<UnityEngine::UI::HorizontalOrVerticalLayoutGroup>, selectionRoot);
    DECLARE_INSTANCE_FIELD(UnityW<UnityEngine::UI::HorizontalOrVerticalLayoutGroup>, loadingRoot);
    DECLARE_INSTANCE_FIELD(UnityW<HMUI::CurvedTextMeshPro>, loadingText);

    DECLARE_INSTANCE_FIELD(UnityW<BSML::ModalView>, imageDetailsModal);
    DECLARE_INSTANCE_FIELD(UnityW<HMUI::CurvedTextMeshPro>, animText);
    DECLARE_INSTANCE_FIELD(UnityW<HMUI::CurvedTextMeshPro>, widthText);
    DECLARE_INSTANCE_FIELD(UnityW<HMUI::CurvedTextMeshPro>, heightText);
    DECLARE_INSTANCE_FIELD(UnityW<HMUI::CurvedTextMeshPro>, fileSizeText);
    DECLARE_INSTANCE_FIELD(UnityW<HMUI::CurvedTextMeshPro>, loadTimeText);

    DECLARE_INSTANCE_FIELD(UnityEngine::Transform*, containerParent);
    DECLARE_INSTANCE_FIELD(UnityEngine::GameObject*, container);
    DECLARE_INSTANCE_METHOD(void, Refresh, IFImage*);
    DECLARE_INSTANCE_METHOD(void, ClearList);
    
    // Table stuff
    DECLARE_INSTANCE_FIELD(UnityW<BSML::CustomListTableData>, imageTableData);
    DECLARE_INSTANCE_FIELD(float, cellSize);
    DECLARE_OVERRIDE_METHOD_MATCH(HMUI::TableCell*, CellForIdx, &HMUI::TableView::IDataSource::CellForIdx, HMUI::TableView* tableView, int idx);
    DECLARE_OVERRIDE_METHOD_MATCH(float, CellSize, &HMUI::TableView::IDataSource::CellSize);
    DECLARE_OVERRIDE_METHOD_MATCH(int, NumberOfCells, &HMUI::TableView::IDataSource::NumberOfCells);
    DECLARE_INSTANCE_METHOD(void, PageUp);
    DECLARE_INSTANCE_METHOD(void, PageDown);
    
    void CreateListElement(UnityW<UnityEngine::Transform> parent, bool isEditing, UnityW<IFImage> image, std::string filename);
    private:
        UnityW<HMUI::TableView> imagesTable() {if(imageTableData) {return imageTableData->tableView;} else return nullptr;}
        custom_types::Helpers::Coroutine SetupListElements();
        std::vector<UnityW<UnityEngine::GameObject>> elems;
};