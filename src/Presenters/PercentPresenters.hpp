#pragma once

#include "Presenter.hpp"

namespace ImageFactory::Presenters {
    class PercentPresenter : public Presenter {
        public:
            std::vector<UnityEngine::GameObject*> GetUIElements(UnityEngine::Transform* parent, IFImage* image) override;
    };

    class PercentRangePresenter : public Presenter {
        public:
            std::vector<UnityEngine::GameObject*> GetUIElements(UnityEngine::Transform* parent, IFImage* image) override;
    };
}