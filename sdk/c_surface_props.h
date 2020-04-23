#pragma once

#include <cstdint>
#include "macros.h"

#define surf_light 0x0001
#define surf_sky2d 0x0002 
#define surf_sky 0x0004
#define surf_warp 0x0008
#define surf_trans 0x0010
#define surf_noportal 0x0020
#define surf_trigger 0x0040
#define surf_nodraw 0x0080
#define surf_hint 0x0100
#define surf_skip 0x0200
#define surf_nolight 0x0400
#define surf_bumplight 0x0800
#define surf_noshadows 0x1000
#define surf_nodecals 0x2000
#define surf_nopaint surf_nodecals 
#define surf_nochop 0x4000
#define surf_hitbox 0x8000

#define char_tex_concrete 'C'
#define char_tex_metal 'M'
#define char_tex_dirt 'D'
#define char_tex_vent 'V'
#define char_tex_grate 'G'
#define char_tex_tile 'T'
#define char_tex_slosh 'S'
#define char_tex_wood 'W'
#define char_tex_computer 'P'
#define char_tex_glass 'Y'
#define char_tex_flesh 'F'
#define char_tex_snow 'N'
#define char_tex_plastic 'L'
#define char_tex_cardboard 'U'

struct surfacephysicsparams
{
	float friction;
	float elasticity;
	float density;
	float thickness;
	float dampening;
};

struct surfaceaudioparams
{
	float reflectivity;
	float hardness_factor;
	float roughness_factor;
	float rough_threshold;
	float hard_threshold;
	float hard_velocity_threshold;
	float high_pitch_occlusion;
	float mid_pitch_occlusion;
	float low_pitch_occlusion;
};

struct surfacesoundnames
{
	unsigned short walk_step_left;
	unsigned short walk_step_right;
	unsigned short run_step_left;
	unsigned short run_step_right;
	unsigned short impact_soft;
	unsigned short impact_hard;
	unsigned short scrape_smooth;
	unsigned short scrape_rough;
	unsigned short bullet_impact;
	unsigned short rolling;
	unsigned short break_sound;
	unsigned short strain_sound;
};

struct surfacegameprops_t
{
	float maxspeedfactor;
	float jumpfactor;
	float penetration_mod;
	float damage_mod;
	uint16_t material;
	uint8_t climbable;
};

struct surfacedata_t
{
	surfacephysicsparams physics;
	surfaceaudioparams audio;
	surfacesoundnames sounds;
	surfacegameprops_t game;
	char pad[48];
};

class c_surface_props
{
public:
	virtual ~c_surface_props() = default;
	virtual int parse_surface_data(const char* filename, const char* textfile) = 0;
	virtual int surface_prop_count() const = 0;
	virtual int get_surface_index(const char* surface_prop_name) const = 0;
	virtual void get_physics_properties(int surface_data_index, float* density, float* thickness, float* friction, float* elasticity) const = 0;
	virtual surfacedata_t* get_surface_data(int surface_data_index) = 0;
	virtual const char* get_string(unsigned short string_table_index) const = 0;
	virtual const char* get_prop_name(int surface_data_index) const = 0;
	virtual void set_world_material_index_table(int* map_array, int map_size) = 0;
	virtual void get_physics_parameters(int surface_data_index, surfacephysicsparams* params_out) const = 0;
};

interface_var(c_surface_props, surface_props, "vphysics.dll", "VPhysicsSurfaceProps")
