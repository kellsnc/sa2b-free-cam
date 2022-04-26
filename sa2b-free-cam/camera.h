#pragma once

void __cdecl Camera_r(ObjectMaster* tp);

void InitFreeCamera();
void ResetFreeCamera();
bool GetFreeCamera(int num);
void SetFreeCamera(int sw, int num);
void SetFreeCameraMode(int sw, int num);
