/**
 * @file Timer.c
 *
 * @author Pavel Chursin
 * @date Nov 2020
 */

#include "../global.h"

typedef struct _SApcProcArgument
{
	/** Канал для сигнала */
	MESSAGE_CHANNEL	hMessageChannel;

	/** ИД сообщения передаваемого в канал */
	DWORD			dwSignalId;
} SApcProcArgument, * PSApcProcArgument;

typedef struct _STimer
{
	/** Поток таймера */
	HANDLE			hThread;

	/** Канал передачи таймаута */
	MESSAGE_CHANNEL hTimeoutChannel;

	/** Дескриптор таймера */
	HANDLE			hWaitableTimer;

	/** Аргумент APC процедуры таймера */
	SApcProcArgument sData;
	
} STimer, *PSTimer;


/**
* Заглушка для прерывания работы таймера
*/
static
VOID
WINAPI
TimerAPCProcStub(
	LPVOID	lpRoutineParameter,
	DWORD	dwLogPartTime,
	DWORD	dwHighPartTime
)
{
	UNREFERENCED_PARAMETER(dwLogPartTime);
	UNREFERENCED_PARAMETER(dwHighPartTime);
	UNREFERENCED_PARAMETER(lpRoutineParameter);

	return;
}

/**
* APC Вызов срабатывающий по истечении таймера 
*/
static
VOID
WINAPI
TimerAPCProc(
	LPVOID	lpRoutineParameter,
	DWORD	dwLogPartTime,
	DWORD	dwHighPartTime
)
{
	UNREFERENCED_PARAMETER(dwLogPartTime);
	UNREFERENCED_PARAMETER(dwHighPartTime);

	PSApcProcArgument arg = (PSApcProcArgument)lpRoutineParameter;

	BOOL bSingal = TRUE;

	SendMessageChannel(
		arg->hMessageChannel,
		arg->dwSignalId,
		&bSingal, sizeof(BOOL),
		EMCBA_STACK_ALLOCATED);
}


/**
* Обработчик потока таймера
*/
static
DWORD
WINAPI
TimerRoutine(
	LPVOID lpThreadParameter
)
{
	PSTimer psTimer = lpThreadParameter;

	while (TRUE)
	{
		DWORD dwSize = 0;
		DWORD dwTimeout = 0;
		DWORD dwMessageId = ReceiveMessageChannel(
			psTimer->hTimeoutChannel,
			&dwTimeout,
			&dwSize);
		if (dwMessageId == MESSAGE_CHANNEL_BAD_RESULT)
		{
			return 0;
		}

		__int64			qwTime = ((__int64)dwTimeout) * (-10000);
		LARGE_INTEGER	liTime;

		liTime.LowPart = (DWORD)(qwTime & 0xFFFFFFFF);
		liTime.HighPart = (LONG)(qwTime >> 32);

		__try
		{
			BOOL bResult = SetWaitableTimer(
				psTimer->hWaitableTimer,
				&liTime, 0, TimerAPCProc,
				&psTimer->sData, FALSE);
			if (!bResult)
			{
				return 0;
			}
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			return 0;
		}
		
		DWORD dwResult = SleepEx(dwTimeout, TRUE);
		if (dwResult != WAIT_IO_COMPLETION)
		{
			return 0;
		}
	}
}



/**
* Создать таймер
* @param[out]   hTimer			Экземпляр таймера.
* @param[in]	hMessageChannel	Канал для подачи сигнала истечения таймера.
* @param[in]	dwSignalId		ИД для передачи в канал.
* @return Результат работы функции.
*/
BOOL
CreateChannelTimer(
	CHANNEL_TIMER	*hTimer,
	MESSAGE_CHANNEL	hMessageChannel,
	DWORD			dwSignalId
)
{
	PSTimer psTimer = malloc(sizeof(STimer));
	if (!psTimer)
	{
		return FALSE;
	}

	BOOL bResult = CreateMessageChannel(&psTimer->hTimeoutChannel);
	if (!bResult)
	{
		free(psTimer);

		return FALSE;
	}

	psTimer->sData.hMessageChannel = hMessageChannel;
	psTimer->sData.dwSignalId = dwSignalId;
	psTimer->hWaitableTimer = CreateWaitableTimer(
		NULL, FALSE, NULL);
	if (!psTimer->hWaitableTimer)
	{
		CloseMessageChannel(psTimer->hTimeoutChannel);
		free(psTimer);

		return FALSE;
	}

	psTimer->hThread = CreateThread(
		NULL, 0,
		TimerRoutine,
		psTimer, 0,
		NULL);
	if (!psTimer->hThread)
	{
		CloseHandle(psTimer->hWaitableTimer);
		CloseMessageChannel(psTimer->hTimeoutChannel);
		free(psTimer);

		return FALSE;
	}

	*hTimer = psTimer;

	return TRUE;
}


/**
* Установить таймер
* @param[out]   hTimer			Экземпляр таймера.
* @param[in]	dwTimeout		Время срабатывания.
* @return Результат работы функции.
*/
BOOL
SetChannelTimer(
	CHANNEL_TIMER	hTimer,
	DWORD			dwTimeout
)
{
	PSTimer psTimer = hTimer;

	BOOL bResult = SendMessageChannel(
		psTimer->hTimeoutChannel,
		0, &dwTimeout, sizeof(dwTimeout),
		EMCBA_STACK_ALLOCATED
	);

	return bResult;
}


/**
* Сбросить таймер
* @param[out]   hTimer			Экземпляр таймера.
*/
VOID
ResetChannelTimer(
	CHANNEL_TIMER	hTimer
)
{
	PSTimer psTimer = hTimer;
	CancelWaitableTimer(psTimer->hWaitableTimer);
	QueueUserAPC(TimerAPCProcStub, psTimer->hThread, NULL);
}

/**
* Удалить таймер
* @param[out]   hTimer			Экземпляр таймера.
*/
VOID
DeleteChannelTimer(
	CHANNEL_TIMER	hTimer
)
{
	PSTimer psTimer = hTimer;

	SendMessageChannel(
		psTimer->hTimeoutChannel,
		MESSAGE_CHANNEL_BAD_RESULT,
		NULL, sizeof(PVOID),
		EMCBA_HEAP_ALLOCATED
	);

	WaitForSingleObject(psTimer->hThread, INFINITE);
	CloseHandle(psTimer->hThread);

	CloseMessageChannel(psTimer->hTimeoutChannel);

	CancelWaitableTimer(psTimer->hWaitableTimer);
	CloseHandle(psTimer->hWaitableTimer);

	free(psTimer);
}