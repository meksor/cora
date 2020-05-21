#include "tiff.h"
#include <cstdint>
#include <istream>

namespace tiff {
    IFD::IFD(uint_fast16_t ne, std::map<uint_fast16_t, struct IFDEntry> es, uint_fast16_t ni): 
        numEntries(ne),
        entries(es),
        nextIFDOffset(ni) {
    }

    Image::Image(io::AbstractIo::shared_ptr io):
        io(io) {

        io->seek(0);
        io::Byteorder bo = this->parseByteorder();
        if(bo == io::Byteorder::Unknown)
            throw std::invalid_argument("Unknown byteorder.");
        io->setByteorder(bo);
        
        if(!this->checkMagicNumber())
            throw std::invalid_argument("File is missing magic number. Check byte order and file integrity.");

        ifds = this->parseIFDs();
    };

    io::Byteorder Image::parseByteorder() {
        uint_fast16_t byteorder = io->read16();;

        if (byteorder == BYTEORDER_INTEL) {
            return io::Byteorder::LittleEndian;
        }
        else if (byteorder == BYTEORDER_MOTOROLA) {
            return io::Byteorder::BigEndian;
        }
        else {
            return io::Byteorder::Unknown;
        }
    };
    
    bool Image::checkMagicNumber() {
        uint_fast16_t magicn = io->read16();        
        return magicn == TIFF_MAGICNUM;
    };
    
    std::vector<IFD> Image::parseIFDs() {
        io->seek(4);
        uint_fast32_t offset = io->read32();
        std::vector<IFD> ifds;
        do {
            IFD ifd = parseIFD(offset);
            ifds.push_back(ifd);
            offset = ifd.nextIFDOffset;
        }
        while (offset != 0);
        return ifds;
    };

    IFD Image::parseIFD(int_fast32_t offset) {
        io->seek(offset);

        uint_fast16_t numEntries = io->read16();

        std::map<uint_fast16_t, struct IFDEntry> entries = this->parseIFDEntries(numEntries);
        uint_fast32_t nextIFDOffset = io->read32();

        return IFD(numEntries, entries, nextIFDOffset);
    };

    std::map<uint_fast16_t, struct IFDEntry> Image::parseIFDEntries(uint_fast16_t numEntries) {
        std::map<uint_fast16_t, struct IFDEntry> entries({});

        for (int i=0; i<numEntries; i++) {
            IFDEntry entry = this->parseIFDEntry();
            entries[entry.id] = entry;
        }
        return entries;
    };

    struct IFDEntry Image::parseIFDEntry() {
        return IFDEntry {
            .id=io->read16(),
            .type=io->read16(),
            .count=io->read32(),
            .value=io->read32()
        };
    };
}
