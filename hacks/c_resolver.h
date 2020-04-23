#pragma once

#include "c_animation_system.h"
#include "../sdk/c_game_event_manager.h"

class c_resolver
{
public:
	bool has_target;

	c_qangle last_aim_angle;
	c_qangle last_eye_pos;
	matrix3x4 last_bones[128];
	c_cs_player::hitbox last_hitbox;

	int last_shot_missed_index;
	int missed_due_to_spread[65];
	int missed_due_to_bad_resolve[65];
	int resolver_method[65];
	int base_weight[65];
	int base_prev_weight[65];
	int cycle_difference[65];

	static void on_player_hurt(c_game_event* event);
	static void on_bullet_impact(c_game_event* event);
	static void on_round_start(c_game_event* event);
	static void on_player_death(c_game_event* event);
	static void resolve(c_animation_system::animation* record);
	static bool get_target_freestand_yaw(c_cs_player* target, float* yaw);
	static float get_max_desync_delta(c_cs_player* player);
}; extern c_resolver resolver;