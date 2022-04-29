#include "pch.h"
#include "camera.h"
#include "..\include\freecam_api.h"

void freecamera_authorize(int32_t num, bool authorize)
{
	SetFreeCameraMode(authorize, num);
}

void freecamera_set(int32_t num, bool enabled)
{
	SetFreeCamera(enabled, num);
}

bool freecamera_get(int32_t num)
{
	return GetFreeCamera(num);
}

void freecamera_reset()
{
	ResetFreeCamera();
}