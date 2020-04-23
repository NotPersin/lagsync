#include "c_antiaim.h"
#include "../utils/math.h"
#include "../sdk/c_game_rules.h"
#include "../sdk/c_client_state.h"
#include "../sdk/c_debug_overlay.h"
#include "../sdk/c_input.h"
#include "../sdk/c_prediction.h"
#include "../sdk/c_weapon_system.h"
#include "../menu/c_menu.h"
#include "c_aimhelper.h"
#include "c_miscellaneous.h"
#include "c_ragebot.h"
#include "c_trace_system.h"
#include "c_prediction_system.h"
#include "../hooks/c_events.h"
#include <random>
#include "c_animation_system.h"

static std::random_device rd;
static std::mt19937 rng(rd());

namespace antiaim_globals {
	float to_choke = 0;
	//float fake_angle = 0;
	//float real_angle = 0;
	float max_delta = 0;
	bool init_antaims = false;
	matrix3x4 fake_matrix[128] = { matrix3x4() };
	matrix3x4 real_matrix[128] = { matrix3x4() };
}

void c_antiaim::fakelag(c_cs_player* local, c_user_cmd* cmd, bool& send_packet)
{
	static constexpr auto target_standing = 1;
	static constexpr auto target_moving = 13;
	static constexpr auto target_air = 8;

	static auto last_origin = c_vector3d();
	static auto last_simtime = 0.f;
	static auto last_ducked = false;
	static auto fakeland = false;

	static auto onpeek_timer = 0.f;
	static auto unduck_timer = 0.f;

	static auto onpeek = false;

	const auto weapon = reinterpret_cast<c_base_combat_weapon*>(
		client_entity_list()->get_client_entity_from_handle(local->get_current_weapon_handle()));

	const auto info = weapon_system->get_weapon_data(weapon->get_item_definition());

	if (!local ||
		!local->is_alive() ||
		!weapon || !info ||
		local->get_move_type() == c_base_player::movetype_observer ||
		local->get_move_type() == c_base_player::movetype_noclip ||
		local->get_move_type() == c_base_player::movetype_ladder ||
		(local->get_flags() & c_base_player::frozen) ||
		cmd->buttons & c_user_cmd::flags::use ||
		info->WeaponType == weapontype_grenade ||
		weapon->get_item_definition() == weapon_hegrenade || weapon->get_item_definition() == weapon_molotov || weapon->get_item_definition() == weapon_incgrenade || weapon->get_item_definition() == weapon_smokegrenade ||
		(config->Ragebot.disableaaonknife && info->WeaponType == weapontype_knife) ||
		(config->Ragebot.disableaaonfreeze && local->get_flags() & c_cs_player::frozen))
	{
		//		animation_system->update_simple_local_player(local, cmd);
		return;
	}

	const auto standing = local->get_velocity().length2d() <= 15;
	const auto moving = local->get_velocity().length2d() > 15;

	Menu22::ragebot::antiaim_config cfg;

	auto choke_amount = 1;
	auto max_choke_amount = 14;

	if (config->Ragebot.fakelag_enable) {
		if ((cmd->buttons & c_user_cmd::jump) || GetAsyncKeyState(VK_SPACE) || !(local->get_flags() & c_cs_player::flags::on_ground)) {
			cfg = config->Ragebot.air;
			choke_amount = config->Ragebot.air_fakelag;
		}
		else {
			if (standing) {
				cfg = config->Ragebot.stand;
				choke_amount = config->Ragebot.stand_fakelag;
			}
			else if (moving) {
				cfg = config->Ragebot.moving;
				choke_amount = config->Ragebot.move_fakelag;
			}
		}

		if ((cmd->buttons & c_user_cmd::attack && config->Ragebot.fl_disable_shot) ||
			(cmd->buttons & c_user_cmd::reload && config->Ragebot.fl_disable_reload) ||
			((cmd->buttons & c_user_cmd::weapon1 || cmd->buttons & c_user_cmd::weapon2) && config->Ragebot.fl_disable_switch))
			choke_amount = 0;

		if ((cmd->buttons & c_user_cmd::attack && config->Ragebot.fl_boost_shot) ||
			(cmd->buttons & c_user_cmd::reload && config->Ragebot.fl_boost_reload) ||
			((cmd->buttons & c_user_cmd::weapon1 || cmd->buttons & c_user_cmd::weapon2) && config->Ragebot.fl_boost_switch) ||
			((antiaim->get_last_max() >= 56) && config->Ragebot.fl_boost_max_delta))
			choke_amount = config->Ragebot.fl_boost_scale;

		std::clamp(choke_amount, 0, max_choke_amount);
	}

	if ((config->Ragebot.exploits > 0 && choke_amount > 2 && local->get_velocity().length2d() < 150) || (config->Ragebot._antiaim && cfg.fake_type > 0 && config->Ragebot.stand_fakelag < 3))
		choke_amount = 2;

	static int smooth_amount = choke_amount;

	if (smooth_amount != choke_amount && global_vars_base->tickcount % 4 == 0) {
		if (smooth_amount > choke_amount)
			smooth_amount--;
		else
			smooth_amount++;
	}

	if (config->Ragebot.fakelag_enable || cfg.fake_type > 0) {
		send_packet = client_state->choked_commands >= (config->Ragebot.lag_type == 0 ? smooth_amount : choke_amount);
		estimated_choke = (config->Ragebot.lag_type == 0 ? smooth_amount : choke_amount);
	}

	if (GetAsyncKeyState(config->Ragebot.fakeduck) && config->Ragebot.fakeduck_enable)
	{
		static int choke = 0;
		const auto max = 12;
		//send_packet = choke > max;
		choke = choke > max ? 0 : choke + 1;
		auto d = max / 2;
		if (choke > static_cast<int>(client_state->choked_commands))
			send_packet = false;

		estimated_choke = choke;
	}

}

void c_antiaim::run(c_cs_player* local, c_user_cmd* cmd)
{
	static const auto get_check_sum = reinterpret_cast<uint32_t(__thiscall*)(c_user_cmd*)>(
		sig("client_panorama.dll", "53 8B D9 83 C8"));

	const auto weapon = reinterpret_cast<c_base_combat_weapon*>(
		client_entity_list()->get_client_entity_from_handle(local->get_current_weapon_handle()));

	const auto info = weapon_system->get_weapon_data(weapon->get_item_definition());

	if (
		!local ||
		!local->is_alive() ||
		!weapon || 
		!info ||
		local->get_move_type() == c_base_player::movetype_observer ||
		local->get_move_type() == c_base_player::movetype_noclip ||
		local->get_move_type() == c_base_player::movetype_ladder ||
		cmd->buttons & c_user_cmd::flags::use ||
		info->WeaponType == weapontype_grenade ||
		weapon->get_item_definition() == weapon_hegrenade || weapon->get_item_definition() == weapon_molotov || weapon->get_item_definition() == weapon_incgrenade || weapon->get_item_definition() == weapon_smokegrenade ||
		!config->Ragebot._antiaim ||
		(config->Ragebot.disableaaonknife  && info->WeaponType == weapontype_knife)||
		(config->Ragebot.disableaaonfreeze && (local->get_flags() & c_cs_player::frozen || game_rules->is_freeze_period())))
	{
		last_real = cmd->viewangles.y;
		last_fake = cmd->viewangles.y;
		inied = false;
		return;
	}

	inied = true;
	jitter = !jitter;

	const auto standing = local->get_velocity().length2d() <= 15 && !(cmd->buttons & c_user_cmd::jump);
	const auto moving   = local->get_velocity().length2d() > 15 && !(cmd->buttons & c_user_cmd::jump);
	const auto air = !(local->get_flags() & c_cs_player::flags::on_ground) || cmd->buttons & c_user_cmd::jump;
	Menu22::ragebot::antiaim_config cfg;

	if (standing && !air)
		cfg = config->Ragebot.stand;
	else if (moving && !air)
		cfg = config->Ragebot.moving;
	if (air)
		cfg = config->Ragebot.air;

	for (auto i = 0; i <= client_state->choked_commands; i++)
	{
		auto& c = input->commands[(cmd->command_number + i - client_state->choked_commands) % 150];
		auto& verified = input->verified_commands[(cmd->command_number + i - client_state->choked_commands) % 150];

		if (c.command_number >= shot_cmd && shot_cmd >= cmd->command_number - client_state->choked_commands)
		{
			auto target_angle = c.viewangles;
			c.viewangles = input->commands[shot_cmd % 150].viewangles;
			c_aimhelper::fix_movement(&c, target_angle);
			math::ensure_bounds(c.viewangles, *reinterpret_cast<c_vector3d*>(&c.forwardmove));
			c_miscellaneous::set_buttons_for_direction(&c);
		}
		else
		{
			auto target_angle = c.viewangles;
			view_angles = c.viewangles;
			static auto rotate = false;
			auto demoted_delta = std::clamp(max_delta, 0.f, cfg.max_delta); // 
			auto proper_delta = rotate ? demoted_delta : -demoted_delta; //  sway
			static float old_delta = proper_delta;
			if (i != client_state->choked_commands) // desync
			{
				sendpacket = true;

				//if (cmd->buttons & c_user_cmd::flags::attack && !cfg.desync_on_shot)
				//	return;

				if (cfg.avoid_overlap) {
					if (c.viewangles.y + 15 >= c.viewangles.y + proper_delta)
						proper_delta += 15;
					else if (c.viewangles.y - 15 <= c.viewangles.y + proper_delta)
						proper_delta -= 15;
				}

				if (config->Ragebot.fake_invert > 0) {
					switch (config->Ragebot.fake_invert) {
						case 1: {
							if (at_target(local) > 0 && at_target(local) < 180)
								rotate = !rotate; break;
						}
						case 2: {
							static size_t lastTime = 0;
							if (GetAsyncKeyState(config->Ragebot.desync_invert))
							{
								if (GetTickCount64() > lastTime) {
									rotate = !rotate;
									lastTime = GetTickCount64() + 450;
								}
							}						
							break;
						}
						case 3: {
							if (stand && !air)
								rotate = !rotate; break;
						}
						case 4: {
							if (moving && !air)
								rotate = !rotate; break;
						}
						case 5: {
							if (air)
								rotate = !rotate; break;
						}
					}
				}

				if (cfg.fake_type == 3) { // jitter
					if (jitter)
						rotate = !rotate;
				}

				// lby with lean
				if (lby_update < global_vars_base->curtime && cfg.lean_break != 0) {
					const auto lean_delta = cfg.twist && cmd->tick_count % 2 == 0 ? ((cfg.lean_break * 0.5) / 100.f) : (cfg.lean_break / 100.f);
					c.viewangles.y = math::normalize_yaw(c.viewangles.y + proper_delta * lean_delta);
				}
				else if (cfg.fake_type == 1)
					c.viewangles.y = math::normalize_yaw(c.viewangles.y + proper_delta);
				else if (cfg.fake_type == 2) {
					if (c.viewangles.y >= 0)
						c.viewangles.y = math::normalize_yaw(c.viewangles.y + proper_delta);
					else if (c.viewangles.y < 0)
						c.viewangles.y = math::normalize_yaw(c.viewangles.y - proper_delta);
				}
				old_delta = proper_delta;				
				last_fake =(c.viewangles.y);
				// 
			}
			else // real
			{
				sendpacket = false;

				if ((cmd->buttons & c_user_cmd::flags::attack) && real_shot)
					return;

				switch (cfg.pitch) {
					case 1: { c.viewangles.x = -89.f; }  break;
					case 2: { c.viewangles.x = 89.f; }  break;
					case 3: { c.viewangles.x = cmd->tick_count % 2 ? -cfg.pitch_range : cfg.pitch_range; }  break;
				}
				
				auto yaw = cfg.yaw == 1 || cfg.yaw == 2 ? c.viewangles.y + 180.f : c.viewangles.y;

				static float manual_yaw = 0.f;

				auto manual = [&]() -> bool {
					manual_yaw = engine_client()->get_view_angles().y + 180.f;

					if (GetAsyncKeyState(config->Ragebot.left) && config->Ragebot.left_enable)
						_manual_yaw = -90;
					if (GetAsyncKeyState(config->Ragebot.right) && config->Ragebot.right_enable)
						_manual_yaw = 90;
					if (GetAsyncKeyState(config->Ragebot.back) && config->Ragebot.back_enable)
						_manual_yaw = 0.1f;


					if (_manual_yaw != 0) {
						manual_yaw += _manual_yaw;
						return true;
					}
					return false;
				};

				if (cfg.yawbase == 1 && at_target(local) != 0)
					yaw += at_target(local);
				else if (cfg.yawbase == 2)// && !manual())
					yaw = calculate_ideal_yaw(local, true);

				if (config->Ragebot.manaul_override && manual())
					yaw = manual_yaw;

				if (cfg.yaw == 2 && cfg.jitter_length != 0)
					yaw += (jitter ? 1.f : -1.f) * (cfg.jitter_length);
				
				if (cfg.fake_type == 1) {
					if (yaw >= 0) {
						c.viewangles.y = (yaw - proper_delta);
						last_real = (yaw - proper_delta);
					}
					else {
						c.viewangles.y = (yaw + proper_delta);
						last_real = (yaw + proper_delta);
					}
				}
				else {
					c.viewangles.y = math::normalize_yaw(yaw);
					last_real = (yaw);
				}

				view_angles = c.viewangles;
			}

			c_aimhelper::fix_movement(&c, target_angle);
			math::ensure_bounds(c.viewangles, *reinterpret_cast<c_vector3d*>(&c.forwardmove));
			c_miscellaneous::set_buttons_for_direction(&c);
		}

		const auto in_attack = (cmd->buttons & c_user_cmd::attack) ||
			(cmd->buttons & c_user_cmd::attack2);

		if (!in_attack && local->get_velocity().length2d() < 15.f 
		&& !(cmd->buttons & c_user_cmd::move_left)
		&& !(cmd->buttons & c_user_cmd::move_right)
		&& config->Ragebot.fakelag_enable)
			cmd->sidemove = cmd->command_number % 2 == 0 ? 2.f : -2.f;
	}


}

void c_antiaim::predict(c_cs_player* local, c_user_cmd* cmd)
{
	const auto state = local->get_anim_state();

	if (!local->is_local_player() || !state)
		return;

	const auto time = ticks_to_time(local->get_tick_base());

	if (local->get_velocity().length2d() >= .1f || fabsf(local->get_velocity().z) >= 100.f)
		next_lby_update = time + .22f;
	else if (time > next_lby_update)
	{
		lby_update = time;
		next_lby_update = time + 1.1f;
	}

	const auto weapon = reinterpret_cast<c_base_combat_weapon*>(
		client_entity_list()->get_client_entity_from_handle(local->get_current_weapon_handle()));

	min_delta = *reinterpret_cast<float*>(&state->pad10[512]);
	max_delta = *reinterpret_cast<float*>(&state->pad10[516]);

	float max_speed = 260.f;

	if (weapon)
	{
		const auto info = weapon_system->get_weapon_data(weapon->get_item_definition());
#undef max

		if (info)
			max_speed = std::max(.001f, info->flMaxPlayerSpeed);
	}

	auto velocity = local->get_velocity();
	const auto abs_velocity_length = powf(velocity.length(), 2.f);
	const auto fraction = 1.f / (abs_velocity_length + .00000011920929f);

	if (abs_velocity_length > 97344.008f)
		velocity *= velocity * 312.f;

	auto speed = velocity.length();

	if (speed >= 260.f)
		speed = 260.f;

	feet_speed_stand = (1.923077f / max_speed) * speed;
	feet_speed_ducked = (2.9411764f / max_speed) * speed;

	auto feet_speed = (((stop_to_full_running_fraction * -.3f) - .2f) * std::clamp(feet_speed_stand, 0.f, 1.f)) + 1.f;

	if (state->duck_amount > 0.f)
		feet_speed = feet_speed + ((std::clamp(feet_speed_ducked, 0.f, 1.f) * state->duck_amount) * (.5f - feet_speed));

	min_delta *= feet_speed;
	max_delta *= feet_speed;

	if (stop_to_full_running_fraction > 0.0 && stop_to_full_running_fraction < 1.0)
	{
		const auto interval = global_vars_base->interval_per_tick * 2.f;

		if (inied)
			stop_to_full_running_fraction = stop_to_full_running_fraction - interval;
		else
			stop_to_full_running_fraction = interval + stop_to_full_running_fraction;

		stop_to_full_running_fraction = std::clamp(stop_to_full_running_fraction, 0.f, 1.f);
	}

	if (speed > 135.2f && inied)
	{
		stop_to_full_running_fraction = fmaxf(stop_to_full_running_fraction, .0099999998f);
		inied = false;
	}

	if (speed < 135.2f && !inied)
	{
		stop_to_full_running_fraction = fminf(stop_to_full_running_fraction, .99000001f);
		inied = true;
	}
}

bool c_antiaim::on_peek(c_cs_player* local, bool& target)
{
	target = true;

	const auto weapon_cfg = c_aimhelper::get_weapon_conf();

	if (local->get_abs_velocity().length2d() < 2.f || !weapon_cfg.has_value())
		return false;

	target = false;

	const auto velocity = local->get_velocity() * (2.f / 3.f);
	const auto ticks = client_state->choked_commands > 1 ? 14 : 10;
	const auto pos = local->get_shoot_position() + velocity * ticks_to_time(ticks);

	auto found = false;

	client_entity_list()->for_each_player([&] (c_cs_player* player) -> void
	{
		if (!player->is_enemy() || player->is_dormant() || player->is_local_player())
			return;

		const auto record = animation_system->get_latest_animation(player);

		if (!record.has_value())
			return;

		const auto scan = c_ragebot::scan_record_gun(local, record.value(), pos);

		if (scan.has_value() && (scan.value().damage > weapon_cfg.value().min_dmg
				|| player->get_health() < scan.value().damage))
			found = true;
	});

	if (found)
		return true;

	target = true;
	return false;
}

float c_antiaim::calculate_ideal_yaw(c_cs_player* local, bool estimate)
{
		// step in which we test for damage.
		static constexpr auto step = 90.f;

		// maybe the name and the arguments of this function are questionable.
		const auto target = c_aimhelper::get_legit_target(28738174.56f, 0.f, c_cs_player::hitbox::body);

		if (!target.has_value())
			return engine_client()->get_view_angles().y + 180;

		const auto anim = std::get<2>(target.value());
		const auto head = anim->player->get_shoot_position();
		const auto shoot = local->get_shoot_position();
		const auto direction = math::calc_angle(local->get_origin(), anim->player->get_origin());

		// determine damage points.
		float angle_to_damage_ratio[4] = { };
		c_vector3d positions[7] = { };
		for (auto i = 0; i < 4; i++)
		{
			const auto current_angle = direction.y + i * step;
			auto& ratio = angle_to_damage_ratio[i];

			auto back_pos = math::rotate_2d(shoot, current_angle, 19.f);

			if (i == 0 || i == 2)
			{
				positions[0] = math::rotate_2d(back_pos + c_vector3d(0.f, 0.f, -6.f), current_angle + 90.f, 5.f);
				positions[1] = math::rotate_2d(back_pos + c_vector3d(0.f, 0.f, -6.f), current_angle + 90.f, -5.f);
				positions[2] = math::rotate_2d(back_pos + c_vector3d(0.f, 0.f, 0.f), current_angle + 90.f, 0.f);
				positions[3] = math::rotate_2d(back_pos + c_vector3d(0.f, 0.f, -6.f), current_angle + 90.f, 0.f);
				positions[4] = math::rotate_2d(back_pos + c_vector3d(0.f, 0.f, -6.f), current_angle + 90.f, 2.5f);
				positions[5] = math::rotate_2d(back_pos + c_vector3d(0.f, 0.f, -6.f), current_angle + 90.f, -2.5f);
				positions[6] = math::rotate_2d(back_pos + c_vector3d(0.f, 0.f, -3.f), current_angle + 90.f, 0.f);
			}
			else
			{
				positions[0] = math::rotate_2d(shoot + c_vector3d(0.f, 0.f, -6.f), current_angle, 27.f);
				positions[1] = math::rotate_2d(shoot + c_vector3d(0.f, 0.f, -6.f), current_angle, 21.f);
				positions[2] = math::rotate_2d(shoot + c_vector3d(0.f, 0.f, 0.f), current_angle, 21.f);
				positions[3] = math::rotate_2d(shoot + c_vector3d(0.f, 0.f, -7.f), current_angle, 13.f);
				positions[4] = math::rotate_2d(shoot + c_vector3d(0.f, 0.f, -6.f), current_angle, 24.f);
				positions[5] = math::rotate_2d(shoot + c_vector3d(0.f, 0.f, -6.f), current_angle, 17.f);
				positions[6] = math::rotate_2d(shoot + c_vector3d(0.f, 0.f, -3.f), current_angle, 21.f);
			}

			// the run is only for visual representation, we do not need accurate data, so we drop multipoints.
			if (estimate)
			{
				const auto wall = trace_system->wall_penetration(head, positions[2], nullptr, local);

				if (!wall.has_value())
					continue;

				if (wall.value().damage > ratio)
					ratio = wall.value().damage;

				continue;
			}

			for (auto& pos : positions)
			{
				const auto wall = trace_system->wall_penetration(head, pos, nullptr, local);

				if (!wall.has_value())
					continue;

				if (wall.value().damage > ratio)
					ratio = wall.value().damage;

				// abort if angle is already out in the open.
				if (ratio >= 60000.f)
					break;
			}
		}

		// determine lowest and highest damage.
		auto lowest_dmg = std::make_pair(0, FLT_MAX), highest_dmg = std::make_pair(0, 0.f);
		for (auto i = 3; i >= 0; i--)
		{
			const auto& ratio = angle_to_damage_ratio[i];

			// only ever face the enemy forwards if we gain a significant
			// advantage compared to all alternatives.
			if (i == 0 && lowest_dmg.second > 0.f
				&& fabsf(lowest_dmg.second - ratio) < 100.f)
				continue;

			if (lowest_dmg.second > ratio)
				lowest_dmg = std::make_pair(i, ratio);

			if (ratio > highest_dmg.second)
				highest_dmg = std::make_pair(i, ratio);
		}

		// determine second highest damage.
		auto second_highest_dmg = 0.f;
		for (auto i = 3; i >= 0; i--)
		{
			const auto& ratio = angle_to_damage_ratio[i];

			if (ratio > second_highest_dmg&& highest_dmg.first != i)
				second_highest_dmg = ratio;
		}

		// no suitable point found.
		if (fabsf(lowest_dmg.second - highest_dmg.second) < 20.f && highest_dmg.second <= 40000.f)
			return engine_client()->get_view_angles().y + 180.f;

		// force backwards at target.
		if (fabsf(lowest_dmg.second - highest_dmg.second) < 100.f)
			return direction.y + 2 * step;

		// force opposite when head on edge.
		if (fabsf(second_highest_dmg - highest_dmg.second) >= 50000.f)
			return direction.y + highest_dmg.first * step + 180.f;

		// set target yaw.
		return direction.y + lowest_dmg.first * step;

	
}

float c_antiaim::at_target( c_cs_player* local) const
{
	c_cs_player* target = nullptr;
	c_qangle target_angle;

	c_qangle original_viewangles;
	original_viewangles = engine_client()->get_view_angles();

	auto lowest_fov = 90.f;
	for (auto i = 0; i < global_vars_base->max_clients; i++)
	{
		auto player = reinterpret_cast<c_cs_player*>(client_entity_list()->get_client_entity(i));
		if (!player || !player->is_alive() || !player->is_enemy() || player == c_cs_player::get_local_player())
			continue;

		if (player->is_dormant() && (player->get_simtime() > global_vars_base->curtime || player->get_simtime() + 5.f < global_vars_base->curtime))
			continue;

		auto enemy_pos = player->get_origin();
		enemy_pos.z += 64.f;

		const auto angle = math::calc_angle(c_cs_player::get_local_player()->get_shoot_position(), enemy_pos);
		const auto fov = math::get_fov(original_viewangles, angle);

		if (fov < lowest_fov)
		{
			target = player;
			lowest_fov = fov;
			target_angle = angle;
		}
	}

	if (!target)
		return 0.f;

	if (target->is_dormant())
		return 0.f;

	return target_angle.y + 180;
}

//deprecated

float c_antiaim::get_visual_choke()
{
	return visual_choke;
}

void c_antiaim::increment_visual_progress()
{
	if (!config->Ragebot._antiaim || !engine_client()->is_ingame())
		return;

	visual_choke = 1.f;

	if (estimated_choke >= 2)
		visual_choke = static_cast<float>(client_state->choked_commands) / static_cast<float>(estimated_choke);
}

float c_antiaim::get_pitch(Menu22::ragebot::antiaim_config& conf) {
	float pitch = 0.f;

	//const char* pitch[] = { "off", "normal", "jitter", "fake jitter" };

	switch (conf.pitch) {
	case 1: { pitch = -89.f; } break;
	case 2: { pitch = 89.f; } break;
	case 3: { pitch = jitter ? -89 : 0.f; } break;
	}

	return pitch;
}

float c_antiaim::get_last_real()
{
	return last_real;
}

float c_antiaim::get_last_fake()
{
	return last_fake;
}

float c_antiaim::get_last_lby()
{
	return last_lby;
}
