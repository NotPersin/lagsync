#pragma once

#include "../includes.h"
#include "../sdk/c_client_state.h"

class c_net_channel_
{
	typedef bool(__thiscall* send_netmsg_t)(c_net_channel*, c_net_msg&, bool, bool);
public:
	static void hook();
	static void apply_to_net_chan(c_net_channel* channel);
private:
	inline static std::unique_ptr<c_hook<c_net_channel>> hk;
	inline static send_netmsg_t _send_netmsg;
	static bool __fastcall send_netmsg(c_net_channel* channel, uint32_t, c_net_msg& msg, bool reliable, bool voice);
};

