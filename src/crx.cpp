#include "crx.h"
#include "jpeg.h"
#include "tiff.h"
#include <fstream>

namespace crx {
    Image::Image(io::AbstractIo::shared_ptr io):
        tiff::Image(io),
        io(io) {
        
        io->seek(8);

        if(!this->checkCRMarker())
            throw std::invalid_argument("File does not contain canon raw data.");
        
        version = this->parseVersion();
    }

    bool Image::checkCRMarker() {
        uint_fast16_t marker = io->read16();        
        return marker == CRX_MARKER;
    };

    uint_fast16_t Image::parseVersion() {
        uint_fast16_t version = io->read16();        
        return version;
    }
    
    io::AbstractIo::shared_ptr Image::extractJPG(tiff::IFD ifd, uint_fast16_t offsEntry, uint_fast16_t lenEntry) {
        tiff::IFDEntry stripOffs = ifd.entries[offsEntry];
        tiff::IFDEntry stripLen = ifd.entries[lenEntry];

        char * buffer = new char[stripLen.value];
        io->seek(stripOffs.value);
        io->read(buffer, stripLen.value);
        
        auto io = std::make_shared<io::MemIo>(buffer, stripLen.value);
        return io;
    }
}
