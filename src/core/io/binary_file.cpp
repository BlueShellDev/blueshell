#include "binary_file.h"
#include <fstream>

Core::ByteArray Core::ReadByteArrayFromFile(const std::string &filePath)
{
    std::ifstream file(filePath, std::ios::binary);
    if (!file)
    {
        Core::Console::Global().Err("Failed to open file: {}", filePath);
        return ByteArray();
    }

    std::vector<u8> data((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    Core::Console::Global().Log("Opened {} bytes from file: {}", data.size(), filePath);
    return ByteArray(data);
}

void Core::WriteByteArrayToFile(const std::string &filePath, const ByteArray &data)
{
    std::ofstream file(filePath, std::ios::binary);
    if (!file)
    {
        Core::Console::Global().Err("Failed to open file for writing: {}", filePath);
        return;
    }
    file.write(reinterpret_cast<const char *>(data.bytes.data()), data.bytes.size());
    Core::Console::Global().Log("Wrote {} bytes to file: {}", data.bytes.size(), filePath);
}
