/*	Benjamin DELPY `gentilkiwi`
	https://blog.gentilkiwi.com
	benjamin@gentilkiwi.com
	Licence : https://creativecommons.org/licenses/by/4.0/
*/
#pragma once
#include <stdio.h>
#include "libkftdi_mpsse.h"
#include "jedec_manufacturers.h"
#include "utils.h"

#include "chips/generic.h"
#include "chips/fm25.h"
#include "chips/at45db.h"
#include "chips/w25.h"
#include "chips/m95.h"
#include "chips/zd25ld.h"
#include "chips/nrf24l.h"

#define SPI_CONFIG_DEFAULT_FREQUENCY	5000000
#define COMPARED_READ_ITERATIONS		1//5

void GenericComparedRead(PKFTDI_MPSSE_SPI_HANDLE pKFTDI, BYTE OptAddrSize, DWORD Size, PCWSTR filename);
void FM25V0x(PKFTDI_MPSSE_SPI_HANDLE pKFTDI, BYTE* pOptAddrSize, DWORD* pSize);
void AT45DBx(PKFTDI_MPSSE_SPI_HANDLE pKFTDI, BYTE* pOptAddrSize, DWORD* pSize);
void W25X(PKFTDI_MPSSE_SPI_HANDLE pKFTDI, DWORD UID, BYTE* pOptAddrSize, DWORD* pSize);
void M95(PKFTDI_MPSSE_SPI_HANDLE pKFTDI, BYTE* pOptAddrSize, DWORD* pSize);
void ZD25LD(PKFTDI_MPSSE_SPI_HANDLE pKFTDI, BYTE* pOptAddrSize, DWORD* pSize);