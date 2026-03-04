#pragma once
#include "core/types.h"

namespace BS
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