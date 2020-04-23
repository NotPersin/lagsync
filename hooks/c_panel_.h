#pragma once

#include "../sdk/c_panel.h"
#include "../sdk/c_surface.h"

class c_panel_
{
	typedef void(__thiscall* paint_traverse_t)(c_panel*, uint32_t panel, bool force_repaint, bool allow_force);
	typedef void(__thiscall* engine_vgui_t)(IEngineVGui*, int);

public:
	static void hook();

private:
	inline static paint_traverse_t _paint_traverse;
	inline static engine_vgui_t _engine_vgui;

	static void __fastcall paint_traverse(c_panel* panel, uint32_t, uint32_t panel_nr, bool force_repaint, bool allow_force);
	static void __stdcall EngineVGUI_Paint(int mode);
};
