#pragma once
#include <memory>
#include <string>
#include <fstream>
#include <vector>
#include <map>
#include "jpeglib.h"

#include "jpegdefs.h"
#include "ppm.h"
#include "io.h"

namespace jpeg {
    class Image {
        public:
            typedef std::shared_ptr<Image> shared_ptr;
            io::AbstractIo::shared_ptr io;

            Image(io::AbstractIo::shared_ptr io);
            ~Image() = default;

            void save(const std::string& f) const;
            ppm::Image::shared_ptr decompress();

        private:
            std::shared_ptr<::jpeg_error_mgr> errorManager;
            
    };
}
