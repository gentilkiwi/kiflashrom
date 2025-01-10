/*	Benjamin DELPY `gentilkiwi`
	https://blog.gentilkiwi.com
	benjamin@gentilkiwi.com
	Licence : https://creativecommons.org/licenses/by/4.0/
*/
/* * * * * * * * * * * * * * * * * * * * * * * * *
 * Ramtron
 * -----------------------------------------------
 * Enhanced Memories
 * Ramtron
 * Mushkin Enhanced Memory
 * Cypress Semiconductor
 * Infineon
 * ...
 * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once
#include "../kiflashrom.h"

#define FM25V_JEDEC_BANK			7
#define FM25V_JEDEC_MANUFACTURER	66

#define FM25_OPCODE_WREN	0x06 // Set write enable latch
#define FM25_OPCODE_WRDI	0x04 // Reset write enable latch
#define FM25_OPCODE_RDSR	0x05 // Read Status Register
#define FM25_OPCODE_WRSR	0x01 // Write Status Register
#define FM25_OPCODE_READ	0x03 // Read memory data
#define FM25_OPCODE_WRITE	0x02 // Write memory data

#define FM25_OPCODE_FSTRD	0x0b // Fast read memory data
#define FM25_OPCODE_SLEEP	0xb9 // Enter sleep mode
#define FM25_OPCODE_RDID	0x9f // Read device ID

/* FM25V
 * -----------------------------------------------
 * FM25V01(A) - C2 21 00(08) - 128-Kbit ( 16K × 8)
 * FM25V02(A) - C2 22 00(08) - 256-Kbit ( 32K × 8)
 * FM25V05    - C2 23 00     - 512-Kbit ( 64K × 8)
 * FM25V10    - C2 24 00     -   1-Mbit (128K × 8)
 * FM25V20(A) - C2 25 00(08) -   2-Mbit (256K × 8)
 */
#define UID_FM25V01		MAKE_CHIP_UID(FM25V_JEDEC_BANK, FM25V_JEDEC_MANUFACTURER, 0x21, 0x00)
#define UID_FM25V01A	MAKE_CHIP_UID(FM25V_JEDEC_BANK, FM25V_JEDEC_MANUFACTURER, 0x21, 0x08)
#define UID_FM25V02		MAKE_CHIP_UID(FM25V_JEDEC_BANK, FM25V_JEDEC_MANUFACTURER, 0x22, 0x00)
#define UID_FM25V02A	MAKE_CHIP_UID(FM25V_JEDEC_BANK, FM25V_JEDEC_MANUFACTURER, 0x22, 0x08)
#define UID_FM25V05		MAKE_CHIP_UID(FM25V_JEDEC_BANK, FM25V_JEDEC_MANUFACTURER, 0x23, 0x00)
#define UID_FM25V10		MAKE_CHIP_UID(FM25V_JEDEC_BANK, FM25V_JEDEC_MANUFACTURER, 0x24, 0x00)
#define UID_FM25V20		MAKE_CHIP_UID(FM25V_JEDEC_BANK, FM25V_JEDEC_MANUFACTURER, 0x25, 0x00)
#define UID_FM25V20A	MAKE_CHIP_UID(FM25V_JEDEC_BANK, FM25V_JEDEC_MANUFACTURER, 0x25, 0x08)

FT_STATUS FM25_Write_Enable(FT_HANDLE handle);
FT_STATUS FM25_Write_Disable(FT_HANDLE handle);
FT_STATUS FM25_Read_Status(FT_HANDLE handle, BYTE* pStatus);
FT_STATUS FM25_Write_Status(FT_HANDLE handle, BYTE Status);
FT_STATUS FM25_Read(FT_HANDLE handle, DWORD address, BYTE* buffer, DWORD size);
