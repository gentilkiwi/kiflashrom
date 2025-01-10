/*	Benjamin DELPY `gentilkiwi`
	https://blog.gentilkiwi.com
	benjamin@gentilkiwi.com
	Licence : https://creativecommons.org/licenses/by/4.0/
*/
#include "generic.h"

const char* UNK_MANUFACTURER = "<not in hardcoded JEDEC table>";
const char* CHIP_UID_TO_MANUFACTURER_NAME(const DWORD UID)
{
	const char* ret = NULL;
	BYTE bank = CHIP_UID_TO_BANK(UID), manufacturer = CHIP_UID_TO_MANUFACTURER(UID);

	if (bank && (bank <= ARRAYSIZE(JEDEC_BANKS_MANUFACTURERS)))
	{
		if (manufacturer && (manufacturer <= ARRAYSIZE(JEDEC_BANKS_MANUFACTURERS[0])))
		{
			ret = JEDEC_BANKS_MANUFACTURERS[bank - 1][manufacturer - 1];
		}
	}

	if (!ret)
	{
		ret = UNK_MANUFACTURER;
	}

	return ret;
}