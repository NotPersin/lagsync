#include "hooks/c_hooks.h"
#include "hooks/idirect3ddevice9.h"
#include "menu/c_menu.h"
#include <thread>
#include "loader_info.h"
#include "ice_key.h"
#include "main.h"
//Loader_info loader_info;

using namespace std::chrono_literals;
constexpr bool release = true;

namespace globals {
	bool release_build ;
	Loader_info loader_info;
};

DWORD g_image_base = 0x0;
HINSTANCE HThisModule;
void on_dll_attach(LPVOID reserved)
{

	while (!GetModuleHandleA("serverbrowser.dll")) {
		std::this_thread::sleep_for(100ms);
	}

	c_menu::instance();
	c_netvar::instance();
	c_hooks::run();

}

BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
{
	if (reason == DLL_PROCESS_ATTACH) {
		CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)on_dll_attach, reserved, 0, 0));
	}
	return TRUE;
}
