/*	Benjamin DELPY `gentilkiwi`
	https://blog.gentilkiwi.com
	benjamin@gentilkiwi.com
	Licence : https://creativecommons.org/licenses/by/4.0/
*/
#include "generic.h"

FT_STATUS Generic_SPI(PKFTDI_MPSSE_SPI_HANDLE pKFTDI, BYTE Command, BYTE Option, DWORD Address, BYTE* Data, DWORD cbData)
{
	FT_STATUS status;
	BYTE Buffer[1 + GENERIC_OPTION_ADDR_MAX + GENERIC_OPTION_DUMMY_MAX];
	WORD idx = 0, cbDummy;

	Buffer[idx++] = Command;

	switch (Option & GENERIC_OPTION_ADDR_MASK) {
	case GENERIC_OPTION_ADDR4B:
		Buffer[idx++] = (BYTE)(Address >> 24);
	case GENERIC_OPTION_ADDR3B:
		Buffer[idx++] = (BYTE)(Address >> 16);
	case GENERIC_OPTION_ADDR2B:
		Buffer[idx++] = (BYTE)(Address >> 8);
	case GENERIC_OPTION_ADDR1B:
		Buffer[idx++] = (BYTE)Address;
	}

	cbDummy = GENERIC_OPTION_DUMMY(Option);
	if (cbDummy)
	{
		RtlZeroMemory(Buffer + idx, cbDummy);
		idx += cbDummy;
	}

	FT_FAST_FAIL(KFTDI_MPSSE_SPI_CS_LOW(pKFTDI));
	FT_FAST_FAIL(KFTDI_MPSSE_SPI_WRITE(pKFTDI, Buffer, idx));
	if (Data && cbData)
	{
		if (Option & GENERIC_OPTION_OP_READ)
		{
			FT_FAST_FAIL(KFTDI_MPSSE_SPI_READ_EX(pKFTDI, Data, cbData));
		}
		else
		{
			FT_FAST_FAIL(KFTDI_MPSSE_SPI_WRITE_EX(pKFTDI, Data, cbData));
		}
	}


FT_FAIL:
	if (!(Option & GENERIC_OPTION_KEEP_CS))
	{
		KFTDI_MPSSE_SPI_CS_HIGH(pKFTDI);
	}

	return status;
}

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

FT_STATUS Generic_Read_ID(PKFTDI_MPSSE_SPI_HANDLE pKFTDI, DWORD* pUID)
{
	FT_STATUS status;
	BYTE in_out[2] = { JEDEC_ReadId, };
	BYTE bank, manufacturer;

	FT_FAST_FAIL(KFTDI_MPSSE_SPI_CS_LOW(pKFTDI));
	FT_FAST_FAIL(KFTDI_MPSSE_SPI_WRITE_READ(pKFTDI, in_out, in_out, 2));
	for (	bank = 1, manufacturer = in_out[1];
			(manufacturer == JEDEC_Continuation_Code) && (bank <= JEDEC_ReadId_MAX_Banks);
			bank++, manufacturer = in_out[0] )
	{
		FT_FAST_FAIL(KFTDI_MPSSE_SPI_READ(pKFTDI, in_out, 1));
	}

	if (manufacturer)
	{
		if (parity8(manufacturer & JEDEC_Continuation_Code) != (manufacturer >> 7))
		{
			manufacturer &= JEDEC_Continuation_Code;
			FT_FAST_FAIL(KFTDI_MPSSE_SPI_READ(pKFTDI, in_out, 2));
			if (pUID)
			{
				*pUID = MAKE_CHIP_UID(bank, manufacturer, in_out[0], in_out[1]);
			}
		}
		else
		{
			status = FT_IO_ERROR;
		}
	}
	else
	{
		status = FT_INVALID_PARAMETER;
	}

FT_FAIL:
	KFTDI_MPSSE_SPI_CS_HIGH(pKFTDI);

	return status;
}

FT_STATUS Generic_Read_SFDP(PKFTDI_MPSSE_SPI_HANDLE pKFTDI, BYTE SFDP[JEDEC_SFDP_Size])
{
	FT_STATUS status;

	FT_FAST_FAIL(Generic_SPI(pKFTDI, JEDEC_ReadSFDP, GENERIC_OPTION_OP_READ | GENERIC_OPTION_ADDR3B | GENERIC_OPTION_DUMMY_FROM(1), 0x000000, SFDP, JEDEC_SFDP_Size));
	if (*(DWORD*)SFDP != 'PDFS')
	{
		status = FT_OTHER_ERROR;
	}

FT_FAIL:
	return status;
}

FT_STATUS Generic_Status_Wait_Ready(PKFTDI_MPSSE_SPI_HANDLE pKFTDI, BYTE* pStatus)
{
	FT_STATUS status;
	BYTE Status;

	do
	{
		FT_FAST_FAIL(Generic_Read_Status(pKFTDI, &Status));
	} while (Status & GENERIC_STATUS_WIP);

	if (pStatus)
	{
		*pStatus = Status;
	}

FT_FAIL:
	return status;
}

FT_STATUS Generic_Write_Enable_Confirmed(PKFTDI_MPSSE_SPI_HANDLE pKFTDI)
{
	FT_STATUS status;
	BYTE Status;

	FT_FAST_FAIL(Generic_Write_Enable(pKFTDI));
	FT_FAST_FAIL(Generic_Read_Status(pKFTDI, &Status));
	if (!(Status & GENERIC_STATUS_WEL))
	{
		status = FT_OTHER_ERROR;
	}

FT_FAIL:
	return status;
}

FT_STATUS Generic_Write_Disable_Confirmed(PKFTDI_MPSSE_SPI_HANDLE pKFTDI)
{
	FT_STATUS status;
	BYTE Status;

	FT_FAST_FAIL(Generic_Write_Disable(pKFTDI));
	FT_FAST_FAIL(Generic_Read_Status(pKFTDI, &Status));
	if (Status & GENERIC_STATUS_WEL)
	{
		status = FT_OTHER_ERROR;
	}

FT_FAIL:
	return status;
}

FT_STATUS Generic_Status_Wait_Ready_and_Write_Enable(PKFTDI_MPSSE_SPI_HANDLE pKFTDI)
{
	FT_STATUS status;
	BYTE Status;

	FT_FAST_FAIL(Generic_Status_Wait_Ready(pKFTDI, &Status));
	if (!(Status & GENERIC_STATUS_WEL))
	{
		status = Generic_Write_Enable_Confirmed(pKFTDI);
	}

FT_FAIL:
	return status;
}

FT_STATUS Generic_Status_Wait_Ready_and_Write_Disable(PKFTDI_MPSSE_SPI_HANDLE pKFTDI)
{
	FT_STATUS status;
	BYTE Status;

	FT_FAST_FAIL(Generic_Status_Wait_Ready(pKFTDI, &Status));
	if (Status & GENERIC_STATUS_WEL)
	{
		status = Generic_Write_Disable_Confirmed(pKFTDI);
	}

FT_FAIL:
	return status;
}

FT_STATUS Generic_Write_Enable_Write_Data(PKFTDI_MPSSE_SPI_HANDLE pKFTDI, BYTE OptAddrSize, DWORD address, const void* buffer, DWORD size)
{
	FT_STATUS status;

	FT_FAST_FAIL(Generic_Status_Wait_Ready_and_Write_Enable(pKFTDI));
	status = Generic_Write_Data(pKFTDI, OptAddrSize, address, buffer, size);
	Generic_Status_Wait_Ready_and_Write_Disable(pKFTDI);

FT_FAIL:
	return status;
}

FT_STATUS Generic_Write_Enable_Chip_Erase(PKFTDI_MPSSE_SPI_HANDLE pKFTDI)
{
	FT_STATUS status;

	FT_FAST_FAIL(Generic_Status_Wait_Ready_and_Write_Enable(pKFTDI));
	status = Generic_Chip_Erase(pKFTDI);
	Generic_Status_Wait_Ready_and_Write_Disable(pKFTDI);

FT_FAIL:
	return status;
}