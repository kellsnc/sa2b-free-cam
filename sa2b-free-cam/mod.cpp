#include "pch.h"
#include <math.h>
#include "SA2ModLoader.h"
#include "Trampoline.h"

enum : unsigned int
{
    MODE_4 = 0x4,
    MODE_8 = 0x8,
    MODE_40000000 = 0x40000000,
    MODE_80000000 = 0x80000000
};

struct FCWRK
{
    bool enabled;
    unsigned int mode;
	float dist1;
	float dist2;
	float dist0;
	float dist;
	Angle3 _ang;
	NJS_POINT3 cammovepos;
	NJS_POINT3 campos;
	int counter;
	int timer;
	Angle3 pang;
	NJS_POINT3 camspd;
	NJS_POINT3 pos;
	NJS_POINT3 _vec;
};

static constexpr int max_player = 4;
static FCWRK fcwrk[max_player];

DataPointer(BYTE, byte_174AFEB, 0x174AFEB);
DataPointer(DWORD, dword_19EF36C, 0x19EF36C);
DataPointer(DWORD, dword_1DCFF04, 0x1DCFF04);

VoidFunc(sub_4ECDF0, 0x4ECDF0);
VoidFunc(sub_4EE490, 0x4EE490);
VoidFunc(sub_4EC840, 0x4EC840);
VoidFunc(sub_4ECC30, 0x4ECC30);
VoidFunc(sub_4EC920, 0x4EC920);
VoidFunc(sub_4EB6E0, 0x4EB6E0);
VoidFunc(sub_4EC9A0, 0x4EC9A0);
VoidFunc(sub_4EC520, 0x4EC520);

static const void* const loc_4EC770 = (void*)0x4EC770;
static inline void sub_4EC770(int num)
{
    __asm
    {
        mov ecx, [num]
        call loc_4EC770
    }
}

static const void* const loc_4F0680 = (void*)0x4F0680;
static inline void sub_4F0680(int num)
{
    __asm
    {
        mov edi, [num]
        call loc_4F0680
    }
}

using SomeFuncPtr = void(__cdecl*)(CameraInfo*, char*);

BOOL MSetPositionWIgnoreAttribute(NJS_POINT3* p, NJS_POINT3* v, Angle3* a, int attrmask, float r)
{
    return FALSE;
}

void FreeCam_GetDistances(FCWRK* cam, EntityData1* pltwp)
{
    cam->dist1 = 16.0f;
    cam->dist2 = 50.0f;
}

void FreeCam_CalcOrigin(FCWRK* cam, EntityData1* pltwp)
{
    NJS_VECTOR unitvector = { 0.0f, 0.0f, cam->dist };
    Angle y = cam->_ang.y;
    Angle x = cam->_ang.x;

    njPushMatrix(_nj_unit_matrix_);
    if (y) njRotateY(_nj_current_matrix_ptr_, y);
    if (x) njRotateX(_nj_current_matrix_ptr_, x);
    njCalcPoint(_nj_current_matrix_ptr_, &unitvector, &unitvector, FALSE);
    njPopMatrixEx();

    cam->pos.x = unitvector.x + pltwp->Position.x;
    cam->pos.y = unitvector.y + pltwp->Position.y + 10.5f;
    cam->pos.z = unitvector.z + pltwp->Position.z;
}

void FreeCamera(int screen)
{
    auto cam = &fcwrk[screen];
    auto pltwp = MainCharObj1[screen];
    auto plmwp = MainCharData2[screen];
    auto plpwp = MainCharObj2[screen];
    auto plper = &Controllers[screen];
    
    if (!pltwp)
    {
        return;
    }

    NJS_VECTOR vec;
    NJS_VECTOR dyncolpos;
    NJS_VECTOR freecampos;

    if (cam->timer)
    {
        --cam->timer;
    }
    else
    {
        cam->mode &= ~MODE_8;
    }

    if ((cam->mode & MODE_80000000))
    {
        FreeCam_GetDistances(cam, pltwp);
        vec.x = cam->pos.x - pltwp->Position.x;
        vec.y = cam->pos.y - pltwp->Position.y - 10.5f;
        vec.z = cam->pos.z - pltwp->Position.z;
        float magnitude = fabsf(njScalor(&vec));
        njUnitVector(&vec);
        cam->_ang.y = NJM_RAD_ANG(atan2(vec.x, vec.z));
        cam->_ang.x = NJM_RAD_ANG(-asin(vec.y));
        cam->_ang.z = 0;

        if (magnitude < cam->dist1 || magnitude > cam->dist2)
        {
            cam->dist0 = cam->dist1;
            cam->dist = cam->dist1;
        }
        else
        {
            cam->dist0 = magnitude;
            cam->dist = magnitude;
        }

        FreeCam_CalcOrigin(cam, pltwp);
        cam->campos = cam->pos;
        cam->counter = 0;
        cam->pang.x = 0;
        cam->pang.y = 0;
        cam->pang.z = 0;
        cam->mode |= MODE_40000000;
    }

    if (cam->mode & MODE_40000000)
    {
        if (!(cam->mode & MODE_80000000))
        {
            FreeCam_GetDistances(cam, pltwp);
            cam->counter = 0;
            cam->dist0 = cam->dist1;
            cam->dist = cam->dist1;
            FreeCam_CalcOrigin(cam, pltwp);
            cam->campos = cam->pos;
        }

        int passes = 60;
        while (1)
        {
            cam->dist0 = cam->dist;
            cam->dist *= 1.2f;

            if (cam->dist >= cam->dist2)
            {
                break;
            }

            FreeCam_CalcOrigin(cam, pltwp);
            cam->cammovepos = cam->campos;
            cam->camspd = { 0.0f, 0.0f, 0.0f };

            freecampos = cam->campos;
            dyncolpos = { 0.0f, 0.0f, 0.0f };

            MSetPositionWIgnoreAttribute(&freecampos, &dyncolpos, nullptr, SurfaceFlag_Water | SurfaceFlag_WaterNoAlpha, 10.0f);

            cam->campos.x = dyncolpos.x + freecampos.x;
            cam->campos.y = dyncolpos.y + freecampos.y;
            cam->campos.z = dyncolpos.z + freecampos.z;

            --passes;

            if (passes == 0 || cam->dist0 == cam->dist)
            {
                break;
            }

        }

        cam->mode &= ~(MODE_80000000 | MODE_40000000);
    }

    if (cam->pang.y > 0)
    {
        cam->pang.y -= 32;

        if (cam->pang.y < 0)
        {
            cam->pang.y = 0;
        }
    }
    else if (cam->pang.y < 0)
    {
        cam->pang.y += 32;

        if (cam->pang.y > 0)
        {
            cam->pang.y = 0;
        }
    }

    if ((plper->x2 > 0 || (plper->r - 128) << 8 > 128) && cam->pang.y < 256)
    {
        cam->pang.y += 64;
    }
    else if ((plper->x2 < 0 || (plper->l - 128) << 8 > 128) && cam->pang.y > -256)
    {
        cam->pang.y -= 64;
    }

    cam->_ang.y += cam->pang.y;

    if (cam->pang.x > 0)
    {
        cam->pang.x -= 32;

        if (cam->pang.x < 0)
        {
            cam->pang.x = 0;
        }
    }
    else if (cam->pang.x < 0)
    {
        cam->pang.x += 32;

        if (cam->pang.x > 0)
        {
            cam->pang.x = 0;
        }
    }

    if (plper->y2 > 0 && cam->pang.x < 256)
    {
        cam->pang.x += 64;
    }
    else if (plper->y2 < 0 && cam->pang.x > -256)
    {
        cam->pang.x -= 64;
    }

    cam->_ang.x += cam->pang.x;

    if (cam->_ang.x >= -12288)
    {
        if (cam->_ang.x > -512)
        {
            cam->_ang.x = -513;
        }
    }
    else
    {
        cam->_ang.x = -12287;
    }

    cam->dist = cam->dist0;

    FreeCam_CalcOrigin(cam, pltwp);
    cam->cammovepos = cam->pos;
    cam->camspd.x = cam->pos.x - cam->campos.x;
    cam->camspd.y = cam->pos.y - cam->campos.y;
    cam->camspd.z = cam->pos.z - cam->campos.z;

    freecampos = cam->campos;
    dyncolpos = cam->camspd;
    int water_cdt = 0;

    if (MSetPositionWIgnoreAttribute(&freecampos, &dyncolpos, nullptr, SurfaceFlag_Water | SurfaceFlag_WaterNoAlpha, 10.0f))
    {
        freecampos = cam->campos;
        dyncolpos = cam->camspd;
        water_cdt = 1;

        if (MSetPositionWIgnoreAttribute(&freecampos, &dyncolpos, nullptr, SurfaceFlag_Water | SurfaceFlag_WaterNoAlpha, 8.0f))
        {
            water_cdt = 2;
        }
    }

    cam->pos.x = dyncolpos.x + freecampos.x;
    cam->pos.y = dyncolpos.y + freecampos.y;
    cam->pos.z = dyncolpos.z + freecampos.z;
    cam->campos = cam->pos;
    vec.x = plmwp->Velocity.x;
    vec.y = plmwp->Velocity.y;
    vec.z = plmwp->Velocity.z;

    if (fabsf(vec.x) >= 1.0f)
    {
        vec.x *= 1.1f;
    }
    else
    {
        vec.x = 0.0f;
    }

    if (fabsf(vec.y) >= 1.0f)
    {
        vec.y *= 1.1f;
    }
    else
    {
        vec.y = 0.0f;
    }

    if (fabsf(vec.z) >= 1.0f)
    {
        vec.z *= 1.1f;
    }
    else
    {
        vec.z = 0.0f;
    }

    vec.x = cam->campos.x - pltwp->Position.x - vec.x;
    vec.y = cam->campos.y - pltwp->Position.y - vec.y - 10.5f;
    vec.z = cam->campos.z - pltwp->Position.z - vec.z;
    njUnitVector(&vec);

    cam->_ang.y = NJM_RAD_ANG(atan2f(vec.x, vec.z));
    cam->_ang.x = NJM_RAD_ANG(-asinf(vec.y));
    vec.x = cam->campos.x - pltwp->Position.x;
    vec.y = cam->campos.y - pltwp->Position.y - 10.5f;
    vec.z = cam->campos.z - pltwp->Position.z;
    cam->dist = fabsf(njScalor(&vec));

    if (cam->dist <= cam->dist2)
    {
        cam->counter = 0;
    }
    else if (++cam->counter > 20)
    {
        cam->mode |= MODE_40000000;
    }

    if (water_cdt)
    {
        if (cam->dist <= cam->dist1)
        {
            cam->dist = cam->dist1;
        }
    }
    else
    {
        cam->dist *= 1.2f;

        if (cam->dist >= cam->dist2)
        {
            cam->dist = cam->dist2;
        }
    }

    cam->dist0 = cam->dist;

    CameraData[screen].pos = cam->campos;
    CameraData[screen].ang = cam->_ang;

    NJS_VECTOR dir = { 0.0f, 0.0f, -1.0f };
    njPushUnitMatrix();
    njRotateY_(_nj_current_matrix_ptr_, cam->_ang.y);
    njRotateX_(_nj_current_matrix_ptr_, cam->_ang.x);
    njCalcVector(&dir, &dir, _nj_current_matrix_ptr_);
    njPopMatrixEx();
    CameraData[screen].dir = dir;
}

void AutoCamera(int screen)
{
    CameraGetStopFlag(screen);
    CameraGetPlayerLastPos();
    if (screen < 2)
    {
        sub_4F0680(screen);
        DoThingForEveryScreen();
    }
    sub_4EE490();
    sub_4EC520();
    if (CameraTimer)
    {
        auto v4 = &CameraData[screen].gap1B8[0x114 * *(DWORD*)&CameraData[screen].gap1B8[156] + 0x11E0];
        SomeFuncPtr func = (SomeFuncPtr) * ((DWORD*)v4 + 3);

        if (func)
        {
            func(&CameraData[screen], v4);
            ++*((DWORD*)v4 + 2);
        }

        sub_4EC520();
        sub_4EC840();
        if (!byte_174AFEB)
        {
            if (dword_1DCFF04)
            {
                sub_4EC9A0();
                sub_4EC520();
            }
        }
    }
    sub_4EC770(screen);
    sub_4ECC30();
    sub_4EC920();
    sub_4EB6E0();
}

void __cdecl cameraCons_Main_r(ObjectMaster* tp)
{
    if (!dword_19EF36C)
    {
        sub_4ECDF0();
        for (CurrentScreen = 0; CurrentScreen < SplitscreenMode; ++CurrentScreen)
        {
            if (fcwrk[CurrentScreen].enabled)
            {
                FreeCamera(CurrentScreen);
            }
            else
            {
                AutoCamera(CurrentScreen);
            }
        }

        ++CameraTimer;

        CameraData1 = tp->Data1.Entity;
        CameraData1->Position = CameraData[0].pos;
        CameraData1->Rotation = CameraData[0].ang;
        CameraData1->Scale = { 1.0f, 1.0f, 1.0f };
        CurrentScreen = 0;
        CameraApplyScreenView(0);
    }
}

void SetFreeCameraMode(int sw, int num)
{
    if (sw)
    {
        if (!(fcwrk[num].mode & MODE_4))
            fcwrk[num].mode |= MODE_80000000 | MODE_4;
    }
    else
    {
        fcwrk[num].mode = fcwrk[num].mode & ~MODE_4 | MODE_80000000;
    }
}

extern "C"
{
	__declspec(dllexport) void __cdecl Init(const char* path, const HelperFunctions& helperFunctions)
	{
        WriteJump(cameraCons_Main, cameraCons_Main_r);
	}

    __declspec(dllexport) void __cdecl OnControl()
    {
        for (int i = 0; i < max_player; ++i)
        {
            if ((Controllers[i].press & (Buttons_L | Buttons_R)) == (Buttons_L | Buttons_R))
            {
                fcwrk[i].enabled = !fcwrk[i].enabled;
                SetFreeCameraMode(fcwrk[i].enabled, i);
            }
        }
    }

	__declspec(dllexport) ModInfo SA2ModInfo = { ModLoaderVer };
}