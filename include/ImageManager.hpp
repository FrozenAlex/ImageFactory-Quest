#pragma once

#include "IFImage.hpp"
#include "UnityEngine/GameObject.hpp"
#include "PluginConfig.hpp"


using namespace std;
using namespace UnityEngine;

DECLARE_CLASS_CODEGEN(ImageFactory, ImageManager, UnityEngine::MonoBehaviour,
    DECLARE_INSTANCE_METHOD(void, AddNewImage, UnityW<IFImage>);
    DECLARE_INSTANCE_METHOD(void, AddImage, UnityW<IFImage>);
    DECLARE_INSTANCE_METHOD(void, RemoveImage, UnityW<IFImage>);
    DECLARE_INSTANCE_METHOD(void, RemoveAllImages);
    DECLARE_INSTANCE_METHOD(void, UpdateImage, UnityW<IFImage>);


    DECLARE_INSTANCE_FIELD(ListW<UnityW<IFImage>>, images);
    DECLARE_INSTANCE_FIELD(UnityW<GameObject>, go);

    public:
        static UnityW<ImageManager> get_instance();
        custom_types::Helpers::Coroutine LoadImages();
)