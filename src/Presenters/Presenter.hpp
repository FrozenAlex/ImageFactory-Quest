#pragma once

#include "IFImage.hpp"
#include "UnityEngine/GameObject.hpp"

namespace ImageFactory::Presenters {
    class Presenter {
        public:
            virtual std::vector<UnityEngine::GameObject*> GetUIElements(UnityEngine::Transform* parent, IFImage* image) { return {}; }

            static bool fullCombo;
            static int currentNoteCount;
    };
}