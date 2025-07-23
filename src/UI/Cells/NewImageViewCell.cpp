#include "UI/Cells/NewImageViewCell.hpp"
#include "bsml/shared/Helpers/utilities.hpp"
#include "logging.hpp"
#include "assets.hpp"

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

ImageFactory::UI::Cells::NewImageViewCell* ImageFactory::UI::Cells::NewImageViewCell::PopulateWithImageData(std::shared_ptr<ImageFactory::Models::IFSourceImage> image) {
    auto bsmlPath = image->path;
    if (!bsmlPath.starts_with("file://")) {
        bsmlPath = "file://" + bsmlPath; // Ensure the path is a valid file URL
    }
    this->fileName->set_text(image->fileName);
    
    if (this->preview != nullptr) {
        BSML::Utilities::SetImage(this->preview,  bsmlPath, false, BSML::Utilities::ScaleOptions(), true, nullptr, nullptr);
    } else {
        DEBUG("Preview is null. Cannot set image.");
    }
    return this;
};
