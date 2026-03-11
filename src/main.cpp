#include <iostream>
#include "blueshell/blueshell.h"

// HEY! you! looking for the main loop?
// it's lin- eh, better just show you. follow meeee! -----------------------+
                                                                        //  |                                                                
int main(int, char **)                                                  //  |
{                                                                       //  |
    BS::MainContext* blueshell = BS::CreateEmptyContext();              //  | almost there!
    if(!BS::InitContext(blueshell, "BlueShell", 1920, 1080))            //  |
        return EXIT_FAILURE;                                            //  |
                                                                        //  |
    BS::RunUI(blueshell);  // < ----- over here, in this function! ---------+

    BS::DestroyContext(blueshell);
    return EXIT_SUCCESS;
}