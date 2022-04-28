#include "pch.h"
#include "SA2ModLoader.h"
#include "config.h"
#include "utilities.h"
#include "camera.h"
#include "menu.h"
#include "patches.h"

extern "C"
{
	__declspec(dllexport) void __cdecl Init(const char* path, const HelperFunctions& helperFunctions)
	{
        config::read(path);

        WriteJump(cameraCons_Main, Camera_r);

        InitPatches();

        if (config::menu_enabled)
        {
            InitMenu();
        }
	}

    __declspec(dllexport) void __cdecl OnControl()
    {
        for (int i = 0; i < max_player; ++i)
        {
            if ((Controllers[i].press & (Buttons_L | Buttons_R)) == (Buttons_L | Buttons_R))
            {
                SetFreeCamera(!GetFreeCamera(i), i);
            }
        }
    }

	__declspec(dllexport) ModInfo SA2ModInfo = { ModLoaderVer };
}
