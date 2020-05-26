#pragma once
// #include <cstdint>
#include <string>
#include <fstream>
#include <stdexcept>
#include <map>
#include <vector>
#include "io.h"

#define BYTEORDER_INTEL 0x4949 // II
#define BYTEORDER_MOTOROLA 0x4d4d // MM
#define TIFF_MAGICNUM 0x002a // 42

namespace tiff {
    struct IFDEntry {
        ushort id;
        ushort type;
        ulong count;
        ulong value;
    };

    class IFD {
        public:
            ushort nEntries;
            std::map<ushort, struct IFDEntry> entries;
            ulong nextIfdOffs;

            IFD(ushort ne, std::map<ushort, struct IFDEntry> es, ulong ni);
    };

    class Image {
        public:
            io::AbstractIo::shared_ptr io;
            io::Byteorder byteorder;
            std::vector<IFD> ifds;

            Image(io::AbstractIo::shared_ptr io);
            ~Image() = default;

        private:
            io::Byteorder parseByteorder();
            bool checkMagicNumber();
            std::vector<struct IFD> parseIFDs();
            struct IFD parseIFD(ulong offset);
            std::map<ushort, struct IFDEntry> parseIFDEntries(ushort numEntries);
            struct IFDEntry parseIFDEntry();
    };
}
