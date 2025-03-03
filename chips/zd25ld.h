/*	Benjamin DELPY `gentilkiwi`
	https://blog.gentilkiwi.com
	benjamin@gentilkiwi.com
	Licence : https://creativecommons.org/licenses/by/4.0/
*/
/* * * * * * * * * * * * * * * * * * * * * * * * *
 * Zetta Devices
 * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once
#include "../kiflashrom.h"

#define ZD25LD_JEDEC_BANK				9
#define ZD25LD_JEDEC_ALTERNATIVE_BANK	1 // chips don't respect 7f until 9th bank...
#define ZD25LD_JEDEC_MANUFACTURER		58

#define UID_ZD25LD40					MAKE_CHIP_UID(ZD25LD_JEDEC_ALTERNATIVE_BANK, ZD25LD_JEDEC_MANUFACTURER, 0x60, 0x13)

// Read
#define ZD25LD_FREAD	0x0b // Read Array (fast)				// GENERIC_OPCODE_FAST_READ
#define ZD25LD_READ		0x03 // Read Array (low power)			// GENERIC_OPCODE_READ_DATA
#define ZD25LD_DREAD	0x3b // Read Dual Output
#define ZD25LD_2READ	0xbb // Read 2x I/O
// Program and Erase
#define ZD25LD_PE		0x81 // Page Erase
#define ZD25LD_SE		0x20 // Sector Erase (4K bytes)
#define ZD25LD_BE32		0x52 // Block Erase (32K bytes)
#define ZD25LD_BE64		0xd8 // Block Erase (64K bytes)
#define ZD25LD_CE		0x60 // 0xc7 - Chip Erase
#define ZD25LD_PP		0x02 // Page Program
#define ZD25LD_2PP		0xa2 // Dual-IN Page Program
#define ZD25LD_PES		0x75 // 0xb0 - Program/Erase Suspend
#define ZD25LD_PER		0x7a // 0x30 - Program/Erase Resume
// Protection
#define ZD25LD_WREN		0x06 // Write Enable					// GENERIC_OPCODE_WRITE_ENABLE
#define ZD25LD_WRDI		0x04 // Write Disable					// GENERIC_OPCODE_WRITE_DISABLE
#define ZD25LD_VWREN	0x50 // Volatile SR Write Enable
// Security
#define ZD25LD_ERSCUR	0x44 // Erase Security Registers
#define ZD25LD_PRSCUR	0x42 // Program Security Registers
#define ZD25LD_RDSCUR	0x48 // Read Security Registers
// Status Register
#define ZD25LD_RDSR		0x05 // Read Status Register			// GENERIC_OPCODE_READ_STATUS
#define ZD25LD_RDSR2	0x35 // Read Status Register - 1
#define ZD25LD_ASI		0x25 // Active Status Interrupt
#define ZD25LD_WRSR		0x01 // Write Status Register			// GENERIC_OPCODE_WRITE_STATUS
// Other Commands
#define ZD25LD_RSTEN	0x66 // Reset Enable					// GENERIC_OPCODE_ENABLE_RESET
#define ZD25LD_RST		0x99 // Reset							// GENERIC_OPCODE_RESET
#define ZD25LD_RDID		0x9f // Read Manufacturer/device ID		// JEDEC_ReadId
#define ZD25LD_REMS		0x90 // Read Manufacture ID
#define ZD25LD_DREMS	0x92 // Dual Read Manufacture ID
#define ZD25LD_DP		0xb9 // Deep Power-down					// GENERIC_OPCODE_SLEEP
#define ZD25LD_RDP_RES	0xab // Release Deep Power-down / Read Electronic ID
#define ZD25LD_RDSFDP	0x5a // Read SFDP						// JEDEC_ReadSFDP
#define ZD25LD_RRE		0xff // Release read enhanced
#define ZD25LD_RUID		0x4b // Read unique ID					// GENERIC_OPCODE_RUID