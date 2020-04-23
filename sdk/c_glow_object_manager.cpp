#pragma once

#include "c_glow_object_manager.h"
#include "../utils/c_memory.h"

CGlowObjectManager* CGlowObjectManager::get()
{
	static const auto glowobject = *(CGlowObjectManager**)(sig("client_panorama.dll", "0F 11 05 ? ? ? ? 83 C8 01") + 3);
	return glowobject;
}
