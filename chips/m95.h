/*	Benjamin DELPY `gentilkiwi`
	https://blog.gentilkiwi.com
	benjamin@gentilkiwi.com
	Licence : https://creativecommons.org/licenses/by/4.0/
*/
/* * * * * * * * * * * * * * * * * * * * * * * * *
 * STMicroelectronics
 * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once
#include "../kiflashrom.h"

#define M95_JEDEC_BANK			1
#define M95_JEDEC_MANUFACTURER	32

#define M95_OPCODE_READ_DATA	0x03 // GENERIC_OPCODE_READ_DATA
#define M95_OPCODE_PAGE_ERASE	0xdb

#define M95_OPCODE_READ_IDENTIFICATION	0x83


#define UID_M95P32		MAKE_CHIP_UID(M95_JEDEC_BANK, M95_JEDEC_MANUFACTURER, 0x00, 0x16)

FT_STATUS M95_PageErase(FT_HANDLE handle, DWORD address);
FT_STATUS M95_ReadIdentification(FT_HANDLE handle, DWORD Address, BYTE* Identification, DWORD cbIdentification);