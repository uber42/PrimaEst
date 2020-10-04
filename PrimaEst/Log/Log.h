/**
 * @file Log.h
 *
 * @author Pavel Chursin
 * @date Oct 2020
 */

#ifndef LOG_H
#define LOG_H

#define LOG_ENTRY_MAX_LENGTH	(1 << 11)
#define LOG_ENUM_MAX_LENGTH		(1 << 5)

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
 * Конвертировать строку EOutputType
 * @param[out]	eOutputType		Тип вывода
 * @param[in]	pszOutputType	Строковое значение типа
 * @return						Результат работы функции
 */
BOOL
StringToEOutputType(
	PEOutputType	eOutputType,
	PCHAR			pszOutputType
);

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
);

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
);

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
);

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
);

/**
 * Инициализировать логгер
 * @param[in]	psLogConfiguration			Структура конфигурации
 * @return									Результат работы функции
 */
BOOL
InitializeAsyncLogger(
	SLogConfiguration sLogConfiguration
);

/**
 * Деинициализировать логгер
 */
VOID
DeinitializeAsyncLogger();

/**
 * Записать отладочные данные
 * @param[in]	szFormat	Формат записи
 * @param[in]	...			Аргументы
 */
VOID
LogDebug(
	PCHAR szFormat,
	...
);

/**
 * Записать информацию о процессе выполнения программы
 * @param[in]	szFormat	Формат записи
 * @param[in]	...			Аргументы
 */
VOID
LogInfo(
	PCHAR szFormat,
	...
);

/**
 * Записать предупреждение
 * @param[in]	szFormat	Формат записи
 * @param[in]	...			Аргументы
 */
VOID
LogWarning(
	PCHAR szFormat,
	...
);

/**
 * Записать ошибку
 * @param[in]	szFormat	Формат записи
 * @param[in]	...			Аргументы
 */
VOID
LogError(
	PCHAR szFormat,
	...
);

/**
 * Записать критическую ошибку
 * @param[in]	szFormat	Формат записи
 * @param[in]	...			Аргументы
 */
VOID
LogExtra(
	PCHAR szFormat,
	...
);

/**
 * Приостановить вывод логов
 */
BOOL
SuspendAsyncLogger();

/**
 * Возобновить/Начать вывод логов
 */
BOOL
ResumeAsyncLogger();

#endif // LOG_H