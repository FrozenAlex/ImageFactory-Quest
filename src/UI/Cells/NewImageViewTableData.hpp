#pragma once

#include "assets.hpp"
#include "bsml/shared/BSML.hpp"
#include "HMUI/Touchable.hpp"
#include "UI/Cells/NewImageViewCell.hpp"
#include "logging.hpp"

namespace ImageFactory::UI::Cells {
    class NewImageViewTableData {
       public:
        static NewImageViewCell* GetCell(HMUI::TableView* tableView) {
            std::string const ReuseIdentifier = "REUSENewImageViewCell";
            auto tableCell = tableView->DequeueReusableCellForIdentifier(ReuseIdentifier);
            if (!tableCell) {
                tableCell = UnityEngine::GameObject::New_ctor("NewImageViewCell")->AddComponent<NewImageViewCell*>();
                tableCell->set_interactable(true);
                tableCell->set_reuseIdentifier(ReuseIdentifier);
                BSML::parse_and_construct(Assets::new_image_view_cell_bsml, tableCell->get_transform(), tableCell);
                // Weird hack cause HMUI touchable is not there for some reason
                tableCell->get_gameObject()->AddComponent<HMUI::Touchable*>();
            }

            return tableCell.cast<NewImageViewCell>();
        }
    };
}
