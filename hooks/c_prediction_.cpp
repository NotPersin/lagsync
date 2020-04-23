#include "c_prediction_.h"
#include "../utils/c_hook.h"
#include "c_client.h"
#include "../hacks/c_prediction_system.h"
#include "../hacks/c_antiaim.h"
#include "../sdk/c_input.h"
#include "../sdk/c_base_view_model.h"
#include "../utils/math.h"
#include <intrin.h>

void c_prediction_::hook()
{
	static c_hook<c_prediction> hook(prediction());
	_in_prediction = hook.apply<in_prediction_t>(14, in_prediction);
	_run_command = hook.apply<run_command_t>(19, run_command);
}

bool __fastcall c_prediction_::in_prediction(c_prediction* prediction, uint32_t)
{
	static const auto return_to_maintain_sequence_transitions =
		sig("client_panorama.dll", "84 C0 74 17 8B 87");

	if (_ReturnAddress() == return_to_maintain_sequence_transitions)
		return true;

	return _in_prediction(prediction);
}

void __fastcall c_prediction_::run_command(c_prediction* prediction, uint32_t,
	c_cs_player* player, c_user_cmd* cmd, c_move_helper* helper)
{
	if (cmd->tick_count >= INT_MAX)
	{
		cmd->predicted = true;
		return;
	}

	if (!player->is_local_player() || !engine_client()->is_ingame())
	{
		_run_command(prediction, player, cmd, helper);
		return;
	}

	_run_command(prediction, player, cmd, helper);

	antiaim->predict(player, cmd);
}