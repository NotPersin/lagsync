#include "c_legitbot.h"
#include "../utils/math.h"
#include "../sdk/c_global_vars_base.h"
#include "../sdk/c_cvar.h"
#include "c_animation_system.h"
#include "c_aimhelper.h"
#include "../hooks/idirect3ddevice9.h"
#include <vector>
#include <optional>
bool c_legitbot::aim(c_user_cmd* cmd)
{

	bunnyhop(cmd);

	if (!config->Legitbot.enabled || config->Ragebot.enable) {
		return false;
	}

	auto local = c_cs_player::get_local_player();
	if (!local || local->get_health() <= 0)return false;
	bool shooting = (cmd->buttons & c_user_cmd::flags::attack) && local->can_shoot(cmd, global_vars_base->curtime, true);
	if (!shooting && config->Legitbot.aimbotonkeybool && !GetAsyncKeyState(config->Legitbot.aimbotkey))return false;

	const auto weapon = (c_base_combat_weapon*)client_entity_list()->get_client_entity_from_handle(local->get_current_weapon_handle());
	if (!weapon)return false;
	/*add a weapon check for knife bomb and shit*/

	if (weapon->is_bomb() || weapon->is_knife() || weapon->is_grenade())return false; // why would we aimbot with that

	std::optional<std::tuple<c_vector3d, float, c_animation_system::animation*, float>> target = std::nullopt;

	if (!target.has_value()) {
		std::vector<c_cs_player::hitbox> hitboxes_scan;
		hitboxes_scan.push_back(c_cs_player::hitbox::head);
		hitboxes_scan.push_back(c_cs_player::hitbox::pelvis);
		hitboxes_scan.push_back(c_cs_player::hitbox::left_foot);
		hitboxes_scan.push_back(c_cs_player::hitbox::right_foot);
		target = c_aimhelper::get_legit_target(config->Legitbot.otherfov, 1, hitboxes_scan);
	}

	if (!target.has_value())return false;

	static bool _config_aimbot = true;
	if (_config_aimbot) {
		const auto view_angle = engine_client()->get_view_angles();
		const auto shoot_position = local->get_shoot_position();
		const auto recoil_scale = cvar()->find_var(_("weapon_recoil_scale"))->get_float();

		auto aim_angle = math::calc_angle(shoot_position, std::get<0>(target.value()));

		auto fired_shots = local->get_shots_fired();
		int _config_minshots = 1;

		static bool _config_beforerecoil = false;
		if (_config_beforerecoil) {
			smooth(aim_angle, view_angle);

			if (_config_minshots < fired_shots)
				aim_angle -= local->get_punch_angle() * recoil_scale;
		}
		else {
			if (_config_minshots < fired_shots)
				aim_angle -= local->get_punch_angle() * recoil_scale;
			smooth(aim_angle, view_angle);
		}
		math::normalize(aim_angle);
		cmd->viewangles = aim_angle;
		engine_client()->set_view_angles(aim_angle);
	}

	cmd->tick_count = time_to_ticks(std::get<1>(target.value())) + lerp_ticks;
}
void c_legitbot::smooth(c_qangle& angle, c_qangle view_angle) {

	auto delta = angle - view_angle;
	math::normalize(delta);

	/*static float xsmoothing = 4;
	static float ysmoothing = 2;*/
	delta.x /= (config->Legitbot.othersmooth / 1.f);
	delta.y /= (config->Legitbot.othersmooth / 2.f);
	angle = view_angle + delta;

	math::normalize(angle);
}

void c_legitbot::bunnyhop(c_user_cmd* cmd) {

	auto local = c_cs_player::get_local_player();

	if (!config->Legitbot.legithop || config->Misc.bhop)
		return;

	static int hop_peng = 0;

	if (config->Legitbot.hop_percent / 10 < 9) {
		if (config->Legitbot.hop_percent / 10 < hop_peng) {
			hop_peng = 0;
		}
	}
	static bool step1 = false;
	static bool step2 = false;
	if (!step1 && step2)
	{
		step2 = false;
		hop_peng++;
		if (hop_peng > 0)
			cmd->buttons |= c_user_cmd::jump;
	}
	else if (cmd->buttons & c_user_cmd::jump)
	{
		if (local->get_flags() & c_cs_player::flags::on_ground)
			step2 = step1 = true;
		else
		{
			if (hop_peng > 0)
				cmd->buttons &= ~c_user_cmd::jump;
			step1 = false;
		}
	}
	else
		step2 = step1 = false;

}