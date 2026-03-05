#pragma once
#include "core/types.h"

namespace Core
{
    class CommandInterface
    {
    public:
        CommandInterface() = default;
        virtual ~CommandInterface() = default;

        virtual void Execute() = 0;
        virtual void Undo() = 0;
    };
}