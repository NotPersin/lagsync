#include "c_menu.h"
#include "../hooks/idirect3ddevice9.h"
#include "../sdk/c_input_system.h"
#include "../images/logo.h"
#include <memory>
#include "../imgui/imgui.h"
#include "../imgui/font.hh"
#include "../new_menu/config.h"
#include "../sdk/c_client_state.h"
#include "../hacks/c_esp.h"

#define IM_ARRAYSIZE(_ARR)      ((int)(sizeof(_ARR)/sizeof(*_ARR)))
#define RGBA_TO_FLOAT(r,g,b,a) (float)r/255.0f, (float)g/255.0f, (float)b/255.0f, (float)a/255.0f
float max_array2[4] = { 1.f, 1.f, 1.f,1.f };
#define array(a) memcpy(a, max_array2, sizeof(a));
bool menuopen;
static int idx = 0;
static ImFont* font_primary;
static ImFont* color_font;
static ImFont* font_secondary;

void local_menu() {

	static const char* spread_type[] = { "none","twinkle","outline","" };
	const char* hitsounds[] = { "none", "arena_switch", "impact","timer" };
	const char* styles[] = { "basic", "advanced" };
	const char* player_models[] = {
	"none",
	"balkan 1",
	"balkan 2",
	"balkan 3",
	"balkan 4",
	"balkan 5",
	"fbi 1",
	"fbi 2",
	"fbi 3",
	"heavy",
	"st6 1",
	"st6 2",
	"st6 3",
	"st6 4",
	"st6 5",
	"st6 6",
	"st6 7",
	"st6 8",
	"st6 9",
	"terror 1",
	"terror 2",
	"terror 3",
	"terror 4",
	"phenoix 1",
	"phenoix 2",
	"phenoix 3",
	"",
	};
	ImGuiStyle& style = ImGui::GetStyle();
	ImGui::Checkbox("esp enabled", &config->Visuals.local.visuals_enabled);
	ImGui::Checkbox("esp box", &config->Visuals.local.visuals_esp_box, true);		ImGui::SameLine(); ImGui::MyColorEdit3(("##box color"), (float*)&config->Visuals.local.box_color, 1 << 7 );
	ImGui::Checkbox("esp name", &config->Visuals.local.visuals_esp_name, true);			ImGui::SameLine(); ImGui::MyColorEdit3(("##name color"), (float*)&config->Visuals.local.name_color, 1 << 7 );
	ImGui::Checkbox("esp health", &config->Visuals.local.visuals_esp_health, true);		
	ImGui::Checkbox("esp armor", &config->Visuals.local.visuals_esp_armor, true);			ImGui::SameLine(); ImGui::MyColorEdit3(("##armor color"), (float*)&config->Visuals.local.armor_color, 1 << 7 );
	ImGui::Checkbox("esp weapon", &config->Visuals.local.visuals_esp_weapon, true);		ImGui::SameLine(); ImGui::MyColorEdit3(("##weapon color"), (float*)&config->Visuals.local.weapon_color, 1 << 7 );
	ImGui::Checkbox("esp icons", &config->Visuals.local.visuals_esp_weapon_icons, true);
	ImGui::Checkbox("esp ammo", &config->Visuals.local.visuals_esp_ammo, true);           ImGui::SameLine(); ImGui::MyColorEdit3(("##ammo color"), (float*)&config->Visuals.local.ammo_color, 1 << 7 );
	ImGui::Checkbox("esp skeleton", &config->Visuals.local.visuals_esp_skeleton, true);			 ImGui::SameLine(); ImGui::MyColorEdit3(("##skeleton color"), (float*)&config->Visuals.local.skeleton_color, 1 << 7 );
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);

	if (ImGui::BeginCombo("flags", "...", ImVec2(0, 255))) {
		const auto cap = 62;
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
		ImGui::Selectable(("armor"), &config->Visuals.local.visuals_esp_armor_flag, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
		ImGui::Selectable(("defuse"), &config->Visuals.local.visuals_esp_defuse_flag, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
		ImGui::Selectable(("fakeduck"), &config->Visuals.local.visuals_esp_fakeduck_flag, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
		ImGui::Selectable(("scope"), &config->Visuals.local.visuals_esp_scope_flag, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
		ImGui::Selectable(("flash"), &config->Visuals.local.visuals_esp_flash_flag, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
		ImGui::EndCombo();
	}
	
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 3);
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 9);

	ImGui::Text("  flag colors"); 
	ImGui::SameLine();
	{
		style.ColorOverride.x = -60;
		ImGui::MyColorEdit3(("##armorflagcolor"), (float*)&config->Visuals.local.armor_flag_color, 1 << 7 );
		ImGui::SameLine();
	}
	{
		style.ColorOverride.x = -45;
		ImGui::MyColorEdit3(("##defuseflagcolor"), (float*)&config->Visuals.local.defuse_flag_color, 1 << 7 );
		ImGui::SameLine();
	}
	{
		style.ColorOverride.x = -30;
		ImGui::MyColorEdit3(("##fakeduckflagcolor"), (float*)&config->Visuals.local.fakeduck_flag_color, 1 << 7 );
		ImGui::SameLine();
	}
	{
		style.ColorOverride.x = -15;
		ImGui::MyColorEdit3(("##scopeflagcolor"), (float*)&config->Visuals.local.scope_flag_color, 1 << 7 );
		ImGui::SameLine();
	}
	{
		style.ColorOverride.x = -1;
		ImGui::MyColorEdit3(("##flashflagcolor"), (float*)&config->Visuals.local.flash_flag_color, 1 << 7 );
		ImGui::SameLine();
	}
	
	ImGui::Text("");
	ImGui::Text("");
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 6);
	ImGui::Checkbox(("beams##bullettracers"), &config->Visuals.bulletbeams, true);	ImGui::SameLine();
	ImGui::MyColorEdit3(("##beamcolorzzz"), (float*)&config->Colors.beamcolor, 1 << 7);
	//ImGui::Checkbox(("movement trails##movementtrails"), &_config->Visuals.trails, true);	ImGui::SameLine();
	//ImGui::MyColorEdit3(("##movecolorzzz"), (float*)&_config->Colors.trails_color, 1 << 7 | ImGuiColorEditFlags_Alpha2);
	ImGui::Combo("player model##playermodel", &config->Visuals.player_model, player_models, ARRAYSIZE(player_models)); ImGui::Text("  ");
	ImGui::Checkbox(("kill effect##killeffect"), &config->Visuals.killeffect);
	ImGui::Checkbox(("hit effect##hiteffect"), &config->Visuals.hiteffect);
	ImGui::Checkbox(("first aa indicator##zxvvxzzxv"), &config->Visuals.aaindicator, 2);   ImGui::SameLine(); style.ColorOverride.x = -20; ImGui::MyColorEdit3(("##fsdsaacolor"), (float*)&config->Colors.f_aa_ind, 1 << 7 | ImGuiColorEditFlags_Alpha2); ImGui::SameLine(); style.ColorOverride.x = -1; ImGui::MyColorEdit3(("##fassdaacolor"), (float*)&config->Colors.f_aa_ind2, 1 << 7 | ImGuiColorEditFlags_Alpha2);
	ImGui::Checkbox(("third aa indicator##sf3242x"), &config->Visuals.thirdaaindicator, 2);  ImGui::SameLine(); style.ColorOverride.x = -20; ImGui::MyColorEdit3(("##t2saacolor"), (float*)&config->Colors.t_aa_ind, 1 << 7); ImGui::SameLine(); style.ColorOverride.x = -1; ImGui::MyColorEdit3(("##asdqqsdx"), (float*)&config->Colors.t_aa_ind2, 1 << 7);
	ImGui::Checkbox("hitmarker", &config->Visuals.hitmarker2d);
	ImGui::Checkbox("penetration crosshair##penetration_crosshairv", &config->Visuals.penetration_crosshair);
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 6);
	ImGui::Combo("spread crosshair##spreadcrosshair", &config->Visuals.spread_crosshair, spread_type, ARRAYSIZE(spread_type)); 
	ImGui::Dummy(ImVec2(0, 8));
	ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 15, ImGui::GetCursorPosY() - 7));
	ImGui::Text("color");
	ImGui::SameLine(); style.ColorOverride.x = -1; ImGui::MyColorEdit3(("##spreadcolor"), (float*)&config->Colors.spread_crosshair, 1 << 7 | ImGuiColorEditFlags_Alpha2); ImGui::Dummy(ImVec2(0, 6));
	ImGui::Combo("hitsound##hitsound", &config->Visuals.hitsound, hitsounds, ARRAYSIZE(hitsounds)); ImGui::Text("");
	//ImGui::Checkbox(("hitmarker"), &_config->Visuals.hitmarker, true);	ImGui::SameLine(); 	style.ColorOverride.x = -1;
	//ImGui::MyColorEdit3(("##hitmarkercolor"), (float*)&_config->Colors.hitmarker2d, 1 << 7 | ImGuiColorEditFlags_Alpha2);
	ImGui::Checkbox(("enchance scope##enchancescope"), &config->Visuals.enchance_scope, true);	ImGui::SameLine(); 	style.ColorOverride.x = -1;
	ImGui::MyColorEdit3(("##enchance_scopezz"), (float*)&config->Colors.enchance_scope, 1 << 7 | ImGuiColorEditFlags_Alpha2);
	ImGui::Checkbox("taser range##drawtaserrange", &config->Visuals.taser_range);
	ImGui::Checkbox("knife range##drawkniferange", &config->Visuals.knife_range);
	ImGui::Text("  ");
}
void enemy_menu() {

	ImGuiStyle& style = ImGui::GetStyle();

	ImGui::Checkbox("esp enabled", &config->Visuals.enemy.visuals_enabled);
	ImGui::Checkbox("esp box", &config->Visuals.enemy.visuals_esp_box, true);						ImGui::SameLine(); ImGui::MyColorEdit3(("##box color"), (float*)&config->Visuals.enemy.box_color, 1 << 7 );
	ImGui::Checkbox("esp name", &config->Visuals.enemy.visuals_esp_name, true);						ImGui::SameLine(); ImGui::MyColorEdit3(("##name color"), (float*)&config->Visuals.enemy.name_color, 1 << 7 );
	ImGui::Checkbox("esp health", &config->Visuals.enemy.visuals_esp_health, true);	
	ImGui::Checkbox("esp armor", &config->Visuals.enemy.visuals_esp_armor, true);					ImGui::SameLine(); ImGui::MyColorEdit3(("##armor color"), (float*)&config->Visuals.enemy.armor_color, 1 << 7 );
	ImGui::Checkbox("esp weapon", &config->Visuals.enemy.visuals_esp_weapon, true);					ImGui::SameLine(); ImGui::MyColorEdit3(("##weapon color"), (float*)&config->Visuals.enemy.weapon_color, 1 << 7 );
	ImGui::Checkbox("esp icons", &config->Visuals.enemy.visuals_esp_weapon_icons, true);			
	ImGui::Checkbox("esp ammo", &config->Visuals.enemy.visuals_esp_ammo, true);						ImGui::SameLine(); ImGui::MyColorEdit3(("##ammo color"), (float*)&config->Visuals.enemy.ammo_color, 1 << 7 );
	ImGui::Checkbox("esp damage", &config->Visuals.enemy.visuals_esp_damage, true);					ImGui::SameLine(); ImGui::MyColorEdit3(("##damage color"), (float*)&config->Visuals.enemy.damage_color, 1 << 7 | ImGuiColorEditFlags_Alpha2);
	ImGui::Checkbox("esp snaplines", &config->Visuals.enemy.visuals_esp_snaplines, true);			ImGui::SameLine(); ImGui::MyColorEdit3(("##snaplines color"), (float*)&config->Visuals.enemy.snaplines_color, 1 << 7 | ImGuiColorEditFlags_Alpha2);
	ImGui::Checkbox("esp skeleton", &config->Visuals.enemy.visuals_esp_skeleton, true);			    ImGui::SameLine(); ImGui::MyColorEdit3(("##skeleton color"), (float*)&config->Visuals.enemy.skeleton_color, 1 << 7 | ImGuiColorEditFlags_Alpha2);
	ImGui::Checkbox("esp history skeleton", &config->Visuals.enemy.visuals_esp_bt_skeleton, true);  ImGui::SameLine(); ImGui::MyColorEdit3(("##bt skeleton color"), (float*)&config->Visuals.enemy.bt_skeleton_color, 1 << 7 | ImGuiColorEditFlags_Alpha2);
	ImGui::Checkbox("esp radar", &config->Visuals.enemy.visuals_esp_radar, true);					ImGui::SameLine(); ImGui::MyColorEdit3(("##radar color"), (float*)&config->Visuals.enemy.radar_color, 1 << 7 | ImGuiColorEditFlags_Alpha2);
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);

	if (ImGui::BeginCombo("flags", "...", ImVec2(0, 255))) {
		const auto cap = 62;
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
		ImGui::Selectable(("armor"), &config->Visuals.enemy.visuals_esp_armor_flag, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
		ImGui::Selectable(("defuse"), &config->Visuals.enemy.visuals_esp_defuse_flag, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
		ImGui::Selectable(("fakeduck"), &config->Visuals.enemy.visuals_esp_fakeduck_flag, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
		ImGui::Selectable(("scope"), &config->Visuals.enemy.visuals_esp_scope_flag, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
		ImGui::Selectable(("flash"), &config->Visuals.enemy.visuals_esp_flash_flag, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
		ImGui::EndCombo();
	}

	ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 3);
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 9);

	ImGui::Text("  flag colors");
	ImGui::SameLine();
	{
		style.ColorOverride.x = -60;
		ImGui::MyColorEdit3(("##armorflagcolor"), (float*)&config->Visuals.enemy.armor_flag_color, 1 << 7 );
		ImGui::SameLine();
	}
	{
		style.ColorOverride.x = -45;
		ImGui::MyColorEdit3(("##defuseflagcolor"), (float*)&config->Visuals.enemy.defuse_flag_color, 1 << 7 );
		ImGui::SameLine();
	}
	{
		style.ColorOverride.x = -30;
		ImGui::MyColorEdit3(("##fakeduckflagcolor"), (float*)&config->Visuals.enemy.fakeduck_flag_color, 1 << 7 );
		ImGui::SameLine();
	}
	{
		style.ColorOverride.x = -15;
		ImGui::MyColorEdit3(("##scopeflagcolor"), (float*)&config->Visuals.enemy.scope_flag_color, 1 << 7 );
		ImGui::SameLine();
	}
	{
		style.ColorOverride.x = -1;
		ImGui::MyColorEdit3(("##flashflagcolor"), (float*)&config->Visuals.enemy.flash_flag_color, 1 << 7 );
		ImGui::SameLine();
	}
	ImGui::Text("");	ImGui::Text("");
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 6);

	ImGui::Checkbox(("enemy beams##enemy bullettracers"), &config->Visuals.e_bulletbeams, true);
	ImGui::SameLine();
	ImGui::MyColorEdit3(("##ebeamcolorzzz"), (float*)&config->Colors.ebeamcolor, 1 << 7);

	ImGui::Checkbox(("hitbox##coloredhitboxes"), &config->Visuals.lagcompydady, true);
	ImGui::SameLine();
	ImGui::MyColorEdit3(("##lagcompcolor"), (float*)&config->Colors.lagcompcolor, 1 << 7 | ImGuiColorEditFlags_Alpha2);
}
void team_menu() {

	ImGuiStyle& style = ImGui::GetStyle();

	ImGui::Checkbox("esp enabled", &config->Visuals.team.visuals_enabled);
	ImGui::Checkbox("esp box", &config->Visuals.team.visuals_esp_box, true);					 ImGui::SameLine(); ImGui::MyColorEdit3(("##box color"), (float*)&config->Visuals.team.box_color, 1 << 7 );
	ImGui::Checkbox("esp name", &config->Visuals.team.visuals_esp_name, true);					 ImGui::SameLine(); ImGui::MyColorEdit3(("##name color"), (float*)&config->Visuals.team.name_color, 1 << 7 );
	ImGui::Checkbox("esp health", &config->Visuals.team.visuals_esp_health, true);				 
	ImGui::Checkbox("esp armor", &config->Visuals.team.visuals_esp_armor, true);				 ImGui::SameLine(); ImGui::MyColorEdit3(("##armor color"), (float*)&config->Visuals.team.armor_color, 1 << 7 );
	ImGui::Checkbox("esp weapon", &config->Visuals.team.visuals_esp_weapon, true);				 ImGui::SameLine(); ImGui::MyColorEdit3(("##weapon color"), (float*)&config->Visuals.team.weapon_color, 1 << 7 );
	ImGui::Checkbox("esp icons", &config->Visuals.team.visuals_esp_weapon_icons, true);			 
	ImGui::Checkbox("esp ammo", &config->Visuals.team.visuals_esp_ammo, true);					 ImGui::SameLine(); ImGui::MyColorEdit3(("##ammo color"), (float*)&config->Visuals.team.ammo_color, 1 << 7 );
	ImGui::Checkbox("esp snaplines", &config->Visuals.team.visuals_esp_snaplines, true);		 ImGui::SameLine(); ImGui::MyColorEdit3(("##snaplines color"), (float*)&config->Visuals.team.snaplines_color, 1 << 7 | ImGuiColorEditFlags_Alpha2);
	ImGui::Checkbox("esp skeleton", &config->Visuals.team.visuals_esp_skeleton, true);			 ImGui::SameLine(); ImGui::MyColorEdit3(("##skeleton color"), (float*)&config->Visuals.team.skeleton_color, 1 << 7 | ImGuiColorEditFlags_Alpha2);
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);

	if (ImGui::BeginCombo("flags", "...", ImVec2(0, 255))) {
		const auto cap = 62;
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
		ImGui::Selectable(("armor"), &config->Visuals.team.visuals_esp_armor_flag, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
		ImGui::Selectable(("defuse"), &config->Visuals.team.visuals_esp_defuse_flag, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
		ImGui::Selectable(("fakeduck"), &config->Visuals.team.visuals_esp_fakeduck_flag, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
		ImGui::Selectable(("scope"), &config->Visuals.team.visuals_esp_scope_flag, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
		ImGui::Selectable(("flash"), &config->Visuals.team.visuals_esp_flash_flag, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
		ImGui::EndCombo();
	}

	ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 3);
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 9);

	ImGui::Text("  flag colors");
	ImGui::SameLine();
	{
		style.ColorOverride.x = -60;
		ImGui::MyColorEdit3(("##armorflagcolor"), (float*)&config->Visuals.team.armor_flag_color, 1 << 7 );
		ImGui::SameLine();
	}
	{
		style.ColorOverride.x = -45;
		ImGui::MyColorEdit3(("##defuseflagcolor"), (float*)&config->Visuals.team.defuse_flag_color, 1 << 7 );
		ImGui::SameLine();
	}
	{
		style.ColorOverride.x = -30;
		ImGui::MyColorEdit3(("##fakeduckflagcolor"), (float*)&config->Visuals.team.fakeduck_flag_color, 1 << 7 );
		ImGui::SameLine();
	}
	{
		style.ColorOverride.x = -15;
		ImGui::MyColorEdit3(("##scopeflagcolor"), (float*)&config->Visuals.team.scope_flag_color, 1 << 7 );
		ImGui::SameLine();
	}
	{
		style.ColorOverride.x = -1;
		ImGui::MyColorEdit3(("##flashflagcolor"), (float*)&config->Visuals.team.flash_flag_color, 1 << 7 );
		ImGui::SameLine();
	}
}
void world_menu() {
	ImGuiStyle& style = ImGui::GetStyle();
	const char* glowstyles[] = { "normal", "full","" };
	const char* nade_styles[] = { "text", "icon","" };

	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);

	if (ImGui::BeginCombo("dropped weapons", "...", ImVec2(0, 255))) {
		const auto cap = 62;
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
		ImGui::Selectable(("name"), &config->Visuals.visuals_dropped_name, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
		ImGui::Selectable(("icons"), &config->Visuals.visuals_droppped_name_icons, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
		ImGui::Selectable(("ammo"), &config->Visuals.visuals_dropped_ammo, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
		ImGui::EndCombo();
	}

	if (config->Visuals.visuals_dropped_name) {
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 7); ImGui::Dummy(ImVec2(0, 0)); ImGui::SameLine();
		ImGui::Text("  dropped name color");  ImGui::SameLine(); style.ColorOverride.x = -1;	ImGui::MyColorEdit3(("##droppednamecolor"), (float*)&config->Visuals.dropped_name_color, 1 << 7 | ImGuiColorEditFlags_Alpha2);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 9);
	}
	if (config->Visuals.visuals_dropped_ammo) {
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 7); ImGui::Dummy(ImVec2(0, 0)); ImGui::SameLine();
		ImGui::Text("  dropped ammo color");  ImGui::SameLine(); style.ColorOverride.x = -1;	ImGui::MyColorEdit3(("##droppedammocolor"), (float*)&config->Visuals.dropped_ammo_color, 1 << 7 | ImGuiColorEditFlags_Alpha2);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 9);
	}

	ImGui::Checkbox("dropped glow##enable_glow", &config->Visuals.droppedglowenable, true);
	ImGui::SameLine(); ImGui::MyColorEdit3(("##enable_glowcolor"), (float*)&config->Colors.droppedglow_col, 1 << 7 | ImGuiColorEditFlags_Alpha2);
	ImGui::Combo("droppped style##localfgopt", &config->Visuals.droppedglowstyle, glowstyles, ARRAYSIZE(glowstyles));
	static const char* Skyboxmode[] = { "off","ymaja","himalaya","mijtm","jungle","new york","light","night","storm","galaxy","" };
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 3);
	ImGui::Combo("skybox##cpskyboxchanga", &config->Visuals.skyboxes, Skyboxmode, IM_ARRAYSIZE(Skyboxmode));
	ImGui::Text("");
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 10); ImGui::Dummy(ImVec2(1, 0)); ImGui::SameLine();
	ImGui::Text("  nightmode"); ImGui::SameLine(); 	style.ColorOverride.x = -30;	ImGui::MyColorEdit3(("##nightcolor"), (float*)&config->Colors.nightcolor, 1 << 7 | ImGuiColorEditFlags_Alpha2); ImGui::SameLine(); style.ColorOverride.x = -15;	ImGui::MyColorEdit3(("##propcolor"), (float*)&config->Colors.propcolor, 1 << 7 | ImGuiColorEditFlags_Alpha2); ImGui::SameLine(); style.ColorOverride.x = -1;	ImGui::MyColorEdit3(("##skycolor"), (float*)&config->Colors.skycolor, 1 << 7);
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 9);
	ImGui::Checkbox(("fullbright##fullbright"), &config->Visuals.fullbright);
	ImGui::Checkbox(("ragdoll gravity##ragddoll"), &config->Visuals.ragdoll);
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);

	if (ImGui::BeginCombo("nade info", "...", ImVec2(0, 255))) {
		const auto cap = 62;
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
		ImGui::Selectable(("molotov"), &config->Visuals.nade_style_mol, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
		ImGui::Selectable(("smoke"), &config->Visuals.nade_style_smoke, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
		ImGui::Selectable(("he"), &config->Visuals.nade_style_he, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
		ImGui::Selectable(("flash"), &config->Visuals.nade_style_flash, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);

		ImGui::EndCombo();
	}

	ImGui::Combo("nade style", &config->Visuals.draw_nades, nade_styles, ARRAYSIZE(nade_styles));

	ImGui::Text(" ");

	ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 7); ImGui::Dummy(ImVec2(0, 0)); ImGui::SameLine();
	ImGui::Text("  nade color");  ImGui::SameLine(); style.ColorOverride.x = -1;	ImGui::MyColorEdit3(("##nadecolor"), (float*)&config->Colors.nadecolorr, 1 << 7 | ImGuiColorEditFlags_Alpha2);
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 9);

	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);

	if (ImGui::BeginCombo("removals", "...", ImVec2(0, 255))) {
		const auto cap = 62;
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
		ImGui::Selectable(("post process"), &config->Visuals.nopostprocsess, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
		ImGui::Selectable(("scope"), &config->Visuals.noscope, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
		ImGui::Selectable(("smoke"), &config->Visuals.nosmoke, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
		ImGui::Selectable(("flash"), &config->Visuals.noflash, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
		ImGui::Selectable(("zoom"), &config->Visuals.remove_zoom, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
		ImGui::Selectable(("fog"), &config->Visuals.no_fog, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
		ImGui::Selectable(("recoil"), &config->Visuals.novisrecoil, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
		ImGui::EndCombo();
	}
	
	if (ImGui::BeginCombo("additions", "...", ImVec2(0, 255))) {
		const auto cap = 62;
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
		ImGui::Selectable(("force crosshair##forcecrosshair"), &config->Visuals.forcecrosshair, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
		ImGui::Selectable(("impacts##aasswdads"), &config->Visuals.showimpacts, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
		ImGui::Selectable("grenade path", &config->Visuals.showgrenade, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
		ImGui::Selectable("bomb timer", &config->Visuals.bombtimer, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
		ImGui::Selectable("preserve killfeed", &config->Visuals.preserve_killfeed, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
		ImGui::Selectable("foot trails", &config->Visuals.trails, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
		ImGui::EndCombo();
	}
	if (config->Visuals.bombtimer) {
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 7); ImGui::Dummy(ImVec2(0, 0)); ImGui::SameLine();
		ImGui::Text("  bomb timer color");  ImGui::SameLine(); style.ColorOverride.x = -1;	ImGui::MyColorEdit3(("##bombtimercolor"), (float*)&config->Colors.bombcolor, 1 << 7 | ImGuiColorEditFlags_Alpha2);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 9);
	}
	if (config->Visuals.trails) {
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 7); ImGui::Dummy(ImVec2(0, 0)); ImGui::SameLine();
		ImGui::Text("  foot trails color");  ImGui::SameLine(); style.ColorOverride.x = -1;	ImGui::MyColorEdit3(("##foottrailscolor"), (float*)&config->Colors.trails_color, 1 << 7 | ImGuiColorEditFlags_Alpha2);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 9);
	}
	//ImGui::Checkbox("force crosshair##forcecrosshair", &config->Visuals.forcecrosshair);
	//ImGui::Checkbox(("show impacts##aasswdads"), &config->Visuals.showimpacts);
	//ImGui::Checkbox(("show grenade path"), &config->Visuals.showgrenade);
	//ImGui::Checkbox("bomb timer", &config->Visuals.bombtimer, true); ImGui::SameLine(); ImGui::MyColorEdit3(("##bombtimer color"), (float*)&config->Colors.bombcolor, 1 << 7 | ImGuiColorEditFlags_Alpha2);
	//ImGui::Checkbox("preserve killfeed", &config->Visuals.preserve_killfeed);
	//ImGui::Checkbox("foot trails", &config->Visuals.trails, true); ImGui::SameLine(); ImGui::MyColorEdit3(("##trails_color color"), (float*)&config->Colors.trails_color, 1 << 7 | ImGuiColorEditFlags_Alpha2);

	//ImGui::SliderInt("bloom scale", &config->Visuals.bloom_scale, 0.f, 500.f, ("%.0f"));
	//ImGui::SliderFloat("bloom exposure", &config->Visuals.bloom_expose, 0.f, 2000.f, ("%0.f"));

	//ImGui::SliderInt("bloom cvar scale", &config->Visuals.bloom_cvar_scale, 0.f, 500.f, ("%.0f"));
	//ImGui::SliderFloat("bloom factor", &config->Visuals.bloom_factor, 0.f, 2000.f, ("%0.f"));

	ImGui::Text("  ");
}
void chams_menu() {

	static const char* chamslayer[] = {"base","underlay"};
	static const char* chamsmode[] = { "arms","weapons","sleeves","enemy","allies","self","desync","backtrack", "" };

	const char* glowstyles[] = { "normal", "full","" };
	const char* materials[] = {
	"none", // -1
	"textured", //  0
	"flat",     //  1
	"custom", //  2
	"GY",	    //  3
	"glowing",  //  4
	"ray",		//  5
	"glass",	//  6
	"plastic",	//  7
	"chrome",	//  8
	"oilspill",	//  9
	"rotation",	//  10
	"wierd",		//  11
	"final",	//  12
	"net",		//  13
	"pearlesscent",	//  14
	"platinum(CUSTOM)",	//  15
	"bubbly",   //  16
	"vital",    //  17
	"",
	};

	ImGuiStyle& style = ImGui::GetStyle();
	auto check_material_type = [&](int _index) -> bool {
		const auto index = _index - 1;
     
		if ( index == -1 || index == 0 || index == 1 || index == 5 || index == 10 || index == 11 || index == 12 || index == 13)
			return false;
		return true;
	};

	auto check_material_type2 = [&](int _index) -> bool {
		const auto index = _index - 1;
		
		if (index == 0 || index == 1 || index == 2 || index == 3 || index == 10 ||index == 5 || index == 6 || index == 10 || index == 11 || index == 12|| index == 13 || index == 17)
			return true;
		return false;
	};

	auto check_transparency = [&](int _index) -> bool {
		const auto index = _index - 1;

		if (index == 2 || index == 3 || index == 6 || index == 17 || index == 18 || index == 19)
			return false;
		return true;
	};

	style.ComboOverride.x = 0;

	ImGui::Checkbox("enable chams", &config->Visuals.__chams);
	ImGui::Combo("chams", &config->Visuals.chams_option, chamsmode, IM_ARRAYSIZE(chamsmode));

	switch (config->Visuals.chams_option) {
	case 0: {
		ImGui::Combo("layer", &config->Visuals.weapon.chams_option, chamslayer, IM_ARRAYSIZE(chamslayer));
		if (config->Visuals.weapon.chams_option == 0) {
			ImGui::Combo("base material##weaasdaponopt", &config->Visuals.weapon.material_type, materials, ARRAYSIZE(materials));
			ImGui::Text(""); ImGui::Dummy(ImVec2(0, 6));
			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 15, ImGui::GetCursorPosY() - 15));
			ImGui::Text("color"); {
				if (config->Visuals.weapon.material_type) {
					if (check_material_type2(config->Visuals.weapon.material_type)) {
						ImGui::SameLine(); style.ColorOverride.x = -1;
						ImGui::MyColorEdit3("##weaponcolor", (float*)&config->Visuals.weapon.color, 1 << 7 | ImGuiColorEditFlags_Alpha2);
					}
					if (check_material_type(config->Visuals.weapon.material_type)) {
						ImGui::SameLine(); style.ColorOverride.x = check_material_type2(config->Visuals.weapon.material_type) ? -17 : -1;
						ImGui::MyColorEdit3("##weapongloss", (float*)&config->Visuals.weapon.gloss, check_transparency(config->Visuals.weapon.material_type) ? 1 << 7 | ImGuiColorEditFlags_Alpha2 : 1 << 7);
					}
				}
				ImGui::Dummy(ImVec2(0, 6));
			}
			ImGui::Checkbox("retain model##w_retainmodel", &config->Visuals.weapon.preserve_model);
			ImGui::Checkbox("pulse##w_pulse", &config->Visuals.weapon.pulse);
			ImGui::Checkbox("wireframe##w_wireframe", &config->Visuals.weapon.wireframe);
		}
		if (config->Visuals.weapon.chams_option == 1) {
			ImGui::Combo("underlay material##wesdaaponopt", &config->Visuals.weapon2.material_type, materials, ARRAYSIZE(materials));
			ImGui::Text(""); ImGui::Dummy(ImVec2(0, 6));
			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 15, ImGui::GetCursorPosY() - 15));
			ImGui::Text("color");
			{
				if (config->Visuals.weapon2.material_type) {
					if (check_material_type2(config->Visuals.weapon2.material_type)) {
						ImGui::SameLine(); style.ColorOverride.x = -1;
						ImGui::MyColorEdit3("##weapon2color", (float*)&config->Visuals.weapon2.color, 1 << 7 | ImGuiColorEditFlags_Alpha2);
					}
					if (check_material_type(config->Visuals.weapon2.material_type)) {
						ImGui::SameLine(); style.ColorOverride.x = check_material_type2(config->Visuals.weapon2.material_type) ? -17 : -1;
						ImGui::MyColorEdit3("##weapon2gloss", (float*)&config->Visuals.weapon2.gloss, check_transparency(config->Visuals.weapon2.material_type) ? 1 << 7 | ImGuiColorEditFlags_Alpha2 : 1 << 7);
					}
				}
				ImGui::Dummy(ImVec2(0, 6));
			}

			ImGui::Checkbox("retain model##w2_retainmodel", &config->Visuals.weapon2.preserve_model);
			ImGui::Checkbox("pulse##w2_pulse", &config->Visuals.weapon2.pulse);
			ImGui::Checkbox("wireframe##w2_wireframe", &config->Visuals.weapon2.wireframe);
		}
	} break; // weapon
	case 1: {
		ImGui::Combo("layer", &config->Visuals.arms.chams_option, chamslayer, IM_ARRAYSIZE(chamslayer));
		if (config->Visuals.arms.chams_option == 0) {
			ImGui::Combo("base material##weaasdaponopt", &config->Visuals.arms.material_type, materials, ARRAYSIZE(materials));
			ImGui::Text(""); ImGui::Dummy(ImVec2(0, 6));
			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 15, ImGui::GetCursorPosY() - 15));
			ImGui::Text("color"); {
				if (config->Visuals.arms.material_type) {
					if (check_material_type2(config->Visuals.arms.material_type)) {
						ImGui::SameLine(); style.ColorOverride.x = -1;
						ImGui::MyColorEdit3("##armscolor", (float*)&config->Visuals.arms.color, 1 << 7 | ImGuiColorEditFlags_Alpha2);
					}
					if (check_material_type(config->Visuals.arms.material_type)) {
						ImGui::SameLine(); style.ColorOverride.x = check_material_type2(config->Visuals.arms.material_type) ? -17 : -1;
						ImGui::MyColorEdit3("##armsgloss", (float*)&config->Visuals.arms.gloss, check_transparency(config->Visuals.arms.material_type) ? 1 << 7 | ImGuiColorEditFlags_Alpha2 : 1 << 7);
					}
				}
				ImGui::Dummy(ImVec2(0, 6));
			}
			ImGui::Checkbox("retain model##w_retainmodel", &config->Visuals.arms.preserve_model);
			ImGui::Checkbox("pulse##w_pulse", &config->Visuals.arms.pulse);
			ImGui::Checkbox("wireframe##w_wireframe", &config->Visuals.arms.wireframe);
		}
		if (config->Visuals.arms.chams_option == 1) {
			ImGui::Combo("underlay material##wesdaaponopt", &config->Visuals.arms2.material_type, materials, ARRAYSIZE(materials));
			ImGui::Text(""); ImGui::Dummy(ImVec2(0, 6));
			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 15, ImGui::GetCursorPosY() - 15));
			ImGui::Text("color");
			{
				if (config->Visuals.arms2.material_type) {
					if (check_material_type2(config->Visuals.arms2.material_type)) {
						ImGui::SameLine(); style.ColorOverride.x = -1;
						ImGui::MyColorEdit3("##arms2color", (float*)&config->Visuals.arms2.color, 1 << 7 | ImGuiColorEditFlags_Alpha2);
					}
					if (check_material_type(config->Visuals.arms2.material_type)) {
						ImGui::SameLine(); style.ColorOverride.x = check_material_type2(config->Visuals.arms2.material_type) ? -17 : -1;
						ImGui::MyColorEdit3("##arms2gloss", (float*)&config->Visuals.arms2.gloss, check_transparency(config->Visuals.arms2.material_type) ? 1 << 7 | ImGuiColorEditFlags_Alpha2 : 1 << 7);
					}
				}
				ImGui::Dummy(ImVec2(0, 6));
			}

			ImGui::Checkbox("retain model##w2_retainmodel", &config->Visuals.arms2.preserve_model);
			ImGui::Checkbox("pulse##w2_pulse", &config->Visuals.arms2.pulse);
			ImGui::Checkbox("wireframe##w2_wireframe", &config->Visuals.arms2.wireframe);
		}
	} break;// arms
	case 2: {
		ImGui::Combo("layer", &config->Visuals.sleeve.chams_option, chamslayer, IM_ARRAYSIZE(chamslayer));
		if (config->Visuals.sleeve.chams_option == 0) {
			ImGui::Combo("base material##weaasdaponopt", &config->Visuals.sleeve.material_type, materials, ARRAYSIZE(materials));
			ImGui::Text(""); ImGui::Dummy(ImVec2(0, 6));
			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 15, ImGui::GetCursorPosY() - 15));
			ImGui::Text("color"); {
				if (config->Visuals.sleeve.material_type) {
					if (check_material_type2(config->Visuals.sleeve.material_type)) {
						ImGui::SameLine(); style.ColorOverride.x = -1;
						ImGui::MyColorEdit3("##sleevecolor", (float*)&config->Visuals.sleeve.color, 1 << 7 | ImGuiColorEditFlags_Alpha2);
					}
					if (check_material_type(config->Visuals.sleeve.material_type)) {
						ImGui::SameLine(); style.ColorOverride.x = check_material_type2(config->Visuals.sleeve.material_type) ? -17 : -1;
						ImGui::MyColorEdit3("##sleevegloss", (float*)&config->Visuals.sleeve.gloss, check_transparency(config->Visuals.sleeve.material_type) ? 1 << 7 | ImGuiColorEditFlags_Alpha2 : 1 << 7);
					}
				}
				ImGui::Dummy(ImVec2(0, 6));
			}
			ImGui::Checkbox("retain model##w_retainmodel", &config->Visuals.sleeve.preserve_model);
			ImGui::Checkbox("pulse##w_pulse", &config->Visuals.sleeve.pulse);
			ImGui::Checkbox("wireframe##w_wireframe", &config->Visuals.sleeve.wireframe);
		}
		if (config->Visuals.sleeve.chams_option == 1) {
			ImGui::Combo("underlay material##wesdaaponopt", &config->Visuals.sleeve2.material_type, materials, ARRAYSIZE(materials));
			ImGui::Text(""); ImGui::Dummy(ImVec2(0, 6));
			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 15, ImGui::GetCursorPosY() - 15));
			ImGui::Text("color");
			{
				if (config->Visuals.sleeve2.material_type) {
					if (check_material_type2(config->Visuals.sleeve2.material_type)) {
						ImGui::SameLine(); style.ColorOverride.x = -1;
						ImGui::MyColorEdit3("##sleeve2color", (float*)&config->Visuals.sleeve2.color, 1 << 7 | ImGuiColorEditFlags_Alpha2);
					}
					if (check_material_type(config->Visuals.sleeve2.material_type)) {
						ImGui::SameLine(); style.ColorOverride.x = check_material_type2(config->Visuals.sleeve2.material_type) ? -17 : -1;
						ImGui::MyColorEdit3("##sleeve2gloss", (float*)&config->Visuals.sleeve2.gloss, check_transparency(config->Visuals.sleeve2.material_type) ? 1 << 7 | ImGuiColorEditFlags_Alpha2 : 1 << 7);
					}
				}
				ImGui::Dummy(ImVec2(0, 6));
			}

			ImGui::Checkbox("retain model##w2_retainmodel", &config->Visuals.sleeve2.preserve_model);
			ImGui::Checkbox("pulse##w2_pulse", &config->Visuals.sleeve2.pulse);
			ImGui::Checkbox("wireframe##w2_wireframe", &config->Visuals.sleeve2.wireframe);
		}
	} break;// sleeves
	case 3: {
		ImGui::Combo("layer", &config->Visuals.enemy.chams_option, chamslayer, IM_ARRAYSIZE(chamslayer));
		if (config->Visuals.enemy.chams_option == 0) {
			ImGui::Combo("base material##weaasdaponopt", &config->Visuals.enemy.material_type, materials, ARRAYSIZE(materials));
			ImGui::Text(""); ImGui::Dummy(ImVec2(0, 6));
			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 15, ImGui::GetCursorPosY() - 15));
			ImGui::Text("color"); {
				if (config->Visuals.enemy.material_type) {
					if (check_material_type2(config->Visuals.enemy.material_type)) {
						ImGui::SameLine(); style.ColorOverride.x = -1;
						ImGui::MyColorEdit3("##enemycolor", (float*)&config->Visuals.enemy.color, 1 << 7 | ImGuiColorEditFlags_Alpha2);
					}
					if (check_material_type(config->Visuals.enemy.material_type)) {
						ImGui::SameLine(); style.ColorOverride.x = check_material_type2(config->Visuals.enemy.material_type) ? -17 : -1;
						ImGui::MyColorEdit3("##enemygloss", (float*)&config->Visuals.enemy.gloss, check_transparency(config->Visuals.enemy.material_type) ? 1 << 7 | ImGuiColorEditFlags_Alpha2 : 1 << 7);
					}
				}
				ImGui::Dummy(ImVec2(0, 6));
			}
			ImGui::Checkbox("retain model##w_retainmodel", &config->Visuals.enemy.preserve_model);
			ImGui::Checkbox("pulse##w_pulse", &config->Visuals.enemy.pulse);
			ImGui::Checkbox("wireframe##w_wireframe", &config->Visuals.enemy.wireframe);
			ImGui::Checkbox("avoid wall##enemy_avoid", &config->Visuals.enemy.xqz, 2);
			if (check_material_type2(config->Visuals.enemy.material_type)) { ImGui::SameLine(); style.ColorOverride.x = -1; ImGui::MyColorEdit3(("##enemyxyzcolor"), (float*)&config->Visuals.enemy.xqz_color, 1 << 7 | ImGuiColorEditFlags_Alpha2); } if (check_material_type(config->Visuals.enemy.material_type)) { ImGui::SameLine(); style.ColorOverride.x = -17; ImGui::MyColorEdit3(("##enemyxyzngloss"), (float*)&config->Visuals.enemy.xqz_gloss, 1 << 7 | ImGuiColorEditFlags_Alpha2); }

		}
		if (config->Visuals.enemy.chams_option == 1) {
			ImGui::Combo("underlay material##wesdaaponopt", &config->Visuals.enemy2.material_type, materials, ARRAYSIZE(materials));
			ImGui::Text(""); ImGui::Dummy(ImVec2(0, 6));
			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 15, ImGui::GetCursorPosY() - 15));
			ImGui::Text("color");
			{
				if (config->Visuals.enemy2.material_type) {
					if (check_material_type2(config->Visuals.enemy2.material_type)) {
						ImGui::SameLine(); style.ColorOverride.x = -1;
						ImGui::MyColorEdit3("##enemy2color", (float*)&config->Visuals.enemy2.color, 1 << 7 | ImGuiColorEditFlags_Alpha2);
					}
					if (check_material_type(config->Visuals.enemy2.material_type)) {
						ImGui::SameLine(); style.ColorOverride.x = check_material_type2(config->Visuals.enemy2.material_type) ? -17 : -1;
						ImGui::MyColorEdit3("##enemy2gloss", (float*)&config->Visuals.enemy2.gloss, check_transparency(config->Visuals.enemy2.material_type) ? 1 << 7 | ImGuiColorEditFlags_Alpha2 : 1 << 7);
					}
				}
				ImGui::Dummy(ImVec2(0, 6));
			}

			ImGui::Checkbox("retain model##w2_retainmodel", &config->Visuals.enemy2.preserve_model);
			ImGui::Checkbox("pulse##w2_pulse", &config->Visuals.enemy2.pulse);
			ImGui::Checkbox("wireframe##w2_wireframe", &config->Visuals.enemy2.wireframe);
			ImGui::Checkbox("avoid wall##enemy2_avoid", &config->Visuals.enemy2.xqz, 2);
			if (check_material_type2(config->Visuals.enemy2.material_type)) { ImGui::SameLine(); style.ColorOverride.x = -1; ImGui::MyColorEdit3(("##enemy2xyzcolor"), (float*)&config->Visuals.enemy2.xqz_color, 1 << 7 | ImGuiColorEditFlags_Alpha2); } if (check_material_type(config->Visuals.enemy2.material_type)) { ImGui::SameLine(); style.ColorOverride.x = -17; ImGui::MyColorEdit3(("##enemy2xyzngloss"), (float*)&config->Visuals.enemy2.xqz_gloss, 1 << 7 | ImGuiColorEditFlags_Alpha2); }

		}

		ImGui::Checkbox("enable glow", &config->Visuals.glowenable, true);
		ImGui::SameLine(); ImGui::MyColorEdit3(("##enable_glowcolor"), (float*)&config->Colors.glow_col, 1 << 7 | ImGuiColorEditFlags_Alpha2);
		ImGui::Combo("glow style", &config->Visuals.glowstyle, glowstyles, ARRAYSIZE(glowstyles));
	} break;// enemy
	case 4: {
		ImGui::Combo("layer", &config->Visuals.team.chams_option, chamslayer, IM_ARRAYSIZE(chamslayer));
		if (config->Visuals.team.chams_option == 0) {
			ImGui::Combo("base material##weaasdaponopt", &config->Visuals.team.material_type, materials, ARRAYSIZE(materials));
			ImGui::Text(""); ImGui::Dummy(ImVec2(0, 6));
			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 15, ImGui::GetCursorPosY() - 15));
			ImGui::Text("color"); {
				if (config->Visuals.team.material_type) {
					if (check_material_type2(config->Visuals.team.material_type)) {
						ImGui::SameLine(); style.ColorOverride.x = -1;
						ImGui::MyColorEdit3("##teamcolor", (float*)&config->Visuals.team.color, 1 << 7 | ImGuiColorEditFlags_Alpha2);
					}
					if (check_material_type(config->Visuals.team.material_type)) {
						ImGui::SameLine(); style.ColorOverride.x = check_material_type2(config->Visuals.team.material_type) ? -17 : -1;
						ImGui::MyColorEdit3("##teamgloss", (float*)&config->Visuals.team.gloss, check_transparency(config->Visuals.team.material_type) ? 1 << 7 | ImGuiColorEditFlags_Alpha2 : 1 << 7);
					}
				}
				ImGui::Dummy(ImVec2(0, 6));
			}
			ImGui::Checkbox("retain model##w_retainmodel", &config->Visuals.team.preserve_model);
			ImGui::Checkbox("pulse##w_pulse", &config->Visuals.team.pulse);
			ImGui::Checkbox("wireframe##w_wireframe", &config->Visuals.team.wireframe);
			ImGui::Checkbox("avoid wall##team_avoid", &config->Visuals.team.xqz, 2);
			if (check_material_type2(config->Visuals.team.material_type)) { ImGui::SameLine(); style.ColorOverride.x = -1; ImGui::MyColorEdit3(("##teamxyzcolor"), (float*)&config->Visuals.team.xqz_color, 1 << 7 | ImGuiColorEditFlags_Alpha2); } if (check_material_type(config->Visuals.team.material_type)) { ImGui::SameLine(); style.ColorOverride.x = -17; ImGui::MyColorEdit3(("##teamxyzngloss"), (float*)&config->Visuals.team.xqz_gloss, 1 << 7 | ImGuiColorEditFlags_Alpha2); }

		}
		if (config->Visuals.team.chams_option == 1) {
			ImGui::Combo("underlay material##wesdaaponopt", &config->Visuals.team2.material_type, materials, ARRAYSIZE(materials));
			ImGui::Text(""); ImGui::Dummy(ImVec2(0, 6));
			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 15, ImGui::GetCursorPosY() - 15));
			ImGui::Text("color");
			{
				if (config->Visuals.team2.material_type) {
					if (check_material_type2(config->Visuals.team2.material_type)) {
						ImGui::SameLine(); style.ColorOverride.x = -1;
						ImGui::MyColorEdit3("##team2color", (float*)&config->Visuals.team2.color, 1 << 7 | ImGuiColorEditFlags_Alpha2);
					}
					if (check_material_type(config->Visuals.team2.material_type)) {
						ImGui::SameLine(); style.ColorOverride.x = check_material_type2(config->Visuals.team2.material_type) ? -17 : -1;
						ImGui::MyColorEdit3("##team2gloss", (float*)&config->Visuals.team2.gloss, check_transparency(config->Visuals.team2.material_type) ? 1 << 7 | ImGuiColorEditFlags_Alpha2 : 1 << 7);
					}
				}
				ImGui::Dummy(ImVec2(0, 6));
			}

			ImGui::Checkbox("retain model##w2_retainmodel", &config->Visuals.team2.preserve_model);
			ImGui::Checkbox("pulse##w2_pulse", &config->Visuals.team2.pulse);
			ImGui::Checkbox("wireframe##w2_wireframe", &config->Visuals.team2.wireframe);
			ImGui::Checkbox("avoid wall##team2_avoid", &config->Visuals.team2.xqz, 2);
			if (check_material_type2(config->Visuals.team2.material_type)) { ImGui::SameLine(); style.ColorOverride.x = -1; ImGui::MyColorEdit3(("##team2xyzcolor"), (float*)&config->Visuals.team2.xqz_color, 1 << 7 | ImGuiColorEditFlags_Alpha2); } if (check_material_type(config->Visuals.team2.material_type)) { ImGui::SameLine(); style.ColorOverride.x = -17; ImGui::MyColorEdit3(("##team2xyzngloss"), (float*)&config->Visuals.team2.xqz_gloss, 1 << 7 | ImGuiColorEditFlags_Alpha2); }

		}
	} break;// team
	case 5: {
		ImGui::Combo("layer", &config->Visuals.local.chams_option, chamslayer, IM_ARRAYSIZE(chamslayer));
		if (config->Visuals.local.chams_option == 0) {
			ImGui::Combo("base material##weaasdaponopt", &config->Visuals.local.material_type, materials, ARRAYSIZE(materials));
			ImGui::Text(""); ImGui::Dummy(ImVec2(0, 6));
			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 15, ImGui::GetCursorPosY() - 15));
			ImGui::Text("color"); {
				if (config->Visuals.local.material_type) {
					if (check_material_type2(config->Visuals.local.material_type)) {
						ImGui::SameLine(); style.ColorOverride.x = -1;
						ImGui::MyColorEdit3("##localcolor", (float*)&config->Visuals.local.color, 1 << 7 | ImGuiColorEditFlags_Alpha2);
					}
					if (check_material_type(config->Visuals.local.material_type)) {
						ImGui::SameLine(); style.ColorOverride.x = check_material_type2(config->Visuals.local.material_type) ? -17 : -1;
						ImGui::MyColorEdit3("##localgloss", (float*)&config->Visuals.local.gloss, check_transparency(config->Visuals.local.material_type) ? 1 << 7 | ImGuiColorEditFlags_Alpha2 : 1 << 7);
					}
				}
				ImGui::Dummy(ImVec2(0, 6));
			}
			ImGui::Checkbox("retain model##w_retainmodel", &config->Visuals.local.preserve_model);
			ImGui::Checkbox("pulse##w_pulse", &config->Visuals.local.pulse);
			ImGui::Checkbox("wireframe##w_wireframe", &config->Visuals.local.wireframe);
			ImGui::Checkbox("avoid wall##local_avoid", &config->Visuals.local.xqz, 2);
			if (check_material_type2(config->Visuals.local.material_type)) { ImGui::SameLine(); style.ColorOverride.x = -1; ImGui::MyColorEdit3(("##localxyzcolor"), (float*)&config->Visuals.local.xqz_color, 1 << 7 | ImGuiColorEditFlags_Alpha2); } if (check_material_type(config->Visuals.local.material_type)) { ImGui::SameLine(); style.ColorOverride.x = -17; ImGui::MyColorEdit3(("##localxyzngloss"), (float*)&config->Visuals.local.xqz_gloss, 1 << 7 | ImGuiColorEditFlags_Alpha2); }

		}
		if (config->Visuals.local.chams_option == 1) {
			ImGui::Combo("underlay material##wesdaaponopt", &config->Visuals.local2.material_type, materials, ARRAYSIZE(materials));
			ImGui::Text(""); ImGui::Dummy(ImVec2(0, 6));
			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 15, ImGui::GetCursorPosY() - 15));
			ImGui::Text("color");
			{
				if (config->Visuals.local2.material_type) {
					if (check_material_type2(config->Visuals.local2.material_type)) {
						ImGui::SameLine(); style.ColorOverride.x = -1;
						ImGui::MyColorEdit3("##local2color", (float*)&config->Visuals.local2.color, 1 << 7 | ImGuiColorEditFlags_Alpha2);
					}
					if (check_material_type(config->Visuals.local2.material_type)) {
						ImGui::SameLine(); style.ColorOverride.x = check_material_type2(config->Visuals.local2.material_type) ? -17 : -1;
						ImGui::MyColorEdit3("##local2gloss", (float*)&config->Visuals.local2.gloss, check_transparency(config->Visuals.local2.material_type) ? 1 << 7 | ImGuiColorEditFlags_Alpha2 : 1 << 7);
					}
				}
				ImGui::Dummy(ImVec2(0, 6));
			}

			ImGui::Checkbox("retain model##w2_retainmodel", &config->Visuals.local2.preserve_model);
			ImGui::Checkbox("pulse##w2_pulse", &config->Visuals.local2.pulse);
			ImGui::Checkbox("wireframe##w2_wireframe", &config->Visuals.local2.wireframe);
			ImGui::Checkbox("avoid wall##local2_avoid", &config->Visuals.local2.xqz, 2);
			if (check_material_type2(config->Visuals.local2.material_type)) { ImGui::SameLine(); style.ColorOverride.x = -1; ImGui::MyColorEdit3(("##local2xyzcolor"), (float*)&config->Visuals.local2.xqz_color, 1 << 7 | ImGuiColorEditFlags_Alpha2); } if (check_material_type(config->Visuals.local2.material_type)) { ImGui::SameLine(); style.ColorOverride.x = -17; ImGui::MyColorEdit3(("##local2xyzngloss"), (float*)&config->Visuals.local2.xqz_gloss, 1 << 7 | ImGuiColorEditFlags_Alpha2); }

		}

		ImGui::Checkbox("enable glow##enable_glow", &config->Visuals.localglowenable, true);
		ImGui::SameLine(); ImGui::MyColorEdit3(("##enable_glowcolor"), (float*)&config->Colors.localglow_col, 1 << 7 | ImGuiColorEditFlags_Alpha2);
		ImGui::Combo("glow style##localfgopt", &config->Visuals.localglowstyle, glowstyles, ARRAYSIZE(glowstyles));
	} break;// local
	case 6: {
		ImGui::Combo("layer", &config->Visuals.desync.chams_option, chamslayer, IM_ARRAYSIZE(chamslayer));
		if (config->Visuals.desync.chams_option == 0) {
			ImGui::Combo("base material##weaasdaponopt", &config->Visuals.desync.material_type, materials, ARRAYSIZE(materials));
			ImGui::Text(""); ImGui::Dummy(ImVec2(0, 6));
			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 15, ImGui::GetCursorPosY() - 15));
			ImGui::Text("color"); {
				if (config->Visuals.desync.material_type) {
					if (check_material_type2(config->Visuals.desync.material_type)) {
						ImGui::SameLine(); style.ColorOverride.x = -1;
						ImGui::MyColorEdit3("##desynccolor", (float*)&config->Visuals.desync.color, 1 << 7 | ImGuiColorEditFlags_Alpha2);
					}
					if (check_material_type(config->Visuals.desync.material_type)) {
						ImGui::SameLine(); style.ColorOverride.x = check_material_type2(config->Visuals.desync.material_type) ? -17 : -1;
						ImGui::MyColorEdit3("##desyncgloss", (float*)&config->Visuals.desync.gloss, check_transparency(config->Visuals.desync.material_type) ? 1 << 7 | ImGuiColorEditFlags_Alpha2 : 1 << 7);
					}
				}
				ImGui::Dummy(ImVec2(0, 6));
			}
			ImGui::Checkbox("retain model##w_retainmodel", &config->Visuals.desync.preserve_model);
			ImGui::Checkbox("pulse##w_pulse", &config->Visuals.desync.pulse);
			ImGui::Checkbox("wireframe##w_wireframe", &config->Visuals.desync.wireframe);
		}
		if (config->Visuals.desync.chams_option == 1) {
			ImGui::Combo("underlay material##wesdaaponopt", &config->Visuals.desync2.material_type, materials, ARRAYSIZE(materials));
			ImGui::Text(""); ImGui::Dummy(ImVec2(0, 6));
			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 15, ImGui::GetCursorPosY() - 15));
			ImGui::Text("color");
			{
				if (config->Visuals.desync2.material_type) {
					if (check_material_type2(config->Visuals.desync2.material_type)) {
						ImGui::SameLine(); style.ColorOverride.x = -1;
						ImGui::MyColorEdit3("##desync2color", (float*)&config->Visuals.desync2.color, 1 << 7 | ImGuiColorEditFlags_Alpha2);
					}
					if (check_material_type(config->Visuals.desync2.material_type)) {
						ImGui::SameLine(); style.ColorOverride.x = check_material_type2(config->Visuals.desync2.material_type) ? -17 : -1;
						ImGui::MyColorEdit3("##desync2gloss", (float*)&config->Visuals.desync2.gloss, check_transparency(config->Visuals.desync2.material_type) ? 1 << 7 | ImGuiColorEditFlags_Alpha2 : 1 << 7);
					}
				}
				ImGui::Dummy(ImVec2(0, 6));
			}

			ImGui::Checkbox("retain model##w2_retainmodel", &config->Visuals.desync2.preserve_model);
			ImGui::Checkbox("pulse##w2_pulse", &config->Visuals.desync2.pulse);
			ImGui::Checkbox("wireframe##w2_wireframe", &config->Visuals.desync2.wireframe);
		}
	} break;// desync
	case 7: {
		ImGui::Combo("layer", &config->Visuals.backtrack.chams_option, chamslayer, IM_ARRAYSIZE(chamslayer));
		if (config->Visuals.backtrack.chams_option == 0) {
			ImGui::Combo("base material##weaasdaponopt", &config->Visuals.backtrack.material_type, materials, ARRAYSIZE(materials));
			ImGui::Text(""); ImGui::Dummy(ImVec2(0, 6));
			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 15, ImGui::GetCursorPosY() - 15));
			ImGui::Text("color"); {
				if (config->Visuals.backtrack.material_type) {
					if (check_material_type2(config->Visuals.backtrack.material_type)) {
						ImGui::SameLine(); style.ColorOverride.x = -1;
						ImGui::MyColorEdit3("##backtrackcolor", (float*)&config->Visuals.backtrack.color, 1 << 7 | ImGuiColorEditFlags_Alpha2);
					}
					if (check_material_type(config->Visuals.backtrack.material_type)) {
						ImGui::SameLine(); style.ColorOverride.x = check_material_type2(config->Visuals.backtrack.material_type) ? -17 : -1;
						ImGui::MyColorEdit3("##backtrackgloss", (float*)&config->Visuals.backtrack.gloss, check_transparency(config->Visuals.backtrack.material_type) ? 1 << 7 | ImGuiColorEditFlags_Alpha2 : 1 << 7);
					}
				}
				ImGui::Dummy(ImVec2(0, 6));
			}
			ImGui::Checkbox("retain model##w_retainmodel", &config->Visuals.backtrack.preserve_model);
			ImGui::Checkbox("pulse##w_pulse", &config->Visuals.backtrack.pulse);
			ImGui::Checkbox("wireframe##w_wireframe", &config->Visuals.backtrack.wireframe);
			ImGui::Checkbox("avoid wall##backtrack_avoid", &config->Visuals.backtrack.xqz, 2);
			if (check_material_type2(config->Visuals.backtrack.material_type)) { ImGui::SameLine(); style.ColorOverride.x = -1; ImGui::MyColorEdit3(("##backtrackxyzcolor"), (float*)&config->Visuals.backtrack.xqz_color, 1 << 7 | ImGuiColorEditFlags_Alpha2); } if (check_material_type(config->Visuals.backtrack.material_type)) { ImGui::SameLine(); style.ColorOverride.x = -17; ImGui::MyColorEdit3(("##backtrackxyzngloss"), (float*)&config->Visuals.backtrack.xqz_gloss, 1 << 7 | ImGuiColorEditFlags_Alpha2); }

		}
	} break;// backtrack
	case 8: {
		ImGui::Combo("layer", &config->Visuals.backtrack.chams_option, chamslayer, IM_ARRAYSIZE(chamslayer));
		if (config->Visuals.backtrack.chams_option == 0) {
			ImGui::Combo("base material##weaasdaponopt", &config->Visuals.backtrack.material_type, materials, ARRAYSIZE(materials));
			ImGui::Text(""); ImGui::Dummy(ImVec2(0, 6));
			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 15, ImGui::GetCursorPosY() - 15));
			ImGui::Text("color"); {
				if (config->Visuals.backtrack.material_type) {
					if (check_material_type2(config->Visuals.backtrack.material_type)) {
						ImGui::SameLine(); style.ColorOverride.x = -1;
						ImGui::MyColorEdit3("##backtrackcolor", (float*)&config->Visuals.backtrack.color, 1 << 7 | ImGuiColorEditFlags_Alpha2);
					}
					if (check_material_type(config->Visuals.backtrack.material_type)) {
						ImGui::SameLine(); style.ColorOverride.x = check_material_type2(config->Visuals.backtrack.material_type) ? -17 : -1;
						ImGui::MyColorEdit3("##backtrackgloss", (float*)&config->Visuals.backtrack.gloss, check_transparency(config->Visuals.backtrack.material_type) ? 1 << 7 | ImGuiColorEditFlags_Alpha2 : 1 << 7);
					}
				}
				ImGui::Dummy(ImVec2(0, 6));
			}
			ImGui::Checkbox("retain model##w_retainmodel", &config->Visuals.backtrack.preserve_model);
			ImGui::Checkbox("pulse##w_pulse", &config->Visuals.backtrack.pulse);
			ImGui::Checkbox("wireframe##w_wireframe", &config->Visuals.backtrack.wireframe);
			ImGui::Checkbox("avoid wall##backtrack_avoid", &config->Visuals.backtrack.xqz, 2);
			if (check_material_type2(config->Visuals.backtrack.material_type)) { ImGui::SameLine(); style.ColorOverride.x = -1; ImGui::MyColorEdit3(("##backtrackxyzcolor"), (float*)&config->Visuals.backtrack.xqz_color, 1 << 7 | ImGuiColorEditFlags_Alpha2); } if (check_material_type(config->Visuals.backtrack.material_type)) { ImGui::SameLine(); style.ColorOverride.x = -17; ImGui::MyColorEdit3(("##backtrackxyzngloss"), (float*)&config->Visuals.backtrack.xqz_gloss, 1 << 7 | ImGuiColorEditFlags_Alpha2); }

		}
	} break;// backtrack
	}
	
}
void main_aimbot() {
	
	const char* resolver[] = { "none","normal", "beta" };
	const char* priority[] = { "damage", "distance","crosshair" };
	const char* weapons[] = { "pistol", "auto", "scout", "awp", "heavy", "other","taser","" };

	ImGui::Checkbox("enable##rageautofire", &config->Ragebot.enable);
	ImGui::Combo("priority", &config->Ragebot.priority, priority, ARRAYSIZE(priority)); 
	ImGui::Text("  ");

	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);

	if (ImGui::BeginCombo("automatization", "...", ImVec2(0, 255))) {
		const auto cap = 62;
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
		ImGui::Selectable(("scope"), &config->Ragebot.autoscope, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
		ImGui::Selectable(("fire"), &config->Ragebot.autofire, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
		ImGui::Selectable(("stop"), &config->Ragebot.autostop, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
		ImGui::Selectable(("knife"), &config->Ragebot.autoknife, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
		ImGui::Selectable(("zeus"), &config->Ragebot.autozeus, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
		ImGui::Selectable(("revolver"), &config->Ragebot.autorevolver, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
		ImGui::Selectable(("recoil control"), &config->Ragebot.no_recoil, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
		ImGui::EndCombo();
	}
	ImGui::Checkbox("resolver", &config->Ragebot.resolver);
	ImGui::Combo("weapon##weapon_select", &config->Ragebot.weaponconfig, weapons, ARRAYSIZE(weapons));

	switch (config->Ragebot.weaponconfig) {
		case 0: {
			ImGui::Text("  ");
			ImGui::SliderInt("hitchance##hitchance", &config->Ragebot.pistol.hitchance, 0.f, 100.f, ("%0.f"));
			ImGui::SliderInt("mininum damage visible", &config->Ragebot.pistol.min_dmg_vis, 0.f, 100.f, ("%0.f"));
			ImGui::SliderInt("mininum damage hidden", &config->Ragebot.pistol.min_dmg, 0.f, 100.f, ("%0.f"));
			
		} break;
		case 1: {
			ImGui::Text("  ");
			ImGui::SliderInt("hitchance##hitchance", &config->Ragebot._auto.hitchance, 0.f, 100.f, ("%0.f"));
			ImGui::SliderInt("mininum damage visible", &config->Ragebot._auto.min_dmg_vis, 0.f, 100.f, ("%0.f"));
			ImGui::SliderInt("mininum damage hidden", &config->Ragebot._auto.min_dmg, 0.f, 100.f, ("%0.f")); ImGui::Dummy(ImVec2(2, 0)); ImGui::SameLine();
			
		} break;
		case 2: {
			ImGui::Text("  ");
			ImGui::SliderInt("hitchance##hitchance", &config->Ragebot.scout.hitchance, 0.f, 100.f, ("%0.f"));
			ImGui::SliderInt("mininum damage visible", &config->Ragebot.scout.min_dmg_vis, 0.f, 100.f, ("%0.f"));
			ImGui::SliderInt("mininum damage hidden", &config->Ragebot.scout.min_dmg, 0.f, 100.f, ("%0.f")); ImGui::Dummy(ImVec2(2, 0)); ImGui::SameLine();
			
		} break;
		case 3: {
			ImGui::Text("  ");
			ImGui::SliderInt("hitchance##hitchance", &config->Ragebot.awp.hitchance, 0.f, 100.f, ("%0.f"));
			ImGui::SliderInt("mininum damage visible", &config->Ragebot.awp.min_dmg_vis, 0.f, 100.f, ("%0.f"));
			ImGui::SliderInt("mininum damage hidden", &config->Ragebot.awp.min_dmg, 0.f, 100.f, ("%0.f")); ImGui::Dummy(ImVec2(1, 0)); ImGui::SameLine();
			
		} break;
		case 4: {
			ImGui::Text("  ");
			ImGui::SliderInt("hitchance##hitchance", &config->Ragebot.heavy.hitchance, 0.f, 100.f, ("%0.f"));
			ImGui::SliderInt("mininum damage visible", &config->Ragebot.heavy.min_dmg_vis, 0.f, 100.f, ("%0.f"));
			ImGui::SliderInt("mininum damage hidden", &config->Ragebot.heavy.min_dmg, 0.f, 100.f, ("%0.f")); ImGui::Dummy(ImVec2(1, 0)); ImGui::SameLine();
		
		} break;
		case 5: {
			ImGui::Text("  ");
			ImGui::SliderInt("hitchance##hitchance", &config->Ragebot.other.hitchance, 0.f, 100.f, ("%0.f"));
			ImGui::SliderInt("mininum damage visible", &config->Ragebot.other.min_dmg_vis, 0.f, 100.f, ("%0.f"));
			ImGui::SliderInt("mininum damage hidden", &config->Ragebot.other.min_dmg, 0.f, 100.f, ("%0.f")); ImGui::Dummy(ImVec2(1, 0)); ImGui::SameLine();
		
		} break;
		case 6: {
			ImGui::Text("  ");
			ImGui::SliderInt("hitchance##hitchance", &config->Ragebot.taser.hitchance, 0.f, 100.f, ("%0.f"));
			ImGui::SliderInt("mininum damage", &config->Ragebot.taser.min_dmg_vis, 0.f, 100.f, ("%0.f"));
		} break;
	}
	ImGui::Text("  ");
}
void hitscan_aimbot() {

	const char* weapons[] = { "pistol", "auto", "scout", "awp", "heavy", "other","taser","" };

	ImGui::Combo("weapon##weapon_select", &config->Ragebot.weaponconfig, weapons, ARRAYSIZE(weapons));

	switch (config->Ragebot.weaponconfig) {
		case 0: {
			ImGui::Text("");

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);

			if (ImGui::BeginCombo("hitscan hitboxes", "...", ImVec2(0, 255))) {
				const auto cap = 62;
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
				ImGui::Selectable(("head"), &config->Ragebot.pistol.hitscan_head, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
				ImGui::Selectable(("chest"), &config->Ragebot.pistol.hitscan_chest, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
				ImGui::Selectable(("stomach"), &config->Ragebot.pistol.hitscan_stomach, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
				ImGui::Selectable(("pelvis"), &config->Ragebot.pistol.hitscan_pelvis, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
				ImGui::Selectable(("legs"), &config->Ragebot.pistol.hitscan_legs, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
				ImGui::Selectable(("feet"), &config->Ragebot.pistol.hitscan_feet, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
				ImGui::EndCombo();
			}

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);

			if (ImGui::BeginCombo("multipoint hitboxes", "...", ImVec2(0, 255))) {
				const auto cap = 62;
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
				ImGui::Selectable(("head"), &config->Ragebot.pistol.multipoint_head, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
				ImGui::Selectable(("chest"), &config->Ragebot.pistol.multipoint_chest, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
				ImGui::Selectable(("stomach"), &config->Ragebot.pistol.multipoint_stomach, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
				ImGui::Selectable(("pelvis"), &config->Ragebot.pistol.multipoint_pelvis, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
				ImGui::Selectable(("legs"), &config->Ragebot.pistol.multipoint_legs, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
				ImGui::Selectable(("feet"), &config->Ragebot.pistol.multipoint_feet, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
				ImGui::EndCombo();
			}

			if (config->Ragebot.pistol.multipoint_head) 
				ImGui::SliderInt("head scale", &config->Ragebot.pistol.head_scale, 0.f, 100.f, ("%0.f"));
			if (config->Ragebot.pistol.multipoint_chest)
				ImGui::SliderInt("chest scale", &config->Ragebot.pistol.chest_scale, 0.f, 100.f, ("%0.f"));
			if (config->Ragebot.pistol.multipoint_stomach)
				ImGui::SliderInt("stomach scale", &config->Ragebot.pistol.stomach_scale, 0.f, 100.f, ("%0.f"));
			if (config->Ragebot.pistol.multipoint_pelvis)
				ImGui::SliderInt("pelvis scale", &config->Ragebot.pistol.pelvis_scale, 0.f, 100.f, ("%0.f"));
			if (config->Ragebot.pistol.multipoint_legs)
				ImGui::SliderInt("legs scale", &config->Ragebot.pistol.legs_scale, 0.f, 100.f, ("%0.f"));
			if (config->Ragebot.pistol.multipoint_feet)
				ImGui::SliderInt("feet scale", &config->Ragebot.pistol.feet_scale, 0.f, 100.f, ("%0.f"));

			ImGui::Checkbox("static point scale", &config->Ragebot.pistol.static_point);
			ImGui::Checkbox("safe point override ", &config->Ragebot.pistol.safe_point_override_enable); 
			if (config->Ragebot.pistol.safe_point_override_enable){
				ImGui::SameLine();
				ImGui::Hotkey("##save_point_override_key", &config->Ragebot.pistol.save_point_override, ImVec2(50, 40));
			}
		} break;
		case 1: { 
			ImGui::Text("");
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);

			if (ImGui::BeginCombo("hitscan hitboxes", "...", ImVec2(0, 255))) {
				const auto cap = 62;
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
				ImGui::Selectable(("head"), &config->Ragebot._auto.hitscan_head, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
				ImGui::Selectable(("chest"), &config->Ragebot._auto.hitscan_chest, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
				ImGui::Selectable(("stomach"), &config->Ragebot._auto.hitscan_stomach, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
				ImGui::Selectable(("pelvis"), &config->Ragebot._auto.hitscan_pelvis, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
				ImGui::Selectable(("legs"), &config->Ragebot._auto.hitscan_legs, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
				ImGui::Selectable(("feet"), &config->Ragebot._auto.hitscan_feet, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
				ImGui::EndCombo();
			}

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);

			if (ImGui::BeginCombo("multipoint hitboxes", "...", ImVec2(0, 255))) {
				const auto cap = 62;
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
				ImGui::Selectable(("head"), &config->Ragebot._auto.multipoint_head, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
				ImGui::Selectable(("chest"), &config->Ragebot._auto.multipoint_chest, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
				ImGui::Selectable(("stomach"), &config->Ragebot._auto.multipoint_stomach, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
				ImGui::Selectable(("pelvis"), &config->Ragebot._auto.multipoint_pelvis, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
				ImGui::Selectable(("legs"), &config->Ragebot._auto.multipoint_legs, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
				ImGui::Selectable(("feet"), &config->Ragebot._auto.multipoint_feet, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
				ImGui::EndCombo();
			}

			if (config->Ragebot._auto.multipoint_head)
				ImGui::SliderInt("head scale", &config->Ragebot._auto.head_scale, 0.f, 100.f, ("%0.f"));
			if (config->Ragebot._auto.multipoint_chest)
				ImGui::SliderInt("chest scale", &config->Ragebot._auto.chest_scale, 0.f, 100.f, ("%0.f"));
			if (config->Ragebot._auto.multipoint_stomach)
				ImGui::SliderInt("stomach scale", &config->Ragebot._auto.stomach_scale, 0.f, 100.f, ("%0.f"));
			if (config->Ragebot._auto.multipoint_pelvis)
				ImGui::SliderInt("pelvis scale", &config->Ragebot._auto.pelvis_scale, 0.f, 100.f, ("%0.f"));
			if (config->Ragebot._auto.multipoint_legs)
				ImGui::SliderInt("legs scale", &config->Ragebot._auto.legs_scale, 0.f, 100.f, ("%0.f"));
			if (config->Ragebot._auto.multipoint_feet)
				ImGui::SliderInt("feet scale", &config->Ragebot._auto.feet_scale, 0.f, 100.f, ("%0.f"));

			ImGui::Checkbox("static point scale", &config->Ragebot._auto.static_point);
			ImGui::Checkbox("safe point override ", &config->Ragebot._auto.safe_point_override_enable); 	
			if (config->Ragebot._auto.safe_point_override_enable) {
				ImGui::SameLine();
				ImGui::Hotkey("##save_point_override_key", &config->Ragebot._auto.save_point_override, ImVec2(50, 40));
			}
		} break;
		case 2: { 
			ImGui::Text("");
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);

			if (ImGui::BeginCombo("hitscan hitboxes", "...", ImVec2(0, 255))) {
				const auto cap = 62;
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
				ImGui::Selectable(("head"), &config->Ragebot.scout.hitscan_head, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
				ImGui::Selectable(("chest"), &config->Ragebot.scout.hitscan_chest, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
				ImGui::Selectable(("stomach"), &config->Ragebot.scout.hitscan_stomach, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
				ImGui::Selectable(("pelvis"), &config->Ragebot.scout.hitscan_pelvis, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
				ImGui::Selectable(("legs"), &config->Ragebot.scout.hitscan_legs, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
				ImGui::Selectable(("feet"), &config->Ragebot.scout.hitscan_feet, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
				ImGui::EndCombo();
			}

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);

			if (ImGui::BeginCombo("multipoint hitboxes", "...", ImVec2(0, 255))) {
				const auto cap = 62;
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
				ImGui::Selectable(("head"), &config->Ragebot.scout.multipoint_head, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
				ImGui::Selectable(("chest"), &config->Ragebot.scout.multipoint_chest, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
				ImGui::Selectable(("stomach"), &config->Ragebot.scout.multipoint_stomach, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
				ImGui::Selectable(("pelvis"), &config->Ragebot.scout.multipoint_pelvis, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
				ImGui::Selectable(("legs"), &config->Ragebot.scout.multipoint_legs, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
				ImGui::Selectable(("feet"), &config->Ragebot.scout.multipoint_feet, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
				ImGui::EndCombo();
			}

			if (config->Ragebot.scout.multipoint_head)
				ImGui::SliderInt("head scale", &config->Ragebot.scout.head_scale, 0.f, 100.f, ("%0.f"));
			if (config->Ragebot.scout.multipoint_chest)
				ImGui::SliderInt("chest scale", &config->Ragebot.scout.chest_scale, 0.f, 100.f, ("%0.f"));
			if (config->Ragebot.scout.multipoint_stomach)
				ImGui::SliderInt("stomach scale", &config->Ragebot.scout.stomach_scale, 0.f, 100.f, ("%0.f"));
			if (config->Ragebot.scout.multipoint_pelvis)
				ImGui::SliderInt("pelvis scale", &config->Ragebot.scout.pelvis_scale, 0.f, 100.f, ("%0.f"));
			if (config->Ragebot.scout.multipoint_legs)
				ImGui::SliderInt("legs scale", &config->Ragebot.scout.legs_scale, 0.f, 100.f, ("%0.f"));
			if (config->Ragebot.scout.multipoint_feet)
				ImGui::SliderInt("feet scale", &config->Ragebot.scout.feet_scale, 0.f, 100.f, ("%0.f"));

			ImGui::Checkbox("static point scale", &config->Ragebot.scout.static_point);
			ImGui::Checkbox("safe point override ", &config->Ragebot.scout.safe_point_override_enable); 
			if (config->Ragebot.scout.safe_point_override_enable) {
				ImGui::SameLine();
				ImGui::Hotkey("##save_point_override_key", &config->Ragebot.scout.save_point_override, ImVec2(50, 40));
			}
		} break;
		case 3: {
			ImGui::Text("");
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);

			if (ImGui::BeginCombo("hitscan hitboxes", "...", ImVec2(0, 255))) {
				const auto cap = 62;
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
				ImGui::Selectable(("head"), &config->Ragebot.awp.hitscan_head, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
				ImGui::Selectable(("chest"), &config->Ragebot.awp.hitscan_chest, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
				ImGui::Selectable(("stomach"), &config->Ragebot.awp.hitscan_stomach, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
				ImGui::Selectable(("pelvis"), &config->Ragebot.awp.hitscan_pelvis, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
				ImGui::Selectable(("legs"), &config->Ragebot.awp.hitscan_legs, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
				ImGui::Selectable(("feet"), &config->Ragebot.awp.hitscan_feet, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
				ImGui::EndCombo();
			}

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);

			if (ImGui::BeginCombo("multipoint hitboxes", "...", ImVec2(0, 255))) {
				const auto cap = 62;
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
				ImGui::Selectable(("head"), &config->Ragebot.awp.multipoint_head, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
				ImGui::Selectable(("chest"), &config->Ragebot.awp.multipoint_chest, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
				ImGui::Selectable(("stomach"), &config->Ragebot.awp.multipoint_stomach, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
				ImGui::Selectable(("pelvis"), &config->Ragebot.awp.multipoint_pelvis, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
				ImGui::Selectable(("legs"), &config->Ragebot.awp.multipoint_legs, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
				ImGui::Selectable(("feet"), &config->Ragebot.awp.multipoint_feet, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
				ImGui::EndCombo();
			}

			if (config->Ragebot.awp.multipoint_head)
				ImGui::SliderInt("head scale", &config->Ragebot.awp.head_scale, 0.f, 100.f, ("%0.f"));
			if (config->Ragebot.awp.multipoint_chest)
				ImGui::SliderInt("chest scale", &config->Ragebot.awp.chest_scale, 0.f, 100.f, ("%0.f"));
			if (config->Ragebot.awp.multipoint_stomach)
				ImGui::SliderInt("stomach scale", &config->Ragebot.awp.stomach_scale, 0.f, 100.f, ("%0.f"));
			if (config->Ragebot.awp.multipoint_pelvis)
				ImGui::SliderInt("pelvis scale", &config->Ragebot.awp.pelvis_scale, 0.f, 100.f, ("%0.f"));
			if (config->Ragebot.awp.multipoint_legs)
				ImGui::SliderInt("legs scale", &config->Ragebot.awp.legs_scale, 0.f, 100.f, ("%0.f"));
			if (config->Ragebot.awp.multipoint_feet)
				ImGui::SliderInt("feet scale", &config->Ragebot.awp.feet_scale, 0.f, 100.f, ("%0.f"));

			ImGui::Checkbox("static point scale", &config->Ragebot.awp.static_point);
			ImGui::Checkbox("safe point override ", &config->Ragebot.awp.safe_point_override_enable);
			if (config->Ragebot.awp.safe_point_override_enable) {
				ImGui::SameLine();
				ImGui::Hotkey("##save_point_override_key", &config->Ragebot.awp.save_point_override, ImVec2(50, 40));
			}
		} break;
		case 4: {
			ImGui::Text("");
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);

			if (ImGui::BeginCombo("hitscan hitboxes", "...", ImVec2(0, 255))) {
				const auto cap = 62;
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
				ImGui::Selectable(("head"), &config->Ragebot.heavy.hitscan_head, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
				ImGui::Selectable(("chest"), &config->Ragebot.heavy.hitscan_chest, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
				ImGui::Selectable(("stomach"), &config->Ragebot.heavy.hitscan_stomach, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
				ImGui::Selectable(("pelvis"), &config->Ragebot.heavy.hitscan_pelvis, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
				ImGui::Selectable(("legs"), &config->Ragebot.heavy.hitscan_legs, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
				ImGui::Selectable(("feet"), &config->Ragebot.heavy.hitscan_feet, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
				ImGui::EndCombo();
			}

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);

			if (ImGui::BeginCombo("multipoint hitboxes", "...", ImVec2(0, 255))) {
				const auto cap = 62;
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
				ImGui::Selectable(("head"), &config->Ragebot.heavy.multipoint_head, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
				ImGui::Selectable(("chest"), &config->Ragebot.heavy.multipoint_chest, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
				ImGui::Selectable(("stomach"), &config->Ragebot.heavy.multipoint_stomach, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
				ImGui::Selectable(("pelvis"), &config->Ragebot.heavy.multipoint_pelvis, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
				ImGui::Selectable(("legs"), &config->Ragebot.heavy.multipoint_legs, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
				ImGui::Selectable(("feet"), &config->Ragebot.heavy.multipoint_feet, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
				ImGui::EndCombo();
			}

			if (config->Ragebot.heavy.multipoint_head)
				ImGui::SliderInt("head scale", &config->Ragebot.heavy.head_scale, 0.f, 100.f, ("%0.f"));
			if (config->Ragebot.heavy.multipoint_chest)
				ImGui::SliderInt("chest scale", &config->Ragebot.heavy.chest_scale, 0.f, 100.f, ("%0.f"));
			if (config->Ragebot.heavy.multipoint_stomach)
				ImGui::SliderInt("stomach scale", &config->Ragebot.heavy.stomach_scale, 0.f, 100.f, ("%0.f"));
			if (config->Ragebot.heavy.multipoint_pelvis)
				ImGui::SliderInt("pelvis scale", &config->Ragebot.heavy.pelvis_scale, 0.f, 100.f, ("%0.f"));
			if (config->Ragebot.heavy.multipoint_legs)
				ImGui::SliderInt("legs scale", &config->Ragebot.heavy.legs_scale, 0.f, 100.f, ("%0.f"));
			if (config->Ragebot.heavy.multipoint_feet)
				ImGui::SliderInt("feet scale", &config->Ragebot.heavy.feet_scale, 0.f, 100.f, ("%0.f"));

			ImGui::Checkbox("static point scale", &config->Ragebot.heavy.static_point);
			ImGui::Checkbox("safe point override ", &config->Ragebot.heavy.safe_point_override_enable);
			if (config->Ragebot.heavy.safe_point_override_enable) {
				ImGui::SameLine();
				ImGui::Hotkey("##save_point_override_key", &config->Ragebot.heavy.save_point_override, ImVec2(50, 40));
			}
		} break;
		case 5: { 
			ImGui::Text("");
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);

			if (ImGui::BeginCombo("hitscan hitboxes", "...", ImVec2(0, 255))) {
				const auto cap = 62;
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
				ImGui::Selectable(("head"), &config->Ragebot.other.hitscan_head, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
				ImGui::Selectable(("chest"), &config->Ragebot.other.hitscan_chest, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
				ImGui::Selectable(("stomach"), &config->Ragebot.other.hitscan_stomach, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
				ImGui::Selectable(("pelvis"), &config->Ragebot.other.hitscan_pelvis, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
				ImGui::Selectable(("legs"), &config->Ragebot.other.hitscan_legs, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
				ImGui::Selectable(("feet"), &config->Ragebot.other.hitscan_feet, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
				ImGui::EndCombo();
			}

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);

			if (ImGui::BeginCombo("multipoint hitboxes", "...", ImVec2(0, 255))) {
				const auto cap = 62;
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
				ImGui::Selectable(("head"), &config->Ragebot.other.multipoint_head, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
				ImGui::Selectable(("chest"), &config->Ragebot.other.multipoint_chest, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
				ImGui::Selectable(("stomach"), &config->Ragebot.other.multipoint_stomach, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
				ImGui::Selectable(("pelvis"), &config->Ragebot.other.multipoint_pelvis, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
				ImGui::Selectable(("legs"), &config->Ragebot.other.multipoint_legs, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
				ImGui::Selectable(("feet"), &config->Ragebot.other.multipoint_feet, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
				ImGui::EndCombo();
			}

			if (config->Ragebot.other.multipoint_head)
				ImGui::SliderInt("head scale", &config->Ragebot.other.head_scale, 0.f, 100.f, ("%0.f"));
			if (config->Ragebot.other.multipoint_chest)
				ImGui::SliderInt("chest scale", &config->Ragebot.other.chest_scale, 0.f, 100.f, ("%0.f"));
			if (config->Ragebot.other.multipoint_stomach)
				ImGui::SliderInt("stomach scale", &config->Ragebot.other.stomach_scale, 0.f, 100.f, ("%0.f"));
			if (config->Ragebot.other.multipoint_pelvis)
				ImGui::SliderInt("pelvis scale", &config->Ragebot.other.pelvis_scale, 0.f, 100.f, ("%0.f"));
			if (config->Ragebot.other.multipoint_legs)
				ImGui::SliderInt("legs scale", &config->Ragebot.other.legs_scale, 0.f, 100.f, ("%0.f"));
			if (config->Ragebot.other.multipoint_feet)
				ImGui::SliderInt("feet scale", &config->Ragebot.other.feet_scale, 0.f, 100.f, ("%0.f"));

			ImGui::Checkbox("static point scale", &config->Ragebot.other.static_point);
			ImGui::Checkbox("safe point override ", &config->Ragebot.other.safe_point_override_enable); 
			if (config->Ragebot.other.safe_point_override_enable) {
				ImGui::SameLine();
				ImGui::Hotkey("##save_point_override_key", &config->Ragebot.other.save_point_override, ImVec2(50, 40));
			}
		} break;
		case 6: {
			ImGui::Text("");
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);

			if (ImGui::BeginCombo("hitscan hitboxes", "...", ImVec2(0, 255))) {
				const auto cap = 62;
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
				ImGui::Selectable(("head"), &config->Ragebot.taser.hitscan_head, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
				ImGui::Selectable(("chest"), &config->Ragebot.taser.hitscan_chest, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
				ImGui::Selectable(("stomach"), &config->Ragebot.taser.hitscan_stomach, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
				ImGui::Selectable(("pelvis"), &config->Ragebot.taser.hitscan_pelvis, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
				ImGui::Selectable(("legs"), &config->Ragebot.taser.hitscan_legs, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
				ImGui::Selectable(("feet"), &config->Ragebot.taser.hitscan_feet, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
				ImGui::EndCombo();
			}

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);

			if (ImGui::BeginCombo("multipoint hitboxes", "...", ImVec2(0, 255))) {
				const auto cap = 62;
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
				ImGui::Selectable(("head"), &config->Ragebot.taser.multipoint_head, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
				ImGui::Selectable(("chest"), &config->Ragebot.taser.multipoint_chest, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
				ImGui::Selectable(("stomach"), &config->Ragebot.taser.multipoint_stomach, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
				ImGui::Selectable(("pelvis"), &config->Ragebot.taser.multipoint_pelvis, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
				ImGui::Selectable(("legs"), &config->Ragebot.taser.multipoint_legs, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
				ImGui::Selectable(("feet"), &config->Ragebot.taser.multipoint_feet, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
				ImGui::EndCombo();
			}

			if (config->Ragebot.taser.multipoint_head)
				ImGui::SliderInt("head scale", &config->Ragebot.taser.head_scale, 0.f, 100.f, ("%0.f"));
			if (config->Ragebot.taser.multipoint_chest)
				ImGui::SliderInt("chest scale", &config->Ragebot.taser.chest_scale, 0.f, 100.f, ("%0.f"));
			if (config->Ragebot.taser.multipoint_stomach)
				ImGui::SliderInt("stomach scale", &config->Ragebot.taser.stomach_scale, 0.f, 100.f, ("%0.f"));
			if (config->Ragebot.taser.multipoint_pelvis)
				ImGui::SliderInt("pelvis scale", &config->Ragebot.taser.pelvis_scale, 0.f, 100.f, ("%0.f"));
			if (config->Ragebot.taser.multipoint_legs)
				ImGui::SliderInt("legs scale", &config->Ragebot.taser.legs_scale, 0.f, 100.f, ("%0.f"));
			if (config->Ragebot.taser.multipoint_feet)
				ImGui::SliderInt("feet scale", &config->Ragebot.taser.feet_scale, 0.f, 100.f, ("%0.f"));

			ImGui::Checkbox("static point scale", &config->Ragebot.taser.static_point);
			ImGui::Checkbox("safe point override ", &config->Ragebot.taser.safe_point_override_enable);
			if (config->Ragebot.taser.safe_point_override_enable) {
				ImGui::SameLine();
				ImGui::Hotkey("##save_point_override_key", &config->Ragebot.taser.save_point_override, ImVec2(50, 40));
			}
		} break;
	}
}
void prefer_aimbot() {

	const char* weapons[] = { "pistol", "auto", "scout", "awp", "heavy", "other","taser","" };
	const char* exploits[] = { "none","double tap", "rapid fire","hideshots" };
	const char* aim_prefer[] = { "head", "body" };
	const char* hitboxes[] = { "none","head", "chest","stomach","pelvis","legs" };

	ImGuiStyle& style = ImGui::GetStyle();

	ImGui::Combo("weapon##weapon_select", &config->Ragebot.weaponconfig, weapons, ARRAYSIZE(weapons));

	ImGui::Text("");

	switch (config->Ragebot.weaponconfig) {
	case 0: {

		ImGui::Combo("exploits##exploitss", &config->Ragebot.pistol.exploits, exploits, ARRAYSIZE(exploits));

		ImGui::Combo("prefer hitbox", &config->Ragebot.pistol.hitbox_prefer, aim_prefer, ARRAYSIZE(aim_prefer)); ImGui::Text("  ");

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);

		if (ImGui::BeginCombo("body aim", "...", ImVec2(0, 255))) {
			const auto cap = 62;
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
			ImGui::Selectable(("lethal"), &config->Ragebot.pistol.body_aim_lethal, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
			ImGui::Selectable(("in-air"), &config->Ragebot.pistol.body_aim_in_air, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
			ImGui::Selectable(("slow-walking"), &config->Ragebot.pistol.body_aim_slow_walk, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
			ImGui::Selectable(("not resolved"), &config->Ragebot.pistol.body_aim_if_not_on_shot, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
			ImGui::Selectable(("after x shots"), &config->Ragebot.pistol.body_after_x_missed_enable, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
			ImGui::Selectable(("override on heath"), &config->Ragebot.pistol.health_override_enable, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
			ImGui::Selectable(("key"), &config->Ragebot.pistol.body_aim_key_enable, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
			ImGui::EndCombo();
		}

		if (config->Ragebot.pistol.body_after_x_missed_enable)
			ImGui::SliderInt("missed shots", &config->Ragebot.pistol.body_after_x_missed, 0, 5, ("%0.f"));
		if (config->Ragebot.pistol.health_override_enable)
			ImGui::SliderInt("override on health##slider", &config->Ragebot.pistol.health_override, 0.f, 100.f, ("%0.f"));
		if (config->Ragebot.pistol.body_aim_key_enable) {
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 15);
			ImGui::Text("key"); style.KeyBindOverride.x += 18;
			ImGui::SameLine(); ImGui::Hotkey("##bodykey", &config->Ragebot.pistol.body_aim_key, ImVec2(50, 22));
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 7);
		}

		ImGui::SliderInt("shot delay (ms)##", &config->Ragebot.pistol.delay_shot, 0.f, 200.f, ("%0.f"));

		ImGui::Combo("hitbox override", &config->Ragebot.pistol.hitbox_override, hitboxes, ARRAYSIZE(hitboxes));
		ImGui::Dummy(ImVec2(0, 8));
		ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 15, ImGui::GetCursorPosY() - 7));
		ImGui::Text("key"); style.KeyBindOverride.x += 18;
		ImGui::SameLine(); ImGui::Hotkey("##hitboxoverridekey", &config->Ragebot.pistol.hitbox_override_key, ImVec2(50, 22));

	} break;
	case 1: {
		ImGui::Combo("exploits##exploitss", &config->Ragebot._auto.exploits, exploits, ARRAYSIZE(exploits));

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);

		if (ImGui::BeginCombo("body aim", "...", ImVec2(0, 255))) {
			const auto cap = 62;
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
			ImGui::Selectable(("lethal"), &config->Ragebot._auto.body_aim_lethal, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
			ImGui::Selectable(("in-air"), &config->Ragebot._auto.body_aim_in_air, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
			ImGui::Selectable(("slow-walking"), &config->Ragebot._auto.body_aim_slow_walk, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
			ImGui::Selectable(("not resolved"), &config->Ragebot._auto.body_aim_if_not_on_shot, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
			ImGui::Selectable(("after x shots"), &config->Ragebot._auto.body_after_x_missed_enable, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
			ImGui::Selectable(("override on heath"), &config->Ragebot._auto.health_override_enable, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
			ImGui::Selectable(("key"), &config->Ragebot._auto.body_aim_key_enable, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
			ImGui::EndCombo();
		}

		if (config->Ragebot._auto.body_after_x_missed_enable)
			ImGui::SliderInt("missed shots", &config->Ragebot._auto.body_after_x_missed, 0, 5, ("%0.f"));
		if (config->Ragebot._auto.health_override_enable)
			ImGui::SliderInt("override on health##slider", &config->Ragebot._auto.health_override, 0.f, 100.f, ("%0.f"));
		if (config->Ragebot._auto.body_aim_key_enable) {
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 15);
			ImGui::Text("key"); style.KeyBindOverride.x += 18;
			ImGui::SameLine(); ImGui::Hotkey("##bodykey", &config->Ragebot._auto.body_aim_key, ImVec2(50, 22));
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 7);
		}

		ImGui::SliderInt("shot delay (ms)##", &config->Ragebot._auto.delay_shot, 0.f, 200.f, ("%0.f"));

		ImGui::Combo("hitbox override", &config->Ragebot._auto.hitbox_override, hitboxes, ARRAYSIZE(hitboxes));
		ImGui::Dummy(ImVec2(0, 8));
		ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 15, ImGui::GetCursorPosY() - 7));
		ImGui::Text("key"); style.KeyBindOverride.x += 18;
		ImGui::SameLine(); ImGui::Hotkey("##hitboxoverridekey", &config->Ragebot._auto.hitbox_override_key, ImVec2(50, 22));
	} break;
	case 2: {
		ImGui::Combo("exploits##exploitss", &config->Ragebot.scout.exploits, exploits, ARRAYSIZE(exploits));

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);

		if (ImGui::BeginCombo("body aim", "...", ImVec2(0, 255))) {
			const auto cap = 62;
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
			ImGui::Selectable(("lethal"), &config->Ragebot.scout.body_aim_lethal, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
			ImGui::Selectable(("in-air"), &config->Ragebot.scout.body_aim_in_air, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
			ImGui::Selectable(("slow-walking"), &config->Ragebot.scout.body_aim_slow_walk, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
			ImGui::Selectable(("not resolved"), &config->Ragebot.scout.body_aim_if_not_on_shot, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
			ImGui::Selectable(("after x shots"), &config->Ragebot.scout.body_after_x_missed_enable, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
			ImGui::Selectable(("override on heath"), &config->Ragebot.scout.health_override_enable, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
			ImGui::Selectable(("key"), &config->Ragebot.scout.body_aim_key_enable, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
			ImGui::EndCombo();
		}

		if (config->Ragebot.scout.body_after_x_missed_enable)
			ImGui::SliderInt("missed shots", &config->Ragebot.scout.body_after_x_missed, 0, 5, ("%0.f"));
		if (config->Ragebot.scout.health_override_enable)
			ImGui::SliderInt("override on health##slider", &config->Ragebot.scout.health_override, 0.f, 100.f, ("%0.f"));
		if (config->Ragebot.scout.body_aim_key_enable) {
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 15);
			ImGui::Text("key"); style.KeyBindOverride.x += 18;
			ImGui::SameLine(); ImGui::Hotkey("##bodykey", &config->Ragebot.scout.body_aim_key, ImVec2(50, 22));
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 7);
		}

		ImGui::SliderInt("shot delay (ms)##", &config->Ragebot.scout.delay_shot, 0.f, 200.f, ("%0.f"));

		ImGui::Combo("hitbox override", &config->Ragebot.scout.hitbox_override, hitboxes, ARRAYSIZE(hitboxes));
		ImGui::Dummy(ImVec2(0, 8));
		ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 15, ImGui::GetCursorPosY() - 7));
		ImGui::Text("key"); style.KeyBindOverride.x += 18;
		ImGui::SameLine(); ImGui::Hotkey("##hitboxoverridekey", &config->Ragebot.scout.hitbox_override_key, ImVec2(50, 22));
	} break;
	case 3: {
		ImGui::Combo("exploits##exploitss", &config->Ragebot.awp.exploits, exploits, ARRAYSIZE(exploits));
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);

		if (ImGui::BeginCombo("body aim", "...", ImVec2(0, 255))) {
			const auto cap = 62;
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
			ImGui::Selectable(("lethal"), &config->Ragebot.awp.body_aim_lethal, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
			ImGui::Selectable(("in-air"), &config->Ragebot.awp.body_aim_in_air, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
			ImGui::Selectable(("slow-walking"), &config->Ragebot.awp.body_aim_slow_walk, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
			ImGui::Selectable(("not resolved"), &config->Ragebot.awp.body_aim_if_not_on_shot, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
			ImGui::Selectable(("after x shots"), &config->Ragebot.awp.body_after_x_missed_enable, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
			ImGui::Selectable(("override on heath"), &config->Ragebot.awp.health_override_enable, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
			ImGui::Selectable(("key"), &config->Ragebot.awp.body_aim_key_enable, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
			ImGui::EndCombo();
		}

		if (config->Ragebot.awp.body_after_x_missed_enable)
			ImGui::SliderInt("missed shots", &config->Ragebot.awp.body_after_x_missed, 0, 5, ("%0.f"));
		if (config->Ragebot.awp.health_override_enable)
			ImGui::SliderInt("override on health##slider", &config->Ragebot.awp.health_override, 0.f, 100.f, ("%0.f"));
		if (config->Ragebot.awp.body_aim_key_enable) {
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 15);
			ImGui::Text("key"); style.KeyBindOverride.x += 18;
			ImGui::SameLine(); ImGui::Hotkey("##bodykey", &config->Ragebot.awp.body_aim_key, ImVec2(50, 22));
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 7);
		}

		ImGui::SliderInt("shot delay (ms)##", &config->Ragebot.awp.delay_shot, 0.f, 200.f, ("%0.f"));

		ImGui::Combo("hitbox override", &config->Ragebot.awp.hitbox_override, hitboxes, ARRAYSIZE(hitboxes));
		ImGui::Dummy(ImVec2(0, 8));
		ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 15, ImGui::GetCursorPosY() - 7));
		ImGui::Text("key"); style.KeyBindOverride.x += 18;
		ImGui::SameLine(); ImGui::Hotkey("##hitboxoverridekey", &config->Ragebot.awp.hitbox_override_key, ImVec2(50, 22));
	} break;
	case 4: {
		ImGui::Combo("exploits##exploitss", &config->Ragebot.heavy.exploits, exploits, ARRAYSIZE(exploits));
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);

		if (ImGui::BeginCombo("body aim", "...", ImVec2(0, 255))) {
			const auto cap = 62;
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
			ImGui::Selectable(("lethal"), &config->Ragebot.heavy.body_aim_lethal, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
			ImGui::Selectable(("in-air"), &config->Ragebot.heavy.body_aim_in_air, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
			ImGui::Selectable(("slow-walking"), &config->Ragebot.heavy.body_aim_slow_walk, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
			ImGui::Selectable(("not resolved"), &config->Ragebot.heavy.body_aim_if_not_on_shot, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
			ImGui::Selectable(("after x shots"), &config->Ragebot.heavy.body_after_x_missed_enable, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
			ImGui::Selectable(("override on heath"), &config->Ragebot.heavy.health_override_enable, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
			ImGui::Selectable(("key"), &config->Ragebot.heavy.body_aim_key_enable, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
			ImGui::EndCombo();
		}

		if (config->Ragebot.heavy.body_after_x_missed_enable)
			ImGui::SliderInt("missed shots", &config->Ragebot.heavy.body_after_x_missed, 0, 5, ("%0.f"));
		if (config->Ragebot.heavy.body_aim_key_enable)
			ImGui::SliderInt("override on health##slider", &config->Ragebot.heavy.health_override, 0.f, 100.f, ("%0.f"));
		if (config->Ragebot.heavy.health_override_enable) {
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 15);
			ImGui::Text("key"); style.KeyBindOverride.x += 18;
			ImGui::SameLine(); ImGui::Hotkey("##bodykey", &config->Ragebot.heavy.body_aim_key, ImVec2(50, 22));
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 7);
		}

		ImGui::SliderInt("shot delay (ms)##", &config->Ragebot.heavy.delay_shot, 0.f, 200.f, ("%0.f"));

		ImGui::Combo("hitbox override", &config->Ragebot.heavy.hitbox_override, hitboxes, ARRAYSIZE(hitboxes));
		ImGui::Dummy(ImVec2(0, 8));
		ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 15, ImGui::GetCursorPosY() - 7));
		ImGui::Text("key"); style.KeyBindOverride.x += 18;
		ImGui::SameLine(); ImGui::Hotkey("##hitboxoverridekey", &config->Ragebot.heavy.hitbox_override_key, ImVec2(50, 22));
	} break;
	case 5: {
		ImGui::Combo("exploits##exploitss", &config->Ragebot.other.exploits, exploits, ARRAYSIZE(exploits));
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);

		if (ImGui::BeginCombo("body aim", "...", ImVec2(0, 255))) {
			const auto cap = 62;
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
			ImGui::Selectable(("lethal"), &config->Ragebot.other.body_aim_lethal, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
			ImGui::Selectable(("in-air"), &config->Ragebot.other.body_aim_in_air, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
			ImGui::Selectable(("slow-walking"), &config->Ragebot.other.body_aim_slow_walk, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
			ImGui::Selectable(("not resolved"), &config->Ragebot.other.body_aim_if_not_on_shot, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
			ImGui::Selectable(("after x shots"), &config->Ragebot.other.body_after_x_missed_enable, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
			ImGui::Selectable(("override on heath"), &config->Ragebot.other.health_override_enable, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
			ImGui::Selectable(("key"), &config->Ragebot.other.body_aim_key_enable, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
			ImGui::EndCombo();
		}

		if (config->Ragebot.other.body_after_x_missed_enable)
			ImGui::SliderInt("missed shots", &config->Ragebot.other.body_after_x_missed, 0, 5, ("%0.f"));
		if (config->Ragebot.other.body_aim_key_enable)
			ImGui::SliderInt("override on health##slider", &config->Ragebot.other.health_override, 0.f, 100.f, ("%0.f"));
		if (config->Ragebot.other.health_override_enable) {
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 15);
			ImGui::Text("key"); style.KeyBindOverride.x += 18;
			ImGui::SameLine(); ImGui::Hotkey("##bodykey", &config->Ragebot.other.body_aim_key, ImVec2(50, 22));
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 7);
		}

		ImGui::SliderInt("shot delay (ms)##", &config->Ragebot.other.delay_shot, 0.f, 200.f, ("%0.f"));

		ImGui::Combo("hitbox override", &config->Ragebot.other.hitbox_override, hitboxes, ARRAYSIZE(hitboxes));
		ImGui::Dummy(ImVec2(0, 8));
		ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 15, ImGui::GetCursorPosY() - 7));
		ImGui::Text("key"); style.KeyBindOverride.x += 18;
		ImGui::SameLine(); ImGui::Hotkey("##hitboxoverridekey", &config->Ragebot.other.hitbox_override_key, ImVec2(50, 22));
	} break;
	case 6: {
		ImGui::Combo("exploits##exploitss", &config->Ragebot.taser.exploits, exploits, ARRAYSIZE(exploits));

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);

		if (ImGui::BeginCombo("body aim", "...", ImVec2(0, 255))) {
			const auto cap = 62;
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
			ImGui::Selectable(("lethal"), &config->Ragebot.taser.body_aim_lethal, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
			ImGui::Selectable(("in-air"), &config->Ragebot.taser.body_aim_in_air, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
			ImGui::Selectable(("slow-walking"), &config->Ragebot.taser.body_aim_slow_walk, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
			ImGui::Selectable(("not resolved"), &config->Ragebot.taser.body_aim_if_not_on_shot, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
			ImGui::Selectable(("override on heath"), &config->Ragebot.taser.health_override_enable, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
			ImGui::Selectable(("key"), &config->Ragebot.taser.body_aim_key_enable, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
			ImGui::EndCombo();
		}

		if (config->Ragebot.taser.body_aim_key_enable)
			ImGui::SliderInt("override on health##slider", &config->Ragebot.taser.health_override, 0.f, 100.f, ("%0.f"));
		if (config->Ragebot.taser.health_override_enable) {
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 15);
			ImGui::Text("key"); style.KeyBindOverride.x += 18;
			ImGui::SameLine(); ImGui::Hotkey("##bodykey", &config->Ragebot.taser.body_aim_key, ImVec2(50, 22));
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 7);
		}

		ImGui::SliderInt("shot delay (ms)##", &config->Ragebot.taser.delay_shot, 0.f, 200.f, ("%0.f"));

		ImGui::Combo("hitbox override", &config->Ragebot.taser.hitbox_override, hitboxes, ARRAYSIZE(hitboxes));
		ImGui::Dummy(ImVec2(0, 8));
		ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 15, ImGui::GetCursorPosY() - 7));
		ImGui::Text("key"); style.KeyBindOverride.x += 18;
		ImGui::SameLine(); ImGui::Hotkey("##hitboxoverridekey", &config->Ragebot.pistol.hitbox_override_key, ImVec2(50, 22));
	} break;
	}
}

void c_menu::GUI_Init(HWND window, IDirect3DDevice9* pDevice)
{
	ImGui_ImplDX9_Init(window, pDevice);

	ImGuiStyle        _style = ImGui::GetStyle(); 
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontDefault();

	font_primary = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(Verdana_compressed_data, Verdana_compressed_size, 12.f);
	//font_primary = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(Regular_compressed_data, Regular_compressed_size, 15.f);
	color_font = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(Regular_compressed_data, Regular_compressed_size, 26);

	ImGuiStyle& style = ImGui::GetStyle();
	style.Alpha = 1.0f;
	style.WindowPadding = ImVec2(0, 0);
	style.WindowMinSize = ImVec2(32, 32);
	style.WindowRounding = 0.f;
	style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
	style.ChildWindowRounding = 0.f;
	style.FramePadding = ImVec2(0, 0);
	style.FrameRounding = 0.f;
	style.ItemSpacing = ImVec2(8, 4);
	style.ItemInnerSpacing = ImVec2(4, 4);
	style.TouchExtraPadding = ImVec2(0, 0);
	style.IndentSpacing = 21.0f;
	style.ColumnsMinSpacing = 6.0f;
	style.ScrollbarSize = 6.0f;
	style.ScrollbarRounding = 0.0f;
	style.GrabMinSize = 5.0f;
	style.GrabRounding = 0.0f;
	//style.FrameBorderSize = 1.0f;
	//style.ChildBorderSize = 1.0f;
	style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
	style.DisplayWindowPadding = ImVec2(22, 22);
	style.DisplaySafeAreaPadding = ImVec2(0, 0);
	style.AntiAliasedLines = false;
	style.AntiAliasedShapes = false;
	style.CurveTessellationTol = 1.25f;

	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(RGBA_TO_FLOAT(config->Misc.menucolor[0] * 255, config->Misc.menucolor[1] * 255, config->Misc.menucolor[2] * 255, 230));
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(RGBA_TO_FLOAT(config->Misc.menucolor[0] * 255, config->Misc.menucolor[1] * 255, config->Misc.menucolor[2] * 255, 255));
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(RGBA_TO_FLOAT(config->Misc.menucolor[0] * 255, config->Misc.menucolor[1] * 255, config->Misc.menucolor[2] * 255, 255));
	style.Colors[ImGuiCol_Text] = ImVec4(1.f, 1.f, 1.f, 1.00f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	style.Colors[ImGuiCol_WindowBg] = ImColor(91, 233, 255); // background window
	style.Colors[ImGuiCol_ChildWindowBg] = ImColor(15, 15, 15, 255); // child window
	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.19f, 0.01f, 0.19f, 0.f);
	style.Colors[ImGuiCol_Border] = ImVec4(config->Misc.menucolor[0], config->Misc.menucolor[1], config->Misc.menucolor[2], 1.f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.46f, 0.47f, 0.5f, 0.f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.09f, 0.09f, 0.09f, 1.f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.07f, 0.07f, 0.07f, 0.48f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.07f, 0.48f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.07f, 0.07f, 0.07f, 0.48f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(config->Misc.menucolor[0], config->Misc.menucolor[1], config->Misc.menucolor[2], 1.f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(config->Misc.menucolor[0], config->Misc.menucolor[1], config->Misc.menucolor[2], 1.f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(config->Misc.menucolor[0], config->Misc.menucolor[1], config->Misc.menucolor[2], 1.f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(config->Misc.menucolor[0], config->Misc.menucolor[1], config->Misc.menucolor[2], 1.f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(config->Misc.menucolor[0], config->Misc.menucolor[1], config->Misc.menucolor[2], 1.f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(config->Misc.menucolor[0], config->Misc.menucolor[1], config->Misc.menucolor[2], 1.f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(config->Misc.menucolor[0], config->Misc.menucolor[1], config->Misc.menucolor[2], 1.f);
	style.Colors[ImGuiCol_Button] = ImColor(75, 75, 75, 230);
	style.Colors[ImGuiCol_ButtonHovered] = ImColor(60, 60, 60, 255);
	style.Colors[ImGuiCol_ButtonActive] = ImColor(255, 255, 255, 255);

	style.Colors[ImGuiCol_Header] = ImVec4(config->Misc.menucolor[0], config->Misc.menucolor[1], config->Misc.menucolor[2], 1.f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(config->Misc.menucolor[0], config->Misc.menucolor[1], config->Misc.menucolor[2], 1.f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(config->Misc.menucolor[0], config->Misc.menucolor[1], config->Misc.menucolor[2], 1.f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);

	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(config->Misc.menucolor[0], config->Misc.menucolor[1], config->Misc.menucolor[2], 1.f);
	style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

	//style.ColorButtonPosition = ImGuiDir_Down;

	ImGui::SetColorEditOptions(
		ImGuiColorEditFlags_Uint8 |
		ImGuiColorEditFlags_NoInputs |
		ImGuiColorEditFlags_NoTooltip |
		ImGuiColorEditFlags_NoLabel |
		ImGuiColorEditFlags_AlphaBar |
		ImGuiColorEditFlags_NoOptions |
		ImGuiColorEditFlags_NoSidePreview
	);

	config->Misc.menucolor[0] = { 0.062745098 };
	config->Misc.menucolor[1] = { 0.92549019607 };
	config->Misc.menucolor[2] = { 0.83921568627 };

	config->Misc.accentcolor[0] = { 0.062745098 };
	config->Misc.accentcolor[1] = { 0.92549019607 };
	config->Misc.accentcolor[2] = { 0.83921568627 };


	array(config->Colors.boxcol)
	array(config->Colors.namecol)
	array(config->Colors.healthcol)
	array(config->Colors.weaponcol)
	array(config->Colors.ammocol)
	array(config->Colors.ammobar)
	array(config->Colors.armorcol)
	array(config->Colors.localchamscol)
	array(config->Colors.localglow_col)
	array(config->Colors.glow_col)
	array(config->Colors.history_skeletoncol)
	array(config->Colors.poovcob)
	array(config->Colors.beamcolor)
	array(config->Colors.nightcolor)
	array(config->Colors.skycolor)
	array(config->Colors.propcolor)
	array(config->Colors.nadecolorr)
	array(config->Colors.skeletoncol)
	array(config->Colors.trails_color)
	array(config->Colors.ebeamcolor)
	array(config->Colors.lagcompcolor)
	array(config->Colors.bombcolor)
	array(config->Colors.dmgcolor)
	array(config->Colors.dmgcolor2)
	array(config->Colors.droppedcolor)
	array(config->Colors.hitmarker)
	array(config->Colors.hitmarker2d)
	array(config->Colors.f_aa_ind)
	array(config->Colors.f_aa_ind2)
	array(config->Colors.t_aa_ind)
	array(config->Colors.t_aa_ind2)
	array(config->Colors.enchance_scope)
	array(config->Colors.bombcolor)

	config->Misc.watermark = true;

	array(config->Visuals.local.box_color)
	array(config->Visuals.team.box_color)
	array(config->Visuals.enemy.box_color)

	array(config->Visuals.local.name_color)
	array(config->Visuals.team.name_color)
	array(config->Visuals.enemy.name_color)

	array(config->Visuals.local.icons_color)
	array(config->Visuals.team.icons_color)
	array(config->Visuals.enemy.icons_color)

	array(config->Visuals.local.armor_flag_color) 
	array(config->Visuals.team.armor_flag_color) 
	array(config->Visuals.enemy.armor_flag_color) 

	array(config->Visuals.local.fakeduck_flag_color)
	array(config->Visuals.team.fakeduck_flag_color)
	array(config->Visuals.enemy.fakeduck_flag_color)

	array(config->Visuals.local.flash_flag_color)
	array(config->Visuals.team.flash_flag_color)
	array(config->Visuals.enemy.flash_flag_color)

	array(config->Visuals.local.scope_flag_color)
	array(config->Visuals.team.scope_flag_color)
	array(config->Visuals.enemy.scope_flag_color)

	array(config->Visuals.local.defuse_flag_color)
	array(config->Visuals.team.defuse_flag_color)
	array(config->Visuals.enemy.defuse_flag_color)

	array(config->Visuals.local.health_color)
	array(config->Visuals.team.health_color) 
	array(config->Visuals.enemy.health_color)

	array(config->Visuals.local.weapon_color)
	array(config->Visuals.team.weapon_color) 
	array(config->Visuals.enemy.weapon_color)

	array(config->Visuals.local.ammo_color) 
	array(config->Visuals.team.ammo_color) 
	array(config->Visuals.enemy.ammo_color) 

	array(config->Visuals.local.armor_color) 
	array(config->Visuals.team.armor_color)
	array(config->Visuals.enemy.armor_color)

	array(config->Visuals.team.snaplines_color)
	array(config->Visuals.enemy.snaplines_color)

	array(config->Visuals.enemy.damage_color)
	array(config->Visuals.enemy.radar_color)

	array(config->Visuals.dropped_box_color)
	array(config->Visuals.dropped_name_color)
	array(config->Visuals.dropped_icon_color)
	array(config->Visuals.dropped_ammo_color)
	array(config->Colors.droppedglow_col)
	array(config->Colors.molov_timer)
	array(config->Colors.smoke_timer)
	
	array(config->Visuals.team.skeleton_color)
	array(config->Visuals.local.skeleton_color)
	array(config->Visuals.enemy.bt_skeleton_color)
	array(config->Visuals.enemy.skeleton_color)
	array(config->Colors.spread_crosshair)

	d3dinit = true;
}

const char* gloves[] =
{
	"none",
	"bloodhound",
	"sporty",
	"slick",
	"handwrap",
	"motorcycle",
	"specialist",

	"",
};

const char* knives[] =
{
	"bayonet",
	"flip",
	"gut",
	"karambit",
	"m9 bayonet",
	"huntsman",
	"butterfly",
	"falchion",
	"daggers",
	"bowie",
	"navaja",
	"stiletto",
	"ursus",
	"talon",
	"css",
	"",
};

const char* guns[] =
{
		"none","awp", "usp", "p2k", "glock", "deg", "revol", "ssg", "dual", "p250", "cz75", "five7", "tec9", "nova", "xm1014", "mag7", "sawed", "negev", "m249", "mp9", "mac10", "mp7", "ump", "p90", "ppbizon", "galil", "famas", "m4a4", "m4a1", "ak47", "aug", "sg553", "scar20", "g3sg1", "mp5", "knife",
		""
};

void c_menu::mainWindow()
{

	ImGui::SetNextWindowSize(ImVec2(500, 449));
	ImGui::PushFont(font_primary);

	static const char* tabs[] = { "ragebot","antiaims","visuals","misc","skins" };
	const char* rifles[] = { "disabled", "auto", "scout", "awp" };
	const char* cfgs[] = { "C_1", "C_2", "C_3", "C_4", "C_5", "C_6" };

	static int cur_tab = 0;
	ImGui::Begin("       LAgsync Software", &open, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_ShowBorders | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_AlwaysAutoResize);
	{
		ImGui::Dummy(ImVec2(0, 5));
		const float tabsize = ((ImGui::GetWindowWidth() - 20.f) / 5);
		for (auto i = 0; i < 5; i++)
			if (ImGui::Tab(tabs[i], i == cur_tab, ImVec2(tabsize, 20)))
				cur_tab = i;

		auto save_dragging = [&]()->void {
			config->i_drag = mouse::i_drag;
			config->s_drag = mouse::s_drag;
			config->d_drag = mouse::d_drag;
			config->k_drag = mouse::k_drag;
		};

		auto load_dragging = [&]()->void {
			mouse::i_drag = config->i_drag;
			mouse::s_drag = config->s_drag;
			mouse::d_drag = config->d_drag;
			mouse::k_drag = config->k_drag;
		};

		ImGui::Dummy(ImVec2(0, 75));
		static int cfg = 0;
		ImGui::Switch("config", cfgs, ImVec2(tabsize, 20), sizeof(cfgs) / sizeof(cfgs[0]), cfg);
		if (ImGui::Tab("save", 1 == 1, ImVec2(tabsize, 20))) {
			save_dragging();
			g_Config->Save(cfg);
		}
		if (ImGui::Tab("load", 1 == 1, ImVec2(tabsize, 20))) {
			g_Config->Load(cfg);
			load_dragging();
		}
		ImGui::SameLine(0);
		ImGui::SetCursorPos(ImVec2(75, 9));

		if (ImGui::BeginChild("##", ImVec2(-1, -1), true, 0))
		{
			switch (cur_tab)
			{//rage, aa, vis, misc,skins
			case 0: {
				static int cur_sub_time = 0;
				static const char* sub_tabs[] = { "main","modes","targeting" };
				const float sub_tabsize = ((ImGui::GetWindowWidth() - 40.f) / 3);

				ImVec2 p = ImGui::GetCursorScreenPos();
				ImGui::GetWindowDrawList()->AddRect(ImVec2(p.x, p.y), ImVec2(p.x + ImGui::GetWindowWidth() - 8, p.y + 30), ImColor(60, 60, 60, 250), 0, -1, 1);
				ImGui::GetWindowDrawList()->AddLine(ImVec2(p.x, p.y), ImVec2(p.x + ImGui::GetWindowWidth() - 9, p.y), ImColor(60, 60, 60, 250), 2.f);

				for (auto i = 0; i < 3; i++)
				{
					if (ImGui::Tab(sub_tabs[i], i == cur_sub_time, ImVec2(sub_tabsize, 20)))
						cur_sub_time = i;

					if (i < 3)
						ImGui::SameLine(0);
				}

				ImGui::Dummy(ImVec2(0, 12));
				switch (cur_sub_time) {
				case 0: {
					main_aimbot();
				}	break;
				case 1: {
					prefer_aimbot();
				}	break;
				case 2: {
					hitscan_aimbot();
				}	break;
				}
			}break;
			case 1: {
				static int cur_sub_time = 0;
				static const char* sub_tabs[] = { "main","misc" };
				const float sub_tabsize = ((ImGui::GetWindowWidth() - 40.f) / 2);
				ImGuiStyle& style = ImGui::GetStyle();

				ImVec2 p = ImGui::GetCursorScreenPos();
				ImGui::GetWindowDrawList()->AddRect(ImVec2(p.x, p.y), ImVec2(p.x + ImGui::GetWindowWidth() - 8, p.y + 30), ImColor(60, 60, 60, 250), 0, -1, 1);
				ImGui::GetWindowDrawList()->AddLine(ImVec2(p.x, p.y), ImVec2(p.x + ImGui::GetWindowWidth() - 9, p.y), ImColor(60, 60, 60, 250), 2.f);

				for (auto i = 0; i < 2; i++)
				{
					if (ImGui::Tab(sub_tabs[i], i == cur_sub_time, ImVec2(sub_tabsize, 20)))
						cur_sub_time = i;

					if (i < 2)
						ImGui::SameLine(0);
				}

				ImGui::Dummy(ImVec2(0, 12));

				switch (cur_sub_time) {
				case 0: {
					const char* playerstates[] = { "stand", "move","air" };
					const char* pitch[] = { "none", "up","down","jitter" };
					const char* yaw[] = { "none","180","jitter" };
					const char* desync[] = { "none","fake","real","jitter" };
					const char* yawbase[] = { "none","targets","freestand" };

					ImGui::Checkbox("enable anti-aim##enableaa", &config->Ragebot._antiaim);
					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);

					if (ImGui::BeginCombo("off-condition", "...", ImVec2(0, 255))) {
						const auto cap = 62;
						ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
						ImGui::Selectable(("knife"), &config->Ragebot.disableaaonknife, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
						ImGui::Selectable(("freeze"), &config->Ragebot.disableaaonfreeze, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::EndCombo();
					}
					ImGui::Combo("playerstates##playerstates", &config->Ragebot.antiaim_type, playerstates, ARRAYSIZE(playerstates));
					if (config->Ragebot.antiaim_type == 0) {
						ImGui::Combo("pitch", &config->Ragebot.stand.pitch, pitch, ARRAYSIZE(pitch));
						if (config->Ragebot.stand.pitch == 3) {
							ImGui::Text("  ");
							ImGui::SliderInt("##jitter range", &config->Ragebot.stand.pitch_range, -89, 89, ("%0.f"));
						}

						ImGui::Combo("yaw base", &config->Ragebot.stand.yawbase, yawbase, ARRAYSIZE(yawbase)); ImGui::Text("  ");

						ImGui::Combo("yaw", &config->Ragebot.stand.yaw, yaw, ARRAYSIZE(yaw));
						if (config->Ragebot.stand.yaw == 2) {
							ImGui::Text("  ");
							ImGui::SliderInt("length##jitter length", &config->Ragebot.stand.jitter_length, 0, 180, ("%0.f"));
						}
						ImGui::Combo("desync", &config->Ragebot.stand.fake_type, desync, ARRAYSIZE(desync)); ImGui::Text("  ");
						ImGui::SliderFloat("##maxdesync", &config->Ragebot.stand.max_delta, 0, 58, ("%0.f"));
						ImGui::SliderInt("lean %", &config->Ragebot.stand.lean_break, 0, 100, ("%0.f"));
						ImGui::Checkbox("twist", &config->Ragebot.stand.twist);
						ImGui::Checkbox("desync on shot", &config->Ragebot.stand.desync_on_shot);
						ImGui::Checkbox("avoid overlap", &config->Ragebot.stand.avoid_overlap);
					}
					if (config->Ragebot.antiaim_type == 1) {
						ImGui::Combo("pitch", &config->Ragebot.moving.pitch, pitch, ARRAYSIZE(pitch));
						if (config->Ragebot.moving.pitch == 3) {
							ImGui::Text("  ");
							ImGui::SliderInt("##jitter range", &config->Ragebot.moving.pitch_range, -89, 89, ("%0.f"));
						}

						ImGui::Combo("yaw base", &config->Ragebot.moving.yawbase, yawbase, ARRAYSIZE(yawbase)); ImGui::Text("  ");

						ImGui::Combo("yaw", &config->Ragebot.moving.yaw, yaw, ARRAYSIZE(yaw));
						if (config->Ragebot.moving.yaw == 2) {
							ImGui::Text("  ");
							ImGui::SliderInt("length##jitter length", &config->Ragebot.moving.jitter_length, 0, 180, ("%0.f"));
						}
						ImGui::Combo("desync", &config->Ragebot.moving.fake_type, desync, ARRAYSIZE(desync)); ImGui::Text("  ");
						ImGui::SliderFloat("##maxdesync", &config->Ragebot.moving.max_delta, 0, 58, ("%0.f"));
						ImGui::SliderInt("lean %", &config->Ragebot.moving.lean_break, 0, 100, ("%0.f"));
						ImGui::Checkbox("twist", &config->Ragebot.moving.twist);
						ImGui::Checkbox("desync on shot", &config->Ragebot.moving.desync_on_shot);
						ImGui::Checkbox("avoid overlap", &config->Ragebot.moving.avoid_overlap);
					}
					if (config->Ragebot.antiaim_type == 2) {
						ImGui::Combo("pitch", &config->Ragebot.air.pitch, pitch, ARRAYSIZE(pitch));
						if (config->Ragebot.air.pitch == 3) {
							ImGui::Text("  ");
							ImGui::SliderInt("##jitter range", &config->Ragebot.air.pitch_range, -89, 89, ("%0.f"));
						}

						ImGui::Combo("yaw base", &config->Ragebot.air.yawbase, yawbase, ARRAYSIZE(yawbase)); ImGui::Text("  ");

						ImGui::Combo("yaw", &config->Ragebot.air.yaw, yaw, ARRAYSIZE(yaw));
						if (config->Ragebot.air.yaw == 2) {
							ImGui::Text("  ");
							ImGui::SliderInt("length##jitter length", &config->Ragebot.air.jitter_length, 0, 180, ("%0.f"));
						}
						ImGui::Combo("desync", &config->Ragebot.air.fake_type, desync, ARRAYSIZE(desync)); ImGui::Text("  ");
						ImGui::SliderFloat("##maxdesync", &config->Ragebot.air.max_delta, 0, 58, ("%0.f"));
						ImGui::SliderInt("lean %", &config->Ragebot.air.lean_break, 0, 100, ("%0.f"));
						ImGui::Checkbox("twist", &config->Ragebot.air.twist);
						ImGui::Checkbox("desync on shot", &config->Ragebot.air.desync_on_shot);
						ImGui::Checkbox("avoid overlap", &config->Ragebot.air.avoid_overlap);
					}
					ImGui::Text("");
				}break;
				case 1: {
					const char* lag_type[] = { "jitter", "factor" };
					const char* desync_dir_type[] = { "none", "manual","auto" };
					const char* invertion[] = { "none","auto","manual","stand","moving","air" };

					ImGui::Checkbox("enable fakelag##enablefakelag", &config->Ragebot.fakelag_enable);
					ImGui::Combo("lag type##lagtypes", &config->Ragebot.lag_type, lag_type, ARRAYSIZE(lag_type)); ImGui::Text("  ");
					if (config->Ragebot.fakelag_enable) {
						ImGui::SliderInt("standing length##standfakealg", &config->Ragebot.stand_fakelag, 0, 14, ("%0.f"));
						ImGui::SliderInt("moving length##movefakealg", &config->Ragebot.move_fakelag, 0, 14, ("%0.f"));
						ImGui::SliderInt("air length##airfakealg", &config->Ragebot.air_fakelag, 0, 14, ("%0.f"));
					}
					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);

					if (ImGui::BeginCombo("boost fakelag", "...", ImVec2(0, 255))) {
						const auto cap = 62;
						ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
						ImGui::Selectable(("max delta"), &config->Ragebot.fl_boost_max_delta, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
						ImGui::Selectable(("reload"), &config->Ragebot.fl_boost_reload, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
						ImGui::Selectable(("shot"), &config->Ragebot.fl_boost_shot, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
						ImGui::Selectable(("switch"), &config->Ragebot.fl_boost_switch, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::EndCombo();
					}

					ImGui::SliderInt("boost amount", &config->Ragebot.fl_boost_scale, 0, 14, ("%0.f"));

					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3);

					if (ImGui::BeginCombo("disable fakelag", "...", ImVec2(0, 255))) {
						const auto cap = 32;
						ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
						ImGui::Selectable(("reload"), &config->Ragebot.fl_disable_reload, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
						ImGui::Selectable(("shot"), &config->Ragebot.fl_disable_shot, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
						ImGui::Selectable(("switch"), &config->Ragebot.fl_disable_switch, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::EndCombo();
					}

					ImGui::Checkbox("manual override##enablemanualoverride", &config->Ragebot.manaul_override);
					if (config->Ragebot.manaul_override) {
						ImGui::Checkbox("left ", &config->Ragebot.left_enable); ImGui::SameLine();
						ImGui::Hotkey("##dcxczxbvxxcb", &config->Ragebot.left, ImVec2(50, 40));
						ImGui::Checkbox("right", &config->Ragebot.right_enable); ImGui::SameLine();
						ImGui::Hotkey("##gsdcnbicbs", &config->Ragebot.right, ImVec2(50, 40));
						ImGui::Checkbox("back", &config->Ragebot.back_enable); ImGui::SameLine();
						ImGui::Hotkey("##inccdsjs", &config->Ragebot.back, ImVec2(50, 40));
					}
					ImGui::Combo("invert type", &config->Ragebot.fake_invert, invertion, ARRAYSIZE(invertion));
					if (config->Ragebot.fake_invert == 2)
					{
						ImGui::Text("");
						ImGui::SetCursorPosY(ImGui::GetCursorPosY() -6);
						ImGui::Dummy(ImVec2(1, 0)); ImGui::SameLine();
						ImGui::Text("  key "); ImGui::SameLine();  ImGui::SameLine(); ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 216, ImGui::GetCursorPosY() + 4));  style.KeyBindOverride.x += 18;
						ImGui::Hotkey("##desync_invert", &config->Ragebot.desync_invert, ImVec2(50, 40));
					}
				} break;
				}
			}break;
			case 2: {
				static int cur_sub_time = 0;
				static const char* sub_tabs[] = { "local","enemy","team","world","chams" };
				const float sub_tabsize = ((ImGui::GetWindowWidth() - 40.f) / 5);

				ImVec2 p = ImGui::GetCursorScreenPos();
				ImGui::GetWindowDrawList()->AddRect(ImVec2(p.x, p.y), ImVec2(p.x + ImGui::GetWindowWidth() - 8, p.y + 30), ImColor(60, 60, 60, 250), 0, -1, 1);
				ImGui::GetWindowDrawList()->AddLine(ImVec2(p.x, p.y), ImVec2(p.x + ImGui::GetWindowWidth() - 9, p.y), ImColor(60, 60, 60, 250), 2.f);

				for (auto i = 0; i < 5; i++)
				{
					if (ImGui::Tab(sub_tabs[i], i == cur_sub_time, ImVec2(sub_tabsize, 20)))
						cur_sub_time = i;

					if (i < 5)
						ImGui::SameLine(0);
				}

				ImGui::Dummy(ImVec2(0, 12));

				switch (cur_sub_time) {
				case 0: local_menu(); break;
				case 1: enemy_menu(); break;
				case 2: team_menu(); break;
				case 3: world_menu(); break;
				case 4: chams_menu(); break;
				};
			}break;
			case 3: {

				static int cur_sub_time = 0;
				static const char* sub_tabs[] = { "main","movements","misc" };
				const float sub_tabsize = ((ImGui::GetWindowWidth() - 40.f) / 3);

				ImVec2 p = ImGui::GetCursorScreenPos();
				ImGui::GetWindowDrawList()->AddRect(ImVec2(p.x, p.y), ImVec2(p.x + ImGui::GetWindowWidth() - 8, p.y + 30), ImColor(60, 60, 60, 250), 0, -1, 1);
				ImGui::GetWindowDrawList()->AddLine(ImVec2(p.x, p.y), ImVec2(p.x + ImGui::GetWindowWidth() - 9, p.y), ImColor(60, 60, 60, 250), 2.f);

				for (auto i = 0; i < 3; i++)
				{
					if (ImGui::Tab(sub_tabs[i], i == cur_sub_time, ImVec2(sub_tabsize, 20)))
						cur_sub_time = i;

					if (i < 5)
						ImGui::SameLine(0);
				}

				ImGui::Dummy(ImVec2(0, 12));

				ImGuiStyle& style = ImGui::GetStyle();
				const char* events[] = { "none", "basic" };
				const char* styles[] = { "basic", "advanced" };

				switch (cur_sub_time) {
					case 0: {
					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2); ImGui::Dummy(ImVec2(1, 0)); ImGui::SameLine();
					ImGui::Text("  menu color");
					ImGui::SameLine(); 	style.ColorOverride = ImVec2(-1, 3);
					ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 5); ImGui::MyColorEdit3(("##menucolor"), (float*)&config->Misc.menucolor, 1 << 7);
					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);	ImGui::Dummy(ImVec2(1, 0)); ImGui::SameLine();
					ImGui::Text("  accent color");
					ImGui::SameLine(); 	style.ColorOverride = ImVec2(-1, 3);
					ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 5); ImGui::MyColorEdit3(("##accentcolor"), (float*)&config->Misc.accentcolor, 1 << 7);
					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
					ImGui::Combo("window styles", &config->Visuals.indicators_styles, styles, ARRAYSIZE(styles));
					ImGui::Text("");
					if (ImGui::BeginCombo("windows", "...", ImVec2(0, 255))) {
						const auto cap = 62;
						ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
						ImGui::Selectable(("round summary"), &config->Visuals.round_summ, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
						ImGui::Selectable(("indicators"), &config->Visuals.indicators, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
						ImGui::Selectable(("spectators"), &config->Visuals.spec_list, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
						ImGui::Selectable(("keybinds"), &config->Visuals.keybinds, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::EndCombo();
					}
					ImGui::Checkbox("watermark##watermark", &config->Misc.watermark);
					ImGui::Checkbox("clantag##clantag_changer", &config->Misc.clantag);
					ImGui::Checkbox(("flip hand on knife##fliponknifew"), &config->Visuals.leftknife);

					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);

					if (ImGui::BeginCombo("view setup", "...", ImVec2(0, 255))) {
						const auto cap = 62;
						ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
						ImGui::Selectable(("fov"), &config->Visuals.view_setup_fov_enable, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
						ImGui::Selectable(("viewmodel"), &config->Visuals.view_setup_viewmodel_enable, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
						ImGui::Selectable(("aspect ratio"), &config->Visuals.view_setup_aspect_enable, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
						ImGui::Selectable(("x offset"), &config->Visuals.view_setup_xoffset_enable, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
						ImGui::Selectable(("y offset"), &config->Visuals.view_setup_yoffset_enable, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
						ImGui::Selectable(("z offset"), &config->Visuals.view_setup_zoffset_enable, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::EndCombo();
					}

					if (config->Visuals.view_setup_fov_enable)
						ImGui::SliderFloat("fov##fovzzz", &config->Visuals.fieldofview, 0.f, 350.f, ("%0.f"));
					if (config->Visuals.view_setup_viewmodel_enable)
						ImGui::SliderFloat("view model", &config->Visuals.viewmodelfov, 0.f, 180.f, ("%0.f"));
					if (config->Visuals.view_setup_aspect_enable)
						ImGui::SliderFloat("aspect ratio", &config->Visuals.viewmodelratio, 0, 5, ("%0.f"));
					if (config->Visuals.view_setup_xoffset_enable)
						ImGui::SliderFloat("x offset", &config->Visuals.view_setup_xoffset, -30, 30, ("%0.f"));
					if (config->Visuals.view_setup_yoffset_enable)
						ImGui::SliderFloat("y offset", &config->Visuals.view_setup_yoffset, -30, 30, ("%0.f"));
					if (config->Visuals.view_setup_zoffset_enable)
						ImGui::SliderFloat("z offset", &config->Visuals.view_setup_zoffset, -30, 30, ("%0.f"));


					ImGui::Checkbox("enable event", &config->Misc.eventlogs);
					ImGui::Checkbox("thirdperson##thirdpersonkeyeanble", &config->Visuals.thirdperson_enable);
					ImGui::SameLine(); ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 12);
					ImGui::Hotkey("##thirdpersonsdckey", &config->Visuals.thirdperson, ImVec2(50, 40));
					if (config->Visuals.thirdperson_enable)
					{
						ImGui::SliderFloat("##thirdperson distance", &config->Visuals.thirdperson_dist, 0, 120, ("%0.f"));

					}

					ImGui::Dummy(ImVec2(7, 0));
					ImGui::SameLine();
					if (ImGui::Button("reset windows", ImVec2(100, 18)))
					{
						auto reset_dragging = [&]()->void {
							mouse::i_drag = c_vector2d(0, 0);
							mouse::i_drag = c_vector2d(0, 0);
							mouse::s_drag = c_vector2d(0, 0);
							mouse::d_drag = c_vector2d(0, 0);
							mouse::k_drag = c_vector2d(0, 0);
						};

						reset_dragging();
					}
					ImGui::Dummy(ImVec2(0, 15));
				}
						  break;
					case 1: {
						ImGui::Checkbox("bhop##bunnyhcvop", &config->Misc.bhop);
						ImGui::Checkbox("auto strafe##auvstostrafe", &config->Misc.autostrafe);
						ImGui::Checkbox("airstuck##airstucks", &config->Misc.airstuck_enable);
						if (config->Misc.airstuck_enable)
						{
							ImGui::SameLine();
							ImGui::Hotkey("##jdfxzcdscb", &config->Misc.airstuck_key, ImVec2(50, 40));
						}
						ImGui::Checkbox("slowwalk##slowwalkeanable", &config->Misc.slowwalk);
						if (config->Misc.slowwalk)
						{
							ImGui::SameLine(); ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 12);
							ImGui::Hotkey("##jdxxvzzxdxfxzc", &config->Misc.slowwalk_key, ImVec2(50, 40));
							ImGui::SliderFloat("##slowwalkspeed", &config->Misc.slowwalkspeed, 0, 100, ("%0.f"));
						}
						ImGui::Checkbox("fakeduck##fakeduck", &config->Ragebot.fakeduck_enable);
						if (config->Ragebot.fakeduck_enable) {
							ImGui::SameLine();
							ImGui::Hotkey("##fakeduckkey", &config->Ragebot.fakeduck, ImVec2(50, 40));
						}
					}
						  break;
					case 2: {
					
						ImGui::Checkbox("enable buybot##buybot", &config->Misc.buybot);
					
						if (config->Misc.buybot)
					{
						const char* rifles[] = { "none", "auto", "scout", "awp","ak" };
						const char* pistol[] = { "none", "dual", "p250" , "cz75" , "deagle" ,"" };
						const char* equipment[] = { "none", "kevlar", "full" };


						ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3);

						if (ImGui::BeginCombo("buybot primary", "...", ImVec2(0, 255))) {
							const auto cap = 32;
							ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
							ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
							ImGui::Selectable(("scar20"), &config->Misc.buy_bot_scar20, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
							ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
							ImGui::Selectable(("g3sg1"), &config->Misc.buy_bot_g3sg1, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
							ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
							ImGui::Selectable(("ssg08"), &config->Misc.buy_bot_ssg08, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
							ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
							ImGui::Selectable(("awp"), &config->Misc.buy_bot_awp, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
							ImGui::EndCombo();
						}

						ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3);

						if (ImGui::BeginCombo("buybot secondary", "...", ImVec2(0, 255))) {
							const auto cap = 32;
							ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
							ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
							ImGui::Selectable(("elite"), &config->Misc.buy_bot_elite, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
							ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
							ImGui::Selectable(("p250"), &config->Misc.buy_bot_p250, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
							ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
							ImGui::Selectable(("tec9"), &config->Misc.buy_bot_tec9, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
							ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
							ImGui::Selectable(("DE/R8"), &config->Misc.buy_bot_deg, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
							ImGui::EndCombo();
						}

						ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3);

						if (ImGui::BeginCombo("buybot grenades", "...", ImVec2(0, 255))) {
							const auto cap = 32;
							ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
							ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
							ImGui::Selectable(("molotov"), &config->Misc.buy_bot_molotov, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
							ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
							ImGui::Selectable(("incendiary"), &config->Misc.buy_bot_incgrenade, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
							ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
							ImGui::Selectable(("he"), &config->Misc.buy_bot_hegrenade, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
							ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
							ImGui::Selectable(("smoke"), &config->Misc.buy_bot_smokegrenade, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
							ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
							ImGui::Selectable(("flashbang"), &config->Misc.buy_bot_flashbang, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
							ImGui::EndCombo();
						}
						ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3);

						if (ImGui::BeginCombo("buybot equipment", "...", ImVec2(0, 255))) {
							const auto cap = 32;
							ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
							ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
							ImGui::Selectable(("kelvar"), &config->Misc.buy_bot_vest, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
							ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
							ImGui::Selectable(("kelvar + helm"), &config->Misc.buy_bot_vesthelm, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
							ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
							ImGui::Selectable(("taser"), &config->Misc.buy_bot_taser, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
							ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cap);
							ImGui::Selectable(("defuser"), &config->Misc.buy_bot_defuser, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
							ImGui::EndCombo();
						}
					}
					
					}
						  break;
				}
			} break;
			case 4: {
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);

				ImGui::Checkbox("enable##skinchanger", &config->Skinchanger.Enabled);
				ImGui::Combo(("knife"), &config->Skinchanger.knife_switch, knives, ARRAYSIZE(knives));
				//ImGui::Combo(("gloves"), &config->Skinchanger.gloves_switch, gloves, ARRAYSIZE(gloves));
				//ImGui::InputInt("id##inpu1ttextawp", &_config->Skinchanger.customidknife);  //ImGui::SliderInt("##knifecuystomsa8hfasf7sss8s", &_config->Skinchanger.customidknife, 1.f, 700.f);
				ImGui::Combo(("weapon"), &config->Skinchanger.weapon_switch, guns, ARRAYSIZE(guns));
				ImGui::Text("  ");

				if (config->Skinchanger.weapon_switch == 1) {
					ImGui::InputInt("skin id##awp", &config->Skinchanger.awp.id);
					ImGui::Text("  ");
					ImGui::InputInt("qualtity##awp", &config->Skinchanger.awp.quality);
					ImGui::Text("  ");
					ImGui::InputInt("seed##awp", &config->Skinchanger.awp.seed);
					ImGui::Text("  ");
					ImGui::InputInt("stattrak##awp", &config->Skinchanger.awp.stattrak);
					ImGui::Text("  ");
				}
				if (config->Skinchanger.weapon_switch == 2) {
					ImGui::InputInt("skin id##usp", &config->Skinchanger.usp.id);
					ImGui::Text("  ");
					ImGui::InputInt("qualtity##usp", &config->Skinchanger.usp.quality);
					ImGui::Text("  ");
					ImGui::InputInt("seed##usp", &config->Skinchanger.usp.seed);
					ImGui::Text("  ");
					ImGui::InputInt("stattrak##usp", &config->Skinchanger.usp.stattrak);
					ImGui::Text("  ");
				}
				if (config->Skinchanger.weapon_switch == 3) {
					ImGui::InputInt("skin id##p2k", &config->Skinchanger.p2k.id);
					ImGui::Text("  ");
					ImGui::InputInt("qualtity##p2k", &config->Skinchanger.p2k.quality);
					ImGui::Text("  ");
					ImGui::InputInt("seed##p2k", &config->Skinchanger.p2k.seed);
					ImGui::Text("  ");
					ImGui::InputInt("stattrak##p2k", &config->Skinchanger.p2k.stattrak);
					ImGui::Text("  ");
				}
				if (config->Skinchanger.weapon_switch == 4) {
					ImGui::InputInt("skin id##glock", &config->Skinchanger.glock.id);
					ImGui::Text("  ");
					ImGui::InputInt("qualtity##glock", &config->Skinchanger.glock.quality);
					ImGui::Text("  ");
					ImGui::InputInt("seed##glock", &config->Skinchanger.glock.seed);
					ImGui::Text("  ");
					ImGui::InputInt("stattrak##glock", &config->Skinchanger.glock.stattrak);
					ImGui::Text("  ");
				}
				if (config->Skinchanger.weapon_switch == 5) {
					ImGui::InputInt("skin id##deg", &config->Skinchanger.deg.id);
					ImGui::Text("  ");
					ImGui::InputInt("qualtity##deg", &config->Skinchanger.deg.quality);
					ImGui::Text("  ");
					ImGui::InputInt("seed##deg", &config->Skinchanger.deg.seed);
					ImGui::Text("  ");
					ImGui::InputInt("stattrak##deg", &config->Skinchanger.deg.stattrak);
					ImGui::Text("  ");
				}
				if (config->Skinchanger.weapon_switch == 6) {
					ImGui::InputInt("skin id##revol", &config->Skinchanger.revol.id);
					ImGui::Text("  ");
					ImGui::InputInt("qualtity##revol", &config->Skinchanger.revol.quality);
					ImGui::Text("  ");
					ImGui::InputInt("seed##revol", &config->Skinchanger.revol.seed);
					ImGui::Text("  ");
					ImGui::InputInt("stattrak##revol", &config->Skinchanger.revol.stattrak);
					ImGui::Text("  ");
				}
				if (config->Skinchanger.weapon_switch == 7) {
					ImGui::InputInt("skin id##ssg", &config->Skinchanger.ssg.id);
					ImGui::Text("  ");
					ImGui::InputInt("qualtity##ssg", &config->Skinchanger.ssg.quality);
					ImGui::Text("  ");
					ImGui::InputInt("seed##ssg", &config->Skinchanger.ssg.seed);
					ImGui::Text("  ");
					ImGui::InputInt("stattrak##ssg", &config->Skinchanger.ssg.stattrak);
					ImGui::Text("  ");
				}
				if (config->Skinchanger.weapon_switch == 8) {
					ImGui::InputInt("skin id##dual", &config->Skinchanger.dual.id);
					ImGui::Text("  ");
					ImGui::InputInt("qualtity##dual", &config->Skinchanger.dual.quality);
					ImGui::Text("  ");
					ImGui::InputInt("seed##dual", &config->Skinchanger.dual.seed);
					ImGui::Text("  ");
					ImGui::InputInt("stattrak##dual", &config->Skinchanger.dual.stattrak);
					ImGui::Text("  ");
				}
				if (config->Skinchanger.weapon_switch == 9) {
					ImGui::InputInt("skin id##p250", &config->Skinchanger.p250.id);
					ImGui::Text("  ");
					ImGui::InputInt("qualtity##p250", &config->Skinchanger.p250.quality);
					ImGui::Text("  ");
					ImGui::InputInt("seed##p250", &config->Skinchanger.p250.seed);
					ImGui::Text("  ");
					ImGui::InputInt("stattrak##p250", &config->Skinchanger.p250.stattrak);
					ImGui::Text("  ");
				}
				if (config->Skinchanger.weapon_switch == 10) {
					ImGui::InputInt("skin id##cz75", &config->Skinchanger.cz75.id);
					ImGui::Text("  ");
					ImGui::InputInt("qualtity##cz75", &config->Skinchanger.cz75.quality);
					ImGui::Text("  ");
					ImGui::InputInt("seed##cz75", &config->Skinchanger.cz75.seed);
					ImGui::Text("  ");
					ImGui::InputInt("stattrak##cz75", &config->Skinchanger.cz75.stattrak);
					ImGui::Text("  ");
				}
				if (config->Skinchanger.weapon_switch == 11) {
					ImGui::InputInt("skin id##five7", &config->Skinchanger.five7.id);
					ImGui::Text("  ");
					ImGui::InputInt("qualtity##five7", &config->Skinchanger.five7.quality);
					ImGui::Text("  ");
					ImGui::InputInt("seed##five7", &config->Skinchanger.five7.seed);
					ImGui::Text("  ");
					ImGui::InputInt("stattrak##five7", &config->Skinchanger.five7.stattrak);
					ImGui::Text("  ");
				}
				if (config->Skinchanger.weapon_switch == 12) {
					ImGui::InputInt("skin id##tec9", &config->Skinchanger.tec9.id);
					ImGui::Text("  ");
					ImGui::InputInt("qualtity##tec9", &config->Skinchanger.tec9.quality);
					ImGui::Text("  ");
					ImGui::InputInt("seed##tec9", &config->Skinchanger.tec9.seed);
					ImGui::Text("  ");
					ImGui::InputInt("stattrak##tec9", &config->Skinchanger.tec9.stattrak);
					ImGui::Text("  ");
				}
				if (config->Skinchanger.weapon_switch == 13) {
					ImGui::InputInt("skin id##nova", &config->Skinchanger.nova.id);
					ImGui::Text("  ");
					ImGui::InputInt("qualtity##nova", &config->Skinchanger.nova.quality);
					ImGui::Text("  ");
					ImGui::InputInt("seed##nova", &config->Skinchanger.nova.seed);
					ImGui::Text("  ");
					ImGui::InputInt("stattrak##nova", &config->Skinchanger.nova.stattrak);
					ImGui::Text("  ");
				}
				if (config->Skinchanger.weapon_switch == 14) {
					ImGui::InputInt("skin id##xm1014", &config->Skinchanger.xm1014.id);
					ImGui::Text("  ");
					ImGui::InputInt("qualtity##xm1014", &config->Skinchanger.xm1014.quality);
					ImGui::Text("  ");
					ImGui::InputInt("seed##xm1014", &config->Skinchanger.xm1014.seed);
					ImGui::Text("  ");
					ImGui::InputInt("stattrak##xm1014", &config->Skinchanger.xm1014.stattrak);
					ImGui::Text("  ");
				}
				if (config->Skinchanger.weapon_switch == 15) {
					ImGui::InputInt("skin id##mag7", &config->Skinchanger.mag7.id);
					ImGui::Text("  ");
					ImGui::InputInt("qualtity##mag7", &config->Skinchanger.mag7.quality);
					ImGui::Text("  ");
					ImGui::InputInt("seed##mag7", &config->Skinchanger.mag7.seed);
					ImGui::Text("  ");
					ImGui::InputInt("stattrak##mag7", &config->Skinchanger.mag7.stattrak);
					ImGui::Text("  ");
				}
				if (config->Skinchanger.weapon_switch == 16) {
					ImGui::InputInt("skin id##sawed", &config->Skinchanger.sawed.id);
					ImGui::Text("  ");
					ImGui::InputInt("qualtity##sawed", &config->Skinchanger.sawed.quality);
					ImGui::Text("  ");
					ImGui::InputInt("seed##sawed", &config->Skinchanger.sawed.seed);
					ImGui::Text("  ");
					ImGui::InputInt("stattrak##sawed", &config->Skinchanger.sawed.stattrak);
					ImGui::Text("  ");
				}
				if (config->Skinchanger.weapon_switch == 17) {
					ImGui::InputInt("skin id##negev", &config->Skinchanger.negev.id);
					ImGui::Text("  ");
					ImGui::InputInt("qualtity##negev", &config->Skinchanger.negev.quality);
					ImGui::Text("  ");
					ImGui::InputInt("seed##negev", &config->Skinchanger.negev.seed);
					ImGui::Text("  ");
					ImGui::InputInt("stattrak##negev", &config->Skinchanger.negev.stattrak);
					ImGui::Text("  ");
				}
				if (config->Skinchanger.weapon_switch == 18) {
					ImGui::InputInt("skin id##m249", &config->Skinchanger.m249.id);
					ImGui::Text("  ");
					ImGui::InputInt("qualtity##m249", &config->Skinchanger.m249.quality);
					ImGui::Text("  ");
					ImGui::InputInt("seed##m249", &config->Skinchanger.m249.seed);
					ImGui::Text("  ");
					ImGui::InputInt("stattrak##m249", &config->Skinchanger.m249.stattrak);
					ImGui::Text("  ");
				}
				if (config->Skinchanger.weapon_switch == 19) {
					ImGui::InputInt("skin id##mp9", &config->Skinchanger.mp9.id);
					ImGui::Text("  ");
					ImGui::InputInt("qualtity##mp9", &config->Skinchanger.mp9.quality);
					ImGui::Text("  ");
					ImGui::InputInt("seed##mp9", &config->Skinchanger.mp9.seed);
					ImGui::Text("  ");
					ImGui::InputInt("stattrak##mp9", &config->Skinchanger.mp9.stattrak);
					ImGui::Text("  ");
				}
				if (config->Skinchanger.weapon_switch == 20) {
					ImGui::InputInt("skin id##mac10", &config->Skinchanger.mac10.id);
					ImGui::Text("  ");
					ImGui::InputInt("qualtity##mac10", &config->Skinchanger.mac10.quality);
					ImGui::Text("  ");
					ImGui::InputInt("seed##mac10", &config->Skinchanger.mac10.seed);
					ImGui::Text("  ");
					ImGui::InputInt("stattrak##mac10", &config->Skinchanger.mac10.stattrak);
					ImGui::Text("  ");
				}
				if (config->Skinchanger.weapon_switch == 21) {
					ImGui::InputInt("skin id##mp7", &config->Skinchanger.mp7.id);
					ImGui::Text("  ");
					ImGui::InputInt("qualtity##mp7", &config->Skinchanger.mp7.quality);
					ImGui::Text("  ");
					ImGui::InputInt("seed##mp7", &config->Skinchanger.mp7.seed);
					ImGui::Text("  ");
					ImGui::InputInt("stattrak##mp7", &config->Skinchanger.mp7.stattrak);
					ImGui::Text("  ");
				}
				if (config->Skinchanger.weapon_switch == 22) {
					ImGui::InputInt("skin id##ump", &config->Skinchanger.ump.id);
					ImGui::Text("  ");
					ImGui::InputInt("qualtity##ump", &config->Skinchanger.ump.quality);
					ImGui::Text("  ");
					ImGui::InputInt("seed##ump", &config->Skinchanger.ump.seed);
					ImGui::Text("  ");
					ImGui::InputInt("stattrak##ump", &config->Skinchanger.ump.stattrak);
					ImGui::Text("  ");
				}
				if (config->Skinchanger.weapon_switch == 23) {
					ImGui::InputInt("skin id##p90", &config->Skinchanger.p90.id);
					ImGui::Text("  ");
					ImGui::InputInt("qualtity##p90", &config->Skinchanger.p90.quality);
					ImGui::Text("  ");
					ImGui::InputInt("seed##p90", &config->Skinchanger.p90.seed);
					ImGui::Text("  ");
					ImGui::InputInt("stattrak##p90", &config->Skinchanger.p90.stattrak);
					ImGui::Text("  ");
				}
				if (config->Skinchanger.weapon_switch == 24) {
					ImGui::InputInt("skin id##ppbizon", &config->Skinchanger.ppbizon.id);
					ImGui::Text("  ");
					ImGui::InputInt("qualtity##ppbizon", &config->Skinchanger.ppbizon.quality);
					ImGui::Text("  ");
					ImGui::InputInt("seed##ppbizon", &config->Skinchanger.ppbizon.seed);
					ImGui::Text("  ");
					ImGui::InputInt("stattrak##ppbizon", &config->Skinchanger.ppbizon.stattrak);
					ImGui::Text("  ");
				}
				if (config->Skinchanger.weapon_switch == 25) {
					ImGui::InputInt("skin id##galil", &config->Skinchanger.galil.id);
					ImGui::Text("  ");
					ImGui::InputInt("qualtity##galil", &config->Skinchanger.galil.quality);
					ImGui::Text("  ");
					ImGui::InputInt("seed##galil", &config->Skinchanger.galil.seed);
					ImGui::Text("  ");
					ImGui::InputInt("stattrak##galil", &config->Skinchanger.galil.stattrak);
					ImGui::Text("  ");
				}
				if (config->Skinchanger.weapon_switch == 26) {
					ImGui::InputInt("skin id##famas", &config->Skinchanger.famas.id);
					ImGui::Text("  ");
					ImGui::InputInt("qualtity##famas", &config->Skinchanger.famas.quality);
					ImGui::Text("  ");
					ImGui::InputInt("seed##famas", &config->Skinchanger.famas.seed);
					ImGui::Text("  ");
					ImGui::InputInt("stattrak##famas", &config->Skinchanger.famas.stattrak);
					ImGui::Text("  ");
				}
				if (config->Skinchanger.weapon_switch == 27) {
					ImGui::InputInt("skin id##m4a4", &config->Skinchanger.m4a4.id);
					ImGui::Text("  ");
					ImGui::InputInt("qualtity##m4a4", &config->Skinchanger.m4a4.quality);
					ImGui::Text("  ");
					ImGui::InputInt("seed##m4a4", &config->Skinchanger.m4a4.seed);
					ImGui::Text("  ");
					ImGui::InputInt("stattrak##m4a4", &config->Skinchanger.m4a4.stattrak);
					ImGui::Text("  ");
				}
				if (config->Skinchanger.weapon_switch == 28) {
					ImGui::InputInt("skin id##m4a1", &config->Skinchanger.m4a1.id);
					ImGui::Text("  ");
					ImGui::InputInt("qualtity##m4a1", &config->Skinchanger.m4a1.quality);
					ImGui::Text("  ");
					ImGui::InputInt("seed##m4a1", &config->Skinchanger.m4a1.seed);
					ImGui::Text("  ");
					ImGui::InputInt("stattrak##m4a1", &config->Skinchanger.m4a1.stattrak);
					ImGui::Text("  ");
				}
				if (config->Skinchanger.weapon_switch == 29) {
					ImGui::InputInt("skin id##ak47", &config->Skinchanger.ak47.id);
					ImGui::Text("  ");
					ImGui::InputInt("qualtity##ak47", &config->Skinchanger.ak47.quality);
					ImGui::Text("  ");
					ImGui::InputInt("seed##ak47", &config->Skinchanger.ak47.seed);
					ImGui::Text("  ");
					ImGui::InputInt("stattrak##ak47", &config->Skinchanger.ak47.stattrak);
					ImGui::Text("  ");
				}
				if (config->Skinchanger.weapon_switch == 30) {
					ImGui::InputInt("skin id##aug", &config->Skinchanger.aug.id);
					ImGui::Text("  ");
					ImGui::InputInt("qualtity##aug", &config->Skinchanger.aug.quality);
					ImGui::Text("  ");
					ImGui::InputInt("seed##aug", &config->Skinchanger.aug.seed);
					ImGui::Text("  ");
					ImGui::InputInt("stattrak##aug", &config->Skinchanger.aug.stattrak);
					ImGui::Text("  ");
				}
				if (config->Skinchanger.weapon_switch == 31) {
					ImGui::InputInt("skin id##sg553", &config->Skinchanger.sg553.id);
					ImGui::Text("  ");
					ImGui::InputInt("qualtity##sg553", &config->Skinchanger.sg553.quality);
					ImGui::Text("  ");
					ImGui::InputInt("seed##sg553", &config->Skinchanger.sg553.seed);
					ImGui::Text("  ");
					ImGui::InputInt("stattrak##sg553", &config->Skinchanger.sg553.stattrak);
					ImGui::Text("  ");
				}
				if (config->Skinchanger.weapon_switch == 32) {
					ImGui::InputInt("skin id##scar20", &config->Skinchanger.scar20.id);
					ImGui::Text("  ");
					ImGui::InputInt("qualtity##scar20", &config->Skinchanger.scar20.quality);
					ImGui::Text("  ");
					ImGui::InputInt("seed##scar20", &config->Skinchanger.scar20.seed);
					ImGui::Text("  ");
					ImGui::InputInt("stattrak##scar20", &config->Skinchanger.scar20.stattrak);
					ImGui::Text("  ");
				}
				if (config->Skinchanger.weapon_switch == 33) {
					ImGui::InputInt("skin idd##g3sg1id", &config->Skinchanger.g3sg1.id);
					ImGui::Text("  ");
					ImGui::InputInt("qualtity##g3sg1", &config->Skinchanger.g3sg1.quality);
					ImGui::Text("  ");
					ImGui::InputInt("seed##g3sg1", &config->Skinchanger.g3sg1.seed);
					ImGui::Text("  ");
					ImGui::InputInt("stattrak##g3sg1", &config->Skinchanger.g3sg1.stattrak);
					ImGui::Text("  ");
				}
				if (config->Skinchanger.weapon_switch == 34) {
					ImGui::InputInt("skin id##mp5", &config->Skinchanger.mp5.id);
					ImGui::Text("  ");
					ImGui::InputInt("qualtity##mp5", &config->Skinchanger.mp5.quality);
					ImGui::Text("  ");
					ImGui::InputInt("seed##mp5", &config->Skinchanger.mp5.seed);
					ImGui::Text("  ");
					ImGui::InputInt("stattrak##mp5", &config->Skinchanger.mp5.stattrak);
					ImGui::Text("  ");
				}
				if (config->Skinchanger.weapon_switch == 35) {
					ImGui::InputInt("skin id##knife", &config->Skinchanger.knife.id);
					ImGui::Text("  ");
					ImGui::InputInt("qualtity##knife", &config->Skinchanger.knife.quality);
					ImGui::Text("  ");
					ImGui::InputInt("seed##knife", &config->Skinchanger.knife.seed);
					ImGui::Text("  ");
					ImGui::InputInt("stattrak##knife", &config->Skinchanger.knife.stattrak);
					ImGui::Text("  ");
				}
				//if (config->Skinchanger.weapon_switch == 36) {
				//	ImGui::InputInt("skin id##gloves", &config->Skinchanger.gloves.id);
				//	ImGui::Text("  ");
				//	ImGui::InputInt("qualtity##gloves", &config->Skinchanger.gloves.quality);
				//	ImGui::Text("  ");
				//	ImGui::InputInt("seed##gloves", &config->Skinchanger.gloves.seed);
				//	ImGui::Text("  ");
				//	ImGui::InputInt("stattrak##gloves", &config->Skinchanger.gloves.stattrak);
				//	ImGui::Text("  ");
				//}
				ImGui::Dummy(ImVec2(6, 0));
				ImGui::SameLine();

				if (ImGui::Button("update##updateskins", ImVec2(100, 18)))
				{
					config->Skinchanger.updated = true;
					client_state->request_full_update();
					config->Skinchanger.updated = false;
				}
			}break;
			}
			ImGui::EndChild();
			ImGui::SetCursorPos(ImVec2(3, 3));
			ImVec2 p = ImGui::GetCursorScreenPos();
			ImGui::GetWindowDrawList()->AddRect(ImVec2(p.x, p.y + 1), ImVec2(p.x + ImGui::GetWindowWidth() - 6, p.y + 1), ImColor(config->Misc.menucolor[0], config->Misc.menucolor[1], config->Misc.menucolor[2], 240.f), 0.f, -1, 1);
			ImGui::End();
			ImGui::PopFont();
		}
	}
}