#pragma once

#include <cstdint>
#include <cstring>
#include <string>
#include <fstream>
#include <memory>
#include <vector>
#include <math.h>
#include "jpegdefs.h"

#define MAX_READ_BYTES 8

namespace io {
    enum Byteorder {
        Unknown = -1,
        LittleEndian = 'I',
        BigEndian = 'M',
    };
    
    class AbstractIo {
        public:   
            typedef std::shared_ptr<AbstractIo> shared_ptr;

            virtual void seek(int offset) = 0;
            virtual size_t tell() = 0;
            virtual void read(std::vector<uint8_t> &buf, int len) = 0;
            virtual void save(std::string& outName) = 0;
            virtual size_t size() = 0;

            virtual void setByteorder(Byteorder bo) { byteorder = bo; };
            
            template<typename T>
            T readType();
            
            virtual uint8_t read8();
            virtual uint16_t read16();
            virtual uint32_t read32();
            void debugPrint(size_t length);

        private:
            Byteorder byteorder = Byteorder::LittleEndian;

    };
    
    class MemIo : public AbstractIo {
        public:
            MemIo(size_t len): data(std::vector<uint8_t>(len, 0)), length(len) {};
            ~MemIo() = default;

            virtual void seek(int offset) override;
            virtual ulong tell() override;

            virtual void read(std::vector<uint8_t> &buf, int len) override;
            virtual void save(std::string& oname) override;
            virtual ulong size() override { return length; };

            std::vector<uint8_t> &getData() { return data; };
            void setData(std::vector<uint8_t> d) { data = d; };

        private:
            size_t offset = 0;
            std::vector<uint8_t> data;
            size_t length;
    };

    class FileIo : public AbstractIo  {
        public:
            FileIo(std::string& fileName): fileName(fileName), fileStream(fileName, std::ios::binary) {};
            ~FileIo() = default;

            virtual void seek(int offset) override;
            virtual size_t tell() override;
            virtual void read(std::vector<uint8_t> &buf, int len) override;
            virtual void save(std::string& oname) override;
            virtual size_t size() override { 
                fileStream.seekg(0, std::ios::end);
                return fileStream.tellg();
            };

        private:
            std::ifstream fileStream;
            std::string fileName;
    };
}