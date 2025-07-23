#pragma once

#include "beatsaber-hook/shared/utils/typedefs-string.hpp"
#include "IFImage.hpp"
#include "Utils/UIUtils.hpp"
#include "UnityEngine/GameObject.hpp"
#include "bsml/shared/BSML/SharedCoroutineStarter.hpp"
#include "custom-types/shared/delegate.hpp"

using namespace UnityEngine;
using namespace GlobalNamespace;


namespace ImageFactory::Presenters {
    class Presenter {
        public:
            virtual std::vector<GameObject*> GetUIElements(Transform* parent, IFImage* image) { return {}; }

            static bool fullCombo;
            static int currentNoteCount;
    };
}