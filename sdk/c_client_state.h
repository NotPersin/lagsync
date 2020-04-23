#pragma once

#include <cstdint>
#include "c_net_channel.h"
#include "c_vector3d.h"
#include "../utils/c_memory.h"
#include "../sdk/c_engine_client.h"

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

class bf_read {
public:
	const char* m_pDebugName;
	bool m_bOverflow;
	int m_nDataBits;
	unsigned int m_nDataBytes;
	unsigned int m_nInBufWord;
	int m_nBitsAvail;
	const unsigned int* m_pDataIn;
	const unsigned int* m_pBufferEnd;
	const unsigned int* m_pData;

	bf_read() = default;

	bf_read(const void* pData, int nBytes, int nBits = -1) {
		StartReading(pData, nBytes, 0, nBits);
	}

	void StartReading(const void* pData, int nBytes, int iStartBit, int nBits) {
		// Make sure it's dword aligned and padded.
		m_pData = (uint32_t*)pData;
		m_pDataIn = m_pData;
		m_nDataBytes = nBytes;

		if (nBits == -1) {
			m_nDataBits = nBytes << 3;
		}
		else {
			m_nDataBits = nBits;
		}
		m_bOverflow = false;
		m_pBufferEnd = reinterpret_cast<uint32_t const*>(reinterpret_cast<uint8_t const*>(m_pData) + nBytes);
		if (m_pData)
			Seek(iStartBit);
	}

	bool Seek(int nPosition) {
		bool bSucc = true;
		if (nPosition < 0 || nPosition > m_nDataBits) {
			m_bOverflow = true;
			bSucc = false;
			nPosition = m_nDataBits;
		}
		int nHead = m_nDataBytes & 3; // non-multiple-of-4 bytes at head of buffer. We put the "round off"
									  // at the head to make reading and detecting the end efficient.

		int nByteOfs = nPosition / 8;
		if ((m_nDataBytes < 4) || (nHead && (nByteOfs < nHead))) {
			// partial first dword
			uint8_t const* pPartial = (uint8_t const*)m_pData;
			if (m_pData) {
				m_nInBufWord = *(pPartial++);
				if (nHead > 1)
					m_nInBufWord |= (*pPartial++) << 8;
				if (nHead > 2)
					m_nInBufWord |= (*pPartial++) << 16;
			}
			m_pDataIn = (uint32_t const*)pPartial;
			m_nInBufWord >>= (nPosition & 31);
			m_nBitsAvail = (nHead << 3) - (nPosition & 31);
		}
		else {
			int nAdjPosition = nPosition - (nHead << 3);
			m_pDataIn = reinterpret_cast<uint32_t const*>(
				reinterpret_cast<uint8_t const*>(m_pData) + ((nAdjPosition / 32) << 2) + nHead);
			if (m_pData) {
				m_nBitsAvail = 32;
				GrabNextDWord();
			}
			else {
				m_nInBufWord = 0;
				m_nBitsAvail = 1;
			}
			m_nInBufWord >>= (nAdjPosition & 31);
			m_nBitsAvail = min(m_nBitsAvail, 32 - (nAdjPosition & 31)); // in case grabnextdword overflowed
		}
		return bSucc;
	}

	FORCEINLINE void GrabNextDWord(bool bOverFlowImmediately = false) {
		if (m_pDataIn == m_pBufferEnd) {
			m_nBitsAvail = 1; // so that next read will run out of words
			m_nInBufWord = 0;
			m_pDataIn++; // so seek count increments like old
			if (bOverFlowImmediately)
				m_bOverflow = true;
		}
		else if (m_pDataIn > m_pBufferEnd) {
			m_bOverflow = true;
			m_nInBufWord = 0;
		}
		else {
			m_nInBufWord = DWORD(*(m_pDataIn++));
		}
	}
};

class bf_write {
public:
	unsigned char* m_pData;
	int m_nDataBytes;
	int m_nDataBits;
	int m_iCurBit;
	bool m_bOverflow;
	bool m_bAssertOnOverflow;
	const char* m_pDebugName;

	void StartWriting(void* pData, int nBytes, int iStartBit = 0, int nBits = -1) {
		// Make sure it's dword aligned and padded.
		// The writing code will overrun the end of the buffer if it isn't dword aligned, so truncate to force alignment
		nBytes &= ~3;

		m_pData = (unsigned char*)pData;
		m_nDataBytes = nBytes;

		if (nBits == -1) {
			m_nDataBits = nBytes << 3;
		}
		else {
			m_nDataBits = nBits;
		}

		m_iCurBit = iStartBit;
		m_bOverflow = false;
	}

	bf_write() {
		m_pData = NULL;
		m_nDataBytes = 0;
		m_nDataBits = -1; // set to -1 so we generate overflow on any operation
		m_iCurBit = 0;
		m_bOverflow = false;
		m_bAssertOnOverflow = true;
		m_pDebugName = NULL;
	}

	// nMaxBits can be used as the number of bits in the buffer.
	// It must be <= nBytes*8. If you leave it at -1, then it's set to nBytes * 8.
	bf_write(void* pData, int nBytes, int nBits = -1) {
		m_bAssertOnOverflow = true;
		m_pDebugName = NULL;
		StartWriting(pData, nBytes, 0, nBits);
	}

	bf_write(const char* pDebugName, void* pData, int nBytes, int nBits = -1) {
		m_bAssertOnOverflow = true;
		m_pDebugName = pDebugName;
		StartWriting(pData, nBytes, 0, nBits);
	}
};

class c_net_graph_system {
public:
	static c_net_graph_system* c_net_graph_system::get()
	{
		static const auto netgraph = **reinterpret_cast<c_net_graph_system***>(sig("client_panorama.dll", "89 1D ? ? ? ? 8B C3") + 0x2);
		return netgraph;
	}
};

class CLC_Move {
private:
	char __PAD0[0x8]; // 0x0 two vtables
public:
	int m_nBackupCommands; // 0x8
	int m_nNewCommands;    // 0xC
	std::string* m_data;   // 0x10 std::string
	bf_read m_DataIn;      // 0x14
	bf_write m_DataOut;    // 0x38
};                       // size: 0x50

template < typename T >
class CNetMessagePB : public c_net_msg, public T {};
using CCLCMsg_Move_t = CNetMessagePB< CLC_Move >;

class c_client_state {
	class c_clock_drift_mgr {
	public:
		float clock_offsets[17];
		uint32_t cur_clock_offset;
		uint32_t server_tick;
		uint32_t client_tick;
	};

public:
	static c_client_state* get()
	{
		static const auto state = **reinterpret_cast<c_client_state***>(sig("engine.dll", "8B 3D ? ? ? ? 8A F9") + 2);
		return state;
	}

	void request_full_update()
	{
		delta_tick = -1;
	}

	char pad_0000[0x9C];
	c_net_channel* net_channel;
	uint32_t m_nChallengeNr;
	char pad_00A4[0x64];
	uint32_t m_nSignonState;
	char pad_010C[0x8];
	float m_flNextCmdTime;
	uint32_t m_nServerCount;
	uint32_t current_sequence;
	char pad_0120[4];
	c_clock_drift_mgr clock_drift_mgr;
	int32_t delta_tick;
	bool m_bPaused;
	char pad_0179[7];
	uint32_t m_nViewEntity;
	uint32_t m_nPlayerSlot;
	char m_szLevelName[260];
	char m_szLevelNameShort[80];
	char m_szGroupName[80];
	char pad_032C[92];
	uint32_t m_nMaxClients;
	char pad_0314[18820];
	float m_flLastServerTickTime;
	bool in_simulation;
	char pad_4C9D[3];
	uint32_t oldtickcount;
	float m_tickRemainder;
	float m_frameTime;
	uint32_t last_command;
	uint32_t choked_commands;
	uint32_t last_command_ack;
	uint32_t command_ack;
	uint32_t m_nSoundSequence;
	char pad_4CC0[80];
	c_vector3d viewangles;
	char pad_4D14[0xD0];
	c_event_info* events;
};

#define client_state c_client_state::get()
#define net_graph c_net_graph_system::get()

#define net_channel client_state->net_channel