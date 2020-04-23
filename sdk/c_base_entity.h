#pragma once

#include "macros.h"
#include "c_client_entity.h"

class c_base_entity : public c_client_entity
{
public:
	static constexpr auto efl_dirty_absvelocity = 0x1000;
	static constexpr auto ef_bone_merge = 1;
	static constexpr auto ef_nointerp = 8;
	static constexpr auto ef_itemblink = 0x100;

	static constexpr auto damage_no = 0;
	static constexpr auto damage_yes = 2;

	enum move_type
	{
		movetype_none = 0,
		movetype_isometric,
		movetype_walk,
		movetype_step,
		movetype_fly,
		movetype_flygravity,
		movetype_vphysics,
		movetype_push,
		movetype_noclip,
		movetype_ladder,
		movetype_observer,
		movetype_custom,
		movetype_last = movetype_custom,
		movetype_max_bits = 4
	};

	enum invalidate_physics_bits
	{
		position_changed = 0x1,
		angles_changed = 0x2,
		velocity_changed = 0x4,
		animation_changed = 0x8,
		bounds_changed = 0x10,
		sequence_changed = 0x20
	};

	void set_abs_origin(c_vector3d origin) {
		static auto set_abs_origin_fn = reinterpret_cast<void(__thiscall*)(c_base_entity*, const c_vector3d&)>(
			sig("client_panorama.dll", "55 8B EC 83 E4 F8 51 53 56 57 8B F1 E8"));
		set_abs_origin_fn(this, origin);
	}

	void set_abs_angles(c_qangle angle) {
		static auto set_abs_angles_fn = reinterpret_cast<void(__thiscall*)(c_base_entity*, const c_qangle&)>(
			sig("client_panorama.dll", "55 8B EC 83 E4 F8 83 EC 64 53 56 57 8B F1 E8"));
		set_abs_angles_fn(this, angle);
	}

	void invalidate_physics_recursive(int32_t flags) {
		static const auto invalidate_physics_recursive = reinterpret_cast<void(__thiscall*)(c_base_entity*, int32_t)>(
			sig("client_panorama.dll", "55 8B EC 83 E4 F8 83 EC 0C 53 8B 5D 08 8B C3 56"));
		invalidate_physics_recursive(this, flags);
	}

	void force_bone_rebuild()
	{
		/*
		string: "current: %d" = C_CSPlayer::ClientThink
		scroll down to a call = C_CSPlayer::ReevauluateAnimLOD

		0xA30 = last_occlusiontime
		0xA28 = occlusion_flags
		*/

		writable_bones() = 0; // writable bones
		static auto invalidate_bone_cache = sig("client_panorama.dll", "80 3D ? ? ? ? ? 74 16 A1 ? ? ? ? 48 C7 81");

		*(int*)(uintptr_t(this) + 0x2924) = 0xFF7FFFFF;
		*(int*)(uintptr_t(this) + 0x2690) = int(**(unsigned long**)(invalidate_bone_cache + 10) - 1);

		//*(std::uint32_t*)(uintptr_t(this) + 0x26B0) = 0;
		//*(std::uint32_t*)(uintptr_t(this) + 0x26AC) = 0;
	}

	bool is_breakable()
	{
		static auto is_breakable_fn = reinterpret_cast<bool(__thiscall*)(c_base_entity*)>(
			sig("client_panorama.dll", "55 8B EC 51 56 8B F1 85 F6 74 68"));

		const auto result = is_breakable_fn(this);

		if (!result &&
			(get_class_id() == cbasedoor ||
				get_class_id() == cbreakablesurface ||
				(get_class_id() == cbaseentity && get_collideable()->get_solid() == solid_bsp)))
			return true;

		return result;
	}

	bool is_dormant()
	{
		const auto that = reinterpret_cast<c_base_entity*>(this);
		return is_dormant_virt() || that->get_old_simtime() > that->get_simtime() || fabsf(that->get_old_simtime() - that->get_simtime()) > .5f;
	}

	c_vector3d get_bone_position(int i)
	{
		matrix3x4 matrix[128];
		if (this->setup_bones(matrix, 128, 0x00000100, GetTickCount64()))
		{
			return c_vector3d(matrix[i][0][3], matrix[i][1][3], matrix[i][2][3]);
		}
		return c_vector3d(0, 0, 0);
	}

	c_vector3d get_bone_position(int Bone, matrix3x4 MatrixArray[128])
	{
		c_vector3d pos = c_vector3d();

		matrix3x4& HitboxMatrix = MatrixArray[Bone];

		pos = c_vector3d(HitboxMatrix[0][3], HitboxMatrix[1][3], HitboxMatrix[2][3]);

		return pos;
	}

	c_base_handle* m_hMyWeapons()
	{
		return (c_base_handle*)((uintptr_t)this + 0x2DF8);

	}

	vfunc(10, get_abs_origin(), const c_vector3d& (__thiscall*)(c_base_entity*))()
	vfunc(11, get_abs_angles(), const c_qangle& (__thiscall*)(c_base_entity*))()
	vfunc(17, get_pred_desc_map(), datamap_t* (__thiscall*)(c_base_entity*))()
	vfunc(170, should_collide(const int collision_group, const int contents_mask), bool(__thiscall*)(void*, int, int))(collision_group, contents_mask)
	vfunc(165, is_weapon(), bool(__thiscall*)(c_base_entity*))()
	vfunc(75, set_model_index(int _model), void(__thiscall*)(c_base_entity*, int))(_model)
	c_qangle& abs_angles() {
		using original_fn = c_qangle & (__thiscall*)(void*);
		return (*(original_fn * *)this)[11](this);
	}
	netvar(cordinate_frame(), matrix3x4, "CBaseEntity", "m_CollisionGroup", -0x30)
	netvar(writable_bones(), int, "CBaseAnimating", "m_nForceBone", -0x20)
	netvar(get_team(), int32_t, "CBaseEntity", "m_iTeamNum")
	netvar(get_origin(), c_vector3d, "CBaseEntity", "m_vecOrigin")
	netvar_offset(get_old_origin(), c_vector3d, "CBaseEntity", "m_vecOrigin", 0x274)
	netvar(get_simtime(), float, "CBaseEntity", "m_flSimulationTime")
	netvar_offset(get_old_simtime(), float, "CBaseEntity", "m_flSimulationTime", 4)
	netvar(get_rotation(), c_qangle, "CBaseEntity", "m_angRotation")
	netvar(get_mins(), c_vector3d, "CBaseEntity", "m_vecMins")
	netvar(get_maxs(), c_vector3d, "CBaseEntity", "m_vecMaxs")
	netvar(is_spotted(), bool, "CBaseEntity", "m_bSpotted")
	netvar(get_lifestate(), int32_t, "CBasePlayer", "m_lifeState")
	netvar(m_hMyWearables(), uintptr_t, "DT_BaseCombatCharacter", "m_hMyWearables")

	datamap(get_effects(), int, "m_fEffects")
	datamap(get_eflags(), int, "m_iEFlags")
	datamap(get_duck_amount(), float, "m_flDuckAmount")
	datamap(get_abs_velocity(), c_vector3d, "m_vecAbsVelocity")
	datamap(get_move_type(), int, "m_MoveType")
	datamap(get_owner_entity(), c_base_handle, "m_hOwnerEntity")
	datamap(get_move_parent(), c_base_handle, "m_hNetworkMoveParent")

	offset(get_take_damage(), int32_t, 0x27C)
	offset(get_abs_entity_origin(), c_vector3d, 0xA0)

	bool is_alive()
	{
		return this->get_lifestate() == 0;
	}
};