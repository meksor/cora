#pragma once

#include "tiff.h"
#include "jpeg.h"
#include "io.h"

#define CRX_MARKER 0x5243 // "CR"

#define THUMBNAIL_OFFSET 0x0201
#define THUMBNAIL_LENGTH 0x0202

#define STRIP_OFFSET 0x0111
#define STRIP_BYTECOUNT 0x0117

namespace crx {
    class Image : public tiff::Image {
        public:
            io::AbstractIo::shared_ptr io;
            int version;

            Image(io::AbstractIo::shared_ptr io);
            ~Image() = default;

            io::AbstractIo::shared_ptr extractJPG(tiff::IFD ifd, uint_fast16_t offsEntry, uint_fast16_t lenEntry);

        private:
            bool checkCRMarker();
            uint_fast16_t parseVersion();
    };
};
