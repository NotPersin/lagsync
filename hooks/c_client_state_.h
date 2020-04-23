#pragma once

#include "../sdk/c_client_state.h"

class c_client_state_
{
	typedef bool(__thiscall* temp_entities_t)(void*, void*);
public:
	static void hook();
private:
	inline static temp_entities_t _temp_entites;
	static bool __fastcall temp_entites(c_client_state* state, uint32_t, void* msg);
};