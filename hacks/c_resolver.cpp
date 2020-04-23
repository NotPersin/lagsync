#include "c_resolver.h"
#include "../utils/math.h"
#include "c_aimhelper.h"
#include "c_trace_system.h"
#include "../sdk/c_client_entity_list.h"
#include <random>
#include "c_esp.h"

c_resolver resolver;

void c_resolver::resolve(c_animation_system::animation* record)
{
	float resolver_yaw = 0.f;

	// is it neither a bot nor a legit player?
	if (!record->player || record->player->get_info().fakeplayer || !config->Ragebot.resolver)
	{
		resolver.missed_due_to_bad_resolve[record->index] = 0;
		return;
	}

	const auto animstate = record->anim_state;

	if (!record->has_anim_state)
		return;

	auto resolver_strength = 0.8f;
	resolver_strength = std::clamp(resolver_strength, 0.8f, 1.f);

	if (record->player->get_velocity().length2d() >= 150.f)
		resolver_strength = 0.5f;

	if (record->player->get_velocity().length2d() >= 150.f)
		resolver.missed_due_to_bad_resolve[record->index] = 0;

	const auto missed_shots = resolver.missed_due_to_bad_resolve[record->index];

	auto logic_resolver_2 = [&]()
	{
		const auto local = c_cs_player::get_local_player();
		if (!local)
			return 0.f;

		auto resolver_yaw = 0.f;
		auto freestanding_yaw = 0.f;

		const auto current_shot = missed_shots % 3;
		const auto maximum_desync = resolver.get_max_desync_delta(record->player) * resolver_strength;
		const auto plus_desync = record->eye_angles.y + maximum_desync;
		const auto minus_desync = record->eye_angles.y - maximum_desync;

		get_target_freestand_yaw(local, &freestanding_yaw);

		const auto diff_from_plus_desync = fabs(math::angle_diff(freestanding_yaw, plus_desync));
		const auto diff_from_minus_desync = fabs(math::angle_diff(freestanding_yaw, minus_desync));

		const auto first_yaw = diff_from_plus_desync < diff_from_minus_desync ? plus_desync : minus_desync;
		const auto second_yaw = diff_from_plus_desync < diff_from_minus_desync ? minus_desync : plus_desync;
		const auto third_yaw = math::calc_angle(local->get_shoot_position(),
			record->player->get_shoot_position()).y;

		switch (current_shot)
		{
		case 0:
			resolver_yaw = first_yaw;
			break;
		case 1:
			resolver_yaw = second_yaw;
			break;
		case 2:
			resolver_yaw = third_yaw;
			break;
		default:
			break;
		}

		return resolver_yaw;
	};

	resolver_yaw = logic_resolver_2();

	record->anim_state.goal_feet_yaw = math::normalize_yaw(resolver_yaw);

	const auto current_animstate = record->player->get_anim_state();

	if (current_animstate)
		current_animstate->goal_feet_yaw = record->anim_state.goal_feet_yaw;
}

void c_resolver::on_player_hurt(c_game_event* event)
{
	if (!event || !config->Ragebot.enable)
		return;

	auto local = c_cs_player::get_local_player();

	if (!local || !local->is_alive())
		return;

	const auto attacker = client_entity_list()->get_client_entity(
		engine_client()->get_player_for_user_id(event->get_int(_("attacker"))));

	const auto target = reinterpret_cast<c_cs_player*>(client_entity_list()->get_client_entity(
		engine_client()->get_player_for_user_id(event->get_int(_("userid")))));

	if (attacker && target && attacker == local && target->is_enemy())
	{
		static const auto hit_msg = __("Hit %s in %s for %d damage");
		_rt(hit, hit_msg);
		char msg[255];

		const auto hitgroup = event->get_int("hitgroup");
		const auto dmg = event->get_int("dmg_health");

		switch (hitgroup)
		{
		case hitgroup_head:
			sprintf_s(msg, hit, target->get_info().name, _("head"), dmg);
			break;
		case hitgroup_leftleg:
		case hitgroup_rightleg:
			sprintf_s(msg, hit, target->get_info().name, _("leg"), dmg);
			break;
		case hitgroup_stomach:
			sprintf_s(msg, hit, target->get_info().name, _("stomach"), dmg);
			break;
		default:
			sprintf_s(msg, hit, target->get_info().name, _("body"), dmg);
			break;
		}

		_events.push_back(_event(msg)); //logging->info(msg);

		int index = target->index();
		resolver.missed_due_to_bad_resolve[index]--;
		resolver.missed_due_to_bad_resolve[index] =
			std::clamp(resolver.missed_due_to_bad_resolve[index], 0, 99);
	}

}

void c_resolver::on_bullet_impact(c_game_event* event)
{
	if (!event || !config->Ragebot.enable)
		return;

	auto local = c_cs_player::get_local_player();

	if (!local || !local->is_alive())
		return;

	const auto target = reinterpret_cast<c_cs_player*>(client_entity_list()->get_client_entity(engine_client()->get_player_for_user_id(event->get_int(_("userid")))));

	if (!target || target != local)
		return;

	c_vector3d pos(event->get_float(_("x")), event->get_float(_("y")), event->get_float(_("z")));

	// this can still be improved by a lot.
	// it doesnt account for the delay between the server, client and such.
	auto missed_due_to_spread = [&](c_vector3d pos)
	{
		c_vector3d aim_eye_pos = resolver.last_eye_pos;
		c_qangle impact_angle = math::calc_angle(aim_eye_pos, pos);

		c_vector3d forward, right, up, new_angle, end;
		math::angle_vectors(impact_angle, forward, right, up);
		math::vector_angles(forward, new_angle);

		// calculate end point of trace.
		math::angle_vectors(new_angle, end);

		if (resolver.last_shot_missed_index > 0)
		{
			if (!c_aimhelper::can_hit_hitbox(resolver.last_shot_missed_index,
				resolver.last_eye_pos,
				resolver.last_eye_pos + end.normalize() * 8192.f,
				resolver.last_bones, resolver.last_hitbox))
			{
				resolver.missed_due_to_spread[resolver.last_shot_missed_index]++;
				resolver.missed_due_to_spread[resolver.last_shot_missed_index] = std::clamp(resolver.missed_due_to_spread[resolver.last_shot_missed_index], 0, 99);

				_events.push_back(_event("Missed due to spread")); //logging->info(_("Missed due to spread"));
			}
			else
			{
				resolver.missed_due_to_bad_resolve[resolver.last_shot_missed_index]++;
				resolver.missed_due_to_bad_resolve[resolver.last_shot_missed_index] = std::clamp(resolver.missed_due_to_bad_resolve[resolver.last_shot_missed_index], 0, 99);
			}
		}
		resolver.last_shot_missed_index = 0;
	};
	missed_due_to_spread(pos);
}

void c_resolver::on_round_start(c_game_event* event)
{
	resolver.last_shot_missed_index = 0;
	resolver.has_target = false;

	std::fill(resolver.missed_due_to_bad_resolve, resolver.missed_due_to_bad_resolve +
		ARRAYSIZE(resolver.missed_due_to_bad_resolve), 0);

	std::fill(resolver.missed_due_to_spread, resolver.missed_due_to_spread +
		ARRAYSIZE(resolver.missed_due_to_spread), 0);
}

void c_resolver::on_player_death(c_game_event* event)
{
	if (!event || !config->Ragebot.enable)
		return;

	auto local = c_cs_player::get_local_player();

	if (!local || !local->is_alive())
	{
		resolver.last_shot_missed_index = 0;
		return;
	}

	int victim_id = event->get_int(_("userid"));
	int killer_id = event->get_int(_("attacker"));

	bool is_headshot = event->get_int(_("headshot"));

	int victim_index = engine_client()->get_player_for_user_id(victim_id);
	int killer_index = engine_client()->get_player_for_user_id(killer_id);

	if (victim_index && killer_index && local)
	{
		resolver.missed_due_to_bad_resolve[victim_index] = 0;
		resolver.missed_due_to_spread[victim_index] = 0;
	}
}

bool c_resolver::get_target_freestand_yaw(c_cs_player* target, float* yaw)
{
	float dmg_left = 0.f;
	float dmg_right = 0.f;

	static auto get_rotated_pos = [](c_vector3d start, float rotation, float distance)
	{
		float rad = deg2rad(rotation);
		start.x += cos(rad) * distance;
		start.y += sin(rad) * distance;

		return start;
	};

	const auto local = c_cs_player::get_local_player();

	if (!local || !target || !local->is_alive())
		return false;

	c_vector3d local_eye_pos = local->get_shoot_position();
	c_vector3d eye_pos = target->get_shoot_position();
	c_qangle angle = math::calc_angle(local_eye_pos, eye_pos);

	auto backwards = angle.y;

	c_vector3d pos_left = get_rotated_pos(eye_pos, angle.y + 90.f, 40.f);
	c_vector3d pos_right = get_rotated_pos(eye_pos, angle.y + 90.f, -40.f);

	const auto wall_left = trace_system->wall_penetration(local_eye_pos, pos_left,
		nullptr, local);

	const auto wall_right = trace_system->wall_penetration(local_eye_pos, pos_right,
		nullptr, local);

	if (wall_left.has_value())
		dmg_left = wall_left.value().damage;

	if (wall_right.has_value())
		dmg_right = wall_right.value().damage;

	// we can hit both sides, lets force backwards
	if (fabsf(dmg_left - dmg_right) < 10.f)
	{
		*yaw = backwards;
		return false;
	}

	bool direction = dmg_left > dmg_right;
	*yaw = direction ? angle.y - 90.f : angle.y + 90.f;

	return true;
}

float c_resolver::get_max_desync_delta(c_cs_player* player)
{
	if (!player || !player->is_alive() || player->is_dormant_virt())
		return 0.f;

	auto animstate = uintptr_t(player->get_anim_state());
	if (!animstate)
		return 0.f;

	float duckammount = *(float*)(animstate + 0xA4);
	float speedfraction = std::max<float>(0, std::min(*reinterpret_cast<float*>(animstate + 0xF8), 1.f));
	float speedfactor = speedfraction >= 0.f ? std::max<float>(0, std::min(1.f, *reinterpret_cast<float*> (animstate + 0xFC))) : speedfactor = 0.f;

	float unk1 = ((*reinterpret_cast<float*> (animstate + 0x11C) * -0.30000001) - 0.19999999)* speedfraction;
	float unk2 = unk1 + 1.f;

	if (duckammount > 0)
		unk2 += +((duckammount * speedfactor) * (0.5f - unk2));

	auto retvalue = *(float*)(animstate + 0x334) * unk2;

	if (retvalue > 1.f) // account for rounding errors
		retvalue -= 1.f;

	retvalue = std::clamp(retvalue, 28.f, 60.f);
	return retvalue;
}