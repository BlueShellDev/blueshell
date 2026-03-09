#include <iostream>
#include "blueshell/blueshell.h"

int main(int, char **)
{
    BS::MainContext* blueshell = BS::CreateEmptyContext();
    if(!BS::InitContext(blueshell, "BlueShell v0.2.0a", 1920, 1080))
        return EXIT_FAILURE;

    BS::RunUI(blueshell);

    BS::DestroyContext(blueshell);
    return EXIT_SUCCESS;
}