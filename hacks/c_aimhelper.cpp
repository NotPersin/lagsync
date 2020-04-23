#include "c_aimhelper.h"
#include "../sdk/c_cs_player.h"
#include "../sdk/c_client_entity_list.h"
#include "c_animation_system.h"
#include "../utils/math.h"
#include "../sdk/c_weapon_system.h"
#include "c_trace_system.h"
#include "../menu/c_menu.h"
static std::vector<std::tuple<float, float, float>> precomputed_seeds = {};

std::optional<Menu22::ragebot::weapon_config> c_aimhelper::get_weapon_conf()
{

	if (!engine_client()->is_ingame() || !engine_client()->is_connected() || !c_cs_player::get_local_player() || !c_cs_player::get_local_player()->is_alive())
		return std::nullopt;

	const auto local = c_cs_player::get_local_player();

	if (!local)
		return std::nullopt;

	const auto weapon = reinterpret_cast<c_base_combat_weapon*>(
		client_entity_list()->get_client_entity_from_handle(local->get_current_weapon_handle()));
	if (!weapon)
		return std::nullopt;

	const auto info = weapon_system->get_weapon_data(weapon->get_item_definition());
	if (!info)
		return std::nullopt;

	if (info->get_weapon_id() == weapon_g3sg1 || info->get_weapon_id() == weapon_scar20) {
		if (!menu->open)config->Ragebot.weaponconfig = 1;
		return config->Ragebot._auto;
	}
	else if (info->get_weapon_id() == weapon_ssg08) {
		if (!menu->open)config->Ragebot.weaponconfig = 2;
		return config->Ragebot.scout;
	}
	else if (info->get_weapon_id() == weapon_awp) {
		if (!menu->open)config->Ragebot.weaponconfig = 3;
		return config->Ragebot.awp;
	}
	else if (info->get_weapon_id() == weapon_deagle || info->get_weapon_id() == weapon_revolver) {
		if (!menu->open)config->Ragebot.weaponconfig = 4;
		return config->Ragebot.heavy;
	}
	else if (info->WeaponType == weapontype_pistol) {
		if (!menu->open)config->Ragebot.weaponconfig = 0;
		return config->Ragebot.pistol;
	}
	else if (info->get_weapon_id() == weapon_taser) {
		if (!menu->open)config->Ragebot.weaponconfig = 6;
		return config->Ragebot.taser;
	}
	else {
		if (!menu->open)config->Ragebot.weaponconfig = 5;
		return config->Ragebot.other;
	}
}

std::optional<std::tuple<c_vector3d, float, c_animation_system::animation*, float>> c_aimhelper::get_legit_target(float target_fov, float range, std::vector<c_cs_player::hitbox> override_hitbox, bool visible)
{
	auto local = c_cs_player::get_local_player();

	if (!local)
		return std::nullopt;

	auto current_pos = local->get_shoot_position();
	auto current_angle = engine_client()->get_view_angles();
	const auto recoil_scale = cvar()->find_var(_("weapon_recoil_scale"))->get_float();
	auto current_angle_with_recoil = current_angle + (local->get_punch_angle() * recoil_scale);
	math::normalize(current_angle_with_recoil);
	std::tuple<c_vector3d, c_animation_system::animation*, float> best_match = { c_vector3d(), nullptr, FLT_MAX };
	std::tuple<c_vector3d, c_animation_system::animation*, float, float> best_match1 = { c_vector3d(), nullptr, FLT_MAX, FLT_MAX };

	client_entity_list()->for_each_player([&](c_cs_player* player) -> void
	{
		if (!player->is_alive() || player->is_dormant() /*|| (visible && player->is_dormant())*/ || !player->is_enemy())
			return;//look at the visible shit

		//if (!visible && player->get_simtime() + 7.f < global_vars_base->curtime)
			//return;//look at the visible shit

		auto view = player->get_shoot_position();
		const auto estimate_fov = math::get_fov(current_angle_with_recoil, current_pos, view);

		if (estimate_fov > 2.f * target_fov)//cool check but rather useless
			return;

		const auto current = animation_system->get_lastest_animation_unsafe(player);
		if (!current)return;//dunno why that would happen
		/*if (!current.has_value() ||
			(visible && !player->is_visible(const_cast<matrix3x4*>(current.value()->bones))))
			return;*/

		const auto animations = animation_system->get_valid_animations(player, 1.f); //range > 0.f ?  : std::vector<c_animation_system::animation*> { current.value() };

		/*find closest hitbox/record*/
		for (auto& animation : animations) {
			for (const auto& hitbox : override_hitbox) {
				auto box = player->get_hitbox_position(hitbox, const_cast<matrix3x4*>(animation->bones));
				if (!box.has_value())
					continue;

				const auto fov = math::get_fov(current_angle_with_recoil, current_pos, box.value());
				if (fov <= target_fov)
					if (fov < std::get<2>(best_match1))
						best_match1 = std::make_tuple(box.value(), animation, fov, 0);
			}
		}
		/*damage check*/
		if (std::get<2>(best_match1) != FLT_MAX) {//we have something useful
			if (std::get<1>(best_match1)->index == player->index()) {//current scan has the closest
				const auto wall = trace_system->wall_penetration(local->get_shoot_position(),
					std::get<0>(best_match1), std::get<1>(best_match1));

				if (wall.has_value()) {
					if (wall.value().damage > std::get<3>(best_match1)) {
						std::get<3>(best_match1) = wall.value().damage;
						best_match = std::make_tuple(std::get<0>(best_match1), std::get<1>(best_match1), std::get<2>(best_match1));
					}
				}
			}
		}
	});

	if (std::get<2>(best_match) == FLT_MAX || !std::get<1>(best_match) || !std::get<1>(best_match)->player)
		return std::nullopt;
	std::optional<std::tuple<c_vector3d, float, c_animation_system::animation*, float>> target = std::nullopt;
	auto t = std::make_tuple(std::get<0>(best_match), std::get<1>(best_match)->sim_time, std::get<1>(best_match), std::get<2>(best_match));
	target = t;
	return std::make_tuple(std::get<0>(best_match), std::get<1>(best_match)->sim_time, std::get<1>(best_match), std::get<2>(best_match));
}

std::optional<std::tuple<c_vector3d, float, c_animation_system::animation*, float>> c_aimhelper::get_legit_target(float target_fov, float range, std::vector<c_cs_player::hitbox> override_hitbox)
{
	auto local = c_cs_player::get_local_player();

	if (!local)
		return std::nullopt;

	auto current_pos = local->get_shoot_position();
	auto current_angle = engine_client()->get_view_angles();
	const auto recoil_scale = cvar()->find_var(_("weapon_recoil_scale"))->get_float();
	auto current_angle_with_recoil = current_angle + (local->get_punch_angle() * recoil_scale);
	math::normalize(current_angle_with_recoil);
	std::tuple<c_vector3d, c_animation_system::animation*, float> best_match = { c_vector3d(), nullptr, FLT_MAX };
	std::tuple<c_vector3d, c_animation_system::animation*, float, float> best_match1 = { c_vector3d(), nullptr, FLT_MAX, FLT_MAX };

	client_entity_list()->for_each_player([&](c_cs_player* player) -> void
	{
		if (!player->is_alive() || player->is_dormant() /*|| (visible && player->is_dormant())*/ || !player->is_enemy())
			return;//look at the visible shit

		//if (!visible && player->get_simtime() + 7.f < global_vars_base->curtime)
			//return;//look at the visible shit

		auto view = player->get_shoot_position();
		const auto estimate_fov = math::get_fov(current_angle_with_recoil, current_pos, view);

		if (estimate_fov > 2.f * target_fov)//cool check but rather useless
			return;

		const auto current = animation_system->get_lastest_animation_unsafe(player);
		if (!current)return;//dunno why that would happen
		/*if (!current.has_value() ||
			(visible && !player->is_visible(const_cast<matrix3x4*>(current.value()->bones))))
			return;*/

		const auto animations = animation_system->get_valid_animations(player, 1.f); //range > 0.f ?  : std::vector<c_animation_system::animation*> { current.value() };

		/*find closest hitbox/record*/
		for (auto& animation : animations) {
			for (const auto& hitbox : override_hitbox) {
				auto box = player->get_hitbox_position(hitbox, const_cast<matrix3x4*>(animation->bones));
				if (!box.has_value())
					continue;

				const auto fov = math::get_fov(current_angle_with_recoil, current_pos, box.value());
				if (fov <= target_fov)
					if (fov < std::get<2>(best_match1))
						best_match1 = std::make_tuple(box.value(), animation, fov, 0);
			}
		}
		/*damage check*/
		if (std::get<2>(best_match1) != FLT_MAX) {//we have something useful
			if (std::get<1>(best_match1)->index == player->index()) {//current scan has the closest
				const auto wall = trace_system->wall_penetration(local->get_shoot_position(),
					std::get<0>(best_match1), std::get<1>(best_match1));

				if (wall.has_value()) {
					if (wall.value().damage > std::get<3>(best_match1)) {
						std::get<3>(best_match1) = wall.value().damage;
						best_match = std::make_tuple(std::get<0>(best_match1), std::get<1>(best_match1), std::get<2>(best_match1));
					}
				}
			}
		}
	});

	if (std::get<2>(best_match) == FLT_MAX || !std::get<1>(best_match) || !std::get<1>(best_match)->player)
		return std::nullopt;
	std::optional<std::tuple<c_vector3d, float, c_animation_system::animation*, float>> target = std::nullopt;
	auto t = std::make_tuple(std::get<0>(best_match), std::get<1>(best_match)->sim_time, std::get<1>(best_match), std::get<2>(best_match));
	target = t;
	return std::make_tuple(std::get<0>(best_match), std::get<1>(best_match)->sim_time, std::get<1>(best_match), std::get<2>(best_match));
}

std::optional<std::tuple<c_vector3d, float, c_animation_system::animation*>> c_aimhelper::get_legit_target(float target_fov, float range, std::optional<c_cs_player::hitbox> override_hitbox, bool visible)
{
	auto local = c_cs_player::get_local_player();

	if (!local)
		return std::nullopt;

	auto current_pos = local->get_shoot_position();
	auto current_angle = engine_client()->get_view_angles();

	std::tuple<c_vector3d, c_animation_system::animation*, float> best_match = { c_vector3d(), nullptr, FLT_MAX };

	client_entity_list()->for_each_player([&](c_cs_player* player) -> void
	{
		if (!player->is_alive() || (visible && player->is_dormant()) || !player->is_enemy())
			return;

		if (!visible && player->get_simtime() + 7.f < global_vars_base->curtime)
			return;

		const auto current = animation_system->get_lastest_animation_unsafe(player);

		if (!current.has_value() ||
			(visible && !player->is_visible(const_cast<matrix3x4*>(current.value()->bones))))
			return;

		// check fov estimate for performance reasons
		auto view = player->get_shoot_position();
		const auto estimate_fov = math::get_fov(current_angle, current_pos, view);

		if (estimate_fov > 2.f * target_fov)
			return;

		const auto animations = range > 0.f ? animation_system->get_valid_animations(player, range) : std::vector<c_animation_system::animation*>{ current.value() };

		for (auto& animation : animations)
			if (!override_hitbox.has_value())
				for (const auto& hitbox : c_cs_player::hitboxes)
				{
					auto box = player->get_hitbox_position(hitbox, const_cast<matrix3x4*>(animation->bones));
					if (!box.has_value())
						continue;

					const auto fov = math::get_fov(current_angle, current_pos, box.value());

					if (fov < std::get<2>(best_match) && fov <= target_fov)
						best_match = std::make_tuple(box.value(), animation, fov);
				}
			else
			{
				auto box = player->get_hitbox_position(override_hitbox.value(), const_cast<matrix3x4*>(animation->bones));
				if (!box.has_value())
					continue;

				const auto fov = math::get_fov(current_angle, current_pos, box.value());

				if (fov < std::get<2>(best_match) && fov <= target_fov)
					best_match = std::make_tuple(box.value(), animation, fov);
			}
	});

	if (std::get<2>(best_match) == FLT_MAX || !std::get<1>(best_match) || !std::get<1>(best_match)->player)
		return std::nullopt;

	return std::make_tuple(std::get<0>(best_match), std::get<1>(best_match)->sim_time, std::get<1>(best_match));
}

std::vector<c_aimhelper::aim_info> c_aimhelper::select_multipoint(c_animation_system::animation* animation, c_cs_player::hitbox box, int32_t group,
	std::optional < Menu22::ragebot::weapon_config> cfg)
{
	std::vector<aim_info> points = {};

	if (!animation || !animation->player || !cfg.has_value())
		return points;

	if (!is_multipoint_hitbox(box))
		return points;

	auto baim_scale = std::max(cfg->pelvis_scale, cfg->body_scale);

	static auto hitbox_scale = 0.f;

	switch (box) {
	case c_cs_player::hitbox::pelvis: {baim_scale = cfg->head_scale; } break;
	case c_cs_player::hitbox::body: {baim_scale = cfg->stomach_scale; } break;
	case c_cs_player::hitbox::thorax: {baim_scale = cfg->stomach_scale; } break;
	case c_cs_player::hitbox::chest: {baim_scale = cfg->chest_scale; } break;
	case c_cs_player::hitbox::upper_chest: {baim_scale = cfg->chest_scale; } break;
	case c_cs_player::hitbox::left_calf: {baim_scale = cfg->legs_scale; } break;
	case c_cs_player::hitbox::right_calf: {baim_scale = cfg->legs_scale; } break;
	case c_cs_player::hitbox::left_foot: {baim_scale = cfg->feet_scale; } break;
	case c_cs_player::hitbox::right_foot: {baim_scale = cfg->feet_scale; } break;
	}

	auto scale = box == c_cs_player::hitbox::head ? cfg->head_scale : baim_scale;
	if (scale > 0.f)
		scale /= 100.f;

	if (scale <= .2f || scale > 1.f)
		return points;

	const auto model = animation->player->get_model();
	if (!model)
		return points;

	const auto studio_model = model_info_client()->get_studio_model(model);
	if (!studio_model)
		return points;

	const auto local = c_cs_player::get_local_player();
	if (!local)
		return points;

	const auto weapon = reinterpret_cast<c_base_combat_weapon*>(
		client_entity_list()->get_client_entity_from_handle(local->get_current_weapon_handle()));
	if (!weapon)
		return points;

	const auto anim = animation_system->get_animation_info(animation->player);
	if (!anim)
		return points;

	const auto hitbox = studio_model->get_hitbox(static_cast<uint32_t>(box), 0);
	if (!hitbox)
		return points;

	const auto is_zeus = weapon->get_item_definition() == weapon_taser;
	if (is_zeus)
		return points;

	auto& mat = animation->bones[hitbox->bone];

	c_vector3d min, max;
	math::vector_transform(hitbox->bbmax, mat, max);
	math::vector_transform(hitbox->bbmin, mat, min);

	const auto center = (min + max) * 0.5f;
	const auto cur_angles = math::calc_angle(center, local->get_shoot_position());

	c_vector3d forward;
	math::angle_vectors(cur_angles, forward);

	auto rs = hitbox->radius * scale;

	if (rs < .2f)
		return points;

	const auto right = forward.cross(c_vector3d(0.f, 0.f, 1.f)) * rs;
	const auto left = c_vector3d(-right.x, -right.y, right.z);
	const auto top = c_vector3d(0.f, 0.f, 1.f) * rs;

	const auto delta = (max - min).normalize();
	c_qangle angle;
	math::vector_angles(delta, angle);
	angle -= cur_angles;
	math::normalize(angle);

	const auto is_horizontal = angle.x < 45.f && angle.x > -45.f;
	const auto is_flipped = angle.y < 0.f;

	if (box == c_cs_player::hitbox::head || (box != c_cs_player::hitbox::left_foot
		&& box != c_cs_player::hitbox::right_foot))
	{
		points.emplace_back(max + top, 0.f, animation, false, center, hitbox->radius, rs, box, group);
		points.emplace_back(min - top, 0.f, animation, false, center, hitbox->radius, rs, box, group);
	}

	points.emplace_back(max - (is_horizontal ? c_vector3d() - top : left),
		0.f, animation, false, center, hitbox->radius, rs, box, group);
	points.emplace_back(max - (is_horizontal ? is_flipped ? left : right : right),
		0.f, animation, false, center, hitbox->radius, rs, box, group);

	if (box != c_cs_player::hitbox::left_foot && box != c_cs_player::hitbox::right_foot)
	{
		points.emplace_back(min - (is_horizontal ? top : left),
			0.f, animation, false, center, hitbox->radius, rs, box, group);
		points.emplace_back(min + (is_horizontal ? is_flipped ? left : right : left),
			0.f, animation, false, center, hitbox->radius, rs, box, group);
	}

	return points;
}

void c_aimhelper::optimize_multipoint(aim_info& info)
{
	static constexpr auto steps = 6;

	const auto local = c_cs_player::get_local_player();

	if (!local || info.rs == 0.f || info.radius == 0.f)
		return;

	auto original_position = info.position;
	auto optimal_position = original_position;
	auto optimal_damage = info.damage;

	for (auto i = 0; i <= steps; i++)
	{
		const auto scale = info.rs / info.radius;
		if (scale <= .2f)
			break;

		const auto target_scale = (scale - .2f) / steps * (steps - i);
		const auto direction = (info.position - info.center) / info.rs;

		original_position = info.center + direction * info.radius * target_scale;

		const auto wall = trace_system->wall_penetration(local->get_shoot_position(),
			original_position, info.animation);

		if (wall.has_value())
		{
			if (info.damage > info.animation->player->get_health()
				&& wall.value().damage < info.animation->player->get_health() + 20.f)
				continue;

			if (wall.value().damage + 20.f < info.damage)
				continue;

			optimal_position = original_position;
			optimal_damage = wall.value().damage;
		}
	}

	info.position = optimal_position;
	info.damage = optimal_damage;
}


bool c_aimhelper::can_hit(c_cs_player* local, c_animation_system::animation* animation, const c_vector3d position, const float chance, c_cs_player::hitbox box)
{
	// generate look-up-table to enhance performance.
	build_seed_table();

	const auto weapon = reinterpret_cast<c_base_combat_weapon*>(
		client_entity_list()->get_client_entity_from_handle(local->get_current_weapon_handle()));

	if (!weapon)
		return false;

	const auto info = weapon_system->get_weapon_data(weapon->get_item_definition());

	if (!info)
		return false;

	const auto studio_model = model_info_client()->get_studio_model(animation->player->get_model());

	if (!studio_model)
		return false;

	// performance optimization.
	if ((local->get_shoot_position() - position).length() > info->flRange)
		return false;

	// setup calculation parameters.
	const auto round_acc = [](const float accuracy) { return roundf(accuracy * 1000.f) / 1000.f; };
	const auto sniper = info->get_weapon_id() == weapon_awp || info->get_weapon_id() == weapon_g3sg1
		|| info->get_weapon_id() == weapon_scar20 || info->get_weapon_id() == weapon_ssg08;
	const auto crouched = local->get_flags() & c_base_player::flags::ducking;

	// calculate inaccuracy.
	const auto weapon_inaccuracy = weapon->get_inaccuracy();

	if (info->get_weapon_id() == weapon_revolver)
		return weapon_inaccuracy < (crouched ? .0050f : .00825f);

	// no need for hitchance, if we can't increase it anyway.
	if (crouched)
	{
		if (round_acc(weapon_inaccuracy) == round_acc(sniper ? info->flInaccuracyCrouchAlt : info->flInaccuracyCrouch))
			return true;
	}
	else
	{
		if (round_acc(weapon_inaccuracy) == round_acc(sniper ? info->flInaccuracyStandAlt : info->flInaccuracyStand))
			return true;
	}

	// calculate start and angle.
	const auto start = local->get_shoot_position();
	const auto aim_angle = math::calc_angle(start, position);
	c_vector3d forward, right, up;
	math::angle_vectors(aim_angle, forward, right, up);

	// keep track of all traces that hit the enemy.
	auto current = 0;

	// setup calculation parameters.
	c_vector3d total_spread, spread_angle, end;
	float inaccuracy, spread_x, spread_y;
	std::tuple<float, float, float>* seed;

	// use look-up-table to find average hit probability.
	for (auto i = 0u; i < total_seeds; i++)  // NOLINT(modernize-loop-convert)
	{
		// get seed.
		seed = &precomputed_seeds[i];

		// calculate spread.
		inaccuracy = std::get<0>(*seed) * weapon_inaccuracy;
		spread_x = std::get<2>(*seed) * inaccuracy;
		spread_y = std::get<1>(*seed) * inaccuracy;
		total_spread = (forward + right * spread_x + up * spread_y).normalize();

		// calculate angle with spread applied.
		math::vector_angles(total_spread, spread_angle);

		// calculate end point of trace.
		math::angle_vectors(spread_angle, end);
		end = start + end.normalize() * info->flRange;

		// did we hit the hitbox?
		if (can_hit_hitbox(start, end, animation, studio_model, box))
			current++;

		// abort if hitchance is already sufficent.
		if (static_cast<float>(current) / static_cast<float>(total_seeds) >= chance)
			return true;

		// abort if we can no longer reach hitchance.
		if (static_cast<float>(current + total_seeds - i) / static_cast<float>(total_seeds) < chance)
			return false;
	}

	return static_cast<float>(current) / static_cast<float>(total_seeds) >= chance;
}

bool c_aimhelper::can_hit_hitbox(const c_vector3d start, const c_vector3d end, c_animation_system::animation* animation, studiohdr* hdr, c_cs_player::hitbox box)
{
	const auto studio_box = hdr->get_hitbox(static_cast<uint32_t>(box), 0);

	if (!studio_box)
		return false;

	c_vector3d min, max;

	const auto is_capsule = studio_box->radius != -1.f;

	if (is_capsule)
	{
		math::vector_transform(studio_box->bbmin, animation->bones[studio_box->bone], min);
		math::vector_transform(studio_box->bbmax, animation->bones[studio_box->bone], max);
		const auto dist = math::segment_to_segment(start, end, min, max);

		if (dist < studio_box->radius)
			return true;
	}

	if (!is_capsule)
	{
		math::vector_transform(math::vector_rotate(studio_box->bbmin, studio_box->rotation), animation->bones[studio_box->bone], min);
		math::vector_transform(math::vector_rotate(studio_box->bbmax, studio_box->rotation), animation->bones[studio_box->bone], max);

		math::vector_i_transform(start, animation->bones[studio_box->bone], min);
		math::vector_i_rotate(end, animation->bones[studio_box->bone], max);

		if (math::intersect_line_with_bb(min, max, studio_box->bbmin, studio_box->bbmax))
			return true;
	}

	return false;
}

// pasted from aimware. thanks polak.
void c_aimhelper::fix_movement(c_user_cmd* cmd, c_qangle& wishangle)
{
	c_vector3d view_fwd, view_right, view_up, cmd_fwd, cmd_right, cmd_up;
	math::angle_vectors(wishangle, view_fwd, view_right, view_up);
	math::angle_vectors(cmd->viewangles, cmd_fwd, cmd_right, cmd_up);

	const auto v8 = sqrtf((view_fwd.x * view_fwd.x) + (view_fwd.y * view_fwd.y));
	const auto v10 = sqrtf((view_right.x * view_right.x) + (view_right.y * view_right.y));
	const auto v12 = sqrtf(view_up.z * view_up.z);

	const c_vector3d norm_view_fwd((1.f / v8) * view_fwd.x, (1.f / v8) * view_fwd.y, 0.f);
	const c_vector3d norm_view_right((1.f / v10) * view_right.x, (1.f / v10) * view_right.y, 0.f);
	const c_vector3d norm_view_up(0.f, 0.f, (1.f / v12) * view_up.z);

	const auto v14 = sqrtf((cmd_fwd.x * cmd_fwd.x) + (cmd_fwd.y * cmd_fwd.y));
	const auto v16 = sqrtf((cmd_right.x * cmd_right.x) + (cmd_right.y * cmd_right.y));
	const auto v18 = sqrtf(cmd_up.z * cmd_up.z);

	const c_vector3d norm_cmd_fwd((1.f / v14) * cmd_fwd.x, (1.f / v14) * cmd_fwd.y, 0.f);
	const c_vector3d norm_cmd_right((1.f / v16) * cmd_right.x, (1.f / v16) * cmd_right.y, 0.f);
	const c_vector3d norm_cmd_up(0.f, 0.f, (1.f / v18) * cmd_up.z);

	const auto v22 = norm_view_fwd.x * cmd->forwardmove;
	const auto v26 = norm_view_fwd.y * cmd->forwardmove;
	const auto v28 = norm_view_fwd.z * cmd->forwardmove;
	const auto v24 = norm_view_right.x * cmd->sidemove;
	const auto v23 = norm_view_right.y * cmd->sidemove;
	const auto v25 = norm_view_right.z * cmd->sidemove;
	const auto v30 = norm_view_up.x * cmd->upmove;
	const auto v27 = norm_view_up.z * cmd->upmove;
	const auto v29 = norm_view_up.y * cmd->upmove;

	cmd->forwardmove = ((((norm_cmd_fwd.x * v24) + (norm_cmd_fwd.y * v23)) + (norm_cmd_fwd.z * v25))
		+ (((norm_cmd_fwd.x * v22) + (norm_cmd_fwd.y * v26)) + (norm_cmd_fwd.z * v28)))
		+ (((norm_cmd_fwd.y * v30) + (norm_cmd_fwd.x * v29)) + (norm_cmd_fwd.z * v27));
	cmd->sidemove = ((((norm_cmd_right.x * v24) + (norm_cmd_right.y * v23)) + (norm_cmd_right.z * v25))
		+ (((norm_cmd_right.x * v22) + (norm_cmd_right.y * v26)) + (norm_cmd_right.z * v28)))
		+ (((norm_cmd_right.x * v29) + (norm_cmd_right.y * v30)) + (norm_cmd_right.z * v27));
	cmd->upmove = ((((norm_cmd_up.x * v23) + (norm_cmd_up.y * v24)) + (norm_cmd_up.z * v25))
		+ (((norm_cmd_up.x * v26) + (norm_cmd_up.y * v22)) + (norm_cmd_up.z * v28)))
		+ (((norm_cmd_up.x * v30) + (norm_cmd_up.y * v29)) + (norm_cmd_up.z * v27));

	const auto ratio = 2.f - fmaxf(fabsf(cmd->sidemove), fabsf(cmd->forwardmove)) / math::forward_bounds;
	cmd->forwardmove *= ratio;
	cmd->sidemove *= ratio;

	wishangle = cmd->viewangles;
}

__forceinline void c_aimhelper::build_seed_table()
{
	if (!precomputed_seeds.empty())
		return;

	for (auto i = 0; i < total_seeds; i++) {
		math::random_seed(i + 1);

		const auto pi_seed = math::random_float(0.f, pi_2);

		precomputed_seeds.emplace_back(math::random_float(0.f, 1.f),
			sin(pi_seed), cos(pi_seed));
	}
}

bool c_aimhelper::can_hit_hitbox(int index, const c_vector3d start, const c_vector3d end, matrix3x4* bones, c_cs_player::hitbox box)
{
	const auto entity = (c_cs_player*)client_entity_list()->get_client_entity(index);

	if (!entity)
		return false;

	const auto model = entity->get_model();
	if (!model)
		return false;

	auto* p_studio_hdr = model_info_client()->get_studio_model(model);
	if (!p_studio_hdr)
		return false;

	const auto studio_box = p_studio_hdr->get_hitbox(static_cast<uint32_t>(box), 0);

	if (!studio_box)
		return false;

	c_vector3d min, max;

	const auto is_capsule = studio_box->radius != -1.f;

	if (is_capsule)
	{
		math::vector_transform(studio_box->bbmin, bones[studio_box->bone], min);
		math::vector_transform(studio_box->bbmax, bones[studio_box->bone], max);
		const auto dist = math::segment_to_segment(start, end, min, max);

		if (dist < studio_box->radius)
			return true;
	}

	if (!is_capsule)
	{
		math::vector_transform(math::vector_rotate(studio_box->bbmin, studio_box->rotation), bones[studio_box->bone], min);
		math::vector_transform(math::vector_rotate(studio_box->bbmax, studio_box->rotation), bones[studio_box->bone], max);

		math::vector_i_transform(start, bones[studio_box->bone], min);
		math::vector_i_rotate(end, bones[studio_box->bone], max);

		if (math::intersect_line_with_bb(min, max, studio_box->bbmin, studio_box->bbmax))
			return true;
	}

	return false;
}