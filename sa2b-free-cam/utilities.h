#pragma once

#define TARGET_DYNAMIC(name) ((decltype(name##_r)*)name##_t->Target())

static constexpr int max_player = 4;

DataPointer(BOOL, DisableFog, 0x1A55808);
DataPointer(int, ScreenDrawMode, 0x25EFFCC);
DataPointer(int, PauseMode, 0x17472BC);
DataPointer(char, LevelFinished, 0x174B002);

DataArray(const char*, PauseMenuMainTextsJP, 0x174A7D0, 10);
DataArray(const char*, PauseMenuMainTextsUS, 0x174A828, 10);
DataArray(const char*, PauseMenuMainTextsFR, 0x174A880, 10);
DataArray(const char*, PauseMenuMainTextsES, 0x174A8D8, 10);
DataArray(const char*, PauseMenuMainTextsGE, 0x174A930, 10);
DataArray(const char*, PauseMenuMainTextsIT, 0x174A988, 10);
DataArray(void*, PauseMenuMainFontTexs, 0x1A558C4, 6);
DataArray(void*, PauseMenuHelpFontTexs, 0x1A558DC, 4);

VoidFunc(sub_4ECDF0, 0x4ECDF0);
VoidFunc(CameraCameraMode, 0x4EE490);
VoidFunc(CameraUpdateLocation_, 0x4EC840);
VoidFunc(sub_4ECC30, 0x4ECC30);
VoidFunc(CameraManager, 0x4F0730);

FunctionPointer(void*, CreateFontTexture, (const char* text, int language, __int16 a3, __int16 a4), 0x6B7F40);
FunctionPointer(void, ApplyFog, (int id, float a2, float a3, int a4), 0x420510);

static const void* const loc_4F0680 = (void*)0x4F0680;
static inline void CameraManagerRegional(int num)
{
    __asm
    {
        mov edi, [num]
        call loc_4F0680
    }
}

static const void* const loc_43F390 = (void*)0x43F390;
static inline void DrawPauseBackground(int line_count)
{
    __asm
    {
        mov esi, [line_count]
        call loc_43F390
    }
}

static const void* const loc_43F640 = (void*)0x43F640;
static inline void DrawPauseText(void* texture, int i, int ii)
{
    __asm
    {
        push[ii]
        push[i]
        mov esi, [texture]
        call loc_43F640
        add esp, 8
    }
}

static const void* const loc_6B7E50 = (void*)0x6B7E50;
static inline void FreeImage(void* image)
{
    __asm
    {
        mov eax, [image]
        call loc_6B7E50
    }
}

static const void* const loc_43F800 = (void*)0x43F800;
static inline void DrawPauseMassage(int a1, void* image)
{
    __asm
    {
        mov edi, [a1]
        mov esi, [image]
        call loc_43F800
    }
}

static const void* const loc_46C730 = (void*)0x46C730;
static inline int CheckCollisionP(NJS_POINT3* p, float r)
{
    int result;
    __asm
    {
        push[r]
        mov eax, [p]
        call loc_46C730
        mov result, eax
        add esp, 4
    }
    return result;
}

static const void* const loc_41DC80 = (void*)0x41DC80;
static inline void DrawSpriteThing(void* image, float a2, float a3, float a4, float a5, float a6, float a7, float a8, float a9, float a10, int a11)
{
    __asm
    {
        push[a11]
        push[a10]
        push[a9]
        push[a8]
        push[a7]
        push[a6]
        push[a5]
        push[a4]
        push[a3]
        push[a2]
        mov eax, [image]
        call loc_41DC80
        add esp, 40
    }
}

BOOL MSetPositionWIgnoreAttribute(NJS_POINT3* p, NJS_POINT3* v, Angle3* a, int attrmask, float r);
void CameraCameraAdjust();
void CamcontSetCameraLOOKAT();
void njDisableFog();
void njEnableFog();
void FreeFontTexture(void** texture);
