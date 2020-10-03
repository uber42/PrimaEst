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

#define LOG_MESSAGE_QUEUE_MAX			0x80

#define LOG_HEADER_SIZE					0x80

#define LOG_THREAD_TERMINATE_TIMEOUT	2000

 /**
  * Данные записи лога
  */
typedef struct _SLogEntry
{
	/** Номер записи */
	LONGLONG			llNumber;

	/** Время */
	SYSTEMTIME			sTime;

	/** Приоритет записи */
	ELevelPriority		eLevelPriority;

	/** Длина сообщения */
	DWORD				dwMessageLength;

	/** Сообщение */
	CHAR				szMessage[LOG_ENTRY_MAX_LENGTH];
} SLogEntry, * PSLogEntry;

 /**
  * Callback для записи лога
  */
typedef BOOL(*FAsyncLoggerWriter)(
	HANDLE		hFile,
	PSLogEntry	sLogEntry
);

/**
 * Состояния логера
 */
typedef enum _ELoggerState
{
	/** Не инициализирован */
	ELS_NOT_INITIALIZED = 0,

	/** Инициализирован */
	ELS_INITIALIZED,

	/** Работает */
	ELS_WORKS,

	/** Приостановлен */
	ELS_PAUSED
} ELoggerState, *PELoggerState;

/**
 * Структура асинхронного логгера
 */
typedef struct _SAsyncLogger
{
	/** Состояние логгера */
	ELoggerState		eLoggerState;

	/** Дескриптор потока пишущего лог */
	HANDLE				hThreadWriter;

	/** Дескпритор очереди сообщений */
	MESSAGE_QUEUE		hMessageQueue;

	/** Показатель работы логгера */
	BOOL				bIsWork;

	/** Конфигурация логгера */
	SLogConfiguration	sConfiguration;

	/** Дескриптор файла лога */
	HANDLE				hLogFile;

	/** Количество записей */
	volatile LONGLONG	llEntriesCount;

	/** Функция описывающая способ записи 
	 *  в зависимости от конфигурации EOutputType */
	FAsyncLoggerWriter	fWriter;
} SAsyncLogger, *PSAsyncLogger;


/** Глобальный экземпляр логгера */
static SAsyncLogger g_sAsyncLogger = { 0 };

/**
 * Форматировать запись лога
 * @param[out]	szBuffer	Выходной буфер
 * @param[in]	sLogEntry	Запись лога
 * @return					Количество форматированных байт
 */
static
INT
FormatLogEntry(
	CHAR		szBuffer[LOG_HEADER_SIZE + LOG_ENTRY_MAX_LENGTH],
	PSLogEntry	psLogEntry
)
{
	CHAR szLevelPriority[16];
	ELevelPriorityToString(
		psLogEntry->eLevelPriority,
		szLevelPriority
	);

	return sprintf(
		szBuffer,
		"| %I64u | %d/%d/%d %d:%d:%d | %s | %d |\n\r%s\n",
		psLogEntry->llNumber,
		psLogEntry->sTime.wDay,
		psLogEntry->sTime.wMonth,
		psLogEntry->sTime.wYear,
		psLogEntry->sTime.wHour,
		psLogEntry->sTime.wMinute,
		psLogEntry->sTime.wSecond,
		szLevelPriority,
		psLogEntry->dwMessageLength,
		psLogEntry->szMessage
	);
}


/**
 * Записать в консоль
 * @param[in]	hFile		Дескриптор файла
 * @param[in]	sLogEntry	Запись лога
 * @return					Результат работы функции
 */
static
BOOL
ALConsoleWriter(
	HANDLE		hFile,
	PSLogEntry	psLogEntry
)
{
	UNREFERENCED_PARAMETER(hFile);

	CHAR szBuffer[LOG_HEADER_SIZE + LOG_ENTRY_MAX_LENGTH];
	INT iSize = FormatLogEntry(szBuffer, psLogEntry);

	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	return WriteFile(
		hStdOut,
		szBuffer, iSize,
		NULL, NULL
	);
}


/**
 * Распарсить файл с конфигурацией лога
 * @param[in]	pszBuffer			Буфер с данными JSON файла
 * @param[in]	dwBufferSize		Размер буфера
 * @param[out]	psLogConfiguration	Структура конфигурации
 * @return							Результат работы функции
 */
BOOL
ParseLoggerConfigurationInternal(
	PCHAR				pszBuffer,
	DWORD				dwBufferSize,
	PSLogConfiguration	psLogConfiguration
);

/**
 * Записать в лог
 * @param[in]	szBuffer		Буфер с сообщением
 * @param[in]	dwMessageSize	Размер сообщения
 * @param[in]	eLevelPriority	Приоритет
 */
VOID
LogWriteInternal(
	CHAR			szMessage[LOG_ENTRY_MAX_LENGTH],
	DWORD			dwMessageSize,
	ELevelPriority	eLevelPriority
);

/**
 * Конвертировать строку EOutputType
 * @param[out]	eOutputType		Тип вывода
 * @param[in]	pszOutputType	Строковое значение типа
 * @return						Результат работы функции
 */
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
		FNV_1A_SEED
	);

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

/**
 * Конвертировать EOutputType в строку
 * @param[in]	eOutputType		Тип вывода
 * @param[out]	pszOutputType	Строковое значение типа
 * @return						Результат работы функции
 */
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
		strcpy(pszOutputType, LOG_EOT_BINARYFILE);
		return TRUE;
	case EOT_TEXTFILE:
		strcpy(pszOutputType, LOG_EOT_TEXTFILE);
		return TRUE;
	case EOT_CONSOLE:
		strcpy(pszOutputType, LOG_EOT_CONSOLE);
		return TRUE;
	default:
		return FALSE;
	}
}

/**
 * Конвертировать строку в ELevelPriority
 * @param[out]	eLevelPriority		Уровень приоритета
 * @param[in]	pszLevelPriority	Строковое значение типа
 * @return							Результат работы функции
 */
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

/**
 * Конвертировать ELevelPriority в строку
 * @param[in]	eLevelPriority		Уровень приоритета
 * @param[out]	pszLevelPriority	Строковое значение типа
 * @return							Результат работы функции
 */
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
		strcpy(pszLevelPriority, LOG_ELP_DEBUG);
		return TRUE;
	case ELP_INFO:
		strcpy(pszLevelPriority, LOG_ELP_INFO);
		return TRUE;
	case ELP_WARN:
		strcpy(pszLevelPriority, LOG_ELP_WARN);
		return TRUE;
	case ELP_ERROR:
		strcpy(pszLevelPriority, LOG_ELP_ERROR);
		return TRUE;
	case ELP_EXTRA:
		strcpy(pszLevelPriority, LOG_ELP_EXTRA);
		return TRUE;
	default:
		return FALSE;
	}
}

/**
 * Распарсить файл с конфигурацией лога
 * @param[in]	pwszConfigurationPath		Путь к файлу с конфигурацией
 * @param[out]	psLogConfiguration			Структура конфигурации
 * @return									Результат работы функции
 */
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

/**
 * Обработчик потока логгера
 * @param[in] lpThreadParameter Контекст потока
 */
static
PVOID
LoggerRoutine(
	LPVOID lpThreadParameter
)
{
	UNREFERENCED_PARAMETER(lpThreadParameter);

	while (MessageQueueGetCount(g_sAsyncLogger.hMessageQueue) != 0 ||
		g_sAsyncLogger.bIsWork)
	{
		DWORD dwError;
		SMessageData sMessageData = { 0 };
		BOOL bResult = MessageQueuePop(
			g_sAsyncLogger.hMessageQueue,
			&sMessageData, &dwError
		);

		if (bResult == FALSE)
		{
			if (dwError == MESSAGE_QUEUE_CLOSED)
			{
				return NULL;
			}
			continue;
		}

		PSLogEntry psLogEntry = (PSLogEntry)sMessageData.pBuffer;
		g_sAsyncLogger.fWriter(
			g_sAsyncLogger.hLogFile,
			psLogEntry
		);
		free(psLogEntry);
	}

	return NULL;
}

/**
 * Инициализировать логгер
 * @param[in]	psLogConfiguration			Структура конфигурации
 * @return									Результат работы функции
 */
BOOL
InitializeAsyncLogger(
	SLogConfiguration sLogConfiguration
)
{
	memcpy(
		&g_sAsyncLogger.sConfiguration,
		&sLogConfiguration,
		sizeof(SLogConfiguration));

	g_sAsyncLogger.hThreadWriter = CreateThread(
		NULL, 0,
		(LPTHREAD_START_ROUTINE)&LoggerRoutine,
		NULL, CREATE_SUSPENDED, NULL
	);
	if (g_sAsyncLogger.hThreadWriter == NULL)
	{
		return FALSE;
	}

	BOOL bResult = InitializeMessageQueue(
		&g_sAsyncLogger.hMessageQueue,
		LOG_MESSAGE_QUEUE_MAX,
		sizeof(SLogEntry),
		&(DWORD){ 0 });
	if (bResult == FALSE)
	{
		TerminateThread(g_sAsyncLogger.hThreadWriter, 0);
		CloseHandle(g_sAsyncLogger.hThreadWriter);

		return FALSE;
	}

	g_sAsyncLogger.hLogFile = CreateFile(
		sLogConfiguration.szLogFilePath,
		GENERIC_WRITE,
		FILE_SHARE_READ,
		NULL,
		CREATE_ALWAYS, 
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);
	if (g_sAsyncLogger.hLogFile == INVALID_HANDLE_VALUE)
	{
		TerminateThread(g_sAsyncLogger.hThreadWriter, 0);
		CloseHandle(g_sAsyncLogger.hThreadWriter);

		DeinitializeMessageQueue(g_sAsyncLogger.hMessageQueue);

		return FALSE;
	}

	g_sAsyncLogger.llEntriesCount = 0;
	g_sAsyncLogger.bIsWork = TRUE;
	g_sAsyncLogger.eLoggerState = ELS_INITIALIZED;

	switch (sLogConfiguration.eOutputType)
	{
	case EOT_BINARYFILE:
	case EOT_TEXTFILE:
	case EOT_CONSOLE:
		g_sAsyncLogger.fWriter = ALConsoleWriter;
		break;
	default:
		return FALSE;
	}
}

/**
 * Записать отладочные данные
 * @param[in]	szFormat	Формат записи
 * @param[in]	...			Аргументы
 */
VOID
LogDebug(
	PCHAR szFormat,
	...
)
{
	va_list args;
	va_start(args, szFormat);

	CHAR szBuffer[LOG_ENTRY_MAX_LENGTH];
	INT dwBytesWrite = vsprintf(szBuffer, szFormat, args);

	va_end(args);

	LogWriteInternal(
		szFormat,
		dwBytesWrite,
		ELP_DEBUG);
}

/**
 * Записать информацию о процессе выполнения программы
 * @param[in]	szFormat	Формат записи
 * @param[in]	...			Аргументы
 */
VOID
LogInfo(
	PCHAR szFormat,
	...
)
{
	va_list args;
	va_start(args, szFormat);

	CHAR szBuffer[LOG_ENTRY_MAX_LENGTH];
	INT dwBytesWrite = vsprintf(szBuffer, szFormat, args);

	va_end(args);

	LogWriteInternal(
		szBuffer,
		dwBytesWrite,
		ELP_INFO);
}

/**
 * Записать предупреждение
 * @param[in]	szFormat	Формат записи
 * @param[in]	...			Аргументы
 */
VOID
LogWarning(
	PCHAR szFormat,
	...
)
{
	va_list args;
	va_start(args, szFormat);

	CHAR szBuffer[LOG_ENTRY_MAX_LENGTH];
	INT dwBytesWrite = vsprintf(szBuffer, szFormat, args);

	va_end(args);

	LogWriteInternal(
		szFormat,
		dwBytesWrite,
		ELP_WARN);
}

/**
 * Записать ошибку
 * @param[in]	szFormat	Формат записи
 * @param[in]	...			Аргументы
 */
VOID
LogError(
	PCHAR szFormat,
	...
)
{
	va_list args;
	va_start(args, szFormat);

	CHAR szBuffer[LOG_ENTRY_MAX_LENGTH];
	INT dwBytesWrite = vsprintf(szBuffer, szFormat, args);

	va_end(args);

	LogWriteInternal(
		szFormat,
		dwBytesWrite,
		ELP_ERROR);
}

/**
 * Записать критическую ошибку
 * @param[in]	szFormat	Формат записи
 * @param[in]	...			Аргументы
 */
VOID
LogExtra(
	PCHAR szFormat,
	...
)
{
	va_list args;
	va_start(args, szFormat);

	CHAR szBuffer[LOG_ENTRY_MAX_LENGTH];
	INT dwBytesWrite = vsprintf(szBuffer, szFormat, args);

	va_end(args);

	LogWriteInternal(
		szFormat,
		dwBytesWrite,
		ELP_EXTRA);
}

/**
 * Записать в лог
 * @param[in]	szBuffer		Буфер с сообщением
 * @param[in]	dwMessageSize	Размер сообщения
 * @param[in]	eLevelPriority	Приоритет
 */
VOID
LogWriteInternal(
	CHAR			szMessage[LOG_ENTRY_MAX_LENGTH],
	DWORD			dwMessageSize,
	ELevelPriority	eLevelPriority
)
{
	if (!g_sAsyncLogger.bIsWork)
	{
		return;
	}

	if ((DWORD)g_sAsyncLogger.sConfiguration.eLowerLevelPriority > (DWORD)eLevelPriority ||
		(DWORD)g_sAsyncLogger.sConfiguration.eUpperLevelPriority < (DWORD)eLevelPriority)
	{
		return;
	}

	PSLogEntry psLogEntry = malloc(sizeof(SLogEntry));
	if (psLogEntry == NULL)
	{
		return;
	}

	GetLocalTime(&psLogEntry->sTime);
	strcpy(psLogEntry->szMessage, szMessage);
	psLogEntry->eLevelPriority = eLevelPriority;
	psLogEntry->dwMessageLength = dwMessageSize;
	psLogEntry->llNumber = InterlockedIncrement64(
		&g_sAsyncLogger.llEntriesCount);

	SMessageData sMessageData =
	{
		.pBuffer = psLogEntry,
		.dwBufferSize = sizeof(SLogEntry)
	};
	BOOL bResult = MessageQueuePush(
		g_sAsyncLogger.hMessageQueue,
		sMessageData, &(DWORD){0}
	);
	if (bResult == FALSE)
	{
		free(psLogEntry);
	}
}

/**
 * Распарсить файл с конфигурацией лога
 * @param[in]	pszBuffer			Буфер с данными JSON файла
 * @param[in]	dwBufferSize		Размер буфера
 * @param[out]	psLogConfiguration	Структура конфигурации
 * @return							Результат работы функции
 */
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

/**
 * Приостановить вывод логов
 */
BOOL
SuspendAsyncLogger()
{
	if (g_sAsyncLogger.eLoggerState == ELS_WORKS)
	{
		SuspendThread(g_sAsyncLogger.hThreadWriter);
		return TRUE;
	}
	return FALSE;
}

/**
 * Возобновить/Начать вывод логов
 */
BOOL
ResumeAsyncLogger()
{
	if (g_sAsyncLogger.eLoggerState == ELS_PAUSED ||
		g_sAsyncLogger.eLoggerState == ELS_INITIALIZED)
	{
		ResumeThread(g_sAsyncLogger.hThreadWriter);
		return TRUE;
	}
	return FALSE;
}

/**
 * Деинициализировать логгер
 */
VOID
DeinitializeAsyncLogger()
{
	if (g_sAsyncLogger.eLoggerState == ELS_NOT_INITIALIZED)
	{
		return;
	}

	g_sAsyncLogger.bIsWork = FALSE;
	
	DWORD dwWaitRes = WaitForSingleObject(
		g_sAsyncLogger.hThreadWriter,
		LOG_THREAD_TERMINATE_TIMEOUT);
	if (dwWaitRes == WAIT_TIMEOUT)
	{
		TerminateThread(g_sAsyncLogger.hThreadWriter, 0);
	}
	CloseHandle(g_sAsyncLogger.hThreadWriter);

	DeinitializeMessageQueue(g_sAsyncLogger.hMessageQueue);

	if (g_sAsyncLogger.sConfiguration.eOutputType != EOT_CONSOLE)
	{
		CloseHandle(g_sAsyncLogger.hLogFile);
	}
	
	g_sAsyncLogger.llEntriesCount = 0;
	g_sAsyncLogger.eLoggerState = ELS_NOT_INITIALIZED;
	memset(&g_sAsyncLogger.sConfiguration,
		0, sizeof(SLogConfiguration));
}