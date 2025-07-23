#pragma once

namespace ImageFactory {    
    namespace Hooks {
        void MenuTransitionsHelper();
        void LoadImagesHook();
    }

    namespace Presenters {
        void ComboHooks();
        void PercentHooks();
        void LastNoteHooks();
        void PresenterHooks();
        void ResultsHooks();
    }

    void InstallHooks();
}