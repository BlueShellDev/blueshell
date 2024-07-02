#pragma once

#include <vector>
#include <array>
#include <fstream>

// helper types
using u8    = std::uint8_t;
using u16   = std::uint16_t;
using u32   = std::uint32_t;
using u64   = std::uint64_t;

// signed
using s8    = std::int32_t;
using s16   = std::int16_t;
using s32   = std::int32_t;
using s64   = std::int64_t;

// make size_t shorter :p
using sz_t  = std::size_t;

namespace IO
{
    // define trivially copyable constraint
    template<class T>
    concept Trivial = std::is_trivially_copyable_v<T>;

    enum class Endian { Little, Big };

    // a byte array is simlpy a alias to a vector of u8
    using ByteArray = std::vector<u8>;

    // read a bytearray from a filestream
    inline
    ByteArray
    bytes_from_file
    (
        std::ifstream& file,
        const sz_t& offset = 0,
        const sz_t& size = 0
    )
    {
        ByteArray res;
        file.seekg(0, std::ios::end);

        // if size was never speicified use the whole file
        if (size < 1)
            res.resize(file.tellg());
        else 
            res.resize(size);
        
        file.seekg(offset, std::ios::beg);
        file.read
        (
            reinterpret_cast<char*>(res.data()),
            res.size()
        );

        return res;
    }
    //
    
    // write a bytearray into a filestream
    inline
    void
    bytes_to_file
    (
        ByteArray& bytes,
        std::ofstream& file,
        const sz_t& offset = 0
    )
    {
        file.seekp(offset, std::ios::beg);
        file.write
        (
            reinterpret_cast<const char*>(bytes.data()),
            bytes.size()
        );
    }
    //
    
    // read a value from a byte array at a specified address
    template <Trivial T, Endian E = Endian::Little>
    T read
    (
        const ByteArray& from, 
        sz_t& at
    )
    {
        T res = 0;
        for (sz_t i = 0; i < sizeof(T); i++)
        {
            sz_t lit_iter = i + at;
            sz_t big_iter = (sizeof(T) - i - 1) + at;

            switch (E)
            {
            case Endian::Little:
                res += from.at(lit_iter) << i * 8;
                break;
            case Endian::Big:
                res += from.at(big_iter) << i * 8;
                break;
            }
        }
        at += sizeof(T);
        return res;
    }
    //

    // write a value to a byte array at a specified address
    template <Trivial T, Endian E = Endian::Little>
    void write
    (
        const T& value, 
        ByteArray& to, 
        sz_t& at
    )
    {
        for (sz_t i = 0; i < sizeof(T); i++)
        {
            sz_t lit_iter = i + at;
            sz_t big_iter = (sizeof(T) - i - 1) + at;

            switch (E)
            {
            case Endian::Little:
                to.at(lit_iter) = (value >> 8 * i) & 0xFF;
                break;
            case Endian::Big:
                to.at(big_iter) = (value >> 8 * i) & 0xFF;
                break;
            }
        }
        at += sizeof(T);
    }
    //
}