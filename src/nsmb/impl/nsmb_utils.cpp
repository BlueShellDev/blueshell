#include "nsmb/simple_nsmb.h"

constexpr size_t NSMB::GFX::TilesetObjectRowSizeBytes(const TilesetObjectRow &objectRow)
{
    (void)objectRow;
    return sizeof(TilesetObjectControlFlag);
}

constexpr size_t NSMB::GFX::TilesetObjectSizeBytes(const TilesetObject &object)
{
    (void)object;
    return sizeof(TilesetObjectControlFlag) + sizeof(TilesetObjectControlFlag);
}
