#include "../menu/c_menu.h"
#include "config.h"
#include "ShlObj_core.h"
float max_array[4] = { 1.f, 1.f, 1.f,1.f };
#define array(a) memcpy(a, max_array, sizeof(a));

void C_Config::Setup()
{


	SetupValue(config->Ragebot.enable, false, "ragebotenable");

	int weapon_index = 0;

	auto weapon_save = [&](Menu22::ragebot::weapon_config& _config) -> void {

		weapon_index++;
		std::string name = "";
		switch (weapon_index) {
		case 1: name = "pistol "; break;
		case 2: name = "auto "; break;
		case 3: name = "scout "; break;
		case 4: name = "awp "; break;
		case 5: name = "heavy "; break;
		case 6: name = "other "; break;
		case 7: name = "taser "; break;
		}
		SetupValue(_config.exploits, 0, name + "exploits");
		SetupValue(_config.hitchance, 0, name + "hitchance");
		SetupValue(_config.min_dmg_vis, 0, name + "mininum damage visible");
		SetupValue(_config.min_dmg, 0, name + "mininum damage hidden");
		SetupValue(_config.head_scale, 0, name + "head scale");
		SetupValue(_config.chest_scale, 0, name + "chest scale");
		SetupValue(_config.stomach_scale, 0, name + "stomach scale");
		SetupValue(_config.pelvis_scale, 0, name + "pelvis scale");
		SetupValue(_config.legs_scale, 0, name + "legs scale");
		SetupValue(_config.feet_scale, 0, name + "feet scale");
						  
		SetupValue(_config.hitscan_head, false, name + "hitscan head");
		SetupValue(_config.multipoint_head, false, name + "multipoint head");
		SetupValue(_config.hitscan_chest, false, name + "hitscan chest");
		SetupValue(_config.multipoint_chest, false, name + "multipoint chest");
		SetupValue(_config.hitscan_stomach, false, name + "hitscan stomach");
		SetupValue(_config.multipoint_stomach, false, name + "multipoint stomach");
		SetupValue(_config.hitscan_pelvis, false, name + "hitscan pelvis");
		SetupValue(_config.multipoint_pelvis, false, name + "multipoint pelvis");
		SetupValue(_config.hitscan_legs, false, name + "hitscan legs");
		SetupValue(_config.multipoint_legs, false, name + "multipoint legs");
		SetupValue(_config.hitscan_feet, false, name + "hitscan feet");
		SetupValue(_config.multipoint_feet, false, name + "multipoint feet");
		SetupValue(_config.body_aim_lethal, false, name + "baim lethal");
		SetupValue(_config.body_aim_in_air, false, name + "baim air");
		SetupValue(_config.body_aim_slow_walk, false, name + "baim slow walk");
		SetupValue(_config.body_aim_if_not_on_shot, false, name + "baim on shot");
		SetupValue(_config.head_aim_only_while_firing, false, name + "head aim on shot");
		SetupValue(_config.body_after_x_missed_enable, 0, name + "body after x missed enable");
		SetupValue(_config.body_after_x_missed, 0, name + "body after x missed");
		SetupValue(_config.body_aim_key, 0, name + "body key");
		SetupValue(_config.body_aim_key_enable, 0, name + "body key enable");
		SetupValue(_config.health_override, 0, name + "override on health");
		SetupValue(_config.hitbox_prefer, 0, name + "prefer hitbox");
		SetupValue(_config.delay_shot, 0, name + "shot dely");
		SetupValue(_config.hitbox_override, 0, name + "hitbox override");
		SetupValue(_config.hitbox_override_key, 0, name + "hitbox override key");
		SetupValue(_config.static_point, false, name + "static point scale");
		SetupValue(_config.safe_point_override_enable, false, name + "safe point override");
		SetupValue(_config.save_point_override, 0, name + "save point override key");

	};

	weapon_save(config->Ragebot.pistol);
	weapon_save(config->Ragebot._auto);
	weapon_save(config->Ragebot.scout);
	weapon_save(config->Ragebot.awp);
	weapon_save(config->Ragebot.heavy);
	weapon_save(config->Ragebot.other);
	weapon_save(config->Ragebot.taser);

	SetupValue(config->Ragebot.priority, false, "priority setting");
	SetupValue(config->Ragebot.autofire, false, "auto fire");
	SetupValue(config->Ragebot.autoscope, false, "auto scope");
	SetupValue(config->Ragebot.autostop, false, "auto stop");
	SetupValue(config->Ragebot.autoknife, false, "auto knife");
	SetupValue(config->Ragebot.autozeus, false, "auto zeus");
	SetupValue(config->Ragebot.autorevolver, false, "auto revolver");
	SetupValue(config->Ragebot.no_recoil, false, "remove recoil");
	SetupValue(config->Ragebot.exploits, 0, "exploits");
	SetupValue(config->Ragebot._antiaim, false, "antiaim enable");
	SetupValue(config->Ragebot.disableaaonknife, false, "disable antiaim knife");
	SetupValue(config->Ragebot.disableaaonfreeze, false, "disable antiaim knife");


	auto aa_save = [&](Menu22::ragebot::antiaim_config& _config, const char* name_char) -> void {

		std::string name = name_char;

		SetupValue(_config.pitch, 0, name + "pitch");
		SetupValue(_config.pitch_range, 0, name + "pitch jitter range");
		SetupValue(_config.yawbase, 0, name + "yaw base");
		SetupValue(_config.yaw, 0, name + "yaw");
		SetupValue(_config.yaw_jitter, false, name + "yaw jitter");
		SetupValue(_config.twist, false, name + "lby twist");
		SetupValue(_config.jitter_length, 0, name + "jitter length");
						  
		SetupValue(_config.fake_type, 0, name + "desync");
						  
		SetupValue(_config.max_delta, 0, name + "max_delta");
		SetupValue(_config.lean_break, 0, name + "lean percentage");
						  
		SetupValue(_config.desync_on_shot, false, name + "desync on shot");
		SetupValue(_config.avoid_overlap, false, name + "avoid overlap");						  
	};

	// its not like you save any better :P
	aa_save(config->Ragebot.stand, "stand");
	aa_save(config->Ragebot.moving, "moving");
	aa_save(config->Ragebot.air, "air");

	SetupValue(config->Ragebot.fakelag_enable, false, "fakelag enable");
	SetupValue(config->Ragebot.fakelag_onshot, false, "fakelag onshot");
	SetupValue(config->Ragebot.lag_type, 0, "lag type");

	SetupValue(config->Ragebot.stand_fakelag, 0, "lag stand");
	SetupValue(config->Ragebot.move_fakelag, 0, "lag moving");
	SetupValue(config->Ragebot.air_fakelag, 0, "lag air");

	SetupValue(config->Ragebot.manaul_override, false, "manaul override");
	SetupValue(config->Ragebot.left_enable, false, ("manual left enable"));
	SetupValue(config->Ragebot.right_enable, false, ("manual right enable"));
	SetupValue(config->Ragebot.back_enable, false, ("manual back enable"));

	SetupValue(config->Ragebot.left, 0, "manual left key");
	SetupValue(config->Ragebot.right, 0, "manual right key");
	SetupValue(config->Ragebot.back, 0, "manual back key");

	SetupValue(config->Ragebot.fl_boost_max_delta, false, "boost max delta");
	SetupValue(config->Ragebot.fl_boost_reload, false, "boost reload");
	SetupValue(config->Ragebot.fl_boost_shot, false, "boost shot");
	SetupValue(config->Ragebot.fl_boost_switch, false, "boost switch");
	SetupValue(config->Ragebot.fl_boost_scale, 0, "boost amount");

	SetupValue(config->Ragebot.fl_disable_reload, false, "disable max reload");
	SetupValue(config->Ragebot.fl_disable_shot, false, "disable shot");
	SetupValue(config->Ragebot.fl_disable_switch, false, "disable switch");

	SetupValue(config->Ragebot.fake_invert, 0, "fake invert type");
	SetupValue(config->Ragebot.desync_invert, 0, "fake invert key");

	auto save_color = [&](float* color, const char* name) -> void {
		bool once = false;

		if (!once) {
			array(color); once = true;
		}

		std::string red = name; red += " red";
		SetupValue(color[0], 0, red);
		std::string green = name; green += " green";
		SetupValue(color[1], 0, green);
		std::string blue = name; blue += " blue";
		SetupValue(color[2], 0, blue);
		std::string alpha = name; alpha += " alpha";
		SetupValue(color[3], 1, alpha);
	};

	SetupValue(config->Visuals.local.visuals_enabled, false, "enable local visuals");
	SetupValue(config->Visuals.team.visuals_enabled, false, "enable team visuals");
	SetupValue(config->Visuals.enemy.visuals_enabled, false, "enable enemy visuals");

	SetupValue(config->Visuals.local.visuals_esp_box, false, "esp local box");
	save_color(config->Visuals.local.box_color, "local box color");
	SetupValue(config->Visuals.team.visuals_esp_box, false, "espt team box");
	save_color(config->Visuals.team.box_color, "team box color");
	SetupValue(config->Visuals.enemy.visuals_esp_box, false, "esp enemy box");
	save_color(config->Visuals.enemy.box_color, "enemy box color");

	SetupValue(config->Visuals.local.visuals_esp_name, false, "esp local name");
	save_color(config->Visuals.local.name_color, "local name color");
	SetupValue(config->Visuals.team.visuals_esp_name, false, "esp team name");
	save_color(config->Visuals.team.name_color, "team name color");
	SetupValue(config->Visuals.enemy.visuals_esp_name, false, "esp enemy name");
	save_color(config->Visuals.enemy.name_color, "team name color");

	SetupValue(config->Visuals.local.visuals_esp_weapon_icons, false, "esp local weapon icons");
	save_color(config->Visuals.local.icons_color, "local icons color");
	SetupValue(config->Visuals.team.visuals_esp_weapon_icons, false, "esp team weapon icons");
	save_color(config->Visuals.team.icons_color, "team icons color");
	SetupValue(config->Visuals.enemy.visuals_esp_weapon_icons, false, "esp enemy weapon icons");
	save_color(config->Visuals.enemy.icons_color, "enemy icons color");

	SetupValue(config->Visuals.local.visuals_esp_armor_flag, false, "esp local armor flag");
	save_color(config->Visuals.local.armor_flag_color, "local armor flag color");
	SetupValue(config->Visuals.team.visuals_esp_armor_flag, false, "esp team armor flag");
	save_color(config->Visuals.team.armor_flag_color, "team armor flag color");
	SetupValue(config->Visuals.enemy.visuals_esp_armor_flag, false, "esp enemy armor flag");
	save_color(config->Visuals.enemy.armor_flag_color, "enemy armor flag color");

	SetupValue(config->Visuals.local.visuals_esp_defuse_flag, false, "esp local defuse flag");
	save_color(config->Visuals.local.defuse_flag_color, "defuse flag color");
	SetupValue(config->Visuals.team.visuals_esp_defuse_flag, false, "esp team defuse flag");
	save_color(config->Visuals.team.defuse_flag_color, "defuse flag color");
	SetupValue(config->Visuals.enemy.visuals_esp_defuse_flag, false, "esp enemy defuse flag");
	save_color(config->Visuals.enemy.defuse_flag_color, "defuse flag color");

	SetupValue(config->Visuals.local.visuals_esp_fakeduck_flag, false, "esp local fakeduck flag");
	save_color(config->Visuals.local.fakeduck_flag_color, "fakeduck flag color");
	SetupValue(config->Visuals.team.visuals_esp_fakeduck_flag, false, "esp team fakeduck flag");
	save_color(config->Visuals.team.fakeduck_flag_color, "fakeduck flag color");
	SetupValue(config->Visuals.enemy.visuals_esp_fakeduck_flag, false, "esp enemy fakeduck flag");
	save_color(config->Visuals.enemy.fakeduck_flag_color, "fakeduck flag color");

	SetupValue(config->Visuals.local.visuals_esp_scope_flag, false, "esp local scope flag");
	save_color(config->Visuals.local.scope_flag_color, "scope flag color");
	SetupValue(config->Visuals.team.visuals_esp_scope_flag, false, "esp team scope flag");
	save_color(config->Visuals.team.scope_flag_color, "scope flag color");
	SetupValue(config->Visuals.enemy.visuals_esp_scope_flag, false, "esp enemy scope flag");
	save_color(config->Visuals.enemy.scope_flag_color, "scope flag color");

	SetupValue(config->Visuals.local.visuals_esp_flash_flag, false, "esp local flash flag");
	save_color(config->Visuals.local.flash_flag_color, "flash flag color");
	SetupValue(config->Visuals.team.visuals_esp_flash_flag, false, "esp team flash flag");
	save_color(config->Visuals.team.flash_flag_color, "flash flag color");
	SetupValue(config->Visuals.enemy.visuals_esp_flash_flag, false, "esp enemy flash flag");
	save_color(config->Visuals.enemy.flash_flag_color, "flash flag color");

	SetupValue(config->Visuals.local.visuals_esp_health, false, "esp local health");
	save_color(config->Visuals.local.health_color, "local health color");
	SetupValue(config->Visuals.team.visuals_esp_health, false, "esp team health");
	save_color(config->Visuals.team.health_color, "team health color");
	SetupValue(config->Visuals.enemy.visuals_esp_health, false, "esp enemy health");
	save_color(config->Visuals.enemy.health_color, "enemy health color");

	SetupValue(config->Visuals.local.visuals_esp_weapon, false, "esp local weapon");
	save_color(config->Visuals.local.weapon_color, "local weapon color");
	SetupValue(config->Visuals.team.visuals_esp_weapon, false, "esp team weapon");
	save_color(config->Visuals.team.weapon_color, "local team color");
	SetupValue(config->Visuals.enemy.visuals_esp_weapon, false, "esp enemy weapon");
	save_color(config->Visuals.enemy.weapon_color, "local enemy color");

	SetupValue(config->Visuals.local.visuals_esp_ammo, false, "esp local ammo");
	save_color(config->Visuals.local.ammo_color, "local ammo color");
	SetupValue(config->Visuals.team.visuals_esp_ammo, false, "esp team ammo");
	save_color(config->Visuals.team.ammo_color, "team ammo color");
	SetupValue(config->Visuals.enemy.visuals_esp_ammo, false, "esp enemy ammo");
	save_color(config->Visuals.enemy.ammo_color, "enemy ammo color");

	SetupValue(config->Visuals.local.visuals_esp_armor, false, "esp local armor");
	save_color(config->Visuals.local.armor_color, "enemy armor color");
	SetupValue(config->Visuals.team.visuals_esp_armor, false, "esp team armor");
	save_color(config->Visuals.team.armor_color, "team armor color");
	SetupValue(config->Visuals.enemy.visuals_esp_armor, false, "esp enemy armor");
	save_color(config->Visuals.enemy.armor_color, "enemy armor color");

	SetupValue(config->Visuals.team.visuals_esp_snaplines, false, "esp team snaplines");
	save_color(config->Visuals.team.snaplines_color, "team snaplines color");
	SetupValue(config->Visuals.enemy.visuals_esp_snaplines, false, "esp enemy snaplines");
	save_color(config->Visuals.enemy.snaplines_color, "enemy snaplines color");

	SetupValue(config->Visuals.enemy.visuals_esp_damage, false, "esp enemy damage");
	save_color(config->Visuals.enemy.damage_color, "enemy damage color");

	SetupValue(config->Visuals.enemy.visuals_esp_skeleton, false, "esp enemy skeleton");
	save_color(config->Visuals.enemy.skeleton_color, "enemy skeleton color");
	SetupValue(config->Visuals.enemy.visuals_esp_bt_skeleton, false, "esp enemy bt skeleton");
	save_color(config->Visuals.enemy.bt_skeleton_color, "enemy bt skeleton color");

	SetupValue(config->Visuals.local.visuals_esp_skeleton, false, "esp local skeleton");
	save_color(config->Visuals.local.skeleton_color, "local skeleton color");

	SetupValue(config->Visuals.team.visuals_esp_skeleton, false, "esp team skeleton");
	save_color(config->Visuals.team.skeleton_color, "team skeleton color");

	SetupValue(config->Visuals.visuals_dropped_box, false, "esp dropped box");
	save_color(config->Visuals.dropped_box_color, "dropped box color");
	SetupValue(config->Visuals.visuals_dropped_name, false, "esp dropped name");
	save_color(config->Visuals.dropped_name_color, "dropped name color");
	SetupValue(config->Visuals.visuals_droppped_name_icons, false, "esp dropped icons");
	save_color(config->Visuals.dropped_icon_color, "dropped icons color");
	SetupValue(config->Visuals.visuals_dropped_ammo, false, "esp dropped ammo");
	save_color(config->Visuals.dropped_ammo_color, "dropped ammo color");
	SetupValue(config->Visuals.droppedglowenable, false, "esp dropped glow");
	SetupValue(config->Visuals.droppedglowstyle, 0, "esp dropped style");
	save_color(config->Colors.droppedglow_col, "dropped glow color");

	SetupValue(config->Visuals.enemy.visuals_esp_radar, 0, "esp enemy radar");
	save_color(config->Visuals.enemy.radar_color, "enemy radar color");

	SetupValue(config->Visuals.showimpacts, false, "show impacts");
	SetupValue(config->Visuals.__chams, false, "chams enable");
	SetupValue(config->Visuals.xyzchams, false, "chamsxyz");
	SetupValue(config->Visuals.localchams, false, "localchams");
	SetupValue(config->Visuals.glowenable, false, "enemy glowenable");
	SetupValue(config->Visuals.glowstyle, 0, "enemy glowstyle");
	SetupValue(config->Visuals.glowalpha, 0, "enemy glowalpha");

	SetupValue(config->Visuals.localglowenable, false, "local glow enable");
	SetupValue(config->Visuals.localglowstyle, 0, "local glow style");
	SetupValue(config->Visuals.localglowalpha, 0, "local glow alpha");
	SetupValue(config->Visuals.molov_timer, false, "molotov timer");
	SetupValue(config->Visuals.smoke_timer, false, "smoke timer");

	SetupValue(config->Visuals.novisrecoil, false, "no visual recoil");
	SetupValue(config->Visuals.nopostprocsess, false, "no post proccessing");
	SetupValue(config->Visuals.noscope, false, "no scope");
	SetupValue(config->Visuals.nosmoke, false, "no smoke");
	SetupValue(config->Visuals.noflash, 0, "no flash");
	SetupValue(config->Visuals.skyboxes, 0, "skyboxes");
	SetupValue(config->Ragebot.delay_shot, 0, "delay shot");
	SetupValue(config->Visuals.penetration_crosshair, 0, "penetration crosshair");
	SetupValue(config->Visuals.enemy.xqz, 0, "enemy avoid wall");
	SetupValue(config->Visuals.e_bulletbeams, false, "enemy bullet beams");

	SetupValue(config->Visuals.bulletbeams, false, "beams");
	SetupValue(config->Visuals.remove_zoom, false, "removezoom");
	SetupValue(config->Visuals.trails, false, "movement trails");
	SetupValue(config->Visuals.grenade_predict, false, "grenade prediction");
	SetupValue(config->Visuals.nightmode, false, "nightmode");
	SetupValue(config->Visuals.round_summ, false, "round summary");
	SetupValue(config->Visuals.indicators, false, "indicators");
	SetupValue(config->Visuals.thirdaaindicator, false, "thirdperson aa indicator");
	SetupValue(config->Visuals.lagcompydady, false, "lagcomp");
	SetupValue(config->Visuals.hitsound, 0, "hitsound");
	SetupValue(config->Visuals.fullbright, false, "fullbright");
	SetupValue(config->Visuals.forcecrosshair, false, "force crosshair");
	SetupValue(config->Visuals.no_fog, false, "no fog");
	SetupValue(config->Visuals.leftknife, false, "left knife");
	SetupValue(config->Visuals.spread_crosshair, false, "spread crosshair");
	SetupValue(config->Visuals.snaplines, false, "snaplines");
	SetupValue(config->Visuals.taser_range, false, "taser range");
	SetupValue(config->Visuals.knife_range, false, "knife range");
	SetupValue(config->Visuals.spec_list, false, "spectators list");
	SetupValue(config->Visuals.keybinds, false, "keybinds");

	SetupValue(config->Visuals.hitmarker, false, "hitmarker");
	SetupValue(config->Visuals.hitmarker2d, false, "hitmarker 2d");
	SetupValue(config->Visuals.killeffect, false, "kill effect");
	SetupValue(config->Visuals.hiteffect, false, "hit effect");

	SetupValue(config->Visuals.fieldofview, 0, "view setup fov");
	SetupValue(config->Visuals.viewmodelfov, 0, "view setup viewmodel");
	SetupValue(config->Visuals.viewmodelratio, 0, "aspect ratio");
	SetupValue(config->Visuals.enchance_scope, false, "enchance scope");
	SetupValue(config->Visuals.preserve_killfeed, false, "preserve killfeed");
	SetupValue(config->Visuals.bombtimer, false, "bomb timer");
	SetupValue(config->Visuals.showgrenade, false, "grenade path");
	SetupValue(config->Visuals.draw_nades, false, "nade info");

	SetupValue(config->Misc.clantag, false, "clantag changer");
	SetupValue(config->Misc.bhop, false, "bunnyhop");
	SetupValue(config->Misc.autostrafe, false, "autostrafe");
	SetupValue(config->Visuals.ragdoll, false, "ragdoll");
	SetupValue(config->Visuals.indicators_styles, 0, "window styles");

	SetupValue(config->Misc.airstuck_enable, false, "airstuck enable");
	SetupValue(config->Misc.airstuck_key, 0, "airstuck key");
	SetupValue(config->Visuals.thirdperson, 0, "thirdperson key");
	SetupValue(config->Visuals.thirdperson_dist, 0, "thirdperson distance");
	SetupValue(config->Visuals.player_model, 0, "custom player model");
	SetupValue(config->Misc.watermark, false, "watermark");
	SetupValue(config->Misc.slowwalk, false, "slowwalk enable");
	SetupValue(config->Misc.slowwalkspeed, 0, "slowwalk speed");
	SetupValue(config->Misc.slowwalk_key, 0, "slowwalk key");

	SetupValue(config->Ragebot.fakeduck, 0, "fakeduck");
	SetupValue(config->Ragebot.fakeduck_enable, false, "fakeduck enable");
	SetupValue(config->Visuals.thirdperson_enable, false, "thirdperson enable");

	SetupValue(config->Visuals.local.blend, false, "local blend");
	SetupValue(config->Visuals.local2.blend, false, "local blend 2");

	SetupValue(config->Misc.eventlogs, false, "enable events");
	SetupValue(config->Visuals.view_setup_fov_enable, false, "view setup fov enable");
	SetupValue(config->Visuals.view_setup_viewmodel_enable, false, "view setup viewmodel enable");
	SetupValue(config->Visuals.view_setup_aspect_enable, false, "view setup aspect enable");
	SetupValue(config->Visuals.view_setup_xoffset_enable, false, "view setup xoffset enable");
	SetupValue(config->Visuals.view_setup_zoffset_enable, false, "view setup zoffset enable");
	SetupValue(config->Visuals.view_setup_xoffset, 0, "view setup x offset");
	SetupValue(config->Visuals.view_setup_yoffset, 0, "view setup y offset");
	SetupValue(config->Visuals.view_setup_zoffset, 0, "view setup z offset");

	SetupValue(config->Misc.menucolor[0], 0, "menucolor red  ");
	SetupValue(config->Misc.menucolor[1], 0, "menucolor green");
	SetupValue(config->Misc.menucolor[2], 0, "menucolor blue ");

	SetupValue(config->Misc.accentcolor[0], 0, "accentcolor red");
	SetupValue(config->Misc.accentcolor[1], 0, "accentcolor green");
	SetupValue(config->Misc.accentcolor[2], 0, "accentcolor blue");
	SetupValue(config->Ragebot.resolver, false, "resolver");

	SetupValue(config->Misc.buybot, false, "buybot");

	//primary
	bool buy_bot_scar20, buy_bot_g3sg1, buy_bot_ssg08, buy_bot_awp;
	//secondary
	bool buy_bot_elite, buy_bot_p250, buy_bot_tec9, buy_bot_deg;
	//nades molotov; buy incgrenade; buy hegrenade; buy smokegrenade; buy flashbang;
	bool buy_bot_molotov, buy_bot_incgrenade, buy_bot_hegrenade, buy_bot_smokegrenade, buy_bot_flashbang;
	//secondary
	bool buy_bot_vest, buy_bot_vesthelm, buy_bot_taser, buy_bot_defuser;

	SetupValue(config->Misc.buy_bot_scar20, false, "buybot scar20");
	SetupValue(config->Misc.buy_bot_g3sg1, false, "buybot g3sg1");
	SetupValue(config->Misc.buy_bot_ssg08, false, "buybot ssg08");
	SetupValue(config->Misc.buy_bot_awp, false, "buybot awp");
	SetupValue(config->Misc.buy_bot_elite, false, "buybot elite");
	SetupValue(config->Misc.buy_bot_p250, false, "buybot p250");
	SetupValue(config->Misc.buy_bot_tec9, false, "buybot tec9");
	SetupValue(config->Misc.buy_bot_deg, false, "buybot deg");

	SetupValue(config->Misc.buy_bot_molotov,      false, "buybot molotov");
	SetupValue(config->Misc.buy_bot_incgrenade, false, "buybot incgrenade");
	SetupValue(config->Misc.buy_bot_hegrenade, false, "buybot hegrenade");
	SetupValue(config->Misc.buy_bot_smokegrenade, false, "buybot smokegrenade");
	SetupValue(config->Misc.buy_bot_flashbang, false, "buybot flashbang");
	SetupValue(config->Misc.buy_bot_vest, false, "buybot vest");
	SetupValue(config->Misc.buy_bot_vesthelm, false, "buybot vesthelm");
	SetupValue(config->Misc.buy_bot_taser, false, "buybot taser");
	SetupValue(config->Misc.buy_bot_defuser, false, "buybot defuser");

	SetupValue(config->Colors.boxcol[0], 0, "boxcol red  ");
	SetupValue(config->Colors.boxcol[1], 0, "boxcol green ");
	SetupValue(config->Colors.boxcol[2], 0, "boxcol blue");
	SetupValue(config->Colors.boxcol[3], 1, "boxcol alpha");

	SetupValue(config->Colors.namecol[0], 0, "namecol red  ");
	SetupValue(config->Colors.namecol[1], 0, "namecol green ");
	SetupValue(config->Colors.namecol[2], 0, "namecol blue");
	SetupValue(config->Colors.namecol[3], 1, "namecol alpha");

	SetupValue(config->Colors.trails_color[0], 0, "trails_color red  ");
	SetupValue(config->Colors.trails_color[1], 0, "trails_color green ");
	SetupValue(config->Colors.trails_color[2], 0, "trails_color blue");
	SetupValue(config->Colors.trails_color[3], 1, "trails_color alpha");

	SetupValue(config->Colors.hitmarker2d[0], 0, "hitmarker red  ");
	SetupValue(config->Colors.hitmarker2d[1], 0, "hitmarker green ");
	SetupValue(config->Colors.hitmarker2d[2], 0, "hitmarker blue");
	SetupValue(config->Colors.hitmarker2d[3], 1, "hitmarker alpha");

	SetupValue(config->Colors.droppedcolor[0], 0, "dropped color red  ");
	SetupValue(config->Colors.droppedcolor[1], 0, "dropped color green ");
	SetupValue(config->Colors.droppedcolor[2], 0, "dropped color blue");
	SetupValue(config->Colors.droppedcolor[3], 1, "dropped color alpha");

	SetupValue(config->Colors.nightcolor[0], 0, "night color red  ");
	SetupValue(config->Colors.nightcolor[1], 0, "night color green ");
	SetupValue(config->Colors.nightcolor[2], 0, "night color blue");
	SetupValue(config->Colors.nightcolor[3], 1, "night color alpha");

	SetupValue(config->Colors.propcolor[0], 0, "prop color red  ");
	SetupValue(config->Colors.propcolor[1], 0, "prop color green ");
	SetupValue(config->Colors.propcolor[2], 0, "prop color blue");
	SetupValue(config->Colors.propcolor[3], 1, "prop color alpha");

	SetupValue(config->Colors.skycolor[0], 0, "sky color red  ");
	SetupValue(config->Colors.skycolor[1], 0, "sky color green ");
	SetupValue(config->Colors.skycolor[2], 0, "sky color blue");
												   
	SetupValue(config->Colors.bombcolor[0], 0, "bomb timer red  ");
	SetupValue(config->Colors.bombcolor[1], 0, "bomb timer green ");
	SetupValue(config->Colors.bombcolor[2], 0, "bomb timer blue");
	SetupValue(config->Colors.bombcolor[3], 1, "bomb timer alpha");

	SetupValue(config->Colors.molov_timer[0], 0, "molovtov timer red  ");
	SetupValue(config->Colors.molov_timer[1], 0, "molovtov timer green ");
	SetupValue(config->Colors.molov_timer[2], 0, "molovtov timer blue");
	SetupValue(config->Colors.molov_timer[3], 1, "molovtov timer alpha");

	SetupValue(config->Colors.smoke_timer[0], 0, "smoke timer red");
	SetupValue(config->Colors.smoke_timer[1], 0, "smoke timer green");
	SetupValue(config->Colors.smoke_timer[2], 0, "smoke timer blue");
	SetupValue(config->Colors.smoke_timer[3], 1, "smoke timer alpha");

	SetupValue(config->Visuals.local2.color[0], 0, "local2 chams color red  ");
	SetupValue(config->Visuals.local2.color[1], 0, "local2 chams color green");
	SetupValue(config->Visuals.local2.color[2], 0, "local2 chams color blue");
	SetupValue(config->Visuals.local2.color[3], 1, "local2 chams color alpha");

	SetupValue(config->Visuals.local2.gloss[0], 0, "local2 chams gloss red  ");
	SetupValue(config->Visuals.local2.gloss[1], 0, "local2 chams gloss green");
	SetupValue(config->Visuals.local2.gloss[2], 0, "local2 chams gloss blue");
	SetupValue(config->Visuals.local2.gloss[3], 1, "local2 chams gloss alpha");

	SetupValue(config->Colors.ebeamcolor[0], 0, "enemy beam color red");
	SetupValue(config->Colors.ebeamcolor[1], 0, "enemy beam color green");
	SetupValue(config->Colors.ebeamcolor[2], 0, "enemy beam color blue");
	SetupValue(config->Colors.ebeamcolor[3], 1, "enemy beam color alpha");

	SetupValue(config->Colors.localglow_col[0], 0, "local glow red");
	SetupValue(config->Colors.localglow_col[1], 0, "local glow green");
	SetupValue(config->Colors.localglow_col[2], 0, "local glow blue");
	SetupValue(config->Colors.localglow_col[3], 1, "local glow alpha");

	SetupValue(config->Colors.glow_col[0], 0, "enemy glow color red");
	SetupValue(config->Colors.glow_col[1], 0,  "enemy glow color green");
	SetupValue(config->Colors.glow_col[2], 0,  "enemy glow blue");
	SetupValue(config->Colors.glow_col[3], 1,  "enemy glow alpha");

	SetupValue(config->Colors.spread_crosshair[0], 0, "spread_crosshair red");
	SetupValue(config->Colors.spread_crosshair[1], 0, "spread_crosshair green");
	SetupValue(config->Colors.spread_crosshair[2], 0, "spread_crosshair blue");
	SetupValue(config->Colors.spread_crosshair[3], 1, "spread_crosshair alpha");

	SetupValue(config->Colors.beamcolor[0], 0, "local beamcolor red");
	SetupValue(config->Colors.beamcolor[1], 0, "local beamcolor green");
	SetupValue(config->Colors.beamcolor[2], 0, "local beamcolor blue");
	SetupValue(config->Colors.beamcolor[3], 1, "local beamcolor alpha");

	SetupValue(config->Colors.nadecolorr[0], 0, "nade info color red");
	SetupValue(config->Colors.nadecolorr[1], 0,  "nade info color green");
	SetupValue(config->Colors.nadecolorr[2], 0,  "nade info color blue");
	SetupValue(config->Colors.nadecolorr[3], 1,  "nade info color alpha");

	SetupValue(config->Colors.droppedammocolor[0], 0, "dropped ammocolor red");
	SetupValue(config->Colors.droppedammocolor[1], 0, "dropped ammocolor green");
	SetupValue(config->Colors.droppedammocolor[2], 0, "dropped ammocolor blue");
	SetupValue(config->Colors.droppedammocolor[3], 1, "dropped ammocolor alpha");

	SetupValue(config->Colors.f_aa_ind[0], 0, "first person indicator red");
	SetupValue(config->Colors.f_aa_ind[1], 0, "first person indicator green");
	SetupValue(config->Colors.f_aa_ind[2], 0, "first person indicator blue");
	SetupValue(config->Colors.f_aa_ind[3], 1, "first person indicator alpha");

	SetupValue(config->Colors.f_aa_ind2[0], 0, "first person indicator fake red");
	SetupValue(config->Colors.f_aa_ind2[1], 0, "first person indicator fake green");
	SetupValue(config->Colors.f_aa_ind2[2], 0, "first person indicator fake blue");
	SetupValue(config->Colors.f_aa_ind2[3], 1, "first person indicator fake alpha");

	SetupValue(config->Colors.t_aa_ind[0], 0, "third person indicator red");
	SetupValue(config->Colors.t_aa_ind[1], 0, "third person indicator green");
	SetupValue(config->Colors.t_aa_ind[2], 0, "third person indicator blue");
	SetupValue(config->Colors.t_aa_ind[3], 1, "third person indicator alpha");

	SetupValue(config->Colors.t_aa_ind2[0], 0, "third person indicator fake red");
	SetupValue(config->Colors.t_aa_ind2[1], 0, "third person indicator fake green");
	SetupValue(config->Colors.t_aa_ind2[2], 0, "third person indicator fake blue");
	SetupValue(config->Colors.t_aa_ind2[3], 1, "third person indicator fake alpha");

	SetupValue(config->Skinchanger.Enabled, false, "skins enable");
	SetupValue(config->Skinchanger.weapon_switch, 0, "weapon cfgs kins");
	SetupValue(config->Skinchanger.knife_switch, 0, "knife models");

	SetupValue(config->Visuals.weapon.material_type, 0, "weapon material type");
	SetupValue(config->Visuals.enemy.material_type, 0, "enemy material type");
	//SetupValue(_config->Visuals.team.material_type, 0, "teammaterial type");
	SetupValue(config->Visuals.arms.material_type, 0, "hand material type");
	SetupValue(config->Visuals.desync.material_type, 0, "desync type");
	SetupValue(config->Visuals.sleeve.material_type, 0, "sleeves material type");
	SetupValue(config->Visuals.local.material_type, 0, "local material type");
	SetupValue(config->Visuals.enemy.xqz_type, 0, "desync material type");
	//SetupValue(_config->Visuals.team.xqz_type, 0, "desynclddt");

	SetupValue(config->Visuals.enemy2.xqz, false, "enemy2 xqz");

	SetupValue(config->Visuals.weapon2.material_type, 0, "weaponmaterial type2");
	SetupValue(config->Visuals.enemy2.material_type, 0, "enemy material type2");
	SetupValue(config->Visuals.team2.material_type, 0, "team material type2");
	SetupValue(config->Visuals.arms2.material_type, 0, "hand material type2");
	SetupValue(config->Visuals.sleeve2.material_type, 0, "sleeves material type2");
	SetupValue(config->Visuals.local2.material_type, 0, "local material type2");
	SetupValue(config->Visuals.backtrack.material_type, 0, "backtrack material type2");
	SetupValue(config->Visuals.enemy2.xqz_type, 0, "enemy2 material type2");
	SetupValue(config->Visuals.team2.xqz_type, 0, "team2 material type2");
	SetupValue(config->Visuals.desync2.xqz_type, 0, "desync material type2");

	SetupValue(config->Visuals.enemy.xqz_color[0], 0, "enemy xqz color red");
	SetupValue(config->Visuals.enemy.xqz_color[1], 0,"enemy xqz color green");
	SetupValue(config->Visuals.enemy.xqz_color[2], 0,"enemy xqz color blue");
	SetupValue(config->Visuals.enemy.xqz_color[3], 1,"enemy xqz color alpha");

	SetupValue(config->Visuals.enemy2.xqz_color[0], 0, "enemy2 xqz color red");
	SetupValue(config->Visuals.enemy2.xqz_color[1], 0, "enemy2 xqz color green");
	SetupValue(config->Visuals.enemy2.xqz_color[2], 0, "enemy2 xqz color blue");
	SetupValue(config->Visuals.enemy2.xqz_color[3], 1, "enemy2 xqz color alpha");

	SetupValue(config->Visuals.team2.xqz_color[0], 0, "team2 xqz color red");
	SetupValue(config->Visuals.team2.xqz_color[1], 0, "team2 xqz color green");
	SetupValue(config->Visuals.team2.xqz_color[2], 0, "team2 xqz color blue");
	SetupValue(config->Visuals.team2.xqz_color[3], 1, "team2 xqz color alpha");

	SetupValue(config->Visuals.desync2.xqz_color[0], 0, "desync2 xqz color");
	SetupValue(config->Visuals.desync2.xqz_color[1], 0, "desync2 xqz color");
	SetupValue(config->Visuals.desync2.xqz_color[2], 0, "desync2 xqz color");
	SetupValue(config->Visuals.desync2.xqz_color[3], 1, "desync2 xqz color");

	SetupValue(config->Visuals.weapon.color[0], 0, "weapon color red");
	SetupValue(config->Visuals.weapon.color[1], 0, "weapon color green");
	SetupValue(config->Visuals.weapon.color[2], 0, "weapon color blue");
	SetupValue(config->Visuals.weapon.color[3], 1, "weapon color alpha");

	SetupValue(config->Visuals.weapon2.color[0], 0,"weapon2 color red");
	SetupValue(config->Visuals.weapon2.color[1], 0,"weapon2 color green");
	SetupValue(config->Visuals.weapon2.color[2], 0,"weapon2 color blue");
	SetupValue(config->Visuals.weapon2.color[3], 1,"weapon2 color alpha");

	SetupValue(config->Visuals.enemy.color[0], 0, "enemy color red");
	SetupValue(config->Visuals.enemy.color[1], 0, "enemy color green");
	SetupValue(config->Visuals.enemy.color[2], 0, "enemy color blue");
	SetupValue(config->Visuals.enemy.color[3], 1, "enemy color alpha");
															   
	SetupValue(config->Visuals.enemy2.color[0], 0, "enemy2 color red");
	SetupValue(config->Visuals.enemy2.color[1], 0, "enemy2 color green");
	SetupValue(config->Visuals.enemy2.color[2], 0, "enemy2 color blue");
	SetupValue(config->Visuals.enemy2.color[3], 1, "enemy2 color alpha");

	SetupValue(config->Visuals.team.color[0], 0, "team color red");
	SetupValue(config->Visuals.team.color[1], 0, "team color green");
	SetupValue(config->Visuals.team.color[2], 0, "team color blue");
	SetupValue(config->Visuals.team.color[3], 1, "team color alpha");

	SetupValue(config->Visuals.team2.color[0], 0, "team2 color red");
	SetupValue(config->Visuals.team2.color[1], 0, "team2 color green");
	SetupValue(config->Visuals.team2.color[2], 0, "team2 color blue");
	SetupValue(config->Visuals.team2.color[3], 1, "team2 color alpha");

	SetupValue(config->Visuals.arms.color[0], 0, "arms color red");
	SetupValue(config->Visuals.arms.color[1], 0, "arms color green");
	SetupValue(config->Visuals.arms.color[2], 0, "arms color blue");
	SetupValue(config->Visuals.arms.color[3], 1, "arms color alpha");

	SetupValue(config->Visuals.arms2.color[0], 0, "arms2 color red");
	SetupValue(config->Visuals.arms2.color[1], 0, "arms2 color green");
	SetupValue(config->Visuals.arms2.color[2], 0, "arms2 color blue");
	SetupValue(config->Visuals.arms2.color[3], 1, "arms2 color alpha");

	SetupValue(config->Visuals.desync.color[0], 0, "desync color red");
	SetupValue(config->Visuals.desync.color[1], 0, "desync color green");
	SetupValue(config->Visuals.desync.color[2], 0, "desync color blue");
	SetupValue(config->Visuals.desync.color[3], 1, "desync color alpha");

	SetupValue(config->Visuals.desync2.color[0], 0, "desync2 color red");
	SetupValue(config->Visuals.desync2.color[1], 0, "desync2 color green");
	SetupValue(config->Visuals.desync2.color[2], 0, "desync2 color blue");
	SetupValue(config->Visuals.desync2.color[3], 1, "desync2 color alpha");

	SetupValue(config->Visuals.sleeve.color[0], 0, "sleeves color red");
	SetupValue(config->Visuals.sleeve.color[1], 0, "sleeves color green");
	SetupValue(config->Visuals.sleeve.color[2], 0, "sleeves color blue");
	SetupValue(config->Visuals.sleeve.color[3], 1, "sleeves color alpha");

	SetupValue(config->Visuals.sleeve2.color[0], 0, "sleeves2 color red");
	SetupValue(config->Visuals.sleeve2.color[1], 0, "sleeves2 color green");
	SetupValue(config->Visuals.sleeve2.color[2], 0, "sleeves2 color blue");
	SetupValue(config->Visuals.sleeve2.color[3], 1, "sleeves2 color alpha");

	SetupValue(config->Visuals.local.color[0], 0, "local color red");
	SetupValue(config->Visuals.local.color[1], 0, "local color green");
	SetupValue(config->Visuals.local.color[2], 0, "local color blue");
	SetupValue(config->Visuals.local.color[3], 1, "local color alpha");

	SetupValue(config->Visuals.local2.color[0], 0, "local2 color red");
	SetupValue(config->Visuals.local2.color[1], 0, "local2 color green");
	SetupValue(config->Visuals.local2.color[2], 0, "local2 color blue");
	SetupValue(config->Visuals.local2.color[3], 1, "local2 color alpha");

	SetupValue(config->Visuals.backtrack.color[0], 0, "backtrack color red");
	SetupValue(config->Visuals.backtrack.color[1], 0, "backtrack color green");
	SetupValue(config->Visuals.backtrack.color[2], 0, "backtrack color blue");
	SetupValue(config->Visuals.backtrack.color[3], 1, "backtrack color alpha");

	SetupValue(config->Visuals.weapon.gloss[0], 0, "weapon gloss red");
	SetupValue(config->Visuals.weapon.gloss[1], 0, "weapon gloss green");
	SetupValue(config->Visuals.weapon.gloss[2], 0, "weapon gloss blue");
	SetupValue(config->Visuals.weapon.gloss[3], 1, "weapon gloss alpha");

	SetupValue(config->Visuals.weapon2.gloss[0], 0, "weapon2 gloss red");
	SetupValue(config->Visuals.weapon2.gloss[1], 0, "weapon2 gloss green");
	SetupValue(config->Visuals.weapon2.gloss[2], 0, "weapon2 gloss blue");
	SetupValue(config->Visuals.weapon2.gloss[3], 1, "weapon2 gloss alpha");

	SetupValue(config->Visuals.enemy.gloss[0], 0, "enemy gloss red");
	SetupValue(config->Visuals.enemy.gloss[1], 0, "enemy gloss green");
	SetupValue(config->Visuals.enemy.gloss[2], 0, "enemy gloss blue");
	SetupValue(config->Visuals.enemy.gloss[3], 1, "enemy gloss alpha");

	SetupValue(config->Visuals.enemy2.gloss[0], 0, "enemy2 gloss red");
	SetupValue(config->Visuals.enemy2.gloss[1], 0, "enemy2 gloss green");
	SetupValue(config->Visuals.enemy2.gloss[2], 0, "enemy2 gloss blue");
	SetupValue(config->Visuals.enemy2.gloss[3], 1, "enemy2 gloss alpha");

	SetupValue(config->Visuals.team2.gloss[0], 0, "team2 gloss red");
	SetupValue(config->Visuals.team2.gloss[1], 0, "team2 gloss green");
	SetupValue(config->Visuals.team2.gloss[2], 0, "team2 gloss blue");
	SetupValue(config->Visuals.team2.gloss[3], 1, "team2 gloss alpha");

	SetupValue(config->Visuals.arms.gloss[0], 0, "arms gloss red");
	SetupValue(config->Visuals.arms.gloss[1], 0, "arms gloss green");
	SetupValue(config->Visuals.arms.gloss[2], 0, "arms gloss blue");
	SetupValue(config->Visuals.arms.gloss[3], 1, "arms gloss alpha");

	SetupValue(config->Visuals.arms2.gloss[0], 0, "arms2 gloss red");
	SetupValue(config->Visuals.arms2.gloss[1], 0, "arms2 gloss green");
	SetupValue(config->Visuals.arms2.gloss[2], 0, "arms2 gloss blue");
	SetupValue(config->Visuals.arms2.gloss[3], 1, "arms2 gloss alpha");

	SetupValue(config->Visuals.desync.gloss[0], 0, "desync gloss red");
	SetupValue(config->Visuals.desync.gloss[1], 0, "desync gloss green");
	SetupValue(config->Visuals.desync.gloss[2], 0, "desync gloss blue");
	SetupValue(config->Visuals.desync.gloss[3], 1, "desync gloss alpha");

	SetupValue(config->Visuals.desync2.gloss[0], 0, "desync2 gloss red");
	SetupValue(config->Visuals.desync2.gloss[1], 0, "desync2 gloss green");
	SetupValue(config->Visuals.desync2.gloss[2], 0, "desync2 gloss blue");
	SetupValue(config->Visuals.desync2.gloss[3], 1, "desync2 gloss alpha");

	SetupValue(config->Visuals.sleeve.gloss[0], 0, "sleeve gloss red");
	SetupValue(config->Visuals.sleeve.gloss[1], 0, "sleeve gloss green");
	SetupValue(config->Visuals.sleeve.gloss[2], 0, "sleeve gloss blue");
	SetupValue(config->Visuals.sleeve.gloss[3], 1, "sleeve gloss alpha");

	SetupValue(config->Visuals.sleeve2.gloss[0], 0, "sleeve2 gloss red");
	SetupValue(config->Visuals.sleeve2.gloss[1], 0, "sleeve2 gloss green");
	SetupValue(config->Visuals.sleeve2.gloss[2], 0, "sleeve2 gloss blue");
	SetupValue(config->Visuals.sleeve2.gloss[3], 1, "sleeve2 gloss alpha");

	SetupValue(config->Visuals.local.gloss[0], 0, "lcoal gloss red");
	SetupValue(config->Visuals.local.gloss[1], 0, "lcoal gloss green");
	SetupValue(config->Visuals.local.gloss[2], 0, "lcoal gloss blue");
	SetupValue(config->Visuals.local.gloss[3], 1, "lcoal gloss alpha");

	SetupValue(config->Visuals.local2.gloss[0], 0, "local2 gloss red");
	SetupValue(config->Visuals.local2.gloss[1], 0, "local2 gloss green");
	SetupValue(config->Visuals.local2.gloss[2], 0, "local2 gloss blue");
	SetupValue(config->Visuals.local2.gloss[3], 1, "local2 gloss alpha");

	SetupValue(config->Visuals.backtrack.gloss[0], 0, "backtrack gloss red");
	SetupValue(config->Visuals.backtrack.gloss[1], 0, "backtrack gloss green");
	SetupValue(config->Visuals.backtrack.gloss[2], 0, "backtrack gloss blue");
	SetupValue(config->Visuals.backtrack.gloss[3], 1, "backtrack gloss alpha");

	SetupValue(config->Colors.enchance_scope[0], 0, "enchanced scope red");
	SetupValue(config->Colors.enchance_scope[1], 0, "enchanced scope green");
	SetupValue(config->Colors.enchance_scope[2], 0, "enchanced scope blue");
	SetupValue(config->Colors.enchance_scope[3], 1, "enchanced scope alpha");

	SetupValue(config->Visuals.weapon2.material_type, 0, "weapon material type2");
	SetupValue(config->Visuals.enemy2.material_type, 0, "enemy material type2");
	SetupValue(config->Visuals.team2.material_type, 0, "team material type2");
	SetupValue(config->Visuals.arms2.material_type, 0, "hand material type2");
	SetupValue(config->Visuals.sleeve2.material_type, 0, "sleeves material type2");
	SetupValue(config->Visuals.local2.material_type, 0, "local material type2");
	SetupValue(config->Visuals.desync2.material_type, 0, "desync material type2");

	SetupValue(config->Visuals.desync.preserve_model, false, "desync preserve_model");
	SetupValue(config->Visuals.desync2.preserve_model, false, "desync2 preserve_model");

	SetupValue(config->Visuals.weapon.preserve_model, false, "weapon preserve_model");
	SetupValue(config->Visuals.sleeve.preserve_model, false, "sleeve preserve_model");
	SetupValue(config->Visuals.arms.preserve_model, false,   "arms preserve_model");
	SetupValue(config->Visuals.backtrack.preserve_model, false, "backtrack preserve_model");
	SetupValue(config->Visuals.local.preserve_model, false, "local preserve_model");
	SetupValue(config->Visuals.local.pulse, false, "local pulse");

	SetupValue(config->Visuals.weapon2.preserve_model, false, "weapon2 preserve_model");
	SetupValue(config->Visuals.sleeve2.preserve_model, false, "sleeve2 preserve_models2");
	SetupValue(config->Visuals.arms2.preserve_model, false, "arms2 preserve_modelh2");

	SetupValue(config->Visuals.weapon.wireframe, false,   "weapon wireframe");
	SetupValue(config->Visuals.enemy.wireframe, false,	  "enemy wireframe");
	SetupValue(config->Visuals.arms.wireframe, false,	  "arms wireframe");
	SetupValue(config->Visuals.team.wireframe, false,	  "team wireframe");
	SetupValue(config->Visuals.desync.wireframe, false,   "desync wireframe");
	SetupValue(config->Visuals.sleeve.wireframe, false,   "sleeve wireframe");
	SetupValue(config->Visuals.backtrack.wireframe, false,"backtrack wireframe");
	SetupValue(config->Visuals.local.wireframe, false,    "local wireframe");

	SetupValue(config->Visuals.weapon2.wireframe, false, "weapon2 wireframe");
	SetupValue(config->Visuals.enemy2.wireframe, false, "enemy2 wireframe");
	SetupValue(config->Visuals.arms2.wireframe, false, "arms2 wireframe");
	SetupValue(config->Visuals.team2.wireframe, false, "team2 wireframe");
	SetupValue(config->Visuals.desync2.wireframe, false, "desync2 wireframe");
	SetupValue(config->Visuals.sleeve2.wireframe, false, "sleeve2 wireframe");
	SetupValue(config->Visuals.local2.wireframe, false, "local2 wireframe");

	SetupValue(config->Skinchanger.awp.id, 0, "skin awp id");
	SetupValue(config->Skinchanger.awp.quality, 0, "skin quality id");
	SetupValue(config->Skinchanger.awp.seed, 0, "skin seed id");
	SetupValue(config->Skinchanger.awp.stattrak, 0, "skin stattrak id");

	SetupValue(config->Skinchanger.usp.id, 0, "skin usp id");
	SetupValue(config->Skinchanger.usp.quality, 0, "skin usp quality id");
	SetupValue(config->Skinchanger.usp.seed, 0, "skin usp seed id");
	SetupValue(config->Skinchanger.usp.stattrak, 0, "skin usp stattrak id");

	SetupValue(config->Skinchanger.p2k.id, 0, "skin p2k id");
	SetupValue(config->Skinchanger.p2k.quality, 0, "skin p2k quality id");
	SetupValue(config->Skinchanger.p2k.seed, 0, "skin p2k seed id");
	SetupValue(config->Skinchanger.p2k.stattrak, 0, "skin p2k stattrak id");

	SetupValue(config->Skinchanger.glock.id, 0, "skin glock id");
	SetupValue(config->Skinchanger.glock.quality, 0, "skin glock quality id");
	SetupValue(config->Skinchanger.glock.seed, 0, "skin glock seed id");
	SetupValue(config->Skinchanger.glock.stattrak, 0, "skin glock stattrak id");

	SetupValue(config->Skinchanger.deg.id, 0, "skin deg awp id");
	SetupValue(config->Skinchanger.deg.quality, 0, "skin deg quality id");
	SetupValue(config->Skinchanger.deg.seed, 0, "skin deg seed id");
	SetupValue(config->Skinchanger.deg.stattrak, 0, "skin deg stattrak id");

	SetupValue(config->Skinchanger.revol.id, 0, "skin revol id");
	SetupValue(config->Skinchanger.revol.quality, 0, "skin revol quality id");
	SetupValue(config->Skinchanger.revol.seed, 0, "skin revol seed id");
	SetupValue(config->Skinchanger.revol.stattrak, 0, "skin revol stattrak id");

	SetupValue(config->Skinchanger.ssg.id, 0, "skin ssg id");
	SetupValue(config->Skinchanger.ssg.quality, 0, "skin ssg quality id");
	SetupValue(config->Skinchanger.ssg.seed, 0, "skin ssg seed id");
	SetupValue(config->Skinchanger.ssg.stattrak, 0, "skin ssg stattrak id");

	SetupValue(config->Skinchanger.dual.id, 0, "skin dual id");
	SetupValue(config->Skinchanger.dual.quality, 0, "skin dual quality id");
	SetupValue(config->Skinchanger.dual.seed, 0, "skin dual seed id");
	SetupValue(config->Skinchanger.dual.stattrak, 0, "skin dual stattrak id");

	SetupValue(config->Skinchanger.p250.id, 0, "skin p250 id");
	SetupValue(config->Skinchanger.p250.quality, 0, "skin p250 quality id");
	SetupValue(config->Skinchanger.p250.seed, 0, "skin p250 seed id");
	SetupValue(config->Skinchanger.p250.stattrak, 0, "skin p250 stattrak id");

	SetupValue(config->Skinchanger.cz75.id, 0, "skin cz75 id");
	SetupValue(config->Skinchanger.cz75.quality, 0, "skin cz75 quality id");
	SetupValue(config->Skinchanger.cz75.seed, 0, "skin cz75 seed id");
	SetupValue(config->Skinchanger.cz75.stattrak, 0, "skin cz75 stattrak id");

	SetupValue(config->Skinchanger.five7.id, 0, "skin five7 awp id");
	SetupValue(config->Skinchanger.five7.quality, 0, "skin five7 quality id");
	SetupValue(config->Skinchanger.five7.seed, 0, "skin five7 seed id");
	SetupValue(config->Skinchanger.five7.stattrak, 0, "skin five7 stattrak id");

	SetupValue(config->Skinchanger.tec9.id, 0, "skin tec9 id");
	SetupValue(config->Skinchanger.tec9.quality, 0, "skin tec9 quality id");
	SetupValue(config->Skinchanger.tec9.seed, 0, "skin tec9 seed id");
	SetupValue(config->Skinchanger.tec9.stattrak, 0, "skin tec9 stattrak id");

	SetupValue(config->Skinchanger.nova.id, 0, "skin nova id");
	SetupValue(config->Skinchanger.nova.quality, 0, "skin nova quality id");
	SetupValue(config->Skinchanger.nova.seed, 0, "skin nova seed id");
	SetupValue(config->Skinchanger.nova.stattrak, 0, "skin nova stattrak id");

	SetupValue(config->Skinchanger.xm1014.id, 0, "skin xm1014 id");
	SetupValue(config->Skinchanger.xm1014.quality, 0, "skin xm1014 quality id");
	SetupValue(config->Skinchanger.xm1014.seed, 0, "skin xm1014 seed id");
	SetupValue(config->Skinchanger.xm1014.stattrak, 0, "skin xm1014 stattrak id");

	SetupValue(config->Skinchanger.knife.id, 0, "skin knife id");
	SetupValue(config->Skinchanger.knife.quality, 0, "skin knife quality id");
	SetupValue(config->Skinchanger.knife.seed, 0, "skin knife id");
	SetupValue(config->Skinchanger.knife.stattrak, 0, "skin knife stattrak id");

	SetupValue(config->Skinchanger.mag7.id, 0, "skin mag7 id");
	SetupValue(config->Skinchanger.mag7.quality, 0, "skin mag7 quality id");
	SetupValue(config->Skinchanger.mag7.seed, 0, "skin mag7 seed id");
	SetupValue(config->Skinchanger.mag7.stattrak, 0, "skin mag7 stattrak id");

	SetupValue(config->Skinchanger.sawed.id, 0, "skin sawed id");
	SetupValue(config->Skinchanger.sawed.quality, 0, "skin sawed quality id");
	SetupValue(config->Skinchanger.sawed.seed, 0, "skin sawed seed id");
	SetupValue(config->Skinchanger.sawed.stattrak, 0, "skin sawed stattrak id");

	SetupValue(config->Skinchanger.negev.id, 0, "skin negev id");
	SetupValue(config->Skinchanger.negev.quality, 0, "skin negev quality id");
	SetupValue(config->Skinchanger.negev.seed, 0, "skin negev seed id");
	SetupValue(config->Skinchanger.negev.stattrak, 0, "skin negev stattrak id");

	SetupValue(config->Skinchanger.m249.id, 0, "skin m249 id");
	SetupValue(config->Skinchanger.m249.quality, 0, "skin m249 quality id");
	SetupValue(config->Skinchanger.m249.seed, 0, "skin m249 seed id");
	SetupValue(config->Skinchanger.m249.stattrak, 0, "skin m249 stattrak id");

	SetupValue(config->Skinchanger.mp9.id, 0, "skin mp9 id");
	SetupValue(config->Skinchanger.mp9.quality, 0, "skin mp9 quality id");
	SetupValue(config->Skinchanger.mp9.seed, 0, "skin mp9 seed id");
	SetupValue(config->Skinchanger.mp9.stattrak, 0, "skin mp9 stattrak id");

	SetupValue(config->Skinchanger.mac10.id, 0, "skin mac10 id");
	SetupValue(config->Skinchanger.mac10.quality, 0, "skin mac10 quality id");
	SetupValue(config->Skinchanger.mac10.seed, 0, "skin mac10 seed id");
	SetupValue(config->Skinchanger.mac10.stattrak, 0, "skin mac10 stattrak id");

	SetupValue(config->Skinchanger.mp7.id, 0, "skin mp7 id");
	SetupValue(config->Skinchanger.mp7.quality, 0, "skin mp7 quality id");
	SetupValue(config->Skinchanger.mp7.seed, 0, "skin mp7 seed id");
	SetupValue(config->Skinchanger.mp7.stattrak, 0, "skin mp7 stattrak id");

	SetupValue(config->Skinchanger.ump.id, 0, "skin ump id");
	SetupValue(config->Skinchanger.ump.quality, 0, "skin ump quality id");
	SetupValue(config->Skinchanger.ump.seed, 0, "skin ump seed id");
	SetupValue(config->Skinchanger.ump.stattrak, 0, "skin ump stattrak id");

	SetupValue(config->Skinchanger.p90.id, 0, "skin p90 id");
	SetupValue(config->Skinchanger.p90.quality, 0, "skin p90 quality id");
	SetupValue(config->Skinchanger.p90.seed, 0, "skin p90 seed id");
	SetupValue(config->Skinchanger.p90.stattrak, 0, "skin p90 stattrak id");

	SetupValue(config->Skinchanger.ppbizon.id, 0, "skin ppbizon id");
	SetupValue(config->Skinchanger.ppbizon.quality, 0, "skin ppbizon quality id");
	SetupValue(config->Skinchanger.ppbizon.seed, 0, "skin ppbizon seed id");
	SetupValue(config->Skinchanger.ppbizon.stattrak, 0, "skin ppbizon stattrak id");

	SetupValue(config->Skinchanger.galil.id, 0, "skin galil id");
	SetupValue(config->Skinchanger.galil.quality, 0, "skin galil quality id");
	SetupValue(config->Skinchanger.galil.seed, 0, "skin galil seed id");
	SetupValue(config->Skinchanger.galil.stattrak, 0, "skin galil stattrak id");

	SetupValue(config->Skinchanger.famas.id, 0, "skin famas id");
	SetupValue(config->Skinchanger.famas.quality, 0, "skin famas quality id");
	SetupValue(config->Skinchanger.famas.seed, 0, "skin famas seed id");
	SetupValue(config->Skinchanger.famas.stattrak, 0, "skin famas stattrak id");

	SetupValue(config->Skinchanger.m4a4.id, 0, "skin m4a4 id");
	SetupValue(config->Skinchanger.m4a4.quality, 0, "skin m4a4 quality id");
	SetupValue(config->Skinchanger.m4a4.seed, 0, "skin m4a4 seed id");
	SetupValue(config->Skinchanger.m4a4.stattrak, 0, "skin m4a4 stattrak id");

	SetupValue(config->Skinchanger.m4a1.id, 0, "skin m4a1 id");
	SetupValue(config->Skinchanger.m4a1.quality, 0, "skin m4a1 quality id");
	SetupValue(config->Skinchanger.m4a1.seed, 0, "skin m4a1 seed id");
	SetupValue(config->Skinchanger.m4a1.stattrak, 0, "skin m4a1 stattrak id");

	SetupValue(config->Skinchanger.ak47.id, 0, "skin ak47 id");
	SetupValue(config->Skinchanger.ak47.quality, 0, "skin ak47 quality id");
	SetupValue(config->Skinchanger.ak47.seed, 0, "skin ak47 seed id");
	SetupValue(config->Skinchanger.ak47.stattrak, 0, "skin ak47 stattrak id");

	SetupValue(config->Skinchanger.aug.id, 0, "skin aug id");
	SetupValue(config->Skinchanger.aug.quality, 0, "skin aug quality id");
	SetupValue(config->Skinchanger.aug.seed, 0, "skin aug seed id");
	SetupValue(config->Skinchanger.aug.stattrak, 0, "skin aug stattrak id");

	SetupValue(config->Skinchanger.sg553.id, 0, "skin sg553 id");
	SetupValue(config->Skinchanger.sg553.quality, 0, "skin sg553 quality id");
	SetupValue(config->Skinchanger.sg553.seed, 0, "skin sg553 seed id");
	SetupValue(config->Skinchanger.sg553.stattrak, 0, "skin sg553 stattrak id");

	SetupValue(config->Skinchanger.scar20.id, 0, "skin scar20 id");
	SetupValue(config->Skinchanger.scar20.quality, 0, "skin scar20 quality id");
	SetupValue(config->Skinchanger.scar20.seed, 0, "skin scar20 seed id");
	SetupValue(config->Skinchanger.scar20.stattrak, 0, "skin scar20 stattrak id");

	SetupValue(config->Skinchanger.g3sg1.id, 0, "skin awp id");
	SetupValue(config->Skinchanger.g3sg1.quality, 0, "skin quality id");
	SetupValue(config->Skinchanger.g3sg1.seed, 0, "skin seed id");
	SetupValue(config->Skinchanger.g3sg1.stattrak, 0, "skin stattrak id");

	SetupValue(config->Skinchanger.mp5.id, 0, "skin mp5 id");
	SetupValue(config->Skinchanger.mp5.quality, 0, "skin mp5 quality id");
	SetupValue(config->Skinchanger.mp5.seed, 0, "skin mp5 seed id");
	SetupValue(config->Skinchanger.mp5.stattrak, 0, "skin mp5 stattrak id");

	SetupValue(config->Skinchanger.knife.id, 0, "skin knife id");
	SetupValue(config->Skinchanger.knife.quality, 0, "skin knife quality id");
	SetupValue(config->Skinchanger.knife.seed, 0, "skin knife seed id");
	SetupValue(config->Skinchanger.knife.stattrak, 0, "skin knife stattrak id");

	SetupValue(config->i_drag.x, 0, "indicators pos x");
	SetupValue(config->i_drag.y, 0, "indicators pos y");

	SetupValue(config->s_drag.x, 0, "specatators pos x");
	SetupValue(config->s_drag.y, 0, "specatators pos y");

	SetupValue(config->d_drag.x, 0, "round summary pos x");
	SetupValue(config->d_drag.y, 0, "round summary pos y");

	SetupValue(config->k_drag.x, 0, "keybinds pos x");
	SetupValue(config->k_drag.y, 0, "keybinds pos y");	
}

void C_Config::SetupValue(int& value, int def, std::string name)
{
	value = def;
	ints.push_back(new ConfigValue< int >(name, &value));
}

void C_Config::SetupValue(float& value, float def, std::string name)
{
	value = def;
	floats.push_back(new ConfigValue< float >(name, &value));
}

void C_Config::SetupValue(bool& value, bool def, std::string name)
{
	value = def;
	bools.push_back(new ConfigValue< bool >(name, &value));
}

void C_Config::SetupValue(c_vector3d& value, c_vector3d def, std::string name)
{
	value = def;
	vector.push_back(new ConfigValue< c_vector3d >(name, &value));
}

void C_Config::SetupValue(bool* value, bool def, int size, std::string name) // for multiboxes
{
	for (int c = 0; c < size; c++)
	{
		value[c] = def;

		name += std::to_string(c);

		bools.push_back(new ConfigValue< bool >(name, &value[c]));
	}
}

static char name[] = "lagsync";
void C_Config::Save(int idx)
{
	std::string folder, file;
	static TCHAR path[MAX_PATH];


	if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, path)))
	{
		folder = std::string(path) + ("\\lagsync\\");

		switch (idx) {
		case 0:
			file = std::string(path) + ("\\lagsync\\C_1.ini");break;
		case 1:
			file = std::string(path) + ("\\lagsync\\C_2.ini");break;
		case 2:
			file = std::string(path) + ("\\lagsync\\C_3.ini");break;
		case 3:
			file = std::string(path) + ("\\lagsync\\C_4.ini");break;
		case 4:
			file = std::string(path) + ("\\lagsync\\C_5.ini");break;
		case 5:
			file = std::string(path) + ("\\lagsync\\C_6.ini");break;
		}
	}

	CreateDirectory(folder.c_str(), NULL);

	for (auto value : ints)
		WritePrivateProfileString(name, value->name.c_str(), std::to_string(*value->value).c_str(), file.c_str());

	for (auto value : floats)
		WritePrivateProfileString(name, value->name.c_str(), std::to_string(*value->value).c_str(), file.c_str());

	for (auto value : bools)
		WritePrivateProfileString(name, value->name.c_str(), *value->value ? "true" : "false", file.c_str());

}

void C_Config::Load(int idx)
{
	static TCHAR path[MAX_PATH];
	std::string folder, file;

	if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, path)))
	{
		folder = std::string(path) + ("\\lagsync\\");

		switch (idx) {
		case 0:
			file = std::string(path) + ("\\lagsync\\C_1.ini"); break;
		case 1:
			file = std::string(path) + ("\\lagsync\\C_2.ini"); break;
		case 2:
			file = std::string(path) + ("\\lagsync\\C_3.ini"); break;
		case 3:
			file = std::string(path) + ("\\lagsync\\C_4.ini"); break;
		case 4:
			file = std::string(path) + ("\\lagsync\\C_5.ini"); break;
		case 5:
			file = std::string(path) + ("\\lagsync\\C_6.ini"); break;
		}
	}

	CreateDirectory(folder.c_str(), NULL);

	char value_l[32] = { '\0' };

	for (auto value : ints)
	{
		GetPrivateProfileString(name, value->name.c_str(), "", value_l, 32, file.c_str());
		*value->value = atoi(value_l);
	}

	for (auto value : floats)
	{
		GetPrivateProfileString(name, value->name.c_str(), "", value_l, 32, file.c_str());
		*value->value = atof(value_l);
	}

	for (auto value : bools)
	{
		GetPrivateProfileString(name, value->name.c_str(), "", value_l, 32, file.c_str());
		*value->value = !strcmp(value_l, "true");
	}
}

C_Config* g_Config = new C_Config();
