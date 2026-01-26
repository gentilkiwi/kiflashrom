/*	Benjamin DELPY `gentilkiwi`
	https://blog.gentilkiwi.com
	benjamin@gentilkiwi.com
	Licence : https://creativecommons.org/licenses/by/4.0/
*/
#include "m95.h"

FT_STATUS M95_PageErase(PKFTDI_MPSSE_SPI_HANDLE pKFTDI, DWORD address)
{
	return Generic_SPI(pKFTDI, M95_OPCODE_PAGE_ERASE, GENERIC_OPTION_ADDR3B, address, NULL, 0);
}

FT_STATUS M95_ReadIdentification(PKFTDI_MPSSE_SPI_HANDLE pKFTDI, DWORD Address, BYTE *Identification, DWORD cbIdentification)
{
	FT_STATUS status;

	status = Generic_SPI(pKFTDI, M95_OPCODE_READ_IDENTIFICATION, GENERIC_OPTION_OP_READ | GENERIC_OPTION_ADDR3B, Address, Identification, cbIdentification);

	return status;
}