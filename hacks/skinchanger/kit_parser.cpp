#include <algorithm>
#include "../../menu/c_menu.h"
#include "kit_parser.h"
#include "../../sdk/recv.h"
#include "../../sdk/c_base_view_model.h"

typedef void(*RecvVarProxyFn)(const c_recv_proxy_data* pData, void* pStruct, void* pOut);

#define SEQUENCE_DEFAULT_DRAW 0
#define SEQUENCE_DEFAULT_IDLE1 1
#define SEQUENCE_DEFAULT_IDLE2 2
#define SEQUENCE_DEFAULT_LIGHT_MISS1 3
#define SEQUENCE_DEFAULT_LIGHT_MISS2 4
#define SEQUENCE_DEFAULT_HEAVY_MISS1 9
#define SEQUENCE_DEFAULT_HEAVY_HIT1 10
#define SEQUENCE_DEFAULT_HEAVY_BACKSTAB 11
#define SEQUENCE_DEFAULT_LOOKAT01 12

#define SEQUENCE_BUTTERFLY_DRAW 0
#define SEQUENCE_BUTTERFLY_DRAW2 1
#define SEQUENCE_BUTTERFLY_LOOKAT01 13
#define SEQUENCE_BUTTERFLY_LOOKAT03 15

#define SEQUENCE_FALCHION_IDLE1 1
#define SEQUENCE_FALCHION_HEAVY_MISS1 8
#define SEQUENCE_FALCHION_HEAVY_MISS1_NOFLIP 9
#define SEQUENCE_FALCHION_LOOKAT01 12
#define SEQUENCE_FALCHION_LOOKAT02 13

#define SEQUENCE_DAGGERS_IDLE1 1
#define SEQUENCE_DAGGERS_LIGHT_MISS1 2
#define SEQUENCE_DAGGERS_LIGHT_MISS5 6
#define SEQUENCE_DAGGERS_HEAVY_MISS2 11
#define SEQUENCE_DAGGERS_HEAVY_MISS1 12

#define SEQUENCE_BOWIE_IDLE1 1

#define SEQUENCE_CSS1 13
#define SEQUENCE_CSS2 14
using namespace std;
// Helper function to generate a random sequence number.
inline int RandomSequence(int low, int high) {
	return (rand() % (high - low + 1) + low);
}

#define	LIFE_ALIVE 0

#define RandomInt(nMin, nMax) (rand() % (nMax - nMin + 1) + nMin);

RecvVarProxyFn fnSequenceProxyFn = nullptr;
RecvVarProxyFn oRecvnModelIndex;

void Hooked_RecvProxy_Viewmodel(c_recv_proxy_data* pData, void* pStruct, void* pOut)
{
	// Get the knife view model id's
	int default_t = model_info_client()->get_model_index("models/weapons/v_knife_default_t.mdl");
	int default_ct = model_info_client()->get_model_index("models/weapons/v_knife_default_ct.mdl");
	int iBayonet = model_info_client()->get_model_index("models/weapons/v_knife_bayonet.mdl");
	int iButterfly = model_info_client()->get_model_index("models/weapons/v_knife_butterfly.mdl");
	int iFlip = model_info_client()->get_model_index("models/weapons/v_knife_flip.mdl");
	int iGut = model_info_client()->get_model_index("models/weapons/v_knife_gut.mdl");
	int iKarambit = model_info_client()->get_model_index("models/weapons/v_knife_karam.mdl");
	int iM9Bayonet = model_info_client()->get_model_index("models/weapons/v_knife_m9_bay.mdl");
	int iHuntsman = model_info_client()->get_model_index("models/weapons/v_knife_tactical.mdl");
	int iFalchion = model_info_client()->get_model_index("models/weapons/v_knife_falchion_advanced.mdl");
	int iDagger = model_info_client()->get_model_index("models/weapons/v_knife_push.mdl");
	int iBowie = model_info_client()->get_model_index("models/weapons/v_knife_survival_bowie.mdl");
	int iGunGame = model_info_client()->get_model_index("models/weapons/v_knife_gg.mdl");

	int Navaja = model_info_client()->get_model_index("models/weapons/v_knife_gypsy_jackknife.mdl");
	int Stiletto = model_info_client()->get_model_index("models/weapons/v_knife_stiletto.mdl");
	int Ursus = model_info_client()->get_model_index("models/weapons/v_knife_ursus.mdl");
	int Talon = model_info_client()->get_model_index("models/weapons/v_knife_widowmaker.mdl");
	int css = model_info_client()->get_model_index("models/weapons/v_knife_css.mdl");

	// Get local player (just to stop replacing spectators knifes)
	auto  pLocal = c_cs_player::get_local_player();;
	if (!pLocal)
		return;
	if (config->Skinchanger.Enabled && pLocal)
	{
		// If we are alive and holding a default knife(if we already have a knife don't worry about changing)
		if (pLocal->is_alive() && (
			pData->value.m_Int == default_t ||
			pData->value.m_Int == default_ct ||
			pData->value.m_Int == iBayonet ||
			pData->value.m_Int == iFlip ||
			pData->value.m_Int == iGunGame ||
			pData->value.m_Int == iGut ||
			pData->value.m_Int == iKarambit ||
			pData->value.m_Int == iM9Bayonet ||
			pData->value.m_Int == iHuntsman ||
			pData->value.m_Int == iBowie ||
			pData->value.m_Int == iButterfly ||
			pData->value.m_Int == iFalchion ||
			pData->value.m_Int == iDagger ||
			pData->value.m_Int == Navaja ||
			pData->value.m_Int == Stiletto ||
			pData->value.m_Int == Ursus ||
			pData->value.m_Int == Talon ||
			pData->value.m_Int == css))
		{
			// Set whatever knife we want
			if (config->Skinchanger.knife_switch == 0)
				pData->value.m_Int = iBayonet;
			else if (config->Skinchanger.knife_switch == 9)
				pData->value.m_Int = iBowie;
			else if (config->Skinchanger.knife_switch == 6)
				pData->value.m_Int = iButterfly;
			else if (config->Skinchanger.knife_switch == 7)
				pData->value.m_Int = iFalchion;
			else if (config->Skinchanger.knife_switch == 1)
				pData->value.m_Int = iFlip;
			else if (config->Skinchanger.knife_switch == 2)
				pData->value.m_Int = iGut;
			else if (config->Skinchanger.knife_switch == 5)
				pData->value.m_Int = iHuntsman;
			else if (config->Skinchanger.knife_switch == 3)
				pData->value.m_Int = iKarambit;
			else if (config->Skinchanger.knife_switch == 4)
				pData->value.m_Int = iM9Bayonet;
			else if (config->Skinchanger.knife_switch == 8)
				pData->value.m_Int = iDagger;
			else if (config->Skinchanger.knife_switch == 10)
				pData->value.m_Int = Navaja;
			else if (config->Skinchanger.knife_switch == 11)
				pData->value.m_Int = Stiletto;
			else if (config->Skinchanger.knife_switch == 12)
				pData->value.m_Int = Ursus;
			else if (config->Skinchanger.knife_switch == 13)
				pData->value.m_Int = Talon;
			else if (config->Skinchanger.knife_switch == 14)
				pData->value.m_Int = css;
		}
	}
	// Carry on the to original proxy
	oRecvnModelIndex(pData, pStruct, pOut);
}


void SetViewModelSequence2(const c_recv_proxy_data* pDataConst, void* pStruct, void* pOut)
{

	// Make the incoming data editable.
	c_recv_proxy_data* pData = const_cast<c_recv_proxy_data*>(pDataConst);
	if (!pData)
		return;
	// Confirm that we are replacing our view model and not someone elses.
	CBaseViewModel* pViewModel = static_cast<CBaseViewModel*>(pStruct);
	if (!pViewModel)
		return;
	if (pViewModel)
	{
		c_cs_player* pOwner = static_cast<c_cs_player*>(client_entity_list()->get_client_entity(pViewModel->GetOwner() & 0xFFF));
		if (!pOwner)
			return;
		// Compare the owner entity of this view model to the local player entity.
		if (pOwner && pOwner->index() == engine_client()->get_local_player())
		{
			// Get the filename of the current view model.
			std::string szModel = model_info_client()->get_model_name(model_info_client()->get_model(pViewModel->modelindex()));

			// Store the current sequence.
			int m_nSequence = pData->value.m_Int;
			if (szModel == "models/weapons/v_knife_butterfly.mdl")
			{
				// Fix animations for the Butterfly Knife.
				switch (m_nSequence)
				{
				case SEQUENCE_DEFAULT_DRAW:
					m_nSequence = RandomInt(SEQUENCE_BUTTERFLY_DRAW, SEQUENCE_BUTTERFLY_DRAW2);
					break;
				case SEQUENCE_DEFAULT_LOOKAT01:
					m_nSequence = RandomInt(SEQUENCE_BUTTERFLY_LOOKAT01, SEQUENCE_BUTTERFLY_LOOKAT03);
					break;
				default:
					m_nSequence++;
				}
			}
			else if (szModel == "models/weapons/v_knife_falchion_advanced.mdl")
			{
				// Fix animations for the Falchion Knife.
				switch (m_nSequence)
				{
				case SEQUENCE_DEFAULT_IDLE2:
					m_nSequence = SEQUENCE_FALCHION_IDLE1; break;
				case SEQUENCE_DEFAULT_HEAVY_MISS1:
					m_nSequence = RandomInt(SEQUENCE_FALCHION_HEAVY_MISS1, SEQUENCE_FALCHION_HEAVY_MISS1_NOFLIP);
					break;
				case SEQUENCE_DEFAULT_LOOKAT01:
					m_nSequence = RandomInt(SEQUENCE_FALCHION_LOOKAT01, SEQUENCE_FALCHION_LOOKAT02);
					break;
				case SEQUENCE_DEFAULT_DRAW:
				case SEQUENCE_DEFAULT_IDLE1:
					break;
				default:
					m_nSequence--;
				}
			}
			else if (szModel == "models/weapons/v_knife_push.mdl")
			{
				// Fix animations for the Shadow Daggers.
				switch (m_nSequence)
				{
				case SEQUENCE_DEFAULT_IDLE2:
					m_nSequence = SEQUENCE_DAGGERS_IDLE1; break;
				case SEQUENCE_DEFAULT_LIGHT_MISS1:
				case SEQUENCE_DEFAULT_LIGHT_MISS2:
					m_nSequence = RandomInt(SEQUENCE_DAGGERS_LIGHT_MISS1, SEQUENCE_DAGGERS_LIGHT_MISS5);
					break;
				case SEQUENCE_DEFAULT_HEAVY_MISS1:
					m_nSequence = RandomInt(SEQUENCE_DAGGERS_HEAVY_MISS2, SEQUENCE_DAGGERS_HEAVY_MISS1);
					break;
				case SEQUENCE_DEFAULT_HEAVY_HIT1:
				case SEQUENCE_DEFAULT_HEAVY_BACKSTAB:
				case SEQUENCE_DEFAULT_LOOKAT01:
					m_nSequence += 3; break;
				case SEQUENCE_DEFAULT_DRAW:
				case SEQUENCE_DEFAULT_IDLE1:
					break;
				default:
					m_nSequence += 2;
				}
			}
			else if (szModel == "models/weapons/v_knife_survival_bowie.mdl")
			{
				// Fix animations for the Bowie Knife.
				switch (m_nSequence)
				{
				case SEQUENCE_DEFAULT_DRAW:
				case SEQUENCE_DEFAULT_IDLE1:
					break;
				case SEQUENCE_DEFAULT_IDLE2:
					m_nSequence = SEQUENCE_BOWIE_IDLE1;
					break;
				default:
					m_nSequence--;
				}
			}
			else if (szModel == "models/weapons/v_knife_ursus.mdl")
			{
				// Fix animations for the Bowie Knife.
				switch (m_nSequence)
				{
				case SEQUENCE_DEFAULT_DRAW:
					m_nSequence = RandomInt(SEQUENCE_BUTTERFLY_DRAW, SEQUENCE_BUTTERFLY_DRAW2);
					break;
				case SEQUENCE_DEFAULT_LOOKAT01:
					m_nSequence = RandomInt(SEQUENCE_BUTTERFLY_LOOKAT01, SEQUENCE_BUTTERFLY_LOOKAT03);
					break;
				default:
					m_nSequence++;
					break;
				}
			}
			else if (szModel == "models/weapons/v_knife_stiletto.mdl")
			{
				// Fix animations for the Bowie Knife.
				switch (m_nSequence)
				{
				case SEQUENCE_DEFAULT_LOOKAT01:
					m_nSequence = RandomInt(12, 13);
					break;
				}
			}
			else if (szModel == "models/weapons/v_knife_widowmaker.mdl")
			{
				// Fix animations for the Bowie Knife.
				switch (m_nSequence)
				{
				case SEQUENCE_DEFAULT_LOOKAT01:
					m_nSequence = RandomInt(14, 15);
					break;
				}
			}
			else if (szModel == "models/weapons/v_knife_css.mdl")
			{
				// Fix animations for the Bowie Knife.
				switch (m_nSequence)
				{
				case SEQUENCE_CSS1:
					m_nSequence = RandomInt(13, 14);
					break;
				case SEQUENCE_CSS2:
					m_nSequence = RandomInt(13, 14);
					break;
				}
			}
			pData->value.m_Int = m_nSequence;
		}
	}
	fnSequenceProxyFn(pData, pStruct, pOut);
}

void AnimationFixHook()
{
	for (c_client_class* pClass = base_client()->get_all_classes(); pClass; pClass = pClass->next)
	{
		if (!pClass)
			return;
		if (!strcmp(pClass->network_name, "CBaseViewModel")) {
			// Search for the 'm_nModelIndex' property.
			recv_table* pClassTable = pClass->recv_table;

			for (int nIndex = 0; nIndex < pClassTable->count; nIndex++) {
				recv_prop* pProp = &pClassTable->props[nIndex];

				if (!pProp || strcmp(pProp->name, "m_nSequence"))
					continue;

				fnSequenceProxyFn = (RecvVarProxyFn)pProp->proxy_fn;

				pProp->proxy_fn = (recv_var_proxy_fn)SetViewModelSequence2;

				break;
			}

			break;
		}
	}
}


void NetvarHook()
{


	AnimationFixHook();
	c_client_class* pClass = base_client()->get_all_classes();
	if (!pClass)
		return;
	while (pClass)
	{
		const char* pszName = pClass->recv_table->table_name;

		for (int i = 0; i < pClass->recv_table->count; i++)
		{
			recv_prop* pProp = &(pClass->recv_table->props[i]);
			const char* name = pProp->name;

			if (!strcmp(pszName, "DT_BaseViewModel"))
			{
				// Knives
				if (!strcmp(name, "m_nModelIndex"))
				{
					oRecvnModelIndex = (RecvVarProxyFn)pProp->proxy_fn;
					pProp->proxy_fn = Hooked_RecvProxy_Viewmodel;
				}
			}

		}

		pClass = pClass->next;
	}
}

