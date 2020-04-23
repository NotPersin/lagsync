
#include "c_hooks.h"
#include "c_wnd_proc.h"
#include "idirect3ddevice9.h"
#include "c_client_mode_shared.h"
#include "c_surface_.h"
#include "c_events.h"
#include "c_model_render_.h"
#include "c_client.h"
#include "c_game_event_manager_.h"
#include "c_engine_client.h"
#include "c_base_animating_.h"
#include "c_cl_csm_shadows.h"
#include "c_net_channel_.h"
#include "c_cs_player_.h"
#include "dbghelp.h"
#include <thread>
#include "c_cl_camera_height_restriction_debug.h"
#include "c_view_render_.h"
#include "c_prediction_.h"
#include "c_client_state_.h"
#include "c_render_view_.h"
#include "c_engine_sound_.h"
#include "c_sv_cheats.h"
#include "c_panel_.h"
#include "engine_bsp.h"
#include "../hacks/c_esp.h"
#include "../hacks/skinchanger/kit_parser.h"
#include "../main.h"
#include "../sdk/c_base_tonecontrol.h"

using namespace std::chrono_literals;

namespace fonts {

	DWORD ESP;
	DWORD health_font;
	DWORD weaponnamefont;
	DWORD weapon_font;
	DWORD flagfont;
	DWORD arialfont;

}

void c_hooks::run()
{
	c_client_mode_shared::hook();
	c_wnd_proc::hook();
	//c_net_channel_::hook();
	c_cs_player_::hook();
	c_surface_::hook();
	c_events::hook();
	c_render_view_::hook();
	c_model_render_::hook();
	c_client::hook();
	c_vgui_panel::hook();
	c_game_event_manager_::hook();
	//c_base_animating_::hook();
	c_engine_client_::hook();
	c_cl_csm_shadows::hook();
	c_cl_camera_height_restriction_debug::hook();
	c_view_render_::hook();
	c_prediction_::hook();
	c_client_state_::hook();
	c_engine_sound_::hook();
	c_sv_cheats::hook();
	c_panel_::hook();
	//engine_bsp::hook();
	env_tonemap_controllor::hook();
	idirect3ddevice9::instance();

	NetvarHook();

	//hooks.bspQuery.hookAt(6, list_hook::listLeavesInBox);
}

/*
#ifdef RELEASE
void reflection_creation()
{
	/*
		.text:4B32B37C E8 4F EF FB FF                                call    ZwClose
		.text:4B32B381 39 7D F4                                      cmp     [ebp+var_C], edi
		.text:4B32B384 74 12                                         jz      short loc_4B32B398
		.text:4B32B386 57                                            push    edi
		.text:4B32B387 57                                            push    edi
		.text:4B32B388 FF 75 F4                                      push    [ebp+var_C]
		.text:4B32B38B E8 90 EE FB FF                                call    ZwWaitForSingleObject
		.text:4B32B390 FF 75 F4                                      push    [ebp+var_C]
		.text:4B32B393 E8 38 EF FB FF                                call    ZwClose
	 */
/*	RTLP_PROCESS_REFLECTION_REFLECTION_INFORMATION info = {};
	import("ntdll.dll", RtlCreateProcessReflection)(current_process, 0, nullptr, nullptr, nullptr, &info);
}
#endif
*/
