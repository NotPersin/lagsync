#pragma once

#include "../includes.h"
#include "../menu/c_menu.h"
#include "../sdk/c_client_state.h"

class c_antiaim : public c_singleton<c_antiaim>
{
public:
	void fakelag(c_cs_player* local, c_user_cmd* cmd, bool& send_packet);
	void run(c_cs_player* local, c_user_cmd* cmd);
	void prepare_animation(c_cs_player* local);
	void predict(c_cs_player* local, c_user_cmd* cmd);

	float get_visual_choke();
	void increment_visual_progress();

	float get_pitch(Menu22::ragebot::antiaim_config& conf);
	float at_target(c_cs_player* local) const;
	float get_last_real();
	float get_last_fake();
	float get_last_max() { return max_delta; };
	float get_last_lby();
	int get_last_choke() { return estimated_choke; };
	void set_last_shot(bool statment) { real_shot = statment;  };
	bool get_last_shot() { return real_shot; };
	bool get_last_sendpack() { return sendpacket; };
	c_vector3d get_last_view() { return view_angles; }

	uint32_t shot_cmd{};

	matrix3x4 last_fake_matrix[128];
	matrix3x4 last_fakelag_matrix[128];

	bool is_fakeducking = false;
	float _manual_yaw = 0.f;
	bool is_freestanding = false;
private:
	bool on_peek(c_cs_player* local, bool& target);
	float calculate_ideal_yaw(c_cs_player* local, bool estimate = false);

	float at_target( c_user_cmd* cmd, c_cs_player* local) const;

	float visual_choke = 0.f,
		last_real = 0.f, last_fake = 0.f, last_fakelag = 0.f,last_lby = 0.f,
		next_lby_update = 0.f, lby_update = 0.f,
		min_delta = 0.f, max_delta = 0.f,
		stop_to_full_running_fraction = 0.f,
		feet_speed_stand = 0.f, feet_speed_ducked = 0.f;
	int tick = 0, tick2 = 0;
	bool inied = false, jitter, freestand, real_shot, sendpacket;
	c_vector3d view_angles;

	uint32_t estimated_choke = 0;
};

#define antiaim c_antiaim::instance()
