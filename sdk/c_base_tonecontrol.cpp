#include "c_base_tonecontrol.h"
#include "../new_menu/config.h"

void env_tonemap_controllor::hook()
{
	static auto custom_use_bloom_scale = recv_prop_hook(env_tonemap_control::custom_use_bloom_scale());
	_m_bUseCustomBloomScale = custom_use_bloom_scale.hook_prop(m_bUseCustomBloomScale);

	static auto custom_bloom_scale = recv_prop_hook(env_tonemap_control::custom_bloom_scale());
	_m_flCustomBloomScale = custom_bloom_scale.hook_prop(m_flCustomBloomScale);

	//static auto UseCustomAutoExposureMin = recv_prop_hook(env_tonemap_control::UseCustomAutoExposureMin());
	//_m_bUseCustomAutoExposureMin = UseCustomAutoExposureMin.hook_prop(m_bUseCustomAutoExposureMin);
	//static auto UseCustomAutoExposureMax = recv_prop_hook(env_tonemap_control::UseCustomAutoExposureMax());
	//_m_bUseCustomAutoExposureMax = UseCustomAutoExposureMax.hook_prop(m_bUseCustomAutoExposureMax);

	//static auto CustomAutoExposureMin = recv_prop_hook(env_tonemap_control::CustomAutoExposureMin());
	//_m_flCustomAutoExposureMin = CustomAutoExposureMin.hook_prop(m_bUseCustomAutoExposureMin);
	//static auto CustomAutoExposureMax = recv_prop_hook(env_tonemap_control::CustomAutoExposureMax());
	//_m_flCustomAutoExposureMax = CustomAutoExposureMax.hook_prop(m_bUseCustomAutoExposureMax);
}

void env_tonemap_controllor::m_bUseCustomBloomScale(c_recv_proxy_data* data, void* entity, void* output)
{
	const auto local = c_cs_player::get_local_player();

	if (!local)
		return _m_bUseCustomBloomScale(data, entity, output);

	data->value.m_Int = 1;

	_m_bUseCustomBloomScale(data, entity, output);
}

void env_tonemap_controllor::m_flCustomBloomScale(c_recv_proxy_data* data, void* entity, void* output)
{
	const auto local = c_cs_player::get_local_player();

	if (!local)
		return _m_flCustomBloomScale(data, entity, output);

	static float iBloomValue = 1;
	if (iBloomValue != config->Visuals.bloom_scale) {
		data->value.m_Float = config->Visuals.bloom_scale;
		iBloomValue = config->Visuals.bloom_scale;
	}
	_m_flCustomBloomScale(data, entity, output);
}

void env_tonemap_controllor::m_bUseCustomAutoExposureMin(c_recv_proxy_data* data, void* entity, void* output)
{
	const auto local = c_cs_player::get_local_player();

	if (!local)
		return _m_bUseCustomAutoExposureMin(data, entity, output);

	data->value.m_Int = 1;

	_m_bUseCustomAutoExposureMin(data, entity, output);
}


void env_tonemap_controllor::m_bUseCustomAutoExposureMax(c_recv_proxy_data* data, void* entity, void* output)
{
	const auto local = c_cs_player::get_local_player();

	if (!local)
		return _m_bUseCustomAutoExposureMax(data, entity, output);

	data->value.m_Int = 1;

	_m_bUseCustomAutoExposureMax(data, entity, output);
}

void env_tonemap_controllor::m_flCustomAutoExposureMin(c_recv_proxy_data* data, void* entity, void* output)
{
	const auto local = c_cs_player::get_local_player();

	if (!local)
		return _m_flCustomAutoExposureMin(data, entity, output);

	static float iBloomValue = 1;
	if (iBloomValue != config->Visuals.bloom_cvar_scale) {
		data->value.m_Float = config->Visuals.bloom_expose  *0.001;
		iBloomValue = config->Visuals.bloom_cvar_scale;
	}
	_m_flCustomAutoExposureMin(data, entity, output);
}

void env_tonemap_controllor::m_flCustomAutoExposureMax(c_recv_proxy_data* data, void* entity, void* output)
{
	const auto local = c_cs_player::get_local_player();

	if (!local)
		return _m_flCustomAutoExposureMax(data, entity, output);

	static float iBloomValue = 1;
	if (iBloomValue != config->Visuals.bloom_expose) {
		data->value.m_Float = config->Visuals.bloom_expose  *0.001;
		iBloomValue = config->Visuals.bloom_expose;
	}
	_m_flCustomAutoExposureMax(data, entity, output);
}