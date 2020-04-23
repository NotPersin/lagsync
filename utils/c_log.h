#pragma once

#include "c_singleton.h"
#include <string>
#include "../sdk/c_global_vars_base.h"
#include <vector>
struct _event
{
	_event(const std::string& msg)
	{
		_msg = msg;
		_displayed = false;
		_time = global_vars_base->realtime + 4.f;
	}

	bool _displayed = false;
	float _time = 0;
	std::string _msg = "";
};

extern std::vector<_event> _events;

class c_log : public c_singleton<c_log>
{
	enum class logtype { debug, info, warning, error };

public:
	c_log();

	static void draw_logs();
	static void add_log(std::string& text);
	static void clear_log();

	static void debug(const std::string& message);
	static void info(const std::string& message);
	static void warning(const std::string& message);
	static void error(const std::string& message);

	static void logs();

private:
	static void print(logtype type, const std::string& message);
};

#define logging c_log::instance()
