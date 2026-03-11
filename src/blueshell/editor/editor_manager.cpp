#include "editor_manager.h"
#include "core/console.h"

bool BS::EditorManager::SlotIsEmpty_(int slot) const
{
    return (loadedFiles_.at(slot).state == EditableFile::State::Empty);
}

BS::EditableFile &BS::EditorManager::GetFile(FileSlot slot)
{
    return loadedFiles_.at(static_cast<int>(slot));
}

bool BS::EditorManager::LoadFile(FileSlot slot, const std::filesystem::path &file_path)
{
    if (!SlotIsEmpty_(static_cast<int>(slot)))
    {
        Core::Console::Global().Warn("Could not load file {} into slot {} because the slot is not empty", 
            file_path.string(), 
            static_cast<int>(slot));
        return false;
    }

    GetFile(slot).data = Core::ReadByteArrayFromFile(file_path);
    GetFile(slot).locationOnDisk = file_path;
    GetFile(slot).state = EditableFile::State::Clean;
    return true;
}

bool BS::EditorManager::LoadFileNextAvail(const std::filesystem::path &file_path)
{
    for (int i = 0; i < 3; i++)
    {
        if(SlotIsEmpty_(i))
            return LoadFile(static_cast<FileSlot>(i), file_path);
    }

    Core::Console::Global().Warn("All file slots are being used");
    return false;
}

bool BS::EditorManager::SaveFile(FileSlot slot, const std::filesystem::path &file_path)
{
    if (SlotIsEmpty_(static_cast<int>(slot)))
    {
        Core::Console::Global().Warn("Nothing in slot {} to save!", 
            static_cast<int>(slot));
        return false;
    }

    Core::WriteByteArrayToFile(file_path, GetFile(slot).data);
    GetFile(slot).state = BS::EditableFile::State::Clean;
    return true;
}

bool BS::EditorManager::SaveFile(FileSlot slot)
{
    return SaveFile(slot, GetFile(slot).locationOnDisk);
}

void BS::EditorManager::SaveAllFiles()
{
    for (int i = 0; i < 3; i++)
    {
        if(!SaveFile(static_cast<FileSlot>(i)))
            continue;
    }
}

void BS::EditorManager::CloseFile(FileSlot slot)
{
    GetFile(slot).locationOnDisk.clear();
    GetFile(slot).data.bytes.clear();
    GetFile(slot).state = EditableFile::State::Empty;
}
