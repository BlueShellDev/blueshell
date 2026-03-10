#pragma once

namespace BS::UI
{
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