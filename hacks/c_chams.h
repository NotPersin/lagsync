#pragma once

#include <functional>
#include "../utils/c_config.h"
#include "../sdk/c_cs_player.h"
#include "../menu/c_menu.h"

class c_chams : public c_singleton<c_chams>
{
public:
	c_chams();

	void latch_timer();

	static void draw_players();
	static void draw_cs_entities();
	static bool get_backtrack_matrix(c_cs_player* player, matrix3x4* out);
	static bool get_simple_backtrack_matrix(c_cs_player* player, matrix3x4* out);
	static c_material* GetMat(int material);

	static c_material* GetOverlay(int material);



	static void CustomMaterial(c_material* mat);

	static void player_chams(std::function<void()> original, Menu22::visaul::esp& conf, bool draw_fake, bool scope_blend = false);
	static void player_chams(Menu22::visaul::esp& conf, c_cs_player* entity, bool draw_fake, bool scope_blend);
	void player_chams_fakelag(const std::function<void()> original, Menu22::visaul::esp& conf, bool draw_fake, bool scope_blend);
	static void hand_chams(const std::function<void()> original, Menu22::visaul::esp& conf, Menu22::visaul::esp& conf2, bool draw_hand);
	static void weapon_chams(const std::function<void()> original, Menu22::visaul::esp& conf, Menu22::visaul::esp& conf2, bool draw_weapon);
	static void set_ignorez(bool enabled, c_material* mat);
	static void on_player_hurt(c_game_event* event);
	c_cs_player* current_player;
	matrix3x4* current_matrix;
	bool second_pass;
private:
	static void modulate(c_color color, Menu22::visaul::esp& conf, c_material* material = nullptr, bool xyz= false);
	static void modulate_exp(c_material* material, float alpha = 1.f, float width = 6.f);
	static void modulate_reflectivity(c_material* material);
	int alpha;
	bool direction; 
	static bool did_hit;
}; extern c_chams chams;