#pragma once

namespace config
{
	extern bool menu_enabled;
	extern float min_dist;
	extern float max_dist;
	extern float height;
	extern int analog_spd;
	extern int analog_max;

	void read(const char* path);
}