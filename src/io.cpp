
#include "io.h"

namespace io {
    template<typename T>
    T AbstractIo::readType() {
        T o = 0;
        char ib[sizeof(T)];
        this->read(ib, sizeof(T));

        for (auto i = 0; i < sizeof(T); ++i)
        {
            o = o << 8;
            if (byteorder != io::Byteorder::LittleEndian)
                o |= ib[i] & 0xff;
            else
                o |= ib[((sizeof(T) - 1) - i)] & 0xff;
        }
        this->currentByte = ib[sizeof(T) - 1];
        this->nextBit = 8;
        return o;
    };
    
    uint_fast8_t AbstractIo::read8() {
        return this->readType<uint8_t>();
    };
    uint_fast16_t AbstractIo::read16() {
        return this->readType<uint16_t>();
    };
    uint_fast32_t AbstractIo::read32() {
        return this->readType<uint32_t>();
    };

    uint_fast8_t AbstractIo::readBit() {
        if (nextBit == 8) {
            currentByte = read8();
            if (currentByte == 0xFF) {
                uint16_t mark = (0xFF << 8) | read8();
                if (mark != JPEG_FF) { 
                    throw std::runtime_error("Unexpected JPEG marker.");
                }
            }
            nextBit = 0;
        }
        uint_fast8_t s = (7 - (nextBit));
        uint_fast8_t bit = (currentByte >> s) & 1;
        nextBit++;
        return bit;
    }

    uint_fast32_t AbstractIo::readBits(size_t length) {
        size_t i;
        uint_fast32_t ret = 0;

        for (i = 0; i<length; i++) {
            uint_fast16_t bit = readBit();
            ret = (ret << 1) | bit;
        }
        return ret;
    }

    void AbstractIo::align() {
        if (nextBit != 8) {
            currentByte = read8();
        }
    }
    
    void AbstractIo::debugPrint(size_t length) {
        uint_fast32_t start = tell();

        for (size_t i = 0; i<length; i++) {
            uint_fast8_t byte = read8();
            if (i % 4 == 0)
                printf(" ");
            if (i % 16 == 0)
                printf("\n");
            if (i % 16 == 0) {
                printf("%04hhX : ", tell());
            }
            printf("%02hhX ", byte);
           
        }
        printf("\n ");

        seek(start);
    }

    void MemIo::seek(int offset) {
        this->offset = offset;
    };
    uint_fast32_t MemIo::tell() {
        return offset;
    };
    void MemIo::read(char * buf, int len) {
        std::memcpy(buf, this->data + this->offset, len);
        this->offset += len;
    };
    void MemIo::save(std::string& outName) {
        std::ofstream outStream;
        outStream.open(outName, std::ios::out | std::ios::trunc | std::ios::binary);
        outStream.write(data, len);
        outStream.close();
    }
  
    void FileIo::seek(int offset) {
        this->fileStream.seekg(offset);
    };
    uint_fast32_t FileIo::tell() {
        return this->fileStream.tellg();
    };
    void FileIo::read(char * buf, int len) {
        this->fileStream.read(buf, len);
    };
    void FileIo::save(std::string& outName) {
        std::ofstream outStream;
        outStream.open(outName, std::ios::out | std::ios::trunc | std::ios::binary);
        outStream << this->fileStream.rdbuf();
        outStream.close();
    };
}