#pragma once
#include <vector>
#include "core/types.h"
#include "core/console.h"

namespace Core
{
    using Addr32 = u32;

    struct ByteArray
    {
        static bool LogReadsAndWrites;

        std::vector<u8> bytes;
        enum class Endianness
        {
            LittleEndian,
            BigEndian
        } endianness{Endianness::LittleEndian};

        inline ByteArray() = default;
        inline ByteArray(const std::vector<u8> &data, Endianness endianness = Endianness::LittleEndian) : bytes(data), endianness(endianness) {}

        // probably should do a concept and check T is an integral type or something but for now just assume it's used correctly
        template <typename T>
        T Read(Addr32 offset, const T &mask = ~0) const
        {
            T value{};
            if (offset + sizeof(T) > bytes.size())
                return value; // or throw an exception
            if (endianness == Endianness::LittleEndian)
            {
                for (size_t i = 0; i < sizeof(T); ++i)
                    reinterpret_cast<u8 *>(&value)[i] = bytes[offset + i];
            }
            else
            {
                for (size_t i = 0; i < sizeof(T); ++i)
                    reinterpret_cast<u8 *>(&value)[sizeof(T) - 1 - i] = bytes[offset + i];
            }
            value &= mask;
            if (LogReadsAndWrites)
                Core::Console::Global().Log("Read value {} from offset {} (0x{:X})", value, offset, offset);
            return value;
        }

        template <typename T>
        T ReadEx(Addr32 &offset, const T &mask = ~0) const
        {
            T value = Read<T>(offset, mask);
            offset += sizeof(T);
            return value;
        }

        template <typename T>
        std::vector<T> ReadArray(Addr32 offset, size_t size, const T &entry_mask = ~0ULL) const
        {
            std::vector<T> array;
            size_t current{offset};
            for (size_t i = 0; i < size; ++i)
                array.push_back(ReadEx<T>(current, entry_mask));
            return array;
        }

        template <typename T>
        void Write(Addr32 offset, T value)
        {
            if (offset + sizeof(T) > bytes.size())
                return; // or throw an exception
            if (endianness == Endianness::LittleEndian)
            {
                for (size_t i = 0; i < sizeof(T); ++i)
                    bytes[offset + i] = reinterpret_cast<u8 *>(&value)[i];
            }
            else
            {
                for (size_t i = 0; i < sizeof(T); ++i)
                    bytes[offset + i] = reinterpret_cast<u8 *>(&value)[sizeof(T) - 1 - i];
            }
            if (LogReadsAndWrites)
                Core::Console::Global().Log("Wrote value {} to offset {} (0x{:X})", value, offset, offset);
        }

        template <typename T>
        void WriteEx(Addr32 &offset, T value)
        {
            Write<T>(offset, value);
            offset += sizeof(T);
        }

        template <typename T>
        void WriteArray(Addr32 offset, const std::vector<T> &values)
        {
            for (const T &value : values)
                Write(offset, value);
        }
    };
};