#pragma once

#include <sstream>
#include <cstring>
#include <vector>
#include <memory>
#include "io.h"

namespace netpbm {

    enum Type {
        Bitmap = '4',
        Greymap = '5',
        Pixelmap = '6',
    };
    
    template<typename T>
    class Image {
        public:
            typedef std::shared_ptr<Image<T>> shared_ptr;
            io::AbstractIo::shared_ptr io;

            Image(Type type, T * bitmap, size_t width, size_t height, size_t bitdepth) {
                size_t imageLen, headerLen, len, bytes, i, j;
                std::ostringstream header;
                header << "P" << (char)type << " " << width << " " << height << " " << ((1 << bitdepth) - 1) << "\n";
                headerLen = header.str().size();
                bytes = ceil(bitdepth / 8.);

                if (type == Type::Bitmap)
                    imageLen = (width * height);
                if (type == Type::Greymap)
                    imageLen = (width * height);
                if (type == Type::Pixelmap)
                    imageLen = (width * height * 3);

                len = (imageLen * bytes) + headerLen;
                char * buf = new char[len];

                for (i = 0; i < imageLen; i++) {
                    for (j = 0; j < bytes; j++) { 
                        buf[headerLen + ((i * bytes) + j )] = (bitmap[i] >> ((bytes-1) - j)*8);
                    }
                }

                memcpy(buf, header.str().c_str(), headerLen);

                this->io = std::shared_ptr<io::MemIo>(new io::MemIo(buf, len));
            }
    };

}
