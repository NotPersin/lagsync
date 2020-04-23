#pragma once

#include "../includes.h"
#include "../renderer/c_renderer.h"
#include "../imgui/imgui.h"
#include "../imgui/DX9/imgui_impl_dx9.h"
extern HWND window;
class idirect3ddevice9 : public c_singleton<idirect3ddevice9>
{
	typedef HRESULT(__stdcall* reset_t)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);
	typedef HRESULT(__stdcall* end_scene_t)(IDirect3DDevice9*);
	typedef HRESULT(__stdcall* present_t)(IDirect3DDevice9*, RECT*, CONST RECT*, HWND, CONST RGNDATA*);

public:
    idirect3ddevice9();

	c_renderer* get_renderer() const;
	static IDirect3DDevice9* direct_device();
private:
	std::unique_ptr<c_renderer> default_renderer;

	reset_t _reset;
	end_scene_t _end_scene;
	present_t _present;

	static HRESULT __stdcall reset(IDirect3DDevice9* dev, D3DPRESENT_PARAMETERS* param);
	static HRESULT __stdcall end_scene(IDirect3DDevice9* dev);
	static HRESULT __stdcall present(IDirect3DDevice9* dev, RECT*, CONST RECT*, HWND, CONST RGNDATA*);
};

extern IMGUI_API void        ImGui_ImplDX9_NewFrame();

#define device idirect3ddevice9::instance()->direct_device()
#define renderer idirect3ddevice9::instance()->get_renderer()
