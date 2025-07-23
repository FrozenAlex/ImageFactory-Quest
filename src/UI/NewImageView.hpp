#pragma once

#include "HMUI/ViewController.hpp"
#include "UnityEngine/Transform.hpp"
#include "custom-types/shared/macros.hpp"
#include "custom-types/shared/coroutine.hpp"

#include "GlobalNamespace/LoadingControl.hpp"
#include "HMUI/ImageView.hpp"
#include <map>
#include "bsml/shared/BSML/Components/HotReloadFileWatcher.hpp"
#include "bsml/shared/BSML/Components/CustomListTableData.hpp"
#include "UnityEngine/UI/HorizontalOrVerticalLayoutGroup.hpp"
#include "HMUI/CurvedTextMeshPro.hpp"


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

    #ifdef HotReload
    DECLARE_INSTANCE_FIELD(BSML::HotReloadFileWatcher*, fileWatcher);
    #endif

    private:
        UnityW<HMUI::TableView> imagesTable() {if(imageTableData) {return imageTableData->tableView;} else return nullptr;}
        custom_types::Helpers::Coroutine SetupListElements();
};
