#pragma once

#include "../../ui/items/level_render_item.h"
#include "../../util/util.hpp"

namespace NSMB::LevelComponents
{
    struct BGDatObject : UI::AreaRenderItem
    {
        void unpack(IO::ByteArray& from, sz_t& at);
        void pack(IO::ByteArray& to, sz_t& at);
    };

    struct StageObject : UI::AreaRenderItem
    {
        IO::ByteArray data;

        void unpack(IO::ByteArray& from, sz_t& at);
        void pack(IO::ByteArray& to, sz_t& at);
    };

    struct Entrance : UI::AreaRenderItem
    {
        void unpack(IO::ByteArray& from, sz_t& at);
        void pack(IO::ByteArray& to, sz_t& at);
    };

    struct View : UI::AreaRenderItem
    {
        void unpack(IO::ByteArray& from, sz_t& at);
        void pack(IO::ByteArray& to, sz_t& at);
    };

    struct Path : UI::AreaRenderItem
    {
        void unpack(IO::ByteArray& from, sz_t& at);
        void pack(IO::ByteArray& to, sz_t& at);
    };
}