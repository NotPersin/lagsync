#include "c_damage_esp.h"
#include "c_resolver.h"
#include "c_esp.h"
#include "../utils/math.h"
#include "../sdk/c_model_info_client.h"
#include "../menu/c_menu.h"
#include "../sdk/c_weapon_system.h"
#include "../sdk/c_view_beams.h"
#include "../sdk/c_debug_overlay.h"
#include "../sdk/c_surface.h"

#define ARRAY_TO_CCOLOR(VAR) c_color(VAR[0] * 255, VAR[1] * 255, VAR[2] * 255, VAR[3] * 255)

void c_damageesp::draw_damage()
{
	auto local = c_cs_player::get_local_player();

	if (!local || !local->is_alive() || !config->Visuals.enemy.visuals_esp_damage)
		return;

	float curTime = global_vars_base->curtime;

	for (int i = 0; i < MAX_FLOATING_TEXTS; i++)
	{
		FloatingText* txt = &floatingTexts[i % MAX_FLOATING_TEXTS];

		if (!txt->valid)
			continue;

		float endTime = txt->startTime + 1.1f;

		if (endTime < curTime)
		{
			txt->valid = false;
			continue;
		}

		c_vector3d origin_screen;

		if (c_esp::world_to_screen(txt->hitPosition, origin_screen)) {

			float t = 1.0f - (endTime - curTime) / (endTime - txt->startTime);

			origin_screen.y -= t * (35.0f);
			origin_screen.x -= (float)txt->randomIdx * t * 3.0f;

			char msg[12];
			sprintf_s(msg, 12, _("-%dHP"), txt->damage);

			Drawing::DrawString(F::ESP, origin_screen.x, origin_screen.y - 18, c_color(config->Visuals.enemy.damage_color[0] * 255, config->Visuals.enemy.damage_color[1] * 255, config->Visuals.enemy.damage_color[2] * 255, 255),  false, msg);
		}
	}
}

void c_damageesp::on_bullet_impact(c_game_event* event)
{
	auto local = c_cs_player::get_local_player();

	if (!local || !local->is_alive())
		return;

	float x = event->get_float(_("x"));
	float y = event->get_float(_("y"));
	float z = event->get_float(_("z"));

	const auto target = reinterpret_cast<c_cs_player*>(client_entity_list()->get_client_entity(engine_client()->get_player_for_user_id(event->get_int(_("userid")))));

	if (!target || target != local)
		return;

	c_impactpos = c_vector3d(x, y, z);

}

void c_damageesp::on_player_hurt(c_game_event* event)
{
	auto local = c_cs_player::get_local_player();

	if (!local || !local->is_alive())
		return;

	float curTime = global_vars_base->curtime;

	int dmg_health = event->get_int(_("dmg_health"));
	int hitgroup = event->get_int(_("hitgroup"));

	const auto entity = reinterpret_cast<c_cs_player*>(client_entity_list()->get_client_entity(engine_client()->get_player_for_user_id(event->get_int(_("userid")))));
	const auto attacker = reinterpret_cast<c_cs_player*>(client_entity_list()->get_client_entity(engine_client()->get_player_for_user_id(event->get_int(_("attacker")))));

	if (!entity || attacker != local)
		return;

	FloatingText txt;
	txt.startTime = curTime;
	txt.hitgroup = hitgroup;
	txt.hitPosition = c_impactpos;
	txt.damage = dmg_health;
	txt.randomIdx = math::random_float(-5.f, 5.f);
	txt.valid = true;

	matrix3x4 bones[128]{};
	c_vector3d pov;
	if (entity->setup_bones(bones, 128, bone_used_by_anything, global_vars_base->curtime)) {

		auto pos = entity->get_hitbox_position(c_cs_player::hitbox::head, bones);

		if (pos.has_value())
			c_esp::world_to_screen(pos.value(), snipe_scope::pos);
	}
	floatingTexts[floatingTextsIdx++ % MAX_FLOATING_TEXTS] = txt;
}