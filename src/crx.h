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
            ushort version;

            Image(io::AbstractIo::shared_ptr io);
            ~Image() = default;

            io::AbstractIo::shared_ptr extractJPG(tiff::IFD ifd, ushort oentry, ushort lentry);

        private:
            bool checkCRMarker();
            ushort parseVersion();
    };
};
