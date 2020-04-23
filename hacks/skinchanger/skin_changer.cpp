#pragma once
#pragma once
#include "../../includes.h"
#include "../../new_menu/config.h"
#include <unordered_map>
#define RandomInt(nMin, nMax) (rand() % (nMax - nMin + 1) + nMin);
std::unordered_map<char*, char*> killIcons = {};
#define INVALID_EHANDLE_INDEX 0xFFFFFFFF
c_base_handle worldmodel_handle;
c_base_combat_weapon* worldmodel;


void SkinChanger2()
{
	auto pLocal = c_cs_player::get_local_player();
	if (!pLocal)
		return;
	auto weapons = pLocal->m_hMyWeapons();
	for (size_t i = 0; weapons[i] != INVALID_EHANDLE_INDEX; i++)
	{
		c_cs_player* pEntity = (c_cs_player*)client_entity_list()->get_client_entity_from_handle(weapons[i]);
		if (pEntity)
		{
			c_base_combat_weapon* pWeapon = (c_base_combat_weapon*)pEntity;
			if (config->Skinchanger.Enabled)
			{
				int Model = config->Skinchanger.knife_switch;
				int weapon = *pWeapon->GetItemDefenitionIndex();
				switch (weapon)
				{

				case weapon_awp:
				{
					int awp_kit = config->Skinchanger.awp.id;
					int awp_seed = config->Skinchanger.awp.seed;
					int awp_qaultiy = config->Skinchanger.awp.quality;
					int awp_stattrack = config->Skinchanger.awp.stattrak;

					*pWeapon->FallbackPaintKit() = awp_kit;
					*pWeapon->FallbackSeed() = awp_seed;
					*pWeapon->FallbackWear() = awp_qaultiy / 100;
					if (awp_stattrack != 0) 					*pWeapon->FallbackStatTrak() = awp_stattrack;
				}
				break;
				case weapon_usp_silencer:
				{
					int awp_kit = config->Skinchanger.usp.id;
					int awp_seed = config->Skinchanger.usp.seed;
					int awp_qaultiy = config->Skinchanger.usp.quality;
					int awp_stattrack = config->Skinchanger.usp.stattrak;

					*pWeapon->FallbackPaintKit() = awp_kit;
					*pWeapon->FallbackSeed() = awp_seed;
					*pWeapon->FallbackWear() = awp_qaultiy / 100;
					if (awp_stattrack != 0) 					*pWeapon->FallbackStatTrak() = awp_stattrack;
				}
				break;
				case weapon_hkp2000:
				{
					int awp_kit = config->Skinchanger.p2k.id;
					int awp_seed = config->Skinchanger.p2k.seed;
					int awp_qaultiy = config->Skinchanger.p2k.quality;
					int awp_stattrack = config->Skinchanger.p2k.stattrak;

					*pWeapon->FallbackPaintKit() = awp_kit;
					*pWeapon->FallbackSeed() = awp_seed;
					*pWeapon->FallbackWear() = awp_qaultiy / 100;
					if (awp_stattrack != 0) 					*pWeapon->FallbackStatTrak() = awp_stattrack;
				}
				break;
				case weapon_glock:
				{
					int awp_kit = config->Skinchanger.glock.id;
					int awp_seed = config->Skinchanger.glock.seed;
					int awp_qaultiy = config->Skinchanger.glock.quality;
					int awp_stattrack = config->Skinchanger.glock.stattrak;

					*pWeapon->FallbackPaintKit() = awp_kit;
					*pWeapon->FallbackSeed() = awp_seed;
					*pWeapon->FallbackWear() = awp_qaultiy / 100;
					if (awp_stattrack != 0) 					*pWeapon->FallbackStatTrak() = awp_stattrack;
				}
				break;
				case weapon_deagle:
				{
					int awp_kit = config->Skinchanger.deg.id;
					int awp_seed = config->Skinchanger.deg.seed;
					int awp_qaultiy = config->Skinchanger.deg.quality;
					int awp_stattrack = config->Skinchanger.deg.stattrak;

					*pWeapon->FallbackPaintKit() = awp_kit;
					*pWeapon->FallbackSeed() = awp_seed;
					*pWeapon->FallbackWear() = awp_qaultiy / 100;
					if (awp_stattrack != 0) 					*pWeapon->FallbackStatTrak() = awp_stattrack;
				}
				break;
				case weapon_revolver: // CZ-75
				{
					int awp_kit = config->Skinchanger.revol.id;
					int awp_seed = config->Skinchanger.revol.seed;
					int awp_qaultiy = config->Skinchanger.revol.quality;
					int awp_stattrack = config->Skinchanger.revol.stattrak;

					*pWeapon->FallbackPaintKit() = awp_kit;
					*pWeapon->FallbackSeed() = awp_seed;
					*pWeapon->FallbackWear() = awp_qaultiy / 100;
					if (awp_stattrack != 0) 					*pWeapon->FallbackStatTrak() = awp_stattrack;

				}
				break;
				case weapon_ssg08: // Five-7
				{
					int awp_kit = config->Skinchanger.ssg.id;
					int awp_seed = config->Skinchanger.ssg.seed;
					int awp_qaultiy = config->Skinchanger.ssg.quality;
					int awp_stattrack = config->Skinchanger.ssg.stattrak;

					*pWeapon->FallbackPaintKit() = awp_kit;
					*pWeapon->FallbackSeed() = awp_seed;
					*pWeapon->FallbackWear() = awp_qaultiy / 100;
					if (awp_stattrack != 0) 					*pWeapon->FallbackStatTrak() = awp_stattrack;
				}
				break;
				case weapon_elite: // tec9
				{
					int awp_kit = config->Skinchanger.dual.id;
					int awp_seed = config->Skinchanger.dual.seed;
					int awp_qaultiy = config->Skinchanger.dual.quality;
					int awp_stattrack = config->Skinchanger.dual.stattrak;

					*pWeapon->FallbackPaintKit() = awp_kit;
					*pWeapon->FallbackSeed() = awp_seed;
					*pWeapon->FallbackWear() = awp_qaultiy / 100;
					if (awp_stattrack != 0) 					*pWeapon->FallbackStatTrak() = awp_stattrack;
				}
				break;
				case weapon_p250:
				{
					int awp_kit = config->Skinchanger.p250.id;
					int awp_seed = config->Skinchanger.p250.seed;
					int awp_qaultiy = config->Skinchanger.p250.quality;
					int awp_stattrack = config->Skinchanger.p250.stattrak;

					*pWeapon->FallbackPaintKit() = awp_kit;
					*pWeapon->FallbackSeed() = awp_seed;
					*pWeapon->FallbackWear() = awp_qaultiy / 100;
					if (awp_stattrack != 0) 					*pWeapon->FallbackStatTrak() = awp_stattrack;
				}
				break;
				case weapon_cz75a:
				{
					int awp_kit = config->Skinchanger.cz75.id;
					int awp_seed = config->Skinchanger.cz75.seed;
					int awp_qaultiy = config->Skinchanger.cz75.quality;
					int awp_stattrack = config->Skinchanger.cz75.stattrak;

					*pWeapon->FallbackPaintKit() = awp_kit;
					*pWeapon->FallbackSeed() = awp_seed;
					*pWeapon->FallbackWear() = awp_qaultiy / 100;
					if (awp_stattrack != 0) 					*pWeapon->FallbackStatTrak() = awp_stattrack;
				}
				break;
				//heavies
				case weapon_fiveseven:
				{
					int awp_kit = config->Skinchanger.five7.id;
					int awp_seed = config->Skinchanger.five7.seed;
					int awp_qaultiy = config->Skinchanger.five7.quality;
					int awp_stattrack = config->Skinchanger.five7.stattrak;

					*pWeapon->FallbackPaintKit() = awp_kit;
					*pWeapon->FallbackSeed() = awp_seed;
					*pWeapon->FallbackWear() = awp_qaultiy / 100;
					if (awp_stattrack != 0) 					*pWeapon->FallbackStatTrak() = awp_stattrack;
				}
				break;
				case weapon_tec9: // XM1014
				{
					int awp_kit = config->Skinchanger.tec9.id;
					int awp_seed = config->Skinchanger.tec9.seed;
					int awp_qaultiy = config->Skinchanger.tec9.quality;
					int awp_stattrack = config->Skinchanger.tec9.stattrak;

					*pWeapon->FallbackPaintKit() = awp_kit;
					*pWeapon->FallbackSeed() = awp_seed;
					*pWeapon->FallbackWear() = awp_qaultiy / 100;
					if (awp_stattrack != 0) 					*pWeapon->FallbackStatTrak() = awp_stattrack;
				}
				break;
				case weapon_nova: // MAG-7
				{
					int awp_kit = config->Skinchanger.nova.id;
					int awp_seed = config->Skinchanger.nova.seed;
					int awp_qaultiy = config->Skinchanger.nova.quality;
					int awp_stattrack = config->Skinchanger.nova.stattrak;

					*pWeapon->FallbackPaintKit() = awp_kit;
					*pWeapon->FallbackSeed() = awp_seed;
					*pWeapon->FallbackWear() = awp_qaultiy / 100;
					if (awp_stattrack != 0) 					*pWeapon->FallbackStatTrak() = awp_stattrack;
				}
				break;
				case weapon_xm1014: // Sawed-Off
				{
					int awp_kit = config->Skinchanger.xm1014.id;
					int awp_seed = config->Skinchanger.xm1014.seed;
					int awp_qaultiy = config->Skinchanger.xm1014.quality;
					int awp_stattrack = config->Skinchanger.xm1014.stattrak;

					*pWeapon->FallbackPaintKit() = awp_kit;
					*pWeapon->FallbackSeed() = awp_seed;
					*pWeapon->FallbackWear() = awp_qaultiy / 100;
					if (awp_stattrack != 0) 					*pWeapon->FallbackStatTrak() = awp_stattrack;
				}
				break;
				case weapon_mag7: // Negev
				{
					int awp_kit = config->Skinchanger.mag7.id;
					int awp_seed = config->Skinchanger.mag7.seed;
					int awp_qaultiy = config->Skinchanger.mag7.quality;
					int awp_stattrack = config->Skinchanger.mag7.stattrak;

					*pWeapon->FallbackPaintKit() = awp_kit;
					*pWeapon->FallbackSeed() = awp_seed;
					*pWeapon->FallbackWear() = awp_qaultiy / 100;
					if (awp_stattrack != 0) 					*pWeapon->FallbackStatTrak() = awp_stattrack;
				}
				break;
				case weapon_sawedoff: // M249
				{
					int awp_kit = config->Skinchanger.sawed.id;
					int awp_seed = config->Skinchanger.sawed.seed;
					int awp_qaultiy = config->Skinchanger.sawed.quality;
					int awp_stattrack = config->Skinchanger.sawed.stattrak;

					*pWeapon->FallbackPaintKit() = awp_kit;
					*pWeapon->FallbackSeed() = awp_seed;
					*pWeapon->FallbackWear() = awp_qaultiy / 100;
					if (awp_stattrack != 0) 					*pWeapon->FallbackStatTrak() = awp_stattrack;
				}
				break;
				//mp's
				case weapon_negev: // MP9
				{

					int awp_kit = config->Skinchanger.negev.id;
					int awp_seed = config->Skinchanger.negev.seed;
					int awp_qaultiy = config->Skinchanger.negev.quality;
					int awp_stattrack = config->Skinchanger.negev.stattrak;

					*pWeapon->FallbackPaintKit() = awp_kit;
					*pWeapon->FallbackSeed() = awp_seed;
					*pWeapon->FallbackWear() = awp_qaultiy / 100;
					if (awp_stattrack != 0) 					*pWeapon->FallbackStatTrak() = awp_stattrack;
				}
				break;
				case weapon_m249: // MAC-10
				{
					int awp_kit = config->Skinchanger.m249.id;
					int awp_seed = config->Skinchanger.m249.seed;
					int awp_qaultiy = config->Skinchanger.m249.quality;
					int awp_stattrack = config->Skinchanger.m249.stattrak;

					*pWeapon->FallbackPaintKit() = awp_kit;
					*pWeapon->FallbackSeed() = awp_seed;
					*pWeapon->FallbackWear() = awp_qaultiy / 100;
					if (awp_stattrack != 0) 					*pWeapon->FallbackStatTrak() = awp_stattrack;
				}
				break;
				case weapon_mp9: // MP7
				{
					int awp_kit = config->Skinchanger.mp9.id;
					int awp_seed = config->Skinchanger.mp9.seed;
					int awp_qaultiy = config->Skinchanger.mp9.quality;
					int awp_stattrack = config->Skinchanger.mp9.stattrak;

					*pWeapon->FallbackPaintKit() = awp_kit;
					*pWeapon->FallbackSeed() = awp_seed;
					*pWeapon->FallbackWear() = awp_qaultiy / 100;
					if (awp_stattrack != 0) 					*pWeapon->FallbackStatTrak() = awp_stattrack;
				}
				break;
				case weapon_mac10: // UMP-45
				{
					int awp_kit = config->Skinchanger.mac10.id;
					int awp_seed = config->Skinchanger.mac10.seed;
					int awp_qaultiy = config->Skinchanger.mac10.quality;
					int awp_stattrack = config->Skinchanger.mac10.stattrak;

					*pWeapon->FallbackPaintKit() = awp_kit;
					*pWeapon->FallbackSeed() = awp_seed;
					*pWeapon->FallbackWear() = awp_qaultiy / 100;
					if (awp_stattrack != 0) 					*pWeapon->FallbackStatTrak() = awp_stattrack;
				}
				break;
				case weapon_mp7: // P90
				{
					int awp_kit = config->Skinchanger.mp7.id;
					int awp_seed = config->Skinchanger.mp7.seed;
					int awp_qaultiy = config->Skinchanger.mp7.quality;
					int awp_stattrack = config->Skinchanger.mp7.stattrak;

					*pWeapon->FallbackPaintKit() = awp_kit;
					*pWeapon->FallbackSeed() = awp_seed;
					*pWeapon->FallbackWear() = awp_qaultiy / 100;
					if (awp_stattrack != 0) 					*pWeapon->FallbackStatTrak() = awp_stattrack;
				}
				break;
				case weapon_ump45: // PP-Bizon
				{
					int awp_kit = config->Skinchanger.ump.id;
					int awp_seed = config->Skinchanger.ump.seed;
					int awp_qaultiy = config->Skinchanger.ump.quality;
					int awp_stattrack = config->Skinchanger.ump.stattrak;

					*pWeapon->FallbackPaintKit() = awp_kit;
					*pWeapon->FallbackSeed() = awp_seed;
					*pWeapon->FallbackWear() = awp_qaultiy / 100;
					if (awp_stattrack != 0) 					*pWeapon->FallbackStatTrak() = awp_stattrack;
				}
				break;
				//Rifles
				case weapon_p90: // Galil
				{
					int awp_kit = config->Skinchanger.p90.id;
					int awp_seed = config->Skinchanger.p90.seed;
					int awp_qaultiy = config->Skinchanger.p90.quality;
					int awp_stattrack = config->Skinchanger.p90.stattrak;

					*pWeapon->FallbackPaintKit() = awp_kit;
					*pWeapon->FallbackSeed() = awp_seed;
					*pWeapon->FallbackWear() = awp_qaultiy / 100;
					if (awp_stattrack != 0) 					*pWeapon->FallbackStatTrak() = awp_stattrack;
				}
				break;
				case weapon_bizon: // Famas
				{
					int awp_kit = config->Skinchanger.ppbizon.id;
					int awp_seed = config->Skinchanger.ppbizon.seed;
					int awp_qaultiy = config->Skinchanger.ppbizon.quality;
					int awp_stattrack = config->Skinchanger.ppbizon.stattrak;

					*pWeapon->FallbackPaintKit() = awp_kit;
					*pWeapon->FallbackSeed() = awp_seed;
					*pWeapon->FallbackWear() = awp_qaultiy / 100;
					if (awp_stattrack != 0) 					*pWeapon->FallbackStatTrak() = awp_stattrack;
				}
				break;
				case weapon_galilar: // M4A4
				{
					int awp_kit = config->Skinchanger.galil.id;
					int awp_seed = config->Skinchanger.galil.seed;
					int awp_qaultiy = config->Skinchanger.galil.quality;
					int awp_stattrack = config->Skinchanger.galil.stattrak;

					*pWeapon->FallbackPaintKit() = awp_kit;
					*pWeapon->FallbackSeed() = awp_seed;
					*pWeapon->FallbackWear() = awp_qaultiy / 100;
					if (awp_stattrack != 0) 					*pWeapon->FallbackStatTrak() = awp_stattrack;
				}
				break;
				case weapon_famas: // M4A1-S
				{
					int awp_kit = config->Skinchanger.famas.id;
					int awp_seed = config->Skinchanger.famas.seed;
					int awp_qaultiy = config->Skinchanger.famas.quality;
					int awp_stattrack = config->Skinchanger.famas.stattrak;

					*pWeapon->FallbackPaintKit() = awp_kit;
					*pWeapon->FallbackSeed() = awp_seed;
					*pWeapon->FallbackWear() = awp_qaultiy / 100;
					if (awp_stattrack != 0) 					*pWeapon->FallbackStatTrak() = awp_stattrack;
				}
				break;
				case weapon_m4a1_silencer: // AK47
				{

					int awp_kit = config->Skinchanger.m4a4.id;
					int awp_seed = config->Skinchanger.m4a4.seed;
					int awp_qaultiy = config->Skinchanger.m4a4.quality;
					int awp_stattrack = config->Skinchanger.m4a4.stattrak;

					*pWeapon->FallbackPaintKit() = awp_kit;
					*pWeapon->FallbackSeed() = awp_seed;
					*pWeapon->FallbackWear() = awp_qaultiy / 100;
					if (awp_stattrack != 0) 					*pWeapon->FallbackStatTrak() = awp_stattrack;
				}
				break;
				case weapon_m4a1: // AUG
				{
					int awp_kit = config->Skinchanger.m4a1.id;
					int awp_seed = config->Skinchanger.m4a1.seed;
					int awp_qaultiy = config->Skinchanger.m4a1.quality;
					int awp_stattrack = config->Skinchanger.m4a1.stattrak;

					*pWeapon->FallbackPaintKit() = awp_kit;
					*pWeapon->FallbackSeed() = awp_seed;
					*pWeapon->FallbackWear() = awp_qaultiy / 100;
					if (awp_stattrack != 0) 					*pWeapon->FallbackStatTrak() = awp_stattrack;
				}
				break;
				case weapon_ak47: // SG 553
				{
					int awp_kit = config->Skinchanger.ak47.id;
					int awp_seed = config->Skinchanger.ak47.seed;
					int awp_qaultiy = config->Skinchanger.ak47.quality;
					int awp_stattrack = config->Skinchanger.ak47.stattrak;

					*pWeapon->FallbackPaintKit() = awp_kit;
					*pWeapon->FallbackSeed() = awp_seed;
					*pWeapon->FallbackWear() = awp_qaultiy / 100;
					if (awp_stattrack != 0) 					*pWeapon->FallbackStatTrak() = awp_stattrack;
				}
				break;

				//snipers

				case weapon_aug: // SSG 08
				{
					int awp_kit = config->Skinchanger.aug.id;
					int awp_seed = config->Skinchanger.aug.seed;
					int awp_qaultiy = config->Skinchanger.aug.quality;
					int awp_stattrack = config->Skinchanger.aug.stattrak;

					*pWeapon->FallbackPaintKit() = awp_kit;
					*pWeapon->FallbackSeed() = awp_seed;
					*pWeapon->FallbackWear() = awp_qaultiy / 100;
					if (awp_stattrack != 0) 					*pWeapon->FallbackStatTrak() = awp_stattrack;
				}
				break;
				case weapon_sg556: // AWP
				{
					int awp_kit = config->Skinchanger.sg553.id;
					int awp_seed = config->Skinchanger.sg553.seed;
					int awp_qaultiy = config->Skinchanger.sg553.quality;
					int awp_stattrack = config->Skinchanger.sg553.stattrak;

					*pWeapon->FallbackPaintKit() = awp_kit;
					*pWeapon->FallbackSeed() = awp_seed;
					*pWeapon->FallbackWear() = awp_qaultiy / 100;
					if (awp_stattrack != 0)
						*pWeapon->FallbackStatTrak() = awp_stattrack;
				}
				break;
				case weapon_scar20: // SCAR-20
				{
					int awp_kit = config->Skinchanger.scar20.id;
					int awp_seed = config->Skinchanger.scar20.seed;
					int awp_qaultiy = config->Skinchanger.scar20.quality;
					int awp_stattrack = config->Skinchanger.scar20.stattrak;

					*pWeapon->FallbackPaintKit() = awp_kit;
					*pWeapon->FallbackSeed() = awp_seed;
					*pWeapon->FallbackWear() = awp_qaultiy / 100;
					if (awp_stattrack != 0)
						*pWeapon->FallbackStatTrak() = awp_stattrack;
				}
				break;
				case weapon_g3sg1: // G3SG1
				{
					int awp_kit = config->Skinchanger.g3sg1.id;
					int awp_seed = config->Skinchanger.g3sg1.seed;
					int awp_qaultiy = config->Skinchanger.g3sg1.quality;
					int awp_stattrack = config->Skinchanger.g3sg1.stattrak;

					*pWeapon->FallbackPaintKit() = awp_kit;
					*pWeapon->FallbackSeed() = awp_seed;
					*pWeapon->FallbackWear() = awp_qaultiy / 100;
					if (awp_stattrack != 0)
						*pWeapon->FallbackStatTrak() = awp_stattrack;
				}
				break;
				case weapon_mp5sd:
				{
					int awp_kit = config->Skinchanger.mp5.id;
					int awp_seed = config->Skinchanger.mp5.seed;
					int awp_qaultiy = config->Skinchanger.mp5.quality;
					int awp_stattrack = config->Skinchanger.mp5.stattrak;

					*pWeapon->FallbackPaintKit() = awp_kit;
					*pWeapon->FallbackSeed() = awp_seed;
					*pWeapon->FallbackWear() = awp_qaultiy / 100;
					if (awp_stattrack != 0)
						*pWeapon->FallbackStatTrak() = awp_stattrack;
				}
				break;
				default:
					break;
				}
				if (pEntity->get_client_class()->id == (int)cknife)
				{
					worldmodel_handle = pWeapon->m_hWeaponWorldModel();
					if (worldmodel_handle) worldmodel = (c_base_combat_weapon*)client_entity_list()->get_client_entity_from_handle(worldmodel_handle);
					if (Model == 0) // Bayonet
					{
						int iBayonet = model_info_client()->get_model_index("models/weapons/v_knife_bayonet.mdl");
						pWeapon->modelindex() = iBayonet; // m_nModelIndex
						pWeapon->m_iViewModelIndex() = iBayonet;
						if (worldmodel) worldmodel->modelindex() = iBayonet + 1;
						*pWeapon->GetItemDefenitionIndex() = 500;
						*pWeapon->GetEntityQuality() = 3;
						killIcons.clear();
						killIcons["knife_default_ct"] = "bayonet";
						killIcons["knife_t"] = "bayonet";

						int knife_kit = config->Skinchanger.knife.id;
						int knife_seed = config->Skinchanger.knife.seed;
						int knife_qaultiy = config->Skinchanger.knife.quality;
						int knife_stattrack = config->Skinchanger.knife.stattrak;

						*pWeapon->FallbackPaintKit() = knife_kit;
						*pWeapon->FallbackSeed() = knife_seed;
						*pWeapon->FallbackWear() = knife_qaultiy / 100;
						if (knife_stattrack != 0)
							*pWeapon->FallbackStatTrak() = knife_stattrack;
					}
					else if (Model == 9) // Bowie Knife
					{
						int iBowie = model_info_client()->get_model_index("models/weapons/v_knife_survival_bowie.mdl");
						pWeapon->modelindex() = iBowie; // m_nModelIndex
						pWeapon->m_iViewModelIndex() = iBowie;
						if (worldmodel) worldmodel->modelindex() = iBowie + 1;
						*pWeapon->GetItemDefenitionIndex() = 514;
						*pWeapon->GetEntityQuality() = 3;
						killIcons.clear();
						killIcons["knife_default_ct"] = "knife_survival_bowie";
						killIcons["knife_t"] = "knife_survival_bowie";
						int knife_kit = config->Skinchanger.knife.id;
						int knife_seed = config->Skinchanger.knife.seed;
						int knife_qaultiy = config->Skinchanger.knife.quality;
						int knife_stattrack = config->Skinchanger.knife.stattrak;

						*pWeapon->FallbackPaintKit() = knife_kit;
						*pWeapon->FallbackSeed() = knife_seed;
						*pWeapon->FallbackWear() = knife_qaultiy / 100;
						if (knife_stattrack != 0)
							*pWeapon->FallbackStatTrak() = knife_stattrack;
					}
					else if (Model == 6) // Butterfly Knife
					{
						int iButterfly = model_info_client()->get_model_index("models/weapons/v_knife_butterfly.mdl");
						pWeapon->modelindex() = iButterfly; // m_nModelIndex
						pWeapon->m_iViewModelIndex() = iButterfly;
						if (worldmodel) worldmodel->modelindex() = iButterfly + 1;
						*pWeapon->GetItemDefenitionIndex() = 515;
						*pWeapon->GetEntityQuality() = 3;
						killIcons.clear();
						killIcons["knife_default_ct"] = "knife_butterfly";
						killIcons["knife_t"] = "knife_butterfly";
						int knife_kit = config->Skinchanger.knife.id;
						int knife_seed = config->Skinchanger.knife.seed;
						int knife_qaultiy = config->Skinchanger.knife.quality;
						int knife_stattrack = config->Skinchanger.knife.stattrak;

						*pWeapon->FallbackPaintKit() = knife_kit;
						*pWeapon->FallbackSeed() = knife_seed;
						*pWeapon->FallbackWear() = knife_qaultiy / 100;
						if (knife_stattrack != 0)
							*pWeapon->FallbackStatTrak() = knife_stattrack;
					}
					else if (Model == 7) // Falchion Knife
					{
						int iFalchion = model_info_client()->get_model_index("models/weapons/v_knife_falchion_advanced.mdl");
						pWeapon->modelindex() = iFalchion; // m_nModelIndex
						pWeapon->m_iViewModelIndex() = iFalchion;
						if (worldmodel) worldmodel->modelindex() = iFalchion + 1;
						*pWeapon->GetItemDefenitionIndex() = 512;
						*pWeapon->GetEntityQuality() = 3;
						killIcons.clear();
						killIcons["knife_default_ct"] = "knife_falchion";
						killIcons["knife_t"] = "knife_falchion";
						int knife_kit = config->Skinchanger.knife.id;
						int knife_seed = config->Skinchanger.knife.seed;
						int knife_qaultiy = config->Skinchanger.knife.quality;
						int knife_stattrack = config->Skinchanger.knife.stattrak;

						*pWeapon->FallbackPaintKit() = knife_kit;
						*pWeapon->FallbackSeed() = knife_seed;
						*pWeapon->FallbackWear() = knife_qaultiy / 100;
						if (knife_stattrack != 0)
							*pWeapon->FallbackStatTrak() = knife_stattrack;
					}
					else if (Model == 1) // Flip Knife
					{
						int iFlip = model_info_client()->get_model_index("models/weapons/v_knife_flip.mdl");
						pWeapon->modelindex() = iFlip; // m_nModelIndex
						pWeapon->m_iViewModelIndex() = iFlip;
						if (worldmodel) worldmodel->modelindex() = iFlip + 1;
						*pWeapon->GetItemDefenitionIndex() = 505;
						*pWeapon->GetEntityQuality() = 3;
						killIcons.clear();
						killIcons["knife_default_ct"] = "knife_flip";
						killIcons["knife_t"] = "knife_flip";
						int knife_kit = config->Skinchanger.knife.id;
						int knife_seed = config->Skinchanger.knife.seed;
						int knife_qaultiy = config->Skinchanger.knife.quality;
						int knife_stattrack = config->Skinchanger.knife.stattrak;

						*pWeapon->FallbackPaintKit() = knife_kit;
						*pWeapon->FallbackSeed() = knife_seed;
						*pWeapon->FallbackWear() = knife_qaultiy / 100;
						if (knife_stattrack != 0)
							*pWeapon->FallbackStatTrak() = knife_stattrack;
					}
					else if (Model == 2) // Gut Knife
					{
						int iGut = model_info_client()->get_model_index("models/weapons/v_knife_gut.mdl");
						pWeapon->modelindex() = iGut; // m_nModelIndex
						pWeapon->m_iViewModelIndex() = iGut;
						if (worldmodel) worldmodel->modelindex() = iGut + 1;
						*pWeapon->GetItemDefenitionIndex() = 506;
						*pWeapon->GetEntityQuality() = 3;
						killIcons.clear();
						killIcons["knife_default_ct"] = "knife_gut";
						killIcons["knife_t"] = "knife_gut";
						int knife_kit = config->Skinchanger.knife.id;
						int knife_seed = config->Skinchanger.knife.seed;
						int knife_qaultiy = config->Skinchanger.knife.quality;
						int knife_stattrack = config->Skinchanger.knife.stattrak;

						*pWeapon->FallbackPaintKit() = knife_kit;
						*pWeapon->FallbackSeed() = knife_seed;
						*pWeapon->FallbackWear() = knife_qaultiy / 100;
						if (knife_stattrack != 0)
							*pWeapon->FallbackStatTrak() = knife_stattrack;
					}
					else if (Model == 5) // Huntsman Knife
					{
						int iHuntsman = model_info_client()->get_model_index("models/weapons/v_knife_tactical.mdl");
						pWeapon->modelindex() = iHuntsman; // m_nModelIndex
						pWeapon->m_iViewModelIndex() = iHuntsman;
						if (worldmodel) worldmodel->modelindex() = iHuntsman + 1;
						*pWeapon->GetItemDefenitionIndex() = 509;
						*pWeapon->GetEntityQuality() = 3;
						killIcons.clear();
						killIcons["knife_default_ct"] = "knife_tactical";
						killIcons["knife_t"] = "knife_tactical";
						int knife_kit = config->Skinchanger.knife.id;
						int knife_seed = config->Skinchanger.knife.seed;
						int knife_qaultiy = config->Skinchanger.knife.quality;
						int knife_stattrack = config->Skinchanger.knife.stattrak;

						*pWeapon->FallbackPaintKit() = knife_kit;
						*pWeapon->FallbackSeed() = knife_seed;
						*pWeapon->FallbackWear() = knife_qaultiy / 100;
						if (knife_stattrack != 0)
							*pWeapon->FallbackStatTrak() = knife_stattrack;
					}
					else if (Model == 3) // Karambit
					{
						int iKarambit = model_info_client()->get_model_index("models/weapons/v_knife_karam.mdl");
						pWeapon->modelindex() = iKarambit; // m_nModelIndex
						pWeapon->m_iViewModelIndex() = iKarambit;
						if (worldmodel) worldmodel->modelindex() = iKarambit + 1;
						*pWeapon->GetItemDefenitionIndex() = 507;
						*pWeapon->GetEntityQuality() = 3;
						killIcons.clear();
						killIcons["knife_default_ct"] = "knife_karambit";
						killIcons["knife_t"] = "knife_karambit";
						int knife_kit = config->Skinchanger.knife.id;
						int knife_seed = config->Skinchanger.knife.seed;
						int knife_qaultiy = config->Skinchanger.knife.quality;
						int knife_stattrack = config->Skinchanger.knife.stattrak;

						*pWeapon->FallbackPaintKit() = knife_kit;
						*pWeapon->FallbackSeed() = knife_seed;
						*pWeapon->FallbackWear() = knife_qaultiy / 100;
						if (knife_stattrack != 0)
							*pWeapon->FallbackStatTrak() = knife_stattrack;
					}
					else if (Model == 4) // M9 Bayonet
					{
						int iM9Bayonet = model_info_client()->get_model_index("models/weapons/v_knife_m9_bay.mdl");
						pWeapon->modelindex() = iM9Bayonet; // m_nModelIndex
						pWeapon->m_iViewModelIndex() = iM9Bayonet;
						if (worldmodel) worldmodel->modelindex() = iM9Bayonet + 1;
						*pWeapon->GetItemDefenitionIndex() = 508;
						*pWeapon->GetEntityQuality() = 3;
						killIcons.clear();
						killIcons["knife_default_ct"] = "knife_m9_bayonet";
						killIcons["knife_t"] = "knife_m9_bayonet";
						int knife_kit = config->Skinchanger.knife.id;
						int knife_seed = config->Skinchanger.knife.seed;
						int knife_qaultiy = config->Skinchanger.knife.quality;
						int knife_stattrack = config->Skinchanger.knife.stattrak;

						*pWeapon->FallbackPaintKit() = knife_kit;
						*pWeapon->FallbackSeed() = knife_seed;
						*pWeapon->FallbackWear() = knife_qaultiy / 100;
						if (knife_stattrack != 0)
							*pWeapon->FallbackStatTrak() = knife_stattrack;
					}
					else if (Model == 8)
					{
						int iDagger = model_info_client()->get_model_index("models/weapons/v_knife_push.mdl");
						pWeapon->modelindex() = iDagger; // m_nModelIndex
						pWeapon->m_iViewModelIndex() = iDagger;
						if (worldmodel) worldmodel->modelindex() = iDagger + 1;
						*pWeapon->GetItemDefenitionIndex() = 516;
						*pWeapon->GetEntityQuality() = 3;
						killIcons.clear();
						killIcons["knife_default_ct"] = "knife_push";
						killIcons["knife_t"] = "knife_push";
						int knife_kit = config->Skinchanger.knife.id;
						int knife_seed = config->Skinchanger.knife.seed;
						int knife_qaultiy = config->Skinchanger.knife.quality;
						int knife_stattrack = config->Skinchanger.knife.stattrak;

						*pWeapon->FallbackPaintKit() = knife_kit;
						*pWeapon->FallbackSeed() = knife_seed;
						*pWeapon->FallbackWear() = knife_qaultiy / 100;
						if (knife_stattrack != 0)
							*pWeapon->FallbackStatTrak() = knife_stattrack;
					}
					else if (Model == 10)
					{
						int Navaja = model_info_client()->get_model_index("models/weapons/v_knife_gypsy_jackknife.mdl");
						pWeapon->modelindex() = Navaja; // m_nModelIndex
						pWeapon->m_iViewModelIndex() = Navaja;
						if (worldmodel) worldmodel->modelindex() = Navaja + 1;
						*pWeapon->GetItemDefenitionIndex() = 520;
						*pWeapon->GetEntityQuality() = 3;
						killIcons.clear();
						killIcons["knife_default_ct"] = "knife_push";
						killIcons["knife_t"] = "knife_push";
						int knife_kit = config->Skinchanger.knife.id;
						int knife_seed = config->Skinchanger.knife.seed;
						int knife_qaultiy = config->Skinchanger.knife.quality;
						int knife_stattrack = config->Skinchanger.knife.stattrak;

						*pWeapon->FallbackPaintKit() = knife_kit;
						*pWeapon->FallbackSeed() = knife_seed;
						*pWeapon->FallbackWear() = knife_qaultiy / 100;
						if (knife_stattrack != 0)
							*pWeapon->FallbackStatTrak() = knife_stattrack;
					}
					else if (Model == 11) // Shadow Daggers
					{
						int Stiletto = model_info_client()->get_model_index("models/weapons/v_knife_stiletto.mdl");
						pWeapon->modelindex() = Stiletto; // m_nModelIndex
						pWeapon->m_iViewModelIndex() = Stiletto;
						if (worldmodel) worldmodel->modelindex() = Stiletto + 1;
						*pWeapon->GetItemDefenitionIndex() = 522;
						*pWeapon->GetEntityQuality() = 3;
						killIcons.clear();
						killIcons["knife_default_ct"] = "knife_push";
						killIcons["knife_t"] = "knife_push";
						int knife_kit = config->Skinchanger.knife.id;
						int knife_seed = config->Skinchanger.knife.seed;
						int knife_qaultiy = config->Skinchanger.knife.quality;
						int knife_stattrack = config->Skinchanger.knife.stattrak;

						*pWeapon->FallbackPaintKit() = knife_kit;
						*pWeapon->FallbackSeed() = knife_seed;
						*pWeapon->FallbackWear() = knife_qaultiy / 100;
						if (knife_stattrack != 0)
							*pWeapon->FallbackStatTrak() = knife_stattrack;
					}
					else if (Model == 12)
					{
						int Ursus = model_info_client()->get_model_index("models/weapons/v_knife_ursus.mdl");
						pWeapon->modelindex() = Ursus; // m_nModelIndex
						pWeapon->m_iViewModelIndex() = Ursus;
						if (worldmodel) worldmodel->modelindex() = Ursus + 1;
						*pWeapon->GetItemDefenitionIndex() = 519;
						*pWeapon->GetEntityQuality() = 3;
						killIcons.clear();
						killIcons["knife_default_ct"] = "knife_push";
						killIcons["knife_t"] = "knife_push";
						int knife_kit = config->Skinchanger.knife.id;
						int knife_seed = config->Skinchanger.knife.seed;
						int knife_qaultiy = config->Skinchanger.knife.quality;
						int knife_stattrack = config->Skinchanger.knife.stattrak;

						*pWeapon->FallbackPaintKit() = knife_kit;
						*pWeapon->FallbackSeed() = knife_seed;
						*pWeapon->FallbackWear() = knife_qaultiy / 100;
						if (knife_stattrack != 0)
							*pWeapon->FallbackStatTrak() = knife_stattrack;
					}
					else if (Model == 13)
					{
						int Talon = model_info_client()->get_model_index("models/weapons/v_knife_widowmaker.mdl");
						pWeapon->modelindex() = Talon; // m_nModelIndex
						pWeapon->m_iViewModelIndex() = Talon;
						if (worldmodel) worldmodel->modelindex() = Talon + 1;
						*pWeapon->GetItemDefenitionIndex() = 523;
						*pWeapon->GetEntityQuality() = 3;
						killIcons.clear();
						killIcons["knife_default_ct"] = "knife_push";
						killIcons["knife_t"] = "knife_push";
						int knife_kit = config->Skinchanger.knife.id;
						int knife_seed = config->Skinchanger.knife.seed;
						int knife_qaultiy = config->Skinchanger.knife.quality;
						int knife_stattrack = config->Skinchanger.knife.stattrak;

						*pWeapon->FallbackPaintKit() = knife_kit;
						*pWeapon->FallbackSeed() = knife_seed;
						*pWeapon->FallbackWear() = knife_qaultiy / 100;
						if (knife_stattrack != 0)
							*pWeapon->FallbackStatTrak() = knife_stattrack;
					}
					else if (Model == 14)
					{
						int v_knife_css = model_info_client()->get_model_index("models/weapons/v_knife_css.mdl");
						pWeapon->modelindex() = v_knife_css; // m_nModelIndex
						pWeapon->m_iViewModelIndex() = v_knife_css;
						if (worldmodel) worldmodel->modelindex() = v_knife_css + 1;
						*pWeapon->GetItemDefenitionIndex() = 503;
						*pWeapon->GetEntityQuality() = 3;
						killIcons.clear();
						killIcons["knife_default_ct"] = "knife_push";
						killIcons["knife_t"] = "knife_push";
						int knife_kit = config->Skinchanger.knife.id;
						int knife_seed = config->Skinchanger.knife.seed;
						int knife_qaultiy = config->Skinchanger.knife.quality;
						int knife_stattrack = config->Skinchanger.knife.stattrak;

						*pWeapon->FallbackPaintKit() = knife_kit;
						*pWeapon->FallbackSeed() = knife_seed;
						*pWeapon->FallbackWear() = knife_qaultiy / 100;
						if (knife_stattrack != 0)
							*pWeapon->FallbackStatTrak() = knife_stattrack;
					}
				}
			}
			*pWeapon->GetAccountID() = pLocal->get_info().xuid_low;
			*pWeapon->OwnerXuidLow() = 0;
			*pWeapon->OwnerXuidHigh() = 0;
			*pWeapon->fixItemIDHigh() = 1;
		}
	}
}


void GloveChanger()
{
	if (config->Skinchanger.Enabled)
	{
		player_info localPlayerInfo;

		auto pLocal = c_cs_player::get_local_player();
		if (pLocal)
		{
			if (engine_client()->get_player_info(engine_client()->get_local_player(), &localPlayerInfo))
			{
				DWORD* hMyWearables = (DWORD*)((size_t)pLocal->m_hMyWearables());

				if (hMyWearables)
				{
					if (!client_entity_list()->get_client_entity_from_handle((DWORD)hMyWearables[0]))
					{

						for (auto pClass = base_client()->get_all_classes(); pClass; pClass = pClass->next)
						{
							if (pClass->id != ceconwearable)
								continue;

							int iEntry = (client_entity_list()->get_highest_entity_index() + 1),
								iSerial = RandomInt(0x0, 0xFFF);

							pClass->create_fn(iEntry, iSerial);
							hMyWearables[0] = iEntry | (iSerial << 16);
							break;
						}

						/*static c_client_class* pClass;

						if (!pClass)
							pClass = base_client()->get_all_classes();
						while (pClass)
						{
							if (pClass->id == ceconwearable)
								break;
							pClass = pClass->next;
						}

						int iEntry = client_entity_list()->get_highest_entity_index() + 1;
						int iSerial = RandomInt(0x0, 0xFFF);

						pClass->create_fn(iEntry, iSerial);
						hMyWearables[0] = iEntry | (iSerial << 16);*/

						auto pEnt = (c_base_entity*)client_entity_list()->get_client_entity_from_handle((DWORD)hMyWearables[0]);

						if (pEnt)
						{
							int modelindex = 0;

							if (config->Skinchanger.gloves_switch == 1)
								modelindex = model_info_client()->get_model_index(("models/weapons/v_models/arms/glove_bloodhound/v_glove_bloodhound.mdl"));
							else if (config->Skinchanger.gloves_switch == 2)
								modelindex = model_info_client()->get_model_index(("models/weapons/v_models/arms/glove_sporty/v_glove_sporty.mdl"));
							else if (config->Skinchanger.gloves_switch == 3)
								modelindex = model_info_client()->get_model_index(("models/weapons/v_models/arms/glove_slick/v_glove_slick.mdl"));
							else if (config->Skinchanger.gloves_switch == 4)
								modelindex = model_info_client()->get_model_index(("models/weapons/v_models/arms/glove_handwrap_leathery/v_glove_handwrap_leathery.mdl"));
							else if (config->Skinchanger.gloves_switch == 5)
								modelindex = model_info_client()->get_model_index(("models/weapons/v_models/arms/glove_motorcycle/v_glove_motorcycle.mdl"));
							else if (config->Skinchanger.gloves_switch == 6)
								modelindex = model_info_client()->get_model_index(("models/weapons/v_models/arms/glove_specialist/v_glove_specialist.mdl"));

							int ItemDefinitionIndex;
							if (config->Skinchanger.gloves_switch == 1)
							{
								ItemDefinitionIndex = 5027;
							}
							else if (config->Skinchanger.gloves_switch == 2)
							{
								ItemDefinitionIndex = 5030;
							}
							else if (config->Skinchanger.gloves_switch == 3)
							{
								ItemDefinitionIndex = 5031;
							}
							else if (config->Skinchanger.gloves_switch == 4)
							{
								ItemDefinitionIndex = 5032;
							}
							else if (config->Skinchanger.gloves_switch == 5)
							{
								ItemDefinitionIndex = 5033;
							}
							else if (config->Skinchanger.gloves_switch == 6)
							{
								ItemDefinitionIndex = 5034;
							}
							else
								ItemDefinitionIndex = 0;

							const int paintkit = config->Skinchanger.gloves.id;

							*(int*)((DWORD)pEnt + 0x2F9A) = ItemDefinitionIndex;
							*(int*)((DWORD)pEnt + 0x2FB0) = -1;
							*(int*)((DWORD)pEnt + 0x2F9C) = 4;
							*(int*)((DWORD)pEnt + 0x2FB8) = localPlayerInfo.xuid_low;
							*(float*)((DWORD)pEnt + 0x31B0) = config->Skinchanger.gloves.quality;
							*(int*)((DWORD)pEnt + 0x31AC) = 0;
							*(int*)((DWORD)pEnt + 0x31B4) = -1;
							*(int*)((DWORD)pEnt + 0x31A8) = paintkit;

							pEnt->set_model_index(modelindex);
							pEnt->pre_data_update(0);
						}
					}
				}

			}
		}
	}
}
