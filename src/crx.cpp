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
        ushort marker = io->read16();        
        return marker == CRX_MARKER;
    };

    ushort Image::parseVersion() {
        ushort version = io->read16();        
        return version;
    }
    
    io::AbstractIo::shared_ptr Image::extractJPG(tiff::IFD ifd, ushort oentry, ushort lentry) {
        tiff::IFDEntry offs = ifd.entries[oentry];
        tiff::IFDEntry len = ifd.entries[lentry];
        auto jio = std::make_shared<io::MemIo>(len.value);
        std::vector<uint8_t> jdata = jio->getData();

        io->seek(offs.value);
        io->read(jdata, len.value);
        
        jio->setData(jdata);
        return jio;
    }
}
