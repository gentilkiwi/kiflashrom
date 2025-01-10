/*	Benjamin DELPY `gentilkiwi`
	https://blog.gentilkiwi.com
	benjamin@gentilkiwi.com
	Licence : https://creativecommons.org/licenses/by/4.0/
*/
#pragma once
#include <windows.h>
#include "../jedec_manufacturers.h"

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
