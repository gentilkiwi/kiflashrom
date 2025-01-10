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
#include "chips/at45.h"

#define SPI_CONFIG_DEFAULT_FREQUENCY	5000000

#define JEDEC_ReadId	0x9f
#define JEDEC_Continuation_Code 0x7f
#define JEDEC_ReadId_MAX_Banks	(2* ARRAYSIZE(JEDEC_BANKS_MANUFACTURERS)) // we want to avoid a endless 0x7f loop

#define COMPARED_READ_ITERATIONS	5

typedef FT_STATUS(*PKSIMPLE_READ) (FT_HANDLE handle, DWORD address, BYTE* buffer, DWORD size);

FT_STATUS Read_ID(FT_HANDLE handle, DWORD* pUID);
void GenericComparedRead(FT_HANDLE handle, PKSIMPLE_READ SimpleRead, DWORD Size, PCWSTR filename);
void FM25V0x(FT_HANDLE handle, PKSIMPLE_READ* pSimpleRead, DWORD* pSize);
void AT45DBx(FT_HANDLE handle, PKSIMPLE_READ* pSimpleRead, DWORD* pSize);
