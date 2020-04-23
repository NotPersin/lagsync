#include "c_esp.h"
#include "../utils/math.h"
#include "../sdk/c_model_info_client.h"
#include "../utils/c_config.h"
#include "../sdk/c_weapon_system.h"
#include "../sdk/c_view_beams.h"
#include "../sdk/c_debug_overlay.h"
#include "../menu/c_menu.h"
#include "../sdk/c_surface.h"
#include "c_chams.h"
#include "c_damageesp.h"
#include "c_trace_system.h"
#include "c_antiaim.h"
#include "../sdk/c_material_system.h"
#include "../hooks/c_events.h"
#include "c_resolver.h"
#include "c_hitmarker.h"
#include "../utils/dlight.h"
#include "../hooks/c_client.h"
#include "../sdk/c_input.h"
#include "../main.h"
#include "../utils/c_log.h"
#include "../hooks/c_client_mode_shared.h"
#include "../hooks/c_model_render_.h"

float dormant_alpha[64];

#define FADE_TIME 2.5f
#define ARRAY_TO_CCOLOR(VAR) c_color(VAR[0] * 255, VAR[1] * 255, VAR[2] * 255, VAR[3] * 255)
#define global_color  c_color(18, 18, 18, 200)
#define t_selected_color  c_color(config->Misc.menucolor[0] * 255, config->Misc.menucolor[1] * 255, config->Misc.menucolor[2] * 255, 245)

namespace mouse {
	c_vector2d main_mouse;
	c_vector2d last_mouse;

	c_vector2d i_drag;
	c_vector2d s_drag;
	c_vector2d d_drag;
	c_vector2d k_drag;
};

void c_esp::draw_progresstext(c_vector2d& position, const char* name, const float progress)
{
	auto size = Drawing::GetTextSize(F::Indicators, name);
	int width = size.right, height = size.top;

	const auto green = c_color(173, 255, 47);
	const auto yellow = c_color(219, 219, 50);
	const auto red = c_color(191, 0, 10);
	const auto white = c_color(255, 250, 250);
	auto color = c_color(255,255,255);
	if (progress >= 0.70f)
		color = green;
	else if (progress < 0.70f && progress > 0.50f)
		color = yellow;
	else if (progress == -1.f)
		color = white;
	else
		color = red;
	Drawing::DrawString(F::Indicators, position.x - 25, position.y, color, false, name);
	position.y -= height;

	return;
}

void c_esp::draw_progressbar(c_vector2d& position, const char* name, const c_color color, const float progress)
{

	auto size = Drawing::GetTextSize(F::ESP, name);
	int width = size.right, height =  size.top;

	 Drawing::DrawRect(position.x + 8, position.y, 184, 20, c_color(32, 32, 32, 255));
	Drawing::DrawRect(position.x + 9, position.y, 182, 20, c_color(35, 35, 45, 255));
	Drawing::DrawRect(position.x + 9, position.y, 1, 20, c_color(config->Misc.menucolor[0] * 255, config->Misc.menucolor[1] * 255, config->Misc.menucolor[2] * 255, 255));

	Drawing::DrawString(F::ESP, position.x + 14, position.y + 2, c_color(255, 255, 255, 155), false, name);
	//surface()->OutlinedRect(position.x + width + 22, position.y + 8,  100, 3, c_color(65, 65,65, 255));
		Drawing::DrawRect(position.x + width + 22, position.y + 8, 100, 3, c_color(25, 25, 25, 255));
		Drawing::DrawRect(position.x + width + 22, position.y + 8, progress * 100, 3, c_color(config->Misc.menucolor[0] * 255, config->Misc.menucolor[1] * 255, config->Misc.menucolor[2] * 255, 255));
	
	if (config->Ragebot.fakelag_enable && config->Ragebot.enable)
		position.y += config->Visuals.indicators_styles == 1 ? 20 : -20;

	return;
}

void c_esp::draw_progressbar(c_vector2d& position, const char* name, const c_color color, const float progress, DWORD font)
{
	float progress_alpha = 155 + progress * 100.f;
	Drawing::DrawRect(position.x, position.y, 100, 20, c_color(24, 19, 28, progress_alpha));
	Drawing::DrawRect(position.x, position.y + 17, progress * 100.f, 3, c_color(config->Misc.menucolor[0] * 255, config->Misc.menucolor[1] * 255, config->Misc.menucolor[2] * 255, progress_alpha));
	Drawing::DrawString(font,position.x, position.y, c_color(255, 255, 255, 240), false, name);

	position.y += config->Visuals.indicators_styles == 1 ? 20 : -20;

	return;
}

float lerp(float t, float a, float b) {
	return (1 - t) * a + t * b;
}

const float ind_speed = 0.1f;

void c_esp::indicator() {

	if (config->Visuals.indicators_styles != 0 || (!config->Visuals.keybinds && !config->Visuals.indicators))
		return;
	
	c_vector2d position = c_vector2d(100, 500);

	bool dragging = false;
	if (mouse::main_mouse.x > position.x - mouse::i_drag.x && mouse::main_mouse.y > position.y - mouse::i_drag.y && mouse::main_mouse.x < (position.x - mouse::i_drag.x) + 200 && mouse::main_mouse.y < (position.y - mouse::i_drag.y) + 20 && GetAsyncKeyState(VK_LBUTTON) && menu->open) {

		mouse::i_drag.x += mouse::main_mouse.x - mouse::last_mouse.x;
		mouse::i_drag.y += mouse::main_mouse.y - mouse::last_mouse.y;
		dragging = true;
	}
	// move container
	position.x -= mouse::i_drag.x;
	position.y -= mouse::i_drag.y;

	std::string spec_text = "indicators / keybinds";

	if (config->Visuals.keybinds && !config->Visuals.indicators)
		spec_text = "keybinds";
	if (!config->Visuals.keybinds && config->Visuals.indicators)
		spec_text = "indicators";

	if (menu->open)
		Drawing::DrawString(F::ESP, position.x, position.y + 6, dragging ? t_selected_color : c_color(255, 255, 255, 155), FONT_CENTER, spec_text.c_str());

	if (!c_cs_player::get_local_player())
		return;
	if (!c_cs_player::get_local_player()->is_alive())
		return;
	if (menu->open)
		return;

	if (config->Visuals.indicators) {
		if (config->Ragebot.fakelag_enable && config->Ragebot.enable) { // packet
			static float value = 0.f;
			float newvalue = antiaim->get_visual_choke();
			value = lerp(ind_speed, value, newvalue);
			//char buf[24];
			//sprintf(buf, "LAG : %i", antiaim->get_last_choke());
			draw_progresstext(position, "LAG", value);
		}
		if (config->Ragebot.fakelag_enable && config->Ragebot.enable) { // desync
			const auto value = (antiaim->get_last_max() / 58.f);
			//char buf[24];
			//sprintf(buf, "FAKE : %f", value);
			draw_progresstext(position, "FAKE", value);

		}
	}
	if (config->Visuals.keybinds) {
		if (GetAsyncKeyState(config->Misc.slowwalk_key) && config->Misc.slowwalk) { // slowwalk
			draw_progresstext(position, "SLOW", -1);
		}
		if (config->Misc.airstuck_enable && GetAsyncKeyState(config->Misc.airstuck_key)) { // air stuck
			draw_progresstext(position, "AIR", -1);
		}
		if (config->Ragebot.exploits == 1) { // double
			const auto value = exploits.exploiting ? 0.f : 1.f;
			draw_progresstext(position, "DT", value);
		}
		if (GetAsyncKeyState(config->Ragebot.fakeduck) && config->Ragebot.fakeduck_enable) { // fakeduck
			draw_progresstext(position, "DUCK", -1);
		}
	}
}

void c_esp::indicator_ini() {

	if (!config->Visuals.indicators || config->Visuals.indicators_styles == 0)
		return;

	c_vector2d position =c_vector2d(0, 500);

	auto c = c_color(config->Misc.menucolor[0] * 255, config->Misc.menucolor[1] * 255, config->Misc.menucolor[2] * 255, 255);

	bool dragging = false;

	if (mouse::main_mouse.x > position.x - mouse::i_drag.x && mouse::main_mouse.y > position.y - mouse::i_drag.y && mouse::main_mouse.x < (position.x - mouse::i_drag.x) + 200 && mouse::main_mouse.y < (position.y - mouse::i_drag.y) + 20 && GetAsyncKeyState(VK_LBUTTON) && menu->open) {

		mouse::i_drag.x += mouse::main_mouse.x - mouse::last_mouse.x;
		mouse::i_drag.y += mouse::main_mouse.y - mouse::last_mouse.y;
		dragging = true;
	}

	int amount = 0;

	if (c_cs_player::get_local_player() && c_cs_player::get_local_player()->is_alive()) {
		if (config->Ragebot.fakelag_enable && config->Ragebot.enable)
			amount++;
		amount++;
	}

	// move container
	position.x -= mouse::i_drag.x;
	position.y -= mouse::i_drag.y;

	float dheight = (2 + amount) * 20 + 5;
	std::string spec_text = "indicators";
	Drawing::DrawRect(position.x + 5, position.y + 5, 200 - 10, dheight - 10 - 5, global_color);
	Drawing::DrawString(F::ESP, position.x + 100, position.y + 6, dragging ? t_selected_color : c_color(255, 255, 255, 155), FONT_CENTER, spec_text.c_str());
	Drawing::DrawRect(position.x + 8, position.y + 23, 200 - 16, 3, c_color(config->Misc.menucolor[0] * 255, config->Misc.menucolor[1] * 255, config->Misc.menucolor[2] * 255, 255));
	
	if (!c_cs_player::get_local_player())
		return;
	if (!c_cs_player::get_local_player()->is_alive())
		return;

	position.y += 30;

	if (config->Ragebot.fakelag_enable && config->Ragebot.enable) { // packet
		static float value = 0.f;
		float newvalue = antiaim->get_visual_choke();
		value = lerp(ind_speed, value, newvalue);
		draw_progressbar(position, "packets", c, value);
	}
	{ // local velocity
		static float value = 0.f;
		static auto maxvel = cvar()->find_var("sv_maxvelocity");

		if (maxvel) {

			float newvalue = std::clamp(c_cs_player::get_local_player()->get_velocity().length2d(), 0.f, 300.f) / 300.f; /* FOR NOW cause more retarded servers do max vel at 3500*/
			value = lerp(ind_speed, value, newvalue);
			draw_progressbar(position, "velocity", c, value);
		}
	}
}

void c_esp::draw_spectators() {

	if (!config->Visuals.spec_list)
		return;

	auto people_spec = 0;
	auto spec_modes = 0;
	std::string current_spectators[25];
	std::string current_modes[65];
	const auto local = c_cs_player::get_local_player();

	int w, h;
	engine_client()->get_screen_size(w, h);

	c_vector2d position = c_vector2d(w - 200, (h / 2) * 0.9);


	static bool dragging_container = false;
	bool dragging = false;
	if (mouse::main_mouse.x > position.x - mouse::s_drag.x && mouse::main_mouse.y > position.y - mouse::s_drag.y && mouse::main_mouse.x < (position.x - mouse::s_drag.x) + 200 && mouse::main_mouse.y < (position.y - mouse::s_drag.y) + 20 && GetAsyncKeyState(VK_LBUTTON) && menu->open) {

		mouse::s_drag.x += mouse::main_mouse.x - mouse::last_mouse.x;
		mouse::s_drag.y += mouse::main_mouse.y - mouse::last_mouse.y;
		dragging = true;
	}


	// move container
	position.x -= mouse::s_drag.x;
	position.y -= mouse::s_drag.y;

	float height = (people_spec + 2) * 20 + 5;

	if (config->Visuals.indicators_styles == 1)
		Drawing::DrawRect(position.x + 5, position.y + 5, 200 - 10, height - 10 - 5, global_color);

	std::string spec_text = "spectators";

	if (config->Visuals.indicators_styles == 0 && menu->open) {
		auto re_size = Drawing::GetTextSize(F::ESP, spec_text.c_str());
		Drawing::DrawString(F::ESP, position.x + 100, position.y + 6, dragging ? t_selected_color : c_color(255, 255, 255, 155), FONT_CENTER, spec_text.c_str());
	}
	else if (config->Visuals.indicators_styles == 1)
		Drawing::DrawString(F::ESP, position.x + 100, position.y + 6, dragging ? t_selected_color : c_color(255, 255, 255, 155), FONT_CENTER, spec_text.c_str());

	if (config->Visuals.indicators_styles == 1)
		Drawing::DrawRect(position.x + 8, position.y + 23, 200 - 16, 3, c_color(config->Misc.menucolor[0] * 255, config->Misc.menucolor[1] * 255, config->Misc.menucolor[2] * 255, 255));

	if (!local || !local->is_alive())
		return;

	for (int i = 0; i < engine_client()->get_max_clients(); i++) {

		auto player = (c_cs_player*)client_entity_list()->get_client_entity(i);

		if (!player)
			continue;

		auto observer_target = (c_cs_player*)client_entity_list()->get_client_entity_from_handle(player->m_hObserverTarget());

		if (player->is_alive() || player == local || player->is_dormant() || observer_target != local)
			continue;

		auto info = player->get_info();

		if (info.ishltv)
			continue;

		current_spectators[people_spec] = info.name;

		switch (player->get_observer_mode()) {
		case c_cs_player::obeserver_modes::OBS_MODE_IN_EYE:
			current_modes[people_spec] = "person"; break;
		case c_cs_player::obeserver_modes::OBS_MODE_CHASE:
			current_modes[people_spec] = "chase"; break;
		case c_cs_player::obeserver_modes::OBS_MODE_ROAMING:
			current_modes[people_spec] = "roaming"; break;
		case c_cs_player::obeserver_modes::OBS_MODE_DEATHCAM:
			current_modes[people_spec] = "death"; break;
		case c_cs_player::obeserver_modes::OBS_MODE_FREEZECAM:
			current_modes[people_spec] = "freeze"; break;
		case c_cs_player::obeserver_modes::OBS_MODE_FIXED:
			current_modes[people_spec] = "fixed"; break;
		}
		people_spec++;
	}

	if (config->Visuals.indicators_styles == 1)
		position.y += 30;

	if (menu->open)
		return;

	for (int i = 0; i < people_spec; i++) {
		if (config->Visuals.indicators_styles == 1) {
			Drawing::DrawRect(position.x + 8, position.y, 184, 20, c_color(32, 32, 32, 255));
			Drawing::DrawRect(position.x + 9, position.y, 182, 20, c_color(35, 35, 45, 255));
			Drawing::DrawRect(position.x + 9, position.y, 1, 20, c_color(config->Misc.menucolor[0] * 255, config->Misc.menucolor[1] * 255, config->Misc.menucolor[2] * 255, 255));
		}
		auto size = Drawing::GetTextSize(F::ESP, current_spectators[i].c_str());
		int width = size.right;
		static int i_size = 10;
		if (width > 50) {
			i_size = 5;
		}
		if (config->Visuals.indicators_styles == 1) {
			Drawing::DrawString(F::ESP, position.x + 14, position.y + 2, c_color(255, 255, 255, 155), false, current_spectators[i].substr(0, i_size).c_str());
			Drawing::DrawString(F::ESP, position.x + 110, position.y + 2, c_color(255, 255, 255, 155), false, current_modes[i].c_str());
			Drawing::DrawString(F::ESP, position.x + 175, position.y + 2, c_color(255, 255, 255, 155), false, std::to_string(i + 1).c_str());
		}
		else {
			auto re_size = Drawing::GetTextSize(F::ESP, current_spectators[i].substr(0, i_size).c_str());
			Drawing::DrawString(F::ESP, position.x - (re_size.right / 2) + 100, position.y + 6, c_color(255, 255, 255, 155), false, current_spectators[i].substr(0, i_size).c_str());
		}
		position.y += size.bottom + 2;
	}


}

struct damage_infos {
	c_cs_player* player{};
	char name[25]{};
	int health_before{};
	int damage{};
	int health_after{};
	int last{};
	int shots{};
	int miss{};
	int lag{};
	bool killed{};
	std::string method;
	std::string hitbox;
};
std::vector<damage_infos> dmg_info;

void c_esp::clear_dmg() {


	if (!config->Visuals.round_summ)
		return;

	auto local = c_cs_player::get_local_player();

	if (!local || !local->is_alive())
		return;

	if (dmg_info.size() > 0)
		dmg_info.clear();

	dmg_info.clear();
}

void c_esp::draw_dmg() {

	if (!config->Visuals.round_summ) {
		return;
	}

	auto draw_text = [](const char* text, c_vector2d pos, int x_offset, c_color Color) {
		Drawing::DrawString(F::ESP, pos.x + x_offset, pos.y + 2, Color,  false, text);
	};

	int w, h;
	engine_client()->get_screen_size(w, h);
	c_vector2d position = c_vector2d(0.f, ((h / 2) * 1.2));

	std::string gap = " | ";
	std::string showcase = "name" + gap + "damage" + gap + "hitbox" + gap + "ticks"; // + gap + method;

	auto size = Drawing::GetTextSize(F::ESP, showcase.c_str());
	int width = size.right, height = size.top;


	bool dragging = false;

	if (mouse::main_mouse.x > position.x - mouse::d_drag.x && mouse::main_mouse.y > position.y - mouse::d_drag.y && mouse::main_mouse.x < (position.x - mouse::d_drag.x) + width && mouse::main_mouse.y < (position.y - mouse::d_drag.y) + 20 && GetAsyncKeyState(VK_LBUTTON) && menu->open) {

		mouse::d_drag.x += mouse::main_mouse.x - mouse::last_mouse.x;
		mouse::d_drag.y += mouse::main_mouse.y - mouse::last_mouse.y;
		dragging = true;
	}

	// move container
	position.x -= mouse::d_drag.x;
	position.y -= mouse::d_drag.y;

	float dheight = ((dmg_info.size() > 5 ? 5 : dmg_info.size()) + 2) * 20 + 5;

	if (config->Visuals.indicators_styles == 1) 
	Drawing::DrawRect(position.x + 5, position.y + 5, 200 - 10, dheight - 10 - 5, global_color);

	std::string spec_text = "round summary";
	if (config->Visuals.indicators_styles == 0 && menu->open) {
		Drawing::DrawString(F::ESP, position.x + 100, position.y + 6, dragging ? t_selected_color : c_color(255, 255, 255, 155), FONT_CENTER, spec_text.c_str());
		Drawing::DrawString(F::ESP, position.x + 100, position.y + 30, dragging ? t_selected_color : c_color(255, 255, 255, 155), FONT_CENTER, showcase.c_str());

	}
	else if (config->Visuals.indicators_styles ==  1)
		Drawing::DrawString(F::ESP, position.x + 100, position.y + 6, dragging ? t_selected_color : c_color(255, 255, 255, 155), FONT_CENTER, spec_text.c_str());

	if (config->Visuals.indicators_styles == 1) 
		Drawing::DrawRect(position.x + 8, position.y + 23, 200 - 16, 3, c_color(config->Misc.menucolor[0] * 255, config->Misc.menucolor[1] * 255, config->Misc.menucolor[2] * 255, 255));

	if (dmg_info.empty() || dmg_info.size() <= 0) {
		return;
	}
	auto local = c_cs_player::get_local_player();
	if (!local || local->get_flags() & c_cs_player::flags::frozen) {
		clear_dmg();
		return;
	}

	position.y += 30;

	auto it = dmg_info.rbegin();

	if (menu->open)
		return;

	for (int i = 0; it != dmg_info.rend() && i < 5; i++, it++) {
			
		const char* name = it->name;
		std::string damage = std::to_string(it->damage);
		std::string hitbox = it->hitbox;
		std::string method = it->method;
		int ticks = it->lag;
		if (config->Visuals.indicators_styles == 1) {
			Drawing::DrawRect(position.x + 8, position.y, 184, 20, c_color(32, 32, 32, 255));
			Drawing::DrawRect(position.x + 9, position.y, 182, 20, c_color(35, 35, 45, 255));
			Drawing::DrawRect(position.x + 9, position.y, 1, 20, c_color(config->Misc.menucolor[0] * 255, config->Misc.menucolor[1] * 255, config->Misc.menucolor[2] * 255, 255));
		}

		Drawing::DrawString( F::ESP,position.x + 14, position.y + 2, c_color(255, 255, 255, 155),false, name);
		Drawing::DrawString( F::ESP,position.x + 80, position.y + 2, c_color(255, 255, 255, 155),false, damage.c_str());
		Drawing::DrawString( F::ESP,position.x + 115, position.y + 2, c_color(255, 255, 255, 155) ,false, hitbox.c_str());
		Drawing::DrawString( F::ESP,position.x + 165, position.y + 2, c_color(255, 255, 255, 155), false, std::to_string(ticks).c_str());

		position.y += 20;
	}
}

void c_esp::adddmg(c_game_event* event) {


	if (!event || !config->Visuals.round_summ)
		return;

	auto local = c_cs_player::get_local_player();

	if (!local || !local->is_alive())
		return;

	const auto attacker = client_entity_list()->get_client_entity(engine_client()->get_player_for_user_id(event->get_int(_("attacker"))));
	const auto target = reinterpret_cast<c_cs_player*>(client_entity_list()->get_client_entity(engine_client()->get_player_for_user_id(event->get_int(_("userid")))));

	if (attacker && target && attacker == local && target->is_enemy())
	{
		damage_infos toadd;

		const auto latest = animation_system->get_latest_animation(target);

		if (latest.has_value()) {
			toadd.lag = latest.value()->lag;
		}
		else
			toadd.lag = FALSE;

		toadd.player = target;

		memcpy(toadd.name, target->get_info().name, 9);
		toadd.name[8] = '\0';

		auto size = Drawing::GetTextSize(F::ESP, toadd.name);
		int width = size.right;

		if (width > 50) {
			memcpy(toadd.name, target->get_info().name, 5);
			toadd.name[4] = '\0';
		}

		toadd.health_before = toadd.player->get_health();
		toadd.damage = event->get_int(_("dmg_health"));
		const auto hitgroup = event->get_int("hitgroup");

		switch (hitgroup)
		{
		case hitgroup_generic:
			toadd.hitbox = "generic";
			break;
		case hitgroup_head:
			toadd.hitbox = "head";
			break;
		case hitgroup_chest:
			toadd.hitbox = "chest";
			break;
		case hitgroup_stomach:
			toadd.hitbox = "stomach";
			break;
		case hitgroup_leftarm:
			toadd.hitbox = "arm";
			break;
		case hitgroup_rightarm:
			toadd.hitbox = "arm";
			break;
		case hitgroup_leftleg:
			toadd.hitbox = "leg";
		case hitgroup_rightleg:
			toadd.hitbox = "leg";
			break;
		case hitgroup_gear:
			toadd.hitbox = "gear";
			break;
		default:
			toadd.hitbox = "body";
			break;
		}

		switch (resolver.resolver_method[target->index()]) {
		case 0: toadd.method = "real"; break;
		case 1: toadd.method = "slow lby"; break;
		case 2: toadd.method = "old"; break;
		case 3: toadd.method = "lag"; break;
		case 4: toadd.method = "slowwalk"; break;
		case 5: toadd.method = "lby"; break;
		}

		dmg_info.push_back(toadd);
	}
}

void c_esp::esp_keybinds() {

	const auto local = c_cs_player::get_local_player();

	if (!config->Visuals.keybinds || config->Visuals.indicators_styles == 0)
		return;

	int w, h;
	engine_client()->get_screen_size(w, h);
	c_vector2d pos = c_vector2d(0, (h / 2) * 1.07);
	bool dragging = false;
	if (mouse::main_mouse.x > pos.x - mouse::k_drag.x && mouse::main_mouse.y > pos.y - mouse::k_drag.y && mouse::main_mouse.x < (pos.x - mouse::k_drag.x) + 200 && mouse::main_mouse.y < (pos.y - mouse::k_drag.y) + 20 && GetAsyncKeyState(VK_LBUTTON) && menu->open) {

		mouse::k_drag.x += mouse::main_mouse.x - mouse::last_mouse.x;
		mouse::k_drag.y += mouse::main_mouse.y - mouse::last_mouse.y;
		dragging = true;
	}

	// move container
	pos.x -= mouse::k_drag.x;
	pos.y -= mouse::k_drag.y;

	int amount = 0;

	if (GetAsyncKeyState(config->Misc.slowwalk_key) && config->Misc.slowwalk)
		amount++;
	if (GetAsyncKeyState(config->Ragebot.fakeduck) && config->Ragebot.fakeduck_enable)
		amount++;
	if (GetAsyncKeyState(config->Visuals.thirdperson) && config->Visuals.thirdperson_enable)
		amount++;
	if (antiaim->_manual_yaw == -90 ? true : false && config->Ragebot.manaul_override)
		amount++;
	if (antiaim->_manual_yaw == 90 ? true : false && config->Ragebot.manaul_override)
		amount++;
	if (antiaim->is_freestanding && config->Ragebot.freestanding_enable)
		amount++;
	if (config->Misc.airstuck_enable && GetAsyncKeyState(config->Misc.airstuck_key))
		amount++;
	if (config->Ragebot.desync_invert_enable && GetAsyncKeyState(config->Ragebot.desync_invert))
		amount++;

	float dheight = (amount + 2) * 20 + 5;
	if (config->Visuals.indicators_styles == 1) 
		Drawing::DrawRect(pos.x + 5, pos.y + 5, 200 - 10, dheight - 10 - 5, global_color);

	std::string spec_text = "keybinds";
	Drawing::DrawString(F::ESP, pos.x + 100, pos.y + 6, dragging ? t_selected_color : c_color(255, 255, 255, 155), FONT_CENTER, spec_text.c_str());
	if (config->Visuals.indicators_styles == 1) 
		Drawing::DrawRect(pos.x + 8, pos.y + 23, 200 - 16, 3, c_color(config->Misc.menucolor[0] * 255, config->Misc.menucolor[1] * 255, config->Misc.menucolor[2] * 255, 255));


	pos.y += 30;

	if (!local || !local->is_alive())
		return;

	auto keybind = [&](const char* text, int key) -> void {

		auto size = Drawing::GetTextSize(F::ESP, text);
		int temp_width = size.right, temp_height = size.top;

		if (GetAsyncKeyState(key)) {

			auto size2 = Drawing::GetTextSize(F::ESP, text);
			int width = size.right, height = size.top;
			if (config->Visuals.indicators_styles == 1) {
				Drawing::DrawRect(pos.x + 8, pos.y, 184, 20, c_color(32, 32, 32, 255));
				Drawing::DrawRect(pos.x + 9, pos.y, 182, 20, c_color(35, 35, 45, 255));
				Drawing::DrawRect(pos.x + 9, pos.y, 1, 20, c_color(config->Misc.menucolor[0] * 255, config->Misc.menucolor[1] * 255, config->Misc.menucolor[2] * 255, 255));
			}
			Drawing::DrawString(F::ESP, pos.x + 14, pos.y + 2, c_color(255, 255, 255, 155), false, text);

			pos.y += 20;
		}
	};

	auto keybind_ = [&](const char* text, bool key) -> void {

		auto size = Drawing::GetTextSize(F::ESP, text);
		int temp_width = size.right, temp_height = size.top;


		if (key) {

			auto size2 = Drawing::GetTextSize(F::ESP, text);
			int width = size.right, height = size.top;
			if (config->Visuals.indicators_styles == 1) {
				Drawing::DrawRect(pos.x + 8, pos.y, 184, 20, c_color(32, 32, 32, 255));
				Drawing::DrawRect(pos.x + 9, pos.y, 182, 20, c_color(35, 35, 45, 255));
				Drawing::DrawRect(pos.x + 9, pos.y, 1, 20, c_color(config->Misc.menucolor[0] * 255, config->Misc.menucolor[1] * 255, config->Misc.menucolor[2] * 255, 255));
			}
			Drawing::DrawString(F::ESP, pos.x + 14, pos.y + 2, c_color(255, 255, 255, 155), false, text);


			pos.y += 20;

		}
	};

	if (config->Misc.slowwalk)	keybind("slowalk", config->Misc.slowwalk_key);
	if (config->Ragebot.fakeduck_enable) keybind("fakeduck", config->Ragebot.fakeduck);
	keybind("body aim", config->Ragebot.body_aim_key);
	if (config->Misc.airstuck_enable) keybind("airstuck", config->Misc.airstuck_key);
	if (config->Visuals.thirdperson_enable) keybind("thirdperson", config->Visuals.thirdperson);
	if (config->Ragebot.desync_invert_enable) keybind("desync invert", config->Ragebot.desync_invert);

	if (config->Ragebot.manaul_override) keybind_("manual left", antiaim->_manual_yaw == -90 ? true : false);
	if (config->Ragebot.manaul_override) keybind_("manual back", antiaim->_manual_yaw == 0.1 ? true : false);
	if (config->Ragebot.manaul_override) keybind_("manual right", antiaim->_manual_yaw == 90 ? true : false);
	if (config->Ragebot.freestanding_enable) keybind_("freestand", antiaim->is_freestanding);

}

void c_esp::render_watermark() {

	if (!config->Misc.watermark)
		return;

	char buffer[255];
	char timebuff[80];

	time_t now = time(0);   // get time now
	struct tm  tstruct;
	tstruct = *localtime(&now);

	const auto net = net_channel;
	float rtt = 0.f;

	if (net != nullptr)
		rtt = net->get_latency(0) * 2000.f;

	strftime(timebuff, sizeof(timebuff), "%X", &tstruct);

	sprintf(buffer, _("LAGSYNC.vip | rtt: %.0fms | rate: %.0f | %s"), rtt, 1.f / global_vars_base->interval_per_tick, timebuff);

	const auto watermark_size = Drawing::GetTextSize(F::Menu, buffer);

	int w, h;
	engine_client()->get_screen_size(w, h);

	Drawing::DrawString(F::Menu, w - watermark_size.right - 15.f, 11.f, c_color(0,255,255,170),false, buffer);
}

void c_esp::draw_impact(c_vector3d start, c_vector3d end, c_color color)
{
	beam_info info;

	info.type = 0;
	info.model_name = ("sprites/purplelaser1.vmt");
	info.model_index = -1;
	info.halo_scale = 0.f;
	info.life = 3.f;
	info.width = 1.61803398875f;
	info.end_width = 1.61803398875f / 1.61803398875f;
	info.fade_length = 0.f;
	info.amplitude = 2.f;
	info.brightness = 255.f;
	info.speed = .2f;
	info.start_frame = 0;
	info.frame_rate = 0.f;
	info.red = float(color.red);
	info.green = float(color.green);
	info.blue = float(color.blue);
	info.segments = 2;
	info.renderable = true;
	info.flags = 0;
	info.start = start;
	info.end = end;

	auto beam = view_render_beams->create_beam_points(info);

	if (beam)
		view_render_beams->draw_beam(beam);
}

void c_esp::draw_enemy_impact(c_game_event* event)
{
	auto local = c_cs_player::get_local_player();

	if ((!config->Visuals.bulletbeams && !config->Visuals.e_bulletbeams) || !local) {
		return;
	}

	const auto attacker = reinterpret_cast<c_cs_player*>(client_entity_list()->get_client_entity(engine_client()->get_player_for_user_id(event->get_int(_("userid")))));
	c_vector3d start = attacker->get_shoot_position();

	c_color col;

	if (attacker->is_enemy()) {
		if (!config->Visuals.e_bulletbeams) {
			return;
		}
		col = ARRAY_TO_CCOLOR(config->Colors.ebeamcolor);
	}
	else if (attacker == local) {
		if (!config->Visuals.bulletbeams) {
			return;
		}
		col = ARRAY_TO_CCOLOR(config->Colors.beamcolor);
	}
	else if (!attacker->is_enemy()) {
		return;
	}
	c_vector3d end = c_vector3d(event->get_float("x"), event->get_float("y"), event->get_float("z") * 1.05f);
	draw_impact(start, end, col);

	if (config->Visuals.showimpacts && attacker->is_enemy()) {
		debug_overlay()->AddBoxOverlay(end, c_vector3d(-2, -2, -2), c_vector3d(2, 2, 2), c_qangle(),
			col.red, col.green, col.blue, 255, 2.f);
	}
	if (config->Visuals.showimpacts && attacker == local) {
		debug_overlay()->AddBoxOverlay(end, c_vector3d(-2, -2, -2), c_vector3d(2, 2, 2), c_qangle(),
			col.red, col.green, col.blue, 255, 2.f);
	}
}

void draw_beam(c_vector3d Start, c_vector3d End, c_color color, float Width)
{
	if (!c_cs_player::get_local_player() || !c_cs_player::get_local_player()->is_alive())
		return;

	beam_info info;

	info.type = 0;
	info.model_name = "sprites/purplelaser1.vmt";
	info.model_index = -1;
	info.halo_scale = 0.f;
	info.life = 2.5f;
	info.width = Width;
	info.end_width = 1;
	info.fade_length = 0.f;
	info.amplitude = 2.f;
	info.brightness = float(color.alpha);
	info.speed = .2f;
	info.start_frame = 0;
	info.frame_rate = 0.f;
	info.red = float(color.red);
	info.green = float(color.green);
	info.blue = float(color.blue);
	info.segments = 2;
	info.renderable = true;
	info.flags = 0;
	info.start = Start;
	info.end = End;

	auto beam = view_render_beams->create_beam_points(info);

	if (beam)
		view_render_beams->draw_beam(beam);
}

void c_esp::draw_trails(c_cs_player* local)
{
	if (!config->Visuals.trails || !local)
	{
		return;
	}

	static c_vector3d old_origin = local->get_abs_origin();
	const auto origin = local->get_abs_origin() + c_vector3d(0, 0, 2);

	auto start = old_origin, end = origin;
	c_color color = c_color(config->Colors.trails_color[0] * 255, config->Colors.trails_color[1] * 255, config->Colors.trails_color[2] * 255, config->Colors.trails_color[3] * 255);
	constexpr auto width = 1.6f;

	if (old_origin != origin && local->is_alive())
		draw_beam(start, end, color, width);

	old_origin = origin;
}

void c_esp::draw_firstperson_indicators()
{

	if (!c_cs_player::get_local_player() || !engine_client()->is_ingame() || !engine_client()->is_connected())
		return;

	if (!config->Ragebot._antiaim)
		return;

	if (config->Visuals.aaindicator) {

		const c_color selected_color = c_color(config->Colors.f_aa_ind[0] * 255, config->Colors.f_aa_ind[1] * 255, config->Colors.f_aa_ind[2] * 255, config->Colors.f_aa_ind[3] * 255);// _config->Colors.f_aa_ind[3])
		const c_color disabled_color = c_color(config->Colors.f_aa_ind2[0] * 255, config->Colors.f_aa_ind2[1] * 255, config->Colors.f_aa_ind2[2] * 255, config->Colors.f_aa_ind2[3] * 255);// _config->Colors.f_aa_ind[3])

		static auto draw_arrow = [](float angle, c_color color) {

			c_vector2d pos[8] =
			{
				{ -7.f, -50.f },
				{ -7.f, -140.f },

				{ 7.f, -50.f },
				{ 7.f, -140.f },

				{ -20.f, -130.f },
				{ 0.f, -160.f },

				{ 20.f, -130.f },
				{ 0.f, -160.f }
			};

			for (auto& p : pos)
			{
				const auto s = sin(angle);
				const auto c = cos(angle);

				int w, h;
				engine_client()->get_screen_size(w, h);
				p = c_vector2d(p.x * c - p.y * s, p.x * s + p.y * c) + c_vector2d(w / 2, h / 2);
			}

			Drawing::DrawLine(pos[4].x, pos[4].y, pos[6].x, pos[6].y, color);
			Drawing::DrawLine(pos[4].x, pos[4].y, pos[5].x, pos[5].y, color);
			Drawing::DrawLine(pos[6].x, pos[6].y, pos[7].x, pos[7].y, color);

		};

		const auto yaw = engine_client()->get_view_angles().y;
		const auto real = deg2rad(yaw - antiaim->get_last_real());
		const auto fake = deg2rad(yaw - antiaim->get_last_fake());

		draw_arrow(real, selected_color);
		draw_arrow(fake, disabled_color);
	}
}

void c_esp::draw_thirdperson_indicators()
{

	if (!engine_client()->is_ingame() || !engine_client()->is_connected() || !c_cs_player::get_local_player() || !c_cs_player::get_local_player()->is_alive())
		return;

	if (!config->Ragebot._antiaim)
		return;

	if (config->Visuals.thirdaaindicator)
	{
		c_vector2d src, dst;
		c_vector3d dst3D, src3D, forward;

		float duration = global_vars_base->interval_per_tick * 2;

		game_trace tr;
		ray r;
		c_trace_filter filter;

		filter.skip_entity = c_cs_player::get_local_player();

		const c_color real_color = c_color(config->Colors.t_aa_ind[0] * 255, config->Colors.t_aa_ind[1] * 255, config->Colors.t_aa_ind[2] * 255, config->Colors.t_aa_ind[3] * 255);// _config->Colors.f_aa_ind[3])
		const c_color fake_color = c_color(config->Colors.t_aa_ind2[0] * 255, config->Colors.t_aa_ind2[1] * 255, config->Colors.t_aa_ind2[2] * 255, config->Colors.t_aa_ind2[3] * 255);// _config->Colors.f_aa_ind[3])

		/*REAL_ANGLE */
		math::angle_vectors(c_vector3d(0, antiaim->get_last_real(), 0), forward);
		src3D = c_cs_player::get_local_player()->get_origin();

		dst3D = src3D + (forward * 25.f);

		r.init(src3D, dst3D);
		engine_trace()->trace_ray(r, 0, &filter, &tr);
		debug_overlay()->AddLineOverlay(src3D, tr.endpos, real_color.red, real_color.green, real_color.blue, 1, duration);
		debug_overlay()->AddSphereOverlay(tr.endpos, 3.f, 10, 10, real_color.red, real_color.green, real_color.blue, real_color.alpha, duration);

		/*DESYC_ANGLE*/
		math::angle_vectors(c_vector3d(0, antiaim->get_last_fake(), 0), forward);
		dst3D = src3D + (forward * 25.f);

		r.init(src3D, dst3D);

		engine_trace()->trace_ray(r, 0, &filter, &tr);
		debug_overlay()->AddLineOverlay(src3D, tr.endpos, fake_color.red, fake_color.green, fake_color.blue, 1, duration);
		debug_overlay()->AddSphereOverlay(tr.endpos, 3.f, 10, 10, fake_color.red, fake_color.green, fake_color.blue, fake_color.alpha, duration);

	}
}

void c_esp::esp_no_scope()
{

	if (!c_cs_player::get_local_player() || !config->Visuals.noscope || config->Visuals.enchance_scope)
		return;

	int screen_x;
	int screen_y;
	int center_x;
	int center_y;
	engine_client()->get_screen_size(screen_x, screen_y);
	engine_client()->get_screen_size(center_x, center_y);
	center_x /= 2; center_y /= 2;

	if (c_cs_player::get_local_player()->is_scoped())
	{
		Drawing::DrawLine(0, center_y, screen_x, center_y, c_color(0, 0, 0, 255));
		Drawing::DrawLine(center_x, 0, center_x, screen_y, c_color(0, 0, 0, 255));
	}
}

void c_esp::fsn_nightmode() {

	static bool skybox_toggle = false;
	static float oldworld_data[4];
	static float oldprop_data[4];
	static float oldsky_data[4];
	static int OldSky = 0;
	static std::string old_Skyname = "";
	static convar* r_DrawSpecificStaticProp;

	if (!engine_client()->is_ingame() || !engine_client()->is_connected() || !c_cs_player::get_local_player())
	{
		old_Skyname = "";
		OldSky = 0;
		oldworld_data[0] = { 1.f }; oldworld_data[1] = { 1.f }; oldworld_data[2] = { 1.f }; oldworld_data[3] = { 1.f };
		oldprop_data[0] = { 1.f }; oldprop_data[1] = { 1.f }; oldprop_data[2] = { 1.f }; oldprop_data[3] = { 1.f };
		oldsky_data[0] = { 1.f }; oldsky_data[1] = { 1.f }; oldsky_data[2] = { 1.f }; oldsky_data[3] = { 1.f };
	}

	const float world_data[4] = { config->Colors.nightcolor[0], config->Colors.nightcolor[1], config->Colors.nightcolor[2], config->Colors.nightcolor[3] };
	const float prop_data[4] = { config->Colors.propcolor[0], config->Colors.propcolor[1], config->Colors.propcolor[2], config->Colors.propcolor[3] };
	const float sky_data[4] = { config->Colors.skycolor[0], config->Colors.skycolor[1], config->Colors.skycolor[2], 1.f };

	if (engine_client()->is_ingame() && engine_client()->is_connected() && c_cs_player::get_local_player()) {
		if ((world_data[0] != oldworld_data[0] || world_data[1] != oldworld_data[1] || world_data[2] != oldworld_data[2] || world_data[3] != oldworld_data[3]))
		{
			for (MaterialHandle_t i = material_system()->FirstMaterial(); i != material_system()->InvalidMaterial(); i = material_system()->NextMaterial(i))
			{
				IMaterial* pMaterial = material_system()->GetMaterial(i);
				if (!pMaterial)
					continue;
				if (strstr(pMaterial->GetTextureGroupName(), "World"))
				{
					pMaterial->ColorModulate(world_data[0], world_data[1], world_data[2]);
					pMaterial->AlphaModulate(world_data[3]);
				}
			}
			memcpy(oldworld_data, world_data, sizeof(world_data));
		}
		if ((prop_data[0] != oldprop_data[0] || prop_data[1] != oldprop_data[1] || prop_data[2] != oldprop_data[2] || prop_data[3] != oldprop_data[3]))
		{
			r_DrawSpecificStaticProp = cvar()->find_var(("r_DrawSpecificStaticProp"));
			r_DrawSpecificStaticProp->flags &= ~(1 << 14);
			r_DrawSpecificStaticProp->set_value(0);
			for (MaterialHandle_t i = material_system()->FirstMaterial(); i != material_system()->InvalidMaterial(); i = material_system()->NextMaterial(i))
			{
				IMaterial* pMaterial = material_system()->GetMaterial(i);
				if (!pMaterial)
					continue;
				if (strstr(pMaterial->GetTextureGroupName(), "StaticProp"))
				{
					pMaterial->ColorModulate(prop_data[0], prop_data[1], prop_data[2]);
					pMaterial->AlphaModulate(prop_data[3]);
				}
			}
			memcpy(oldprop_data, prop_data, sizeof(prop_data));
		}
		if ((sky_data[0] != oldsky_data[0] || sky_data[1] != oldsky_data[1] || sky_data[2] != oldsky_data[2] || sky_data[3] != oldsky_data[3]))
		{
			for (MaterialHandle_t i = material_system()->FirstMaterial(); i != material_system()->InvalidMaterial(); i = material_system()->NextMaterial(i))
			{
				IMaterial* pMaterial = material_system()->GetMaterial(i);
				if (!pMaterial)
					continue;
				if (strstr(pMaterial->GetTextureGroupName(), "SkyBox"))
				{
					pMaterial->ColorModulate(sky_data[0], sky_data[1], sky_data[2]);
					pMaterial->AlphaModulate(sky_data[3]);
				}
			}
			memcpy(oldsky_data, sky_data, sizeof(sky_data));
		}

		static convar* fullbright;
		fullbright = cvar()->find_var(("mat_fullbright"));
		fullbright->flags &= ~(1 << 14);
		fullbright->set_value(config->Visuals.fullbright);

		// to lazy to make another skybox heres venus' lazy method

		if (OldSky != config->Visuals.skyboxes)
		{
			auto LoadNamedSky = reinterpret_cast<void(__fastcall*)(const char*)>(sig("engine.dll", "55 8B EC 81 EC ? ? ? ? 56 57 8B F9 C7 45"));
			if (old_Skyname == "")
			{
				//old_Skyname = g_pCvar->FindVar("sv_skyname")->GetName();
			}
			int type = config->Visuals.skyboxes;
			/*if(type == 0)
			LoadNamedSky(old_Skyname.c_str());
			else */if (type == 1)
				LoadNamedSky("cs_baggage_skybox_");
			else if (type == 2)
				LoadNamedSky("cs_tibet");
			else if (type == 3)
				LoadNamedSky("italy");
			else if (type == 4)
				LoadNamedSky("jungle");
			else if (type == 5)
				LoadNamedSky("office");
			else if (type == 6)
				LoadNamedSky("sky_cs15_daylight02_hdr");
			else if (type == 7)
				LoadNamedSky("sky_csgo_night02");
			else if (type == 8)
				LoadNamedSky("vertigo");
			else if (type == 9)
				LoadNamedSky("sky_descent");
			else if (type == 10)
				LoadNamedSky("sky081");
			OldSky = config->Visuals.skyboxes;
		}
	}
}

void c_esp::draw_penetration_crosshair()
{
	if (!config->Visuals.penetration_crosshair)
		return;

	const auto local = c_cs_player::get_local_player();
	if (!local)
		return;

	float damage = 0.f;

	int sizex = 0;
	int sizey = 0;

	engine_client()->get_screen_size(sizex, sizey);

	auto sqaure = [](c_color color, int x, int y) -> void {
		Drawing::DrawRect(x - 1, y - 1, 2.f, 2.f, color);

	};

	if (trace_system->can_wallbang(damage))
		sqaure(c_color(0, 255, 0, 255), sizex / 2, sizey / 2);
	else
		sqaure(c_color(255, 0, 0, 255), sizex / 2, sizey / 2);
}

void c_esp::draw_molotov_timer() {

	if (!config->Visuals.nade_style_mol)
		return;

	static const std::string name = "molotov";
	const c_color color = c_color(255, 255, 255, 255);//_config->Colors.molotimer[0] * 255, _config->Colors.molotimer[1] * 255, _config->Colors.molotimer[2] * 255, _config->Colors.molotimer[3] * 255);

	client_entity_list()->for_every_entity([](c_cs_player* entity) -> void {	

		auto inferno = reinterpret_cast <c_cs_inferno*> (entity); 
		
		const auto origin = inferno->get_abs_origin();
		auto get_w2s_origin = c_vector3d{};

		if (!world_to_screen(origin, get_w2s_origin))
			return;

		const auto spawn_time = inferno->get_entity_spawn_time();
		static constexpr float max_time = 7.03125;
		const auto actual = (((spawn_time + max_time) - global_vars_base->curtime) / max_time);
		static auto size = c_vector2d( 70, 3 );
		const auto pos = c_vector2d(get_w2s_origin.x - size.x * 0.5, get_w2s_origin.y - size.y * 0.5);
		c_color background = c_color(35, 35, 35, config->Colors.nadecolorr[3] * 255);
		c_color bar = c_color(config->Colors.nadecolorr[0] * 255, config->Colors.nadecolorr[1] * 255, config->Colors.nadecolorr[2] * 255, config->Colors.nadecolorr[3] * 255);
		c_color text = c_color( 255, 255, 255, config->Colors.nadecolorr[3] * 255);

		if (config->Visuals.draw_nades == 0)
			Drawing::DrawString(F::ESPInfo, pos.x + 32, pos.y - 20, text, FONT_CENTER, name.c_str());
		else
			Drawing::DrawString(F::ESPWeapon, pos.x + 32, pos.y - 20, text, FONT_CENTER, "l");

		Drawing::DrawRect(pos.x, pos.y, size.x, size.y, background);
		Drawing::DrawRect(pos.x + 1, pos.y + 1, (size.x - 2) * actual, size.y - 2, bar);		
		//Drawing::DrawString((pos.x + 1)* actual, (pos.y + 1), text,  F::ESPInfo, true, std::to_string(actual).c_str());

	}, class_id::cinferno);

}

void c_esp::draw_smoke_timer() {

	if (!config->Visuals.nade_style_smoke)
		return;

	static const std::string name = "smoke";
	const c_color color = c_color(255, 255, 255, 255);//_config->Colors.molotimer[0] * 255, _config->Colors.molotimer[1] * 255, _config->Colors.molotimer[2] * 255, _config->Colors.molotimer[3] * 255);

	client_entity_list()->for_every_entity([](c_cs_player* entity) -> void {

		auto smoke = reinterpret_cast <c_cs_inferno*> (entity);

		const auto origin = smoke->get_abs_origin();
		auto get_w2s_origin = c_vector3d{};

		if (!world_to_screen(origin, get_w2s_origin))
			return;

		const auto spawn_time = smoke->get_entity_spawn_time();
		static constexpr float max_time = 17.5;
		const auto actual = (((spawn_time + max_time) - global_vars_base->curtime) / max_time);

		if (actual <= 0)
			return;
		static auto size = c_vector2d(70, 3);
		const auto pos = c_vector2d(get_w2s_origin.x - size.x * 0.5, get_w2s_origin.y - size.y * 0.5);
		c_color background = c_color(35, 35, 35, config->Colors.nadecolorr[3] * 255);
		c_color bar = c_color(config->Colors.nadecolorr[0] * 255, config->Colors.nadecolorr[1] * 255, config->Colors.nadecolorr[2] * 255, config->Colors.nadecolorr[3] * 255);
		c_color text = c_color(255, 255, 255, config->Colors.nadecolorr[3] * 255);

		if (config->Visuals.draw_nades == 0)
			Drawing::DrawString(F::ESPInfo, pos.x + 32, pos.y - 20, text, FONT_CENTER, name.c_str());
		else
			Drawing::DrawString(F::ESPWeapon, pos.x + 32, pos.y - 20, text, FONT_CENTER, "k");

		Drawing::DrawRect(pos.x, pos.y, size.x, size.y, background);
		Drawing::DrawRect(pos.x + 1, pos.y + 1, (size.x - 2) * actual, size.y - 2, bar);
		//Drawing::DrawString((pos.x + 1)* actual, (pos.y + 1), text,  F::ESPInfo, true, std::to_string(actual).c_str());

	}, class_id::csmokegrenadeprojectile);

}

void c_esp::draw_taser_esp() {

	c_vector3d scrn_pos, origin_pos;

	auto local = c_cs_player::get_local_player();

	if (!local)
		return;

	const auto weapon = reinterpret_cast<c_base_combat_weapon*>(
		client_entity_list()->get_client_entity_from_handle(local->get_current_weapon_handle()));

	if (!weapon)
		return;

	const auto info = weapon_system->get_weapon_data(weapon->get_item_definition());

	if (!info)
		return;

	if (!input->camera_in_third_person)
		return;

	static float radius = 0;

	if (weapon->get_item_definition() == weapon_taser && config->Visuals.taser_range)
		radius = info->flRange / 2;
	else if (info->WeaponType == weapontype_knife && config->Visuals.knife_range && weapon->get_item_definition() != weapon_taser)
		radius = 32;
	else
		return;

	float multi = pi * 2.0 / 2047;
	c_vector3d place = local->get_shoot_position();
	static float hue;

	for (float rota = 0; rota < (pi * 2.0); rota += multi) {
		
		hue += 0.0000005f;

		c_vector3d position = c_vector3d(radius * cos(rota) + place.x, radius * sin(rota) + place.y, place.z);

		ray ray;
		game_trace trace;
		c_trace_filter filter;

		filter.skip_entity = local;
		ray.init(place, position);
		engine_trace()->trace_ray(ray, mask_shot_brushonly, &filter, &trace);
	
		if (world_to_screen(trace.endpos, scrn_pos)) {
			if (origin_pos.is_valid() && scrn_pos.is_valid()) {

				if (origin_pos == c_vector3d(0, 0, 0) || scrn_pos == c_vector3d(0, 0, 0)) {
					origin_pos = scrn_pos;
				}

				if (hue > 1.f)
					hue = 0;

				Drawing::DrawLine(origin_pos.x, origin_pos.y, scrn_pos.x, scrn_pos.y, c_color::from_hsb(hue, 1.f, 1.f));
			}
			origin_pos = scrn_pos;
		}
	}
}

void c_esp::draw_spread_crosshair() {

	auto local = c_cs_player::get_local_player();

	if (!local)
		return;

	if (!local->is_alive())
		return;

	const auto weapon = reinterpret_cast<c_base_combat_weapon*>(
		client_entity_list()->get_client_entity_from_handle(local->get_current_weapon_handle()));

	if (!weapon)
		return;

	const auto info = weapon_system->get_weapon_data(weapon->get_item_definition());

	if (!info)
		return;

	int screen_w, screen_h;
	engine_client()->get_screen_size(screen_w, screen_h);
	int cross_x = screen_w / 2, cross_y = screen_h / 2;

	float recoil_step = screen_h / 180;

	cross_x -= (int)(local->get_punch_angle().y * recoil_step);
	cross_y += (int)(local->get_punch_angle().x * recoil_step);

	weapon->update_accuracy();
	float inaccuracy = weapon->get_inaccuracy();
	float spread = weapon->get_spread();

	float cone = inaccuracy * spread;
	cone *= screen_h * 0.7f;
	cone *= 90.f / 180;

	c_color color = c_color(config->Colors.spread_crosshair[0] * 255, config->Colors.spread_crosshair[1] * 255, config->Colors.spread_crosshair[2] * 255, config->Colors.spread_crosshair[3] * 255);

	if (config->Visuals.spread_crosshair == 1) {

		for (int seed{ }; seed < 512; ++seed) {
			math::random_seed(math::random_float(0, 512));

			float rand_a = math::random_float(0.f, 1.f);
			float pi_rand_a = math::random_float(0.f, 2.f * pi);
			float rand_b = math::random_float(0.0f, 1.f);
			float pi_rand_b = math::random_float(0.f, 2.f * pi);

			float spread_x = cos(pi_rand_a) * (rand_a * inaccuracy) + cos(pi_rand_b) * (rand_b * spread);
			float spread_y = sin(pi_rand_a) * (rand_a * inaccuracy) + sin(pi_rand_b) * (rand_b * spread);

			float max_x = cos(pi_rand_a) * cone + cos(pi_rand_b) * cone;
			float max_y = sin(pi_rand_a) * cone + sin(pi_rand_b) * cone;

			float step = screen_h / 180 * 90.f;
			int screen_spread_x = (int)(spread_x * step * 0.7f);
			int screen_spread_y = (int)(spread_y * step * 0.7f);

			float percentage = (rand_a * inaccuracy + rand_b * spread) / (inaccuracy + spread);

			Drawing::DrawRect(cross_x + screen_spread_x, cross_y + screen_spread_y, 1, 2, color);
		}
	}
	else if (config->Visuals.spread_crosshair > 1) {

		float spread_x = cos(pi) * (1 * inaccuracy) + cos(pi) * (1 * spread);
		float spread_y = sin(pi) * (1 * inaccuracy) + sin(pi) * (1 * spread);

		float step = screen_h / 180 * 90.f;
		int screen_spread_x = (int)(spread_x * step * 0.7f);
		int screen_spread_y = (int)(spread_y * step * 0.7f);

		if (config->Visuals.spread_crosshair == 2)
			Drawing::DrawOutlinedCircle(screen_w / 2, screen_h / 2, inaccuracy * 550.f, 60, color);
		else
			Drawing::DrawFilledCircle  (screen_w / 2, screen_h / 2, inaccuracy * 550.f, 60, color);

	}
}

namespace snipe_scope {
	c_vector3d pos;
	int hitdamage;
};

void c_esp::sniper_scope() {

	const auto local = c_cs_player::get_local_player();

	if (!local || !local->is_alive() || !config->Visuals.enchance_scope)
		return;

	const auto weapon = reinterpret_cast<c_base_combat_weapon*>(
		client_entity_list()->get_client_entity_from_handle(local->get_current_weapon_handle()));

	if (!weapon)
		return;

	const auto info = weapon_system->get_weapon_data(weapon->get_item_definition());

	if (!info)
		return;

	const auto sniper = info->get_weapon_id() == weapon_awp || info->get_weapon_id() == weapon_g3sg1
		|| info->get_weapon_id() == weapon_scar20 || info->get_weapon_id() == weapon_ssg08;

	if (!sniper || !local->is_scoped())
		return;

	int wid, hig;
	engine_client()->get_screen_size(wid, hig);
	auto center = c_vector3d(wid / 2, hig / 2, 0);

	weapon->update_accuracy();
	float inaccuracy = weapon->get_inaccuracy();

	const c_color color = c_color(config->Colors.enchance_scope[0] * 255, config->Colors.enchance_scope[1] * 255, config->Colors.enchance_scope[2] * 255, config->Colors.enchance_scope[3] * 255);

	Drawing::DrawRect(0, center.y, center.x - 11, 1.f, color);
	Drawing::DrawRect(center.x, 0, 1.f, center.y - 11, color);

	Drawing::DrawRect(center.x + 10, center.y, center.x - 10, 1.f, color);
	Drawing::DrawRect(center.x, center.y + 10, 1.f, center.y - 10, color);

	Drawing::DrawOutlinedCircle(center.x, center.y, center.y * 0.8, 400, color);

	std::string scope = std::to_string(weapon->get_zoom_level()) + "X";

	Drawing::DrawString(F::ESPInfo, center.x * 1.05, center.y, color, false, scope.c_str());

	if (!(snipe_scope::pos == c_vector3d(0, 0, 0))) {

		const auto calc_angle = math::calc_angle(center, snipe_scope::pos);

		const int cur = global_vars_base->curtime;

		if (global_vars_base->tickcount % 2 == 0 && !(snipe_scope::pos == center))
			snipe_scope::pos += ( center - snipe_scope::pos) / c_vector3d(12, 12,0);// c_vector3d(center, center., 0);

		Drawing::DrawOutlinedCircle(snipe_scope::pos.x, snipe_scope::pos.y, 12, 400, color);

	}
	else
		Drawing::DrawOutlinedCircle(center.x, center.y, 8, 400, color);
}

bool c_esp::get_espbox(c_cs_player* entity, int& x, int& y, int& w, int& h)
{
	const auto collideable = entity->get_collideable();

	if (collideable == nullptr)
		return false;

	c_vector3d maxs = collideable->obb_maxs();
	c_vector3d pos, pos3d;
	c_vector3d top, top3d;
	pos3d = entity->get_abs_origin();
	top3d = pos3d + c_vector3d(0, 0, maxs.z);

	if (!world_to_screen(pos3d, pos) || !world_to_screen(top3d, top))
		return false;

	int iMiddle = (pos.y - top.y) + 4;
	int iWidth = iMiddle / 3.5;
	int iCornerWidth = iWidth / 4;

	x = top.x - iWidth;
	y = top.y;
	w = iWidth * 2;
	h = iMiddle;

	return true;
}

void c_esp::bomb_timer(c_cs_player* entity) {

	auto bomb = (c_cs_bomb*)entity;
	if (!bomb || !config->Visuals.bombtimer)
		return;

	const float c4_time = bomb->get_c4_blow_time();
	const float real_time = c4_time - (global_vars_base->interval_per_tick * c_cs_player::get_local_player()->get_tick_base());
	char time_char[5];
	sprintf_s(time_char, "%1.f", real_time);

	if (real_time <= 0)
		return;

	constexpr auto size = 13;

	const int red = 255 - ((real_time / 45) * 2.55);
	const  int green = (real_time / 45) * 2.55;

	int screen_w, screen_h;
	engine_client()->get_screen_size(screen_w, screen_h);

	Drawing::DrawRect(0, screen_h - size, screen_w, size, c_color(25, 25, 25, 100));
	Drawing::DrawRect(0, screen_h - size, screen_w * (real_time / 45), size, c_color(config->Colors.bombcolor[0]* 255, config->Colors.bombcolor[1] * 255, config->Colors.bombcolor[2] * 255, config->Colors.bombcolor[3] * 255));
	Drawing::DrawString(F::BombTimer, screen_w * (real_time / 45) + 5, screen_h - (size + 1), c_color(255, 255, 255, 155), FONT_LEFT, time_char);

	int x, y, w, h;
	if (get_espbox(entity, x, y, w, h))
		Drawing::DrawString(F::ESPInfo, x + w / 2, y, c_color(255, 255, 255, 255 * 0.8f), FONT_CENTER, time_char);

}

void c_esp::esp_radar(c_cs_player* player)
{
	const auto local = c_cs_player::get_local_player();

	if (!local || !local->is_alive())
		return;

	matrix3x4 bones[128]{};

	if (!player->setup_bones(bones, 128, bone_used_by_anything, global_vars_base->curtime))
		return;

	auto head_position = player->get_hitbox_position(c_cs_player::hitbox::head, bones);


	auto is_on_screen = [](c_vector3d origin, c_vector3d& screen) -> bool
	{
		if (!world_to_screen(origin, screen))
			return false;

		int iScreenWidth, iScreenHeight;
		engine_client()->get_screen_size(iScreenWidth, iScreenHeight);

		bool xOk = iScreenWidth > screen.x > 0, yOk = iScreenHeight > screen.y > 0;
		return xOk && yOk;
	};

	c_vector3d screenPos;
	if (is_on_screen(head_position.value(), screenPos)) //TODO (?): maybe a combo/checkbox to turn this on/off
		return;

	const auto view = engine_client()->get_view_angles();
	int w, h;
	engine_client()->get_screen_size(w, h);
	const auto vector = c_vector2d(w / 2, h / 2);
	const auto angle_to = math::calc_angle(local->get_shoot_position(), head_position.value());
	auto target_angle = angle_to - view;
	math::normalize(target_angle);

	const auto angle = target_angle.y;
	const auto height = 400.f;

	const auto diff_angle = 1.22f;
	const auto diff_height = 20;

	const auto a = vector - c_vector2d(
		(height - diff_height) * sin(deg2rad(angle + diff_angle)),
		(height - diff_height) * cos(deg2rad(angle + diff_angle))
	);

	const auto b = vector - c_vector2d(
		height * sin(deg2rad(angle)),
		height * cos(deg2rad(angle))
	);

	const auto c = vector - c_vector2d(
		(height - diff_height) * sin(deg2rad(angle - diff_angle)),
		(height - diff_height) * cos(deg2rad(angle - diff_angle))
	);

	Drawing::Triangle(a, b, c, c_color(config->Visuals.enemy.radar_color[0] * 255, config->Visuals.enemy.radar_color[1] * 255, config->Visuals.enemy.radar_color[2] * 255, config->Visuals.enemy.radar_color[3] * 255));
}

void c_esp::esp_skeleton(c_cs_player* player, Menu22::visaul::esp cfg)
{
	auto entity = (c_cs_player*)client_entity_list()->get_client_entity(player->index());

	if (!entity)
		return;

	studiohdr* pStudioHdr = model_info_client()->get_studio_model(entity->get_model());

	if (!pStudioHdr)
		return;

	c_vector3d vParent, vChild, sParent, sChild;
	for (int j = 0; j < pStudioHdr->numbones; j++)
	{
		mstudiobone_t* pBone = pStudioHdr->get_bone(j);

		if (pBone && (pBone->flags & 0x00000100) && (pBone->parent != -1))
		{
			vChild = entity->get_bone_position(j);
			vParent = entity->get_bone_position(pBone->parent);

			if (world_to_screen(vParent, sParent) && world_to_screen(vChild, sChild))
			{
				Drawing::DrawLine(sParent[0], sParent[1], sChild[0], sChild[1], c_color(cfg.skeleton_color[0] * 255, cfg.skeleton_color[1] * 255, cfg.skeleton_color[2] * 255, cfg.skeleton_color[3] * 255));
			}
		}
	}
}

void c_esp::esp_history_skeleton(c_cs_player* player, Menu22::visaul::esp cfg)
{
	auto entity = (c_cs_player*)client_entity_list()->get_client_entity(player->index());

	if (!entity)
		return;

	studiohdr* pStudioHdr = model_info_client()->get_studio_model(entity->get_model());

	if (!pStudioHdr)
		return;

	matrix3x4 bt_matrix[128] = {};
	matrix3x4 interpolated[128] = {};

	if (c_chams::get_simple_backtrack_matrix(entity, bt_matrix))
	{

		c_vector3d vParent, vChild, sParent, sChild;

		for (int j = 0; j < pStudioHdr->numbones; j++)
		{
			mstudiobone_t* pBone = pStudioHdr->get_bone(j);

			if (pBone && (pBone->flags & 0x00000100) && (pBone->parent != -1))
			{
				vChild = entity->get_bone_position(j, bt_matrix);
				vParent = entity->get_bone_position(pBone->parent, bt_matrix);

				if (world_to_screen(vParent, sParent) && world_to_screen(vChild, sChild))
				{
					Drawing::DrawLine(sParent[0], sParent[1], sChild[0], sChild[1], c_color(cfg.bt_skeleton_color[0] * 255, cfg.bt_skeleton_color[1] * 255, cfg.bt_skeleton_color[2] * 255, cfg.bt_skeleton_color[3] * 255));
				}
			}
		}

	}
}

void c_esp::esp_nades(c_cs_player* nade)
{

	if (!nade)
		return;

	const model* model = nade->get_model();
	if (!model)
		return;

	studiohdr* hdr = model_info_client()->get_studio_model(model);
	if (!hdr)
		return;

	const c_color Nadecolor = c_color(config->Colors.nadecolorr[0] * 255, config->Colors.nadecolorr[1] * 255, config->Colors.nadecolorr[2] * 255, config->Colors.nadecolorr[3] * 255);
	std::string entityName = hdr->name, icon_character;
	switch (nade->get_client_class()->id)
	{
	case 9:
		if (entityName[16] == 's' && config->Visuals.nade_style_flash)
		{
			entityName = ("flash");
			icon_character = ("i");
		}
		else if (config->Visuals.nade_style_he)
		{
			entityName = _("frag");
			icon_character = _("j");
		}
		break;
	//case csmokegrenadeprojectile:
	//	if ( config->Visuals.nade_style_smoke) {
	//		entityName = ("smoke");
	//		icon_character = ("k");
	//	}
	//	break;
	//case cmolotovprojectile:
	//	if (config->Visuals.nade_style_mol) {
	//		entityName = _("fire");
	//		icon_character = _("l");
	//	}
	//	break;
	case cdecoyprojectile:
		if (config->Visuals.nade_style_decoy) {
			entityName = _("decoy");
			icon_character = _("n");
		}
		break;
	default:
		return;
	}

	c_vector3d min, max, pos, pos3D, top, top3D;
	nade->get_render_bounds(min, max);
	pos3D = nade->get_origin() - c_vector3d(0, 0, 10); top3D = pos3D + c_vector3d(0, 0, max.z + 10);

	if (world_to_screen(pos3D, pos)) {
		if (config->Visuals.draw_nades == 1) // text
			Drawing::DrawString(F::ESPInfo, pos.x, pos.y - 18, Nadecolor,  FONT_CENTER, entityName.c_str());
		else // icons
			Drawing::DrawString(F::ESPWeapon, pos.x, pos.y - 18, Nadecolor, FONT_CENTER, icon_character.c_str());
	}
}

void c_esp::render_player()
{
	surface()->gain_mouse_pos(mouse::main_mouse, mouse::last_mouse);

	render_watermark();
	c_log::logs();
	draw_dmg();
	indicator_ini();
	esp_keybinds();
	draw_spectators();
	indicator();
	if (engine_client()->is_ingame() && c_cs_player::get_local_player()) {
		draw_molotov_timer();
		draw_smoke_timer();
		if (c_cs_player::get_local_player()->is_alive()) {
			c_hitmarker::draw();
			esp_no_scope();
			sniper_scope();
			draw_spread_crosshair();
			draw_penetration_crosshair();
			draw_firstperson_indicators();
			draw_taser_esp();
			draw_trails(c_cs_player::get_local_player());
		}

		for (auto i = 1; i < 64; i++)
		{
			const auto m_player = (c_cs_player*)client_entity_list()->get_client_entity(i);

			if (m_player == nullptr ||
				!m_player->is_alive())
				continue;

			auto cfg = config->Visuals.enemy;

			if (m_player->get_team() == c_cs_player::get_local_player()->get_team() && m_player != c_cs_player::get_local_player())
				cfg = config->Visuals.team;
			else if (m_player == c_cs_player::get_local_player() && input->camera_in_third_person)
				cfg = config->Visuals.local;
			else
				cfg = config->Visuals.enemy;

			if (cfg.visuals_enabled) {

				auto idx = m_player->index() - 1;

				int x, y, w, h;
				if (!get_espbox(m_player, x, y, w, h)) 
					continue;

				const float dt = ticks_to_time(c_cs_player::get_local_player()->get_tick_base()) - m_player->get_simtime();
				cfg.dormant_alpha[idx] = (m_player->is_dormant() ? std::clamp(1.f - (dt / FADE_TIME), 0.f, 1.f) : 1.f) * 255.f;

				if (cfg.dormant_alpha[idx] <= 0.f)
					continue;

				int right = 0;
				int down = 0;

				static auto flag_font_size = Drawing::GetTextSize(F::ESPInfo, "Hi").bottom - 3;

				if (cfg.visuals_esp_skeleton)
					esp_skeleton(m_player, cfg);

				if (cfg.visuals_esp_bt_skeleton)
					esp_history_skeleton(m_player, cfg);

				damageesp->draw_damage();

				if (cfg.visuals_esp_box) {
					Drawing::DrawOutlinedRect(x, y, w, h, c_color(cfg.box_color[0] * 255, cfg.box_color[1] * 255, cfg.box_color[2] * 255, cfg.dormant_alpha[idx]));
					Drawing::DrawOutlinedRect(x - 1, y - 1, w + 2, h + 2, c_color(10, 10, 10, cfg.dormant_alpha[idx] * 0.6f));
					Drawing::DrawOutlinedRect(x + 1, y + 1, w - 2, h - 2, c_color(10, 10, 10, cfg.dormant_alpha[idx] * 0.6f));
				}

				player_info info;

				if (cfg.visuals_esp_name && engine_client()->get_player_info(m_player->index(), &info))
					Drawing::DrawString(F::ESP, x + w / 2, y - 13, c_color(cfg.name_color[0] * 255, cfg.name_color[1] * 255, cfg.name_color[2] * 255, cfg.dormant_alpha[idx] * 0.9f), FONT_CENTER, info.name);

				const auto space = cfg.visuals_esp_armor ? 5 : 1;

				if (cfg.visuals_esp_armor_flag && m_player->get_armor() > 0) {
					Drawing::DrawString(F::ESPInfo, x + w + 2 + space, y - 1 + right++ * flag_font_size, c_color(cfg.armor_flag_color[0] * 255, cfg.armor_flag_color[1] * 255, cfg.armor_flag_color[2] * 255, cfg.dormant_alpha[idx] * 0.8f), false, m_player->has_helmet() ? "HK" : "K");
				}

				if (cfg.visuals_esp_fakeduck_flag) {
					int prev_ticks = 0;
					int ticks[64];
					if (prev_ticks != global_vars_base->tickcount) {
						ticks[m_player->index()]++; prev_ticks = global_vars_base->tickcount;
					}
					if( m_player->get_duck_amount() && m_player->get_duck_amount() > 0.1f && m_player->get_duck_amount() <= 0.9f && ticks[m_player->index()] >= 5) {
						Drawing::DrawString(F::ESPInfo, x + w + 2 + space, y - 1 + right++ * flag_font_size, c_color(cfg.fakeduck_flag_color[0] * 255, cfg.fakeduck_flag_color[1] * 255, cfg.fakeduck_flag_color[2] * 255, cfg.dormant_alpha[idx] * 0.8f), false, "DUCK");
					}
				}

				if (cfg.visuals_esp_defuse_flag && m_player->get_defusing()) { 
					Drawing::DrawString(F::ESPInfo, x + w + 2 + space, y - 1 + right++ * flag_font_size, c_color(cfg.defuse_flag_color[0] * 255, cfg.defuse_flag_color[1] * 255, cfg.defuse_flag_color[2] * 255, cfg.dormant_alpha[idx] * 0.8f), false, "DEFUSE");
				}

				if (cfg.visuals_esp_scope_flag && m_player->is_scoped()) {
					Drawing::DrawString(F::ESPInfo, x + w + 2 + space, y - 1 + right++ * flag_font_size, c_color(cfg.scope_flag_color[0] * 255, cfg.scope_flag_color[1] * 255, cfg.scope_flag_color[2] * 255, cfg.dormant_alpha[idx] * 0.8f), false, "SCOPE");
				}

				if (cfg.visuals_esp_flash_flag && m_player->get_flash_duration() > 0.15f) {
					Drawing::DrawString(F::ESPInfo, x + w + 2 + space, y - 1 + right++ * flag_font_size, c_color(cfg.flash_flag_color[0] * 255, cfg.flash_flag_color[1] * 255, cfg.flash_flag_color[2] * 255, cfg.dormant_alpha[idx] * 0.8f), false, "FLASH");
				}

				////if (cfg.visuals_esp_armor_flag && m_player->get_armor() > 0)
				//{
				//	const char* resolver_method = "none";
				//	switch (resolver.resolver_method[m_player->index()]) {
				//	case 0: resolver_method = "REAL"; break;
				//	case 1: resolver_method = "BRUTE"; break;
				//	case 2: resolver_method = "FREE"; break;
				//	case 3: resolver_method = "DELTA"; break;
				//	case 4: resolver_method = "OLDLBY"; break;
				//	case 5: resolver_method = "LBY"; break;
				//	case 6: resolver_method = "ALT"; break;
				//	case 7: resolver_method = "RLBY"; break;

				//	}

				////	Drawing::DrawString(F::ESPInfo, x + w + 2 + space, y - 1 + right++ * flag_font_size, c_color(cfg.armor_flag_color[0] * 255, cfg.armor_flag_color[1] * 255, cfg.armor_flag_color[2] * 255, cfg.dormant_alpha[idx] * 0.8f), false, resolver_method);
				//}

				if (cfg.visuals_esp_health) {
					int hp = m_player->get_health();

					if (hp > 100)
						hp = 100;

					int hp_percent = h - (int)((h * hp) / 100);

					int width = (w * (hp / 100.f));

					int red = 255 - (hp * 2.55);
					int green = hp * 2.55;

					char hps[10] = "";

					sprintf(hps, "%iHP", hp);

					auto text_size = Drawing::GetTextSize(F::ESPInfo, hps);

					Drawing::DrawRect(x - 6, y, 4, h + 1, c_color(80, 80, 80, cfg.dormant_alpha[idx] * 0.6f));
					Drawing::DrawRect(x - 5, y + hp_percent + 1, 2, h - hp_percent - 1, c_color(red, green, 0, cfg.dormant_alpha[idx]));

					if (hp < 90)
						Drawing::DrawString(F::ESPInfo, x - text_size.right - 6, y - 1, c_color(255, 255, 255, cfg.dormant_alpha[idx] * 0.8f), false, hps);
				}

				if (cfg.visuals_esp_weapon || cfg.visuals_esp_weapon_icons) {

					const auto weapon = reinterpret_cast<c_base_combat_weapon*>(
						client_entity_list()->get_client_entity_from_handle(m_player->get_current_weapon_handle()));

					if (weapon)
					{
						const auto weap_info = weapon_system->get_weapon_data(weapon->get_item_definition());

						if (!weap_info)
							continue;

						char wpn_name[100] = "";
						const auto weapon_name = cfg.visuals_esp_weapon_icons ? weapon->GetGunIcon() : weap_info->szWeaponName + 7;

						sprintf(wpn_name, "%s", weapon_name);

						if (*weapon->GetItemDefenitionIndex() == 64 && !cfg.visuals_esp_weapon_icons)
							strcpy(wpn_name, "revolver");

						const auto space = cfg.visuals_esp_ammo ? 8 : 1;
						const auto font = cfg.visuals_esp_weapon_icons ? F::ESPWeapon : F::ESPInfo;
						Drawing::DrawString(font, x + w / 2, y + h + space, c_color(cfg.weapon_color[0] * 255, cfg.weapon_color[1] * 255, cfg.weapon_color[2] * 255, cfg.dormant_alpha[idx] * 0.8f), FONT_CENTER, wpn_name);
					}
				}

				if (cfg.visuals_esp_ammo) {
					const auto weapon = reinterpret_cast<c_base_combat_weapon*>(
						client_entity_list()->get_client_entity_from_handle(m_player->get_current_weapon_handle()));
					if (weapon)
					{
						const auto weap_info = weapon_system->get_weapon_data(weapon->get_item_definition());

						if (!weap_info)
							continue;

						float ammo = weapon->get_current_clip(), maxammo = weap_info->iMaxClip1, width = (w * (ammo / maxammo));
						char ammo_bf[10] = "";
						sprintf(ammo_bf, "%i", (int)ammo);

						Drawing::DrawRect(x, y + h + 2, w + 1, 4, c_color(80, 80, 80, cfg.dormant_alpha[idx] * 0.6f));
						Drawing::DrawRect(x + 1, y + h + 3, width - 1, 2, c_color(cfg.ammo_color[0] * 255, cfg.ammo_color[1] * 255, cfg.ammo_color[2] * 255, cfg.dormant_alpha[idx] * 0.6f));

						auto text_size = Drawing::GetTextSize(F::ESPInfo, ammo_bf);
						if (ammo != maxammo)
							Drawing::DrawString(F::ESPInfo, x + width + 4 - text_size.right, y + h, c_color(cfg.ammo_color[0] * 255, cfg.ammo_color[1] * 255, cfg.ammo_color[2] * 255, cfg.dormant_alpha[idx] * 0.8f), FONT_CENTER, ammo_bf);
					}
				}

				if (cfg.visuals_esp_armor) {
					int armor = m_player->get_armor();

					if (armor == 0)
						return;

					if (armor > 100)
						armor = 100;

					int hp_percent = h - (int)((h * armor) / 100);

					int width = (w * (armor / 100.f));
					char armor_buf[10] = "";
					sprintf(armor_buf, "%i", armor);

					auto text_size = Drawing::GetTextSize(F::ESPInfo, armor_buf);

					Drawing::DrawRect(x + 2 + w, y, 4, h + 1, c_color(80, 80, 80, cfg.dormant_alpha[idx] * 0.6f));
					Drawing::DrawRect(x + 3 + w, y + hp_percent + 1, 2, h - hp_percent - 1, c_color(cfg.armor_color[0] * 255, cfg.armor_color[1] * 255, cfg.armor_color[2] * 255, cfg.dormant_alpha[idx]));
				}

				if (cfg.visuals_esp_snaplines) {

					if (m_player->is_dormant())
						continue;

					matrix3x4 bones[128];
					m_player->setup_bones(bones, 128, bone_used_by_hitbox, global_vars_base->curtime);
					std::optional<c_vector3d> pos = m_player->get_hitbox_position(c_cs_player::hitbox::head, bones);
					c_vector3d pov_angle;

					if (!pos.has_value())
						return;

					if (!world_to_screen(pos.value(), pov_angle))
						return;

					int w, h;
					engine_client()->get_screen_size(w, h);
					// targting box until we get a setup bones var for esp
					Drawing::DrawLine(w / 2, h / 2, pov_angle.x, pov_angle.y, c_color(cfg.snaplines_color[0] * 255, cfg.snaplines_color[1] * 255, cfg.snaplines_color[2] * 255, 255 * 0.6f));

				}

				//feature::misc.hit_damage();
			}
		}
		for (int i = 0; i <= client_entity_list()->get_highest_entity_index(); i++)
		{
			const auto m_player = (c_cs_player*)client_entity_list()->get_client_entity(i);		
			if (!m_player)
				continue;

			auto client_class = m_player->get_client_class();
			if (!client_class)
				continue;
			
			if (!m_player->is_dormant())
				esp_nades(m_player);

			if (strstr(client_class->network_name, "CPlantedC4"))
				bomb_timer(m_player);

			if (m_player->is_weapon()) {

				if (m_player->get_origin() == c_vector3d(0, 0, 0))
					continue;

				const auto weapon = reinterpret_cast<c_base_combat_weapon*>(m_player);

				if (!weapon)
					continue;

				const auto info = weapon_system->get_weapon_data(weapon->get_item_definition());

				if (!(info))
					continue;

				c_vector3d min, max, pos, pos3D, top, top3D;
				m_player->get_render_bounds(min, max);
				pos3D = m_player->get_origin() - c_vector3d(0, 0, 10); top3D = pos3D + c_vector3d(0, 0, max.z + 10);

				if (!world_to_screen(pos3D, pos))
					continue;

				if (pos == c_vector3d(0, 0, 0))
					continue;
	
				char name[100] = "";
				char ammo_bf[10] = "";

				sprintf(name, "%s", info->szWeaponName + 7);
				if (*weapon->GetItemDefenitionIndex() == 64) {
					if (!config->Visuals.visuals_droppped_name_icons)
						strcpy(name, "revolver");
					else
						strcpy(name, "J");
				}
				int height = (pos.y);
				float box_w = (float)(height / 10);

				float ammo = weapon->get_current_clip(), maxammo = info->iMaxClip1, width = (box_w * (ammo / maxammo));
				sprintf(ammo_bf, "%i", (int)ammo);

				if (config->Visuals.visuals_dropped_ammo)
				{
					Drawing::DrawRect(pos.x , top.y + height + 2, box_w + 1, 4, c_color(80, 80, 80, config->Visuals.dropped_ammo_color[3] * 255));
					Drawing::DrawRect(pos.x + 1, top.y + height + 3, width - 1, 2, c_color(config->Visuals.dropped_ammo_color[0] * 255, config->Visuals.dropped_ammo_color[1] * 255, config->Visuals.dropped_ammo_color[2] * 255, config->Visuals.dropped_ammo_color[3] * 255));
					auto text_size = Drawing::GetTextSize(F::ESPInfo, ammo_bf);

					if (ammo != maxammo)
						Drawing::DrawString(F::ESPInfo, pos.x + width + 4 - text_size.right, top.y + height + 4 - 2, c_color(config->Visuals.dropped_ammo_color[0] * 255, config->Visuals.dropped_ammo_color[1] * 255, config->Visuals.dropped_ammo_color[2] * 255, config->Visuals.dropped_ammo_color[3] * 255), FONT_CENTER, ammo_bf);
				}

				if (config->Visuals.visuals_dropped_name || config->Visuals.visuals_droppped_name_icons)
				{
					const auto space = (config->Visuals.visuals_dropped_box ? -13 : -1) + (config->Visuals.visuals_droppped_name_icons ? -6 : 0);
					const auto weapon_name = config->Visuals.visuals_droppped_name_icons ? weapon->GetGunIcon() : name;
					const auto font = config->Visuals.visuals_droppped_name_icons ? F::ESPWeapon : F::ESPInfo;
					Drawing::DrawString(font, pos.x + box_w / 2, top.y + height - 15, c_color(config->Visuals.dropped_name_color[0] * 255, config->Visuals.dropped_name_color[1] * 255, config->Visuals.dropped_name_color[2] * 255, config->Visuals.dropped_name_color[3] * 255), FONT_CENTER, weapon_name);
				}
			}
		}
	}
}

void c_esp::render_panel()
{
	if (engine_client()->is_ingame() && c_cs_player::get_local_player()) {
		for (auto i = 1; i < 64; i++)
		{
			const auto m_player = (c_cs_player*)client_entity_list()->get_client_entity(i);

			if (m_player == nullptr ||
				!m_player->is_alive() ||
				m_player->is_dormant() ||
				m_player == c_cs_player::get_local_player() ||
				m_player->get_team() == c_cs_player::get_local_player()->get_team()
				)
				continue;

			const auto cfg = config->Visuals.enemy;

			if (cfg.visuals_enabled) {

				matrix3x4 bones[128]{};

				if (!m_player->setup_bones(bones, 128, bone_used_by_anything, global_vars_base->curtime))
					return;

				auto head_position = m_player->get_hitbox_position(c_cs_player::hitbox::head, bones);

				if (cfg.visuals_esp_radar) {
					esp_radar(m_player);
				}
			}
		}
	}
}