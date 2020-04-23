#include "c_miscellaneous.h"
#include "../sdk/c_material_system.h"
#include <cstdint>
#include "../sdk/c_engine_client.h"
#include "../utils/c_config.h"
#include "../utils/c_hotkeys.h"
#include "../sdk/c_cs_player.h"
#include "../sdk/c_input.h"
#include "../sdk/c_cvar.h"
#include "../menu/c_menu.h"
#include "c_antiaim.h"
#include "../sdk/c_client_state.h"
#include "../hooks/c_events.h"
#include "../sdk/c_weapon_system.h"
#include "../utils/math.h"
#include "../hooks/c_client.h"
#include "c_prediction_system.h"
#include "../sdk/c_game_rules.h"

void c_miscellaneous::disable_post_processing()
{
	if (!engine_client()->is_ingame() || !engine_client()->is_connected() || !config->Visuals.nopostprocsess)
		return;

	static auto& disable_post_processing_override = **reinterpret_cast<bool**>(reinterpret_cast<uint32_t>(
		sig("client_panorama.dll", "83 EC 4C 80 3D") + 5));

	static auto blur_overlay = find_mat("dev/scope_bluroverlay");
	static auto lens_dirt = find_mat("models/weapons/shared/scope/scope_lens_dirt");

	disable_post_processing_override = true;
	blur_overlay->set_material_var_flag(material_var_no_draw, true);
	lens_dirt->set_material_var_flag(material_var_no_draw, true);
}

void c_miscellaneous::unlock_client_command_buffer()
{

	if (!engine_client()->is_ingame() || !engine_client()->is_connected() || !c_cs_player::get_local_player() || !c_cs_player::get_local_player()->is_alive())
		return;

	static const auto new_commands = reinterpret_cast<uint8_t*>(reinterpret_cast<uint32_t>(
		sig("engine.dll", "55 8B EC A1 ? ? ? ? 81 EC ? ? ? ? B9 ? ? ? ? 53 8B 98")) + 0xbd);
	// testing 132
	if ((config->Ragebot.enable && *new_commands == 24)
		|| (!config->Ragebot.enable && *new_commands == 15))
		return;

	DWORD old;
	PVOID address = new_commands;
	ULONG size = sizeof(uintptr_t);
	syscall(NtProtectVirtualMemory)(current_process, &address, &size, PAGE_READWRITE, &old);
	address = new_commands;
	size = sizeof(uintptr_t);
	*new_commands = config->Ragebot.enable ? 24 : 15;
	syscall(NtProtectVirtualMemory)(current_process, &address, &size, old, &old);
}

void c_miscellaneous::remove_visual_recoil(c_view_setup& view)
{
	if (!engine_client()->is_ingame() || !engine_client()->is_connected() || !c_cs_player::get_local_player() || !c_cs_player::get_local_player()->is_alive())
		return;

	const auto local = c_cs_player::get_local_player();

	if (!local || !local->is_alive() || input->camera_in_third_person)
		return;

	view.angles -= local->get_punch_angle() * .9f + local->get_view_punch_angle();
}

void c_miscellaneous::set_viewmodel_parameters()
{
	if (!engine_client()->is_ingame() || !engine_client()->is_connected() || !c_cs_player::get_local_player() || !c_cs_player::get_local_player()->is_alive())
		return;

	const auto local = c_cs_player::get_local_player();

	if (!local || !local->is_alive())
		return;

	static const auto cl_wpn_sway_interp = cvar()->find_var(_("cl_wpn_sway_interp"));

	if (!cl_wpn_sway_interp)
		return;

	static const auto cl_bob_lower_amt = cvar()->find_var(_("cl_bob_lower_amt"));

	if (!cl_bob_lower_amt)
		return;

	cl_wpn_sway_interp->value = 0.f;
	cl_bob_lower_amt->value = 0.f;
	


	static const auto weapon_debug_spread_show = cvar()->find_var(_("weapon_debug_spread_show"));
	static const auto weapon_debug_spread_gap = cvar()->find_var(_("weapon_debug_spread_gap"));
	if (config->Visuals.forcecrosshair)
	{
		if (weapon_debug_spread_show && c_cs_player::get_local_player()->is_scoped())
			weapon_debug_spread_show->set_value(0);
		else
		{
			weapon_debug_spread_show->set_value(1);
			weapon_debug_spread_gap->set_value(10);
		}
	}
	else
	{
		weapon_debug_spread_show->set_value(0);
		weapon_debug_spread_gap->set_value(0);
	}

}

void c_miscellaneous::set_camera_to_thirdperson()
{

	if (!engine_client()->is_ingame() || !engine_client()->is_connected() || !c_cs_player::get_local_player())
		return;

	static auto in_thirdperson = false;
	static auto require_reset = false;
	static bool click = false;

	auto open_menu = []() -> void
	{
		static bool is_down = false;
		static bool is_clicked = false;

		if (GetAsyncKeyState(config->Visuals.thirdperson))
		{
			is_clicked = false;
			is_down = true;
		}
		else if (!GetAsyncKeyState(config->Visuals.thirdperson) && is_down)
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
			in_thirdperson = !in_thirdperson;

		}
	};

	open_menu();

	static const auto cam_idealdist = cvar()->find_var(_("cam_idealdist"));
	static const auto cam_collision = cvar()->find_var(_("cam_collision"));

	const auto local = c_cs_player::get_local_player();

	if (!local)
		return;

	if (!config->Visuals.thirdperson_enable) {
		input->camera_in_third_person = false;
		return;
	}

	static auto percent = 0.f;
	if (local->is_alive() && in_thirdperson)
	{
		input->camera_in_third_person = true;

		percent = std::clamp(percent + global_vars_base->frametime * 8.f, 0.f, 1.f);

		cam_idealdist->set_value((config->Visuals.thirdperson_dist + 20.f) * percent);
		cam_collision->set_value(1);
	}
	else if (input->camera_in_third_person)
	{
		percent = in_thirdperson ? 1.f : 0.f;
		input->camera_in_third_person = false;
	}

	if (local->is_alive() || !config->Ragebot.enable)
	{
		require_reset = false;
		return;
	}

	if (require_reset)
		local->get_observer_mode() = 5;

	if (local->get_observer_mode() == 4)
		require_reset = true;


}

void c_miscellaneous::no_visual_recoil(c_view_setup* view_setup) {

	if (!engine_client()->is_ingame() || !engine_client()->is_connected() || !c_cs_player::get_local_player() || !c_cs_player::get_local_player()->is_alive())
		return;

	c_vector3d viewPunch = c_cs_player::get_local_player()->get_view_punch_angle();
	c_vector3d aimPunch = c_cs_player::get_local_player()->get_punch_angle();

	static bool once = false;

	if (config->Visuals.novisrecoil && !once)
	{
		view_setup->angles[0] -= (viewPunch[0] + (aimPunch[0] * 2 * 0.4499999f));
		view_setup->angles[1] -= (viewPunch[1] + (aimPunch[1] * 2 * 0.4499999f));
		view_setup->angles[2] -= (viewPunch[2] + (aimPunch[2] * 2 * 0.4499999f));
		once = true;
	}
	else if (!config->Visuals.novisrecoil) {
		view_setup->angles[0] += (viewPunch[0] + (aimPunch[0] * 2 * 0.4499999f));
		view_setup->angles[1] += (viewPunch[1] + (aimPunch[1] * 2 * 0.4499999f));
		view_setup->angles[2] += (viewPunch[2] + (aimPunch[2] * 2 * 0.4499999f));
		once = false;
	}
}

void c_miscellaneous::no_zoom(c_view_setup* view_setup) {

	if (!engine_client()->is_ingame() || !engine_client()->is_connected() || !c_cs_player::get_local_player() || !c_cs_player::get_local_player()->is_alive())
		return;

	if (c_cs_player::get_local_player()->is_scoped() && config->Visuals.remove_zoom)
		view_setup->fov = config->Visuals.fieldofview ? config->Visuals.fieldofview : 90;
	if (!c_cs_player::get_local_player()->is_scoped() && config->Visuals.fieldofview > 0)
		view_setup->fov = config->Visuals.fieldofview;
}

void c_miscellaneous::remove_smoke()
{
	if (!engine_client()->is_ingame() || !engine_client()->is_connected() || !c_cs_player::get_local_player())
		return;

	static const auto smoke_count = *reinterpret_cast<uint32_t**>(uint32_t(sig("client_panorama.dll", "A3 ? ? ? ? 57 8B CB")) + 1);

	*smoke_count = 0;

	const auto Smokegrenade = material_system()->find_material("particle/vistasmokev1/vistasmokev1_smokegrenade");
	const auto Smokedusk = material_system()->find_material("particle/vistasmokev1/vistasmokev1_emods_impactdust");
	const auto Smokemods = material_system()->find_material("particle/vistasmokev1/vistasmokev1_emods");

	if (!Smokegrenade || Smokegrenade->is_error_material() ||
		!Smokedusk || Smokedusk->is_error_material() ||
		!Smokemods || Smokemods->is_error_material()) {
		return;
	}

	Smokegrenade->incrementreferencecount();
	Smokedusk->incrementreferencecount();
	Smokemods->incrementreferencecount();

	Smokegrenade->set_material_var_flag(material_var_no_draw, config->Visuals.nosmoke);
	Smokedusk->set_material_var_flag(material_var_no_draw, config->Visuals.nosmoke);
	Smokemods->set_material_var_flag(material_var_no_draw, config->Visuals.nosmoke);

}

void c_miscellaneous::remove_flash()
{
	if (!engine_client()->is_ingame() || !engine_client()->is_connected() || !c_cs_player::get_local_player() || !c_cs_player::get_local_player()->is_alive())
		return;

	if (!config->Visuals.noflash)
		return;

	const auto local = c_cs_player::get_local_player();

	if (!local || !local->is_alive())
		return;

	local->get_flash_alpha() = 0.f;
}

void c_miscellaneous::set_buttons_for_direction(c_user_cmd* cmd)
{
	cmd->buttons &= ~c_user_cmd::forward;
	cmd->buttons &= ~c_user_cmd::back;
	if (cmd->forwardmove > 0.f)
		cmd->buttons |= c_user_cmd::forward;
	else if (cmd->forwardmove < 0.f)
		cmd->buttons |= c_user_cmd::back;

	cmd->buttons &= ~c_user_cmd::move_right;
	cmd->buttons &= ~c_user_cmd::move_left;
	if (cmd->sidemove > 0.f)
		cmd->buttons |= c_user_cmd::move_right;
	else if (cmd->sidemove < 0.f)
		cmd->buttons |= c_user_cmd::move_left;
}

void c_miscellaneous::engine_radar()
{
	if (!config->Ragebot.enable)
		return;

	client_entity_list()->for_each_player([](c_cs_player* player) -> void
	{
		if (!player->is_dormant() && player->is_alive())
			player->is_spotted() = true;
	});
}

void c_miscellaneous::misc_cm(c_user_cmd* cmd) {

	if (!engine_client()->is_ingame() || !engine_client()->is_connected() || !c_cs_player::get_local_player())
		return;

	if (c_cs_player::get_local_player()->is_alive()) {

		if (GetAsyncKeyState(config->Misc.airstuck_key) && config->Misc.airstuck_enable) {
			cmd->command_number = INT_MAX;
			cmd->tick_count = INT_MAX;
		}

		cmd->buttons |= c_user_cmd::bull_rush;

		if (GetAsyncKeyState(config->Ragebot.fakeduck) && config->Ragebot.fakeduck_enable)
		{
			const auto local = c_cs_player::get_local_player();

			if (!local)
				return;

			if (client_state->choked_commands > 6)
				cmd->buttons |= c_user_cmd::flags::duck;
			else
				cmd->buttons &= ~c_user_cmd::flags::duck;

			//prediction_system->repredict(local, cmd);
		}

		const auto weapon = reinterpret_cast<c_base_combat_weapon*>(
			client_entity_list()->get_client_entity_from_handle(c_cs_player::get_local_player()->get_current_weapon_handle()));

		if (!weapon)
			return;
		const auto info = weapon_system->get_weapon_data(weapon->get_item_definition());

		if (!info)
			return;

		static auto current_hand_pos = cvar()->find_var(_("cl_righthand"))->value;
		static auto flip_hand = current_hand_pos == 0 ? 1 : 0;

		if (config->Visuals.leftknife) {
			if (info->WeaponType == weapontype_knife) {
				cvar()->find_var("cl_righthand")->set_value(flip_hand);
			}
			else {
				cvar()->find_var("cl_righthand")->set_value(!flip_hand);
			}
		}

		auto fast_sqr = [](float num) -> float {

			unsigned int i = *(unsigned int*)&num;
			i += 127 << 23;
			i >>= 1;
			return *(float*)&i;
		};

		auto clamp_movement = [&](c_user_cmd* cmd, float max_speed) -> void {

			const auto local = c_cs_player::get_local_player();

			if (!local)
				return;

			if (max_speed <= 0.f)
				return;
			float fSpeed = (float)(fast_sqr(square(cmd->forwardmove) + square(cmd->sidemove) + square(cmd->upmove)));
			if (fSpeed <= 0.f)
				return;
			if (cmd->buttons & c_user_cmd::duck)
				max_speed *= 2.94117647f; // TO DO: Maybe look trough the leaked sdk for an exact value since this is straight out of my ass...
			if (fSpeed <= max_speed)
				return;
			float fRatio = max_speed / fSpeed;
			cmd->forwardmove *= fRatio;
			cmd->sidemove *= fRatio;
			cmd->upmove *= fRatio;

			prediction_system->repredict(local, cmd);
		};

		if (config->Misc.slowwalk && GetAsyncKeyState(config->Misc.slowwalk_key)) {
			if (c_cs_player::get_local_player()->get_flags() & c_cs_player::flags::on_ground)
				clamp_movement(cmd, config->Misc.slowwalkspeed / 4);
		}

		engine_radar();
		
		const auto value = cvar()->find_var(_("sv_showimpacts"));
		static bool enabled = false;

		if (config->Visuals.showimpacts) {
			value->set_value(1);
			enabled = true;
		}
		else if (!config->Visuals.showimpacts && enabled) {
			value->set_value(0);
			enabled = false;
		}

		const auto cl_grenadepreview = cvar()->find_var(_("cl_grenadepreview"));
		static bool cl_grenadepreview_enabled = false;

		if (config->Visuals.showgrenade) {
			cl_grenadepreview->set_value(1);
			cl_grenadepreview_enabled = true;
		}
		else if (!config->Visuals.showimpacts && cl_grenadepreview_enabled) {
			cl_grenadepreview->set_value(0);
			cl_grenadepreview_enabled = false;
		}
	}

	preserve_killfeed();

	static const auto value = cvar()->find_var(_("cl_ragdoll_gravity"));
	static const auto push = cvar()->find_var(_("phys_pushscale"));
	static bool saved = false;
	bool option = config->Visuals.ragdoll;

	if (saved != option) {
		value->set_value(option ? -10 : 600);
		push->set_value(option ? 0 : 300);

		saved = option;
	}

	std::vector<std::string> tag_table =
	{ //lmao every heard of curtime and substring
		"-BETA-",
		"+LAGSYNC+",
	};

	static float iLastTime = 0.f;
	float iCurrentTime = global_vars_base->curtime * 2;

	if (iCurrentTime != iLastTime)
	{

		auto ChangeClantag = []
		(std::string tag, std::string name)
		{
			static std::string oClantag;

			if (oClantag == tag)
				return;

			static auto ChangeClantagFn = reinterpret_cast<void(__fastcall*)(const char*, const char*)>(sig("engine.dll", "53 56 57 8B DA 8B F9 FF 15"));

			if (!ChangeClantagFn)
				return;

			ChangeClantagFn(tag.c_str(), name.c_str());

			oClantag = tag;
		};

		if (config->Misc.clantag)
		{
			auto tag = tag_table[int(global_vars_base->curtime * 2) % tag_table.size()] + "";

			ChangeClantag(tag, "lagsync");
		}
		else
			ChangeClantag("", "lagsync");
	}

	iLastTime = iCurrentTime;

}

void c_miscellaneous::view_model()
{
	if (!engine_client()->is_ingame() || !engine_client()->is_connected() || !c_cs_player::get_local_player() || !c_cs_player::get_local_player()->is_alive())
		return;

	if (config->Visuals.viewmodelfov != 0)cvar()->find_var("viewmodel_fov")->set_value(config->Visuals.viewmodelfov);
	if (config->Visuals.view_setup_xoffset != 0)cvar()->find_var("viewmodel_offset_x")->set_value(config->Visuals.view_setup_xoffset);
	if (config->Visuals.view_setup_yoffset != 0)cvar()->find_var("viewmodel_offset_y")->set_value(config->Visuals.view_setup_yoffset);
	if (config->Visuals.view_setup_zoffset != 0)cvar()->find_var("viewmodel_offset_z")->set_value(config->Visuals.view_setup_zoffset);

	static auto true_acspect_ratio = cvar()->find_var("r_aspectratio")->get_float();

	if (config->Visuals.viewmodelratio != 0)
		cvar()->find_var("r_aspectratio")->set_value(config->Visuals.viewmodelratio);
	else
		cvar()->find_var("r_aspectratio")->set_value(true_acspect_ratio);

	static int iBloomValue = 1;
	static float iScalarValue = 0.1;
	if (iBloomValue != config->Visuals.bloom_cvar_scale) {
		const auto bloom_scale = cvar()->find_var("mat_bloomscale");
		bloom_scale->value = config->Visuals.bloom_cvar_scale;
		iBloomValue = config->Visuals.bloom_cvar_scale;
	}
	if (iScalarValue != config->Visuals.bloom_factor) {
		const auto bloom_factor = cvar()->find_var("mat_bloom_scalefactor_scalar");
		bloom_factor->value = config->Visuals.bloom_factor;
		iScalarValue = config->Visuals.bloom_factor;
	}

}

void c_miscellaneous::modify_player_model() {

	if (!engine_client()->is_ingame() || !engine_client()->is_connected() || !c_cs_player::get_local_player() || !c_cs_player::get_local_player()->is_alive())
		return;

	if (c_cs_player::get_local_player())
	{

		std::string player_model = "";

		switch (config->Visuals.player_model) {
		case 1:  player_model = _("models/player/custom_player/legacy/tm_balkan_variantb.mdl"); break;
		case 2:  player_model = _("models/player/custom_player/legacy/tm_balkan_variantc.mdl"); break;
		case 3:  player_model = _("models/player/custom_player/legacy/tm_balkan_variantd.mdl"); break;
		case 4:  player_model = _("models/player/custom_player/legacy/ctm_fbi_variantb.mdl"); break;
		case 5:  player_model = _("models/player/custom_player/legacy/ctm_fbi_variantf.mdl"); break;
		case 6:  player_model = _("models/player/custom_player/legacy/ctm_fbi_varianth.mdl"); break;
		case 7:  player_model = _("models/player/custom_player/legacy/ctm_st6_varianta.mdl"); break;
		case 8:  player_model = _("models/player/custom_player/legacy/ctm_st6.mdl"); break;
		case 9:  player_model = _("models/player/custom_player/legacy/ctm_heavy.mdl"); break;
		case 10: player_model = _("models/player/custom_player/legacy/ctm_st6_variantb.mdl"); break;
		case 11: player_model = _("models/player/custom_player/legacy/ctm_st6_variantc.mdl"); break;
		case 12: player_model = _("models/player/custom_player/legacy/ctm_st6_variantd.mdl"); break;
		case 13: player_model = _("models/player/custom_player/legacy/ctm_st6_variante.mdl"); break;
		case 14: player_model = _("models/player/custom_player/legacy/ctm_st6_variantg.mdl"); break;
		case 15: player_model = _("models/player/custom_player/legacy/ctm_st6_varianti.mdl"); break;
		case 16: player_model = _("models/player/custom_player/legacy/ctm_st6_variantm.mdl"); break;
		case 17: player_model = _("models/player/custom_player/legacy/tm_balkan_variantg.mdl"); break;
		case 18: player_model = _("models/player/custom_player/legacy/tm_balkan_varianth.mdl"); break;
		case 19: player_model = _("models/player/custom_player/legacy/tm_balkan_varianti.mdl"); break;
		case 20: player_model = _("models/player/custom_player/legacy/tm_balkan_variantj.mdl"); break;
		case 21: player_model = _("models/player/custom_player/legacy/tm_phoenix_variantf.mdl"); break;
		case 22: player_model = _("models/player/custom_player/legacy/tm_phoenix_variantg.mdl"); break;
		case 23: player_model = _("models/player/custom_player/legacy/tm_phoenix_varianth.mdl"); break;
		default: player_model = _("models/player/custom_player/legacy/tm_balkan_varianta.mdl"); break;
		}

		if (player_model.length() > 0 && model_info_client())
		{
			const auto model_index = model_info_client()->get_model_index(player_model.c_str());

			if (model_index)
				c_cs_player::get_local_player()->set_model_index(model_index);
		}

	}
}

template<class T>
static T* FindHudElement(const char* name)
{
	static auto pThis = *reinterpret_cast<DWORD**> (sig("client_panorama.dll", "B9 ? ? ? ? E8 ? ? ? ? 8B 5D 08") + 1);

	static auto find_hud_element = reinterpret_cast<DWORD(__thiscall*)(void*, const char*)>(sig("client_panorama.dll", "55 8B EC 53 8B 5D 08 56 57 8B F9 33 F6 39 77 28"));
	return (T*)find_hud_element(pThis, name);
}

bool ReallocatedDeathNoticeHUD = false;
static void(__thiscall* ClearDeathNotices)(DWORD);

void c_miscellaneous::preserve_killfeed() {
	if (!engine_client()->is_ingame() || !engine_client()->is_connected())
		return;

	if (c_cs_player::get_local_player())
	{
		static int old_death_notice = -1;

		const DWORD* deathNotice = FindHudElement<DWORD>("CCSGO_HudDeathNotice"); //Check client_panorama.dll
		//Reallocate after game change ^
		if (!deathNotice)
			return;

		if (config->Visuals.preserve_killfeed) {
			if ((c_cs_player::get_local_player()->get_flags() & c_cs_player::frozen || game_rules->is_freeze_period()) && deathNotice - 20) {
				if (!ClearDeathNotices)
					ClearDeathNotices = (void(__thiscall*)(DWORD))(sig("client_panorama.dll", "55 8B EC 83 EC 0C 53 56 8B 71 58"));
				if (ClearDeathNotices) {
					ClearDeathNotices(((DWORD)deathNotice - 20));
				}
			}
			else {
				float* localDeathNotice = (float*)((DWORD)deathNotice + 0x50);
				if (localDeathNotice) {
					*localDeathNotice = FLT_MAX;
					if (old_death_notice == -1)
						old_death_notice = *localDeathNotice;
				}
			}
		}
		else if (!config->Visuals.preserve_killfeed && old_death_notice != -1) {
			float* localDeathNotice = (float*)((DWORD)deathNotice + 0x50);
			if (localDeathNotice) 
				*localDeathNotice = old_death_notice;	
		}
	}
}

void c_miscellaneous::shift_ticks(c_user_cmd* cmd, bool send_packet) {

	const auto weapon = reinterpret_cast<c_base_combat_weapon*>(
		client_entity_list()->get_client_entity_from_handle(c_cs_player::get_local_player()->get_current_weapon_handle()));

	const auto info = weapon_system->get_weapon_data(weapon->get_item_definition());

	auto cfg = config->Ragebot.pistol;

	if (info->get_weapon_id() == weapon_g3sg1 || info->get_weapon_id() == weapon_scar20) 
		cfg = config->Ragebot._auto;
	else if (info->get_weapon_id() == weapon_ssg08) 
		cfg = config->Ragebot.scout;
	else if (info->get_weapon_id() == weapon_awp) 
		cfg = config->Ragebot.awp;
	else if (info->get_weapon_id() == weapon_deagle || info->get_weapon_id() == weapon_revolver) 
		cfg = config->Ragebot.heavy;
	else if (info->WeaponType == weapontype_pistol) 
		cfg = config->Ragebot.pistol;
	else if (info->get_weapon_id() == weapon_taser) 
		cfg = config->Ragebot.taser;
	else
		cfg = config->Ragebot.other;
	

	if (cfg.exploits == 0  || weapon->get_item_definition() == weapon_revolver || info->WeaponType == weapontype_knife || info->WeaponType == weapontype_grenade || info->WeaponType == weapontype_c4 && weapon->get_item_definition() == weapon_hegrenade && weapon->get_item_definition() == weapon_molotov && weapon->get_item_definition() == weapon_incgrenade && weapon->get_item_definition() == weapon_smokegrenade)
	{
		exploits.exploiting = true;
		return;
	}

	static auto max_shift_ticks = 11;

	if (cfg.exploits == 1)
		max_shift_ticks = 11;
	else if (cfg.exploits == 2)
		max_shift_ticks = 9;

	static auto did_shot = false;

	bool weapon_can_shoot = true;

	//static auto tick_base = c_cs_player::get_local_player()->get_tick_base();
	//static auto gl_static_time = global_vars_base->curtime;

	//if ((!GetAsyncKeyState(config->Ragebot.fakeduck) && config->Ragebot.fakeduck_enable) || !config->Ragebot.fakeduck_enable) {
	//	tick_base = c_cs_player::get_local_player()->get_tick_base();
	//	gl_static_time = global_vars_base->curtime;
	//}
	//else {
	//	const auto diff = global_vars_base->curtime - gl_static_time;
	//	tick_base = c_cs_player::get_local_player()->get_tick_base() + diff;
	//}

	if (cfg.exploits > 0 && !(c_cs_player::get_local_player()->get_flags() & c_cs_player::flags::frozen)) //charge doubletap
	{
		auto recharge = []()
		{		static auto needs_recharge = false;

			if (client_state->choked_commands > 6 || net_channel->choked_packets > 6)
			{
			   needs_recharge = true;
          	}

		if (needs_recharge)
		{
			if (!GetAsyncKeyState(config->Ragebot.fakeduck)) {
     				Sleep(2);
					max_shift_ticks *= 2;
					needs_recharge = false;
					_events.push_back(_event("exploits recharged!"));//logging->info(_("exploits recharged!"));
				 }
			}
		};

		recharge();
	  }


	const auto tick_base = c_cs_player::get_local_player()->get_tick_base();
	const float curtime = ticks_to_time(tick_base - (max_shift_ticks));

	if (curtime < c_cs_player::get_local_player()->get_next_attack())
		weapon_can_shoot = false;

	if (curtime < weapon->get_next_primary_attack())
		weapon_can_shoot = false;

	if (send_packet)
	{
		if ((weapon_can_shoot && cmd->buttons & c_user_cmd::flags::attack) || did_shot) {
			exploits.tickbaseshift = (max_shift_ticks);
			exploits.exploiting = true;
			did_shot = false;
		}
	}
	else
	{
		if (weapon_can_shoot && cmd->buttons & c_user_cmd::flags::attack) 
			did_shot = true;
		exploits.exploiting = false;
	}
}

void c_miscellaneous::buybot()
{
	const auto local = c_cs_player::get_local_player();

	if (!local || !config->Misc.buybot)
		return;

	const auto weapon = reinterpret_cast<c_base_combat_weapon*>(client_entity_list()->get_client_entity_from_handle(local->get_current_weapon_handle()));

	if (!weapon) return;
	const auto info = weapon_system->get_weapon_data(weapon->get_item_definition());

	const auto is_autosniper = info->get_weapon_id() == weapon_scar20 || info->get_weapon_id() == weapon_g3sg1;

	if (config->Misc.buy_bot_scar20)
	{
		if (!is_autosniper)
		{
			engine_client()->clientcmd_unrestricted(_("buy scar20; buy primammo;"), 0);
		}
		if (is_autosniper)
		{
			engine_client()->clientcmd_unrestricted(_("buy primammo; buy secmammo;"), 0);
		}
	}
	if (config->Misc.buy_bot_g3sg1)
	{
		if (!is_autosniper)
		{
			engine_client()->clientcmd_unrestricted(_("buy g3sg1; buy primammo;"), 0);
		}
		if (is_autosniper)
		{
			engine_client()->clientcmd_unrestricted(_("buy primammo; buy secmammo;"), 0);
		}
	}

	if (config->Misc.buy_bot_ssg08)
	{
		engine_client()->clientcmd_unrestricted(
			_("buy ssg08; buy primammo;"), 0);
	}
	if (config->Misc.buy_bot_awp)
	{
		engine_client()->clientcmd_unrestricted(
			_("buy awp; buy primammo;"), 0);
	}

	if (config->Misc.buy_bot_elite)
	{
		engine_client()->clientcmd_unrestricted(
			_("buy elite; buy secmammo;"), 0);
	}
	if (config->Misc.buy_bot_p250)
	{
		engine_client()->clientcmd_unrestricted(
			_("buy p250; buy secmammo;"), 0);
	}
	if (config->Misc.buy_bot_tec9)
	{
		engine_client()->clientcmd_unrestricted(
			_("buy tec9; buy fiveseven; buy secmammo;"), 0);
	}
	if (config->Misc.buy_bot_deg)
	{
		engine_client()->clientcmd_unrestricted(
			_("buy deagle; buy secmammo;"), 0);
	}
	if (config->Misc.buy_bot_vest)
	{
		engine_client()->clientcmd_unrestricted(
			_("buy vest;"), 0);
	}
	if (config->Misc.buy_bot_vesthelm)
	{
		engine_client()->clientcmd_unrestricted(
			_("buy vesthelm;"), 0);
	}
	if (config->Misc.buy_bot_molotov)
	{
		engine_client()->clientcmd_unrestricted(
			_("buy molotov; "), 0);
	}
	if (config->Misc.buy_bot_incgrenade)
	{
		engine_client()->clientcmd_unrestricted(
			_("buy incgrenade; "), 0);
	}
	if (config->Misc.buy_bot_hegrenade)
	{
		engine_client()->clientcmd_unrestricted(
			_("buy hegrenade; "), 0);
	}
	if (config->Misc.buy_bot_smokegrenade)
	{
		engine_client()->clientcmd_unrestricted(
			_("buy smokegrenade; "), 0);
	}
	if (config->Misc.buy_bot_taser)
	{
		engine_client()->clientcmd_unrestricted(
			_("buy taser; "), 0);
	}
	if (config->Misc.buy_bot_defuser)
	{
		engine_client()->clientcmd_unrestricted(
			_("buy defuser; "), 0);
	}

}