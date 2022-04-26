#include "pch.h"
#include <math.h>
#include "SA2ModLoader.h"
#include "utilities.h"
#include "camera.h"

enum : unsigned int
{
    MODE_1 = 0x1,
    MODE_2 = 0x2,
    MODE_4 = 0x4,
    MODE_8 = 0x8,
    MODE_40000000 = 0x40000000,
    MODE_80000000 = 0x80000000
};

struct FCWRK
{
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

static FCWRK fcwrk[max_player];

static const float min_dist = 16.0f;
static const float max_dist = 50.0f;
static const float height = 10.5f;
static const Angle analog_spd = 0x40;

static void FreeCam_GetDistances(FCWRK* cam, EntityData1* pltwp)
{
    cam->dist1 = min_dist;
    cam->dist2 = max_dist;
}

static void FreeCam_CalcOrigin(FCWRK* cam, EntityData1* pltwp)
{
    NJS_VECTOR unitvector = { 0.0f, 0.0f, cam->dist };
    njPushMatrix(_nj_unit_matrix_);
    njRotateY_(_nj_current_matrix_ptr_, cam->_ang.y);
    njRotateX_(_nj_current_matrix_ptr_, cam->_ang.x);
    njCalcPoint(_nj_current_matrix_ptr_, &unitvector, &unitvector, FALSE);
    njPopMatrixEx();

    cam->pos.x = unitvector.x + pltwp->Position.x;
    cam->pos.y = unitvector.y + pltwp->Position.y + height;
    cam->pos.z = unitvector.z + pltwp->Position.z;
}

static bool freecameramode(int screen)
{
    auto cam = &fcwrk[screen];
    auto pltwp = MainCharObj1[screen];
    auto plmwp = MainCharData2[screen];
    auto plpwp = MainCharObj2[screen];
    auto plper = &Controllers[screen];

    if (!pltwp)
    {
        return false;
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

    if (cam->mode & 8 || pltwp->Status & Status_OnPath)
    {
        return false;
    }

    if ((cam->mode & MODE_80000000))
    {
        FreeCam_GetDistances(cam, pltwp);
        vec.x = cam->pos.x - pltwp->Position.x;
        vec.y = cam->pos.y - pltwp->Position.y - height;
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
        cam->pang.y += analog_spd;
    }
    else if ((plper->x2 < 0 || (plper->l - 128) << 8 > 128) && cam->pang.y > -256)
    {
        cam->pang.y -= analog_spd;
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
        cam->pang.x += analog_spd;
    }
    else if (plper->y2 < 0 && cam->pang.x > -256)
    {
        cam->pang.x -= analog_spd;
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
    int colli_cdt = 0;

    if (MSetPositionWIgnoreAttribute(&freecampos, &dyncolpos, nullptr, SurfaceFlag_Water | SurfaceFlag_WaterNoAlpha, 10.0f))
    {
        freecampos = cam->campos;
        dyncolpos = cam->camspd;
        colli_cdt = 1;

        if (MSetPositionWIgnoreAttribute(&freecampos, &dyncolpos, nullptr, SurfaceFlag_Water | SurfaceFlag_WaterNoAlpha, 8.0f))
        {
            colli_cdt = 2;
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
    vec.y = cam->campos.y - pltwp->Position.y - vec.y - height;
    vec.z = cam->campos.z - pltwp->Position.z - vec.z;
    njUnitVector(&vec);

    cam->_ang.y = NJM_RAD_ANG(atan2f(vec.x, vec.z));
    cam->_ang.x = NJM_RAD_ANG(-asinf(vec.y));
    vec.x = cam->campos.x - pltwp->Position.x;
    vec.y = cam->campos.y - pltwp->Position.y - height;
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

    if (colli_cdt)
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
    CameraPos = cam->campos;
    return true;
}

static void AdjustForFreeCamera(CameraInfo* cam, FCWRK* fcp)
{
    auto ptwp = MainCharObj1[CurrentScreen];
    auto ppwp = MainCharObj2[CurrentScreen];

    NJS_POINT3 v1 = { CameraPos.x + ptwp->Position.x, CameraPos.y + ptwp->Position.y, CameraPos.z + ptwp->Position.z };
    float d1 = sqrtf(v1.x * v1.x + v1.y * v1.y + v1.z * v1.z);

    NJS_POINT3 v2 = { cam->location.pos.x + ptwp->Position.x, cam->location.pos.y + ptwp->Position.y, cam->location.pos.z + ptwp->Position.z };
    float d2 = sqrtf(v1.x * v1.x + v1.y * v1.y + v1.z * v1.z);

    float spd = d2 + (d1 - d2) * 0.3f;

    if (spd < 20.0f)
        spd = 20.0f;

    CameraTgt = { ptwp->Position.x, ptwp->Position.y + ppwp->PhysData.CenterHeight, ptwp->Position.z };
    CameraSpeed = spd;
    CamcontSetCameraLOOKAT();
}

void __cdecl Camera_r(ObjectMaster* tp)
{
    if (!CameraEnabled)
    {
        sub_4ECDF0();

        for (CurrentScreen = 0; CurrentScreen < SplitscreenMode; ++CurrentScreen)
        {
            auto cam = &CameraData[CurrentScreen];
            auto fcp = &fcwrk[CurrentScreen];

            CameraGetStopFlag(CurrentScreen);
            CameraSetGlobalLocation();

            if (CurrentScreen < 2)
            {
                CameraManagerRegional(CurrentScreen);
                CameraManager();
            }

            fcp->mode &= ~MODE_2;

            if (fcwrk[CurrentScreen].mode & MODE_1)
            {
                if (freecameramode(CurrentScreen))
                {
                    fcp->mode |= MODE_2;
                }
            }

            if (!(fcp->mode & MODE_2))
            {
                fcwrk[CurrentScreen].mode |= MODE_80000000;
                CameraCameraMode();
                RunCameraTarget();

                if (CameraTimer)
                {
                    CameraCameraAdjust();
                    RunCameraTarget();

                    CameraUpdateLocation_();
                    if (!CameraCollisionDisabled)
                    {
                        if (boolCameraCollision)
                        {
                            RunCameraCollision();
                            RunCameraTarget();
                        }
                    }
                }
            }
            else
            {
                RunCameraTarget();
                AdjustForFreeCamera(cam, fcp);
            }

            CameraUpdateLocation(CurrentScreen);
            sub_4ECC30();
            CameraUpdateFOV();
            RunCameraShake();
        }

        ++CameraTimer;

        CameraData1 = tp->Data1.Entity;
        CameraData1->Position = CameraData[0].location.pos;
        CameraData1->Rotation = CameraData[0].location.ang;
        CameraData1->Scale = { 1.0f, 1.0f, 1.0f };
        CurrentScreen = 0;
        CameraApplyScreenView(0);
    }
}

void InitFreeCamera()
{
    for (auto& i : fcwrk)
    {
        i.timer = 60;
        i.mode |= MODE_80000000 | MODE_8 | MODE_4;
    }
}

void ResetFreeCamera()
{
    for (auto& i : fcwrk)
    {
        i.timer = 60;
        i.mode |= MODE_80000000 | MODE_8;
    }
}

bool GetFreeCamera(int num)
{
    return (fcwrk[num].mode & MODE_1);
}

void SetFreeCamera(int sw, int num)
{
    if (sw)
    {
        if (!(fcwrk[num].mode & MODE_1))
            fcwrk[num].mode |= MODE_80000000 | MODE_1;
    }
    else
    {
        fcwrk[num].mode = fcwrk[num].mode & ~MODE_1 | MODE_80000000;
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
