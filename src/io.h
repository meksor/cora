#pragma once

#include <cstdint>
#include <cstring>
#include <string>
#include <fstream>
#include <memory>
#include <math.h>
#include "jpegdefs.h"

namespace io {
    enum Byteorder {
        Unknown = -1,
        LittleEndian = 'I',
        BigEndian = 'M',
    };
    
    class AbstractIo {
        public:   
            typedef std::shared_ptr<AbstractIo> shared_ptr;
            Byteorder byteorder = Byteorder::LittleEndian;

            virtual void seek(int offset) = 0;
            virtual uint_fast32_t tell() = 0;
            virtual void read(char * buf, int len) = 0;
            virtual void save(std::string& outName) = 0;
            virtual uint_fast32_t length() = 0;

            virtual void setByteorder(Byteorder bo) { byteorder = bo; };
            
            template<typename T>
            T readType();
            
            virtual uint_fast8_t read8();
            virtual uint_fast16_t read16();
            virtual uint_fast32_t read32();
            uint_fast8_t readBit();
            uint_fast16_t readBits(size_t length);
            void align();
            void debugPrint(size_t length);
            uint_fast8_t nextBit = 0;
            uint_fast8_t currentByte = 0;

        private:

    };
    
    class MemIo : public AbstractIo {
        public:
            MemIo(const char * data, uint_fast32_t len) : data(data), len(len) {};
            ~MemIo() = default;

            virtual void seek(int offset) override;
            virtual uint_fast32_t tell() override;

            virtual void read(char * buf, int len) override;
            virtual void save(std::string& outName) override;
            virtual uint_fast32_t length() override { return len; };

        private:
            uint_fast32_t offset = 0;
            const char * data;
            uint_fast32_t len;
    };

    class FileIo : public AbstractIo  {
        public:
            FileIo(std::string& fileName): fileName(fileName), fileStream(fileName, std::ios::binary) {};
            ~FileIo() = default;

            virtual void seek(int offset) override;
            virtual uint_fast32_t tell() override;
            virtual void read(char * buf, int len) override;
            virtual void save(std::string& outName) override;
            virtual uint_fast32_t length() override { 
                fileStream.seekg(0, std::ios::end);
                return fileStream.tellg();
            };

        private:
            std::ifstream fileStream;
            std::string fileName;
    };
}