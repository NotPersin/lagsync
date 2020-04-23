#pragma once

#include "../includes.h"
#include "../sdk/misc.h"

class c_client_mode_shared
{
	typedef void(__thiscall* override_view_t)(uintptr_t*, c_view_setup*);
	typedef void(__thiscall* override_mouse_input_t)(uintptr_t*, float*, float*);
	typedef bool(__thiscall* no_fog_t)(uintptr_t*);
	typedef bool(__thiscall* do_post_screen_space_effects_t)(void*, c_view_setup*);

public:
	static void hook();
	static uintptr_t* client_mode();
private:
	inline static override_view_t _override_view;
	inline static override_mouse_input_t _override_mouse_input;
	inline static no_fog_t _no_fog;
	inline static do_post_screen_space_effects_t _do_post_screen_space_effects;

	static void __fastcall override_view(uintptr_t* client_mode, uint32_t, c_view_setup* view_setup);
	static void __fastcall override_mouse_input(uintptr_t* client_mode, uint32_t, float* x, float* y);
	static bool __fastcall no_fog(uintptr_t* client_mode);
	static bool __fastcall do_post_screen_space_effects(void* ecx, void* edx, c_view_setup* setup);
};
