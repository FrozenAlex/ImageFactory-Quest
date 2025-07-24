#include "UI/Cells/NewImageViewCell.hpp"
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
}

void ImageFactory::UI::Cells::NewImageViewCell::OnEnable() {
    DEBUG("NewImageViewCell::OnEnable called");
    
    
}

void ImageFactory::UI::Cells::NewImageViewCell::OnDestroy() {
    DEBUG("NewImageViewCell::OnDestroy called");
}

custom_types::Helpers::Coroutine ImageFactory::UI::Cells::NewImageViewCell::LoadImageDate() {
    DEBUG("Cell addr in coro, {}", fmt::ptr(this));
co_yield reinterpret_cast<System::Collections::IEnumerator*>(UnityEngine::WaitForSeconds::New_ctor(4.0f));
    DEBUG("Cell addr in coro, {}", fmt::ptr(this));
    if (needsImageLoad) {
        DEBUG("1");
        if (this->fileName) {
            DEBUG("Filename exists");
        } else {
            DEBUG("Filename doesn't exist");
        }
        this->fileName->set_text(imageData.value()->fileName);
        DEBUG("1");
        auto bsmlPath = imageData.value()->path;
        DEBUG("1");
        if (!bsmlPath.starts_with("file://")) {
            bsmlPath = "file://" + bsmlPath; // Ensure the path is a valid file URL
        }
        DEBUG("1");
        // if (this->preview) {
        //     DEBUG("1");
        //     BSML::Utilities::SetImage(this->preview,  bsmlPath);
        //     DEBUG("1");
        // } else {
        //     DEBUG("Preview is null. Cannot set image.");
        // }
        needsImageLoad = false; // Reset the flag after loading the image
    }
    co_return; // End the coroutine
}


ImageFactory::UI::Cells::NewImageViewCell* ImageFactory::UI::Cells::NewImageViewCell::PopulateWithImageData(std::shared_ptr<ImageFactory::Models::IFSourceImage> image) {
    DEBUG("PopulateWithImageData");
    DEBUG("Cell addr before coro, {}", fmt::ptr(this));
    auto starter = BSML::SharedCoroutineStarter::get_instance();
    starter->StartCoroutine(custom_types::Helpers::new_coro(this->LoadImageDate()));
    imageData = image; // Store the image data for later use
    needsImageLoad = true; // Indicate that the image needs to be loaded
    return this;
};
