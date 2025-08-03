#include "UI/Cells/NewImageViewCell.hpp"
#include "UI/NewImageView.hpp"
#include "bsml/shared/Helpers/utilities.hpp"
#include "logging.hpp"
#include "assets.hpp"
#include "custom-types/shared/coroutine.hpp"
#include "bsml/shared/BSML-Lite/Creation/Text.hpp"
#include "bsml/shared/BSML-Lite/Creation/Image.hpp"
#include "bsml/shared/BSML-Lite/Creation/Buttons.hpp"
#include "bsml/shared/BSML-Lite/Creation/Layout.hpp"
#include "bsml/shared/BSML/SharedCoroutineStarter.hpp"
#include "bsml/shared/BSML/MainThreadScheduler.hpp"
#include "Utils/UIUtils.hpp"
#include "UnityEngine/WaitForEndOfFrame.hpp"
#include "UnityEngine/WaitForSeconds.hpp"
#include "UI/NewImageView.hpp"

DEFINE_TYPE(ImageFactory::UI::Cells, NewImageViewCell);

void ImageFactory::UI::Cells::NewImageViewCell::SelectionDidChange(HMUI::SelectableCell::TransitionType transitionType) {

}

void ImageFactory::UI::Cells::NewImageViewCell::HighlightDidChange(HMUI::SelectableCell::TransitionType transitionType) {

}

void ImageFactory::UI::Cells::NewImageViewCell::WasPreparedForReuse() {
    needsImageLoad = false;
    imageData = std::nullopt; // Reset the image data when the cell is reused
}
void ImageFactory::UI::Cells::NewImageViewCell::ClickedCreateButton() {
    // This is where the logic for creating a new image would go.
    // For now, we can just log that the button was clicked.
    UnityW<ImageFactory::UI::NewImageView> parent = GetComponentInParent<ImageFactory::UI::NewImageView*>();
    if (!parent) {
        WARNING("Somehow the new image view does not exist so not doing anything");
        return;
    }
    
    if (!imageData) {
        WARNING("Image data is not set, avoiding crash");
        return;
    }
    DEBUG("Setting the current image");
    parent->SelectImage(imageData.value());
}

void ImageFactory::UI::Cells::NewImageViewCell::OnEnable() {
    DEBUG("NewImageViewCell::OnEnable called");
    
    
}

void ImageFactory::UI::Cells::NewImageViewCell::OnDestroy() {
    DEBUG("NewImageViewCell::OnDestroy called on {}", fmt::ptr(this));
    Logger.Backtrace(100);
}


ImageFactory::UI::Cells::NewImageViewCell* ImageFactory::UI::Cells::NewImageViewCell::PopulateWithImageData(std::shared_ptr<ImageFactory::Models::IFSourceImage> image) {
    imageData = image; // Store the image data for later use

    this->fileName->set_text(imageData.value()->fileName);
    auto bsmlPath = imageData.value()->path;

    if (!bsmlPath.starts_with("file://")) {
        bsmlPath = "file://" + bsmlPath; // Ensure the path is a valid file URL
    }
    if (this->preview) {
        BSML::Utilities::SetImage(this->preview,  bsmlPath);
    }
    return this;
};
