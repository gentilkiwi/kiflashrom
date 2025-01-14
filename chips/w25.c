/*	Benjamin DELPY `gentilkiwi`
	https://blog.gentilkiwi.com
	benjamin@gentilkiwi.com
	Licence : https://creativecommons.org/licenses/by/4.0/
*/
#include "w25.h"

FT_STATUS W25_Read_Status(FT_HANDLE handle, BYTE* pStatus)
{
	FT_STATUS status;
	BYTE Command[2] = { W25_OPCODE_READ_STATUS_REGISTER, };
	DWORD szTransfered;

	status = SPI_ReadWrite(handle, Command, Command, sizeof(Command), &szTransfered, SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE | SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE | SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES);
	if (status == FT_OK)
	{
		*pStatus = Command[1];
	}

	return status;
}

FT_STATUS W25_Read_Data(FT_HANDLE handle, DWORD address, BYTE* buffer, DWORD size)
{
	FT_STATUS status;
	BYTE Command[] = { W25_OPCODE_READ_DATA, (BYTE)(address >> 16), (BYTE)(address >> 8), (BYTE)address };
	DWORD szTransfered;

	status = SPI_Write(handle, Command, sizeof(Command), &szTransfered, SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE | SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES);
	if (status == FT_OK)
	{
		status = SPI_Read(handle, buffer, size, &szTransfered, SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES);
	}
	/*status = */SPI_ToggleCS(handle, FALSE);

	return status;
}

FT_STATUS W25_Read_Unique_ID(FT_HANDLE handle, BYTE UID[W25_UNIQUE_ID_SIZE])
{
	FT_STATUS status;
	BYTE Command[5] = { W25_OPCODE_READ_UNIQUE_ID, };
	DWORD szTransfered;

	status = SPI_Write(handle, Command, sizeof(Command), &szTransfered, SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE | SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES);
	if (status == FT_OK)
	{
		status = SPI_Read(handle, UID, W25_UNIQUE_ID_SIZE, &szTransfered, SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES);
	}
	/*status = */SPI_ToggleCS(handle, FALSE);

	return status;
}