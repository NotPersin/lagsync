#include "c_ragebot.h"
#include "c_aimhelper.h"
#include "c_trace_system.h"
#include "../utils/math.h"
#include "../sdk/c_weapon_system.h"
#include "../sdk/c_debug_overlay.h"
#include "c_prediction_system.h"
#include "c_antiaim.h"
#include "c_resolver.h"

void c_ragebot::aim(c_cs_player* local, c_user_cmd* cmd, bool& send_packet)
{
	if (!engine_client()->is_ingame() || !engine_client()->is_connected() || !c_cs_player::get_local_player() || !c_cs_player::get_local_player()->is_alive())
		return;

	resolver.has_target = false;
	last_pitch = std::nullopt;

	const auto weapon = reinterpret_cast<c_base_combat_weapon*>(
		client_entity_list()->get_client_entity_from_handle(local->get_current_weapon_handle()));
	if (!weapon)
		return;

	const auto wpn_info = weapon_system->get_weapon_data(weapon->get_item_definition());
	if (!wpn_info)
		return;

	if (!local->can_shoot(cmd, global_vars_base->curtime))
		return;

	const auto weapon_cfg = c_aimhelper::get_weapon_conf();

	if (!weapon_cfg.has_value())
		return;

	std::vector<aim_info> hitpoints = {};
	std::deque<std::tuple<float, c_cs_player*>> target_selection = {};

	auto local_origin = local->get_origin();

	client_entity_list()->for_each_player([&](c_cs_player* player) -> void
	{
		if (!player || player->is_dormant_virt() || !player->is_enemy() ||
			!player->is_alive() || player->get_gun_game_immunity())
			return;

		float health = (float)player->get_health();
		auto distance = (local_origin - player->get_origin()).length2d();
		if (distance > 1.f && health > 0.f) // e.g. 10 health = distance * 0.1f
			distance *= (health / 100.f); // e.g. 100 health = distance * 1.f

		target_selection.push_back(std::make_tuple(distance, player));
	});

	std::sort(std::begin(target_selection), std::end(target_selection),
		[](auto const& t1, auto const& t2)
	{
		return std::get<0>(t1) < std::get<0>(t2);
	});

	for (auto current_entity = target_selection.cbegin();
		current_entity != target_selection.cend(); current_entity++)
	{
		// std::get c_cs_player*
		const auto player = (c_cs_player*)(std::get<1>(*current_entity));
		if (!player)
			continue;

		const auto latest = animation_system->get_latest_animation(player);

		if (!latest.has_value())
			continue;

		const auto is_knife = wpn_info->WeaponType == weapontype_knife;

		std::optional<aim_info> aimbot_backtrack = {};
		std::optional<aim_info> target = {};

		auto all_valid_anims = animation_system->get_valid_animations(player);

		for (auto& animation : all_valid_anims)
		{
			if (animation->is_backtrackable)
			{
				const auto alternative = is_knife ? scan_record_knife(local, animation) :
					scan_record_gun(local, animation);

				if (alternative->damage > 0.f)
				{
					aimbot_backtrack = alternative;
					break;
				}
			}
		}

		if (aimbot_backtrack.has_value()) // backtracking
		{
			target = aimbot_backtrack;
		}
		else
		{
			const auto latest_record = is_knife ? scan_record_knife(local, latest.value()) :
				scan_record_gun(local, latest.value());

			if (latest_record.has_value() && latest_record.value().damage > 0.f)
			{
				target = latest_record;
			}
			else
			{
				const auto oldest = animation_system->get_oldest_animation(player);
				if (oldest.has_value())
				{
					const auto oldest_record = is_knife ?
						scan_record_knife(local, latest.value()) :
						scan_record_gun(local, latest.value());

					if (oldest_record.has_value() && oldest_record.value().damage > 0.f)
					{
						target = oldest_record;
					}
					else
					{
						continue;
					}
				}
				else
				{
					continue;
				}
			}
		}

		if (target.has_value())
			hitpoints.push_back(target.value());
	}

	aim_info best_match = { c_vector3d(), -FLT_MAX, nullptr, false, c_vector3d(), 0.f, 0.f, c_cs_player::hitbox::head, 0 };

	// find best target spot of all valid spots.
	for (auto& hitpoint : hitpoints)
		if (hitpoint.damage > best_match.damage)
			best_match = hitpoint;

	// stop if no target found.
	if (best_match.damage < 0.f)
		return;

	// scope the weapon.
	if ((wpn_info->get_weapon_id() == weapon_g3sg1 || wpn_info->get_weapon_id() == weapon_scar20
		|| wpn_info->get_weapon_id() == weapon_ssg08 || wpn_info->get_weapon_id() == weapon_awp
		|| wpn_info->get_weapon_id() == weapon_sg556 || wpn_info->get_weapon_id() == weapon_aug) && weapon->get_zoom_level() == 0 && config->Ragebot.autoscope)
		cmd->buttons |= c_user_cmd::flags::attack2;

	// run autostop.
	if (config->Ragebot.autostop || (config->Misc.slowwalk && GetAsyncKeyState(config->Misc.slowwalk_key)))
		autostop(local, cmd);

	// optimize multipoint and select final aimpoint.
	if (!game_rules->is_valve_ds())
		c_aimhelper::optimize_multipoint(best_match);

	// calculate angle.
	const auto shoot_pos = local->get_shoot_position();
	const auto angle = math::calc_angle(shoot_pos, best_match.position);

	// store pitch for eye correction.
	last_pitch = angle.x;

	if (!c_aimhelper::can_hit(local, best_match.animation, best_match.position, weapon_cfg.value().hitchance / 100.f, best_match.hitbox))
		return;

	// set correct information to user_cmd.
	cmd->viewangles = angle;
	cmd->tick_count = time_to_ticks(best_match.animation->sim_time) + time_to_ticks(calculate_lerp());
	if (config->Ragebot.autofire) {
		antiaim->set_last_shot(true);
		if ((!send_packet && config->Ragebot.fakeduck_enable && GetAsyncKeyState(config->Ragebot.fakeduck)) || (!config->Ragebot.fakeduck_enable || !GetAsyncKeyState(config->Ragebot.fakeduck)))
			cmd->buttons |= best_match.alt_attack ? c_user_cmd::attack2 : c_user_cmd::attack;

		// store shot info for resolver.
		if (!best_match.alt_attack)
		{
			resolver.last_aim_angle = angle;
			resolver.last_eye_pos = local->get_shoot_position();
			resolver.last_shot_missed_index = best_match.animation->index;
			resolver.last_hitbox = best_match.hitbox;
			memcpy(resolver.last_bones, best_match.animation->bones, 128 * sizeof(float) * 12);
			resolver.has_target = true;
		}

		if (config->Visuals.lagcompydady)
			best_match.animation->player->draw_hitboxes(best_match.animation->bones, 5.f, config->Colors.lagcompcolor[0] * 255, config->Colors.lagcompcolor[1] * 255, config->Colors.lagcompcolor[2] * 255, config->Colors.lagcompcolor[3] * 255);
	}
	else {
		// store shot info for resolver.
		if (!best_match.alt_attack)
		{
			resolver.last_aim_angle = angle;
			resolver.last_eye_pos = local->get_shoot_position();
			resolver.last_shot_missed_index = best_match.animation->index;
			resolver.last_hitbox = best_match.hitbox;
			memcpy(resolver.last_bones, best_match.animation->bones, 128 * sizeof(float) * 12);
			resolver.has_target = true;
		}

		if (config->Visuals.lagcompydady)
			best_match.animation->player->draw_hitboxes(best_match.animation->bones, 5.f, config->Colors.lagcompcolor[0] * 255, config->Colors.lagcompcolor[1] * 255, config->Colors.lagcompcolor[2] * 255, config->Colors.lagcompcolor[3] * 255);
	}
}

void c_ragebot::autostop(c_cs_player* local, c_user_cmd* cmd)
{
	if (cmd->buttons & c_user_cmd::jump)
		return;

	static const auto nospread = cvar()->find_var(_("weapon_accuracy_nospread"));

	const auto weapon = reinterpret_cast<c_base_combat_weapon*>(
		client_entity_list()->get_client_entity_from_handle(local->get_current_weapon_handle()));

	if (nospread->get_int() || !local->is_on_ground() ||
		(weapon && weapon->get_item_definition() == weapon_taser) && local->is_on_ground())
		return;

	const auto wpn_info = weapon_system->get_weapon_data(weapon->get_item_definition());

	if (!wpn_info)
		return;

	auto& info = get_autostop_info();

	if (info.call_time == global_vars_base->curtime)
	{
		info.did_stop = true;
		return;
	}

	info.did_stop = false;
	info.call_time = global_vars_base->curtime;

	if (local->get_velocity().length2d() <= wpn_info->get_standing_accuracy(weapon))
		return;
	else
	{

		cmd->forwardmove = 0.f;
		cmd->sidemove = 0.f;

		prediction_system->repredict(local, cmd);

		if (local->get_velocity().length2d() <= wpn_info->get_standing_accuracy(weapon))
			return;
	}

	c_qangle dir;
	math::vector_angles(prediction_system->unpredicted_velocity, dir);
	const auto angles = engine_client()->get_view_angles();
	dir.y = angles.y - dir.y;

	c_vector3d move;
	math::angle_vectors(dir, move);

	if (prediction_system->unpredicted_velocity.length2d() > .1f)
		move *= -math::forward_bounds / std::max(std::abs(move.x), std::abs(move.y));

	cmd->forwardmove = move.x;
	cmd->sidemove = move.y;

	const auto backup = cmd->viewangles;
	cmd->viewangles = angles;
	prediction_system->repredict(local, cmd);
	cmd->viewangles = backup;

	if (local->get_velocity().length2d() > prediction_system->unpredicted_velocity.length2d())
	{
		cmd->forwardmove = 0.f;
		cmd->sidemove = 0.f;
	}

	prediction_system->repredict(local, cmd);
}

std::optional<c_ragebot::aim_info> c_ragebot::scan_record(c_cs_player* local, c_animation_system::animation* animation)
{
	const auto weapon = reinterpret_cast<c_base_combat_weapon*>(
		client_entity_list()->get_client_entity_from_handle(local->get_current_weapon_handle()));

	if (!weapon)
		return std::nullopt;

	const auto info = weapon_system->get_weapon_data(weapon->get_item_definition());

	if (!info)
		return std::nullopt;

	const auto is_zeus = weapon->get_item_definition() == weapon_taser;
	const auto is_knife = !is_zeus && info->WeaponType == weapontype_knife;

	if (is_knife)
		return scan_record_knife(local, animation);

	return scan_record_gun(local, animation);
}

std::optional<c_ragebot::aim_info> c_ragebot::scan_record_knife(c_cs_player* local, c_animation_system::animation* animation)
{
	static const auto is_behind = [](c_cs_player* local, c_animation_system::animation* animation) -> bool
	{
		auto vec_los = animation->origin - local->get_origin();
		vec_los.z = 0.0f;

		c_vector3d forward;
		math::angle_vectors(c_vector3d(animation->eye_angles.x, antiaim->get_last_real(), animation->eye_angles.x), forward);
		forward.z = 0.0f;

		return vec_los.normalize().dot(forward) > 0.475f;
	};

	static const auto should_stab = [](c_cs_player* local, c_animation_system::animation* animation) -> bool
	{
		struct table_t
		{
			unsigned char swing[2][2][2];
			unsigned char stab[2][2];
		};

		static const table_t table = {
			{
				{
					{ 25, 90 },
					{ 21, 76 }
				},
				{
					{ 40, 90 },
					{ 34, 76 }
				}
			},
			{
				{ 65, 180 },
				{ 55, 153 }
			}
		};

		const auto weapon = reinterpret_cast<c_base_combat_weapon*>(
			client_entity_list()->get_client_entity_from_handle(local->get_current_weapon_handle()));

		if (!weapon)
			return false;

		const auto has_armor = animation->player->get_armor() > 0;
		const auto first_swing = weapon->get_next_primary_attack() + 0.4f < global_vars_base->curtime;
		const auto behind = is_behind(local, animation);

		const int stab_dmg = table.stab[has_armor][behind];
		const int slash_dmg = table.swing[false][has_armor][behind];
		const int swing_dmg = table.swing[first_swing][has_armor][behind];

		if (animation->player->get_health() <= swing_dmg)
			return false;

		if (animation->player->get_health() <= stab_dmg)
			return true;

		if (animation->player->get_health() > swing_dmg + slash_dmg + stab_dmg)
			return true;

		return false;
	};

	const auto studio_model = model_info_client()->get_studio_model(animation->player->get_model());

	if (!studio_model)
		return std::nullopt;

	const auto stab = should_stab(local, animation);
	const auto range = stab ? 32.0f : 48.0f;
	game_trace tr;
	auto spot = animation->player->get_hitbox_position(c_cs_player::hitbox::upper_chest, animation->bones);
	const auto hitbox = studio_model->get_hitbox(static_cast<uint32_t>(c_cs_player::hitbox::upper_chest), 0);

	if (!spot.has_value() || !hitbox)
		return std::nullopt;

	c_vector3d forward;
	const auto calc = math::calc_angle(local->get_shoot_position(), spot.value());
	math::angle_vectors(calc, forward);

	spot.value() += forward * hitbox->radius;

	c_trace_system::run_emulated(animation, [&]() -> void
	{
		uint32_t filter[4] = { c_engine_trace::get_filter_simple_vtable(), reinterpret_cast<uint32_t>(local), 0, 0 };

		ray r;
		c_vector3d aim;
		const auto angle = math::calc_angle(local->get_shoot_position(), spot.value());
		math::angle_vectors(angle, aim);
		const auto end = local->get_shoot_position() + aim * range;
		r.init(local->get_shoot_position(), end);

		engine_trace()->trace_ray(r, mask_solid, reinterpret_cast<trace_filter*>(filter), &tr);

		if (tr.fraction >= 1.0f)
		{
#undef min
#undef max

			const c_vector3d min(-16.f, -16.f, -18.f);

			const c_vector3d max(16.f, 16.f, 18.f);
			r.init(local->get_shoot_position(), end, min, max);
			engine_trace()->trace_ray(r, mask_solid, reinterpret_cast<trace_filter*>(filter), &tr);
		}
	});

	if (tr.entity != animation->player || !config->Ragebot.autoknife)
		return std::nullopt;

	return aim_info{ tr.endpos, 100.f, animation, stab, c_vector3d(), 0.f, 0.f, c_cs_player::hitbox::head, 0 };
}


bool c_ragebot::is_breaking_lagcomp(c_animation_system::animation* animation)
{
	static constexpr auto teleport_dist = 64 * 64;

	const auto info = animation_system->get_animation_info(animation->player);

	if (!info || info->frames.size() < 2)
		return false;

	if (info->frames[0].dormant)
		return false;

	auto prev_org = info->frames[0].origin;
	auto skip_first = true;

	// walk context looking for any invalidating event
	for (auto& record : info->frames)
	{
		if (skip_first)
		{
			skip_first = false;
			continue;
		}

		if (record.dormant)
			break;

		auto delta = record.origin - prev_org;
		if (delta.length2dsqr() > teleport_dist)
		{
			// lost track, too much difference
			return true;
		}

		// did we find a context smaller than target time?
		if (record.sim_time <= animation->sim_time)
			break; // hurra, stop

		prev_org = record.origin;
	}

	return false;
}

std::optional<c_ragebot::aim_info> c_ragebot::scan_record_gun(c_cs_player* local, c_animation_system::animation* animation, std::optional<c_vector3d> pos)
{
	const auto weapon_cfg = c_aimhelper::get_weapon_conf();


	const auto weapon = reinterpret_cast<c_base_combat_weapon*>(client_entity_list()->get_client_entity_from_handle(local->get_current_weapon_handle()));

	if (!weapon)
		return std::nullopt;

	if (!animation || !animation->player || !weapon_cfg.has_value())
		return std::nullopt;

	const auto info = animation_system->get_animation_info(animation->player);

	if (!info)
		return std::nullopt;

	const auto cfg = weapon_cfg.value();

	auto should_baim = false;
	const auto center = animation->player->get_hitbox_position(c_cs_player::hitbox::pelvis, animation->bones);
	if (center.has_value())
	{
		const auto center_wall = trace_system->wall_penetration(pos.value_or(local->get_shoot_position()),
			center.value(), animation);

		if (center_wall.has_value() && center_wall.value().hitbox == c_cs_player::hitbox::pelvis
			&& center_wall.value().damage - 10.f > animation->player->get_health())
			should_baim = true;
	}

	aim_info best_match = { c_vector3d(), -FLT_MAX, nullptr, false, c_vector3d(), 0.f, 0.f, c_cs_player::hitbox::head, 0 };

	const auto scan_box = [&](c_cs_player::hitbox hitbox)
	{
		auto box = animation->player->get_hitbox_position(hitbox, const_cast<matrix3x4*>(animation->bones));
		if (!box.has_value())
			return;

		auto points = pos.has_value() ?
			std::vector<aim_info>() :
			c_aimhelper::select_multipoint(animation, hitbox, hitgroup_head, cfg);
		points.emplace_back(box.value(), 0.f, animation, false, box.value(), 0.f, 0.f, hitbox, hitgroup_head);

		for (auto& point : points)
		{
			const auto wall = trace_system->wall_penetration(pos.value_or(local->get_shoot_position()),
				point.position, animation);

			if (!wall.has_value())
				continue;

			if (hitbox == c_cs_player::hitbox::head && hitbox != wall.value().hitbox)
				continue;

			point.hitgroup = wall.value().hitgroup;

			if (hitbox == c_cs_player::hitbox::upper_chest
				&& (wall.value().hitbox == c_cs_player::hitbox::head || wall.value().hitbox == c_cs_player::hitbox::neck))
				continue;

			point.damage = wall.value().damage;

			if (point.damage > best_match.damage)
				best_match = point;
		}
	};

	const auto data = weapon_system->get_weapon_data(weapon->get_item_definition());

	if (!data)
		return std::nullopt;

	if (animation->player->get_health() <= cfg.body_aim_health)
		should_baim = true;

	//if (_config.Ragebot.smart_aim)
	//{
	//	if (animation->player->get_health() <= 50)
	//		should_baim = true;

	//	if (animation->player->get_velocity().length2d() > 0.1 && !animation->player->is_on_ground())
	//		should_baim = true;

	//	const auto is_slow_walking =
	//		animation->anim_state.feet_yaw_rate >= 0.01f &&
	//		animation->anim_state.feet_yaw_rate <= 0.8f;

	//	if (is_slow_walking)
	//		should_baim = true;
	//}

	if (cfg.body_aim_in_air)
		if (animation->player->get_velocity().length2d() > 0.1 && !animation->player->is_on_ground())
			should_baim = true;

	if (cfg.body_aim_lethal && animation->player->get_health() < data->iDamage)
		should_baim = true;

	if (GetAsyncKeyState(cfg.body_aim_key) && cfg.body_aim_key_enable)
		should_baim = true;

	if (resolver.missed_due_to_spread[animation->player->index()] + resolver.missed_due_to_bad_resolve[animation->player->index()] >= cfg.body_after_x_missed_spread + 1)
		should_baim = true;

	if (cfg.body_aim_if_not_on_shot && !animation->lag)
		should_baim = true;

	std::vector<c_cs_player::hitbox> hitboxes_scan;

	if (!weapon_cfg->hitscan_chest && !weapon_cfg->hitscan_stomach && !weapon_cfg->hitscan_pelvis && !weapon_cfg->hitscan_legs && !weapon_cfg->hitscan_feet)
		should_baim = false;

	auto strip_hitboxes =
		resolver.missed_due_to_bad_resolve[animation->player->index()] >= 2;

	if (animation->is_backtrackable)
	{
		if (resolver.missed_due_to_bad_resolve[animation->player->index()] <= 2)
		{
			should_baim = false;
			strip_hitboxes = false;
		}
	}
	if (cfg.hitbox_override != 0 && GetAsyncKeyState(cfg.hitbox_override_key)) {
		switch (cfg.hitbox_override) {
			case 1: {
				hitboxes_scan.push_back(c_cs_player::hitbox::head);
			} break;
			case 2: {
				hitboxes_scan.push_back(c_cs_player::hitbox::upper_chest);
				hitboxes_scan.push_back(c_cs_player::hitbox::chest);
			} break;
			case 3: {
				hitboxes_scan.push_back(c_cs_player::hitbox::body);
				hitboxes_scan.push_back(c_cs_player::hitbox::thorax);
			} break;
			case 4: {
				hitboxes_scan.push_back(c_cs_player::hitbox::left_calf);
				hitboxes_scan.push_back(c_cs_player::hitbox::right_calf);
				hitboxes_scan.push_back(c_cs_player::hitbox::left_thigh);
				hitboxes_scan.push_back(c_cs_player::hitbox::right_thigh);
			} break;
			case 5: {
				hitboxes_scan.push_back(c_cs_player::hitbox::left_foot);
				hitboxes_scan.push_back(c_cs_player::hitbox::right_foot);
			} break;
		}
	}
	else if (should_baim || strip_hitboxes)
	{
		if (weapon_cfg->hitscan_pelvis || strip_hitboxes)
		{
			hitboxes_scan.push_back(c_cs_player::hitbox::pelvis);
		}
		if (!strip_hitboxes)
		{
			if (weapon_cfg->hitscan_chest)
			{
				//hitboxes_scan.push_back(c_cs_player::hitbox::upper_chest);
				hitboxes_scan.push_back(c_cs_player::hitbox::chest);
			}
			if (weapon_cfg->hitscan_stomach)
			{
				hitboxes_scan.push_back(c_cs_player::hitbox::body);
				//hitboxes_scan.push_back(c_cs_player::hitbox::thorax);
			}
			//if (weapon_cfg->hitscan_legs)
			//{
			//	hitboxes_scan.push_back(c_cs_player::hitbox::left_calf);
			//	hitboxes_scan.push_back(c_cs_player::hitbox::right_calf);
			//	hitboxes_scan.push_back(c_cs_player::hitbox::left_thigh);
			//	hitboxes_scan.push_back(c_cs_player::hitbox::right_thigh);
			//}
			//if (weapon_cfg->hitscan_feet)
			//{
			//	hitboxes_scan.push_back(c_cs_player::hitbox::left_foot);
			//	hitboxes_scan.push_back(c_cs_player::hitbox::right_foot);
			//}
		}
	}
	else
	{
		if (weapon_cfg->hitscan_head && weapon_cfg->hitbox_prefer == 0)
			hitboxes_scan.push_back(c_cs_player::hitbox::head);
		if (weapon_cfg->hitscan_chest)
		{
			hitboxes_scan.push_back(c_cs_player::hitbox::upper_chest);
			hitboxes_scan.push_back(c_cs_player::hitbox::chest);
		}
		if (weapon_cfg->hitscan_stomach)
		{
			hitboxes_scan.push_back(c_cs_player::hitbox::body);
			hitboxes_scan.push_back(c_cs_player::hitbox::thorax);
		}
		if (weapon_cfg->hitscan_pelvis)
		{
			hitboxes_scan.push_back(c_cs_player::hitbox::pelvis);
		}
		if (weapon_cfg->hitscan_legs)
		{
			hitboxes_scan.push_back(c_cs_player::hitbox::left_calf);
			hitboxes_scan.push_back(c_cs_player::hitbox::right_calf);
			hitboxes_scan.push_back(c_cs_player::hitbox::left_thigh);
			hitboxes_scan.push_back(c_cs_player::hitbox::right_thigh);
		}
		if (weapon_cfg->hitscan_feet)
		{
			hitboxes_scan.push_back(c_cs_player::hitbox::left_foot);
			hitboxes_scan.push_back(c_cs_player::hitbox::right_foot);
		}
	}
	for (const auto& hitbox : hitboxes_scan)
		scan_box(hitbox);

	if (best_match.damage >= cfg.min_dmg || best_match.damage - 10.f >= animation->player->get_health())
		return best_match;

	return std::nullopt;
}

c_ragebot::autostop_info& c_ragebot::get_autostop_info()
{
	static autostop_info info{ -FLT_MAX, false };
	return info;
}