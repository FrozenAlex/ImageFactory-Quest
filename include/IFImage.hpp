#pragma once

#include "HMUI/ImageView.hpp"
#include "HMUI/Touchable.hpp"
#include "System/TimeSpan.hpp"
#include "UnityEngine/Collider.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/MeshRenderer.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/Quaternion.hpp"
#include "UnityEngine/Sprite.hpp"
#include "UnityEngine/SpriteRenderer.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/UI/Image.hpp"
#include "UnityEngine/Vector2.hpp"
#include "UnityEngine/Vector3.hpp"
#include "UnityEngine/SpriteRenderer.hpp"
#include "VRUIControls/VRPointer.hpp"
#include "custom-types/shared/coroutine.hpp"
#include "custom-types/shared/macros.hpp"
#include "custom-types/shared/types.hpp"
#include "bsml/shared/BSML/FloatingScreen/FloatingScreen.hpp"
#include "bsml/shared/BSML.hpp"
#include "Tweening/TimeTweeningManager.hpp"

using namespace std;
using namespace UnityEngine;

DECLARE_CLASS_CODEGEN(ImageFactory, IFImage, UnityEngine::MonoBehaviour,
    DECLARE_INSTANCE_METHOD(void, Create);
    DECLARE_INSTANCE_METHOD(void, Spawn, bool);
    DECLARE_INSTANCE_METHOD(void, Despawn, bool);
    DECLARE_INSTANCE_METHOD(void, Update, bool); 
    DECLARE_INSTANCE_METHOD(void, Destroy); 
    DECLARE_INSTANCE_METHOD(Vector2, get_size);
    DECLARE_INSTANCE_METHOD(void, set_size, UnityEngine::Vector2); 
    DECLARE_INSTANCE_METHOD(Vector3, get_position);
    DECLARE_INSTANCE_METHOD(void, set_position, UnityEngine::Vector3);
    DECLARE_INSTANCE_METHOD(Quaternion, get_rotation);
    DECLARE_INSTANCE_METHOD(void, set_rotation, UnityEngine::Quaternion);
    DECLARE_INSTANCE_FIELD(float, width);
    DECLARE_INSTANCE_FIELD(float, height);
    DECLARE_INSTANCE_FIELD(Vector3, position);
    DECLARE_INSTANCE_FIELD(Quaternion, rotation);
    DECLARE_INSTANCE_FIELD(Vector2, scale);
    DECLARE_INSTANCE_FIELD(bool, hasBeenCreated);
    DECLARE_INSTANCE_FIELD(bool, inSong);
    DECLARE_INSTANCE_FIELD(bool, canAnimate);
    DECLARE_INSTANCE_FIELD(bool, isAnimated);
    DECLARE_INSTANCE_FIELD(UnityW<SpriteRenderer>, spriteRenderer);
    DECLARE_INSTANCE_FIELD(UnityW<HMUI::ImageView>, image);
    DECLARE_INSTANCE_FIELD(UnityW<HMUI::ImageView>, inSongImage);
    DECLARE_INSTANCE_FIELD(UnityW<Sprite>, sprite);
    DECLARE_INSTANCE_FIELD(UnityW<BSML::FloatingScreen>, screen);
    DECLARE_INSTANCE_FIELD(UnityW<GameObject>, screenGO);
    DECLARE_INSTANCE_FIELD(UnityW<GameObject>, inSongScreen);
    DECLARE_CTOR(ctor, UnityW<Sprite> sprite, StringW path);
    
    public:
        std::string name;
        std::unordered_map<std::string, std::string>* extraData;
        StringW internalName; 
        StringW fileName; 
        std::string path;
        std::string presentationoption; 
        bool enabled;
        custom_types::Helpers::Coroutine SetImage(std::function<void()> onFinished);
        void SetImage();
        void SetExtraData(StringW key, StringW val);
        std::string GetExtraData(std::string key, std::string defaultVal);
    )
