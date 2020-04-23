#include "c_client_mode_shared.h"
#include "c_net_channel_.h"
#include "../sdk/c_input.h"
#include "../hacks/c_miscellaneous.h"
#include "../hacks/c_legitbot.h"
#include "../hacks/c_antiaim.h"
#include "../menu/c_menu.h"
#include "c_model_render_.h"

void c_client_mode_shared::hook()
{
	static c_hook<uintptr_t> hook(client_mode());
	_override_view = hook.apply<override_view_t>(18, override_view);
	_override_mouse_input = hook.apply<override_mouse_input_t>(23, override_mouse_input);
    _no_fog = hook.apply<no_fog_t>(17, no_fog);
//	_do_post_screen_space_effects = hook.apply<do_post_screen_space_effects_t>(44, do_post_screen_space_effects);
}

uintptr_t* c_client_mode_shared::client_mode()
{
	static auto client_mode = **reinterpret_cast<uintptr_t***>((*reinterpret_cast<uintptr_t**>(base_client()))[10] + 5);
	return client_mode;
}

void c_client_mode_shared::override_view(uintptr_t* client_mode, uint32_t,  c_view_setup* view_setup)
{
	if (!engine_client()->is_ingame())
		return _override_view(client_mode, view_setup);

	if (GetAsyncKeyState(config->Ragebot.fakeduck) && config->Ragebot.fakeduck_enable && c_cs_player::get_local_player()->is_alive())//testing if this even works
		view_setup->origin.z = c_cs_player::get_local_player()->get_abs_origin().z + 64.f;

	c_miscellaneous::set_camera_to_thirdperson();
	_override_view(client_mode, view_setup);
	c_miscellaneous::disable_post_processing();
	c_miscellaneous::no_visual_recoil(view_setup);
	c_miscellaneous::no_zoom(view_setup);
}

void c_client_mode_shared::override_mouse_input(uintptr_t* client_mode, uint32_t, float* x, float* y)
{
	_override_mouse_input(client_mode, x, y);
}


bool c_client_mode_shared::no_fog(uintptr_t* client_mode)
{

	if (!engine_client()->is_ingame() || !engine_client()->is_connected())
		return _no_fog(client_mode);

	if (config->Visuals.no_fog)
		return false;
	else
		return _no_fog(client_mode);
}

bool c_client_mode_shared::do_post_screen_space_effects(void* ecx, void* edx, c_view_setup* setup) {

	g_basic_draw = true;

	if (c_cs_player::get_local_player() && c_cs_player::get_local_player()->is_alive() && input->camera_in_third_person) {
		g_local_draw = true;
		c_cs_player::get_local_player()->draw_model(1, 1);
		g_local_draw = false;
	}

	g_global_draw = true;
	const auto return_val = _do_post_screen_space_effects(ecx, setup);
	g_global_draw = false;
	return return_val;
}