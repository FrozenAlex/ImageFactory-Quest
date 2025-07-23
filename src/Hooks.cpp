#include "Hooks.hpp"

void ImageFactory::InstallHooks() {
    Hooks::LoadImagesHook();
    Hooks::MenuTransitionsHelper();
    Presenters::ComboHooks();
    Presenters::PercentHooks();
    Presenters::PresenterHooks();
    Presenters::LastNoteHooks();
    Presenters::ResultsHooks();
}