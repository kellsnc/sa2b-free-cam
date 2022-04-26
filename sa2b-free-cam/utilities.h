#pragma once

static constexpr int max_player = 4;

VoidFunc(sub_4ECDF0, 0x4ECDF0);
VoidFunc(CameraCameraMode, 0x4EE490);
VoidFunc(CameraUpdateLocation_, 0x4EC840);
VoidFunc(sub_4ECC30, 0x4ECC30);
VoidFunc(CameraManager, 0x4F0730);

static const void* const loc_4F0680 = (void*)0x4F0680;
static inline void CameraManagerRegional(int num)
{
    __asm
    {
        mov edi, [num]
        call loc_4F0680
    }
}

BOOL MSetPositionWIgnoreAttribute(NJS_POINT3* p, NJS_POINT3* v, Angle3* a, int attrmask, float r);
void CameraCameraAdjust();
void CamcontSetCameraLOOKAT();
