
#include <thread>
#include <numeric>
#include "idirect3ddevice9.h"
#include "../renderer/c_renderer.h"
#include "../hacks/c_esp.h"
#include "../menu/c_menu.h"
#include "../hacks/c_hitmarker.h"
#include "../hacks/c_chams.h"
#include "../renderer/c_indicators.h"
#include "../utils/c_memory.h"
#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"
#include "../sdk/c_input_system.h"
#include "../hacks/c_damage_esp.h"
idirect3ddevice9::idirect3ddevice9() : default_renderer(std::make_unique<c_renderer>(direct_device()))
{
	static c_hook<IDirect3DDevice9> hook(direct_device());
	_reset = hook.apply<reset_t>(16, reset);
	_end_scene = hook.apply<end_scene_t>(42, end_scene);
}

c_renderer* idirect3ddevice9::get_renderer() const
{
	return default_renderer.get();
}

IDirect3DDevice9* idirect3ddevice9::direct_device()
{
	static const auto direct_device = **reinterpret_cast<IDirect3DDevice9***>(sig("shaderapidx9.dll", "A1 ? ? ? ? 50 8B 08 FF 51 0C") + 1);
	return direct_device;
}

HRESULT idirect3ddevice9::reset(IDirect3DDevice9* dev, D3DPRESENT_PARAMETERS* param)
{
	ImGui_ImplDX9_InvalidateDeviceObjects();
//	instance()->get_renderer()->invalidate_device_objects();
	const auto result = instance()->_reset(dev, param);
	ImGui_ImplDX9_CreateDeviceObjects();
//	instance()->get_renderer()->init_device_objects(dev);
	return result;
}

HRESULT idirect3ddevice9::end_scene(IDirect3DDevice9* dev)
{
	static HWND window = {};

	if (!window)
		window = FindWindowA("Valve001", nullptr);

	if (!menu->d3dinit)
		menu->GUI_Init(window, dev);

	/* This is used because the game calls endscene twice each frame, one for ending the scene and one for finishing textures, this makes it just get called once */
	static auto wanted_ret_address = _ReturnAddress();
	if (_ReturnAddress() == wanted_ret_address)
	{
		//backup render states
		DWORD colorwrite, srgbwrite;
		dev->GetRenderState(D3DRS_COLORWRITEENABLE, &colorwrite);
		dev->GetRenderState(D3DRS_SRGBWRITEENABLE, &srgbwrite);

		// fix drawing without calling engine functons/cl_showpos
		dev->SetRenderState(D3DRS_COLORWRITEENABLE, 0xffffffff);
		// removes the source engine color correction
		dev->SetRenderState(D3DRS_SRGBWRITEENABLE, false);

		auto open_menu = []() -> void 
		{
			static bool is_down = false;
			static bool is_clicked = false;

			if (GetAsyncKeyState(VK_INSERT))
			{
				is_clicked = false;
				is_down = true;
			}
			else if (!GetAsyncKeyState(VK_INSERT) && is_down)
			{
				is_clicked = true;
				is_down = false;
			}
			else
			{
				is_clicked = false;
				is_down = false;
			}

			if (is_clicked)
			{
				menu->open = !menu->open;

			}
		};

		open_menu();

		ImGui::GetIO().MouseDrawCursor = menu->open;

		ImGui_ImplDX9_NewFrame();

		//restore these
		dev->SetRenderState(D3DRS_COLORWRITEENABLE, colorwrite);
		dev->SetRenderState(D3DRS_SRGBWRITEENABLE, srgbwrite);

		if (menu->open)
			menu->mainWindow();

		ImGui::Render();
	}

	ImGuiStyle& style = ImGui::GetStyle();
	if (menu->open)
	{
		if (style.Alpha > 1.f)
			style.Alpha = 1.f;
		else if (style.Alpha != 1.f)
			style.Alpha += 0.01f;
	}
	else
	{
		if (style.Alpha < 0.f)
			style.Alpha = 0.f;
		else if (style.Alpha != 0.f)
			style.Alpha -= 0.01f;
	}

	if (engine_client()->is_ingame() && engine_client()->is_connected() && c_cs_player::get_local_player()) {
		c_chams::instance()->latch_timer();
	}

	return instance()->_end_scene(dev);
}
