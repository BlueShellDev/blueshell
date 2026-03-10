#pragma once

namespace BS::UI
{
    // why not just have a struct for each window with a bool for openness and a draw function?
    
    // sigh, struct of bools, do you even reduce to bits?
    struct VisibilityState
    {
        // windows
        bool ShowConsole            {true};
        bool ShowDebug              {false};
        bool ShowTextureViewer      {false};
        bool ShowFileNameTableViewer{true};
        bool ShowPluginMgr          {false};

        // popups
        bool ShowAboutPopup         {false};
    };
}