#include "pch.h"
#include "SA2ModLoader.h"
#include "Trampoline.h"
#include "utilities.h"
#include "camera.h"
#include "patches.h"

Trampoline* sub_43D7A0_t = nullptr;
Trampoline* Rd_CityEscape_t = nullptr;

// Reset free camera on init/restart
static void __cdecl sub_43D7A0_r()
{
    TARGET_DYNAMIC(sub_43D7A0)();
    InitFreeCamera();
}

// City Escape Truck Chase
void __cdecl Rd_CityEscape_r()
{
    TARGET_DYNAMIC(Rd_CityEscape)();

    for (int i = 0; i < max(1, TwoPlayerMode); ++i)
    {
        auto ptwp = MainCharObj1[i];

        if (ptwp)
        {
            if (ptwp->Position.z > 15910.0f && ptwp->Position.x < 4110.0f)
            {
                SetFreeCameraMode(1, i);
            }
            else if (ptwp->Position.z > 9273.0f)
            {
                SetFreeCameraMode(0, i);
            }
        }
        
    }
}

void InitPatches()
{
    sub_43D7A0_t = new Trampoline(0x43D7A0, 0x43D7A7, sub_43D7A0_r);
    Rd_CityEscape_t = new Trampoline(0x5DD4F0, 0x5DD4F7, Rd_CityEscape_r);
}