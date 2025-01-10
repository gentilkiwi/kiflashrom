/*	Benjamin DELPY `gentilkiwi`
	https://blog.gentilkiwi.com
	benjamin@gentilkiwi.com
	Licence : https://creativecommons.org/licenses/by/4.0/
*/
#include "at45.h"

FT_STATUS AT45_Read_Status(FT_HANDLE handle, BYTE* pStatus)
{
	FT_STATUS status;
	BYTE Command[2] = { AT45DB_OPCODE_STATUS_REGISTER_READ, };
	DWORD szTransfered;

	status = SPI_ReadWrite(handle, Command, Command, sizeof(Command), &szTransfered, SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE | SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE | SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES);
	if (status == FT_OK)
	{
		*pStatus = Command[1];
	}

	return status;
}

FT_STATUS AT45_Read_LF(FT_HANDLE handle, DWORD address, BYTE* buffer, DWORD size)
{
	FT_STATUS status;
	BYTE Command[] = { AT45DB_OPCODE_CONTINUOUS_ARRAY_READ_LOW_FREQ, (BYTE) (address >> 16), (BYTE)(address >> 8), (BYTE) address };
	DWORD szTransfered;

	status = SPI_Write(handle, Command, sizeof(Command), &szTransfered, SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE | SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES);
	if (status == FT_OK)
	{
		status = SPI_Read(handle, buffer, size, &szTransfered, SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES);
	}
	/*status = */SPI_ToggleCS(handle, FALSE);

	return status;
}

FT_STATUS AT45_Read_ID_EDI(FT_HANDLE handle, BYTE *cbEDI, BYTE **ppbEDI)
{
	FT_STATUS status;
	BYTE Command[5] = { AT45DB_OPCODE_MANUFACTURER_AND_DEVICE_ID_READ, };
	DWORD szTransfered;

	status = SPI_ReadWrite(handle, Command, Command, sizeof(Command), &szTransfered, SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE | SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES);
	if (status == FT_OK)
	{
		*cbEDI = Command[4];
		if (*cbEDI)
		{
			*ppbEDI = malloc(*cbEDI);
			if (*ppbEDI)
			{
				status = SPI_Read(handle, *ppbEDI, *cbEDI, &szTransfered, SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES);
				if (status != FT_OK)
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