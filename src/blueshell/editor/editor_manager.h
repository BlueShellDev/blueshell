#pragma once
#include <filesystem>
#include "core/io/binary_file.h"
#include "core/command/command_manager.h"

namespace BS
{
    struct EditableFile
    {
        std::filesystem::path locationOnDisk;
        Core::ByteArray data;

        enum class State
        {
            Empty,
            Clean,
            Dirty
        } state{State::Empty};
    };

    // editor manager that can hold up to 3 files
    // this is just a proof of concept, the editor won't use this exactly
    // but think: how could i adapt this to use areas (or other NSMB structures)?
    class EditorManager
    {
    private:
        std::array<EditableFile, 3> loadedFiles_{};
        bool SlotIsEmpty_(int slot) const;

    public:
        enum class FileSlot : int
        {
            FileA,
            FileB,
            FileC
        };

        EditableFile& GetFile(FileSlot slot);
        bool LoadFile(FileSlot slot, const std::filesystem::path& file_path);
        bool LoadFileNextAvail(const std::filesystem::path& file_path);
        bool SaveFile(FileSlot slot, const std::filesystem::path& file_path);
        bool SaveFile(FileSlot slot);
        void SaveAllFiles();
        void CloseFile(FileSlot slot);
        void CloseAllFiles();
    };
}