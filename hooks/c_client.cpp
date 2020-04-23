#include "c_client.h"
#include "../utils/c_hook.h"
#include "../hacks/c_animation_system.h"
#include "../sdk/c_client_entity_list.h"
#include "../sdk/c_game_rules.h"
#include "../sdk/c_weapon_system.h"
#include "../hacks/c_esp.h"
#include "c_net_channel_.h"
#include "../hacks/c_movement.h"
#include "../hacks/c_resolver.h"
#include "../hacks/debug.h"
#include "../hacks/c_miscellaneous.h"
#include "../hacks/c_ragebot.h"
#include "c_client.h"
#include "../hacks/c_prediction_system.h"
#include "../sdk/c_input.h"
#include "../hacks/c_antiaim.h"
#include "../sdk/c_client_state.h"
#include "../sdk/c_prediction.h"
#include "../sdk/c_base_view_model.h"
#include "../hacks/c_legitbot.h"
#include "c_client_state_.h"
#include "c_events.h"
#include "c_cl_camera_height_restriction_debug.h"
#include "../hacks/skinchanger/skin_changer.h"
#include "../main.h"

void c_client::hook()
{
	static c_hook<c_base_client> hook(base_client());
	//_shutdown = hook.apply<shutdown_t>(4, shutdown);
	//_level_init_pre_entity = hook.apply<level_init_pre_entity_t>(5, level_init_pre_entity);
	_create_move = hook.apply<create_move_t>(22, ::create_move);
	_frame_stage_notify = hook.apply<frame_stage_notify_t>(37, frame_stage_notify);

	cvar()->find_var("viewmodel_fov")->null_callback();
	cvar()->find_var("viewmodel_offset_x")->null_callback();
	cvar()->find_var("viewmodel_offset_y")->null_callback();
	cvar()->find_var("viewmodel_offset_z")->null_callback();
	cvar()->find_var("r_aspectratio")->null_callback();
	cvar()->find_var("sv_showimpacts")->null_callback();
	cvar()->find_command(_("clear"))->dispatch();
}

float fake_angle;
std::array<float,24u> fake_pose;

void __fastcall c_client::shutdown(c_base_client* client, uint32_t)
{

}

void c_client::level_init_pre_entity(c_base_client* client, uint32_t, const char* map_name)
{
	_level_init_pre_entity(client, map_name);
}


template<typename FuncType>
__forceinline static FuncType CallVFuckingFunc(void* ppClass, int index)
{
	int* pVTable = *(int**)ppClass;
	int dwAddress = pVTable[index];
	return (FuncType)(dwAddress);
}


c_user_cmd *c_input::GetUserCmd(int nSlot, int sequence_number)
{
	typedef c_user_cmd*(__thiscall *GetUserCmd_t)(void*, int, int);
	return CallVFuckingFunc<GetUserCmd_t>(this, 8)(this, nSlot, sequence_number);
}

void __stdcall c_client::create_move(int sequence_number, float input_sample_frametime, bool active, bool& sendpacket)
{
	static const auto get_check_sum = reinterpret_cast<uint32_t(__thiscall*)(c_user_cmd*)>(sig("client_panorama.dll", "53 8B D9 83 C8"));
	static int ticks_allowed = 0;

	if (!get_check_sum) {
		cvar()->console_color_printf(false, c_color(255, 100, 100), "FAILED TO FIND `get_check_sum`\n");
	}
	_create_move(base_client(), sequence_number, input_sample_frametime, active);

	auto cmd = &input->commands[sequence_number % 150];
	auto verified_cmd = &input->verified_commands[sequence_number % 150];
	if (!cmd || !cmd->command_number || cmd == 0) return _create_move(base_client(), sequence_number, input_sample_frametime, active);

	const auto local = reinterpret_cast<c_cs_player*>(client_entity_list()->get_client_entity(engine_client()->get_local_player()));

	c_cl_camera_height_restriction_debug::in_cm = true;

	if (!cmd->command_number || !local || !local->is_alive())
	{
		c_miscellaneous::misc_cm(cmd);
		c_cl_camera_height_restriction_debug::in_cm = false;
		return;
	}

	static auto last_netchannel_ptr = net_channel;
	static auto did_once = false;

	if (net_channel && !did_once)
	{
		// first hook.
		c_net_channel_::hook();
		did_once = true;
	}
	if (last_netchannel_ptr != net_channel)
	{
		// rehook upon pointer changes.
		c_net_channel_::apply_to_net_chan(net_channel);
		last_netchannel_ptr = net_channel;
	}

	const auto weapon = reinterpret_cast<c_base_combat_weapon*>(
		client_entity_list()->get_client_entity_from_handle(local->get_current_weapon_handle()));

	c_base_combat_weapon::weapon_data* wpn_info = nullptr;

	if (!weapon || !(wpn_info = weapon_system->get_weapon_data(weapon->get_item_definition())))
	{
		c_cl_camera_height_restriction_debug::in_cm = false;
		return;
	}

	animation_system->in_jump = cmd->buttons & c_user_cmd::jump;

	if (menu->open)
		cmd->buttons &= ~c_user_cmd::attack;

	cmd->buttons &= ~c_user_cmd::speed;

	c_movement::run(local, cmd);
	prediction_system->initial(local, cmd, sequence_number);
	auto target_angle = cmd->viewangles;
	local->can_shoot(cmd, global_vars_base->curtime);
	antiaim->fakelag(local, cmd, sendpacket); 
	c_esp::draw_thirdperson_indicators();
	auto ticks_to_cock =
		(int)(0.25f / (std::round(global_vars_base->interval_per_tick * 1000000.f) / 1000000.f));

	static int revolver_cocked_ticks = 0;
	if (config->Ragebot.autorevolver) {
		if (weapon->get_item_definition() == weapon_revolver && !(cmd->buttons & c_user_cmd::reload))
		{
			revolver_cocked_ticks++;
			if (revolver_cocked_ticks + (antiaim->get_last_choke() <= 1 ? 2 : antiaim->get_last_choke()) < ticks_to_cock)
				cmd->buttons |= c_user_cmd::attack;
			else
				revolver_cocked_ticks = 0;
		}

		if (weapon->get_item_definition() == weapon_revolver)
		{
			const auto revolver_can_shoot = revolver_cocked_ticks + (antiaim->get_last_choke() <= 1 ? 2 : antiaim->get_last_choke()) <= ticks_to_cock;
			if (revolver_can_shoot && config->Ragebot.enable)
			{
				c_ragebot::aim(local, cmd, sendpacket);
			}
		}
		else if (config->Ragebot.enable)
		{
			if (cmd->command_number % 2 == 0)
				c_ragebot::aim(local, cmd, sendpacket);
		}
	}
	else if (config->Ragebot.enable)
	{
		if (cmd->command_number % 2 == 0)
			c_ragebot::aim(local, cmd, sendpacket);
	}

	c_miscellaneous::misc_cm(cmd);

	if ((weapon->get_item_definition() != weapon_revolver && local->is_shooting(cmd, global_vars_base->curtime)) || (weapon->get_item_definition() == weapon_revolver) && antiaim->get_last_shot())
	{
		//auto cfg = config->Ragebot.pistol;

		//if (wpn_info->get_weapon_id() == weapon_g3sg1 || wpn_info->get_weapon_id() == weapon_scar20)
		//	cfg = config->Ragebot._auto;
		//else if (wpn_info->get_weapon_id() == weapon_ssg08)
		//	cfg = config->Ragebot.scout;
		//else if (wpn_info->get_weapon_id() == weapon_awp)
		//	cfg = config->Ragebot.awp;
		//else if (wpn_info->get_weapon_id() == weapon_deagle || wpn_info->get_weapon_id() == weapon_revolver)
		//	cfg = config->Ragebot.heavy;
		//else if (wpn_info->WeaponType == weapontype_pistol)
		//	cfg = config->Ragebot.pistol;
		//else if (wpn_info->get_weapon_id() == weapon_taser)
		//	cfg = config->Ragebot.taser;
		//else
		//	cfg = config->Ragebot.other;

		//if (cfg.exploits > 0)
		//	sendpacket = true;

		antiaim->set_last_shot(false);

		c_aimhelper::fix_movement(cmd, target_angle);
		antiaim->shot_cmd = cmd->command_number;
	}

	if (config->Ragebot.no_recoil && config->Ragebot.enable) {
		static const auto recoil_scale = cvar()->find_var(_("weapon_recoil_scale"));
		cmd->viewangles -= c_cs_player::get_local_player()->get_punch_angle() *
			recoil_scale->get_float();
	}
	antiaim->run(local, cmd);
	c_miscellaneous::shift_ticks(cmd, sendpacket);
	prediction_system->repredict(local, cmd);
	prediction_system->restore();

	if (!sendpacket)
		ticks_allowed++;
	else
		ticks_allowed -= client_state->choked_commands;

	ticks_allowed = std::clamp(ticks_allowed, 0, 16);

	// verify new command.
	math::ensure_bounds(cmd->viewangles, *reinterpret_cast<c_vector3d*>(&cmd->forwardmove));
	c_miscellaneous::set_buttons_for_direction(cmd);
	verified_cmd->cmd = *cmd;
	verified_cmd->crc = get_check_sum(&verified_cmd->cmd);
	c_cl_camera_height_restriction_debug::in_cm = false;
	animation_system->local_animation.eye_angles = cmd->viewangles;
}


void c_client::frame_stage_notify(c_base_client* client, uint32_t, const clientframestage stage)
{
	c_esp::fsn_nightmode();

	if (!engine_client()->is_ingame() || !engine_client()->is_connected())
		return _frame_stage_notify(client, stage);

	const auto local = c_cs_player::get_local_player();

	if (!local)
		return _frame_stage_notify(client, stage);

	static auto cycle = 0.f;
	static auto anim_time = 0.f;

	const auto view_model = local ? reinterpret_cast<c_base_view_model*>(
		client_entity_list()->get_client_entity_from_handle(local->get_view_model())) : nullptr;

	if (stage == clientframestage::frame_net_update_postdataupdate_start && local->is_alive())	{
		if (config->Skinchanger.Enabled)
			SkinChanger2();
	}

	if (stage == frame_render_start)
	{
		if (local->is_alive()) {

			const auto state = local->get_anim_state();
			if (state)
			{
				static auto last_tick = global_vars_base->tickcount;

				if (global_vars_base->tickcount != last_tick)
				{
					last_tick = global_vars_base->tickcount;
					const auto backup_layers = *local->get_animation_layers();

					animation_system->enable_bones = true;
					local->update_clientside_anim();
					animation_system->enable_bones = false;

					if (!client_state->choked_commands)
					{
						animation_system->local_animation.abs_ang.y = state->goal_feet_yaw;
						animation_system->local_animation.poses = local->get_pose_parameter();
					}

					*local->get_animation_layers() = backup_layers;
				}

				local->set_abs_angles(c_vector3d(0, animation_system->local_animation.abs_ang.y, 0));
				local->get_pose_parameter() = animation_system->local_animation.poses;
			}
		}
	}
	
	_frame_stage_notify(client, stage);

	if (stage == frame_render_start)
	{
		if (local->is_alive()) {
			antiaim->increment_visual_progress();
			c_miscellaneous::set_viewmodel_parameters();
			c_miscellaneous::engine_radar();
			c_miscellaneous::view_model();
		}
		c_miscellaneous::remove_flash();
		c_miscellaneous::remove_smoke();

//		c_miscellaneous::preserve_killfeed();
	}

	if (stage == frame_net_update_postdataupdate_start)
	{
		c_miscellaneous::modify_player_model();
		if (view_model && !input->camera_in_third_person)
		{
			view_model->get_anim_time() = anim_time;
			view_model->get_cycle() = cycle;
		}
		//animation_system->server_layers = *local->get_animation_layers();
	}

	if (view_model)
	{
		cycle = view_model->get_cycle();
		anim_time = view_model->get_anim_time();
	}

	if (stage == frame_net_update_end)
		animation_system->post_player_update();
}

__declspec(naked) void create_move(int sequence_number, float input_sample_frametime, bool active)
{
	__asm
	{
		push ebx
		push esp
		push[esp + 0x14]
		push[esp + 0x14]
		push[esp + 0x14]
		call c_client::create_move
		pop ebx
		ret 0x0c
	}
}