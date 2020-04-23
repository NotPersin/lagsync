#pragma once

#include "../includes.h"
#include "../new_menu/config.h"

class c_menu : public c_singleton<c_menu>
{
public:


	bool d3dinit = false;

	void GUI_Init(HWND window, IDirect3DDevice9* pDevice);

	void mainWindow();
	bool open = true;

private:
	static auto constexpr alpha_frequency = 255 / 0.15f;

	uint8_t welcome_placeholder;

	int alpha;
	c_vector2d cursor_pos;
};

#define menu c_menu::instance()

namespace fs = std::filesystem;

struct ColorV2
{
	float red, green, blue, alpha;
};

enum
{
	check_box,
	slider,
	combo_box,
	multi_box
};

class Menu22 : public c_singleton<Menu22>
{
public:
	bool menuOpened;



	struct legit
	{
		bool enabled;
		bool legitbacktrack;
		float backtrackTicks;

		bool autopistol;
		bool autofire;
		bool friendlyfire;

		bool aimbotonkeybool = true;
		int aimbotkey = 0;

		int pistolhitbox;
		float pistolsmooth;
		float pistolfov;
		float pistolrecoilx;


		int sniperhitbox;
		float snipersmooth;
		float sniperfov;
		float sniperrecoilx;


		int otherhitbox;
		float othersmooth;
		float otherfov;
		float otherrecoilx;


		int autoblock;

		bool silentaimpistol;
		bool silentaimother;
		bool silentaimawp;

		bool legithop;
		float hop_percent;
	} Legitbot;
	struct ragebot
	{
		bool enable;
		
		struct weapon_config
		{
			int hitchance = 0.f;
			int min_dmg = 0.f;
			int head_scale = 0.f;
			int body_scale = 0.f;
			int chest_scale = 0.f;
			int stomach_scale = 0.f;
			int pelvis_scale = 0.f;
			int feet_scale = 0.f;
			int legs_scale = 0.f;
			int exploits = 0;

			bool optimize_fps;

			bool body_aim_in_air, body_aim_lethal, body_aim_slow_walk, body_aim_if_not_on_shot, body_after_x_missed_enable, body_on_key_enable, health_override_enable;
			int body_aim_health; int body_aim_key; int head_aim_key; int body_after_x_missed; int body_after_x_missed_resolver, min_dmg_vis;
			bool hitscan_head, hitscan_chest, hitscan_stomach, hitscan_pelvis, hitscan_legs, hitscan_feet, on_shot_hitscan_head, on_shot_hitscan_body, body_aim_key_enable;
			bool multipoint_head, multipoint_chest, multipoint_stomach, multipoint_pelvis, multipoint_legs, multipoint_feet, body_after_x_missed_spread;
			bool static_point, safe_point_override_enable; int save_point_override, hitbox_prefer, delay_shot, health_override, hitbox_override, hitbox_override_key;
			bool auto_shoot, autoscope, head_aim_only_while_firing, smart_aim, override_head_aim_only;
		};

		weapon_config pistol, _auto, scout, awp, heavy, other, taser;

		int revolver_threshold;
		bool autoscope;
		bool autofire;
		bool autostop;
		int autostop_combo;
		int weaponconfig;
		bool hideshot;
		bool anglefdix;
		bool delay_shot_exploits;
		bool awpbody;
		int priority;

		int left;
		bool left_enable;
		int right;
		bool right_enable;
		int back;
		bool back_enable;
		int freestanding;
		bool desync_invert_enable;
		int desync_invert;
		int jitterealval;
		int desync_invert_slider;
		int spinval;
		bool spin;
		bool jitter;
		bool rotate;
		int rotate_min;
		int rotate_max;

		struct antiaim_config
		{
			int pitch;
			int pitch_range;
			int yaw;
			int fake_type;
			float max_delta;
			int yawbase;
			bool yaw_jitter;
			int jitter_length;
			int jitter_inverval;
			bool desync_on_shot;
			bool avoid_overlap;
			int fake_invert;
			int lean_break;
			bool twist;

			//deprecated
			int delta;
			int speed;
			bool yaw_offset;
			int yaw_offsets;
			bool crosshdorment;
		};

		antiaim_config stand, moving, air;

		bool _antiaim;
		bool disableaaonknife;
		bool disableaaonfreeze;
		int antiaim_type;

		bool fakelag_enable;
		int lag_type;
		int stand_fakelag;
		int move_fakelag;
		int air_fakelag;
		bool fakelag_onshot;
		bool enable_boost, enable_disable;
		bool fl_disable_shot,fl_disable_reload,fl_disable_switch;
		bool fl_boost_shot, fl_boost_reload, fl_boost_switch, fl_boost_max_delta;
		int fl_boost_scale;
		bool freestanding_enable;
		bool manaul_override;
		bool backtrack;
		int extra_selection;
		bool desync_yaw;
		int desync_yaws;
		int desync_dir_type;
		bool desync_high_crouch;
		bool desync_low_stand;
		bool ignore_autostop;
		float ingore_astrength;
		bool fakeduck_enable;
		int fakeduck;
		bool autozeus;
		bool autoknife;
		bool silent_aim;
		bool autor8;
		int delay_shot;
		int fake_invert;
		bool fastshoot;

		bool doubletapzz;
		bool exploits_enable;
		int exploits;
		bool resolver;
		int doubletapkey;
		int use_custom;
		int tick_custom;
		bool no_recoil;
		bool autorevolver;

		bool body_aim_in_air, body_aim_lethal, body_aim_slow_walk, body_aim_if_not_on_shot, min_dmg_hp;
		int body_aim_health; int body_aim_key; int head_aim_key; int body_after_x_missed_spread; int body_after_x_missed_resolver;
		bool hitscan_head, hitscan_chest, hitscan_stomach, hitscan_pelvis, hitscan_legs, hitscan_feet, on_shot_hitscan_head, on_shot_hitscan_body;
		bool auto_shoot, head_aim_only_while_firing, smart_aim, override_head_aim_only;

	} Ragebot;
	struct visaul
	{
		struct esp {
			// esp
			bool    visuals_enabled = false; 
			bool    visuals_esp_box = false; float box_color[4] = { 1.f, 1.f, 1.f , 1.f };
			bool    visuals_esp_name = false; float name_color[4] = { 1.f, 1.f, 1.f , 1.f };
			bool	visuals_esp_weapon_icons = false; float icons_color[4] = { 1.f, 1.f, 1.f , 1.f };
			bool    visuals_esp_armor_flag = false; float armor_flag_color[4] = { 1.f, 1.f, 1.f , 1.f };
			bool    visuals_esp_fakeduck_flag = false; float fakeduck_flag_color[4] = { 1.f, 1.f, 1.f , 1.f };
			bool    visuals_esp_defuse_flag = false; float defuse_flag_color[4] = { 1.f, 1.f, 1.f , 1.f };
			bool    visuals_esp_scope_flag = false; float scope_flag_color[4] = { 1.f, 1.f, 1.f , 1.f };
			bool    visuals_esp_flash_flag = false; float flash_flag_color[4] = { 1.f, 1.f, 1.f , 1.f };

			bool    visuals_esp_health = false; float health_color[4] = { 1.f, 1.f, 1.f , 1.f };
			bool    visuals_esp_weapon = false; float weapon_color[4] = { 1.f, 1.f, 1.f , 1.f };
			bool	visuals_esp_ammo = false; float ammo_color[4] = { 1.f, 1.f, 1.f , 1.f };
			bool    visuals_esp_armor = false; float armor_color[4] = { 1.f, 1.f, 1.f , 1.f };
			bool    visuals_esp_snaplines = false; float snaplines_color[4] = { 1.f, 1.f, 1.f , 1.f };
			bool	visuals_esp_damage = false; float damage_color[4] = { 1.f, 1.f, 1.f , 1.f };
			bool    visuals_esp_radar = false; float radar_color[4] = { 1.f, 1.f, 1.f , 1.f };

			bool    visuals_esp_skeleton = false; float skeleton_color[4] = { 1.f, 1.f, 1.f , 1.f };
			bool    visuals_esp_bt_skeleton = false; float bt_skeleton_color[4] = { 1.f, 1.f, 1.f , 1.f };

			float dormant_alpha[64];

			//chams
			int chams_option;
			int material_type;
			int xqz_type;
			float color[4] = { 1.f, 1.f, 1.f,1.f };
			float xqz_color[4] = { 1.f, 1.f, 1.f,1.f };
			float gloss[4] = { 1.f, 1.f, 1.f,1.f };
			float xqz_gloss[4] = { 1.f, 1.f, 1.f,1.f };
			bool pulse;
			bool wireframe;
			bool preserve_model;
			bool xqz;
			bool glow;
			int glow_style;
			float glow_alpha;
			bool blend;

		} enemy, team, local, enemy2, team2, local2, weapon, arms, sleeve, weapon2, arms2, sleeve2, backtrack, desync, desync2;

		bool    visuals_dropped_box = false;		 float dropped_box_color[4] = { 1.f, 1.f, 1.f , 1.f };
		bool    visuals_dropped_name = false;		 float dropped_name_color[4] = { 1.f, 1.f, 1.f , 1.f };
		bool	visuals_droppped_name_icons = false; float dropped_icon_color[4] = { 1.f, 1.f, 1.f , 1.f };
		bool    visuals_dropped_ammo = false;		 float dropped_ammo_color[4] = { 1.f, 1.f, 1.f , 1.f };

		int chams_option;
		bool __chams;
		bool xyzchams;

		bool localchams;
		bool fakeanglechams;

		bool glowenable;
		int glowstyle;
		float glowalpha;

		bool localglowenable;
		bool droppedglowenable;
		int localglowstyle;
		int droppedglowstyle;
		float localglowalpha;
		float droppedglowalpha;

		bool  view_setup_fov_enable = false; 
		bool  view_setup_viewmodel_enable = false;
		bool  view_setup_aspect_enable = false;
		bool  view_setup_xoffset_enable = false;
		bool  view_setup_yoffset_enable = false;
		bool  view_setup_zoffset_enable = false;

		float view_setup_fov = 0;
		float view_setup_viewmodel = 0;
		float view_setup_aspect = 0;
		float view_setup_xoffset = 0;
		float view_setup_yoffset = 0;
		float view_setup_zoffset = 0;

		bool novisrecoil;
		bool nopostprocsess;
		bool leftknife;
		bool preserve_killfeed;
		bool noscope;
		bool nosmoke;
		bool noflash;
		bool no_fog;
		int skyboxes;
		bool nightmode;
		bool trails;
		bool ragdoll;
		bool bulletbeams;
		bool beamimpacts;
		bool e_bulletbeams;
		bool e_beamimpacts;
		bool lagcompydady;
		bool remove_zoom;
		int hitsound;
		bool fullbright;
		bool forcecrosshair;
		bool showimpacts;
		bool showgrenade;
		bool bombtimer;
		bool penetration_crosshair;
		float fieldofview;
		float viewmodelfov = 68;
		float viewmodeloffsetx;
		float viewmodeloffsety;
		float viewmodeloffsetz;
		float viewmodelratio;
		int bloom_scale;
		int bloom_cvar_scale;
		float bloom_factor;
		float bloom_expose;
		int indicators_styles;
		bool hitmarker_damage;
		bool snaplines;
		bool grenade_predict;
		bool molov_timer;
		bool smoke_timer;
		bool knife_range;
		bool taser_range;
		int thirdperson;
		bool thirdperson_enable;
		float thirdperson_dist;
		float thirdperson_dist2;
		bool aaindicator;
		bool thirdaaindicator;
		bool round_summ;
		bool indicators;
		bool spec_list;
		bool keybinds;
		bool hitmarker;
		bool hitmarker2d;
		bool killeffect;
		bool hiteffect;
		int player_model;
		int spread_crosshair;
		bool enchance_scope;
		int draw_nades;
		bool nade_style_mol, nade_style_smoke, nade_style_he, nade_style_flash, nade_style_decoy;

		c_vector2d watermark_pos = c_vector2d(1773, 15);
		c_vector2d watermark_pos2 = c_vector2d(1773, 15);
		c_vector2d keybind_pos = c_vector2d(1773, 15);
		c_vector2d dmg_pos = c_vector2d(1773, 15);
		c_vector2d prog_pos = c_vector2d(1773, 15);
		c_vector2d spec_pos = c_vector2d(1773, 15);
		c_vector2d logs_pos = c_vector2d(1773, 15);

	} Visuals;
	struct skins
	{
		bool Enabled;
		//knife
		int weapon_switch;
		int knife_switch;
		int gloves_switch;

		struct weapon
		{
			int id;
			int quality;
			int seed;
			int stattrak;
		};
		weapon awp, usp, p2k, glock, deg, revol, ssg, dual, p250, cz75, five7, tec9, nova, xm1014, knife, mag7, sawed, negev, m249, mp9, mac10, mp7, ump, p90, ppbizon, galil, famas, m4a4, m4a1, ak47, aug, sg553, scar20, g3sg1, mp5, gloves;

		bool updated = false;
	} Skinchanger;
	struct color
	{

		float boxcol[4] = { 255, 255, 255 , 255 };
		float namecol[4] = { 1.f, 1.f, 1.f , 1.f };
		float healthcol[4] = { 1.f, 1.f, 1.f , 1.f };
		float weaponcol[4] = { 1.f, 1.f, 1.f , 1.f };
		float ammobar[4] = { 1.f, 1.f, 1.f , 1.f };
		float ammocol[4] = { 1.f, 1.f, 1.f , 1.f };
		float armorcol[4] = { 1.f, 1.f, 1.f , 1.f };
		float localchamscol[4] = { 1.f, 1.f, 1.f , 1.f };
		float fakechamscol[4] = { 1.f, 1.f, 1.f , 1.f };
		float xyzchamscolor[4] = { 1.f, 1.f, 1.f , 1.f };
		float visiblechamscolor[4] = { 1.f, 1.f, 1.f , 1.f };
		float history_skeletoncol[4] = { 1.f, 1.f, 1.f , 1.f };
		float localglow_col[4] = { 1.f, 1.f, 1.f , 1.f };
		float droppedglow_col[4] = { 1.f, 1.f, 1.f , 1.f };
		float glow_col[4] = { 1.f, 1.f, 1.f , 1.f };
		float poovcob[4] = { 1.f, 1.f, 1.f , 1.f };
		float beamcolor[4] = { 1.f, 1.f, 1.f , 1.f };
		float nightcolor[4] = { 1.f, 1.f, 1.f , 1.f };
		float skycolor[4] = { 1.f, 1.f, 1.f , 1.f };
		float propcolor[4] = { 1.f, 1.f, 1.f , 1.f };
		float nadecolorr[4] = { 1.f, 1.f, 1.f , 1.f };
		float fakeangler[4] = { 1.f, 1.f, 1.f , 1.f };
		float skeletoncol[4] = { 1.f, 1.f, 1.f , 1.f };
		float trails_color[4] = { 1.f, 1.f, 1.f , 1.f };
		float ebeamcolor[4] = { 1.f, 1.f, 1.f , 1.f };
		float lagcompcolor[4] = { 1.f, 1.f, 1.f , 1.f };
		float bombcolor[4] = { 1.f, 1.f, 1.f , 1.f };
		float dmgcolor[4] = { 1.f, 1.f, 1.f , 1.f };
		float dmgcolor2[4] = { 1.f, 1.f, 1.f , 1.f };
		float droppedcolor[4] = { 1.f, 1.f, 1.f , 1.f };
		float droppedammocolor[4] = { 1.f, 1.f, 1.f , 1.f };
		float hitmarker[4] = { 1.f, 1.f, 1.f , 1.f };
		float hitmarker2d[4] = { 1.f, 1.f, 1.f , 1.f };
		float f_aa_ind[4] = { 1.f, 1.f, 1.f , 1.f };
		float f_aa_ind2[4] = { 1.f, 1.f, 1.f , 1.f };
		float t_aa_ind[4] = { 1.f, 1.f, 1.f , 1.f };
		float t_aa_ind2[4] = { 1.f, 1.f, 1.f , 1.f };
		float grenade_predict[4] = { 1.f, 1.f, 1.f , 1.f };
		float molov_timer[4] = { 1.f, 1.f, 1.f , 1.f };
		float smoke_timer[4] = { 1.f, 1.f, 1.f , 1.f };
		float spread_crosshair[4] = { 1.f, 1.f, 1.f , 1.f };
		float snaplines[4] = { 1.f, 1.f, 1.f , 1.f };
		float enchance_scope[4] = { 1.f, 1.f, 1.f , 1.f };

	}Colors;
	struct misc
	{
		bool safemode;
		bool bhop;
		bool autostrafe;
		bool disableModelOcclusion;
		int events;
		bool eventlogs;
		bool slowwalk;
		float slowwalkspeed;
		int slowwalk_key;
		bool watermark;
		bool fakelag;
		int fakelagstand;
		int fakelagrun;
		int fakelagair;
		int fakeduck;
		bool fakeduck_enable;
		int airstuck_key;
		bool airstuck_enable;
		bool clantag;

		int config_selection;
		bool saving;
		bool loading;
		float menucolor[3] = { 0, 0.949, 0.937 };
		float accentcolor[3] = { 0, 0.949, 0.937 };
		char configname[128];

		bool buybot;
		//primary
		bool buy_bot_scar20, buy_bot_g3sg1, buy_bot_ssg08, buy_bot_awp;
		//secondary
		bool buy_bot_elite, buy_bot_p250, buy_bot_tec9, buy_bot_deg;
		//nades molotov; buy incgrenade; buy hegrenade; buy smokegrenade; buy flashbang;
		bool buy_bot_molotov, buy_bot_incgrenade, buy_bot_hegrenade, buy_bot_smokegrenade, buy_bot_flashbang;
		//secondary
		bool buy_bot_vest, buy_bot_vesthelm, buy_bot_taser, buy_bot_defuser;

		bool fakelag_shooting, fakelag_revolver, fakelag_taser, fakelag_knife, fakelag_grenade, fakelag_peek, fakelag_land, fakelag_shot;

	} Misc;

	c_vector2d i_drag;
	c_vector2d s_drag;
	c_vector2d d_drag;
	c_vector2d k_drag;

	struct {
		bool legitbot, ragebot, visual, misc, skinchanger, color;
		int Config;
	}Tabs;

private:
	struct
	{
		float x = 0.f, y = 0.f;
	}Pos; // lol

	enum
	{
		check_box,
		slider,
		combo_box,
		multi_box
	};
	int ControlsX;
	int GroupTabBottom;
	int OffsetY;
	int OldOffsetY;
	int TabOffset;
	int SubTabOffset;
	float SubTabSize; // cpp fuckin sux had to make this a float or the whole thing crashes
	float TabSize;
	int GroupTabPos[4];
	int TabNum = 0;
	int SubTabNum = 0;
	int PreviousControl = -1;

	/*void Tab(std::string name);
	void SubTab(std::string name);
	void Text(std::string name);
	void CheckBox(std::string name, bool* item);
	void Slider(int max, std::string name, int* item);
	void ComboBox(std::string name, std::vector< std::string > itemname, int* item);
	void MultiComboBox(std::string name, std::vector< std::string > itemname, bool* item);*/
	void ColorPicker(std::string name, ColorV2& item);

	
};

#define config Menu22::instance()

void local_menu();
