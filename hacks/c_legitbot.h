#pragma once
#include "../includes.h"

class c_legitbot
{
public:
	static bool aim(c_user_cmd* cmd);
	static void smooth(c_qangle& angle, c_qangle view_angle);
	static void bunnyhop(c_user_cmd* cmd);
private:
};