#include "area.h"

namespace NSMB
{
    AreaJyotyuType_ get_jyotyu_type
    (
        const Area& area, 
        const IO::ByteArray overlay_0, 
        const sz_t& overlay_0_offset
    )
    {
        sz_t jyotyu_ncl_offset = overlay_0_offset + area.bgnscid;
        const u8 jyotyu_ncl_id = IO::read<u8>(overlay_0, jyotyu_ncl_offset);

        if (jyotyu_ncl_id == 1)
            return AreaJyotyuType_B;
        else if (jyotyu_ncl_id == 2)
            return AreaJyotyuType_R;
        else if (jyotyu_ncl_id == 3)
            return AreaJyotyuType_W;
        else
            return AreaJyotyuType_Normal;
    }

    Area load_area_from_disk
    (
        const std::filesystem::path& header_file,
        const std::filesystem::path& bgdat_file,
        const sz_t& index
    )
    {
        NSMB::Area area;
        area.area_index = index;

        /* header */
        std::ifstream in = Util::begin_ifstream(header_file);
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
        area.start_entrance_id = IO::read<u8>(header_raw, addr);
        area.midway_entrance_id = IO::read<u8>(header_raw, addr);
        area.settings = IO::read<u8>(header_raw, addr);
        addr++;
        area.time_limit = IO::read<u16>(header_raw, addr);
        area.main_tileset_table_offset = header_raw.at(block_ids.at(0).first + 0x0C);
        area.background_top_offset = header_raw.at(block_ids.at(0).first + 0x12);
        area.background_btm_offset = header_raw.at(block_ids.at(0).first + 0x06);
        area.bgnscid = header_raw.at(block_ids.at(2).first + 0x02);


        /* bgdat */
        in = Util::begin_ifstream(bgdat_file);
        IO::ByteArray raw_data = IO::bytes_from_file(in);

        addr = 0;
        sz_t i = 0;
        while (addr < raw_data.size() - 2)
        {
            NSMB::LevelComponents::BGDatObject object;
            object.item_id = i++;
            object.unpack(raw_data, addr);
            area.items.push_back(object);
        }

        /* stageobj */
        addr = block_ids.at(6).first;
        i = 0;
        while (addr < (block_ids.at(6).first + block_ids.at(6).second))
        {
            NSMB::LevelComponents::StageObject stage_object;
            stage_object.item_id = i++;
            stage_object.unpack(header_raw, addr);
            area.items.push_back(stage_object);
        }

        /* entrance */
        addr = block_ids.at(5).first;
        i = 0;
        while (addr < (block_ids.at(5).first + block_ids.at(5).second))
        {
            NSMB::LevelComponents::Entrance entrance;
            entrance.item_id = i++;
            entrance.unpack(header_raw, addr);
            area.items.push_back(entrance);
        }

        /* view */
        addr = block_ids.at(7).first;
        i = 0;
        while (addr < (block_ids.at(7).first + block_ids.at(7).second))
        {
            NSMB::LevelComponents::View view;
            view.item_id = i++;
            view.unpack(header_raw, addr);
            area.items.push_back(view);
        }

        /* zone */

        // TODO

        /* path */

        // TODO

        return area;
    }

    void save_area_to_disk
    (
        const Area& area,
        const std::filesystem::path& header_file,
        const std::filesystem::path& bgdat_file
    )
    {
        std::ofstream out = Util::begin_ofstream(bgdat_file);
        IO::ByteArray raw_data;

        for (auto& item : area.items)
        {
            if (item.item_type == UI::AreaItemType_BgDat)
            {
                
            }
        }
        //sz_t addr = 0;
        //for (auto& item : loaded_area.objects)
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
        const UI::AreaItemType_& item_type = item.item_type;

        // count how many of those items exist 
        sz_t item_id = 0;
        for (auto& i : level.items)
            if (i.item_type == item_type)
                item_id = std::max(item_id, i.item_id);

        // update item with new id
        item.item_id = item_id + 1;

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
    }
}