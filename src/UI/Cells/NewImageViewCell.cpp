#include "UI/Cells/NewImageViewCell.hpp"

DEFINE_TYPE(ImageFactory::UI::Cells, NewImageViewCell);

void ImageFactory::UI::Cells::NewImageViewCell::SelectionDidChange(HMUI::SelectableCell::TransitionType transitionType) {

}

void ImageFactory::UI::Cells::NewImageViewCell::HighlightDidChange(HMUI::SelectableCell::TransitionType transitionType) {

}

void ImageFactory::UI::Cells::NewImageViewCell::WasPreparedForReuse() {

}
void ImageFactory::UI::Cells::NewImageViewCell::ClickedCreateButton() {
    // This is where the logic for creating a new image would go.
    // For now, we can just log that the button was clicked.
}