/*	Benjamin DELPY `gentilkiwi`
	https://blog.gentilkiwi.com
	benjamin@gentilkiwi.com
	Licence : https://creativecommons.org/licenses/by/4.0/
*/
#include "at45DB.h"

FT_STATUS AT45DB_Read_ID_EDI(PKFTDI_MPSSE_SPI_HANDLE pKFTDI, BYTE *cbEDI, BYTE **ppbEDI)
{
	FT_STATUS status;
	BYTE Buffer[4];

	FT_FAST_FAIL(Generic_SPI(pKFTDI, AT45DB_OPCODE_MANUFACTURER_AND_DEVICE_ID_READ, GENERIC_OPTION_ADDRNO | GENERIC_OPTION_OP_READ | GENERIC_OPTION_KEEP_CS, 0, Buffer, sizeof(Buffer)));
	// We ignore JEDEC bytes until EDI
	*cbEDI = Buffer[3];
	if (*cbEDI)
	{
		*ppbEDI = LocalAlloc(LPTR, *cbEDI);
		if (*ppbEDI)
		{
			status = KFTDI_MPSSE_SPI_READ(pKFTDI, *ppbEDI, *cbEDI);
			if (!FT_SUCCESS(status))
			{
				LocalFree(*ppbEDI);
				*ppbEDI = NULL;
				*cbEDI = 0;
			}
		}
	}

FT_FAIL:
	KFTDI_MPSSE_SPI_CS_HIGH(pKFTDI);

	return status;
}