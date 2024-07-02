#pragma once

#include "byteio.hpp"

namespace LZ
{
    // decompress an lz77 (no magic number) compressed filestream into a byte array
    static
    IO::ByteArray decompress(IO::ByteArray& compressed)
    {
        IO::ByteArray result;
        sz_t compressed_iter = 0;

        // get first 4 bytes and parse
        u32 header = IO::read<u32>(compressed, compressed_iter);

        // first 3 bytes are uncomp data size, last byte is comp type which
        // should always be 10h
        u32 expected_size = (header >> 8) & 0x00FFFFFF;
        u8  comp_type     = (header)      & 0x000000FF;

        // resize output vector appropriately and check type
        if (comp_type != 0x10)
            throw std::invalid_argument("file is not type 10h compressed!");

        // header parsing is done, now start to read chunks   
        // chunks made up of 8 entries, always start with a flag byte
        // each bit of flag byte says whether the corresponding entry is a
        // literal data or a reference (1st bit is 1st entry, 2nd is 2nd, ...)
        // literal data is 8 bits, reference is 16 bits

        while (result.size() < expected_size)
        {

        // first, read flag byte, then iterate thru chunk
        // also iterate thru each bit of flag
        u8 flag = IO::read<u8>(compressed, compressed_iter);

        for (sz_t _i = 0; _i < 8; _i++)
        {
            // check the msb of the flag (1st chunk), then just shift flag to the right 1 bit at a time
            // if flagged, it is a reference (2 byte structure), else its just data (byte)
            if (flag & 0b10000000)
            {
                u16 reference = IO::read<u16, IO::Endian::Big>(compressed, compressed_iter);

                // reference format:
                // LLLLDDDDDDDDDDDD
                // L - length of referred data
                // D - displacement needed to calculate the ptr to data

                // why are we adding 3?????
                u8 length           = (reference >> 12) & 0x000F;
                length              += 0x3; // (??)
                // some kind of value needed to calculate the offset of the decomp'd data (?)
                u16 offset          = (reference)       & 0x0FFF;
                // now actually get the offset
                sz_t window_offset  = result.size()     - offset;
                window_offset       -= 1; // off by one nonsense

                // now that the reference struct is unpacked, we can get the decomp'd data
                for (sz_t _j = 0; _j < length; _j++)
                {
                    result.emplace_back( result.at(window_offset) );
                    window_offset++;
                    // check if we're bigger than we should be and leave if so
                    if (result.size() >= expected_size) break;
                }
            }

            // else, flag was not raised, so just copy the literal data
            else
            {
                u8 literal_byte = IO::read<u8>(compressed, compressed_iter);
                result.emplace_back(literal_byte);
            }

            // rotate flag byte so we only have to check the msb
            flag <<= 1;

            // lastly (again) check if we've exceeded bounds
            if (result.size() >= expected_size) break;

        } // end flag for-loop

        } // end while

        return result;
    }

    // compress a vector of bytes using LZ type 16 (10h) compression
    static 
    void compress(IO::ByteArray& decompressed)
    {
        // todo: impl
    };
};