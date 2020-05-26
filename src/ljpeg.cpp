#include "ljpeg.h"
#include <climits>
#include <cstring>
#include <cassert>
#include <stdexcept>
#include <sys/types.h>

namespace ljpeg {
    io::AbstractIo::shared_ptr ljsrc;
    
    void ljpeg_set_source(io::AbstractIo::shared_ptr io) {
        ljsrc = io;
    }

    void ljpeg_read_dht(struct jinfo &ji) {
        uint end, len, i, j, h, t;
        ushort max;
        unsigned char htinfo, htid, httc, symbol;
        std::vector<uint8_t> counts(16, 0);
        
        if (ljsrc->read16() != JPEG_DHT)
            throw std::runtime_error("DHT marker expected.");

        end = ljsrc->tell() + ljsrc->read16();

        t = 0;
        do {
            htinfo = ljsrc->read8();
            htid = htinfo & 0x0F;
            httc = (htinfo >> 4) & 0x0F;

            for (i=0; i<16; i++) counts[i] = ljsrc->read8();
            for (max=15; max && !counts[max]; max--);
            max += 1;
            
            std::vector<ushort> lut(1 << max, 0);
            for (h=0, len=1; len <= max; len++) {
                for (i=0; i < counts[len-1]; i++) {
                    symbol = ljsrc->read8();
                    for (j=0; j < (1 << (max-len)); j++) {
                        if (h < (1 << max))
                            lut[h++] = len << 8 | symbol;
                    }
                }
            }
            ji.dcts.push_back(htable{
                .tc = httc,
                .id = htid,
                .max = max,
                .counts = counts,
                .lut = lut,
            });
        } while(ljsrc->tell() < end);
    }

    void ljpeg_read_sof(struct jinfo &ji) {
        uint end, i;
        ushort id, sf;

        if (ljsrc->read16() != JPEG_SOF3)
            throw std::runtime_error("SOF marker expected.");
        
        end = ljsrc->tell() + ljsrc->read16();
        ji.prec = ljsrc->read8();
        ji.height = ljsrc->read16();
        ji.width = ljsrc->read16();
        ji.ncomp = ljsrc->read8();
        ji.stride = ji.width*ji.ncomp;

        for (i = 0; i<ji.ncomp; i++) {
            id = ljsrc->read8();
            sf = ljsrc->read8();

            ji.comps.push_back(component {
                .id = id,
                .hsf = (sf >> 4) & 0x0f,
                .vsf = (sf & 0x0f),
                .qt = ljsrc->read8(),
            });
        }
    }    

    void ljpeg_read_sos(struct jinfo &ji) {
        uint end, i;
        ushort id, ts;

        if (ljsrc->read16() != JPEG_SOS)
            throw std::runtime_error("SOS marker expected.");

        end = ljsrc->tell() + ljsrc->read16();

        if (ljsrc->read8() != ji.ncomp)
            throw std::runtime_error("Component amounts do not match.");
        
        for (i = 0; i<ji.ncomp; i++) {
            id = ljsrc->read8();
            ts = ljsrc->read8();
            ji.comps[id - 1].dct = (ts >> 4) & 0x0F;
            ji.comps[id - 1].act = (ts & 0x0F);
        }

        ji.spss = ljsrc->read8();
        ji.spse = ljsrc->read8();
        ji.sabp = ljsrc->read8();
        ji.prec -= ji.sabp;

        ljsrc->seek(end);
    }

    unsigned ljpeg_read_bits(int nbits, ushort * lut) {
        static unsigned buf = 0;
        static int vbits = 0;
        unsigned res = 0, in;
        
        if (nbits == 0 || vbits < 0) return 0;
        while (vbits < nbits) {
            in = ljsrc->read8();
            if (in == 0xFF) ljsrc->read8();
            buf = (buf << 8) | (in & 0xFF);
            vbits += 8;
        }

        res = (buf << (32 - vbits)) >> (32 - nbits);
        if (lut) vbits -= (lut[res] >> 8);
        else vbits -= nbits;
        return res;
    }

    ushort ljpeg_read_symbol(htable &ht) {
        unsigned bits = ljpeg_read_bits(ht.max, ht.lut.data());
        return ht.lut[bits] & 0xFF;
    }

    void ljpeg_decode_row(uint row, std::vector<ushort> &rowbuf, struct jinfo &ji) {
        uint i, j, idx;
        int len, coeff, predictor;

        for (i = 0; i<ji.width; i++) {
            for (j = 0; j<ji.ncomp; j++) {
                idx = (i * ji.ncomp) + j;
                len = ljpeg_read_symbol(ji.dcts[ji.comps[j].dct]);
                coeff = ljpeg_read_bits(len, 0);

                if ((coeff & (1 << (len-1))) == 0)
                    coeff -= (1 << len) - 1;
                
                if (i) predictor = rowbuf[idx - ji.ncomp];
                else {
                    predictor = ji.pred[j];
                    ji.pred[j] += coeff;
                }
                
                rowbuf[idx] = predictor + coeff;
            }
        }
    }

    void ljpeg_unscramble_row(uint srow, std::vector<ushort> rowbuf, std::vector<ushort> &ibuf, struct jinfo &ji) {
        uint row, col, scol, idx, slice;
        bool last;
        ushort cr2_slice[3] = {1, 2640, 2640}; // TODO: Extract from tiff tags

        for (scol=0; scol < ji.stride; scol++) {
            if (cr2_slice[0]) {
                idx = srow*ji.stride + scol;
                slice = idx / (cr2_slice[1]*ji.height);

                if ((last = slice >= cr2_slice[0]))
		            slice = cr2_slice[0];
                
                idx -= slice * (cr2_slice[1] * ji.height);
                row = idx / cr2_slice[1+last];
                col = idx % cr2_slice[1+last];
                col += slice * cr2_slice[1];
                ibuf[(ji.stride * row) + col] = rowbuf[scol];
            }          
        }
    }

    std::vector<ushort> ljpeg_decompress(struct jinfo &ji) {
        uint row, col, slice, i;
        if(ljsrc->read16() != JPEG_SOI)
            throw std::runtime_error("Not a JPEG.");

        ljpeg_read_dht(ji);
        ljpeg_read_sof(ji);
        ljpeg_read_sos(ji);

        std::vector<ushort> rowbuf(ji.stride, 0);
        std::vector<ushort> ibuf(ji.stride * ji.height, 0);

        // TODO: What is the max number of components?
        for(i=0; i<4; i++) ji.pred.push_back(1 << (ji.prec - 1)); 
       
        for (row = 0; row<ji.height; row++) {
            ljpeg_decode_row(row, rowbuf, ji);
            ljpeg_unscramble_row(row, rowbuf, ibuf, ji);
        }
        return ibuf;
    }

    Image::Image(io::AbstractIo::shared_ptr io) : io(io) {
        io->setByteorder(io::Byteorder::BigEndian);
    };

    netpbm::Image<ushort>::shared_ptr Image::decompress() {
        uint rgblen, ilen, i, row, col, j = 0;
        std::vector<ushort> ibuf;
        jinfo ji;

        io->seek(0);
        ljpeg_set_source(io);
        ibuf = ljpeg_decompress(ji);        

        auto ppm = std::make_shared<netpbm::Image<ushort>>(
            netpbm::Type::Greymap, ibuf, ji.width*ji.ncomp, ji.height, 14);
        return ppm;
    }
}
    