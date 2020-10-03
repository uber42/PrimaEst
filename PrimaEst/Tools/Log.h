/**
 * @file Log.h
 *
 * @author Pavel Chursin
 * @date Oct 2020
 */

#ifndef LOG_H
#define LOG_H

#define LOG_ENTRY_MAX_LENGTH	1 << 11
#define LOG_ENUM_MAX_LENGTH		1 << 5

typedef PVOID ASYNC_LOGGER, PASYNC_LOGGER;

/**
 * Перечисление видов вывода лога
 * Значения приведены в FNV-1A для быстрого определения
 */
typedef enum _EOutputType
{
	/** Вывод в консоль */
	EOT_CONSOLE = 0xdf47bdd9,

	/** Вывод в текстовый файл */
	EOT_TEXTFILE = 0x44736337,

	/** Вывод в бинайрный файл */
	EOT_BINARYFILE = 0xd7f85465
} EOutputType, * PEOutputType;

/**
 * Перечисление приоритетов вывода
 * Значения приведены в FNV-1A для быстрого определения
 */
typedef enum _ELevelPriority
{
	/** Отладочные данные */
	ELP_DEBUG = 0x043e49e7,

	/** Информация */
	ELP_INFO = 0x94e92a9c,

	/** Предупреждение */
	ELP_WARN = 0xc22283cc,

	/** Ошибка */
	ELP_ERROR = 0xd9d3a7c6,

	/** Критическая ошибка, за которой следует завершение программы */
	ELP_EXTRA = 0xdf823706
} ELevelPriority, * PELevelPriority;

/**
 * Конфигурация лога
 */
typedef struct _SLogConfiguration
{
	/** Тип вывода */
	EOutputType		eOutputType;

	/** Файл для вывода лога */
	CHAR			szLogFilePath[MAX_PATH];

	/** Нижний приоритет */
	ELevelPriority	eLowerLevelPriority;

	/** Верхний приоритет */
	ELevelPriority	eUpperLevelPriority;
} SLogConfiguration, * PSLogConfiguration;

/**
 * Данные записи лога
 */
typedef struct _SLogEntry
{
	/** Номер записи */
	DWORD			dwNumber;

	/** ИД процесса  */
	DWORD			dwProcessId;

	/** ИД нити */
	DWORD			dwThreadId;

	/** Время */
	SYSTEMTIME		sTime;

	/** Приоритет записи */
	ELevelPriority	eLevelPriority;

	/** Длина сообщения */
	DWORD			dwMessageLength;

	/** Сообщение */
	WCHAR			wszMessage[LOG_ENTRY_MAX_LENGTH];
} SLogEntry, * PSLogEntry;

BOOL
StringToEOutputType(
	PEOutputType	eOutputType,
	PCHAR			pszOutputType
);

BOOL
EOutputTypeToString(
	EOutputType		eOutputType,
	PCHAR			pszOutputType
);

BOOL
StringToELevelPriority(
	PELevelPriority	eOutputType,
	PCHAR			pszOutputType
);

BOOL
ELevelPriorityToString(
	ELevelPriority	eOutputType,
	PCHAR			pszOutputType
);

BOOL
ParseLoggerConfiguration(
	PWCHAR				pwszConfigurationPath,
	PSLogConfiguration	psLogConfiguration
);

PASYNC_LOGGER
InitializeAsyncLogger(
	SLogConfiguration sLogConfiguration
);

#endif // LOG_H