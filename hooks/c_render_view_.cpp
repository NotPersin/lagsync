#include "c_render_view_.h"
#include "../utils/c_hook.h"
#include "../hacks/c_chams.h"
#include "../sdk/c_glow_object_manager.h"
#include "../hooks/c_client.h"
#include "../hacks/c_antiaim.h"

void c_render_view_::hook()
{
	static c_hook<c_render_view> hook(render_view());
	_scene_end = hook.apply<scene_end_t>(9, scene_end);
}
void __fastcall c_render_view_::scene_end(c_render_view* render_view, uint32_t)
{

	_scene_end(render_view);


	auto local = c_cs_player::get_local_player();

	if (!local)
		return;

	if (!engine_client()->is_ingame() && engine_client()->is_connected())
		return;

	if (config->Ragebot._antiaim)
	{
		const auto standing = local->get_velocity().length2d() <= 15 && local->is_on_ground();
		const auto moving = local->get_velocity().length2d() > 15 && local->is_on_ground();
		const auto air = !moving && !standing;

		if ((standing && config->Ragebot.stand.fake_type > 0) || (moving && config->Ragebot.moving.fake_type > 0) || (air && config->Ragebot.air.fake_type > 0)) {
			if ((!GetAsyncKeyState(config->Misc.airstuck_key) && config->Misc.airstuck_enable) || !config->Misc.airstuck_enable) {
				c_vector3d OldOrg = local->get_abs_origin();
				c_vector3d OldAngles = local->get_eye_angles();
				local->set_abs_angles(c_vector3d(0, antiaim->get_last_fake(), 0.f));

				c_chams::player_chams(config->Visuals.desync, local, true, false);
				c_chams::player_chams(config->Visuals.desync2, local, true, false);

				//c_chams::player_chams(_config->Visuals.desync, local, false, false);
				//c_chams::player_chams(_config->Visuals.desync, local, false, true);

				local->set_abs_angles(c_vector3d(0, OldAngles.y, 0.f));

				render_view->set_color_modulation(c_color(255, 255, 255));
				render_view->set_blend(1.f);
				model_render()->forced_material_override(nullptr);
			}
		}
	}
	c_chams::draw_players();

	for (auto i = 0; i < glow_object_manager->GetSize(); i++)
	{
		auto& glowObject = glow_object_manager->m_GlowObjectDefinitions[i];
		auto entity = reinterpret_cast<c_cs_player*>(glowObject.m_pEntity);

		if (!entity)
			continue;

		if (glowObject.IsUnused())
			continue;

		if (entity->is_dormant())
			continue;

		bool is_local_player = entity == c_cs_player::get_local_player();
		bool is_teammate = c_cs_player::get_local_player()->get_team() == entity->get_team() && !is_local_player;

		if (entity->get_class_id() == 40) {

			if (is_local_player && config->Visuals.localglowenable)
			{
				glowObject.m_nGlowStyle = config->Visuals.localglowstyle;//0;
				glowObject.m_flRed = config->Colors.localglow_col[0];
				glowObject.m_flGreen = config->Colors.localglow_col[1];
				glowObject.m_flBlue = config->Colors.localglow_col[2];
				glowObject.m_flAlpha = config->Colors.localglow_col[3];
				glowObject.m_bRenderWhenOccluded = true;
				glowObject.m_bRenderWhenUnoccluded = false;
			}

			if (config->Visuals.glowenable && !is_teammate && !is_local_player)
			{
				glowObject.m_nGlowStyle = config->Visuals.glowstyle;//0;
				glowObject.m_flRed = config->Colors.glow_col[0];
				glowObject.m_flGreen = config->Colors.glow_col[1];
				glowObject.m_flBlue = config->Colors.glow_col[2];
				glowObject.m_flAlpha = config->Colors.glow_col[3];
				glowObject.m_bRenderWhenOccluded = true;
				glowObject.m_bRenderWhenUnoccluded = false;
				continue;
			}
		}
		if (entity->is_weapon() && config->Visuals.droppedglowenable) {
			glowObject.m_nGlowStyle = config->Visuals.droppedglowstyle;//0;
			glowObject.m_flRed = config->Colors.droppedglow_col[0];
			glowObject.m_flGreen = config->Colors.droppedglow_col[1];
			glowObject.m_flBlue = config->Colors.droppedglow_col[2];
			glowObject.m_flAlpha = config->Colors.droppedglow_col[3];
			glowObject.m_bRenderWhenOccluded = true;
			glowObject.m_bRenderWhenUnoccluded = false;
			continue;
		}
	}	
}
