#include "area.h"

namespace NSMB
{
    Area load_area
    (
        const Editor::Environment& env,
        const Editor::AreaSource& source
    )
    {
        NSMB::Area level;
        level.source = source;

        /* header */
        std::ifstream in = Util::begin_ifstream(env.fs / env.root / env.stages / level.source.header_fn);
        IO::ByteArray header_raw = IO::bytes_from_file(in);

        std::vector<std::pair<u32, u32>> block_ids;

        sz_t addr = 0;
        for (sz_t i = 0; i < 14; i++)
        {
            u32 block_offset = IO::read<u32>(header_raw, addr);
            u32 block_size = IO::read<u32>(header_raw, addr);
            block_ids.push_back(std::pair<u32, u32>(block_offset, block_size));
        }

        /* settings */
        addr = block_ids.at(0).first;
        level.start_entrance_id = IO::read<u8>(header_raw, addr);
        level.midway_entrance_id = IO::read<u8>(header_raw, addr);
        level.settings = IO::read<u8>(header_raw, addr);
        addr++;
        level.time_limit = IO::read<u16>(header_raw, addr);
        level.main_tileset_table_offset = header_raw.at(block_ids.at(0).first + 0x0C);
        level.background_top_offset = header_raw.at(block_ids.at(0).first + 0x12);
        level.background_btm_offset = header_raw.at(block_ids.at(0).first + 0x06);
        level.bgnscid = header_raw.at(block_ids.at(2).first + 0x02);


        /* bgdat */
        in = Util::begin_ifstream(env.fs / env.root / env.stages / level.source.bgdat_fn);
        IO::ByteArray raw_data = IO::bytes_from_file(in);

        addr = 0;
        sz_t i = 0;
        while (addr < raw_data.size() - 2)
        {
            NSMB::LevelComponents::BGDatObject object;
            object.item_id = i++;
            object.unpack(raw_data, addr);
            level.items.push_back(object);
        }

        /* stageobj */
        addr = block_ids.at(6).first;
        i = 0;
        while (addr < (block_ids.at(6).first + block_ids.at(6).second))
        {
            NSMB::LevelComponents::StageObject stage_object;
            stage_object.item_id = i++;
            stage_object.unpack(header_raw, addr);
            level.items.push_back(stage_object);
        }

        /* entrance */
        addr = block_ids.at(5).first;
        i = 0;
        while (addr < (block_ids.at(5).first + block_ids.at(5).second))
        {
            NSMB::LevelComponents::Entrance entrance;
            entrance.item_id = i++;
            entrance.unpack(header_raw, addr);
            level.items.push_back(entrance);
        }

        /* view */
        addr = block_ids.at(7).first;
        i = 0;
        while (addr < (block_ids.at(7).first + block_ids.at(7).second))
        {
            NSMB::LevelComponents::View view;
            view.item_id = i++;
            view.unpack(header_raw, addr);
            level.items.push_back(view);
        }

        /* zone */

        // TODO

        /* path */

        // TODO

        return level;
    }

    void save_area
    (
        Area& level,
        std::string bgdat_fn
    )
    {
        //std::ofstream out = Util::begin_ofstream(bgdat_fn);
        //IO::ByteArray raw_data;

        //raw_data.resize((loaded_area.objects.size() * 10) + 2);

        //sz_t addr = 0;
        //for (auto& object : loaded_area.objects)
        //{
        //    object.pack(raw_data, addr);
        //}

        //u16 end = 0xFFFF;
        //IO::write(end, raw_data, addr);

        //IO::bytes_to_file(raw_data, out);
    }

    void add_item
    (
        Area& level,
        UI::AreaRenderItem& item
    )
    {
        // first, check type of item we're adding
        const UI::AreaItemType& item_type = item.item_type;

        // count how many of those items exist 
        sz_t item_id = 0;
        for (auto& item : level.items)
            if (item.item_type == item_type)
                item_id++;

        // update item with new id
        item.item_id = item_id;

        // append item to list
        level.items.push_back(item);
    }

    void remove_item
    (
        Area& level,
        UI::AreaRenderItem& remove
    )
    {
        // if we match an item in the loaded_area, kill it
        auto erased = std::erase_if
        (
            level.items,
            [remove](UI::AreaRenderItem& i)
            {
                return ((i.item_type == remove.item_type) && (i.item_id == remove.item_id));
            }
        );

        //// shift item ids down one row starting from where we erased
        //for (auto it = loaded_area.items.begin() + erased; it != loaded_area.items.end(); ++it)
        //    if (it->item_type == remove.item_type)
        //        it->item_id--;
    }
}