#include "c_wnd_proc.h"
#include "../menu/c_menu.h"
#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"
#include "../imgui/DX9/imgui_impl_dx9.h"
void c_wnd_proc::hook()
{
	window = FindWindowA(_("Valve001"), nullptr);
	_wnd_proc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(window, GWLP_WNDPROC, reinterpret_cast<LONG>(wnd_proc)));
}
bool input_shouldListen = false;
bool pressedKey[256] = {};

LRESULT c_wnd_proc::wnd_proc(const HWND wnd, const UINT msg, const WPARAM w_param, const LPARAM l_param)  // NOLINT
{
	switch (msg)
	{
	case WM_LBUTTONDOWN:

		pressedKey[VK_LBUTTON] = true;
		break;

	case WM_LBUTTONUP:

		pressedKey[VK_LBUTTON] = false;
		break;

	case WM_RBUTTONDOWN:

		pressedKey[VK_RBUTTON] = true;
		break;

	case WM_RBUTTONUP:

		pressedKey[VK_RBUTTON] = false;
		break;

	case WM_KEYDOWN:

		pressedKey[w_param] = true;
		break;

	case WM_KEYUP:

		pressedKey[w_param] = false;
		break;

	default: break;
	}



	if (menu->d3dinit && menu->open && ImGui_ImplDX9_WndProcHandler(wnd, msg, w_param, l_param) && !input_shouldListen)
		return true;


	
	return CallWindowProc(_wnd_proc, wnd, msg, w_param, l_param);
}
