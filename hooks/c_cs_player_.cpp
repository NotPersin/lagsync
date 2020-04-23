#include "c_cs_player_.h"
#include "../hacks/c_trace_system.h"
#include "../hacks/c_antiaim.h"
#include "../sdk/c_global_vars_base.h"
#include "../sdk/c_surface_props.h"
#include "../sdk/c_engine_trace.h"
#include "../sdk/c_prediction.h"
#include "../sdk/recv_prop_hook.h"
#include <future>
#include "../utils/math.h"
#include "../utils/c_config.h"
#include "c_events.h"
#include "../hacks/c_ragebot.h"

void c_cs_player_::hook()
{
	static uint32_t dummy[1] = { reinterpret_cast<uint32_t>(c_cs_player::get_vtable()) };
	hk = std::make_unique<c_hook<uint32_t>>(dummy);

	hk->apply<void(__thiscall*)()>(158, is_player);

	_eye_angles = hk->apply<eye_angles_t>(169, eye_angles);

	_build_transformations = hk->apply<build_transformations_t>(189, build_transformations);
	_do_extra_bone_processing = hk->apply<do_extra_bone_processing_t>(197, do_extra_bone_processing);
	_standard_blending_rules = hk->apply<standard_blending_rules_t>(205, standard_blending_rules);
	_update_client_side_animation = hk->apply<update_client_side_animation_t>(223, update_client_side_animation);
}

void c_cs_player_::apply_to_player(c_cs_player* player)
{
	hk->patch_pointer(reinterpret_cast<uintptr_t*>(player));
}

//c_qangle* c_cs_player_::eye_angles(c_cs_player* player, uint32_t)
//{
//	static auto return_to_fire_bullet = sig("client_panorama.dll", "8B 0D ? ? ? ? F3 0F 7E 00 8B 40 08 89 45 E4");
//	static auto return_to_set_first_person_viewangles = sig("client_panorama.dll", "8B 5D 0C 8B 08 89 0B 8B 48 04 89 4B 04 B9");
//
//	if (player->is_local_player()
//		&& _ReturnAddress() != return_to_fire_bullet
//		&& _ReturnAddress() != return_to_set_first_person_viewangles
//		&& (config->Ragebot._antiaim || config->Ragebot.enable)
//		&& engine_client()->is_ingame())
//	{
//		auto& angle = animation_system->local_animation.eye_angles;
//		return &angle;
//	}
//
//	return _eye_angles(player);
//}

c_qangle* c_cs_player_::eye_angles(c_cs_player* player, uint32_t)
{
	static auto return_to_fire_bullet = sig("client_panorama.dll",
		"8B 0D ? ? ? ? F3 0F 7E 00 8B 40 08 89 45 E4");
	static auto return_to_set_first_person_viewangles = sig("client_panorama.dll",
		"8B 5D 0C 8B 08 89 0B 8B 48 04 89 4B 04 B9");
	static const auto return_to_ccsgo_reticle = sig("client_panorama.dll",
		"F3 0F 10 0D ? ? ? ? 8B C8 8D 84 24 ? ? ? ? 89 44 24 30");
	static const auto return_to_radar_angles = sig("client_panorama.dll",
		"8B 08 89 8F ? ? ? ? 8B 48 04 89 8F ? ? ? ? 8B 40 08 89 87 ? ? ? ? EB 30");

	if (player && player->is_local_player() && player->is_alive())
	{
		// fix nametags, radar angles.
		if ((uintptr_t)_ReturnAddress() == (uintptr_t)return_to_ccsgo_reticle ||
			(uintptr_t)_ReturnAddress() == (uintptr_t)return_to_radar_angles)
		{
			auto local_view_normalized = engine_client()->get_view_angles();
			math::normalize(local_view_normalized);

			return &local_view_normalized;
		}

		if (_ReturnAddress() != return_to_fire_bullet &&
			_ReturnAddress() != return_to_set_first_person_viewangles)
		{
			// smooth out clientside landing animation.
			//static auto decay_animation = 0.f;

			auto& angles = animation_system->local_animation.eye_angles;

			return &angles;
		}
	}

	return _eye_angles(player);
}

void __fastcall c_cs_player_::build_transformations(c_cs_player* player, uint32_t, c_studio_hdr* hdr, c_vector3d* pos, quaternion* q,
	const matrix3x4& transform, const int32_t mask, byte* computed)
{
	// backup bone flags.
	const auto backup_bone_flags = hdr->bone_flags;

	// stop procedural animations.
	for (auto i = 0; i < hdr->bone_flags.Count(); i++)
		hdr->bone_flags.Element(i) &= ~bone_always_procedural;

	_build_transformations(player, hdr, pos, q, transform, mask, computed);

	// restore bone flags.
	hdr->bone_flags = backup_bone_flags;
}

int c_cs_player_::proxy_is_player(c_cs_player* player, void* return_address, void* eax)
{
	static const auto return_to_should_skip_animframe = sig("client_panorama.dll", "84 C0 75 02 5F C3 8B 0D");

	if (return_address != return_to_should_skip_animframe)
		return 2;

	const auto local = c_cs_player::get_local_player();

	if (!local || !local->is_alive() || !player->is_enemy())
		return 2;

	return 0;
}

void c_cs_player_::standard_blending_rules(c_cs_player* player, uint32_t, c_studio_hdr* hdr, c_vector3d* pos, quaternion* q, const float time, int mask)
{
	if (player->is_enemy() || player->is_local_player())
		mask = bone_used_by_server;

	if (player->is_local_player())
		mask |= bone_used_by_bone_merge;

	_standard_blending_rules(player, hdr, pos, q, time, mask);
}

void c_cs_player_::do_extra_bone_processing(c_cs_player* player, uint32_t, c_studio_hdr* hdr, c_vector3d* pos, quaternion* q,
	const matrix3x4& matrix, uint8_t* bone_computed, void* context)
{
	return;
}

void c_cs_player_::update_client_side_animation(c_cs_player* player, uint32_t)
{
	const auto local = c_cs_player::get_local_player();
	const auto is_valid_player = player->is_local_player() || player->is_enemy();

	if (!local || !local->is_alive() || !is_valid_player)
		return _update_client_side_animation(player);

	if (animation_system->enable_bones)
	{
		const auto old_animation_flag = player->get_client_side_animation();
		player->get_client_side_animation() = true;
		_update_client_side_animation(player);
		player->get_client_side_animation() = old_animation_flag;
	}
}

// ReSharper disable once CppDeclaratorNeverUsed
static uint32_t is_player_retn_address = 0;

bool __declspec(naked) is_player(void* eax, void* edx)
{
	_asm
	{
		push eax

		mov eax, [esp + 4]
		mov is_player_retn_address, eax

		push is_player_retn_address
		push ecx
		call c_cs_player_::proxy_is_player

		cmp eax, 1
		je _retn1

		cmp eax, 0
		je _retn0

		mov al, 1
		retn

		_retn0 :
		mov al, 0
			retn

			_retn1 :
		pop eax
			mov eax, is_player_retn_address
			add eax, 0x6B
			push eax
			retn
	}
}