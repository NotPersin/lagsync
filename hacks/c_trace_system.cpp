#include "c_trace_system.h"
#include "../utils/math.h"
#include "../sdk/c_weapon_system.h"
#include "../sdk/c_engine_trace.h"
#include "../sdk/c_client_entity_list.h"
#include "../sdk/c_surface_props.h"
#include "../utils/c_config.h"

std::optional<c_trace_system::wall_pen> c_trace_system::wall_penetration(const c_vector3d src, const c_vector3d end,
	c_animation_system::animation* target, c_cs_player* override_player) const
{
	static c_base_combat_weapon::weapon_data override_gun{};

	override_gun.iDamage = 15000;
	override_gun.flRangeModifier = 1.0f;
	override_gun.flPenetration = 10.0f;
	override_gun.flArmorRatio = 2.0f;
	override_gun.flRange = 8192.f;
	override_gun.m_pWeaponDef = reinterpret_cast<c_base_combat_weapon::strike_weapon_definition*>(1);

	const auto local = c_cs_player::get_local_player();

	if (!local || !local->is_alive())
		return std::nullopt;

	const auto weapon = reinterpret_cast<c_base_combat_weapon*>(client_entity_list()
		->get_client_entity_from_handle(local->get_current_weapon_handle()));

	if (!weapon && !override_player)
		return std::nullopt;

	const auto data = override_player ? &override_gun :
		weapon_system->get_weapon_data(weapon->get_item_definition());

	if (!data)
		return std::nullopt;

	std::optional<wall_pen> result = std::nullopt;

	if (!override_player)
		run_emulated(target, [&]() -> void
	{
		// setup trace filter.
		uint32_t filter[4] = { c_engine_trace::get_filter_simple_vtable(),
			reinterpret_cast<uint32_t>(local), 0, 0 };

		// run bullet simulation
		result = fire_bullet(data, src, end,
			reinterpret_cast<trace_filter*>(filter), target->player);
	});
	else
	{
		// setup trace filter.
		c_trace_no_player_filter filter;

		// run bullet simulation
		result = fire_bullet(data, src, end, &filter, override_player, true);
	}

	// filter low dmg.
	if (result.has_value() && result.value().damage < 1.f)
		return std::nullopt;

	// report result
	return result;
}

void c_trace_system::run_emulated(c_animation_system::animation* target, const std::function<void()> fn)
{
	// backup player
	const auto backup_origin = target->player->get_origin();
	const auto backup_abs_origin = target->player->get_abs_origin();
	const auto backup_obb_mins = target->player->get_mins();
	const auto backup_obb_maxs = target->player->get_maxs();
	const auto backup_cache = target->player->get_bone_cache();

	// setup trace data
	target->player->get_origin() = target->origin;
	target->player->set_abs_origin(target->origin);
	target->player->get_mins() = target->obb_mins;
	target->player->get_maxs() = target->obb_maxs;
	target->player->get_bone_cache() = reinterpret_cast<matrix3x4**>(target->bones);

	// run emulation
	fn();

	// restore trace data
	target->player->get_origin() = backup_origin;
	target->player->set_abs_origin(backup_abs_origin);
	target->player->get_mins() = backup_obb_mins;
	target->player->get_maxs() = backup_obb_maxs;
	target->player->get_bone_cache() = backup_cache;
}

void c_trace_system::extrapolate(c_cs_player* player, c_vector3d& origin, c_vector3d& velocity, int& flags, bool on_ground)
{
	static const auto sv_gravity = cvar()->find_var(_("sv_gravity"));
	static const auto sv_jump_impulse = cvar()->find_var(_("sv_jump_impulse"));

	if (!(flags & c_base_player::flags::on_ground))
		velocity.z -= ticks_to_time(sv_gravity->get_float());
	else if (player->get_flags() & c_base_player::flags::on_ground && !on_ground)
		velocity.z = sv_jump_impulse->get_float();

	const auto src = origin;
	auto end = src + velocity * global_vars_base->interval_per_tick;

	ray r;
	r.init(src, end, player->get_mins(), player->get_maxs());

	game_trace t{};
	c_trace_filter filter;
	filter.skip_entity = player;

	engine_trace()->trace_ray(r, mask_playersolid, &filter, &t);

	if (t.fraction != 1.f)
	{
		for (auto i = 0; i < 2; i++)
		{
			velocity -= t.plane.normal * velocity.dot(t.plane.normal);

			const auto dot = velocity.dot(t.plane.normal);
			if (dot < 0.f)
				velocity -= c_vector3d(dot * t.plane.normal.x,
					dot * t.plane.normal.y, dot * t.plane.normal.z);

			end = t.endpos + velocity * ticks_to_time(1.f - t.fraction);

			r.init(t.endpos, end, player->get_mins(), player->get_maxs());
			engine_trace()->trace_ray(r, mask_playersolid, &filter, &t);

			if (t.fraction == 1.f)
				break;
		}
	}

	origin = end = t.endpos;
	end.z -= 2.f;

	r.init(origin, end, player->get_mins(), player->get_maxs());
	engine_trace()->trace_ray(r, mask_playersolid, &filter, &t);

	flags &= ~c_base_player::flags::on_ground;

	if (t.did_hit() && t.plane.normal.z > .7f)
		flags |= c_base_player::flags::on_ground;
}

std::optional<c_trace_system::wall_pen> c_trace_system::fire_bullet(c_base_combat_weapon::weapon_data* data, c_vector3d src,
	const c_vector3d pos, trace_filter* filter, c_cs_player* target, bool point)
{
	c_vector3d angles;
	math::vector_angles(pos - src, angles);

	if (!angles.is_valid())
		return std::nullopt;

	c_vector3d direction;
	math::angle_vectors(angles, direction);

	if (!direction.is_valid())
		return std::nullopt;

	direction.normalize();

	auto penetrate_count = 5;
	auto length = 0.f, damage = static_cast<float>(data->iDamage);
	game_trace enter_trace{};

	const auto start = src;

	while (penetrate_count > 0 && damage >= 1.0f)
	{
		const auto length_remaining = data->flRange - length;
		auto end = src + direction * length_remaining;

		ray r{};
		r.init(src, end);
		engine_trace()->trace_ray(r, mask_shot_player, filter, &enter_trace);

		if (enter_trace.fraction == 1.f && !point)
			break;

		if (point && (enter_trace.fraction == 1.f ||
			(start - enter_trace.endpos).length() > (start - pos).length()))
			return wall_pen{
				scale_damage(target, damage, data->flArmorRatio, hitgroup_head, !data->m_pWeaponDef),
				c_cs_player::hitbox::head, hitgroup_head };

		auto end_extended = end + direction * 40.f;
		engine_trace()->clip_ray_to_player(src, end_extended, mask_shot_player, target, filter, &enter_trace);

		length += enter_trace.fraction * length_remaining;
		damage *= std::powf(data->flRangeModifier, length * .002f);

		if (enter_trace.hitgroup <= 7 && enter_trace.hitgroup > 0)
		{
			if (!enter_trace.entity || enter_trace.entity != target)
				break;

			// we have reached our target!
			return wall_pen{
				scale_damage(target, damage, data->flArmorRatio, enter_trace.hitgroup, !data->m_pWeaponDef),
				static_cast<c_cs_player::hitbox>(enter_trace.hitbox), enter_trace.hitgroup };
		}

		const auto enter_surface = surface_props()->get_surface_data(enter_trace.surface.surface_props);

		if (!enter_surface || enter_surface->game.penetration_mod < .1f)
			break;

		if (!handle_bullet_penetration(data, enter_trace, src, direction, penetrate_count, damage, data->flPenetration))
			break;
	}

	// nothing found
	return std::nullopt;
}

bool c_trace_system::handle_bullet_penetration(c_base_combat_weapon::weapon_data* weapon_data, game_trace& enter_trace,
	c_vector3d& eye_position, const c_vector3d direction, int& penetrate_count, float& current_damage, const float penetration_power)
{
	static const auto ff_damage_reduction_bullets = cvar()->find_var("ff_damage_reduction_bullets");
	static const auto ff_damage_bullet_penetration = cvar()->find_var("ff_damage_bullet_penetration");

	const auto damage_reduction_bullets = ff_damage_reduction_bullets->get_float();
	const auto damage_bullet_penetration = ff_damage_bullet_penetration->get_float();

	game_trace exit_trace{};
	auto enemy = reinterpret_cast<c_cs_player*>(enter_trace.entity);
	const auto enter_surface_data = surface_props()->get_surface_data(enter_trace.surface.surface_props);
	const int enter_material = enter_surface_data->game.material;

	const auto enter_surf_penetration_modifier = enter_surface_data->game.penetration_mod;
	float final_damage_modifier, combined_penetration_modifier;
	const bool is_solid_surf = enter_trace.contents >> 3 & contents_solid;
	const bool is_light_surf = enter_trace.surface.flags >> 7 & surf_light;

	if ((!penetrate_count && !is_light_surf && !is_solid_surf && enter_material != char_tex_grate && enter_material != char_tex_glass)
		|| weapon_data->flPenetration <= 0.f
		|| (!trace_to_exit(enter_trace, exit_trace, enter_trace.endpos, direction, weapon_data->iDamage > 10000.f)
			&& !(engine_trace()->get_point_contents(enter_trace.endpos, mask_shot_hull) & mask_shot_hull)))
		return false;

	const auto exit_surface_data = surface_props()->get_surface_data(exit_trace.surface.surface_props);
	const auto exit_material = exit_surface_data->game.material;
	const auto exit_surf_penetration_modifier = exit_surface_data->game.penetration_mod;

	if (enter_material == char_tex_grate || enter_material == char_tex_glass)
	{
		combined_penetration_modifier = 3.f;
		final_damage_modifier = 0.05f;
	}
	else if (is_solid_surf || is_light_surf)
	{
		combined_penetration_modifier = 1.f;
		final_damage_modifier = 0.16f;
	}
	else if (enter_material == char_tex_flesh && (!enemy->is_enemy() && damage_reduction_bullets == 0.f))
	{
		if (damage_bullet_penetration == 0.f)
			return false;

		combined_penetration_modifier = damage_bullet_penetration;
		final_damage_modifier = 0.16f;
	}
	else
	{
		combined_penetration_modifier = (enter_surf_penetration_modifier + exit_surf_penetration_modifier) / 2.f;
		final_damage_modifier = 0.16f;
	}

	if (enter_material == exit_material)
	{
		if (exit_material == char_tex_cardboard || exit_material == char_tex_wood)
			combined_penetration_modifier = 3.f;
		else if (exit_material == char_tex_plastic)
			combined_penetration_modifier = 2.f;
	}

	auto thickness = (exit_trace.endpos - enter_trace.endpos).length();
	thickness *= thickness;
	thickness *= fmaxf(0.f, 1.0f / combined_penetration_modifier);
	thickness /= 24.0f;

	const auto lost_damage = fmaxf(0.0f, current_damage * final_damage_modifier + fmaxf(0.f, 1.0f / combined_penetration_modifier)
		* 3.0f * fmaxf(0.0f, 3.0f / penetration_power) * 1.25f + thickness);

	if (lost_damage > current_damage)
		return false;

	if (lost_damage > 0.f)
		current_damage -= lost_damage;

	if (current_damage < 1.f)
		return false;

	eye_position = exit_trace.endpos;
	--penetrate_count;
	return true;
}

bool c_trace_system::trace_to_exit(game_trace& enter_trace, game_trace& exit_trace, const c_vector3d start_position,
	const c_vector3d direction, const bool is_local)
{
	const auto max_distance = is_local ? 200.f : 90.f;
	const auto ray_extension = is_local ? 8.f : 4.f;

	float current_distance = 0;
	auto first_contents = 0;

	while (current_distance <= max_distance)
	{
		current_distance += ray_extension;

		auto start = start_position + direction * current_distance;

		if (!first_contents)
			first_contents = engine_trace()->get_point_contents(start, mask_shot_player);

		const auto point_contents = engine_trace()->get_point_contents(start, mask_shot_player);

		if (!(point_contents & mask_shot_hull) || (point_contents & contents_hitbox && point_contents != first_contents))
		{
			const auto end = start - direction * ray_extension;

			ray r{};
			r.init(start, end);
			uint32_t filter[4] = { c_engine_trace::get_filter_simple_vtable(),
				uint32_t(c_cs_player::get_local_player()), 0, 0 };
			engine_trace()->trace_ray(r, mask_shot_player, reinterpret_cast<trace_filter*>(filter), &exit_trace);

			if (exit_trace.startsolid && exit_trace.surface.flags & surf_hitbox)
			{
				r.init(start, start_position);
				filter[1] = reinterpret_cast<uint32_t>(exit_trace.entity);
				engine_trace()->trace_ray(r, mask_shot_player, reinterpret_cast<trace_filter*>(filter), &exit_trace);

				if (exit_trace.did_hit() && !exit_trace.startsolid)
				{
					start = exit_trace.endpos;
					return true;
				}

				continue;
			}

			if (exit_trace.did_hit() && !exit_trace.startsolid)
			{
				if (enter_trace.entity->is_breakable() && exit_trace.entity->is_breakable())
					return true;

				if (enter_trace.surface.flags & surf_nodraw
					|| (!(exit_trace.surface.flags & surf_nodraw)
						&& exit_trace.plane.normal.dot(direction) <= 1.f))
				{
					const auto mult_amount = exit_trace.fraction * 4.f;
					start -= direction * mult_amount;
					return true;
				}

				continue;
			}

			if (!exit_trace.did_hit() || exit_trace.startsolid)
			{
				if (enter_trace.did_hit_non_world_entity() && enter_trace.entity->is_breakable())
				{
					exit_trace = enter_trace;
					exit_trace.endpos = start;
					return true;
				}
			}
		}
	}

	return false;
}

// credits to n0xius
float c_trace_system::scale_damage(c_cs_player* target, float damage, const float weapon_armor_ratio, int hitgroup, bool is_zeus)
{
	const auto is_armored = [&]() -> bool
	{
		if (target->get_armor() > 0.f)
		{
			switch (hitgroup)
			{
			case hitgroup_generic:
			case hitgroup_chest:
			case hitgroup_stomach:
			case hitgroup_leftarm:
			case hitgroup_rightarm:
				return true;
			case hitgroup_head:
				return target->has_helmet();
			default:
				break;
			}
		}

		return false;
	};

	if (!is_zeus)
		switch (hitgroup)
		{
		case hitgroup_head:
			if (target->has_heavy_armor())
				damage = (damage * 3.5f) * .5f;
			else
				damage *= 4.f;
			break;
		case hitgroup_stomach:
			damage *= 1.25f;
			break;
		case hitgroup_leftleg:
		case hitgroup_rightleg:
			damage *= .65f;
			break;
		default:
			break;
		}

	if (is_armored())
	{
		auto modifier = 1.f, armor_bonus_ratio = .5f, armor_ratio = weapon_armor_ratio * .5f;

		if (target->has_heavy_armor())
		{
			armor_bonus_ratio = 0.33f;
			armor_ratio = (weapon_armor_ratio * 0.5f) * 0.5f;
			modifier = 0.33f;
		}

		auto new_damage = damage * armor_ratio;

		if (target->has_heavy_armor())
			new_damage *= 0.85f;

		if ((damage - damage * armor_ratio) * (modifier * armor_bonus_ratio) > target->get_armor())
			new_damage = damage - target->get_armor() / armor_bonus_ratio;

		damage = new_damage;
	}

	return damage;
}

void trace_line(c_vector3d& absStart, c_vector3d& absEnd, unsigned int mask, c_base_entity* ignore, game_trace* ptr)
{
	ray ray;
	ray.init(absStart, absEnd);
	c_trace_filter filter;
	filter.skip_entity = ignore;

	engine_trace()->trace_ray(ray, mask, &filter, ptr);
}

bool trace_to_exit(game_trace& enterTrace, game_trace& exitTrace, c_vector3d startPosition, c_vector3d direction)
{
	/*
	Masks used:
	MASK_SHOT_HULL					 = 0x600400B
	CONTENTS_HITBOX					 = 0x40000000
	MASK_SHOT_HULL | CONTENTS_HITBOX = 0x4600400B

	*/
	c_vector3d start, end;
	float maxDistance = 90.f, rayExtension = 4.f, currentDistance = 0;
	int firstContents = 0;

	while (currentDistance <= maxDistance)
	{
		//Add extra distance to our ray
		currentDistance += rayExtension;

		//Multiply the direction vector to the distance so we go outwards, add our position to it.
		start = startPosition + direction * currentDistance;

		if (!firstContents)
			firstContents = engine_trace()->get_point_contents(start, mask_shot_hull | contents_hitbox); /*0x4600400B*/
		int pointContents = engine_trace()->get_point_contents(start, mask_shot_hull | contents_hitbox);

		if (!(pointContents & mask_shot_hull) || pointContents & contents_hitbox && pointContents != firstContents) /*0x600400B, *0x40000000*/
		{
			//Let's setup our end position by deducting the direction by the extra added distance
			end = start - (direction * rayExtension);

			//Let's cast a ray from our start pos to the end pos
			trace_line(start, end, mask_shot_hull | contents_hitbox, nullptr, &exitTrace);

			//Let's check if a hitbox is in-front of our enemy and if they are behind of a solid wall
			if (exitTrace.startsolid && exitTrace.surface.flags & surf_hitbox)
			{
				trace_line(start, startPosition, mask_shot_hull, exitTrace.entity, &exitTrace);

				if (exitTrace.did_hit() && !exitTrace.startsolid)
				{
					start = exitTrace.endpos;
					return true;
				}

				continue;
			}

			//Can we hit? Is the wall solid?
			if (exitTrace.did_hit() && !exitTrace.startsolid)
			{

				//Is the wall a breakable? If so, let's shoot through it.
				if (enterTrace.entity->is_breakable() && exitTrace.entity->is_breakable())
					return true;

				if (enterTrace.surface.flags & 0x0080 || !(exitTrace.surface.flags & 0x0080) && (exitTrace.plane.normal.dot(direction) <= 1.f))
				{
					float multAmount = exitTrace.fraction * 4.f;
					start -= direction * multAmount;
					return true;
				}

				continue;
			}

			if (!exitTrace.did_hit() || exitTrace.startsolid)
			{
				if (enterTrace.did_hit() && enterTrace.entity->is_breakable())
				{
					exitTrace = enterTrace;
					exitTrace.endpos = start + direction;
					return true;
				}

				continue;
			}
		}
	}
	return false;
}

bool hbp(c_base_combat_weapon::weapon_data* weaponData, game_trace& enterTrace, c_vector3d& eyePosition, c_vector3d direction, int& possibleHitsRemaining, float& currentDamage, float penetrationPower, bool sv_penetration_type, float ff_damage_reduction_bullets, float ff_damage_bullet_penetration)
{
	auto pLocalEntity = c_cs_player::get_local_player();
	if (!pLocalEntity || pLocalEntity->get_health() <= 0) return false;
	//Because there's been issues regarding this- putting this here.
	if (&currentDamage == nullptr)
		throw std::invalid_argument("currentDamage is null!");

	auto data = FireBulletData(pLocalEntity->get_shoot_position());
	data.filter = c_trace_filter();
	data.filter.skip_entity = pLocalEntity;
	game_trace exitTrace;
	c_base_entity* pEnemy = enterTrace.entity;
	surfacedata_t* enterSurfaceData = surface_props()->get_surface_data(enterTrace.surface.surface_props);
	int enterMaterial = enterSurfaceData->game.material;
	//IPhysicsSurfaceProps
	float enterSurfPenetrationModifier = enterSurfaceData->game.penetration_mod;
	float enterDamageModifier = enterSurfaceData->game.damage_mod;
	float thickness, modifier, lostDamage, finalDamageModifier, combinedPenetrationModifier;
	bool isSolidSurf = ((enterTrace.contents >> 3)& contents_solid);
	bool isLightSurf = ((enterTrace.surface.flags >> 7) & 0x0001);

	if (possibleHitsRemaining <= 0
		//Test for "DE_CACHE/DE_CACHE_TELA_03" as the entering surface and "CS_ITALY/CR_MISCWOOD2B" as the exiting surface.
		//Fixes a wall in de_cache which seems to be broken in some way. Although bullet penetration is not recorded to go through this wall
		//Decals can be seen of bullets within the glass behind of the enemy. Hacky method, but works.
		//You might want to have a check for this to only be activated on de_cache.
		|| (enterTrace.surface.name == (const char*)0x2227c261 && exitTrace.surface.name == (const char*)0x2227c868)
		|| (!possibleHitsRemaining && !isLightSurf && !isSolidSurf && enterMaterial != char_tex_grate && enterMaterial != char_tex_glass)
		|| weaponData->flPenetration <= 0.f
		|| !trace_to_exit(enterTrace, exitTrace, enterTrace.endpos, direction)
		&& !(engine_trace()->get_point_contents(enterTrace.endpos, mask_shot_hull) & mask_shot_hull))
		return false;

	surfacedata_t* exitSurfaceData = surface_props()->get_surface_data(exitTrace.surface.surface_props);
	int exitMaterial = exitSurfaceData->game.material;
	float exitSurfPenetrationModifier = exitSurfaceData->game.penetration_mod;
	float exitDamageModifier = exitSurfaceData->game.damage_mod;

	//Are we using the newer penetration system?
	if (sv_penetration_type)
	{
		if (enterMaterial == char_tex_grate || enterMaterial == char_tex_glass)
		{
			combinedPenetrationModifier = 3.f;
			finalDamageModifier = 0.05f;
		}
		else if (isSolidSurf || isLightSurf)
		{
			combinedPenetrationModifier = 1.f;
			finalDamageModifier = 0.16f;
		}
		else if (enterMaterial == char_tex_flesh && (pLocalEntity->get_team() == pEnemy->get_team() && ff_damage_reduction_bullets == 0.f)) //TODO: Team check config
		{
			//Look's like you aren't shooting through your teammate today
			if (ff_damage_bullet_penetration == 0.f)
				return false;

			//Let's shoot through teammates and get kicked for teamdmg! Whatever, atleast we did damage to the enemy. I call that a win.
			combinedPenetrationModifier = ff_damage_bullet_penetration;
			finalDamageModifier = 0.16f;
		}
		else
		{
			combinedPenetrationModifier = (enterSurfPenetrationModifier + exitSurfPenetrationModifier) / 2.f;
			finalDamageModifier = 0.16f;
		}

		//Do our materials line up?
		if (enterMaterial == exitMaterial)
		{
			if (exitMaterial == char_tex_cardboard || exitMaterial == char_tex_wood)
				combinedPenetrationModifier = 3.f;
			else if (exitMaterial == char_tex_plastic)
				combinedPenetrationModifier = 2.f;
		}

		//Calculate thickness of the wall by getting the length of the range of the trace and squaring
		thickness = (exitTrace.endpos - enterTrace.endpos).lengthsqr();
		modifier = fmaxf(1.f / combinedPenetrationModifier, 0.f);

		//This calculates how much damage we've lost depending on thickness of the wall, our penetration, damage, and the modifiers set earlier
		lostDamage = fmaxf(
			((modifier * thickness) / 24.f)
			+ ((currentDamage * finalDamageModifier)
				+ (fmaxf(3.75f / penetrationPower, 0.f) * 3.f * modifier)), 0.f);

		//Did we loose too much damage?
		if (lostDamage > currentDamage)
			return false;

		//We can't use any of the damage that we've lost
		if (lostDamage > 0.f)
			currentDamage -= lostDamage;

		//Do we still have enough damage to deal?
		if (currentDamage < 1.f)
			return false;

		eyePosition = exitTrace.endpos;
		--possibleHitsRemaining;

		return true;
	}
	else //Legacy penetration system
	{
		combinedPenetrationModifier = 1.f;

		if (isSolidSurf || isLightSurf)
			finalDamageModifier = 0.99f; //Good meme :^)
		else
		{
			finalDamageModifier = fminf(enterDamageModifier, exitDamageModifier);
			combinedPenetrationModifier = fminf(enterSurfPenetrationModifier, exitSurfPenetrationModifier);
		}

		if (enterMaterial == exitMaterial && (exitMaterial == char_tex_metal || exitMaterial == char_tex_wood))
			combinedPenetrationModifier += combinedPenetrationModifier;

		thickness = (exitTrace.endpos - enterTrace.endpos).lengthsqr();

		if (sqrt(thickness) <= combinedPenetrationModifier * penetrationPower)
		{
			currentDamage *= finalDamageModifier;
			eyePosition = exitTrace.endpos;
			--possibleHitsRemaining;

			return true;
		}

		return false;
	}
}

bool c_trace_system::can_wallbang(float& dmg) {

	auto local = c_cs_player::get_local_player();
	if (!local || local->get_health() <= 0) return false;
	FireBulletData data = FireBulletData(local->get_shoot_position());
	data.filter = c_trace_filter();
	data.filter.skip_entity = local;

	c_vector3d eye_angle = engine_client()->get_view_angles();

	c_vector3d dst, forward;

	math::angle_vectors(eye_angle, forward);
	dst = data.src + (forward * 8196.f);

	c_vector3d angles;
	angles = math::calc_angle(data.src, dst);
	math::angle_vectors(angles, data.direction);
	math::normalize(data.direction);

	const auto weapon = reinterpret_cast<c_base_combat_weapon*>(
		client_entity_list()->get_client_entity_from_handle(local->get_current_weapon_handle()));

	if (!weapon || weapon->get_current_clip() == 0) return false;


	data.penetrate_count = 1;
	data.trace_length = 0.0f;

	auto weaponData = weapon_system->get_weapon_data(weapon->get_item_definition());

	if (!weaponData)
		return false;

	data.current_damage = (float)weaponData->iDamage;

	data.trace_length_remaining = weaponData->flRange - data.trace_length;

	c_vector3d end = data.src + data.direction * data.trace_length_remaining;

	trace_line(data.src, end, mask_shot | contents_grate, local, &data.enter_trace);

	static convar* penetrationSystem = cvar()->find_var(_("sv_penetration_type"));
	static convar* damageReductionBullets = cvar()->find_var(_("ff_damage_reduction_bullets"));
	static convar* damageBulletPenetration = cvar()->find_var(_("ff_damage_bullet_penetration"));

	auto ff_damage_reduction_bullets = damageReductionBullets->get_float();
	auto ff_damage_bullet_penetration = damageBulletPenetration->get_float();
	auto sv_penetration_type = penetrationSystem->get_bool();

	if (data.enter_trace.fraction == 1.0f)
		return false;

	if (hbp(weaponData, data.enter_trace, local->get_shoot_position(), data.direction, data.penetrate_count, data.current_damage, weaponData->flPenetration, sv_penetration_type, ff_damage_reduction_bullets, ff_damage_bullet_penetration))
	{
		dmg = data.current_damage;
		return true;
	}

	return false;
}

bool c_trace_system::can_wallbang(float& dmg, c_vector3d origin) {

	auto local = c_cs_player::get_local_player();
	if (!local || local->get_health() <= 0) return false;
	FireBulletData data = FireBulletData(local->get_shoot_position());
	data.filter = c_trace_filter();
	data.filter.skip_entity = local;

	c_vector3d eye_angle = engine_client()->get_view_angles();

	c_vector3d dst, forward;

	math::angle_vectors(eye_angle, origin);
	dst = data.src + origin * 8196.f;//(forward * 8196.f);

	c_vector3d angles;
	angles = math::calc_angle(data.src, dst);
	math::angle_vectors(angles, data.direction);
	math::normalize(data.direction);

	const auto weapon = reinterpret_cast<c_base_combat_weapon*>(
		client_entity_list()->get_client_entity_from_handle(local->get_current_weapon_handle()));

	if (!weapon || weapon->get_current_clip() == 0) return false;


	data.penetrate_count = 1;
	data.trace_length = 0.0f;

	auto weaponData = weapon_system->get_weapon_data(weapon->get_item_definition());

	if (!weaponData)
		return false;

	data.current_damage = (float)weaponData->iDamage;

	data.trace_length_remaining = weaponData->flRange - data.trace_length;

	c_vector3d end = data.src + data.direction * data.trace_length_remaining;

	trace_line(data.src, end, mask_shot | contents_grate, local, &data.enter_trace);

	static convar* penetrationSystem = cvar()->find_var(_("sv_penetration_type"));
	static convar* damageReductionBullets = cvar()->find_var(_("ff_damage_reduction_bullets"));
	static convar* damageBulletPenetration = cvar()->find_var(_("ff_damage_bullet_penetration"));

	auto ff_damage_reduction_bullets = damageReductionBullets->get_float();
	auto ff_damage_bullet_penetration = damageBulletPenetration->get_float();
	auto sv_penetration_type = penetrationSystem->get_bool();

	if (data.enter_trace.fraction == 1.0f)
		return false;

	if (hbp(weaponData, data.enter_trace, local->get_shoot_position(), data.direction, data.penetrate_count, data.current_damage, weaponData->flPenetration, sv_penetration_type, ff_damage_reduction_bullets, ff_damage_bullet_penetration))
	{
		dmg = data.current_damage;
		return true;
	}

	return false;
}