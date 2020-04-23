#pragma once

#include "macros.h"
#include "../sdk/c_client_entity.h"

struct player_info {
private:
	__int64 unknown = 0;
public:
	union
	{
		__int64 steam_id64;
		struct
		{
			__int32 xuid_low;
			__int32 xuid_high;
		};
	};

	char name[128];
	int user_id;
	char steam_id[20];
private:
	char pad[0x10]{};
	unsigned long steam_id_2 = 0;
public:
	char friends_name[128];
	bool fakeplayer;
	bool ishltv;
	unsigned int customfiles[4];
	unsigned char filesdownloaded;
};

class c_event_info {
public:
	enum {
		EVENT_INDEX_BITS = 8,
		EVENT_DATA_LEN_BITS = 11,
		MAX_EVENT_DATA = 192,  // ( 1<<8 bits == 256, but only using 192 below )
	};

	// 0 implies not in use
	short					m_class_id;
	float					m_fire_delay;
	const void* m_send_table;
	const client_class* m_client_class;
	int						m_bits;
	uint8_t* m_data;
	int						m_flags;
	char					pad[0x18];
	c_event_info* m_next;
};

class c_engine_client {
protected:
	~c_engine_client() = default;
	vfunc(18, get_view_angles(c_qangle& angle), void(__thiscall*)(c_engine_client*, c_qangle&))(angle)
public:
	vfunc(5, get_screen_size(int& width, int& height), void(__thiscall*)(c_engine_client*, int&, int&))(width, height)
		vfunc(8, get_player_info(const uint32_t index, player_info* info), bool(__thiscall*)(c_engine_client*, uint32_t, player_info*))(index, info)
		vfunc(9, get_player_for_user_id(const int index), int(__thiscall*)(c_engine_client*, int))(index)
		vfunc(12, get_local_player(), uint32_t(__thiscall*)(c_engine_client*))()
		vfunc(19, set_view_angles(c_qangle& angle), void(__thiscall*)(c_engine_client*, c_qangle&))(angle)
		vfunc(20, get_max_clients(), uint32_t(__thiscall*)(c_engine_client*))()
		vfunc(26, is_ingame(), bool(__thiscall*)(c_engine_client*))()
		vfunc(27, is_connected(), bool(__thiscall*)(c_engine_client*))()
		vfunc(104, get_engine_build_number(), int(__thiscall*)(c_engine_client*))()
		vfunc(114, clientcmd_unrestricted(const char* str, const char* flag), void(__thiscall*)(c_engine_client*, const char*, const char*))(str, flag)
		vfunc(37, get_matrix(), VMatrix& (__thiscall*)(c_engine_client*))()

		c_qangle get_view_angles()
	{
		c_qangle angle;
		get_view_angles(angle);
		return angle;
	}
};

interface_var(c_engine_client, engine_client, "engine.dll", "VEngineClient")