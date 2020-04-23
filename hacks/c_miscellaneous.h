#pragma once
#include "../hooks/c_view_render_.h"
#include "../sdk/c_user_cmd.h"
#include "../utils/c_memory.h"
#include "../utils/includes.h"

class c_miscellaneous : public c_singleton<c_miscellaneous>
{
public:
	static void disable_post_processing();
	static void unlock_client_command_buffer();
	static void remove_visual_recoil(c_view_setup& view);
	static void change_fov(c_view_setup& view);
	static void set_viewmodel_parameters();
	static void set_camera_to_thirdperson();
	static void no_visual_recoil(c_view_setup* view_setup);
	static void no_zoom(c_view_setup* view_setup);
	static void remove_smoke();
	static void remove_flash();
	static void set_buttons_for_direction(c_user_cmd* cmd);
	static void engine_radar();
	static void misc_cm(c_user_cmd* cmd);
	static void view_model();
	static void modify_player_model();
	static void preserve_killfeed();
	static void shift_ticks(c_user_cmd* cmd, bool send_packet);
	static void buybot();

	void set_round_start(bool var) { round_start = var; };
	void set_round_end(bool var) { round_end = var; };
	void set_preserve_start(bool var) { preserve_start = var; };

	bool round_start, round_end, preserve_start;
};
