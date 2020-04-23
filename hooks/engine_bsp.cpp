#include "engine_bsp.h"
#include "../utils/c_hook.h"
#include "../menu/c_menu.h"

void engine_bsp::hook() {
	//static c_hook<void> hook(engine_client()->bsp_tree_query());
	//_ListLeavesInBox = hook.apply<ListLeavesInBox>(6, hkListLeavesInBox);
}

struct RenderableInfo_t {
	c_client_renderable* m_pRenderable;
	void* m_pAlphaProperty;
	int m_EnumCount;
	int m_nRenderFrame;
	unsigned short m_FirstShadow;
	unsigned short m_LeafList;
	short m_Area;
	uint16_t m_Flags;   // 0x0016
	uint16_t m_Flags2; // 0x0018
	c_vector3d m_vecBloatedAbsMins;
	c_vector3d m_vecBloatedAbsMaxs;
	c_vector3d m_vecAbsMins;
	c_vector3d m_vecAbsMaxs;
	int pad;
};

#define MAX_COORD_FLOAT ( 16384.0f )
#define MIN_COORD_FLOAT ( -MAX_COORD_FLOAT )

int engine_bsp::hkListLeavesInBox(void* bsp, void* edx, c_vector3d& mins, c_vector3d& maxs, unsigned short* pList, int listMax) {

	if (engine_client() && engine_client()->is_ingame()) {
		static auto return_func = _ListLeavesInBox(bsp, mins, maxs, pList, listMax);
		static auto list_leaves = *(uint32_t*)sig(("client_panorama.dll"), "56 52 FF 50 18") + 5;
		if (!config->Misc.disableModelOcclusion || *(uint32_t*)_ReturnAddress() != list_leaves)
			return return_func;

		auto info = *(RenderableInfo_t**)((uintptr_t)_AddressOfReturnAddress() + 0x14);
		if (!info || !info->m_pRenderable)
			return return_func;

		auto base_entity = info->m_pRenderable->get_client_unknown()->get_base_entity();
		if (!base_entity || !base_entity->is_player())
			return return_func;

		info->m_Flags &= 0x100;
		info->m_Flags2 |= 0xC0;

		static const c_vector3d map_min = c_vector3d(MIN_COORD_FLOAT, MIN_COORD_FLOAT, MIN_COORD_FLOAT);
		static const c_vector3d map_max = c_vector3d(MAX_COORD_FLOAT, MAX_COORD_FLOAT, MAX_COORD_FLOAT);
		auto count = _ListLeavesInBox(bsp, mins, maxs, pList, listMax);
		return count;
	}
}