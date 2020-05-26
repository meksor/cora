#include "tiff.h"
#include <cstdint>
#include <istream>

namespace tiff {
    IFD::IFD(ushort ne, std::map<ushort, struct IFDEntry> es, ulong ni): 
        nEntries(ne),
        entries(es),
        nextIfdOffs(ni) {
    }

    Image::Image(io::AbstractIo::shared_ptr io):
        io(io) {
        io::Byteorder bo;

        io->seek(0);
        bo = this->parseByteorder();

        if(bo == io::Byteorder::Unknown)
            throw std::invalid_argument("Unknown byteorder.");

        io->setByteorder(bo);
        
        if(!this->checkMagicNumber())
            throw std::invalid_argument("File is missing magic number. Check byte order and file integrity.");

        ifds = this->parseIFDs();
    };

    io::Byteorder Image::parseByteorder() {
        ushort byteorder = io->read16();

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
        ushort magicn = io->read16();        
        return magicn == TIFF_MAGICNUM;
    };
    
    std::vector<IFD> Image::parseIFDs() {
        io->seek(4);
        ulong offset = io->read32();
        std::vector<IFD> ifds;
        do {
            IFD ifd = this->parseIFD(offset);
            ifds.push_back(ifd);
            offset = ifd.nextIfdOffs;
        }
        while (offset != 0);
        return ifds;
    };

    IFD Image::parseIFD(ulong offset) {
        ushort nentries;
        ulong nifdoffs;
        std::map<ushort, struct IFDEntry> entries;

        io->seek(offset);

        nentries = io->read16();
        entries = this->parseIFDEntries(nentries);
        nifdoffs = io->read32();

        return IFD(nentries, entries, nifdoffs);
    };

    std::map<ushort, struct IFDEntry> Image::parseIFDEntries(ushort numEntries) {
        std::map<ushort, struct IFDEntry> entries;

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
