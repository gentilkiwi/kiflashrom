/*	Benjamin DELPY `gentilkiwi`
	https://blog.gentilkiwi.com
	benjamin@gentilkiwi.com
	Licence : https://creativecommons.org/licenses/by/4.0/
*/
#include "at45DB.h"

FT_STATUS AT45DB_Read_ID_EDI(FT_HANDLE handle, BYTE *cbEDI, BYTE **ppbEDI)
{
	FT_STATUS status;
	//BYTE Command[5] = { AT45DB_OPCODE_MANUFACTURER_AND_DEVICE_ID_READ, };
	BYTE Buffer[4];
	DWORD szTransfered;

	status = Generic_SPI(handle, AT45DB_OPCODE_MANUFACTURER_AND_DEVICE_ID_READ, GENERIC_OPTION_ADDRNO | GENERIC_OPTION_OP_READ | GENERIC_OPTION_KEEP_CS, 0, Buffer, sizeof(Buffer));
	//status = SPI_ReadWrite(handle, Command, Command, sizeof(Command), &szTransfered, SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE | SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES);
	if (FT_SUCCESS(status))
	{
		// We ignore JEDEC bytes until EDI
		//*cbEDI = Command[4];
		*cbEDI = Buffer[3];
		if (*cbEDI)
		{
			*ppbEDI = malloc(*cbEDI);
			if (*ppbEDI)
			{
				status = SPI_Read(handle, *ppbEDI, *cbEDI, &szTransfered, SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES);
				if (!FT_SUCCESS(status))
				{
					free(*ppbEDI);
					*ppbEDI = NULL;
					*cbEDI = 0;
				}
			}

		}
	}
	/*status = */SPI_ToggleCS(handle, FALSE);

	return status;
}