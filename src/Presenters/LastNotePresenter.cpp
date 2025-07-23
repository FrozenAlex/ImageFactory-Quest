#include "Utils/StringUtils.hpp"
#include "Presenters/PresenterManager.hpp"
#include "Presenters/LastNotePresenter.hpp"
#include "GlobalNamespace/StandardLevelDetailView.hpp"
#include "GlobalNamespace/BeatmapData.hpp"
#include "GlobalNamespace/BeatmapBasicData.hpp"
#include "GlobalNamespace/BeatmapLevel.hpp"
#include "GlobalNamespace/NoteData.hpp"
#include "System/Threading/Tasks/Task_1.hpp"
#include "GlobalNamespace/NoteController.hpp"
#include "logging.hpp"
#include "bsml/shared/BSML/SharedCoroutineStarter.hpp"
#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "Hooks.hpp"

#define StartCoroutine(method) BSML::SharedCoroutineStarter::get_instance()->StartCoroutine(custom_types::Helpers::CoroutineHelper::New(method))

namespace ImageFactory::Presenters {

    std::vector<GameObject*> LastNotePresenter::GetUIElements(Transform* parent, IFImage* image) {
        std::vector<GameObject*> ret;

        auto duration = BSML::Lite::CreateIncrementSetting(parent, "Duration (Seconds)", 2, 0.25, std::stof(image->GetExtraData("last_note_duration", "1")), 0.25, 100, 
            [=](float f){
                image->SetExtraData("last_note_duration", std::to_string(f));
            })->get_gameObject();

        ret.push_back(duration);

        return ret;
    }

    int noteCount = 0;

    MAKE_HOOK_MATCH(StandardLevelDetailView_RefreshContent, &StandardLevelDetailView::RefreshContent, void, StandardLevelDetailView* self) {
        StandardLevelDetailView_RefreshContent(self);


        if (self->_beatmapLevel && self->beatmapKey) {
            auto key = self->beatmapKey;
            auto level = self->_beatmapLevel;

            auto diff = level->GetDifficultyBeatmapData(key.beatmapCharacteristic, key.difficulty);
            if (diff) {
                noteCount = diff->notesCount;
            } else {
                noteCount = 0;
                WARNING("Could not get the note count for the current beatmap");
            }
        }
    }

    MAKE_HOOK_MATCH(NoteController_Init, &NoteController::Init, void, NoteController* self, 
    ::GlobalNamespace::NoteData* noteData, ::ByRef<::GlobalNamespace::NoteSpawnData> noteSpawnData, float_t endRotation, float_t uniformScale, bool rotateTowardsPlayer, bool useRandomRotation) {
        Presenter::currentNoteCount++;

        if (Presenter::currentNoteCount == noteCount) {
            for (std::pair<IFImage*, StringW> pair : *PresenterManager::MAP) {
                IFImage* image = pair.first;
                if (pair.second.starts_with(PresenterManager::LAST_NOTE)) {
                    
                    if (!UIUtils::NoHud()) return;

                    StartCoroutine(PresenterManager::DespawnAfter(image, stof(image->GetExtraData("last_note_duration", "1"))));
                }
            }
        }
        NoteController_Init(self, noteData, noteSpawnData, endRotation, uniformScale,
                            rotateTowardsPlayer, useRandomRotation);
    }

    void LastNoteHooks() {
        INSTALL_HOOK(Logger, StandardLevelDetailView_RefreshContent);
        INSTALL_HOOK(Logger, NoteController_Init);
    }
}