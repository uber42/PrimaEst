/**
 * @file Log.c
 *
 * @author Pavel Chursin
 * @date Oct 2020
 */

#include "../global.h"

#define LOG_EOT_CONSOLE					"console"
#define LOG_EOT_TEXTFILE				"text"
#define LOG_EOT_BINARYFILE				"binary"

#define LOG_ELP_DEBUG					"debug"
#define LOG_ELP_INFO					"info"
#define LOG_ELP_WARN					"warn"
#define LOG_ELP_ERROR					"error"
#define LOG_ELP_EXTRA					"extra"

#define LOG_CONFIGURATION_OUTPUT_TYPE	"OutputType"
#define LOG_CONFIGURATION_LOG_FILE_PATH "LogFilePath"
#define LOG_CONFIGURATION_LOW_PRIORITY	"LowPriority"
#define LOG_CONFIGURATION_HIGH_PRIORITY	"HighPriority"

#define LOG_DEFAULT_OUTPUT_TYPE			EOT_TEXTFILE
#define LOG_DEFAULT_LOW_PRIORITY		ELP_INFO
#define LOG_DEFAULT_HIGH_PRIORITY		ELP_ERROR

#define LOG_ELP_HASH_SEED				0xFFFFFF00

BOOL
ParseLoggerConfigurationInternal(
	PCHAR				pszBuffer,
	DWORD				dwBufferSize,
	PSLogConfiguration	psLogConfiguration
);

BOOL
StringToEOutputType(
	PEOutputType	eOutputType,
	PCHAR			pszOutputType
)
{
	size_t dwLength = strlen(pszOutputType);
	DWORD dwHash = Fnv1aCompute(
		pszOutputType,
		(DWORD)dwLength,
		sizeof(CHAR),
		FNV_1A_SEED);

	switch (dwHash)
	{
	case EOT_BINARYFILE:
	case EOT_TEXTFILE:
	case EOT_CONSOLE:
		*eOutputType = dwHash;
		return TRUE;
	default:
		return FALSE;
	}
}

BOOL
EOutputTypeToString(
	EOutputType		eOutputType,
	PCHAR			pszOutputType
)
{
	DWORD dwHash = (DWORD)eOutputType;
	switch (dwHash)
	{
	case EOT_BINARYFILE:
		memcpy(pszOutputType,
			LOG_EOT_BINARYFILE, sizeof(LOG_EOT_BINARYFILE));
		return TRUE;
	case EOT_TEXTFILE:
		memcpy(pszOutputType,
			LOG_EOT_TEXTFILE, sizeof(LOG_EOT_TEXTFILE));
		return TRUE;
	case EOT_CONSOLE:
		memcpy(pszOutputType,
			LOG_EOT_CONSOLE, sizeof(LOG_EOT_CONSOLE));
		return TRUE;
	default:
		return FALSE;
	}
}

BOOL
StringToELevelPriority(
	PELevelPriority	eLevelPriority,
	PCHAR			pszLevelPriority
)
{
	size_t dwLength = strlen(pszLevelPriority);
	DWORD dwHash = Fnv1aCompute(
		pszLevelPriority,
		(DWORD)dwLength,
		sizeof(CHAR),
		LOG_ELP_HASH_SEED);

	switch (dwHash)
	{
	case ELP_DEBUG:
	case ELP_INFO:
	case ELP_WARN:
	case ELP_ERROR:
	case ELP_EXTRA:
		*eLevelPriority = dwHash;
		return TRUE;
	default:
		return FALSE;
	}
}

BOOL
ELevelPriorityToString(
	ELevelPriority	eLevelPriority,
	PCHAR			pszLevelPriority
)
{
	DWORD dwHash = (DWORD)eLevelPriority;
	switch (dwHash)
	{
	case ELP_DEBUG:
		memcpy(pszLevelPriority,
			LOG_ELP_DEBUG, sizeof(LOG_ELP_DEBUG));
		return TRUE;
	case ELP_INFO:
		memcpy(pszLevelPriority,
			LOG_ELP_INFO, sizeof(LOG_ELP_INFO));
		return TRUE;
	case ELP_WARN:
		memcpy(pszLevelPriority,
			LOG_ELP_WARN, sizeof(LOG_ELP_WARN));
		return TRUE;
	case ELP_ERROR:
		memcpy(pszLevelPriority,
			LOG_ELP_ERROR, sizeof(LOG_ELP_ERROR));
		return TRUE;
	case ELP_EXTRA:
		memcpy(pszLevelPriority,
			LOG_ELP_EXTRA, sizeof(LOG_ELP_EXTRA));
		return TRUE;
	default:
		return FALSE;
	}
}

BOOL
ParseLoggerConfiguration(
	PWCHAR				pwszConfigurationPath,
	PSLogConfiguration	psLogConfiguration
)
{
	HANDLE hFile = CreateFileW(
		pwszConfigurationPath,
		GENERIC_READ,
		0, NULL,
		OPEN_EXISTING,
		0, NULL
	);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	DWORD dwFileSizeLow;
	dwFileSizeLow = GetFileSize(hFile, NULL);
	if (dwFileSizeLow == INVALID_FILE_SIZE)
	{
		CloseHandle(hFile);
		return FALSE;
	}

	PCHAR pszBuffer = malloc(sizeof(CHAR) * (dwFileSizeLow + 1));
	if (pszBuffer == NULL)
	{
		CloseHandle(hFile);
		return FALSE;
	}

	DWORD dwBytesRead;
	BOOL bResult = ReadFile(
		hFile,
		pszBuffer,
		dwFileSizeLow,
		&dwBytesRead,
		NULL
	);
	if (bResult == FALSE ||
		dwBytesRead == 0 ||
		dwBytesRead != dwFileSizeLow)
	{
		CloseHandle(hFile);
		return FALSE;
	}
	CloseHandle(hFile);

	return ParseLoggerConfigurationInternal(
		pszBuffer,
		dwFileSizeLow,
		psLogConfiguration
	);
}

PASYNC_LOGGER
InitializeAsyncLogger(
	SLogConfiguration sLogConfiguration
)
{
	return NULL;
}

BOOL
ParseLoggerConfigurationInternal(
	PCHAR				pszBuffer,
	DWORD				dwBufferSize,
	PSLogConfiguration	psLogConfiguration
)
{
	cJSON* pFound = NULL;
	cJSON* pRoot = cJSON_ParseWithLength(
		pszBuffer,
		dwBufferSize
	);

	if (pRoot == NULL)
	{
		return FALSE;
	}

	BOOL bScopeResult = TRUE;
	do
	{
		BOOL  bConvertResult;
		PCHAR pszItem;

		EOutputType eOutputType = LOG_DEFAULT_OUTPUT_TYPE;
		pFound = cJSON_GetObjectItem(pRoot, LOG_CONFIGURATION_OUTPUT_TYPE);
		if (pFound == NULL)
		{
			bScopeResult = FALSE;
			break;
		}
		pszItem = CharLower(pFound->valuestring);
		StringToEOutputType(&eOutputType, pszItem);
		psLogConfiguration->eOutputType = eOutputType;

		pFound = cJSON_GetObjectItem(pRoot, LOG_CONFIGURATION_LOG_FILE_PATH);
		if (pFound == NULL)
		{
			bScopeResult = FALSE;
			break;
		}
		size_t nLength = strlen(pFound->valuestring);
		if (nLength > sizeof(psLogConfiguration->szLogFilePath))
		{
			bScopeResult = FALSE;
			break;
		}
		memset(psLogConfiguration->szLogFilePath,
			0, sizeof(psLogConfiguration->szLogFilePath));
		memcpy(psLogConfiguration->szLogFilePath,
			pFound->valuestring, nLength);

		ELevelPriority eLowLevelPriority = LOG_DEFAULT_LOW_PRIORITY;
		pFound = cJSON_GetObjectItem(pRoot, LOG_CONFIGURATION_LOW_PRIORITY);
		if (pFound == NULL)
		{
			bScopeResult = FALSE;
			break;
		}
		pszItem = CharLower(pFound->valuestring);
		StringToELevelPriority(&eLowLevelPriority, pszItem);
		psLogConfiguration->eLowerLevelPriority = eLowLevelPriority;

		ELevelPriority eHighLevelPriority = LOG_DEFAULT_HIGH_PRIORITY;
		pFound = cJSON_GetObjectItem(pRoot, LOG_CONFIGURATION_HIGH_PRIORITY);
		if (pFound == NULL)
		{
			bScopeResult = FALSE;
			break;
		}
		pszItem = CharLower(pFound->valuestring);
		StringToELevelPriority(&eHighLevelPriority, pszItem);
		psLogConfiguration->eUpperLevelPriority = eHighLevelPriority;
	} while (FALSE);

	cJSON_Delete(pRoot);
	return bScopeResult;
}