#pragma once

#include "Presenter.hpp"

namespace ImageFactory::Presenters {
    class LastNotePresenter : public Presenter {
        public:
            std::vector<UnityEngine::GameObject*> GetUIElements(UnityEngine::Transform* parent, IFImage* image) override;
    };
}