#pragma once

#include "../sdk/c_base_client.h"
#include "../sdk/c_client_state.h"
#include "../sdk/misc.h"

class c_client
{
	typedef void(__thiscall* shutdown_t)(c_base_client*);
	typedef void(__thiscall* level_init_pre_entity_t)(c_base_client*, const char*);
	typedef void(__thiscall* create_move_t)(c_base_client*, int, float, bool);
	typedef void(__thiscall* frame_stage_notify_t)(c_base_client*, clientframestage);
	typedef bool(__thiscall* do_post_screen_space_effects_t)(void*, c_view_setup*);
public:
    static void hook();


	static void __stdcall create_move(int sequence_number, float input_sample_frametime, bool active, bool& sendpacket);
private:
	inline static shutdown_t _shutdown;
	inline static level_init_pre_entity_t _level_init_pre_entity;
	inline static create_move_t _create_move;
	inline static frame_stage_notify_t _frame_stage_notify;
	inline static do_post_screen_space_effects_t _do_post_screen_space_effects;

	static void __fastcall shutdown(c_base_client* client, uint32_t);
	static void __fastcall level_init_pre_entity(c_base_client* client, uint32_t, const char* map_name);
	static void __fastcall frame_stage_notify(c_base_client* client, uint32_t, clientframestage stage);
};

class exploit {
public:

	int needtoshift;
	int firstuse;
	int tickbaseshift;
	bool insendmove;
	bool firstsendmovepack;
	bool exploiting;
	DWORD sendmovecall;// = (DWORD)PatternScan2(GetModuleHandle("engine.dll"), ("84 C0 74 04 B0 01 EB 02 32 C0 8B FE 46 3B F3 7E C9 84 C0 0F 84 ? ? ? ?"));
	DWORD sendmove;// = (DWORD)PatternScan2(GetModuleHandle("engine.dll"), ("55 8B EC A1 ? ? ? ? 81 EC ? ? ? ? B9 ? ? ? ? 53 8B 98"));
	DWORD writeusercmd;// = (DWORD)PatternScan2(GetModuleHandle("engine.dll"), ("55 8B EC 83 E4 F8 51 53 56 8B D9 8B 0D"));

};

extern exploit exploits;

static void create_move(int sequence_number, float input_sample_frametime, bool active);
