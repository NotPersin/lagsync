#pragma once
#include "c_base_animating.h"
#include "../sdk/recv_prop_hook.h"
#include "../sdk/c_base_animating.h"
#include "../sdk/c_cs_player.h"

class env_tonemap_control {
public:
	netprop(custom_use_bloom_scale(), "CEnvTonemapController", "m_bUseCustomBloomScale")
	netprop(custom_bloom_scale(), "CEnvTonemapController", "m_flCustomBloomScale")

	netprop(UseCustomAutoExposureMin(), "CEnvTonemapController", "m_bUseCustomAutoExposureMin")
	netprop(UseCustomAutoExposureMax(), "CEnvTonemapController", "m_bUseCustomAutoExposureMax")

	netprop(CustomAutoExposureMin(), "CEnvTonemapController", "m_flCustomAutoExposureMin")
	netprop(CustomAutoExposureMax(), "CEnvTonemapController", "m_flCustomAutoExposureMax")
};

class env_tonemap_controllor {
private:
	inline static recv_var_proxy_fn _m_bUseCustomBloomScale;
	inline static recv_var_proxy_fn _m_flCustomBloomScale;

	static void __cdecl m_bUseCustomBloomScale(c_recv_proxy_data* data, void* entity, void* output);
	static void __cdecl m_flCustomBloomScale(c_recv_proxy_data* data, void* entity, void* output);

	inline static recv_var_proxy_fn _m_bUseCustomAutoExposureMin;
	inline static recv_var_proxy_fn _m_bUseCustomAutoExposureMax;

	static void __cdecl m_bUseCustomAutoExposureMin(c_recv_proxy_data* data, void* entity, void* output);
	static void __cdecl m_bUseCustomAutoExposureMax(c_recv_proxy_data* data, void* entity, void* output);

	inline static recv_var_proxy_fn _m_flCustomAutoExposureMin;
	inline static recv_var_proxy_fn _m_flCustomAutoExposureMax;

	static void __cdecl m_flCustomAutoExposureMin(c_recv_proxy_data* data, void* entity, void* output);
	static void __cdecl m_flCustomAutoExposureMax(c_recv_proxy_data* data, void* entity, void* output);
public:
	static void hook();
};
