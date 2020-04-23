#pragma once

#include "c_base_player.h"
#include "c_engine_client.h"
#include <optional>
#include "c_csgo_player_animstate.h"
#include "c_user_cmd.h"

class c_studio_hdr;

class IKContext
{
public:
	void init(c_studio_hdr* hdr, c_vector3d& angles, c_vector3d& origin, float curtime, int framecount, int boneMask);
	void update_targets(c_vector3d* pos, quaternion* q, matrix3x4* bone_array, byte* computed);
	void solve_dependencies(c_vector3d* pos, quaternion* q, matrix3x4* bone_array, byte* computed);

	void clear_targets()
	{
		/*
		08.10.2019
		if ( (unsigned __int8)sub_101C9820(v53) || (*(_DWORD *)(v7 + 236) >> 3) & 1 )
		{
			  v56 = 0;
			  if ( *(_DWORD *)(v105 + 4080) > 0 )
			  {
				   v57 = (_DWORD *)(v105 + 208);
				   do
				   {
					   *v57 = -9999;
						v57 += 85;
						++v56;
					}
					while ( v56 < *(_DWORD *)(v105 + 4080) );
				  }
				}
		}
		*/
		auto v56 = 0;
		if (*(int*)((DWORD)this + 4080) > 0)
		{
			auto v57 = (int*)((DWORD)this + 208);
			do
			{
				*v57 = -9999;
				v57 += 85;
				++v56;
			} while (v56 < *(int*)((DWORD)this + 4080));
		}
	}
};


class c_cs_player : public c_base_player
{
	vfunc(284, weapon_shoot_position(c_vector3d& pos), void(__thiscall*)(c_cs_player*, c_vector3d&))(pos)
public:
	enum class hitbox
	{
		head,
		neck,
		pelvis,
		body,
		thorax,
		chest,
		upper_chest,
		left_thigh,
		right_thigh,
		left_calf,
		right_calf,
		left_foot,
		right_foot,
		left_hand,
		right_hand,
		left_upper_arm,
		left_forearm,
		right_upper_arm,
		right_forearm,
		max
	};

	inline static const hitbox hitboxes[] = {
		hitbox::head,
		hitbox::neck,
		hitbox::pelvis,
		hitbox::body,
		hitbox::thorax,
		hitbox::chest,
		hitbox::upper_chest,
		hitbox::right_thigh,
		hitbox::left_thigh,
		hitbox::right_calf,
		hitbox::left_calf,
		hitbox::right_foot,
		hitbox::left_foot,
		hitbox::right_hand,
		hitbox::left_hand,
		hitbox::right_upper_arm,
		hitbox::right_forearm,
		hitbox::left_upper_arm,
		hitbox::left_forearm
	};

	inline static const hitbox hitboxes_aiming[] = 
	{
		hitbox::head,
		hitbox::neck,
		hitbox::pelvis,
		hitbox::body,
		hitbox::thorax,
		hitbox::chest,
		hitbox::upper_chest,
		hitbox::right_thigh,
		hitbox::left_thigh,
		hitbox::right_calf,
		hitbox::left_calf
	};

	inline static const hitbox hitboxes_baim[] = {
		hitbox::pelvis,
		hitbox::body,
		hitbox::right_thigh,
		hitbox::left_thigh,
		hitbox::right_calf,
		hitbox::left_calf,
	};

	enum obeserver_modes {
		OBS_MODE_NONE = 0,
		OBS_MODE_DEATHCAM = 1,
		OBS_MODE_FREEZECAM = 2,
		OBS_MODE_FIXED = 3,
		OBS_MODE_IN_EYE = 4,
		OBS_MODE_CHASE = 5,
		OBS_MODE_ROAMING = 6
	};

	static uint32_t* get_vtable()
	{
		static const auto table = reinterpret_cast<uint32_t>(sig("client_panorama.dll",
			"55 8B EC 83 E4 F8 83 EC 18 56 57 8B F9 89 7C 24 0C")) + 0x47;
		return *reinterpret_cast<uint32_t**>(table);
	}

	vfunc(191, update_ik_locks(float curtime), void(__thiscall*)(void*, float))(curtime)
	vfunc(192, calculate_ik_locks(float curtime), void(__thiscall*)(void*, float))(curtime)
	vfunc(189, build_transformations(c_studio_hdr* hdr, c_vector3d* vec, quaternion* q, matrix3x4& transform, const int mask, uint8_t* computed),
	void(__thiscall*)(c_cs_player*, c_studio_hdr*, c_vector3d*, quaternion*, matrix3x4 const&, int, uint8_t*))(hdr, vec, q, transform, mask, computed)
	vfunc(205, standard_blending_rules(c_studio_hdr* hdr, c_vector3d* vec, quaternion* q, const float time, const int mask),
		void(__thiscall*)(c_cs_player*, c_studio_hdr*, c_vector3d*, quaternion*, float, int))(hdr, vec, q, time, mask)

	static c_cs_player* get_local_player();

	netvar(m_flHealthShotBoostExpirationTime(), float, "CCSPlayer", "m_flHealthShotBoostExpirationTime")
	netvar(get_view_punch_angle(), c_vector3d, "CCSPlayer", "m_viewPunchAngle")
	netvar(get_punch_angle(), c_vector3d, "CCSPlayer", "m_aimPunchAngle")
	netvar(get_punch_angle_vel(), c_vector3d, "CCSPlayer", "m_aimPunchAngleVel")
	netvar(get_view_offset(), c_vector3d, "CCSPlayer", "m_vecViewOffset_0")
	netvar(get_health(), int32_t, "CCSPlayer", "m_iHealth")
	netvar(get_armor(), int32_t, "CCSPlayer", "m_ArmorValue")
	netvar(get_defusing(), bool, "CCSPlayer", "m_bIsDefusing")
	netvar(get_eye_angles(), c_qangle, "CCSPlayer", "m_angEyeAngles")
	netvar(get_lby(), float, "CCSPlayer", "m_flLowerBodyYawTarget")
	netvar(is_scoped(), bool, "CCSPlayer", "m_bIsScoped")
	netvar(has_helmet(), bool, "CCSPlayer", "m_bHasHelmet")
	netvar(has_heavy_armor(), bool, "CCSPlayer", "m_bHasHeavyArmor")
	netvar(get_observer_mode(), int32_t, "CCSPlayer", "m_iObserverMode")
	netvar(get_flash_alpha(), float, "CCSPlayer", "m_flFlashMaxAlpha")
	netvar(get_flash_duration(), float, "CCSPlayer", "m_flFlashDuration")
	netvar(get_gun_game_immunity(), bool, "CCSPlayer", "m_bGunGameImmunity")
	netvar(get_shots_fired(), int, "CCSPlayer", "m_iShotsFired")
	netprop(get_lby_prop(), "CCSPlayer", "m_flLowerBodyYawTarget")
	netprop(get_InBombZone(), "CCSPlayer", "m_bInBombZone")
	datamap(get_abs_rotation(), c_qangle, "m_angAbsRotation")

	offset(get_think_tick(), int32_t, 0x40)
	// find next 2 in cs player setup bones
	offset(get_occlusion_flags(), uint32_t, 0xA28)
	offset(get_occlusion_framecount(), uint32_t, 0xA30)
	// find next 5 in base entity setup bones
	offset(get_bone_array_for_write(), matrix3x4*, 0x26A8)
	offset(get_readable_bones(), uint32_t, 0x26AC)
	offset(get_writable_bones(), uint32_t, 0x26B0)
	offset(get_most_recent_model_bone_counter(), uint32_t, 0x2690)
	offset(get_last_bone_setup_time(), float, 0x2924)
	// those are easy to spot in cs player spawn (call to reset and set to curtime)
	offset(get_anim_state(), c_csgo_player_anim_state*, 0x3900)
	// next two are easy to find in almost any subroutine of setup bones
	offset(get_ik_context(), IKContext*, 9836 + 0x4) // v155 = *(_DWORD*)(v67 + 9836);
	offset(get_model_hdr(), c_studio_hdr*, 10568 + 0x4) // v87 = *(_DWORD **)(v67 + 10568);
	offset(get_bone_cache(), matrix3x4**, 0x2910)
	offset(get_bone_cache_count(), int, 0x2910 + 0xC)

	vfunc(138, think(), void(__thiscall*)(void*))()
	vfunc(223, update_clientside_anim(), void(__thiscall*)(c_cs_player*))()
	vfunc(315, pre_think(), void(__thiscall*)(void*))()
	vfunc(316, post_think(), void(__thiscall*)(void*))()

	bool is_local_player() const;
	player_info get_info() const;

	void run_pre_think();
	void run_think();

	std::optional<int> get_hitbox_bone_attachment(hitbox id) const;
	std::optional<c_vector3d> get_hitbox_position(hitbox id, matrix3x4* bones) const;
	c_qangle get_hitbox_position_(hitbox id, matrix3x4* boneMatrix);
	c_vector3d get_shoot_position();
	bool is_shooting(c_user_cmd* cmd, float time);
	bool can_shoot(c_user_cmd* cmd, float time, bool manual_shot = false);

	bool is_enemy();
	bool is_visible(matrix3x4* bones) const;

	void draw_hitboxes(matrix3x4* matrix, float time, float r, float g, float b, float a) const;
	float get_spawn_time();
};
