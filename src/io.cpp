
#include "io.h"

namespace io {
    template<typename T>
    T AbstractIo::readType() {
        static std::vector<uint8_t> readbuf(MAX_READ_BYTES, 0);

        T o = 0;
        this->read(readbuf, sizeof(T));
        if (sizeof(T) == 1) return readbuf[0];

        for (auto i = 0; i < sizeof(T); ++i) {
            o = o << 8;
            if (byteorder != io::Byteorder::LittleEndian)
                o |= readbuf[i] & 0xff;
            else
                o |= readbuf[((sizeof(T) - 1) - i)] & 0xff;
        }
        return o;
    };
    uint8_t AbstractIo::read8() {
        return readType<uint8_t>();
    };
    uint16_t AbstractIo::read16() {
        return readType<uint16_t>();
    };
    uint32_t AbstractIo::read32() {
        return readType<uint32_t>();
    };
    void AbstractIo::debugPrint(size_t length) {
        size_t start = this->tell();

        for (size_t i = 0; i<length; i++) {
            uint8_t byte = read8();
            if (i % 4 == 0)
                printf(" ");
            if (i % 16 == 0)
                printf("\n");
            if (i % 16 == 0) {
                printf("%04zX : ", this->tell());
            }
            printf("%02hhX ", byte);
           
        }
        printf("\n ");
        this->seek(start);
    }

    void MemIo::seek(int offset) {
        this->offset = offset;
    };
    size_t MemIo::tell() {
        return offset;
    };
    void MemIo::read(std::vector<uint8_t> &buf, int len) {
        std::memcpy(buf.data(), data.data() + offset, len);
        offset += len;
    };
    void MemIo::save(std::string& oname) {
        std::ofstream outStream;
        outStream.open(oname, std::ios::out | std::ios::trunc | std::ios::binary);
        outStream.write(reinterpret_cast<char *>(data.data()), length);
        outStream.close();
    }

    void FileIo::seek(int offset) {
        fileStream.seekg(offset);
    };
    size_t FileIo::tell() {
        return fileStream.tellg();
    };
    void FileIo::read(std::vector<uint8_t> &buf, int len) {
        fileStream.read(reinterpret_cast<char *>(buf.data()), len);
    };
    void FileIo::save(std::string& oname) {
        std::ofstream outStream;
        outStream.open(oname, std::ios::out | std::ios::trunc | std::ios::binary);
        outStream << this->fileStream.rdbuf();
        outStream.close();
    };
}