#include "pch.h"
#include "SA2ModLoader.h"
#include "Trampoline.h"
#include "utilities.h"
#include "camera.h"

Trampoline* SetCameraTask_t = nullptr;

static void SetCameraTask_r()
{
    ((decltype(SetCameraTask_r)*)SetCameraTask_t->Target())();
    InitFreeCamera();
}

extern "C"
{
	__declspec(dllexport) void __cdecl Init(const char* path, const HelperFunctions& helperFunctions)
	{
        WriteJump(cameraCons_Main, Camera_r);
        SetCameraTask_t = new Trampoline(0x4EBA50, 0x4EBA55, SetCameraTask_r);
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
