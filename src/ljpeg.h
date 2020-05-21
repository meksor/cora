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
        unsigned char * counts; // number of codes per length
        ushort * lut; // lookup table
    };

    struct jinfo {
        size_t width; // image width
        size_t height; // image height
        size_t stride; // image stride
        size_t prec; // sample precision 
        ushort ncomp; // number of color components
        int pred[4]; // predictor value (differential encoding)
        htable dcts[4]; // huffman tables
        component comps[4]; // color components
    };
    
    class Image {
        public:
            typedef std::shared_ptr<Image> shared_ptr;
            io::AbstractIo::shared_ptr io;

            Image(io::AbstractIo::shared_ptr io);
            ~Image() = default;

            void save(const std::string& f) const;
            ppm::Image::shared_ptr decompress();
    };
}
