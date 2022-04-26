#pragma once

void __cdecl Camera_r(ObjectMaster* tp);

void InitFreeCamera();                   // Init free camera flags (on stage init in sadx)
void ResetFreeCamera();                  // Init free camera flags (on adv field in sadx)
bool GetFreeCamera(int num);             // Get if free camera is enabled for player num
void SetFreeCamera(int sw, int num);     // Set free camera enabled for player num
void SetFreeCameraMode(int sw, int num); // Set if free camera is authorized or not for player num
