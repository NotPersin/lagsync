#include "c_log.h"
#include "../c_rifk.h"
#include "../sdk/c_cvar.h"
#include "welcome_message.h"
#include <sstream>
#include "../menu/c_menu.h"
#include "../hooks/c_surface_.h"
#include "../loader_info.h"
#include "../main.h"
#include "../hacks/c_esp.h"

c_log::c_log()
{
}

struct esp_logs {
	char text[42]{};
	int alpha = 155;
	float curtime;

};

int showtime = 20;

std::vector<esp_logs> esp_log;

void c_log::draw_logs() {

	int w, h;
	engine_client()->get_screen_size(w, h);
	c_vector2d position = c_vector2d(1, 1);

	auto draw_text = [](const char* text, c_vector2d pos, int x_offset, c_color Color) {
		Drawing::DrawString(F::ESP, pos.x + x_offset, pos.y + 2, Color, false, text);
	};

	float time = global_vars_base->curtime;

	for (int i = 0; i < esp_log.size(); i++)
	{
		if (esp_log.empty())
			return;

		bool expired = (time >= (esp_log.at(i).curtime + 1.5f));

		if (expired)
			esp_log.at(i).alpha -= 1;

		if (!expired) {
			auto size = Drawing::GetTextSize( fonts::ESP, esp_log.at(i).text);
			int width = size.right - size.left, height = size.bottom - size.top;
			Drawing::DrawRect(position.x + esp_log.at(i).alpha - 153, position.y, width + 2, 20, c_color(25, 25, 25,245));
			Drawing::DrawRect(position.x + esp_log.at(i).alpha - 153, position.y + 17, width + 2, 3, c_color(config->Misc.menucolor[0] * 255, config->Misc.menucolor[1] * 255, config->Misc.menucolor[2] * 255, esp_log.at(i).alpha));
			Drawing::DrawRect(position.x + esp_log.at(i).alpha - 153, position.y, 1, 20, c_color(config->Misc.menucolor[0] * 255, config->Misc.menucolor[1] * 255, config->Misc.menucolor[2] * 255, esp_log.at(i).alpha));

			draw_text(esp_log.at(i).text, c_vector2d(position.x + esp_log.at(i).alpha - 153, position.y), 3, c_color(238, 238, 238, esp_log.at(i).alpha));
			position.y += 20;	
		}

		if (expired && esp_log.at(i).alpha <= 0) {
			esp_log.erase(esp_log.begin() + i);
			continue;
		}
	}
}

void c_log::add_log(std::string& text) {


	esp_logs toadd;
	memcpy(toadd.text, text.c_str(), 42);
	toadd.text[41] = '\0';
	const float curtime = global_vars_base->curtime;
	toadd.curtime = curtime;
	esp_log.push_back(toadd);
}

void c_log::clear_log() {

	if (esp_log.size() > 0)
		esp_log.clear();

	esp_log.clear();
}

void c_log::debug(const std::string& message)
{
#ifndef RELEASE
	print(logtype::debug, message);
#endif
}

void c_log::info(const std::string& message)
{
	print(logtype::info, message);
}

void c_log::warning(const std::string& message)
{
	print(logtype::warning, message);
}

void c_log::error(const std::string& message)
{
	print(logtype::error, message);
}

std::vector<_event> _events;

void c_log::logs()
{
	if (_events.empty() || !config->Misc.eventlogs)
		return;

	int x = 8;
	int y = 5;
	auto count = 0;
	int fontTall = surface()->GetFontTall(F::Events);
	fontTall += 1;
	c_color color = c_color(config->Misc.menucolor[0] * 255, config->Misc.menucolor[1] * 255, config->Misc.menucolor[2] * 255, 255);

	for (auto& event : _events)
	{
		if (_events.back()._time < global_vars_base->realtime)
			_events.pop_back();

		if (event._time < global_vars_base->realtime)
			continue;

		if (count > 10)
			break;

		if (event._msg.size() > 0)
		{
			if (!event._displayed)
			{
				cvar()->console_color_printf(false, color, "[lagsync]");
				cvar()->console_color_printf(false, c_color(255,255,255), " %s\n", event._msg.c_str());

				event._displayed = true;
			}

			c_color clr = c_color(255, 255, 255);

			float timeleft = fabs(event._time - global_vars_base->realtime);

			if (timeleft < .5f)
			{
				float f = std::clamp(timeleft, 0.0f, .5f) / .5f;

				clr.alpha = (int)(f * 255.0f);

				if (count == 0 && f < 0.2f)
				{
					y -= fontTall * (1.0f - f / 0.2f);
				}
			}
			else
			{
				clr.alpha = 255;
			}

			Drawing::DrawString(F::Events, x, y, clr, false, "%s", event._msg.c_str());

			y += fontTall;

			count++;
		}
	}
}


void c_log::print(const logtype type, const std::string& message)
{
	//if (_config->Misc.events == 0)
		return;

	std::stringstream stream;
	c_color color = c_color(config->Misc.menucolor[0] * 255,config->Misc.menucolor[1] * 255, config->Misc.menucolor[2] * 255, 255 );

	/*switch (type) {
	case logtype::debug:
		stream << _("[debug");
		break;
	case logtype::info: 
		stream << _("[desync");
		color = { (int)(_config->Misc.menucolor[0] * 255), (int)(_config->Misc.menucolor[1] * 255),  (int)(_config->Misc.menucolor[2] * 255) };	
		break;
	case logtype::warning:
		stream << _("[warning");
		color = { 255, 70, 0 };
		break;
	case logtype::error:
		stream << _("[error");
		color = { 255, 0, 0 };
		break;
	}*/

	cvar()->find_var(_("developer"))->set_value(0);

	if (config->Misc.events == 2) {
		add_log(stream.str());
	}
	else 

	cvar()->console_color_printf(true, color, stream.str().c_str());
}
