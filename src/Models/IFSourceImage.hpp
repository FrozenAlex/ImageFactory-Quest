#pragma once
#include <string>

namespace ImageFactory::Models {
    struct IFSourceImage {
        std::string path;
        std::string fileName;
        std::string size;
        bool animated;
    };
}
