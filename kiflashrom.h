/*	Benjamin DELPY `gentilkiwi`
	https://blog.gentilkiwi.com
	benjamin@gentilkiwi.com
	Licence : https://creativecommons.org/licenses/by/4.0/
*/
#pragma once
#include <stdio.h>
#include "ftd2xx.h"
#include "libmpsse_spi.h"
#include "jedec_manufacturers.h"
#include "utils.h"

#include "chips/generic.h"
#include "chips/fm25.h"
#include "chips/at45db.h"
#include "chips/w25.h"
#include "chips/m95.h"
#include "chips/zd25ld.h"

#define SPI_CONFIG_DEFAULT_FREQUENCY	5000000
#define COMPARED_READ_ITERATIONS		5

void GenericComparedRead(FT_HANDLE handle, BYTE OptAddrSize, DWORD Size, PCWSTR filename);
void FM25V0x(FT_HANDLE handle, BYTE* pOptAddrSize, DWORD* pSize);
void AT45DBx(FT_HANDLE handle, BYTE* pOptAddrSize, DWORD* pSize);
void W25X(FT_HANDLE handle, DWORD UID, BYTE* pOptAddrSize, DWORD* pSize);
void M95(FT_HANDLE handle, BYTE* pOptAddrSize, DWORD* pSize);
void ZD25LD(FT_HANDLE handle, BYTE* pOptAddrSize, DWORD* pSize);