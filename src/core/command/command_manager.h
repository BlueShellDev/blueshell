#pragma once
#include <vector>
#include "core/command/command_interface.h"
#include "core/command/command_manager.h"

namespace Core
{
    class CommandManager
    {
    private:
        std::vector<CommandInterface *> undoStack;
        std::vector<CommandInterface *> redoStack;

    public:
        CommandManager() = default;
        ~CommandManager();

        void ExecuteCommand(CommandInterface *command);
        void Undo();
        void Redo();
    };
}
