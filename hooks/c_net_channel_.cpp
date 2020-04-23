#include "c_net_channel_.h"
#include "../sdk/c_input.h"
#include "../sdk/c_base_client.h"
#include "../hooks/c_client.h"

void c_net_channel_::hook()
{
	static bool init = false;

	if (!init)
	{
		hk = std::make_unique<c_hook<c_net_channel>>(net_channel);
		_send_netmsg = hk->apply<send_netmsg_t>(40, send_netmsg);
		init = true;
	}
}

void c_net_channel_::apply_to_net_chan(c_net_channel* channel)
{
	hk->patch_pointer(reinterpret_cast<uintptr_t*>(channel));
}

void WriteUsercmd(bf_write* buf, c_user_cmd* incmd, c_user_cmd* outcmd) {
	using WriteUsercmd_t = void(__fastcall*)(void*, c_user_cmd*, c_user_cmd*);
	static WriteUsercmd_t WriteUsercmdF = (WriteUsercmd_t)sig(("client_panorama.dll"), 
		("55 8B EC 83 E4 F8 51 53 56 8B D9 8B 0D"));

	__asm
	{
		mov     ecx, buf
		mov     edx, incmd
		push    outcmd
		call    WriteUsercmdF
		add     esp, 4
	}
}

exploit exploits;

bool write_usercmd_delta_to_buffer(int nSlot, void* buf,
	int from, int to, bool isNewCmd)
{
	using write_usercmd_delta_to_buffer_t = bool(__thiscall*)(void*, int, void*, int, int, bool);

	return ((write_usercmd_delta_to_buffer_t)((*(void***)base_client())[24]))(base_client(), nSlot, buf, from, to, isNewCmd);
}

void BypassChokeLimit(CCLCMsg_Move_t* CL_Move, c_net_channel* pNetChan) {
	// not shifting or dont need do extra fakelag
	if (exploits.tickbaseshift == 0 && (CL_Move->m_nNewCommands != 15 || client_state->choked_commands <= 14))
		return;

	using assign_lol = std::string & (__thiscall*)(void*, uint8_t*, size_t);
	auto assign_std_autistic_string = (assign_lol)sig("client_panorama.dll", 
		"55 8B EC 53 8B 5D 08 56 8B F1 85 DB 74 57 8B 4E 14 83 F9 10 72 04 8B 06 EB 02"); 
	// "55 8B EC 53 8B 5D 08 56 8B F1 85 DB 74 57 8B 4E 14 83 F9 10 72 04 8B 06 EB 02"

	// rebuild CL_SendMove
	uint8_t data[4000];
	bf_write buf;
	buf.m_nDataBytes = 4000;
	buf.m_nDataBits = 32000;
	buf.m_pData = data;
	buf.m_iCurBit = false;
	buf.m_bOverflow = false;
	buf.m_bAssertOnOverflow = false;
	buf.m_pDebugName = false;
	int numCmd = client_state->choked_commands + 1;
	int nextCmdNr = client_state->last_command + numCmd;
	if (numCmd > 62)
		numCmd = 62;

	bool bOk = true;

	auto to = nextCmdNr - numCmd + 1;
	auto from = -1;
	if (to <= nextCmdNr) {
		int newcmdnr = to >= (nextCmdNr - numCmd + 1);
		do {
			bOk = bOk && write_usercmd_delta_to_buffer(0, &buf, from, to, to >= newcmdnr);
			from = to++;
		} while (to <= nextCmdNr);
	}

	if (bOk) {
		if (exploits.tickbaseshift > 0) {
			c_user_cmd from_cmd, to_cmd;
			from_cmd = *input->GetUserCmd(0, nextCmdNr);
			to_cmd = from_cmd;
			to_cmd.tick_count = INT_MAX;

			do {
				if (numCmd >= 62) {
					exploits.tickbaseshift = 0;
					break;
				}

				to_cmd.command_number++;
				WriteUsercmd(&buf, &to_cmd, &from_cmd);

				exploits.tickbaseshift--;
				numCmd++;
			} while (exploits.tickbaseshift > 0);
		}
		else {
			exploits.tickbaseshift = 0;
		}

		// bypass choke limit
		CL_Move->m_nNewCommands = numCmd;
		CL_Move->m_nBackupCommands = 0;

		int curbit = (buf.m_iCurBit + 7) >> 3;
		assign_std_autistic_string(CL_Move->m_data, buf.m_pData, curbit);
	}
}

bool c_net_channel_::send_netmsg(c_net_channel* channel, uint32_t, c_net_msg& msg, const bool reliable, const bool voice)
{
	if (msg.get_group() == 11) {
		uintptr_t uiMsg = (uintptr_t)(&msg);

		BypassChokeLimit((CCLCMsg_Move_t*)&msg, channel);

		return _send_netmsg(channel, msg, reliable, voice);
	}

	return _send_netmsg(channel, msg, reliable, msg.get_group() == 9 ? true : voice);
}