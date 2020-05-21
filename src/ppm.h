#pragma once

#include <sstream>
#include <cstring>
#include <vector>
#include <memory>
#include "io.h"

namespace ppm {
    class Image {
        public:
            typedef std::shared_ptr<Image> shared_ptr;
            io::AbstractIo::shared_ptr io;

            Image(unsigned char * bitmap, size_t width, size_t height);
    };
}