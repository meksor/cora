#include "jpeg.h"
#include "ppm.h"
#include <cstring>
#include <stdexcept>

namespace jpeg {

    Image::Image(io::AbstractIo::shared_ptr io) : io(io) {
        this->errorManager = std::make_shared<::jpeg_error_mgr>();
    };

    netpbm::Image<uint8_t>::shared_ptr Image::decompress() {
        auto dt = [](::jpeg_decompress_struct *ds) {
            ::jpeg_destroy_decompress(ds);
        };
        
        std::unique_ptr<::jpeg_decompress_struct, decltype(dt)> decompressInfo(
            new ::jpeg_decompress_struct,
            dt
        );

        decompressInfo->err = ::jpeg_std_error(this->errorManager.get());
        this->errorManager->error_exit = [](::j_common_ptr cinfo) {
            char jpegLastErrorMsg[JMSG_LENGTH_MAX];
            ( *( cinfo->err->format_message ) )
                (cinfo, jpegLastErrorMsg);
            throw std::runtime_error(jpegLastErrorMsg);
        };

        ::jpeg_create_decompress(decompressInfo.get());

        uint_fast32_t len = io->length();
        char * data = new char[len];
        io->read(data, len);

        ::jpeg_mem_src(
            decompressInfo.get(), 
            reinterpret_cast<const unsigned char *>(data), 
            len
        );

        int rc = ::jpeg_read_header(decompressInfo.get(), TRUE);
        if (rc != 1)
        {
            throw std::runtime_error(
                "File does not seem to be a normal JPEG"
                );
        }

        ::jpeg_start_decompress(decompressInfo.get());

        size_t row_stride = decompressInfo->output_width * decompressInfo->output_components;
        size_t length = (decompressInfo->output_height * row_stride);

        uint8_t * buf = new uint8_t[length];
        uint8_t * p = buf;

        while ( decompressInfo->output_scanline < decompressInfo->output_height ) {
            ::jpeg_read_scanlines(decompressInfo.get(), &p, 1);
            p += row_stride;
        }
        ::jpeg_finish_decompress(decompressInfo.get());
        delete[] data;

        auto ppm = std::make_shared<netpbm::Image<uint8_t>>(
            netpbm::Type::Pixelmap, buf, decompressInfo->output_width, decompressInfo->output_height, 8
        );
        return ppm;
    };
}
