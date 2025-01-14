/*	Benjamin DELPY `gentilkiwi`
	https://blog.gentilkiwi.com
	benjamin@gentilkiwi.com
	Licence : https://creativecommons.org/licenses/by/4.0/
*/
#include "utils.h"

BYTE parity8(BYTE x)
{
	x ^= x >> 4;
	x ^= x >> 2;
	x ^= x >> 1;
	return x & 1;
}

const PCWSTR FT_STATUS_UNK = L"FT_?";
const DUAL_STRING_FT_STATUS FT_STATUS_MESSAGES[] = {
	{L"FT_OK",							FT_OK},
	{L"FT_INVALID_HANDLE",				FT_INVALID_HANDLE},
	{L"FT_DEVICE_NOT_FOUND",			FT_DEVICE_NOT_FOUND},
	{L"FT_DEVICE_NOT_OPENED",			FT_DEVICE_NOT_OPENED},
	{L"FT_IO_ERROR",					FT_IO_ERROR},
	{L"FT_INSUFFICIENT_RESOURCES",		FT_INSUFFICIENT_RESOURCES},
	{L"FT_INVALID_PARAMETER",			FT_INVALID_PARAMETER},
	{L"FT_INVALID_BAUD_RATE",			FT_INVALID_BAUD_RATE},

	{L"FT_DEVICE_NOT_OPENED_FOR_ERASE",	FT_DEVICE_NOT_OPENED_FOR_ERASE},
	{L"FT_DEVICE_NOT_OPENED_FOR_WRITE",	FT_DEVICE_NOT_OPENED_FOR_WRITE},
	{L"FT_FAILED_TO_WRITE_DEVICE",		FT_FAILED_TO_WRITE_DEVICE},

	{L"FT_EEPROM_READ_FAILED",			FT_EEPROM_READ_FAILED},
	{L"FT_EEPROM_WRITE_FAILED",			FT_EEPROM_WRITE_FAILED},
	{L"FT_EEPROM_ERASE_FAILED",			FT_EEPROM_ERASE_FAILED},
	{L"FT_EEPROM_NOT_PRESENT",			FT_EEPROM_NOT_PRESENT},
	{L"FT_EEPROM_NOT_PROGRAMMED",		FT_EEPROM_NOT_PROGRAMMED},

	{L"FT_INVALID_ARGS",				FT_INVALID_ARGS},
	{L"FT_NOT_SUPPORTED",				FT_NOT_SUPPORTED},
	{L"FT_OTHER_ERROR",					FT_OTHER_ERROR},
	{L"FT_DEVICE_LIST_NOT_READY",		FT_DEVICE_LIST_NOT_READY},
};

PCWSTR FT_STATUS_to_NAME(FT_STATUS status)
{
	DWORD i;
	PCWSTR ret = FT_STATUS_UNK;

	for (i = 0; i < ARRAYSIZE(FT_STATUS_MESSAGES); i++)
	{
		if (FT_STATUS_MESSAGES[i].status == status)
		{
			ret = FT_STATUS_MESSAGES[i].name + 3;
			break;
		}
	}

	return ret;
}

BOOL kull_m_crypto_hash_sha256(LPCVOID pcvData, DWORD cbData, BYTE Hash[32])
{
	BOOL status = FALSE;
	HCRYPTPROV hProv;
	HCRYPTHASH hHash;
	DWORD cbHash = 32;

	if (CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_AES, CRYPT_VERIFYCONTEXT))
	{
		if (CryptCreateHash(hProv, CALG_SHA_256, 0, 0, &hHash))
		{
			if (CryptHashData(hHash, (LPCBYTE)pcvData, cbData, 0))
			{
				if (CryptGetHashParam(hHash, HP_HASHVAL, Hash, &cbHash, 0))
				{
					status = TRUE;
				}
				else PRINT_ERROR_AUTO(L"CryptGetHashParam");
			}
			else PRINT_ERROR_AUTO(L"CryptHashData");

			CryptDestroyHash(hHash);
		}
		else PRINT_ERROR_AUTO(L"CryptCreateHash");

		CryptReleaseContext(hProv, 0);
	}
	else PRINT_ERROR_AUTO(L"CryptAcquireContext");

	return status;
}

BOOL kull_m_cli_args_byName(const int argc, const wchar_t* argv[], const wchar_t* name, const wchar_t** theArgs, const wchar_t* defaultValue)
{
	BOOL result = FALSE;
	const wchar_t* pArgName, * pSeparator;
	SIZE_T argLen, nameLen = wcslen(name);
	int i;

	for (i = 0; i < argc; i++)
	{
		if (wcslen(argv[i]) && ((argv[i][0] == L'/') || (argv[i][0] == L'-')))
		{
			pArgName = argv[i] + 1;
			pSeparator = wcschr(argv[i], L':');

			if (!pSeparator)
			{
				pSeparator = wcschr(argv[i], L'=');
			}

			if (pSeparator)
			{
				argLen = pSeparator - pArgName;
			}
			else
			{
				argLen = wcslen(pArgName);
			}

			if ((argLen == nameLen) && !_wcsnicmp(name, pArgName, argLen))
			{
				if (theArgs)
				{
					if (pSeparator)
					{
						*theArgs = pSeparator + 1;
						result = *theArgs[0] != L'\0';
					}
				}
				else
				{
					result = TRUE;
				}
				break;
			}
		}
	}

	if (!result && theArgs)
	{
		if (defaultValue)
		{
			*theArgs = defaultValue;
			result = TRUE;
		}
		else
		{
			*theArgs = NULL;
		}
	}

	return result;
}

PCWCHAR KULL_M_CLI_KPRINTHEX_TYPES[] = {
	L"%02x",		// KPrintHexShort
	L"%02x ",		// KPrintHexSpace
	L"0x%02x, ",	// KPrintHexC
	L"\\x%02x",		// KPrintHexPython
	L"%02X",		// KPrintHexShortCap
};

void kull_m_cli_kprinthex(LPCVOID lpData, DWORD cbData, KPRINT_HEX_TYPES type, DWORD cbWidth, BOOL bWithNewLine)
{
	DWORD i;
	PCWCHAR pType = KULL_M_CLI_KPRINTHEX_TYPES[type];

	if (type == KPrintHexC)
	{
		kprintf(L"\nBYTE data[] = {\n\t");
	}

	for (i = 0; i < cbData; i++)
	{
		kprintf(pType, ((LPCBYTE)lpData)[i]);

		if (cbWidth && !((i + 1) % cbWidth))
		{
			kprintf(L"\n");
			if (type == KPrintHexC)
			{
				kprintf(L"\t");
			}
		}
	}

	if (type == KPrintHexC)
	{
		kprintf(L"\n};\n");
	}

	if (bWithNewLine)
	{
		kprintf(L"\n");
	}
}

void kull_m_cli_DisplayError(PCSTR SourceFunction, PCWSTR SourceError, DWORD dwErrorCode, BOOL bWithMessage, BOOL bWithNewLine)
{
	PWSTR szMessage = NULL;

	if (!dwErrorCode)
	{
		dwErrorCode = GetLastError();
	}

	kprintf(L"ERROR %S ; %s: 0x%08x", SourceFunction, SourceError, dwErrorCode);

	if (bWithMessage)
	{
		if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_MAX_WIDTH_MASK, NULL, dwErrorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&szMessage, 0, NULL))
		{
			if (szMessage)
			{
				kprintf(L" - %s", szMessage);
				LocalFree(szMessage);
			}
		}
		//else if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_MAX_WIDTH_MASK | FORMAT_MESSAGE_FROM_HMODULE, hNTDLLModule, dwErrorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&szMessage, 0, NULL))
		//{
		//	if (szMessage)
		//	{
		//		kprintf(L" - NT - %s", szMessage);
		//		LocalFree(szMessage);
		//	}
		//}
	}

	if (bWithNewLine)
	{
		kprintf(L"\n");
	}
}

BOOL kull_m_file_writeGeneric(PCWSTR szFileName, LPCVOID pbData, DWORD cbData, DWORD dwFlags)
{
	BOOL status = FALSE;
	DWORD dwBytesWritten;
	HANDLE hFile = NULL;

	hFile = CreateFile(szFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, dwFlags, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		if (WriteFile(hFile, pbData, cbData, &dwBytesWritten, NULL))
		{
			if (FlushFileBuffers(hFile))
			{
				status = TRUE;
			}
			else PRINT_ERROR_AUTO(L"FlushFileBuffers");
		}
		else PRINT_ERROR_AUTO(L"WriteFile");

		CloseHandle(hFile);
	}
	else PRINT_ERROR_AUTO(L"CreateFile");

	return status;
}

const PCWSTR FT_K_DEVICES_NAMES[] = {
	L"FT_DEVICE_BM", L"FT_DEVICE_AM", L"FT_DEVICE_100AX", L"FT_DEVICE_UNKNOWN", L"FT_DEVICE_2232C", L"FT_DEVICE_232R", L"FT_DEVICE_2232H", L"FT_DEVICE_4232H",
	L"FT_DEVICE_232H", L"FT_DEVICE_X_SERIES", L"FT_DEVICE_4222H_0", L"FT_DEVICE_4222H_1_2", L"FT_DEVICE_4222H_3", L"FT_DEVICE_4222_PROG", L"FT_DEVICE_900", L"FT_DEVICE_930",
	L"FT_DEVICE_UMFTPD3A", L"FT_DEVICE_2233HP", L"FT_DEVICE_4233HP", L"FT_DEVICE_2232HP", L"FT_DEVICE_4232HP", L"FT_DEVICE_233HP", L"FT_DEVICE_232HP", L"FT_DEVICE_2232HA",
	L"FT_DEVICE_4232HA",
};

FT_STATUS FT_K_DescribeChannel(DWORD index)
{
	FT_STATUS status;
	FT_DEVICE_LIST_INFO_NODE chanInfo;

	status = SPI_GetChannelInfo(index, &chanInfo);
	if (FT_SUCCESS(status))
	{
		kprintf(L"Device # %lu\n"
			L"\xc3 LocId       : 0x%08lx\n"
			L"\xc3 Flags       : 0x%08lx",
			index, chanInfo.LocId, chanInfo.Flags
		);
		if (chanInfo.Flags & FT_FLAGS_OPENED)
		{
			kprintf(L" - OPENED");
		}
		if (chanInfo.Flags & FT_FLAGS_HISPEED)
		{
			kprintf(L" - HISPEED");
		}
		kprintf(L"\n"
			L"\xc3 Type        : 0x%08lx - %s\n"
			L"\xc3 ID          : 0x%08lx (VID 0x%04x - PID 0x%04x)\n"
			L"\xc3 SerialNumber: %.16S\n"
			L"\xc0 Description : %.64S\n\n",
			chanInfo.Type, (chanInfo.Type <= ARRAYSIZE(FT_K_DEVICES_NAMES)) ? FT_K_DEVICES_NAMES[chanInfo.Type] : L"?",
			chanInfo.ID, (WORD)(chanInfo.ID >> 16), (WORD)chanInfo.ID,
			chanInfo.SerialNumber,
			chanInfo.Description
		);
	}
	else PRINT_FT_ERROR(L"SPI_GetChannelInfo", status);

	return status;
}

FT_STATUS FT_K_SelectChannel(int argc, wchar_t* argv[], FT_HANDLE* pHandle)
{
	FT_STATUS status;
	DWORD numChannels, i = 0;
	LPCWSTR device;

	if (GET_CLI_ARG(L"device", &device))
	{
		i = wcstoul(device, NULL, 0);
		kprintf(L"> Using explicit device number: %s (# %lu)\n", device, i);
		status = FT_K_DescribeChannel(i);
	}
	else
	{
		status = SPI_GetNumChannels(&numChannels);
		if (FT_SUCCESS(status))
		{
			if (!numChannels)
			{
				PRINT_ERROR(L"No channel available (?)\n");
				status = FT_DEVICE_NOT_FOUND;
			}
			else if (numChannels == 1)
			{
				kprintf(L"> Using implicit device number # 0\n");
				i = 0;
				status = FT_K_DescribeChannel(i);
			}
			else
			{
				kprintf(L"> Multiple devices are available, consider using /device:_number_ from this list:\n\n");
				for (i = 0; (i < numChannels) && FT_SUCCESS(status); i++)
				{
					status = FT_K_DescribeChannel(i);
				}
				i = 0;
				kprintf(L"> Device # 0 will be used by default\n");
			}
		}
		else PRINT_FT_ERROR(L"SPI_GetNumChannels", status);
	}

	if (FT_SUCCESS(status))
	{
		status = SPI_OpenChannel(i, pHandle);
		if (!FT_SUCCESS(status))
		{
			PRINT_FT_ERROR(L"SPI_OpenChannel", status);
		}
	}

	return status;
}