/*	Benjamin DELPY `gentilkiwi`
	https://blog.gentilkiwi.com
	benjamin@gentilkiwi.com
	Licence : https://creativecommons.org/licenses/by/4.0/
*/
/* * * * * * * * * * * * * * * * * * * * * * * * *
 * Atmel
 * -----------------------------------------------
 * Atmel
 * Microchip
 * Adesto
 * Renesas
 * ...
 * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once
#include "generic.h"

#define AT25_JEDEC_BANK			1
#define AT25_JEDEC_MANUFACTURER	31

#define AT25_WREN	0x06 // Set Write Enable Latch (WEL)
#define AT25_WRDI	0x04 // Reset Write Enable Latch (WEL)
#define AT25_RDSR	0x05 // Read STATUS Register
#define AT25_WRSR	0x01 // Write STATUS Register
#define AT25_READ	0x03 // Read from Memory Array
#define AT25_WRITE	0x02 // or 0x07 - Write to Memory Array
#define AT25_LPWP	0x08 // Low-Power Write Poll