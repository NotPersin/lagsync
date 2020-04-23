#pragma once

#include "../sdk/c_vector3d.h"
#include "../sdk/c_cs_player.h"
#include "c_animation_system.h"
#include <optional>
#include "../utils/c_config.h"
#include "../menu/c_menu.h"

class c_aimhelper
{
	static constexpr auto total_seeds = 255;
	static constexpr auto pi_2 = 6.283186f;

public:
	struct aim_info
	{
		aim_info(const c_vector3d position, const float damage, c_animation_system::animation* animation, const bool alt_attack,
			const c_vector3d center, const float radius, const float rs, const c_cs_player::hitbox hitbox, const int32_t hitgroup)
			: position(position), damage(damage), animation(animation), alt_attack(alt_attack),
			center(center), radius(radius), rs(rs), hitbox(hitbox), hitgroup(hitgroup) { }

		c_vector3d position{};
		float damage{};
		c_animation_system::animation* animation{};
		bool alt_attack{};
		c_vector3d center{};
		float radius{}, rs{};
		c_cs_player::hitbox hitbox{};
		int32_t hitgroup{};
	};

	// yes we're only multipointing head and pelvis right now.
	static __forceinline bool is_multipoint_hitbox(const c_cs_player::hitbox box)
	{
		return box == c_cs_player::hitbox::head || box == c_cs_player::hitbox::pelvis;
	}

	static std::optional<Menu22::ragebot::weapon_config> get_weapon_conf();

	static std::optional<std::tuple<c_vector3d, float, c_animation_system::animation*, float>> get_legit_target(float target_fov, float range, std::vector<c_cs_player::hitbox> override_hitbox);

	static std::optional<std::tuple<c_vector3d, float, c_animation_system::animation*>> get_legit_target(float target_fov, float range = 1.f,
		std::optional<c_cs_player::hitbox> override_hitbox = std::nullopt, bool visible = false);
	static std::optional<std::tuple<c_vector3d, float, c_animation_system::animation*, float>> get_legit_target(float target_fov, float range, std::vector<c_cs_player::hitbox> override_hitbox, bool visible);
	static std::vector<aim_info> select_multipoint(c_animation_system::animation* animation, c_cs_player::hitbox box,
		int32_t group, std::optional < Menu22::ragebot::weapon_config> cfg);
	static std::vector<c_aimhelper::aim_info> select_multipoint(c_animation_system::animation* animation, c_cs_player::hitbox box, int32_t group, float scaled_box);
	static void optimize_multipoint(aim_info& info);

	static bool can_hit(c_cs_player* local, c_animation_system::animation* animation, c_vector3d position, float chance, c_cs_player::hitbox box);
	static bool can_hit_hitbox(c_vector3d start, c_vector3d end, c_animation_system::animation* animation, studiohdr* hdr, c_cs_player::hitbox box);
	static bool can_hit_hitbox(int index, c_vector3d start, c_vector3d end, matrix3x4* bones, c_cs_player::hitbox box);

	static void fix_movement(c_user_cmd* cmd, c_qangle& wish_angle);

	static void build_seed_table();
}; 