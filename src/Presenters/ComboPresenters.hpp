#pragma once

#include "Presenter.hpp"

namespace ImageFactory::Presenters {
    class ComboPresenter : public Presenter {
        public:
            std::vector<UnityEngine::GameObject*> GetUIElements(UnityEngine::Transform* parent, IFImage* image) override;
    };

    class ComboIncrementPresenter : public Presenter {
        public:
            std::vector<UnityEngine::GameObject*> GetUIElements(UnityEngine::Transform* parent, IFImage* image) override;
    };

    class ComboHoldPresenter : public Presenter {
        public:
            std::vector<UnityEngine::GameObject*> GetUIElements(UnityEngine::Transform* parent, IFImage* image) override;
    };

    class ComboDropPresenter : public Presenter {
        public:
            std::vector<UnityEngine::GameObject*> GetUIElements(UnityEngine::Transform* parent, IFImage* image) override;
    };
}