#pragma once

#include "../includes.h"
#include "../hooks/idirect3ddevice9.h"
#include <mutex>
#include "../sdk/c_debug_overlay.h"
#include "./../menu/c_menu.h"

namespace fonts {

	extern DWORD ESP;
	extern DWORD health_font;
	extern DWORD weaponnamefont;
	extern DWORD weapon_font;
	extern DWORD flagfont;
	extern DWORD arialfont;
} 

namespace log_esp {
	struct esp_log_infos {
		char text[16]{};
	};
	extern std::vector<esp_log_infos> esp_log_info;
};

namespace snipe_scope {
	extern c_vector3d pos;
	extern int hitdamage;
};

namespace mouse {
	extern c_vector2d main_mouse;
	extern c_vector2d last_mouse;

	extern c_vector2d i_drag;
	extern c_vector2d s_drag;
	extern c_vector2d d_drag;
	extern c_vector2d k_drag;
};

class c_esp
{

public:
	static void fsn_nightmode();
	static void clear_dmg();
	static void draw_dmg();
	static void adddmg(c_game_event* event);
	static void draw_penetration_crosshair();
	static void draw_molotov_timer();
	static void draw_smoke_timer();
	static void draw_taser_esp();
	static void draw_spread_crosshair();
	static void esp_keybinds();
	static void sniper_scope();
	static bool get_espbox(c_cs_player* entity, int& x, int& y, int& w, int& h);
	static bool get_espboxv2(c_cs_player* entity, int& x, int& y, int& w, int& h);
	static void esp_radar(c_cs_player* player);
	static void esp_skeleton(c_cs_player* player, Menu22::visaul::esp cfg);
	static void bomb_timer(c_cs_player* entity);
	static void esp_radar(c_cs_player* player, std::optional<c_vector3d> position);
	static void esp_radar();
	static void esp_history_skeleton(c_cs_player* player, Menu22::visaul::esp cfg);
	static void esp_nades(c_cs_player* nade);
	static void render_player();
	static void render_panel();
	static void draw_spectators();
	static void draw_progresstext(c_vector2d& position, const char* name, const float progress);
	static void draw_progressbar(c_vector2d& position, const char* name, const c_color color, const float progress);
	static void draw_progressbar(c_vector2d& position, const char* name, const c_color color, const float progress, DWORD font);
	static void indicator();
	static void indicator_ini();
	static void draw_enemy_impact(c_game_event* event);
	static void draw_trails(c_cs_player* local);
	static void draw_firstperson_indicators();
	static void draw_thirdperson_indicators();

	static bool world_to_screen(c_vector3d world, c_vector3d& screen)
	{
		return (debug_overlay()->ScreenPosition(world, screen) != 1);
	}

	static bool should_run_dmg;
private:
	static constexpr auto esp_flags = c_font::font_flags::centered_x | c_font::font_flags::centered_y | c_font::font_flags::drop_shadow;
	static constexpr auto collision_box_top = 72.f;
	static constexpr auto collision_box_mod = 18.f;
	static constexpr auto head_radius = 6.5f;
	static constexpr auto fade_frequency =  255 / 1.f;

	static void esp_no_scope();
	static void render_watermark();
	static void draw_scope();
	static void draw_impact(c_vector3d start, c_vector3d end, c_color color);
};

