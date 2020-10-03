/**
 * @file MessageQueue.c
 *
 * @author Pavel Chursin
 * @date Oct 2020
 */

#include "../global.h"

#define MAX_BUFFER_SZIE 1 << 28

/**
 * Структура очередь сообщений
 **/
typedef struct _SMessageQueue
{
	/** Критическая секция для операций с очередью */
	CRITICAL_SECTION crQueueLock;

	/** Событие оповещающее о поступлении сообщения в очередь */
	HANDLE			 hPushEvent;

	/** Указатель на список */
	SList			 sQueueMessages;

	/** Состояния (Работает/Не работает) */
	BOOL			 bIsWork;

	/** Максимальное число сообщений в очереди */
	DWORD			 dwMessagesMaxCount;

	/** Количество сообщений в очереди */
	DWORD			 dwMessagesCount;
} SMessageQueue, * PSMessageQueue;

/**
 * Контекст сообщения
 **/
typedef struct _SMessageContext
{
	/** Буфер */
	PVOID			 pBuffer;

	/** Размер буфера */
	DWORD			 dwBufferSize;

	/** Указатель на список */
	SList			 sEntry;
} SMessageContext, * PSMessageContext;

/**
* Создать очередь сообщений
* @param[out]   ppsMessageQueue		Экземпляр очереди сообщений.
* @param[in]	dwMessagesMaxCount	Максимальное количество сообщений в очереди.
* @param[in]	dwMessageBufferSize	Размер передаваемого сообщения.
* @param[out]	dwError				Ошибка
* @return Результат работы функции.
*/
BOOL
InitializeMessageQueue(
	MESSAGE_QUEUE* hMessageQueue,
	DWORD		  dwMessagesMaxCount,
	DWORD		  dwMessageBufferSize,
	PDWORD		  dwError
)
{
	if (hMessageQueue == NULL)
	{
		*dwError = MESSAGE_QUEUE_BAD_HANDLER;
		return FALSE;
	}

	if (dwMessagesMaxCount * (dwMessageBufferSize + sizeof(SMessageContext)) > MAX_BUFFER_SZIE)
	{
		*dwError = MESSAGE_QUEUE_BUFFER_NOT_ALLOW;
		return FALSE;
	}

	PSMessageQueue psMessageQueue = malloc(sizeof(SMessageQueue));
	if (psMessageQueue == NULL)
	{
		*dwError = MESSAGE_QUEUE_ALLOC_ERROR;
		return FALSE;
	}

	HANDLE hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (hEvent == NULL)
	{
		free(psMessageQueue);

		*dwError = MESSAGE_QUEUE_EVENT_ERROR;
		return FALSE;
	}

	CRITICAL_SECTION pCriticalSection;
	InitializeCriticalSection(&pCriticalSection);

	psMessageQueue->hPushEvent = hEvent;
	psMessageQueue->crQueueLock = pCriticalSection;
	psMessageQueue->bIsWork = TRUE;
	psMessageQueue->dwMessagesMaxCount = dwMessagesMaxCount;
	psMessageQueue->dwMessagesCount = 0;

	ListHeadInit(&psMessageQueue->sQueueMessages);

	*hMessageQueue = psMessageQueue;

	*dwError = MESSAGE_QUEUE_STATUS_OK;
	return TRUE;
}

/**
* Удалить очередь сообщений
* @param[out]   psMessageQueue		Экземпляр очереди сообщений.
* @return Результат работы функции.
*/
VOID
DeinitializeMessageQueue(
	MESSAGE_QUEUE hMessageQueue
)
{
	PSMessageQueue psMessageQueue = (PSMessageQueue)hMessageQueue;

	EnterCriticalSection(&psMessageQueue->crQueueLock);

	psMessageQueue->bIsWork = FALSE;
	SetEvent(psMessageQueue->hPushEvent);

	LeaveCriticalSection(&psMessageQueue->crQueueLock);

	MessageQueueClear(hMessageQueue);

	DeleteCriticalSection(&psMessageQueue->crQueueLock);
	CloseHandle(psMessageQueue->hPushEvent);

	free(psMessageQueue);
}

/**
* Добавить сообщение в очередь
* @param[out]   psMessageQueue		Экземпляр очереди сообщений.
* @param[out]   sMessageData		Сообщение.
* @return Результат работы функции.
*/
BOOL
MessageQueuePush(
	MESSAGE_QUEUE hMessageQueue,
	SMessageData  sMessageData,
	PDWORD		  dwError
)
{
	PSMessageQueue psMessageQueue = (PSMessageQueue)hMessageQueue;

	PSMessageContext psMessageContext = malloc(sizeof(SMessageContext));
	if (psMessageContext == NULL)
	{
		*dwError = MESSAGE_QUEUE_ALLOC_ERROR;
		return FALSE;
	}

	psMessageContext->pBuffer = sMessageData.pBuffer;
	psMessageContext->dwBufferSize = sMessageData.dwBufferSize;

	EnterCriticalSection(&psMessageQueue->crQueueLock);

	if (psMessageQueue->dwMessagesCount >= psMessageQueue->dwMessagesMaxCount)
	{
		free(psMessageContext);

		LeaveCriticalSection(&psMessageQueue->crQueueLock);

		*dwError = MESSAGE_QUEUE_BUFFER_OVERFLOW;
		return FALSE;
	}

	ListAddToEnd(&psMessageContext->sEntry, &psMessageQueue->sQueueMessages);
	psMessageQueue->dwMessagesCount++;

	LeaveCriticalSection(&psMessageQueue->crQueueLock);

	SetEvent(psMessageQueue->hPushEvent);

	*dwError = MESSAGE_QUEUE_STATUS_OK;
	return TRUE;
}

/**
* Вытащить из очереди первый элемент
* @param[out]   psMessageQueue		Экземпляр очереди сообщений.
* @return Элемент
*/
BOOL
MessageQueuePop(
	MESSAGE_QUEUE hMessageQueue,
	PSMessageData psMessageData,
	PDWORD		  dwError
)
{
	PSMessageQueue psMessageQueue = (PSMessageQueue)hMessageQueue;

	EnterCriticalSection(&psMessageQueue->crQueueLock);
	while (psMessageQueue->bIsWork && ListIsEmpty(&psMessageQueue->sQueueMessages))
	{
		LeaveCriticalSection(&psMessageQueue->crQueueLock);

		DWORD dwWaitRes = WaitForSingleObject(psMessageQueue->hPushEvent, INFINITE);

		if (dwWaitRes != WAIT_OBJECT_0)
		{
			*dwError = MESSAGE_QUEUE_EVENT_ERROR;
			return FALSE;
		}

		if (!psMessageQueue->bIsWork)
		{
			SetEvent(psMessageQueue->hPushEvent);

			*dwError = MESSAGE_QUEUE_CLOSED;
			return FALSE;
		}

		EnterCriticalSection(&psMessageQueue->crQueueLock);
	}

	psMessageQueue->dwMessagesCount--;
	PSList psFrontEntry = psMessageQueue->sQueueMessages.pFlink;
	ListNodeDelete(psFrontEntry);

	PSMessageContext psEntry = CONTAINING_RECORD(
		psFrontEntry,
		SMessageContext,
		sEntry);

	LeaveCriticalSection(&psMessageQueue->crQueueLock);

	psMessageData->dwBufferSize = psEntry->dwBufferSize;
	psMessageData->pBuffer = psEntry->pBuffer;

	free(psEntry);

	*dwError = MESSAGE_QUEUE_STATUS_OK;
	return TRUE;
}

/**
* Очистить очередь сообщений
* @param[out]   psMessageQueue		Экземпляр очереди сообщений.
*/
VOID
MessageQueueClear(
	MESSAGE_QUEUE hMessageQueue
)
{
	PSMessageQueue psMessageQueue = (PSMessageQueue)hMessageQueue;

	EnterCriticalSection(&psMessageQueue->crQueueLock);

	PSList psCurrentEntry = &psMessageQueue->sQueueMessages;
	while (!ListIsEmpty(psCurrentEntry))
	{
		PSList psDeletedEntry = LIST_FIRST(&psMessageQueue->sQueueMessages);
		ListNodeDelete(psDeletedEntry);

		PSMessageContext psEntry = CONTAINING_RECORD(
			psDeletedEntry,
			SMessageContext,
			sEntry);

		free(psEntry);
	}

	psMessageQueue->dwMessagesCount = 0;

	LeaveCriticalSection(&psMessageQueue->crQueueLock);
}