#pragma once
#include <vector>
#include "core/systems/command/command_interface.h"
#include "core/systems/command/command_manager.h"

namespace BS
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
