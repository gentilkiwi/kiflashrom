/*	Benjamin DELPY `gentilkiwi`
	https://blog.gentilkiwi.com
	benjamin@gentilkiwi.com
	Licence : https://creativecommons.org/licenses/by/4.0/
*/
#pragma once
#include <windows.h>
#include "../jedec_manufacturers.h"
#include "../kiflashrom.h"

FT_STATUS Generic_SPI(FT_HANDLE handle, BYTE Command, BYTE Option, DWORD Address, BYTE* Data, DWORD cbData);

// See JEDEC JEP106  - ID   - https://www.jedec.org/standards-documents/docs/jep-106ab
// See JEDEC JESD216 - SFDP - https://www.jedec.org/standards-documents/docs/jesd216b

#define MAKE_CHIP_UID(Bank, Manufacturer, DeviceID0, DeviceID1)	((DWORD) ( \
	(((BYTE) (Bank)) << 24) | \
	(((BYTE) (Manufacturer)) << 16) | \
	(((BYTE) (DeviceID0)) << 8) | \
	(((BYTE) (DeviceID1))) \
))

#define CHIP_UID_TO_BANK(UID)			((BYTE) (((DWORD) (UID)) >> 24))
#define CHIP_UID_TO_MANUFACTURER(UID)	((BYTE) (((DWORD) (UID)) >> 16))
#define CHIP_UID_TO_DEVICEID(UID)		((WORD) ((DWORD) (UID)))

const char* CHIP_UID_TO_MANUFACTURER_NAME(const DWORD UID);

#define JEDEC_ReadId			0x9f
#define JEDEC_Continuation_Code 0x7f
#define JEDEC_ReadId_MAX_Banks	(2* ARRAYSIZE(JEDEC_BANKS_MANUFACTURERS)) // we want to avoid an endless 0x7f loop

#define JEDEC_ReadSFDP			0x5a
#define JEDEC_SFDP_Size			0x200

FT_STATUS Generic_Read_ID(FT_HANDLE handle, DWORD* pUID);
FT_STATUS Generic_Read_SFDP(FT_HANDLE handle, BYTE SFDP[JEDEC_SFDP_Size]);

#define GENERIC_OPTION_ADDRNO			0b00000000
#define GENERIC_OPTION_ADDR1B			0b00000001
#define GENERIC_OPTION_ADDR2B			0b00000010
#define GENERIC_OPTION_ADDR3B			0b00000011
#define GENERIC_OPTION_ADDR4B			0b00000100
#define GENERIC_OPTION_ADDR_MASK		0b00000111
#define GENERIC_OPTION_ADDR_MAX			4

#define GENERIC_OPTION_DUMMY_MASK		0b00111000
#define GENERIC_OPTION_DUMMY_SHIFT		3
#define GENERIC_OPTION_DUMMY(option)	(((BYTE) option & GENERIC_OPTION_DUMMY_MASK) >> GENERIC_OPTION_DUMMY_SHIFT)
#define GENERIC_OPTION_DUMMY_FROM(x)	(((BYTE) x << GENERIC_OPTION_DUMMY_SHIFT) & GENERIC_OPTION_DUMMY_MASK)
#define GENERIC_OPTION_DUMMY_MAX		(((BYTE) GENERIC_OPTION_DUMMY_MASK) >> GENERIC_OPTION_DUMMY_SHIFT)

#define GENERIC_OPTION_OP_WRITE			0b00000000
#define GENERIC_OPTION_OP_READ			0b01000000
#define GENERIC_OPTION_OP_MASK			0b01000000

#define GENERIC_OPTION_KEEP_CS			0b10000000

#define GENERIC_OPCODE_WRITE_STATUS		0x01
#define GENERIC_OPCODE_WRITE_DATA		0x02
#define GENERIC_OPCODE_READ_DATA		0x03
#define GENERIC_OPCODE_WRITE_DISABLE	0x04
#define GENERIC_OPCODE_READ_STATUS		0x05
#define GENERIC_OPCODE_WRITE_ENABLE		0x06
#define GENERIC_OPCODE_PAGE_PROGRAM		0x0a
#define GENERIC_OPCODE_FAST_READ		0x0b
#define GENERIC_OPCODE_RUID				0x4b
#define GENERIC_OPCODE_SLEEP			0xb9
#define GENERIC_OPCODE_ENABLE_RESET		0x66
#define GENERIC_OPCODE_RESET			0x99
#define GENERIC_OPCODE_CHIP_ERASE		0xc7

#define GENERIC_STATUS_WIP				0b00000001
#define GENERIC_STATUS_WEL				0b00000010

#define GENERIC_UNIQUE_ID_SIZE			8

#define Generic_Read_Byte(handle, Command, pByte)	Generic_SPI(handle, Command, GENERIC_OPTION_OP_READ  | GENERIC_OPTION_ADDRNO, 0, pByte, sizeof(BYTE))
#define Generic_Write_Byte(handle, Command, Byte)	Generic_SPI(handle, Command, GENERIC_OPTION_OP_WRITE | GENERIC_OPTION_ADDR1B, Byte, NULL, 0);
#define Generic_Command(handle, Command)			Generic_SPI(handle, Command, GENERIC_OPTION_ADDRNO, 0, NULL, 0)

#define Generic_Read_Status(handle, pStatus)		Generic_Read_Byte(handle, GENERIC_OPCODE_READ_STATUS, pStatus)
#define Generic_Write_Status(handle, status)		Generic_Write_Byte(handle, GENERIC_OPCODE_WRITE_STATUS, status)
#define Generic_Write_Enable(handle)				Generic_Command(handle, GENERIC_OPCODE_WRITE_ENABLE)
#define Generic_Write_Disable(handle)				Generic_Command(handle, GENERIC_OPCODE_WRITE_DISABLE)
#define Generic_Read_Unique_ID(handle, Unique_ID)	Generic_SPI(handle, GENERIC_OPCODE_RUID, GENERIC_OPTION_OP_READ | GENERIC_OPTION_ADDRNO | GENERIC_OPTION_DUMMY_FROM(4), 0, Unique_ID, GENERIC_UNIQUE_ID_SIZE);
#define Generic_Chip_Erase(handle)					Generic_Command(handle, GENERIC_OPCODE_CHIP_ERASE)

#define Generic_Read_Data(handle, option, address, buffer, size)	Generic_SPI(handle, GENERIC_OPCODE_READ_DATA, GENERIC_OPTION_OP_READ | option, address, (BYTE *) buffer, size)
#define Generic_Write_Data(handle, option, address, buffer, size)	Generic_SPI(handle, GENERIC_OPCODE_WRITE_DATA, GENERIC_OPTION_OP_WRITE | option, address, (BYTE *) buffer, size)

FT_STATUS Generic_Status_Wait_Ready(FT_HANDLE handle, BYTE* pStatus);
FT_STATUS Generic_Write_Enable_Confirmed(FT_HANDLE handle);
FT_STATUS Generic_Write_Disable_Confirmed(FT_HANDLE handle);
FT_STATUS Generic_Status_Wait_Ready_and_Write_Enable(FT_HANDLE handle);
FT_STATUS Generic_Status_Wait_Ready_and_Write_Disable(FT_HANDLE handle);

FT_STATUS Generic_Write_Enable_Write_Data(FT_HANDLE handle, BYTE OptAddrSize, DWORD address, const void* buffer, DWORD size);
FT_STATUS Generic_Write_Enable_Chip_Erase(FT_HANDLE handle);