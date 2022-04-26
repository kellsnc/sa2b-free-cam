#include "pch.h"
#include <math.h>
#include "utilities.h"

BOOL MSetPositionWIgnoreAttribute(NJS_POINT3* p, NJS_POINT3* v, Angle3* a, int attrmask, float r)
{
    return MSetPosition(p, v, a, r);
}

void CameraCameraAdjust()
{
    auto param = &CameraData[CurrentScreen].cameraAdjusts[CameraData[CurrentScreen].currentCameraSlot];

    if (param->pObjCameraMode)
    {
        param->pObjCameraMode(&CameraData[CurrentScreen], param);
        ++param->ulTimer;
    }
}

void CamcontSetCameraLOOKAT()
{
    double x = (double)CameraPos.x - (double)CameraTgt.x;
    double z = (double)CameraPos.z - (double)CameraTgt.z;
    CameraAng.y = NJM_RAD_ANG(atan2(x, z));
    CameraAng.x = NJM_RAD_ANG(atan2((double)CameraTgt.y - (double)CameraPos.y, sqrt(x *x + z * z)));
}
