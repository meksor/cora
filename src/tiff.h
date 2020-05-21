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
        uint_fast16_t id;
        uint_fast16_t type;
        uint_fast32_t count;
        uint_fast32_t value;
    };

    class IFD {
        public:
            uint_fast16_t numEntries;
            std::map<uint_fast16_t, struct IFDEntry> entries;
            uint_fast16_t nextIFDOffset;

            IFD(uint_fast16_t ne, std::map<uint_fast16_t, struct IFDEntry> es, uint_fast16_t ni);

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
            struct IFD parseIFD(int_fast32_t offset);
            std::map<uint_fast16_t, struct IFDEntry> parseIFDEntries(uint_fast16_t numEntries);
            struct IFDEntry parseIFDEntry();
    };
}
