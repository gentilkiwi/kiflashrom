/*	Benjamin DELPY `gentilkiwi`
	https://blog.gentilkiwi.com
	benjamin@gentilkiwi.com
	Licence : https://creativecommons.org/licenses/by/4.0/
*/
#include "kiflashrom.h"

int wmain(int argc, wchar_t* argv[])
{
	FT_STATUS status;
	FT_HANDLE handle;
	DWORD libmpsse = 0, libftd2xx = 0;
	ChannelConfig config = { SPI_CONFIG_DEFAULT_FREQUENCY, 10, SPI_CONFIG_OPTION_MODE0 | SPI_CONFIG_OPTION_CS_DBUS3 | SPI_CONFIG_OPTION_CS_ACTIVELOW };
	DWORD UID;
	LPCWSTR frequency, filename, size;
	PKSIMPLE_READ SimpleRead = NULL;
	DWORD Size = 0;

	status = Ver_libMPSSE(&libmpsse, &libftd2xx);
	if (!FT_SUCCESS(status))
	{
		PRINT_FT_ERROR(L"Ver_libMPSSE", status);
		return -1;
	}

	kprintf(L"\n"
		L"  .#####.          kiflashrom 0.0a ( MPSSE %hx.%hhx.%hhx, D2XX %hx.%hhx.%hhx )\n"
		L" .## ^ ##\xda\xc4\xc4\xc4\xc4\xc4\xbf\n"
		L" ## / \\ \xc4\xb4     \xc3\xc4  /*** Benjamin DELPY `gentilkiwi` ( benjamin@gentilkiwi.com )\n"
		L" ## \\ / \xc4\xb4 k   \xc3\xc4      > https://blog.gentilkiwi.com\n"
		L" '## v  \xc4\xb4   f \xc3\xc4\n"
		L"  '#### \xc4\xb4     \xc3\xc4\n"
		L"         \xc0\xc4\xc4\xc4\xc4\xc4\xd9\n\n",
		(WORD)(libmpsse >> 16), (BYTE)(libmpsse >> 8), (BYTE)(libmpsse), (WORD)(libftd2xx >> 16), (BYTE)(libftd2xx >> 8), (BYTE)(libftd2xx));

	/* frequency 
	 * ---------
	 * 30MHz will be divided by 1, 2, etc., some values will not be exact (obviously like 20 MHz)
	 * default is SPI_CONFIG_DEFAULT_FREQUENCY (usually 5 MHz)
	 */
	if (GET_CLI_ARG(L"frequency", &frequency))
	{
		config.ClockRate = wcstoul(frequency, NULL, 0);
		kprintf(L"> Using frequency: %s (%lu Hz)\n", frequency, config.ClockRate);
	}

	if (GET_CLI_ARG(L"file", &filename))
	{
		kprintf(L"> Using filename: %s\n", filename);
	}

	if (GET_CLI_ARG(L"size", &size))
	{
		Size = wcstoul(size, NULL, 0);
		kprintf(L"> Using data size: %s (%lu bytes)\n", size, Size);
	}

	if (argc > 1)
	{
		kprintf(L"\n");
	}
	
	status = FT_K_SelectChannel(argc, argv, &handle);
	if (FT_SUCCESS(status))
	{
		kprintf(L"SPI:\n"L"\xc0 SPI ClockRate: %lu Hz\n", config.ClockRate);
		status = SPI_InitChannel(handle, &config);
		if (FT_SUCCESS(status))
		{
			status = Read_ID(handle, &UID);
			if (FT_SUCCESS(status))
			{
				kprintf(L"\nUID: 0x%08x, JEDEC ID:\n"
					L"\xc3 Bank# %hhu, Manufacturer# %hhu (hex/p: 0x%02X), Name: %S\n"
					L"\xc3 DeviceID : 0x%04hX\n",
					UID,
					CHIP_UID_TO_BANK(UID), CHIP_UID_TO_MANUFACTURER(UID), CHIP_UID_TO_MANUFACTURER(UID) | (!parity8(CHIP_UID_TO_MANUFACTURER(UID)) << 7), CHIP_UID_TO_MANUFACTURER_NAME(UID),
					CHIP_UID_TO_DEVICEID(UID)
				);

				kprintf(L"\xc0 Chip name: ");
				switch (UID)
				{
				case UID_FM25V02:
					kprintf(L"FM25V02\n\n");
					FM25V0x(handle, &SimpleRead, &Size);
					break;
				case UID_AT45DB081:
					kprintf(L"AT45DB081 family\n\n");
					AT45DBx(handle, &SimpleRead, &Size);
					break;
				case UID_W25X20CL_CV:
				case UID_W25Q16_DV_JL_JVxxQ:
				case UID_W25Q32_RV_FVxxG_JVxxQ:
					W25X(handle, UID, &SimpleRead, &Size);
					break;
				default:
					kprintf(L"?\n");
				}

				if (SimpleRead && Size)
				{
					GenericComparedRead(handle, SimpleRead, Size, filename);
				}

			}
			else PRINT_FT_ERROR(L"Read_ID", status);
		}
		else PRINT_FT_ERROR(L"SPI_InitChannel", status);

		status = SPI_CloseChannel(handle);
	}

	return EXIT_SUCCESS;
}

FT_STATUS Read_ID(FT_HANDLE handle, DWORD* pUID)
{
	FT_STATUS status;
	DWORD sizeTransferred;
	BYTE in_out[2] = { JEDEC_ReadId, };
	BYTE bank, manufacturer;
	
	if (handle && pUID)
	{
		status = SPI_ReadWrite(handle, in_out, in_out, 2, &sizeTransferred, SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE | SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES);
		if (FT_SUCCESS(status))
		{
			for (
				bank = 1, manufacturer = in_out[1];
				(manufacturer == JEDEC_Continuation_Code) && (bank <= JEDEC_ReadId_MAX_Banks) && (status == FT_OK);
				bank++, manufacturer = in_out[0]
				)
			{
				status = SPI_Read(handle, in_out, 1, &sizeTransferred, SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES);
			}
			
			if (FT_SUCCESS(status))
			{
				if (manufacturer)
				{
					if (parity8(manufacturer & JEDEC_Continuation_Code) != (manufacturer >> 7))
					{
						manufacturer &= JEDEC_Continuation_Code;
						status = SPI_Read(handle, in_out, 2, &sizeTransferred, SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES);
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
			/*status = */SPI_ToggleCS(handle, FALSE);
		}
	}
	else
	{
		status = FT_INVALID_ARGS;
	}

	return status;
}

void GenericComparedRead(FT_HANDLE handle, PKSIMPLE_READ SimpleRead, DWORD Size, PCWSTR filename)
{
	FT_STATUS status;
	BYTE *buffer, i, isHashOK = 1, Hash[32], HashToComp[32];

	kprintf(L"\nCompared read for %lu byte(s) - %hhu iteration(s)\n", Size, (BYTE) COMPARED_READ_ITERATIONS);

	buffer = malloc(Size);
	if (buffer)
	{
		for (i = 0; (i < COMPARED_READ_ITERATIONS) && isHashOK; i++)
		{
			isHashOK = 0;
			status = SimpleRead(handle, 0, buffer, Size);
			if (FT_SUCCESS(status))
			{
				kprintf(L"\xc3 Read: %lu byte(s) - SHA2-256(data)= ", Size);
				if (kull_m_crypto_hash_sha256(buffer, Size, Hash))
				{
					kprinthex(Hash, sizeof(Hash));
					if (!i)
					{
						memcpy(HashToComp, Hash, sizeof(Hash));
						isHashOK = 1;
					}
					else
					{
						isHashOK = !memcmp(HashToComp, Hash, sizeof(Hash));
					}
				}
			}
			else PRINT_FT_ERROR(L"SimpleRead", status);
		}

		if (isHashOK)
		{
			kprintf(L"%c Data: ", filename ? L'\xc3' : L'\xc0');
			kull_m_cli_kprinthex(buffer, min(Size, 32), KPrintHexSpace, 0, FALSE);
			if (Size > 32)
			{
				kprintf(L"..");
			}
			kprintf(L"\n");

			if (filename)
			{
				kprintf(L"\xc0 Output to: %s\n", filename);
				kull_m_file_writeData(filename, buffer, Size);
			}
		}
		else PRINT_ERROR("Data not reliable\n");

		free(buffer);
	}
	else PRINT_ERROR(L"malloc for %lu byte(s)\n", Size);
}

void FM25V0x(FT_HANDLE handle, PKSIMPLE_READ* pSimpleRead, DWORD* pSize)
{
	FT_STATUS status;
	BYTE Status;

	*pSimpleRead = FM25_Read;
	if (!*pSize)
	{
		*pSize = 0x8000; // 32K
	}

	kprintf(L"FM25V0x tests:\n");

	status = FM25_Read_Status(handle, &Status);
	if (FT_SUCCESS(status))
	{
		kprintf(L"\xc3 Read Status: 0x%02hhx\n", Status);
	}
	else PRINT_FT_ERROR(L"FM25_Read_Status", status);

	status = FM25_Write_Enable(handle);

	status = FM25_Read_Status(handle, &Status);
	if (FT_SUCCESS(status))
	{
		kprintf(L"\xc3 Read Status: 0x%02hhx\n", Status);
	}
	else PRINT_FT_ERROR(L"FM25_Read_Status", status);

	status = FM25_Write_Disable(handle);

	status = FM25_Read_Status(handle, &Status);
	if (FT_SUCCESS(status))
	{
		kprintf(L"\xc3 Read Status: 0x%02hhx\n", Status);
	}
	else PRINT_FT_ERROR(L"FM25_Read_Status", status);
}

void AT45DBx(FT_HANDLE handle, PKSIMPLE_READ *pSimpleRead, DWORD *pSize)
{
	FT_STATUS status;
	BYTE Status;

	*pSimpleRead = AT45_Read_LF;
	if (!*pSize)
	{
		*pSize = 0x100000; // 1M
	}

	kprintf(L"AT45DBx tests:\n");
	status = AT45_Read_Status(handle, &Status);
	if (FT_SUCCESS(status))
	{
		kprintf(L"\xc3 Read Status: 0x%02hhx\n", Status);
	}
	else PRINT_FT_ERROR(L"AT45_Read_Status", status);

	status = AT45_Read_Status(handle, &Status);
	if (FT_SUCCESS(status))
	{
		kprintf(L"\xc0 Read Status: 0x%02hhx\n", Status);
	}
	else PRINT_FT_ERROR(L"AT45_Read_Status", status);
}

void W25X(FT_HANDLE handle, DWORD UID, PKSIMPLE_READ* pSimpleRead, DWORD* pSize)
{
	FT_STATUS status;
	BYTE Status, ChipUID[W25_UNIQUE_ID_SIZE];

	*pSimpleRead = W25_Read_Data;
	if (!*pSize)
	{
		switch(UID)
		{
		case UID_W25X20CL_CV:
			kprintf(L"W25X20CL/CV");
			*pSize = 0x40000; // 256K
			break;
		case UID_W25Q16_DV_JL_JVxxQ:
			kprintf(L"W25Q16DV/JL/JVxxQ");
			*pSize = 0x200000; // 2M
			break;
		case UID_W25Q32_RV_FVxxG_JVxxQ:
			kprintf(L"W25Q32RV/FVxxG/JVxxQ");
			*pSize = 0x400000; // 4M
			break;
		default:
			kprintf(L"?");
			*pSize = 0;
		}
	}

	kprintf(L"\n\nW25x tests:\n");
	status = W25_Read_Status(handle, &Status);
	if (FT_SUCCESS(status))
	{
		kprintf(L"\xc3 Read Status: 0x%02hhx\n", Status);
	}
	else PRINT_FT_ERROR(L"W25_Read_Status", status);

	status = W25_Read_Unique_ID(handle, ChipUID);
	if (FT_SUCCESS(status))
	{
		kprintf(L"\xc0 UID: 0x%016llx / ", *(PULONGLONG)ChipUID);
		kprinthex(ChipUID, sizeof(ChipUID));
	}
	else PRINT_FT_ERROR(L"W25_Read_Unique_ID", status);
}