#pragma once

#include "../sdk/c_game_event_manager.h"

enum round_gameflags
{
	ROUND_STARTING = 0,
	ROUND_IN_PROGRESS,
	ROUND_ENDING,
	RESET_PREVERSE_KILL,
	KILL_PREVERSE_KILL,
};

class c_events : public c_game_event_listener
{
public:
	static void hook();

	void fire_game_event(c_game_event* event) override;
	int get_event_debug_id() override;
	inline static round_gameflags round_flags;
	inline static bool is_active_round = false;
};
