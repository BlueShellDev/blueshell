#include "command_manager.h"
#include "core/systems/console/console.h"

BS::CommandManager::~CommandManager()
{
    for (CommandInterface *command : this->undoStack)
        delete command;
    for (CommandInterface *command : this->redoStack)
        delete command;
}

void BS::CommandManager::ExecuteCommand(CommandInterface *command)
{
    command->Execute();
    this->undoStack.push_back(command);
    // clear redo stack since we have a new command executed
    for (CommandInterface *redoCommand : this->redoStack)
        delete redoCommand;
    this->redoStack.clear();
}

void BS::CommandManager::Undo()
{
    if (this->undoStack.empty())
        return;

    CommandInterface *command = this->undoStack.back();
    command->Undo();
    this->undoStack.pop_back();
    this->redoStack.push_back(command);
}

void BS::CommandManager::Redo()
{
    if (this->redoStack.empty())
        return;

    CommandInterface *command = this->redoStack.back();
    command->Execute();
    this->redoStack.pop_back();
    this->undoStack.push_back(command);
}