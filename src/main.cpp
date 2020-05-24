
#include <cstdint>
#include <exiv2/basicio.hpp>
#include <exiv2/exiv2.hpp>
#include <iostream>
#include <iomanip>
#include <cassert>
#include <fstream>
#include <memory>
#include "crx.h"
#include "ppm.h"
#include "ljpeg.h"
#include "io.h"

int main(int argc, char * const argv[]) {
    std::string fileName(argv[1]);
    auto file = std::make_shared<io::FileIo>(fileName);
    std::unique_ptr<crx::Image> img(new crx::Image(file));

    io::AbstractIo::shared_ptr pio = img->extractJPG(
        img->ifds[0],
        STRIP_OFFSET,
        STRIP_BYTECOUNT
    );
    jpeg::Image::shared_ptr pr = std::make_shared<jpeg::Image>(pio);
    auto ppr = pr->decompress();
    std::string prn = "prv.ppm";
    ppr->io->save(prn);

    io::AbstractIo::shared_ptr rio = img->extractJPG(
        img->ifds[3],
        STRIP_OFFSET,
        STRIP_BYTECOUNT
    );
    ljpeg::Image::shared_ptr raw = std::make_shared<ljpeg::Image>(rio);

    auto rp = raw->decompress();
    std::string rpn = "raw.ppm";
    rp->io->save(rpn);

    return 0;
}
