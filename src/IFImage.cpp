#include "IFImage.hpp"

#include "main.hpp"
#include "PluginConfig.hpp"
#include "Utils/FileUtils.hpp"
#include "Utils/UIUtils.hpp"
#include "UnityEngine/Vector3.hpp"
#include "UnityEngine/Rect.hpp"
#include "UnityEngine/Bounds.hpp"
#include "Tweening/FloatTween.hpp"
#include "custom-types/shared/delegate.hpp"
#include "bsml/shared/Helpers/utilities.hpp"
#include "System/Action.hpp"
#include "System/Action_1.hpp"
#include "GlobalNamespace/EaseType.hpp"
#include "bsml/shared/BSML/SharedCoroutineStarter.hpp"

#include "EasyGifReader.h"

using namespace UnityEngine;

DEFINE_TYPE(ImageFactory, IFImage);

namespace ImageFactory {

    void IFImage::Create() {
        // Can't create twice.
        CRASH_UNLESS(!hasBeenCreated);

        screen = BSML::Lite::CreateFloatingScreen(
                {
                    scaleX * (width / 3),
                    scaleY * (height / 3)
                 },
                 {x, y, z},
                 {angleX, angleY, angleZ},
                 0.0f,
                 false,
                 true
                 );
        screenGO = screen->get_gameObject();
        image = BSML::Lite::CreateImage(screen->get_transform(), sprite, {x, y}, {scaleX * (width / 3), scaleY * (height / 3)});
        Object::DontDestroyOnLoad(screen);
        Object::DontDestroyOnLoad(image);

        screenGO->set_active(false);
        hasBeenCreated = true;
    }

    custom_types::Helpers::Coroutine AnimateIn(IFImage* self) {
        auto size = self->get_position();
        auto screen = self->screen;
        auto image = self->image;
        float y = 0.0f;
        
        while (y < size.y) {
            co_yield nullptr;
            self->set_size({size.x, y});
            y += 1.0f;
        }

        self->canAnimate = true;
        
        co_return;
    }

    custom_types::Helpers::Coroutine AnimateOut(IFImage* self) {
        auto size = self->get_position();
        auto screen = self->screen;
        auto image = self->image;
        float y = size.y;
        
        while (y > 0.0f) {
            co_yield nullptr;
            self->set_size({size.x, y});
            y -= 1.0f;
        }

        screen->get_gameObject()->set_active(false);
        self->canAnimate = false;

        co_return;
    }

    void IFImage::Spawn(bool anim) {
        if (!enabled) return; 
        if (!screen) return;
        if (!getPluginConfig().Enabled.GetValue()) return;

        screenGO->set_active(true);
    }

    void IFImage::Despawn(bool anim) {
        CRASH_UNLESS(screen);

        screenGO->set_active(false);
    }

    void IFImage::Update(bool handle) {
        if (!enabled) {
            Despawn(false);
            return;
        }

        Vector3 oldPos = {x, y, z};
        Vector3 oldRot = {angleX, angleY, angleZ};

        if (screen) {
            oldPos = screen->get_transform()->get_localPosition();
            oldRot = screen->get_transform()->get_eulerAngles();
        }

        screen->set_ScreenSize({scaleX * (width / 3), scaleY * (height / 3)});
        screen->set_ShowHandle(handle);
        screen->set_HandleSide(BSML::Side::Full);
//        floating->get_handle()->set_active(handle);

        screen->get_transform()->set_position(oldPos);
        screen->get_transform()->set_eulerAngles(oldRot);

        auto rectTransform = image->get_transform().cast<RectTransform>();
        rectTransform->set_sizeDelta({scaleX * (width / 3), scaleY * (height / 3)});
        rectTransform->set_anchoredPosition({x, y});

        Object::DontDestroyOnLoad(screen);
        Object::DontDestroyOnLoad(image);

        screenGO->SetActive(enabled);
        image->get_gameObject()->SetActive(enabled);
    }

    void IFImage::Destroy() {
        if (!hasBeenCreated) return;
        
        hasBeenCreated = false;

        if (screen && image) {
            Object::Destroy(screen);
            Object::Destroy(image);
        }
    }

    custom_types::Helpers::Coroutine IFImage::SetImage(std::function<void()> onFinished) {
        if (FileUtils::isGifFile(path)) {
            bool finished = false;

            BSML::Utilities::SetImage(image, "file://" + path, false, BSML::Utilities::ScaleOptions(), [&](){
                finished = true;
            });

            while (!finished) {
                co_yield nullptr;
            }
        } else {
            image->set_sprite(sprite);
        }

        if (onFinished) {
            onFinished();
        }
        co_return;
    }

    void IFImage::SetImage() {
        if (FileUtils::isGifFile(path)) {
            BSML::Utilities::SetImage(image, "file://" + path);
        } else {
            image->set_sprite(sprite);
        }
    }

    Vector2 IFImage::get_size() {
        auto scale = screen->GetComponent<RectTransform*>()->get_sizeDelta();
        return {scale.x, scale.y};
    }

    void IFImage::set_size(Vector2 value) {
        if (value.x == 0)
            value = {0.01f, value.y};
        if (value.y == 0)
            value = {value.x, 0.01f};

        auto position = get_position();
        screen->GetComponent<RectTransform*>()->set_sizeDelta({value.x, value.y});
        image->get_rectTransform()->set_sizeDelta({value.x, value.y});

        if (isAnimated) {
            set_position(position);
        }
    }
    
    Vector3 IFImage::get_position() {
        auto transform = screen->get_transform();

        if (isAnimated) {
            auto pos = Vector3::op_Addition(
                transform->get_position(),
                Vector3(spriteRenderer->get_bounds().get_extents().x, spriteRenderer->get_bounds().get_extents().y, 0.0f)
            );
            return pos;
        } else {    
            return transform->get_position();
        }
    }

    void IFImage::set_position(Vector3 value) {
        auto transform = screen->get_transform();

        if (isAnimated) {
            auto pos = Vector3::op_Subtraction(
                value,
                Vector3(spriteRenderer->get_bounds().get_extents().x, spriteRenderer->get_bounds().get_extents().y, 0.0f)
            );
            transform->set_position(pos);
        } else {
            transform->set_position(value);
        }
    }

    void IFImage::set_rotation(Vector3 value) {
        screen->get_transform()->set_eulerAngles(value);
        image->get_transform()->set_eulerAngles(value);
    }

    Vector3 IFImage::get_rotation() {
        return screen->get_transform()->get_eulerAngles();
    }

    void IFImage::SetExtraData(StringW key, StringW val) {
        if (extraData->contains(key)) {
            extraData->erase(key);
        }
        extraData->insert({key, val});
    }

    std::string IFImage::GetExtraData(std::string key, std::string defaultVal) {
        if (extraData->contains(key)) {
            return extraData->at(key);
        } else {
            return defaultVal;
        }
    }

    void IFImage::ctor(UnityW<UnityEngine::Sprite> s, StringW p) {
        sprite = s;
        width = sprite->get_textureRect().get_width();
        height = sprite->get_textureRect().get_height();
        x = 0.0f;
        y = 3.0f;
        z = 5.0f;
        name = FileUtils::GetFileName(p, false);
        angleX = 0;
        angleY = 0;
        angleZ = 0;
        enabled = true;
        scaleX = 1.0f;
        scaleY = 1.0f;
        presentationoption = "Everywhere";
        inSong = false;
        fileName = FileUtils::GetFileName(p, false);
        path = static_cast<std::string>(p);
        extraData = new std::unordered_map<std::string, std::string>();
        canAnimate = false;
        isAnimated = FileUtils::isGifFile(path);

        if (FileUtils::isGifFile(path)) {
            auto gifReader = EasyGifReader::openFile(path.c_str());
            
            width = gifReader.width();
            height = gifReader.height();  

            sprite = UIUtils::FirstFrame(path);
        }
        
        Create();
        Update(true);
        SetImage();
    }
}

