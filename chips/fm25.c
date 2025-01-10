/*	Benjamin DELPY `gentilkiwi`
	https://blog.gentilkiwi.com
	benjamin@gentilkiwi.com
	Licence : https://creativecommons.org/licenses/by/4.0/
*/
#include "fm25.h"

FT_STATUS FM25_Write_Enable(FT_HANDLE handle)
{
	FT_STATUS status;
	BYTE Command = FM25_OPCODE_WREN;
	DWORD szTransfered;

	status = SPI_Write(handle, &Command, sizeof(Command), &szTransfered, SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE | SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE | SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES);

	return status;
}

FT_STATUS FM25_Write_Disable(FT_HANDLE handle)
{
	FT_STATUS status;
	BYTE Command = FM25_OPCODE_WRDI;
	DWORD szTransfered;

	status = SPI_Write(handle, &Command, sizeof(Command), &szTransfered, SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE | SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE | SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES);

	return status;
}

FT_STATUS FM25_Read_Status(FT_HANDLE handle, BYTE* pStatus)
{
	FT_STATUS status;
	BYTE Command[2] = { FM25_OPCODE_RDSR, };
	DWORD szTransfered;

	status = SPI_ReadWrite(handle, Command, Command, sizeof(Command), &szTransfered, SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE | SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE | SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES);
	if (status == FT_OK)
	{
		*pStatus = Command[1];
	}

	return status;
}

FT_STATUS FM25_Write_Status(FT_HANDLE handle, BYTE Status)
{
	FT_STATUS status;
	BYTE Command[] = { FM25_OPCODE_WRSR, Status };
	DWORD szTransfered;

	status = SPI_Write(handle, Command, sizeof(Command), &szTransfered, SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE | SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE | SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES);

	return status;
}

FT_STATUS FM25_Read(FT_HANDLE handle, DWORD address, BYTE* buffer, DWORD size)
{
	FT_STATUS status;
	BYTE Command[] = { FM25_OPCODE_READ, (BYTE)(address >> 8), (BYTE)(address & 0xff) };
	DWORD szTransfered;

	status = SPI_Write(handle, Command, sizeof(Command), &szTransfered, SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE | SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES);
	if (status == FT_OK)
	{
		status = SPI_Read(handle, buffer, size, &szTransfered, SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES);
	}
	/*status = */SPI_ToggleCS(handle, FALSE);

	return status;
}