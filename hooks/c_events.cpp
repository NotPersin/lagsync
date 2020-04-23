#include "c_events.h"
#include "../c_rifk.h"
#include "../hacks/c_hitmarker.h"
#include "../hacks/c_resolver.h"
#include "../BASS/API.h"
#include "../sdk/c_debug_overlay.h"
#include "../sdk/c_view_beams.h"
#include "../hacks/c_esp.h"
#include "../sdk/c_weapon_system.h"
#include "../menu/c_menu.h"
#include "../hacks/c_damageesp.h"
#include "../hacks/c_chams.h"
#include "../hacks/c_miscellaneous.h"

void c_events::hook()
{
	static c_events events {};
	game_event_manager()->add_listener_global(&events, false);
}

namespace events {
	int total_kills;
	int killed[65];
	int deaths;
	void reset_events();
};

void events::reset_events() {
	for (int x = 0; x < 65; x++)
		killed[x] = 0;
	total_kills = 0;
	deaths = 0;
}

void c_events::fire_game_event(c_game_event* event)
{
	static auto delay = 0;

	if (delay > 0)
		++delay;

	auto run_buy_bot = false;

	if ((delay >= 50))
	{
		delay = 0;
		c_miscellaneous::buybot();
		run_buy_bot = true;
	}

	switch (fnv1a_rt(event->get_name()))
	{
	case fnv1a("client_disconnect"):
		c_esp::clear_dmg();
		c_log::clear_log();

		if (events::total_kills > 0)
			_events.push_back(_event("In your last match you got " + std::to_string(events::total_kills) + " kills and died " + std::to_string(events::deaths) + " times with a total k/d of " + std::to_string(std::clamp(events::total_kills, 1, 999) / std::clamp(events::deaths, 1, 999))));
		events::reset_events();

		break;
	case fnv1a("cs_win_panel_match"): {
		if (events::total_kills > 0)
			_events.push_back(_event("In your this match you got " + std::to_string(events::total_kills) + " kills and died " + std::to_string(events::deaths) + " times with a total k/d of " + std::to_string(std::clamp(events::total_kills, 1, 999) / std::clamp(events::deaths, 1, 999))));
		events::reset_events();
	}
	case fnv1a("player_hurt"): {
		c_hitmarker::on_player_hurt(event);
		c_hitmarker::deadsound(event);
		c_resolver::on_player_hurt(event);
		damageesp->on_player_hurt(event);
		c_esp::adddmg(event);

		const auto attacker = client_entity_list()->get_client_entity(
			engine_client()->get_player_for_user_id(event->get_int(_("attacker"))));
		const auto target = reinterpret_cast<c_cs_player*>(client_entity_list()->get_client_entity(
			engine_client()->get_player_for_user_id(event->get_int(_("userid")))));
		if (attacker == c_cs_player::get_local_player() && target != attacker)
			events::total_kills++;
		else if (target == c_cs_player::get_local_player()) {
			events::deaths++;
		}
	}
	break;
	case fnv1a("player_death"): {
		c_resolver::on_player_death(event);
	}break;
	case fnv1a("bullet_impact"):
		damageesp->on_bullet_impact(event);
		c_esp::draw_enemy_impact(event);
		c_resolver::on_bullet_impact(event);
		c_hitmarker::on_bullet_impact(event);
		break;
	case fnv1a("round_start"):
		c_hitmarker::on_round_start(event);
		c_esp::clear_dmg();
		c_log::clear_log();
		c_resolver::on_round_start(event);

		delay = 1;
		if (!run_buy_bot)
			c_miscellaneous::buybot();

		is_active_round = true;
		break;
	default:
		break;
	}
}

int c_events::get_event_debug_id()
{
	return 42;
}
