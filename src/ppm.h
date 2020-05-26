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

            Image(Type type, std::vector<T> bitmap, size_t width, size_t height, uint8_t bitdepth) {
                size_t imglen, hlen, len, bytes, i, j;
                std::ostringstream header;

                header << "P" << static_cast<char>(type) << " " << width << " " \
                    << height << " " << ((1 << bitdepth) - 1) << "\n";
                hlen = header.str().size();
                
                bytes = ceil(bitdepth / 8.);

                if (type == Type::Bitmap)
                    imglen = (width * height);
                if (type == Type::Greymap)
                    imglen = (width * height);
                if (type == Type::Pixelmap)
                    imglen = (width * height * 3);

                len = (imglen * bytes) + hlen;

                auto memio = std::make_shared<io::MemIo>(len);
                this->io = memio;
                std::vector<uint8_t> &buf = memio->getData();

                for (i = 0; i < imglen; i++) {
                    for (j = 0; j < bytes; j++) { 
                        buf[hlen + ((i * bytes) + j )] = (bitmap[i] >> ((bytes-1) - j)*8);
                    }
                }

                memcpy(buf.data(), header.str().c_str(), hlen);
            }
    };
}
