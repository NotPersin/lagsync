#pragma once

#include "../sdk/c_engine_client.h"

class c_engine_client_
{
	typedef int32_t(__thiscall* is_box_visible_t)(c_engine_client*, c_vector3d&, c_vector3d&);
	typedef bool(__thiscall* is_hltv_t)(c_engine_client*);

public:
	static void hook();

private:
	inline static is_box_visible_t _is_box_visible;
	inline static is_hltv_t _is_hltv;

	static int32_t __fastcall is_box_visible(c_engine_client* engine_client,
		uint32_t, c_vector3d& min, c_vector3d& max);
	static bool __fastcall is_hltv(c_engine_client* engine_client, uint32_t);
};