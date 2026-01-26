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

	KFTDI_MPSSE_SPI_GPIO_AD_SetPinDirValue(pKFTDI, PIN_SPI_CS, PIN_OUTPUT, PIN_LOW);
	status = KFTDI_MPSSE_SPI_DataShift(pKFTDI, Buffer, NULL, idx);
	//status = SPI_Write(pKFTDI, Buffer, idx, &szTransfered, SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE | SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES);
	if (FT_SUCCESS(status) && Data && cbData)
	{
		if (Option & GENERIC_OPTION_OP_READ)
		{
			status = KFTDI_MPSSE_SPI_DataShiftEx(pKFTDI, NULL, Data, cbData);
			//status = SPI_Read(pKFTDI, Data, cbData, &szTransfered, SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES);
		}
		else
		{
			status = KFTDI_MPSSE_SPI_DataShiftEx(pKFTDI, Data, NULL, cbData);
			//status = SPI_Write(pKFTDI, Data, cbData, &szTransfered, SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES);
		}
	}

	if (!(Option & GENERIC_OPTION_KEEP_CS))
	{
		KFTDI_MPSSE_SPI_GPIO_AD_SetPinDirValue(pKFTDI, PIN_SPI_CS, PIN_OUTPUT, PIN_HIGH);
		///*status = */SPI_ToggleCS(pKFTDI, FALSE);
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
	//DWORD sizeTransferred;
	BYTE in_out[2] = { JEDEC_ReadId, };
	BYTE bank, manufacturer;

	if (pKFTDI && pUID)
	{
		KFTDI_MPSSE_SPI_GPIO_AD_SetPinDirValue(pKFTDI, PIN_SPI_CS, PIN_OUTPUT, PIN_LOW);
		status = KFTDI_MPSSE_SPI_DataShift(pKFTDI, in_out, in_out, 2);
		//status = SPI_ReadWrite(pKFTDI, in_out, in_out, 2, &sizeTransferred, SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE | SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES);
		if (FT_SUCCESS(status))
		{
			for (
				bank = 1, manufacturer = in_out[1];
				(manufacturer == JEDEC_Continuation_Code) && (bank <= JEDEC_ReadId_MAX_Banks) && FT_SUCCESS(status);
				bank++, manufacturer = in_out[0]
				)
			{
				status = KFTDI_MPSSE_SPI_DataShift(pKFTDI, NULL, in_out, 1);
				//status = SPI_Read(pKFTDI, in_out, 1, &sizeTransferred, SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES);
			}

			if (FT_SUCCESS(status))
			{
				if (manufacturer)
				{
					if (parity8(manufacturer & JEDEC_Continuation_Code) != (manufacturer >> 7))
					{
						manufacturer &= JEDEC_Continuation_Code;
						status = KFTDI_MPSSE_SPI_DataShift(pKFTDI, NULL, in_out, 2);
						//status = SPI_Read(pKFTDI, in_out, 2, &sizeTransferred, SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES);
						if ((status == FT_OK) && pUID)
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
			}
		}
		KFTDI_MPSSE_SPI_GPIO_AD_SetPinDirValue(pKFTDI, PIN_SPI_CS, PIN_OUTPUT, PIN_HIGH);
		///*status = */SPI_ToggleCS(pKFTDI, FALSE);
	}
	else
	{
		status = FT_INVALID_ARGS;
	}

	return status;
}

FT_STATUS Generic_Read_SFDP(PKFTDI_MPSSE_SPI_HANDLE pKFTDI, BYTE SFDP[JEDEC_SFDP_Size])
{
	FT_STATUS status;

	status = Generic_SPI(pKFTDI, JEDEC_ReadSFDP, GENERIC_OPTION_OP_READ | GENERIC_OPTION_ADDR3B | GENERIC_OPTION_DUMMY_FROM(1), 0x000000, SFDP, JEDEC_SFDP_Size);
	if (FT_SUCCESS(status))
	{
		if (*(DWORD*)SFDP != 'PDFS')
		{
			status = FT_OTHER_ERROR;
		}
	}

	return status;
}

FT_STATUS Generic_Status_Wait_Ready(PKFTDI_MPSSE_SPI_HANDLE pKFTDI, BYTE* pStatus)
{
	FT_STATUS status;
	BYTE Status;

	do
	{
		status = Generic_Read_Status(pKFTDI, &Status);
		if (!FT_SUCCESS(status))
		{
			return status;
		}

	} while (Status & GENERIC_STATUS_WIP);

	if (pStatus)
	{
		*pStatus = Status;
	}

	return status;
}

FT_STATUS Generic_Write_Enable_Confirmed(PKFTDI_MPSSE_SPI_HANDLE pKFTDI)
{
	FT_STATUS status;
	BYTE Status;

	status = Generic_Write_Enable(pKFTDI);
	if (!FT_SUCCESS(status))
	{
		return status;
	}

	status = Generic_Read_Status(pKFTDI, &Status);
	if (!FT_SUCCESS(status))
	{
		return status;
	}

	if (!(Status & GENERIC_STATUS_WEL))
	{
		status = FT_OTHER_ERROR;
	}

	return status;
}

FT_STATUS Generic_Write_Disable_Confirmed(PKFTDI_MPSSE_SPI_HANDLE pKFTDI)
{
	FT_STATUS status;
	BYTE Status;

	status = Generic_Write_Disable(pKFTDI);
	if (!FT_SUCCESS(status))
	{
		return status;
	}

	status = Generic_Read_Status(pKFTDI, &Status);
	if (!FT_SUCCESS(status))
	{
		return status;
	}

	if (Status & GENERIC_STATUS_WEL)
	{
		status = FT_OTHER_ERROR;
	}

	return status;
}

FT_STATUS Generic_Status_Wait_Ready_and_Write_Enable(PKFTDI_MPSSE_SPI_HANDLE pKFTDI)
{
	FT_STATUS status;
	BYTE Status;

	status = Generic_Status_Wait_Ready(pKFTDI, &Status);
	if (!FT_SUCCESS(status))
	{
		return status;
	}

	if (!(Status & GENERIC_STATUS_WEL))
	{
		status = Generic_Write_Enable_Confirmed(pKFTDI);
	}

	return status;
}

FT_STATUS Generic_Status_Wait_Ready_and_Write_Disable(PKFTDI_MPSSE_SPI_HANDLE pKFTDI)
{
	FT_STATUS status;
	BYTE Status;

	status = Generic_Status_Wait_Ready(pKFTDI, &Status);
	if (!FT_SUCCESS(status))
	{
		return status;
	}

	if (Status & GENERIC_STATUS_WEL)
	{
		status = Generic_Write_Disable_Confirmed(pKFTDI);
	}

	return status;
}

FT_STATUS Generic_Write_Enable_Write_Data(PKFTDI_MPSSE_SPI_HANDLE pKFTDI, BYTE OptAddrSize, DWORD address, const void* buffer, DWORD size)
{
	FT_STATUS status;

	status = Generic_Status_Wait_Ready_and_Write_Enable(pKFTDI);
	if (!FT_SUCCESS(status))
	{
		return status;
	}

	status = Generic_Write_Data(pKFTDI, OptAddrSize, address, buffer, size);
	if (!FT_SUCCESS(status))
	{
		return status;
	}

	status = Generic_Status_Wait_Ready_and_Write_Disable(pKFTDI);

	return status;
}

FT_STATUS Generic_Write_Enable_Chip_Erase(PKFTDI_MPSSE_SPI_HANDLE pKFTDI)
{
	FT_STATUS status;

	status = Generic_Status_Wait_Ready_and_Write_Enable(pKFTDI);
	if (!FT_SUCCESS(status))
	{
		return status;
	}

	status = Generic_Chip_Erase(pKFTDI);
	if (!FT_SUCCESS(status))
	{
		return status;
	}

	status = Generic_Status_Wait_Ready_and_Write_Disable(pKFTDI);

	return status;
}