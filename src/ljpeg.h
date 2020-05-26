#pragma once
#include <memory>
#include <string>
#include <fstream>
#include <vector>
#include <map>

#include "jpegdefs.h"
#include "ppm.h"
#include "io.h"

namespace ljpeg {
    
    struct component {
        ushort id; // component id
        int hsf; // horizontal sample factor
        int vsf; // vertical sample factor
        ushort qt; // quantization table idx
        int act; // ac table idx
        int dct; // dc table idx
    };

    struct htable {
        ushort tc;  // table class (ac/dc)
        ushort id;  // table id
        ushort max; // max code length
        std::vector<uint8_t> counts; // number of codes per length
        std::vector<ushort> lut; // lookup table
    };

    struct jinfo {
        size_t width; // image width
        size_t height; // image height
        size_t stride; // image stride
        uint8_t prec; // sample precision 
        uint8_t ncomp; // number of color components

        size_t spss; // spectral / predictor selection start 
        size_t spse; // spectral / predictor selection end 
        size_t sabp; // successive approximation bit positions

        std::vector<int> pred; // predictor value (differential encoding)
        std::vector<htable> dcts; // huffman tables
        std::vector<component> comps; // color components
    };
    
    class Image {
        public:
            typedef std::shared_ptr<Image> shared_ptr;
            io::AbstractIo::shared_ptr io;

            Image(io::AbstractIo::shared_ptr io);
            ~Image() = default;

            netpbm::Image<ushort>::shared_ptr decompress();
    };
}
