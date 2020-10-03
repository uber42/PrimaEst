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
 * ������������ ����� ������ ����
 * �������� ��������� � FNV-1A ��� �������� �����������
 */
typedef enum _EOutputType
{
	/** ����� � ������� */
	EOT_CONSOLE = 0xdf47bdd9,

	/** ����� � ��������� ���� */
	EOT_TEXTFILE = 0x44736337,

	/** ����� � ��������� ���� */
	EOT_BINARYFILE = 0xd7f85465
} EOutputType, * PEOutputType;

/**
 * ������������ ����������� ������
 * �������� ��������� � FNV-1A ��� �������� �����������
 */
typedef enum _ELevelPriority
{
	/** ���������� ������ */
	ELP_DEBUG = 0x043e49e7,

	/** ���������� */
	ELP_INFO = 0x94e92a9c,

	/** �������������� */
	ELP_WARN = 0xc22283cc,

	/** ������ */
	ELP_ERROR = 0xd9d3a7c6,

	/** ����������� ������, �� ������� ������� ���������� ��������� */
	ELP_EXTRA = 0xdf823706
} ELevelPriority, * PELevelPriority;

/**
 * ������������ ����
 */
typedef struct _SLogConfiguration
{
	/** ��� ������ */
	EOutputType		eOutputType;

	/** ���� ��� ������ ���� */
	CHAR			szLogFilePath[MAX_PATH];

	/** ������ ��������� */
	ELevelPriority	eLowerLevelPriority;

	/** ������� ��������� */
	ELevelPriority	eUpperLevelPriority;
} SLogConfiguration, * PSLogConfiguration;

/**
 * ������ ������ ����
 */
typedef struct _SLogEntry
{
	/** ����� ������ */
	DWORD			dwNumber;

	/** �� ��������  */
	DWORD			dwProcessId;

	/** �� ���� */
	DWORD			dwThreadId;

	/** ����� */
	SYSTEMTIME		sTime;

	/** ��������� ������ */
	ELevelPriority	eLevelPriority;

	/** ����� ��������� */
	DWORD			dwMessageLength;

	/** ��������� */
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