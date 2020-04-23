#include "c_model_render_.h"
#include "../utils/c_hook.h"
#include "../sdk/c_model_info_client.h"
#include "../hacks/c_chams.h"
#include "../sdk/c_render_view.h"
#include "../utils/c_config.h"
#include "../sdk/c_cs_player.h"
#include "../sdk/c_input.h"
#include "../sdk/c_client_entity_list.h"
#include "../utils/c_log.h"
#include "../hacks/c_animation_system.h"
#include "../menu/c_menu.h"
#include "../sdk/c_material_system.h"
#include "../hacks/c_antiaim.h"
#include "../sdk/c_weapon_system.h"
#include "c_client_mode_shared.h"

void c_model_render_::hook()
{
	static c_hook<c_model_render> hook(model_render());
	_draw_model_execute = hook.apply<draw_model_execute_t>(21, draw_model_execute);
}

bool g_local_draw = false;
bool g_global_draw = false;
bool g_basic_draw = false;

void c_model_render_::draw_model_execute(c_model_render* model_render, uint32_t, void* ctx, draw_model_state* state, model_render_info_t& info, matrix3x4* matrix)
{
	if (model_render->is_forced_material_override() || !engine_client()->is_ingame() || !engine_client()->is_connected() || !c_cs_player::get_local_player())
		return _draw_model_execute(model_render, ctx, state, info, matrix);

	render_view()->set_color_modulation(c_color(255, 255, 255));

	if (!info.model)
		return _draw_model_execute(model_render, ctx, state, info, matrix);
	const auto original = [&]() -> void
	{
		_draw_model_execute(model_render, ctx, state, info, c_chams::instance()->current_matrix ? c_chams::instance()->current_matrix : matrix);
	};
	const auto fake_matrix = [&]() -> void
	{
		_draw_model_execute(model_render, ctx, state, info, antiaim->last_fake_matrix);
	};

	if (config->Skinchanger.updated)
		return _draw_model_execute(model_render, ctx, state, info, matrix);

	if (!config->Visuals.__chams)
		return _draw_model_execute(model_render, ctx, state, info, matrix);

	auto local = c_cs_player::get_local_player();

	if (local)
	{
		if (local->is_alive()) {

			bool is_sleeve = strstr(info.model->name, "sleeve") != nullptr;
			bool is_arms = strstr(info.model->name, "arms") != nullptr && !is_sleeve;


			if (is_sleeve)
			{
				const auto local = c_cs_player::get_local_player();

				if (!local || !(local->is_alive() || !input->camera_in_third_person || local->is_scoped()))
					return;
				matrix3x4 interpolated[128] = {};

				if (local && config->Visuals.sleeve.material_type == 0)
				{

					c_chams::hand_chams(original, config->Visuals.sleeve, config->Visuals.sleeve2, false);

					original();

					render_view()->set_color_modulation(c_color(255, 255, 255));
					model_render->forced_material_override(nullptr);

				}
				c_chams::hand_chams(original, config->Visuals.sleeve, config->Visuals.sleeve2, false);

			}
			else if (is_arms)
			{
				const auto local = c_cs_player::get_local_player();

				if (!local || !(local->is_alive() || !input->camera_in_third_person || local->is_scoped()))
					return;
				matrix3x4 interpolated[128] = {};

				if (local && config->Visuals.arms.material_type == 0)
				{

					c_chams::hand_chams(original, config->Visuals.arms, config->Visuals.arms2, false);

					original();

					render_view()->set_color_modulation(c_color(255, 255, 255));
					model_render->forced_material_override(nullptr);

				}

				c_chams::hand_chams(original, config->Visuals.arms, config->Visuals.arms2, false);
			}
			else if (strstr(info.model->name, _("models/weapons")) != nullptr && local->is_alive())
			{
				const auto local = c_cs_player::get_local_player();

				if (!local || !(local->is_alive() || !input->camera_in_third_person || local->is_scoped()))
					return;

				if (local && config->Visuals.weapon.material_type == 0)
				{
					original();

						c_chams::weapon_chams(original, config->Visuals.weapon, config->Visuals.weapon2, false);

						original();

						render_view()->set_color_modulation(c_color(255, 255, 255));
						model_render->forced_material_override(nullptr);

					
				}

				c_chams::weapon_chams(original, config->Visuals.weapon, config->Visuals.weapon2, false);
			}
		}
		if (strstr(info.model->name, _("models/player")) != nullptr)
		{
			const auto entity = reinterpret_cast<c_cs_player*>(client_entity_list()->get_client_entity(info.entity_index));			

			if (entity && !entity->is_ragdoll() && entity->is_alive())
			{
				if (!c_chams::instance()->second_pass && c_chams::instance()->current_player == entity && !entity->is_local_player())
					return;

				if (!entity->is_local_player())
				{
					if (entity->is_enemy() && config->Visuals.backtrack.material_type != 0)
					{
						matrix3x4 interpolated[128] = {};

						if (c_chams::get_simple_backtrack_matrix(entity, interpolated))
						{
							c_chams::instance()->current_matrix = interpolated;
							c_chams::player_chams(original, config->Visuals.backtrack, false);
							original();
							render_view()->set_color_modulation(c_color(255, 255, 255));
							model_render->forced_material_override(nullptr);
							c_chams::instance()->current_matrix = nullptr;
						}
					}
					if (entity->is_enemy())
					{
						c_chams::player_chams(original, config->Visuals.enemy, false);
						c_chams::player_chams(original, config->Visuals.enemy2, false);
					}
					else if (!entity->is_enemy())
					{
						c_chams::player_chams(original, config->Visuals.team2, false);
						c_chams::player_chams(original, config->Visuals.team, false);
					}
				}
				else if (entity->is_local_player())
				{
					c_chams::player_chams(original, config->Visuals.local, false, (entity->is_local_player() && entity->is_scoped() && config->Visuals.local.blend&& input->camera_in_third_person));
					c_chams::player_chams(original, config->Visuals.local2, false, (entity->is_local_player() && entity->is_scoped() && config->Visuals.local2.blend&& input->camera_in_third_person));
				}
			}
		}
	}
	original();
	_draw_model_execute(model_render, ctx, state, info, c_chams::instance()->current_matrix ? c_chams::instance()->current_matrix : matrix);
	model_render->forced_material_override(NULL);
}
