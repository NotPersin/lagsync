#include "c_cs_player.h"
#include "c_model_info_client.h"
#include "c_client_entity_list.h"
#include "../utils/math.h"
#include "c_debug_overlay.h"
#include "c_weapon_system.h"
#include "c_engine_trace.h"
#include "c_global_vars_base.h"
#include "../hacks/c_aimhelper.h"

void IKContext::init(c_studio_hdr* hdr, c_vector3d& angles, c_vector3d& origin, float curtime, int framecount, int boneMask) {
	static const auto ik_init_address = sig("client_panorama.dll", "55 8B EC 83 EC 08 8B 45 08 56 57 8B F9 8D 8F");
	reinterpret_cast<void(__thiscall*)(IKContext*, c_studio_hdr*, c_vector3d&, c_vector3d&, float, int, int)>(ik_init_address)(this, hdr, angles, origin, curtime, framecount, boneMask);
}

void IKContext::update_targets(c_vector3d* pos, quaternion* q, matrix3x4* bone_array, byte* computed) {
	static const auto update_targets_address = sig("client_panorama.dll", "55 8B EC 83 E4 F0 81 EC ? ? ? ? 33 D2");
	reinterpret_cast<void(__thiscall*)(IKContext*, c_vector3d*, quaternion*, matrix3x4*, byte*)>(update_targets_address)(this, pos, q, bone_array, computed);
}

void IKContext::solve_dependencies(c_vector3d* pos, quaternion* q, matrix3x4* bone_array, byte* computed) {
	static const auto solve_dependencies_address = sig("client_panorama.dll", "55 8B EC 83 E4 F0 81 EC ? ? ? ? 8B 81");
	reinterpret_cast<void(__thiscall*)(IKContext*, c_vector3d*, quaternion*, matrix3x4*, byte*)>(solve_dependencies_address)(this, pos, q, bone_array, computed);
}

float c_cs_player::get_spawn_time()
{
	static const auto xref_address = sig("client_panorama.dll", 
		"CC 55 8B EC 83 E4 F8 51 A1 ? ? ? ? 56 8B F1 8B 40 10") + 0x1;

	static const auto spawn_time = *(DWORD*)(xref_address + 0x14);

	return *(float*)((DWORD)this + spawn_time);
}

c_cs_player* c_cs_player::get_local_player()
{
	if (!engine_client()->is_ingame())
		return nullptr;

	const auto local = reinterpret_cast<c_cs_player*>(client_entity_list()->get_client_entity(engine_client()->get_local_player()));
	if (!local)
		return nullptr;

	return local;
}

bool c_cs_player::is_local_player() const
{
	return index() == engine_client()->get_local_player();
}

player_info c_cs_player::get_info() const
{
	player_info info;
	engine_client()->get_player_info(this->index(), &info);
	return info;
}

void c_cs_player::run_pre_think()
{
	static auto fn = reinterpret_cast<bool(__thiscall*)(void*, int)>(sig("client_panorama.dll", "55 8B EC 83 EC 10 53 56 57 8B F9 8B 87"));

	if (fn(this, 0))
		pre_think();
}

void c_cs_player::run_think()
{
	static auto fn = reinterpret_cast<void(__thiscall*)(int)>(sig("client_panorama.dll", "55 8B EC 56 57 8B F9 8B B7 ? ? ? ? 8B C6"));

	if (get_think_tick() != -1 && get_think_tick() > 0
		&& get_think_tick() < time_to_ticks(global_vars_base->curtime))
	{
		get_think_tick() = -1;
		fn(0);
		think();
	}
}

std::optional<int> c_cs_player::get_hitbox_bone_attachment(hitbox id) const
{
	const auto studio_model = model_info_client()->get_studio_model(get_model());

	if (studio_model) {
		const auto hitbox = studio_model->get_hitbox(static_cast<uint32_t>(id), 0);

		if (hitbox)
			return hitbox->bone;
	}

	return std::nullopt;

}

std::optional<c_vector3d> c_cs_player::get_hitbox_position(hitbox id, matrix3x4* bones) const
{

	const auto studio_model = model_info_client()->get_studio_model(get_model());

	if (studio_model) {
		const auto hitbox = studio_model->get_hitbox(static_cast<uint32_t>(id), 0);

		if (hitbox)
		{
			c_vector3d res;
			math::vector_transform((hitbox->bbmin + hitbox->bbmax) * .5f, bones[hitbox->bone], res);
			return res;
		}
	}

	return std::nullopt;
}

c_qangle c_cs_player::get_hitbox_position_(hitbox id, matrix3x4* boneMatrix)
{
	auto studio_model = model_info_client()->get_studio_model(get_model());
	if (studio_model) {
		auto hitbox = studio_model->get_hitbox_set(0)->get_hitbox(static_cast<uint32_t>(id));
		if (hitbox) {
			auto
				min = c_qangle{},
				max = c_qangle{};

			math::vector_transform(hitbox->bbmin, boneMatrix[hitbox->bone], min);
			math::vector_transform(hitbox->bbmax, boneMatrix[hitbox->bone], max);

			return (min + max) / 2.0f;
		}
	}
	return c_qangle{};
}

c_vector3d c_cs_player::get_shoot_position()
{
	c_vector3d pos;
	weapon_shoot_position(pos);
	// correct intrinsic server client desync.
	pos.z -= get_view_offset().z - floor(get_view_offset().z);
	return pos;
}

bool c_cs_player::is_shooting(c_user_cmd* cmd, const float time)
{
	const auto weapon_handle = get_current_weapon_handle();

	if (!weapon_handle)
		return false;

	const auto weapon = reinterpret_cast<c_base_combat_weapon*>(
		client_entity_list()->get_client_entity_from_handle(weapon_handle));

	if (!weapon)
		return false;

	const auto wpn_info = weapon_system->get_weapon_data(weapon->get_item_definition());
	if (!wpn_info)
		return false;

	const auto is_zeus = weapon->get_item_definition() == weapon_taser;
	const auto is_knife = !is_zeus && wpn_info->WeaponType == weapontype_knife;

	if (weapon->is_grenade())
		return !weapon->get_pin_pulled() && weapon->get_throw_time() > 0.f && weapon->get_throw_time() < time;
	else if (is_knife)
		return (cmd->buttons & c_user_cmd::attack || cmd->buttons & c_user_cmd::attack2) && can_shoot(cmd, time);
	else
		return cmd->buttons & c_user_cmd::attack && can_shoot(cmd, time);
}

bool c_cs_player::can_shoot(c_user_cmd* cmd, const float time, const bool manual_shot)
{

	if (cmd->weapon_select || get_flags() & c_base_player::frozen || game_rules->is_freeze_period())
	{
		return false;
	}

	const auto weapon_handle = get_current_weapon_handle();

	if (!weapon_handle)
	{
		return false;
	}

	const auto weapon = reinterpret_cast<c_base_combat_weapon*>(
		client_entity_list()->get_client_entity_from_handle(weapon_handle));

	if (!weapon)
	{
		return false;
	}

	const auto info = weapon_system->get_weapon_data(weapon->get_item_definition());

	if (!info)
	{
		return false;
	}

	const auto is_zeus = weapon->get_item_definition() == weapon_taser;
	const auto is_knife = !is_zeus && info->WeaponType == weapontype_knife;

	if (weapon->get_item_definition() == weapon_c4 || weapon->is_grenade())
	{
		return false;
	}

	if (weapon->get_current_clip() < 1 && !is_knife)
	{
		return false;
	}

	if (weapon->get_in_reload())
	{
		return false;
	}

	if (get_next_attack() > time)
	{
		if (weapon->get_item_definition() != weapon_revolver && info->WeaponType == weapontype_pistol)
			cmd->buttons &= ~c_user_cmd::attack;

		return false;
	}
	
	if (weapon->get_next_primary_attack() > time)
	{
		if (weapon->get_item_definition() != weapon_revolver && info->WeaponType == weapontype_pistol)
			cmd->buttons &= ~c_user_cmd::attack;

		return false;
	}

	return true;
}

bool c_cs_player::is_enemy()
{
	const auto local = get_local_player();

	if (!local)
		return false;

	return local->get_team() != get_team();
}

bool c_cs_player::is_visible(matrix3x4* bones) const
{
	const auto local = get_local_player();

	if (!local || !local->is_alive())
		return false;

	auto origin = local->get_shoot_position();
	auto target = get_hitbox_position(hitbox::head, bones);

	if (!target.has_value())
		return false;

	if (c_engine_trace::line_through_smoke(origin, target.value()))
		return false;

	c_trace_world_filter filter{};
	game_trace trace{};
	ray r{};
	r.init(origin, target.value());
	engine_trace()->trace_ray(r, mask_shot_player, reinterpret_cast<c_trace_filter*>(&filter), &trace);
	return trace.fraction == 1.f;

}

void c_cs_player::draw_hitboxes(matrix3x4* matrix, float time, float r, float g, float b, float a) const
{
	const auto model = get_model();

	if (!model)
		return;

	const auto hdr = model_info_client()->get_studio_model(model);

	if (!hdr)
		return;

	const auto set = hdr->get_hitbox_set(0);

	for (auto i = 0; i < set->numhitboxes; i++) {
		const auto hitbox = set->get_hitbox(i);

		if (!hitbox)
			continue;

		if (hitbox->radius == -1.0f)
		{
			const auto position = math::matrix_position(matrix[hitbox->bone]);
			const auto roation = math::angle_matrix(hitbox->rotation);
			auto transform = math::multiply_matrix(matrix[hitbox->bone], roation);
			const auto angles = math::matrix_angles(transform);

			debug_overlay()->AddBoxOverlay(position, hitbox->bbmin, hitbox->bbmax, angles, r, g, b, 160, time);

			debug_overlay()->AddBoxOverlay(position, hitbox->bbmin, hitbox->bbmax, angles, r, g, b, 160, time);

		}
		else
		{
			c_vector3d min, max;
			math::vector_transform(hitbox->bbmin, matrix[hitbox->bone], min);
			math::vector_transform(hitbox->bbmax, matrix[hitbox->bone], max);

			debug_overlay()->add_capsule_overlay(min, max, hitbox->radius, r, g, b, 160, time);
		}
	}
}
