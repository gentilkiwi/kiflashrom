/*	Benjamin DELPY `gentilkiwi`
	https://blog.gentilkiwi.com
	benjamin@gentilkiwi.com
	Licence : https://creativecommons.org/licenses/by/4.0/
*/
#include "kiflashrom.h"

int wmain(int argc, wchar_t* argv[])
{
	FT_STATUS status;
	KFTDI_MPSSE_SPI_HANDLE hKFTDI;
	DWORD dwIndex, libftd2xx = 0, dwFrequency = SPI_CONFIG_DEFAULT_FREQUENCY;
	LPCWSTR frequency, filename, size;
	BYTE OptAddrSize = GENERIC_OPTION_ADDRNO;
	DWORD UID, Size = 0;

	status = FT_GetLibraryVersion(&libftd2xx);
	if (!FT_SUCCESS(status))
	{
		PRINT_FT_ERROR(L"Ver_libMPSSE", status);
		return -1;
	}

	kprintf(L"\n"
		L"  .#####.          kiflashrom 0.0c ( D2XX %hx.%hhx.%hhx )\n"
		L" .## ^ ##\xda\xc4\xc4\xc4\xc4\xc4\xbf\n"
		L" ## / \\ \xc4\xb4     \xc3\xc4  /*** Benjamin DELPY `gentilkiwi` ( benjamin@gentilkiwi.com )\n"
		L" ## \\ / \xc4\xb4 k   \xc3\xc4      > https://blog.gentilkiwi.com\n"
		L" '## v  \xc4\xb4   f \xc3\xc4   ***/\n"
		L"  '#### \xc4\xb4     \xc3\xc4\n"
		L"         \xc0\xc4\xc4\xc4\xc4\xc4\xd9\n\n",
		(WORD)(libftd2xx >> 16), (BYTE)(libftd2xx >> 8), (BYTE)(libftd2xx));

	/* frequency 
	 * ---------
	 * 30MHz will be divided by 1, 2, etc., some values will not be exact (obviously like 20 MHz)
	 * default is SPI_CONFIG_DEFAULT_FREQUENCY (usually 5 MHz)
	 */
	if (GET_CLI_ARG(L"frequency", &frequency))
	{
		dwFrequency = wcstoul(frequency, NULL, 0);
		kprintf(L"> Using frequency: %s (%lu Hz)\n", frequency, dwFrequency);
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
	
	status = FT_K_SelectChannel(argc, argv, &dwIndex);
	if (FT_SUCCESS(status))
	{
		kprintf(L"SPI:\n"L"\xc0 SPI ClockRate: %lu Hz\n", dwFrequency);
		status = KFTDI_MPSSE_SPI_Open(dwIndex, KFTDI_MPSSE_SPI_MODE_0 | KFTDI_MPSSE_SPI_MODE_MSB | KFTDI_MPSSE_SPI_MODE_BYTE, dwFrequency, &hKFTDI);
		if (FT_SUCCESS(status))
		{
			if (GET_CLI_ARG_PRESENT(L"NRF24LU1P")) // does not answer to JEDEC id.
			{
				BOOL bIsNRF24LU1P_F16 = GET_CLI_ARG_PRESENT(L"f16"), bIsBlind = GET_CLI_ARG_PRESENT(L"blind");

				if (GET_CLI_ARG_PRESENT(L"unbrick"))
				{
					NRF24LU1P_Unbrick(&hKFTDI, bIsNRF24LU1P_F16, bIsBlind);
				}

				if (!bIsBlind)
				{
					if (Size == 0)
					{
						Size = bIsNRF24LU1P_F16 ? NRF24LU1P_FLASH_SIZE_F16 : NRF24LU1P_FLASH_SIZE_F32;
					}
					GenericComparedRead(&hKFTDI, GENERIC_OPTION_ADDR2B, Size, filename);
				}
			}
			else
			{
				status = Generic_Read_ID(&hKFTDI, &UID);
				if (FT_SUCCESS(status))
				{
					kprintf(L"\nJEDEC JEP106 Read UID (internal is 0x%08x)\n"
						L"\xc3 Bank# %hhu, Manufacturer# %hhu (hex/p: 0x%02X), Name: %S\n"
						L"\xc3 DeviceID : 0x%04hX\n"
						L"\xc3 !! double check: some manufacturers are not respecting their banks !!\n",
						UID,
						CHIP_UID_TO_BANK(UID), CHIP_UID_TO_MANUFACTURER(UID), CHIP_UID_TO_MANUFACTURER(UID) | (!parity8(CHIP_UID_TO_MANUFACTURER(UID)) << 7), CHIP_UID_TO_MANUFACTURER_NAME(UID),
						CHIP_UID_TO_DEVICEID(UID)
					);

					kprintf(L"\xc0 Chip name: ");
					switch (UID)
					{
					case UID_FM25V02:
						kprintf(L"FM25V02\n");
						FM25V0x(&hKFTDI, &OptAddrSize, &Size);
						break;
					case UID_AT45DB081:
						kprintf(L"AT45DB081 family\n");
						AT45DBx(&hKFTDI, &OptAddrSize, &Size);
						break;
					case UID_W25X20CL_CV:
					case UID_W25Q16_DV_JL_JVxxQ:
					case UID_W25Q32_RV_FVxxG_JVxxQ:
						W25X(&hKFTDI, UID, &OptAddrSize, &Size);
						break;
					case UID_M95P32:
						kprintf(L"M95P32\n");
						M95(&hKFTDI, &OptAddrSize, &Size);
						break;
					case UID_ZD25LD40:
						kprintf(L"ZD25LD40\n");
						ZD25LD(&hKFTDI, &OptAddrSize, &Size);
						break;

					default:
						kprintf(L"?\n");
					}

					if (OptAddrSize && Size)
					{
						GenericComparedRead(&hKFTDI, OptAddrSize, Size, filename);
					}
				}
				else PRINT_FT_ERROR(L"Read_ID", status);
			}
		}
		else PRINT_FT_ERROR(L"KFTDI_MPSSE_SPI_Open", status);

		status = FT_Close(hKFTDI.ftHandle);
	}

	return EXIT_SUCCESS;
}

void GenericComparedRead(PKFTDI_MPSSE_SPI_HANDLE pKFTDI, BYTE OptAddrSize, DWORD Size, PCWSTR filename)
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
			status = Generic_Read_Data(pKFTDI, OptAddrSize, 0, buffer, Size);
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

void FM25V0x(PKFTDI_MPSSE_SPI_HANDLE pKFTDI, BYTE* pOptAddrSize, DWORD* pSize)
{
	UNREFERENCED_PARAMETER(pKFTDI);

	*pOptAddrSize = GENERIC_OPTION_ADDR2B;
	if (!*pSize)
	{
		*pSize = 0x8000; // 32K
	}
}

void AT45DBx(PKFTDI_MPSSE_SPI_HANDLE pKFTDI, BYTE* pOptAddrSize, DWORD *pSize)
{
	UNREFERENCED_PARAMETER(pKFTDI);

	*pOptAddrSize = GENERIC_OPTION_ADDR3B;
	if (!*pSize)
	{
		*pSize = 0x100000; // 1M
	}
}

void W25X(PKFTDI_MPSSE_SPI_HANDLE pKFTDI, DWORD UID, BYTE* pOptAddrSize, DWORD* pSize)
{
	FT_STATUS status;
	BYTE ChipUID[GENERIC_UNIQUE_ID_SIZE];

	*pOptAddrSize = GENERIC_OPTION_ADDR3B;
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

	status = Generic_Read_Unique_ID(pKFTDI, ChipUID);
	if (FT_SUCCESS(status))
	{
		kprintf(L"\xc0 UID: 0x%016llx / ", *(PULONGLONG)ChipUID);
		kprinthex(ChipUID, sizeof(ChipUID));
	}
	else PRINT_FT_ERROR(L"W25_Read_Unique_ID", status);
}

void M95(PKFTDI_MPSSE_SPI_HANDLE pKFTDI, BYTE* pOptAddrSize, DWORD* pSize)
{
	UNREFERENCED_PARAMETER(pKFTDI);

	*pOptAddrSize = GENERIC_OPTION_ADDR3B;
	if (!*pSize)
	{
		*pSize = 0x400000; // 4Mb
	}
}

void ZD25LD(PKFTDI_MPSSE_SPI_HANDLE pKFTDI, BYTE* pOptAddrSize, DWORD* pSize)
{
	FT_STATUS status;
	BYTE ChipUID[GENERIC_UNIQUE_ID_SIZE];

	*pOptAddrSize = GENERIC_OPTION_ADDR3B;
	if (!*pSize)
	{
		*pSize = 0x80000; // 512Kb
	}

	status = Generic_Read_Unique_ID(pKFTDI, ChipUID);
	if (FT_SUCCESS(status))
	{
		kprintf(L"\xc0 UID: 0x%016llx / ", *(PULONGLONG)ChipUID);
		kprinthex(ChipUID, sizeof(ChipUID));
	}
	else PRINT_FT_ERROR(L"ZD25LD_Read_UID", status);
}