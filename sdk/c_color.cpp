#include "c_color.h"

std::optional<int> c_color::alpha_override = std::nullopt;

c_color::c_color() : c_color(0, 0, 0, 255) {}

c_color::c_color(const int red, const int green, const int blue) : c_color(red, green, blue, 255) { }

c_color::c_color(const int red, const int green, const int blue, const int alpha) : red(red), green(green), blue(blue), alpha(alpha) { }

c_color::~c_color() = default;

D3DCOLOR c_color::direct() const
{
	return D3DCOLOR_ARGB(static_cast<int>(alpha * alpha_override.value_or(255) / 255.f), red, green, blue);
}

c_color c_color::fade_to(const c_color color, const float percent, const bool only_alpha) const
{
	const auto new_alpha = static_cast<int>(((1.0f - percent) * alpha + percent * color.alpha + 0.5)
										* alpha_override.value_or(255) / 255.f);

	if (only_alpha)
		return c_color(red, green, blue, new_alpha);

	const auto hsv = to_hsv();
	const auto hsv_target = color.to_hsv();

	auto hsv_current = std::make_tuple(
		fmod(static_cast<float>((1.0f - percent) * std::get<0>(hsv) + percent * std::get<0>(hsv_target)), 180.f),
		fmod(static_cast<float>((1.0f - percent) * std::get<1>(hsv) + percent * std::get<1>(hsv_target)), 255.f),
		fmod(static_cast<float>((1.0f - percent) * std::get<2>(hsv) + percent * std::get<2>(hsv_target)), 255.f)
	);

	auto new_color = from_hsv(hsv_current);
	new_color.alpha = new_alpha;
	return new_color;
}

c_color c_color::adjust_hue(const float delta) const
{
	auto hsv = to_hsv();
	std::get<0>(hsv) = fmod(std::get<0>(hsv) + delta, 180.f);

	auto new_color = from_hsv(hsv);
	new_color.alpha = alpha;
	return new_color;
}

int c_color::get_alpha_override()
{
	return alpha_override.value_or(255);
}

void c_color::set_alpha_override(const std::optional<int> alpha)
{
	alpha_override = alpha;
}

float c_color::get_hue() const
{
	const auto r = red / 255.0f;
	const auto g = green / 255.0f;
	const auto b = blue / 255.0f;
	const auto max = fmaxf(r, fmaxf(g, b));
	const auto min = fminf(r, fminf(g, b));

	float h, s;
	const auto v = max;

	if (max == 0.0f) {
		s = 0;
		h = 0;
	}
	else if (max - min == 0.0f) {
		s = 0;
		h = 0;
	}
	else {
		s = (max - min) / max;

		if (max == r)
			h = 60 * ((g - b) / (max - min)) + 0;
		else if (max == g)
			h = 60 * ((b - r) / (max - min)) + 120;
		else
			h = 60 * ((r - g) / (max - min)) + 240;
	}

	if (h < 0)
		h += 360.0f;

	return h / 2;
}

std::tuple<float, float, float> c_color::to_hsv() const
{
	const auto r = red / 255.0f;
	const auto g = green / 255.0f;
	const auto b = blue / 255.0f;
	const auto max = fmaxf(r, fmaxf(g, b));
	const auto min = fminf(r, fminf(g, b));

	float h, s;
	const auto v = max;

    if (max == 0.0f) {
        s = 0;
        h = 0;
    }
    else if (max - min == 0.0f) {
        s = 0;
        h = 0;
    }
    else {
        s = (max - min) / max;

        if (max == r)
            h = 60 * ((g - b) / (max - min)) + 0;
        else if (max == g)
            h = 60 * ((b - r) / (max - min)) + 120;
        else
            h = 60 * ((r - g) / (max - min)) + 240;
    }

    if (h < 0)
		h += 360.0f;

	return std::make_tuple(h / 2, s * 255, v * 255);
}

c_color c_color::from_hsv(std::tuple<float, float, float>& hsv) const
{
	const auto h = std::get<0>(hsv) * 2.0f;
	const auto s = std::get<1>(hsv) / 255.f;
	const auto v = std::get<2>(hsv) / 255.f;

    auto r = .0f, g = .0f, b = .0f;

	const auto hi = static_cast<int>(h / 60.0f) % 6;
	const auto f  = h / 60.0f - hi;
	const auto p  = v * (1.0f - s);
	const auto q  = v * (1.0f - s * f);
	const auto t  = v * (1.0f - s * (1.0f - f));

    switch(hi) {
        case 0: r = v, g = t, b = p; break;
        case 1: r = q, g = v, b = p; break;
        case 2: r = p, g = v, b = t; break;
        case 3: r = p, g = q, b = v; break;
        case 4: r = t, g = p, b = v; break;
        case 5: r = v, g = p, b = q; break;
		default: break;
    }

	return c_color(static_cast<unsigned char>(r * 255),
				   static_cast<unsigned char>(g * 255),
				   static_cast<unsigned char>(b * 255));
}

c_color c_color::from_hsb(float hue, float saturation, float brightness)
{
	float h = hue == 1.0f ? 0 : hue * 6.0f;
	float f = h - (int)h;
	float p = brightness * (1.0f - saturation);
	float q = brightness * (1.0f - saturation * f);
	float t = brightness * (1.0f - (saturation * (1.0f - f)));

	if (h < 1)
	{
		return c_color(
			(unsigned char)(brightness * 255),
			(unsigned char)(t * 255),
			(unsigned char)(p * 255)
		);
	}
	else if (h < 2)
	{
		return c_color(
			(unsigned char)(q * 255),
			(unsigned char)(brightness * 255),
			(unsigned char)(p * 255)
		);
	}
	else if (h < 3)
	{
		return c_color(
			(unsigned char)(p * 255),
			(unsigned char)(brightness * 255),
			(unsigned char)(t * 255)
		);
	}
	else if (h < 4)
	{
		return c_color(
			(unsigned char)(p * 255),
			(unsigned char)(q * 255),
			(unsigned char)(brightness * 255)
		);
	}
	else if (h < 5)
	{
		return c_color(
			(unsigned char)(t * 255),
			(unsigned char)(p * 255),
			(unsigned char)(brightness * 255)
		);
	}
	else
	{
		return c_color(
			(unsigned char)(brightness * 255),
			(unsigned char)(p * 255),
			(unsigned char)(q * 255)
		);
	}
}

c_color c_color::fuckloki(float h, float s, float v)
{
	float colOut[3]{ };
	if (s == 0.0f)
	{
		red = green = blue = static_cast<int>(v * 255);
		return *this;
	}

	h = std::fmodf(h, 1.0f) / (60.0f / 360.0f);
	int   i = static_cast<int>(h);
	float f = h - static_cast<float>(i);
	float p = v * (1.0f - s);
	float q = v * (1.0f - s * f);
	float t = v * (1.0f - s * (1.0f - f));

	switch (i)
	{
	case 0:
		colOut[0] = v;
		colOut[1] = t;
		colOut[2] = p;
		break;
	case 1:
		colOut[0] = q;
		colOut[1] = v;
		colOut[2] = p;
		break;
	case 2:
		colOut[0] = p;
		colOut[1] = v;
		colOut[2] = t;
		break;
	case 3:
		colOut[0] = p;
		colOut[1] = q;
		colOut[2] = v;
		break;
	case 4:
		colOut[0] = t;
		colOut[1] = p;
		colOut[2] = v;
		break;
	case 5: default:
		colOut[0] = v;
		colOut[1] = p;
		colOut[2] = q;
		break;
	}

	red = static_cast<int>(colOut[0] * 255);
	green = static_cast<int>(colOut[1] * 255);
	blue = static_cast<int>(colOut[2] * 255);
	return *this;
}