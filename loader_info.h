#pragma once

#include <Windows.h>

#pragma pack(8)
struct Loader_info
{
	DWORD   crc32;
	DWORD64 module_base;
	DWORD   module_size;
	DWORD   module_id;
	DWORD   loader_pid;
	char    _reserved0[24];
	DWORD   userid;
	CHAR    username[24];
	CHAR    loader_directory[MAX_PATH];
	CHAR    _config_directory[56];
	CHAR    _config_name[32];
	char    _reserved1[20];
	CHAR    event_name[20];
	DWORD64 custom_data;
	char    _reserved2[16];
	CHAR    vacdef_pipe_name[32];
	char    _reserved3[4];
};