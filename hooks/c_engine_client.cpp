#include "c_engine_client.h"
#include "../utils/c_hook.h"
#include <future>
#include "../hacks/c_animation_system.h"

void c_engine_client_::hook()
{
	static c_hook<c_engine_client> hook(engine_client());
	_is_box_visible = hook.apply<is_box_visible_t>(32, is_box_visible);
	_is_hltv = hook.apply<is_hltv_t>(93, is_hltv);
}

int32_t __fastcall c_engine_client_::is_box_visible(c_engine_client* engine_client,
	uint32_t, c_vector3d& min, c_vector3d& max)
{
	static const auto ret = __("\x85\xC0\x74\x2D\x83\x7D\x10\x00\x75\x1C");

	_rt(addr, ret);
	if (!memcmp(_ReturnAddress(), addr, 10))
		return 1;

	return _is_box_visible(engine_client, min, max);
}

bool c_engine_client_::is_hltv(c_engine_client* engine_client, uint32_t)
{
	static const auto return_to_setup_velocity = sig("client_panorama.dll", "84 C0 75 38 8B 0D ? ? ? ? 8B 01 8B 80");
	static const auto return_to_accumulate_layers = sig("client_panorama.dll", "84 C0 75 0D F6 87");

	if (engine_client->is_ingame())
	{
		if (_ReturnAddress() == return_to_setup_velocity)
			return true;

		if (_ReturnAddress() == return_to_accumulate_layers)
			return true;
	}

	return _is_hltv(engine_client);
}