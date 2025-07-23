#pragma once

#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/UI/VerticalLayoutGroup.hpp"
#include "custom-types/shared/macros.hpp"
#include "HMUI/ViewController.hpp"
#include "HMUI/TableView.hpp"
#include "HMUI/TableCell.hpp"
#include "System/Object.hpp"
#include "TMPro/TextMeshProUGUI.hpp"
#include "bsml/shared/macros.hpp"
#include "bsml/shared/BSML.hpp"
#include "bsml/shared/BSML/Components/CustomListTableData.hpp"


DECLARE_CLASS_CODEGEN(ImageFactory::UI::Cells, SavedImageViewCell, HMUI::TableCell) {
    DECLARE_OVERRIDE_METHOD_MATCH(void, SelectionDidChange, &HMUI::SelectableCell::SelectionDidChange, HMUI::SelectableCell::TransitionType transitionType);
    DECLARE_OVERRIDE_METHOD_MATCH(void, HighlightDidChange, &HMUI::SelectableCell::HighlightDidChange, HMUI::SelectableCell::TransitionType transitionType);
    DECLARE_OVERRIDE_METHOD_MATCH(void, WasPreparedForReuse, &HMUI::TableCell::WasPreparedForReuse);
    DECLARE_INSTANCE_FIELD(HMUI::ImageView*, preview);
    DECLARE_INSTANCE_FIELD(TMPro::TextMeshProUGUI*, saveName);

    DECLARE_INSTANCE_METHOD(void, ClickedDeleteButton);
    DECLARE_INSTANCE_METHOD(void, ClickedEditButton);


public:
    // DownloadHistoryEntry* entry;

    // CustomDownloadListTableCell* PopulateWithImageData(DownloadHistoryEntry* entry);
};
