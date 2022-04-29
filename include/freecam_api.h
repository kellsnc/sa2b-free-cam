#ifndef _FREECAMAPI_H
#define _FREECAMAPI_H

#ifdef __cplusplus
#include <cstdint>
#else
#include <stdint.h>
#include <stdbool.h>
#endif

#ifdef MULTI_API
#define API __declspec(dllexport)
#else
#define API __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C"
{
#endif

	// Set if free camera is authorized for player num
	API void freecamera_authorize(int32_t num, bool authorize);

	// Enable or disable free camera mode for player num
	API void freecamera_set(int32_t num, bool enabled);

	// Get if free camera is enabled for player num
	API bool freecamera_get(int32_t num);

	// Restart free camera system
	API void freecamera_reset();

#ifdef __cplusplus
}
#endif

#undef API

#endif