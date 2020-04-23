#include "c_aimhelper.h"
#include "c_hitmarker.h"
#include "c_esp.h"
#include "c_trace_system.h"
#include "../hooks/c_surface_.h"
#include "../sdk/c_debug_overlay.h"
#include "../sdk/c_client_entity_list.h"

std::vector<impact_info> impacts;
std::vector<hitmarker_info> hitmarkers;
int c_hitmarker::hit_alpha = 0;

#define ARRAY_TO_CCOLOR(VAR) c_color(VAR[0] * 255, VAR[1] * 255, VAR[2] * 255, VAR[3] * 255)

void c_hitmarker::draw()
{
	if (!config->Visuals.hitmarker2d)
		return;

	linear_fade(hit_alpha, 0, 235, 255 / 1.7f, false);

	if (hit_alpha <= 0)
		return;

	int w, h;
	engine_client()->get_screen_size(w, h);
	// top
	Drawing::DrawLine((w / 2) - 10, (h / 2) - 8, (w / 2) - 4, (h / 2) - 3,c_color(255,255,255,hit_alpha));//c_color(255,0,0, hit_alpha)); //  red left
	Drawing::DrawLine((w / 2) + 10, (h / 2) - 8, (w / 2) + 4, (h / 2) - 3,c_color(255,255,255,hit_alpha));//c_color(0, 0, 255, hit_alpha)); // blue right
	// bottom																		
	Drawing::DrawLine((w / 2) + 10, (h / 2) + 8, (w / 2) + 4, (h / 2) + 3,c_color(255,255,255,hit_alpha));//c_color(0, 255, 0, hit_alpha)); // greeb right
	Drawing::DrawLine((w / 2) - 10, (h / 2) + 8, (w / 2) - 4, (h / 2) + 3,c_color(255,255,255,hit_alpha));//c_color(255, 0, 255, hit_alpha)); //  purple left

}

void c_hitmarker::on_round_start(c_game_event* event)
{
	if (hitmarkers.size() > 0)
		hitmarkers.clear();

	hit_alpha = 0;
}

void c_hitmarker::on_bullet_impact(c_game_event* event)
{
	auto local = c_cs_player::get_local_player();

	if (!local || !local->is_alive())
		return;

	impact_info info;

	info.x = event->get_float("x");
	info.y = event->get_float("y");
	info.z = event->get_float("z");

	info.time = global_vars_base->curtime;

	impacts.push_back(info);
}

void c_hitmarker::deadsound(c_game_event* event)
{
}

void c_hitmarker::on_player_hurt(c_game_event* event)
{
	auto local = c_cs_player::get_local_player();

	const auto attacker = client_entity_list()->get_client_entity(engine_client()->get_player_for_user_id(event->get_int(_("attacker"))));
	const auto target = reinterpret_cast<c_cs_player*>(client_entity_list()->get_client_entity(engine_client()->get_player_for_user_id(event->get_int(_("userid")))));

	if (!local || !local->is_alive())
		return;

	if (attacker && attacker == local) {
		if (target && target->is_enemy())
		{
			if (config->Visuals.killeffect && !target->is_alive())
				local->m_flHealthShotBoostExpirationTime() = global_vars_base->curtime + 1.f;

			if (config->Visuals.hiteffect && target->is_alive()) {
				local->m_flHealthShotBoostExpirationTime() = global_vars_base->curtime + 0.5f;
			}

			impact_info best_impact;

			float best_impact_distance = -1;

			float time = global_vars_base->curtime;

			hit_alpha = 255;

			for (int i = 0; i < impacts.size(); i++)
			{
				auto iter = impacts[i];

				if (time > iter.time + 1.f)
				{
					impacts.erase(impacts.begin() + i);
					continue;
				}

				c_vector3d position = c_vector3d(iter.x, iter.y, iter.z);

				c_vector3d enemy_pos = target->get_origin();

				float distance = position.dist_to(enemy_pos);

				if (distance < best_impact_distance || best_impact_distance == -1)
				{
					best_impact_distance = distance;
					best_impact = iter;
				}
			}

			if (best_impact_distance == -1)
				return;

			if (config->Visuals.hitsound == 1)
				surface()->PlaySound_("buttons\\arena_switch_press_02.wav");
			else if (config->Visuals.hitsound == 2)
				surface()->PlaySound_("training\\bell_impact.wav");
			else if (config->Visuals.hitsound == 3)
				surface()->PlaySound_("training\\timer_bell.wav");
		}
	}
}
