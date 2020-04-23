#pragma once

#include "includes.h"

enum dlight_flags {
	dlight_no_world_illumination = 0x1,
	dlight_no_model_illumination = 0x2,
	dlight_add_displacement_alpha = 0x4,
	dlight_subtract_displacement_alpha = 0x8,
	dlight_displacement_mask = (dlight_add_displacement_alpha | dlight_subtract_displacement_alpha),
};

struct dlight_t {
	int flags;
	c_vector3d origin;
	float radius;
	c_color color;
	float die_time;
	float decay;
	float min_light;
	int	key;
	int	style;
	c_vector3d direction;
	float inner_angle;
	float outer_angle;
};

class c_dlight
{
public:
	vfunc(4, alloc_dlight(int key), dlight_t*(__thiscall*)(void*, int))(key)
	vfunc(5, alloc_elight(int key), dlight_t*(__thiscall*)(void*, int))(key)
	vfunc(8, alloc_elight_by_key(int key), dlight_t(__thiscall*)(c_dlight*, int))(key)

};

interface_var(c_dlight, dlight, "engine.dll", "VEngineEffects001")
