#include "c_movement.h"
#include "../utils/math.h"
#include "../sdk/c_cvar.h"
#include "../menu/c_menu.h"

void c_movement::run(c_cs_player* local, c_user_cmd* cmd)
{
	bhop(local, cmd);

	auto slow_walk = [&](c_user_cmd* cmd, float max_speed) -> void
	{
		auto v36 = cmd->sidemove;
		auto v34 = cmd->forwardmove;
		auto a1 = (cmd->sidemove * cmd->sidemove) + (cmd->forwardmove * cmd->forwardmove);
		a1 = sqrt(a1);
		auto v14 = a1;

		if (a1 > max_speed)
		{
			cmd->forwardmove = (v34 / v14) * max_speed;
			a1 = (v36 / v14) * max_speed;
			cmd->sidemove = (v36 / v14) * max_speed;
		}
	};

	//if (_config->Misc.slowwalk && GetAsyncKeyState(_config->Misc.slowwalk_key))
	//	if (c_cs_player::get_local_player()->get_flags() & c_cs_player::flags::on_ground)
	//		slow_walk(cmd, 10.f + _config->Misc.slowwalkspeed);
}

void c_movement::autostrafe(c_cs_player* local, c_user_cmd* cmd)
{
	const auto vel = local->get_velocity().length2d();

	if (vel < 1.f)
		return;

	if (cmd->mousedx > 1 || cmd->mousedx < -1)
		cmd->sidemove = cmd->mousedx < 0.f ? -450.f : 450.f;
	else
	{
		cmd->forwardmove = std::clamp(10000.f / vel, -450.0f, 450.0f);
		cmd->sidemove = cmd->command_number % 2 == 0 ? -450.f : 450.f;
	}
};

void c_movement::autostrafe(c_user_cmd* cmd)
{
	static const auto cl_sidespeed = cvar()->find_var(_("cl_sidespeed"));
	const auto local = c_cs_player::get_local_player();
	float yaw_add = 0.f;

	if (!local || !local->is_alive())
		return;

	if (local->is_grounded() || !config->Misc.autostrafe)
		return;

	if (cmd->buttons & c_user_cmd::flags::back)
		yaw_add = -180.f;
	else if (cmd->buttons & c_user_cmd::flags::move_right)
		yaw_add = -90.f;
	else if (cmd->buttons & c_user_cmd::flags::move_left)
		yaw_add = 90.f;
	else if (cmd->buttons & c_user_cmd::flags::forward)
		yaw_add = 0.f;

	cmd->viewangles.y += yaw_add;
	cmd->forwardmove = 0.f;
	cmd->sidemove = 0.f;

	const auto delta = math::normalize_yaw(cmd->viewangles.y -
		rad2deg(atan2(local->get_velocity().y, local->get_velocity().x)));

	cmd->sidemove = delta > 0.f ? -cl_sidespeed->get_float() : cl_sidespeed->get_float();

	cmd->viewangles.y = math::normalize_yaw(cmd->viewangles.y - delta);
}

void c_movement::bhop(c_cs_player* local, c_user_cmd* cmd)
{
	if (!config->Misc.bhop)
		return;

	static auto last_jumped = false;
	static auto should_fake = false;

	const auto move_type = local->get_move_type();
	const auto flags = local->get_flags();

	if (move_type != c_cs_player::movetype_ladder && move_type != c_cs_player::movetype_noclip &&
		!(flags & c_cs_player::in_water))
	{
		if (!last_jumped && should_fake)
		{
			should_fake = false;
			cmd->buttons |= c_user_cmd::jump;
		}
		else if (cmd->buttons & c_user_cmd::jump)
		{
			autostrafe(cmd);

			if (flags & c_cs_player::on_ground)
			{
				last_jumped = true;
				should_fake = true;
			}
			else
			{
				cmd->buttons &= ~c_user_cmd::jump;
				last_jumped = false;
			}
		}
		else
		{
			last_jumped = false;
			should_fake = false;
		}
	}
}
