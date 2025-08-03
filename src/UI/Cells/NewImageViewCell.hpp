#pragma once

#include <memory>
#include <optional>
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
#include "Models/IFSourceImage.hpp"
#include "UnityEngine/UI/Image.hpp"

DECLARE_CLASS_CODEGEN(ImageFactory::UI::Cells, NewImageViewCell, HMUI::TableCell) {
    DECLARE_OVERRIDE_METHOD_MATCH(void, SelectionDidChange, &HMUI::SelectableCell::SelectionDidChange, HMUI::SelectableCell::TransitionType transitionType);
    DECLARE_OVERRIDE_METHOD_MATCH(void, HighlightDidChange, &HMUI::SelectableCell::HighlightDidChange, HMUI::SelectableCell::TransitionType transitionType);
    DECLARE_OVERRIDE_METHOD_MATCH(void, WasPreparedForReuse, &HMUI::TableCell::WasPreparedForReuse);
    DECLARE_INSTANCE_FIELD(UnityW<UnityEngine::UI::Image>, preview);
    DECLARE_INSTANCE_FIELD(UnityW<TMPro::TextMeshProUGUI>, fileName);

    DECLARE_INSTANCE_METHOD(void, ClickedCreateButton);
    DECLARE_INSTANCE_METHOD(void, OnEnable);
    DECLARE_INSTANCE_METHOD(void, OnDestroy);
public:
    bool needsImageLoad = false;
    std::optional<std::shared_ptr<ImageFactory::Models::IFSourceImage>> imageData;
    NewImageViewCell* PopulateWithImageData(std::shared_ptr<ImageFactory::Models::IFSourceImage> image);
};
