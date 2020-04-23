#include "c_chams.h"
#include "../security/string_obfuscation.h"
#include "../sdk/c_material_system.h"
#include "../sdk/c_render_view.h"
#include "../utils/c_config.h"
#include "../sdk/c_client_entity_list.h"
#include "c_animation_system.h"
#include "../utils/math.h"
#include "../sdk/c_client_state.h"
#include "../sdk/c_prediction.h"
#include "../menu/c_menu.h"
#include "../sdk/c_engine_trace.h"
#include "../sdk/c_glow_object_manager.h"
#include "c_antiaim.h"
#include "../sdk/c_weapon_system.h"

#define ARRAY_TO_CCOLOR(VAR) c_color(VAR[0] * 255, VAR[1] * 255, VAR[2] * 255, VAR[3] * 255)
c_chams::c_chams() : current_player(nullptr), current_matrix(nullptr), second_pass(false), alpha(255), direction(false) { }
c_chams chams;
float timer;

void c_chams::latch_timer()
{
	if (!engine_client()->is_ingame() || !engine_client()->is_connected())
		return;

	if (alpha == 0 || alpha == 255)
		direction = !direction;

	linear_fade(alpha, 0, 255, 255 / 2.5f, direction);
}

void c_chams::draw_players()
{
	if (!engine_client()->is_ingame() || !engine_client()->is_connected())
		return;

	client_entity_list()->for_each_player_fixed_z_order([&](c_cs_player* player) -> void
	{
		if (!player)
			return;

		if (!player->is_alive())
			return;
		if (player->is_dormant())
			return;

		player->draw_model(0x1, 255);
	
	});
}

bool c_chams::get_simple_backtrack_matrix(c_cs_player* player, matrix3x4* out)
{
	if (!engine_client()->is_ingame() || !engine_client()->is_connected())
		return false;

	//auto range = 1;//config->legit.backtrack / 100.f;

	//if (config->rage.enabled)
		//range = 1.f;

	const auto last = animation_system->get_oldest_animation(player);

	if (!last || !last.has_value())
		return false;

	memcpy(out, last.value()->bones, sizeof(matrix3x4[128]));
}
bool c_chams::get_backtrack_matrix(c_cs_player* player, matrix3x4* out)
{
	if (!engine_client()->is_ingame() || !engine_client()->is_connected())
		return false;


	auto range = 1;//config->legit.backtrack / 100.f;

	if (config->Ragebot.enable)
		range = 1.f;

	const auto last = animation_system->get_intermediate_animations(player);

	if (!last || !last.has_value())
		return false;

	const auto& first_invalid = last.value().first;
	const auto& last_valid = last.value().second;

	if ((first_invalid->origin - player->get_abs_origin()).length() < 7.5f)
		return false;

	if (first_invalid->dormant)
		return false;

	if (last_valid->sim_time - first_invalid->sim_time > 0.5f)
		return false;

	const auto next = last_valid->origin;
	if (!prediction() || !prediction()->get_unpredicted_globals())return false;// nullptr 
	const auto curtime = prediction()->get_unpredicted_globals()->curtime;

	auto delta = 1.f - (curtime - last_valid->interp_time) / (last_valid->sim_time - first_invalid->sim_time);
	if (delta < 0.f || delta > 1.f)
		last_valid->interp_time = curtime;

	delta = 1.f - (curtime - last_valid->interp_time) / (last_valid->sim_time - first_invalid->sim_time);

	const auto lerp = math::interpolate(next, first_invalid->origin, std::clamp(delta, 0.f, 1.f));

	matrix3x4 ret[128];
	memcpy(ret, first_invalid->bones, sizeof(matrix3x4[128]));

	for (size_t i{}; i < 128; ++i)
	{
		const auto matrix_delta = math::matrix_get_origin(first_invalid->bones[i]) - first_invalid->origin;
		math::matrix_set_origin(matrix_delta + lerp, ret[i]);
	}

	memcpy(out, ret, sizeof(matrix3x4[128]));
	return true;
}


bool loaded_chams = false;
c_material* c_chams::GetMat(int material) {

	if (!engine_client()->is_ingame() || !engine_client()->is_connected())
		return false;

	if (!loaded_chams) {
		std::ofstream("csgo\\materials\\dsv_textured.vmt") << R"("VertexLitGeneric"
	{
		"$basetexture"	"vgui/white"
		"$model"		"1"
		"$flat"			"0"
		"$nocull"		"1"
		"$halflambert"	"1"
		"$nofog"		"1"
		"$ignorez"		"0"
		"$znearer"		"0"
		"$wireframe"	"0"
	})";
			std::ofstream("csgo\\materials\\dsv_flat.vmt") << R"("UnlitGeneric"
	{
		"$basetexture"	"vgui/white"
		"$model"		"1"
		"$flat"			"1"
		"$nocull"		"1"
		"$selfillum"	"1"
		"$halflambert"	"1"
		"$nofog"		"1"
		"$ignorez"		"0"
		"$znearer"		"0"
		"$wireframe"	"0"
	})";
			std::ofstream("csgo\\materials\\dsv_metallic.vmt") << R"("VertexLitGeneric" 
	{
		"$basetexture"	"vgui/white"
		"$envmap"       "env_cubemap"
		"$model"		"1"
		"$flat"			"0"
		"$nocull"		"1"
		"$halflambert"	"1"
		"$nofog"		"1"
		"$ignorez"		"0"
		"$znearer"		"0"
		"$wireframe"	"0"
	})";
			std::ofstream("csgo\\materials\\dsv_animated.vmt") << "VertexLitGeneric { $envmap editor/cube_vertigo $envmapcontrast 1 $envmaptint \"[.7 .7 .7]\" $basetexture dev/zone_warning proxies { texturescroll { texturescrollvar $basetexturetransform texturescrollrate 0.6 texturescrollangle 90 } } }";
			std::ofstream("csgo\\materials\\skeet_glow.vmt") << R"("VertexLitGeneric" {
		"$basetexture" "models/effects/cube_white"
		"$additive"                    "1"
		"$envmap"                    "models/effects/cube_white"
		"$envmaptint"                "[1.0 1.0. 1.0]"
		"$envmapfresnel"            "1.0"
		"$envmapfresnelminmaxexp"    "[0.0 1.0 2.0]"
		"$alpha"                    "0.7"
	})";
			std::ofstream("csgo\\materials\\dsv_bumpy.vmt") << R"("VertexLitGeneric" {
		"$basetexture" "VGUI/white_additive"
		"$bumpmap" "de_nuke/hr_nuke/pool_water_normals_002"
		"$bumptransform" "center 0.5 0.5 scale 0.25 0.25 rotate 0.0 translate 0.0 0.0"
		"$ignorez" "1"
		"$nofog" "1"
		"$model" "1"
		"$color2" "[1.0, 1.0, 1.0]"
		"$halflambert" "1"
		"$envmap" "env_cubemap"
		"$envmaptint" "[0.1 0.1 0.1]"
		"$envmapfresnel" "1.0"
		"$envmapfresnelminmaxexp" "[1.0, 1.0, 1.0]"
		"$phong" "1"
		"$phongexponent" "100"
		"$phongboost" "2.0"
		"$phongfresnelranges" "[1.0, 1.0, 1.0]"
		"$rimlight" "1"
		"$rimlightexponent" "2.0"
		"$rimlightboost" "1.0"
    
		"Proxies"
		{
			"TextureScroll"
			{
				"textureScrollVar" "$bumptransform"
				"textureScrollRate" "0.25"
				"textureScrollAngle" "0.0"
			}
		}
	})";
			std::ofstream("csgo\\materials\\nigga0.vmt") << R"("UnlitGeneric" {
		// Original shader: SphericalEnvMap
		"$envmap" "dev/dev_envmap"
		//"$envmapsphere" 1
		"$basetexture"       "dev/screenhighlight_pulse"
		"$multiply"          "1"
		"proxies"    {
			"texturescroll"    {
				"texturescrollvar"   "$basetexturetransform"
				"texturescrollrate"  "0.6"
				"texturescrollangle" "90"
			}
		}
		})";

			std::ofstream("csgo/materials/outline.vmt") << R"#("VertexLitGeneric" {
   		   "$additive" "1"
		   "$envmap" "models/effects/cube_white"
		   "$envmaptint" "[1 1 1]"
		   "$envmapfresnel" "1"
		   "$envmapfresnelminmaxexp" "[0 1 2]"
		   "$alpha" "0.8"
		   "$basetexture" "models/inventory_items/dogtags/dogtags_lightray"
			"$localTeamNumber" "0"
			//    "$localBrightnessScale" "0.68"

			"proxies"
			 {
			   "GetTeamNumber"
			   {
				   "resultVar" "$localTeamNumber"
			 }
	
			"select"
			{
				"srcVar1"       "$localTeamNumber"
				"resultVar"     "$color"

				//"default"       "[1.000000 1.000000 1.000000]"
				//"2"             "[1.000000 0.800000 0.200000]"  // T  255 204  51
				// //"3"             "[0.349020 0.615686 0.882353]"  // CT  89 157 225
				//"3"             "[0.395556 0.697975 1.000000]"  // scale up to match brightness of t color better

				// pre-multiply with tuned brightness scale
				"default"       "[0.680000 0.680000 0.680000]"
				"2"             "[0.680000 0.544000 0.136000]"
				"3"             "[0.268978 0.474263 0.680000]"
			}

	//      "multiply"
	//      {
	//            "srcVar1"       "$color"
	//            "srcVar2"       "$localBrightnessScale"
	//            "resultVar"     "$color"
	//       }
	}
	})#";

			std::ofstream("csgo/materials/outline2.vmt") << R"#("VertexLitGeneric" {
 
		"$additive" "1"
		"$envmap" "models/effects/cube_white"
		"$envmaptint" "[1 1 1]"
		"$envmapfresnel" "1"
		"$envmapfresnelminmaxexp" "[0 1 2]"
		"$alpha" "0.8"
	})#";

			std::ofstream("csgo\\materials\\frames.vmt") << R"#("unlitgeneric" {
		"$basetexture" "dev/zone_warning"
		"$multiply" "1"
		"proxies"
		{
			"texturescroll"
			{
				"texturescrollvar"   "$basetexturetransform"
				"texturescrollrate"  "0.6"
				"texturescrollangle" "90"
			}
		}
	}
	)#";
			std::ofstream("csgo\\materials\\palace.vmt") << R"#("UnlitGeneric" {
		// Original shader: SphericalEnvMap
		"$envmap" "dev/dev_envmap"
		//"$envmapsphere" 1
		"$basetexture"       "dev/screenhighlight_pulse"
		"$multiply"          "1"
		"proxies"    {
			"texturescroll"    {
				"texturescrollvar"   "$basetexturetransform"
				"texturescrollrate"  "0.6"
				"texturescrollangle" "90"
			}
		}
		}
		)#";

			std::ofstream("csgo\\materials\\viberent.vmt") << R"#("VertexLitGeneric" {

		"$basetexture" "VGUI/white_additive"
		"$bumpmap" "de_nuke/hr_nuke/pool_water_normals_002"
		"$bumptransform" "center 0.5 0.5 scale 0.25 0.25 rotate 0.0 translate 0.0 0.0"
		"$ignorez" "0"
		"$nofog" "1"
		"$model" "1"
		"$color2" "[1.0, 1.0, 1.0]"
		"$halflambert" "1"
		"$envmap" "env_cubemap"
		"$envmaptint" "[0.1 0.1 0.1]"
		"$envmapfresnel" "1.0"
		"$envmapfresnelminmaxexp" "[1.0, 1.0, 1.0]"
		"$phong" "1"
		"$phongexponent" "1024"
		"$phongboost" "4.0"
		"$phongfresnelranges" "[1.0, 1.0, 1.0]"
		"$rimlight" "1"
		"$rimlightexponent" "4.0"
		"$rimlightboost" "2.0"
    
		"Proxies"
		{
			"TextureScroll"
			{
				"textureScrollVar" "$bumptransform"
				"textureScrollRate" "0.25"
				"textureScrollAngle" "0.0"
			}
		}
	}
	)#";

			std::ofstream("csgo\\materials\\jeub.vmt") << R"("UnlitGeneric" {
		// Original shader: SphericalEnvMap
		"$envmap" "dev/dev_envmap"
		//"$envmapsphere" 1
		"$basetexture"       "models/error/new light1"
		"$multiply"          "1"
		"proxies"    {
			"texturescroll"    {
				"texturescrollvar"   "$basetexturetransform"
				"texturescrollrate"  "0.6"
				"texturescrollangle" "90"
			}
		}
		})";


		std::ofstream("csgo\\materials\\chams_flat.vmt") << R"("UnlitGeneric" {
        
		    "$basetexture" "vgui/white_additive"
	        "$ignorez"      "1"
	        "$nofog"        "1"
	        "$model"        "1"
	        "$nocull"       "0"
	        "$selfillum"    "1"
	        "$halflambert"  "1"
	        "$znearer"      "0"
	        "$flat"         "1"
	    })";

		std::ofstream("csgo\\materials\\galaxy1.vmt") << R"("UnlitGeneric" {      
		    "$basetexture" "skybox/sky_descentbk"
			"$nofog" "1"
			"$ignorez" "1"
	    })";

		std::ofstream("csgo\\materials\\galaxy2.vmt") << R"("UnlitGeneric" {      
		    "$basetexture" "skybox/sky_descentdn"
			"$nofog" "1"
			"$ignorez" "1"
	    })";

		std::ofstream("csgo\\materials\\galaxy3.vmt") << R"("UnlitGeneric" {      
		    "$basetexture" "skybox/sky_descentft"
			"$nofog" "1"
			"$ignorez" "1"
	    })";

		std::ofstream("csgo\\materials\\galaxy4.vmt") << R"("UnlitGeneric" {      
		    "$basetexture" "skybox/sky_descentlf"
			"$nofog" "1"
			"$ignorez" "1"
	    })";

		std::ofstream("csgo\\materials\\galaxy5.vmt") << R"("UnlitGeneric" {      
		    "$basetexture" "skybox/sky_descentrt"
			"$nofog" "1"
			"$ignorez" "1"
	    })";

		std::ofstream("csgo\\materials\\galaxy6.vmt") << R"("UnlitGeneric" {      
		    "$basetexture" "skybox/sky_descentup"
			"$nofog" "1"
			"$ignorez" "1"
	    })";

		std::ofstream("csgo\\materials\\desync_logo1.vmt") << R"("LightmappedGeneric" {      
		    "$basetexture" "skybox/desync_logo"
			"$nofog" "1"
			"$ignorez" "1"
	    })";

		loaded_chams = true;
	}


	switch (material) {
	default:
	case 0: /* textured */
		return find_mat("dsv_textured");
	case 1: /* flat */
		return find_mat("chams_flat");
	case 2: /* metallic */
		return find_mat("dsv_metallic");
	case 3: /* animated */
		return find_mat("dsv_animated");
	case 4: /* glowing */
		return find_mat("dev/glow_armsrace");
	case 5: /* ray */
		return find_mat("models/inventory_items/dogtags/dogtags_lightray");
	case 6: /* glass */
		return find_mat("models/inventory_items/cologne_prediction/cologne_prediction_glass");
	case 7: /* plastic */
		return find_mat("models/inventory_items/trophy_majors/gloss");
	case 8: /* Shiny metallic */
		return find_mat("models/gibs/glass/glass");
	case 9: /* Crystal clear */
		return find_mat("models/inventory_items/trophy_majors/crystal_clear");
	case 10: // Matrix
		return find_mat("models/inventory_items/music_kit/darude_01/mp3_detail");
	case 11: // Wild
		return find_mat("models/props_foliage/urban_tree03_branches");
	case 12: // Galaxy
		return find_mat("models/inventory_items/dreamhack_trophies/dreamhack_star_blur");
	case 13: // Net
		return find_mat("models/props_shacks/fishing_net01");
	case 14: // Matte
		return find_mat("models/inventory_items/trophy_majors/gold_dust");
	case 15: // Platinum
		return find_mat("models/player/ct_fbi/ct_fbi_glass");
	case 16: /* bubbly */
		return find_mat("outline2");
	case 17: /* dsv_bumpy */
		return find_mat("viberent");
	}
}

float alphas = 100.f;
bool alpha_direction = false;


inline void do_wireframe(c_material*& mat, bool should_wireframe)
{
	mat->set_material_var_flag(material_var_wireframe, should_wireframe);
}

void c_chams::player_chams(const std::function<void()> original, Menu22::visaul::esp& conf, bool draw_fake, bool scope_blend)
{
	if (conf.material_type == 0)
		return;

	auto mat = GetMat(conf.material_type - 1);

	if (!mat)
		return;

	c_color c = c_color(conf.color[0] * 255, conf.color[1] * 255, conf.color[2] * 255, conf.color[3] * 255);

	if (conf.material_type - 1 == 18 || conf.material_type - 1 == 14 || conf.material_type - 1 == 9 || conf.material_type - 1 == 7 || conf.material_type - 1 == 4 || conf.material_type - 1 == 15 || conf.material_type - 1 == 16 || conf.material_type - 1 == 8)
		c.alpha = conf.gloss[3] * 255;

	c_color xqz_c = c_color(conf.xqz_color[0] * 255, conf.xqz_color[1] * 255, conf.xqz_color[2] * 255, conf.xqz_color[3] * 255);

	if (conf.material_type - 1 == 18 || conf.material_type - 1 == 14 || conf.material_type - 1 == 9 || conf.material_type - 1 == 7 || conf.material_type - 1 == 4 || conf.material_type - 1 == 15 || conf.material_type - 1 == 16 || conf.material_type - 1 == 8)
		xqz_c.alpha = conf.xqz_gloss[3] * 255;

	bool preserve = conf.preserve_model;
	bool pulse = conf.pulse;
	bool wireframe = conf.wireframe;

	const auto alpha = pulse ? instance()->alpha : scope_blend ? std::clamp(c.alpha / 2, 0, 255) : c.alpha;
	const auto xqz_alpha = pulse ? instance()->alpha : scope_blend ? std::clamp(xqz_c.alpha / 2, 0, 255) : xqz_c.alpha;

	if (!mat->is_error_material())
	{
		mat->incrementreferencecount();

		if (preserve)
		{
			modulate(c_color( 255,  255,  255,  255), conf, mat);
			render_view()->set_blend((preserve ? 255 : alpha) / 255.f);
			original();
		}

		/*modulate(c_color(conf.color[0] * 255, conf.color[1] * 255, conf.color[2] * 255, c.alpha), conf, mat);
		do_wireframe(mat, wireframe);*/

		if (conf.xqz)
		{
			set_ignorez(true, mat);
			modulate(c_color(conf.xqz_color[0] * 255, conf.xqz_color[1] * 255, conf.xqz_color[2] * 255, 255), conf, mat);
			render_view()->set_blend(xqz_alpha / 255.f);
			model_render()->forced_material_override(mat);
			original();
			set_ignorez(false, mat);
			modulate(c_color(conf.color[0] * 255, conf.color[1] * 255, conf.color[2] * 255, 255), conf, mat);
			render_view()->set_blend(alpha / 255.f);
			model_render()->forced_material_override(mat);
			original();
		}
		else
		{
			set_ignorez(false, mat);
			modulate(c_color(conf.color[0] * 255, conf.color[1] * 255, conf.color[2] * 255, 255), conf, mat);
			render_view()->set_blend(alpha / 255.f);
			model_render()->forced_material_override(mat);
			original();
		}
	}
}

void c_chams::player_chams(Menu22::visaul::esp& conf, c_cs_player* entity, bool draw_fake, bool scope_blend)
{
	if (!entity)
		return;

	if (conf.material_type == 0)
		return;

	c_color c = c_color(conf.color[0] * 255, conf.color[1] * 255, conf.color[2] * 255, conf.color[3] * 255);

	if (conf.material_type - 1 == 18 || conf.material_type - 1 == 14 || conf.material_type - 1 == 9 || conf.material_type - 1 == 7 || conf.material_type - 1 == 4 || conf.material_type - 1 == 15 || conf.material_type - 1 == 16 || conf.material_type - 1 == 8)
		c.alpha = conf.gloss[3] * 255;

	c_color xqz_c = c_color(conf.xqz_color[0] * 255, conf.xqz_color[1] * 255, conf.xqz_color[2] * 255, conf.xqz_color[3] * 255);

	if (conf.material_type - 1 == 18 || conf.material_type - 1 == 14 || conf.material_type - 1 == 9 || conf.material_type - 1 == 7 || conf.material_type - 1 == 4 || conf.material_type - 1 == 15 || conf.material_type - 1 == 16 || conf.material_type - 1 == 8)
		xqz_c.alpha = conf.xqz_gloss[3] * 255;

	bool preserve = conf.preserve_model;
	bool pulse = conf.pulse;
	bool wireframe = conf.wireframe;

	const auto alpha = pulse ? instance()->alpha : scope_blend ? std::clamp(c.alpha / 2, 0, 255) : c.alpha;
	const auto xqz_alpha = pulse ? instance()->alpha : scope_blend ? std::clamp(xqz_c.alpha / 2, 0, 255) : xqz_c.alpha;


	auto mat = GetMat(conf.material_type - 1);

	if (!mat)
		return;

	if (!entity->is_alive())
		return;

	if (!mat->is_error_material())
	{
		mat->incrementreferencecount();

		if (preserve)
		{

			modulate(c_color(255,  255,  255,  255), conf, mat);
			entity->draw_model(0x1, (preserve ? 255 : alpha) / 255.f);
		}

		modulate(c_color(conf.color[0] * 255, conf.color[1] * 255, conf.color[2] * 255, conf.color[3] * 255), conf, mat);
		do_wireframe(mat, wireframe);

		/*if (conf.xqz)
		{
			set_ignorez(true, mat);
			modulate(c_color(conf.color[0] * 255, conf.color[1] * 255, conf.color[2] * 255, conf.color[3] * 255), conf, mat);
			model_render()->forced_material_override(mat);
			entity->draw_model(0x1, alpha);
			set_ignorez(false, mat);
			modulate(c_color(conf.color[0] * 255, conf.color[1] * 255, conf.color[2] * 255, conf.color[3] * 255), conf, mat);
			model_render()->forced_material_override(mat);
			entity->draw_model(0x1, alpha); 		
		}
		else*/
		{
			set_ignorez(false, mat);
			modulate(c_color(conf.color[0] * 255, conf.color[1] * 255, conf.color[2] * 255, alpha), conf, mat);
			model_render()->forced_material_override(mat);
			entity->draw_model(0x1, 255);
		}
	}
}

void c_chams::hand_chams(const std::function<void()> original, Menu22::visaul::esp& conf, Menu22::visaul::esp& conf2, bool draw_hand)
{
	if (conf.material_type > 0) {


		int type = conf.material_type - 1;

		c_color c = c_color(conf.color[0] * 255, conf.color[1] * 255, conf.color[2] * 255, conf.color[3] * 255);

		if (conf.material_type - 1 == 18 || conf.material_type - 1 == 14 || conf.material_type - 1 == 9 || conf.material_type - 1 == 7 || conf.material_type - 1 == 4 || conf.material_type - 1 == 15 || conf.material_type - 1 == 16 || conf.material_type - 1 == 8)
			c.alpha = conf.gloss[3] * 255;

		bool pulse = conf.pulse;
		bool wireframe = conf.wireframe;

		const auto alpha = pulse ? instance()->alpha : c.alpha;
		auto mat = GetMat(type);

		if (!mat)
			return;

		if (!mat->is_error_material()) {
			mat->incrementreferencecount();

			const auto alpha = conf.pulse ? instance()->alpha : c.alpha;
			if (conf.preserve_model) {
				render_view()->set_blend((conf.preserve_model ? 255 : alpha) / 255.f);
				original();
			}

			do_wireframe(mat, conf.wireframe);

			modulate(c, conf, mat);
			set_ignorez(instance()->second_pass ? conf.xqz : false, mat);
			modulate_exp(mat, alpha / 255.f);
			render_view()->set_blend(alpha / 255.f);
			model_render()->forced_material_override(mat);
			original();

			if (instance()->second_pass && conf.xqz)
			{
				if (!mat->is_error_material()) {
					set_ignorez(false, mat);
					modulate(c_color(conf.xqz_color[0] * 255, conf.xqz_color[1] * 255, conf.xqz_color[2] * 255, conf.xqz_color[3] * 255), conf, mat);
					render_view()->set_blend(alpha / 255.f);
					model_render()->forced_material_override(mat);
					original();
				}
			}
		}
	}
	if (conf2.material_type > 0) {

		original();
		int type = conf2.material_type - 1;
		c_color c = c_color(conf.color[0] * 255, conf.color[1] * 255, conf.color[2] * 255, conf.color[3] * 255);

		if (conf.material_type - 1 == 18 || conf.material_type - 1 == 14 || conf.material_type - 1 == 9 || conf.material_type - 1 == 7 || conf.material_type - 1 == 4 || conf.material_type - 1 == 15 || conf.material_type - 1 == 16 || conf.material_type - 1 == 8)
			c.alpha = conf.gloss[3] * 255;

		bool pulse = conf.pulse;
		bool wireframe = conf.wireframe;

		const auto alpha = pulse ? instance()->alpha : c.alpha;

		auto mat = GetMat(type);

		if (!mat)
			return;

		if (!mat->is_error_material()) {
			mat->incrementreferencecount();

			const auto alpha = conf2.pulse ? instance()->alpha : c.alpha;
			if (conf2.preserve_model) {
				render_view()->set_blend((conf2.preserve_model ? 255 : alpha) / 255.f);
				original();
			}

			do_wireframe(mat, conf2.wireframe);

			modulate(c, conf2, mat);
			set_ignorez(instance()->second_pass ? conf2.xqz : false, mat);
			modulate_exp(mat, alpha / 255.f);
			render_view()->set_blend(alpha / 255.f);
			model_render()->forced_material_override(mat);
			original();



			if (instance()->second_pass && conf2.xqz)
			{
				if (!mat->is_error_material()) {
					set_ignorez(false, mat);
					modulate(c_color(conf2.xqz_color[0] * 255, conf2.xqz_color[1] * 255, conf2.xqz_color[2] * 255, conf2.xqz_color[3] * 255), conf2, mat);
					render_view()->set_blend(alpha / 255.f);
					model_render()->forced_material_override(mat);
					original();
				}
			}
		}
	}
}

void c_chams::weapon_chams(const std::function<void()> original, Menu22::visaul::esp& conf, Menu22::visaul::esp& conf2, bool draw_weapon)
{
	if (conf.material_type > 0) {


		int type = conf.material_type - 1;
		c_color c = c_color(conf.color[0] * 255, conf.color[1] * 255, conf.color[2] * 255, conf.color[3] * 255);

		if (conf.material_type - 1 == 18 || conf.material_type - 1 == 14 || conf.material_type - 1 == 9 || conf.material_type - 1 == 7 || conf.material_type - 1 == 4 || conf.material_type - 1 == 15 || conf.material_type - 1 == 16 || conf.material_type - 1 == 8)
			c.alpha = conf.gloss[3] * 255;

		bool pulse = conf.pulse;
		bool wireframe = conf.wireframe;

		const auto alpha = pulse ? instance()->alpha : c.alpha;

		auto mat = GetMat(type);
		if (mat != nullptr && !mat->is_error_material()) {
			mat->incrementreferencecount();

			const auto alpha = conf.pulse ? instance()->alpha : c.alpha;
			if (conf.preserve_model) {
				render_view()->set_blend((conf.preserve_model ? 255 : alpha) / 255.f);
				original();
			}

			do_wireframe(mat, conf.wireframe);

			modulate(c, conf, mat);
			set_ignorez(instance()->second_pass ? conf.xqz : false, mat);
			modulate_exp(mat, alpha / 255.f);
			render_view()->set_blend(alpha / 255.f);
			model_render()->forced_material_override(mat);
			original();
			if (instance()->second_pass && conf.xqz)
			{
				if (mat != nullptr && !mat->is_error_material()) {
					set_ignorez(false, mat);
					modulate(c_color(conf.xqz_color[0] * 255, conf.xqz_color[1] * 255, conf.xqz_color[2] * 255, conf.xqz_color[3] * 255), conf, mat);
					render_view()->set_blend(alpha / 255.f);
					model_render()->forced_material_override(mat);
					original();
				}
			}
		}
	}
	if (conf2.material_type > 0) {

		original();
		int type = conf2.material_type - 1;
		c_color c = c_color(conf.color[0] * 255, conf.color[1] * 255, conf.color[2] * 255, conf.color[3] * 255);

		if (conf.material_type - 1 == 18 || conf.material_type - 1 == 14 || conf.material_type - 1 == 9 || conf.material_type - 1 == 7 || conf.material_type - 1 == 4 || conf.material_type - 1 == 15 || conf.material_type - 1 == 16 || conf.material_type - 1 == 8)
			c.alpha = conf.gloss[3] * 255;

		bool pulse = conf.pulse;
		bool wireframe = conf.wireframe;

		const auto alpha = pulse ? instance()->alpha : c.alpha;

		auto mat = GetMat(type);
		if (mat != nullptr && !mat->is_error_material()) {
			mat->incrementreferencecount();

			const auto alpha = conf2.pulse ? instance()->alpha : c.alpha;
			if (conf2.preserve_model) {
				render_view()->set_blend((conf2.preserve_model ? 255 : alpha) / 255.f);
				original();
			}

			do_wireframe(mat, conf2.wireframe);

			modulate(c, conf2, mat);
			set_ignorez(instance()->second_pass ? conf2.xqz : false, mat);
			modulate_exp(mat, alpha / 255.f);
			render_view()->set_blend(alpha / 255.f);
			model_render()->forced_material_override(mat);
			original();
			if (instance()->second_pass && conf2.xqz)
			{
				if (mat != nullptr && !mat->is_error_material()) {
					set_ignorez(false, mat);
					modulate(c_color(conf2.xqz_color[0] * 255, conf2.xqz_color[1] * 255, conf2.xqz_color[2] * 255, conf2.xqz_color[3] * 255), conf2, mat);
					render_view()->set_blend(alpha / 255.f);
					model_render()->forced_material_override(mat);
					original();
				}
			}
		}
	}
}
void c_chams::modulate(const c_color color, Menu22::visaul::esp& conf, c_material* material, bool xyz)
{
	if (!engine_client()->is_ingame())
		return;

	if (material)
	{
		material->modulate(color);

		auto envmap = material->find_var("$envmaptint");
		if (envmap) {
		if (xyz)
			(*(void(__thiscall**)(int, float, float, float))(*(DWORD*)envmap + 44))((uintptr_t)envmap, conf.xqz_gloss[0], conf.xqz_gloss[1], conf.xqz_gloss[2]); //tfw no IMaterialVar class	
		else
			(*(void(__thiscall**)(int, float, float, float))(*(DWORD*)envmap + 44))((uintptr_t)envmap, conf.gloss[0], conf.gloss[1], conf.gloss[2]); //tfw no IMaterialVar class	
		}
	}

	render_view()->set_color_modulation(color);
}


void c_chams::modulate_exp(c_material* material, const float alpha, const float width)
{
	if (!engine_client()->is_ingame())
		return;

	if (!material || material->is_error_material())
		return;

	const auto transform = material->find_var(_("$envmapfresnelminmaxexp"));

	if (transform)
		transform->set_vector_component(width * alpha, 1);
}

void c_chams::set_ignorez(const bool enabled, c_material* mat)
{
	if (!engine_client()->is_ingame())
		return;

	if (mat && !mat->is_error_material()) {
		mat->set_material_var_flag(material_var_ignorez, enabled);
	}
}