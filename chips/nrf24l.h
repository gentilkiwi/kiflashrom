/*	Benjamin DELPY `gentilkiwi`
	https://blog.gentilkiwi.com
	benjamin@gentilkiwi.com
	Licence : https://creativecommons.org/licenses/by/4.0/
*/
/* * * * * * * * * * * * * * * * * * * * * * * * *
 * Specifics for Nordic Semiconductor - NRF24LU1(+)
 * -----------------------------------------------
 * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once
#include "generic.h"

#define NRF24LU1P_PIN_PROG				4

#define NRF24LU1P_PAGE_SIZE				0x200

#define NRF24LU1P_PAGES_COUNT_F16		0x20
#define NRF24LU1P_PAGES_COUNT_F32		0x40


#define NRF24LU1P_FLASH_SIZE_F16		(NRF24LU1P_PAGES_COUNT_F16 * NRF24LU1P_PAGE_SIZE)
#define NRF24LU1P_FLASH_SIZE_F32		(NRF24LU1P_PAGES_COUNT_F32 * NRF24LU1P_PAGE_SIZE)

#define NRF24LU1P_FLASH_PROGRAM_SIZE	0x100

#define NRF24L_ERASE_PAGE	0x52
#define NRF24L_ERASE_ALL	0x62

#define NRF24L_RDFPCR		0x89
#define NRF24L_RDISIP		0x84
#define NRF24L_RDISMB		0x85
#define NRF24L_ENDEBUG		0x86


#define NRF24_STATUS_DBG				(1 << 7)
#define NRF24_STATUS_STP				(1 << 6)
#define NRF24_STATUS_WEN				(1 << 5)
#define NRF24_STATUS_RDYN				(1 << 4)
#define NRF24_STATUS_INFEN				(1 << 3)
#define NRF24_STATUS_RDISMB				(1 << 2)
#define NRF24_STATUS_RDISIP				(1 << 1)

#define NRF24L_Read_FPCR(handle, pStatus)		Generic_Read_Byte(handle, NRF24L_RDFPCR, pStatus)

FT_STATUS NRF24L_Status_Wait_Ready(PKFTDI_MPSSE_SPI_HANDLE pKFTDI, BYTE* pStatus);
FT_STATUS NRF24L_Write_Enable_Confirmed(PKFTDI_MPSSE_SPI_HANDLE pKFTDI);

FT_STATUS NRF24LU1P_Unbrick(PKFTDI_MPSSE_SPI_HANDLE pKFTDI, BOOL bInfoPage, BOOL bIs16);

#define NRF24LU1P_BOOTLOADER_START_F32			0x7800
#define NRF24LU1P_BOOTLOADER_START_F16			0x3800
#define NRF24LU1P_BOOTLOADER_2ND_PART_OFFSET	0x01a0

void NRF24LU1P_Specifics(PKFTDI_MPSSE_SPI_HANDLE pKFTDI, WORD Address, WORD Size, LPCWSTR Filename, BOOL bIsWrite, int argc, wchar_t* argv[]);