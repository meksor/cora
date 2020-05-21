#include "ppm.h"

namespace ppm {
    Image::Image(unsigned char * bitmap, size_t width, size_t height) {
        std::ostringstream header;
        header << "P6 " << width << " " << height << " 255\n";
        size_t headerLen = header.tellp();
        size_t imageLen = (width * height * 3);

        size_t len = imageLen + headerLen;
        char * buf = new char[len];

        memcpy(buf, header.str().c_str(), headerLen);
        memcpy(buf + headerLen, bitmap, imageLen);

        this->io = std::shared_ptr<io::MemIo>(new io::MemIo(buf, len));
    };
}