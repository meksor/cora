
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
    auto img = std::make_unique<crx::Image>(file);
    
    auto pio = img->extractJPG(
        img->ifds[0],
        STRIP_OFFSET,
        STRIP_BYTECOUNT
    );
    auto pr = std::make_shared<jpeg::Image>(pio);
    auto ppr = pr->decompress();
    std::string prn = "prv.ppm";
    ppr->io->save(prn);

    auto rio = img->extractJPG(
        img->ifds[3],
        STRIP_OFFSET,
        STRIP_BYTECOUNT
    );
    auto raw = std::make_shared<ljpeg::Image>(rio);
    auto rp = raw->decompress();
    std::string rpn = "raw.ppm";
    rp->io->save(rpn);

    return 0;
}
