#pragma once
#include "../sdk/misc.h"
#include "../sdk/c_client_state.h"
#include <WinUser.h>

class engine_bsp {
private:
	static int __fastcall hkListLeavesInBox(void* bsp, void* edx, c_vector3d& mins, c_vector3d& maxs, unsigned short* pList, int listMax);
	typedef int(__thiscall* ListLeavesInBox) (void*, const c_vector3d&, const c_vector3d&, unsigned short*, int);
	inline static ListLeavesInBox _ListLeavesInBox;
public:
	static void hook();
};