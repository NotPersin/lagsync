#pragma once

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <Windowsx.h>
#include <cstdint>
#include <algorithm>
#include <memory>
#include <cctype>
#include <string>
#include <unordered_map>
#include <vector>
#include <functional>
#include <optional>
#include <intrin.h>

#include "d3d9/d3d9.h"
#include "d3d9/d3dx9.h"

#include "macros.h"

#include "sounds/sounds.h"

#include "renderer/c_font.h"

#include "sdk/c_client_entity_list.h"
#include "sdk/c_base_client.h"
#include "sdk/c_color.h"
#include "sdk/c_vector2d.h"
#include "sdk/c_vector3d.h"
#include "sdk/c_user_cmd.h"
#include "sdk/c_cs_player.h"
#include "sdk/c_game_event_manager.h"

#include "security/fnv1a.h"
#include "security/string_obfuscation.h"
#include "security/erase_function.h"

#include "utils/c_hook.h"
#include "utils/c_singleton.h"
#include "utils/c_config.h"
#include "utils/c_log.h"
#include "utils/c_hotkeys.h"
#include "utils/c_memory.h"

#undef min
#undef max

static auto linear_fade = [](int32_t& current, const int32_t min, const int32_t max, const float frequency, const bool direction) -> void
{
	if (current < max && direction)
		current += static_cast<int>(std::ceil(frequency * global_vars_base->frametime));
	else if (current > min && !direction)
		current -= static_cast<int>(std::ceil(frequency * global_vars_base->frametime));

	current = std::clamp(current, min, max);
};

static auto get_key_state = [](bool& boolean, int vk) -> void {

	static bool is_down = false;
	static bool is_clicked = false;

	if (GetAsyncKeyState(vk))
	{
		is_clicked = false;
		is_down = true;
	}
	else if (!GetAsyncKeyState(vk) && is_down)
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
		boolean = !boolean;

	}
};