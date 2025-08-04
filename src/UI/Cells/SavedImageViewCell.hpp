#pragma once

#include "custom-types/shared/macros.hpp"
#include "HMUI/TableView.hpp"
#include "HMUI/TableCell.hpp"
#include "TMPro/TextMeshProUGUI.hpp"
#include "HMUI/ImageView.hpp"


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
