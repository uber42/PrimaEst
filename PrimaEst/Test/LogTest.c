#include "../global.h"

#define LOG_EOT_CONSOLE					"console"
#define LOG_EOT_TEXTFILE				"text"
#define LOG_EOT_BINARYFILE				"binary"

#define LOG_ELP_DEBUG					"debug"
#define LOG_ELP_INFO					"info"
#define LOG_ELP_WARN					"warn"
#define LOG_ELP_ERROR					"error"
#define LOG_ELP_EXTRA					"extra"

#define LOG_ELP_HASH_SEED				0xFFFFFF00

static 
DWORD 
QuickFnvExecute(
	PCHAR		pszBuffer,
	DWORD		dwSeed
)
{
	size_t dwLength = strlen(pszBuffer);
	return Fnv1aCompute(
		pszBuffer,
		(DWORD)dwLength,
		sizeof(CHAR),
		0xFFFFFF00);
}

VOID
LogEnumFnvTest()
{
	DWORD dwEotConsole = QuickFnvExecute(LOG_EOT_CONSOLE, FNV_1A_SEED);
	DWORD dwEotTextFile = QuickFnvExecute(LOG_EOT_TEXTFILE, FNV_1A_SEED);
	DWORD dwEotBinaryFile = QuickFnvExecute(LOG_EOT_BINARYFILE, FNV_1A_SEED);

	assert(dwEotConsole == (DWORD)EOT_CONSOLE);
	assert(dwEotTextFile == (DWORD)EOT_TEXTFILE);
	assert(dwEotBinaryFile == (DWORD)EOT_BINARYFILE);

	DWORD dwElpDebug = QuickFnvExecute(LOG_ELP_DEBUG, LOG_ELP_HASH_SEED);
	DWORD dwElpInfo = QuickFnvExecute(LOG_ELP_INFO, LOG_ELP_HASH_SEED);
	DWORD dwElpWarn = QuickFnvExecute(LOG_ELP_WARN, LOG_ELP_HASH_SEED);
	DWORD dwElpError = QuickFnvExecute(LOG_ELP_ERROR, LOG_ELP_HASH_SEED);
	DWORD dwElpCritical = QuickFnvExecute(LOG_ELP_EXTRA, LOG_ELP_HASH_SEED);

	DWORD dwArr[5] = { dwElpDebug , dwElpInfo, dwElpWarn, dwElpError, dwElpCritical };
	for (size_t i = 1; i < ARRAYSIZE(dwArr); i++)
	{
		assert(dwArr[i - 1] < dwArr[i]);
	}

	assert(dwElpDebug == (DWORD)ELP_DEBUG);
	assert(dwElpInfo == (DWORD)ELP_INFO);
	assert(dwElpWarn == (DWORD)ELP_WARN);
	assert(dwElpError == (DWORD)ELP_ERROR);
	assert(dwElpCritical == (DWORD)ELP_EXTRA);
}

VOID
ParseLogTest_1()
{
	SLogConfiguration sLogConfiguration;

	BOOL bResult = ParseLoggerConfiguration(
		L"../PrimaEst/Test/Resources/TestLogConfigurationOk.json",
		&sLogConfiguration);
	
	assert(bResult == TRUE);
	assert(sLogConfiguration.eLowerLevelPriority == ELP_DEBUG);
	assert(sLogConfiguration.eUpperLevelPriority == ELP_EXTRA);
	assert(strcmp(sLogConfiguration.szLogFilePath, "./Log.txt") == 0);
	assert(sLogConfiguration.eOutputType == EOT_TEXTFILE);
}

VOID
ParseLogTest_2()
{
	SLogConfiguration sLogConfiguration;

	BOOL bResult = ParseLoggerConfiguration(
		L"../PrimaEst/Test/Resources/TestLogConfigurationBad.json",
		&sLogConfiguration);

	assert(bResult == FALSE);
}