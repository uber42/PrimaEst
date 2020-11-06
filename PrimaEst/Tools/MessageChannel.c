/**
 * @file MessageChannel.c
 *
 * @author Pavel Chursin
 * @date Nov 2020
 */

#include "../global.h"

/**
* Структура канала сообщений
*/
typedef struct _SMessageChannel
{
	/** Признак работы */
	volatile BOOL		bWork;

	/** Событие отправки сообщения */
	HANDLE				hSendEvent;

	/** Событие принятия сообщения */
	HANDLE				hReceiveEvent;

	/** Событие окончания отправки сообщения */
	HANDLE				hNextSend;

	/** Структура хранящая данные текущего сообщения */
	struct 
	{
		/** Успешно ли передано */
		BOOL				bSuccess;

		/** ИД текущего сообщения */
		DWORD				dwMessageId;

		/** Место где выделено текущее сообщение */
		EMCBufferAllocated	eBufferAllocatedIn;

		/** Данные текущего сообщения */
		PVOID				pCurrentBuffer;

		/** Размер текущего сообщения */
		DWORD				dwCurrentBufferSize;
	} sCurrentData;
} SMessageChannel, *PSMessageChannel;

/**
* Создать канал сообщений
* @param[out]   hMessageChannel		Экземпляр канала сообщений.
* @return Результат работы функции.
*/
BOOL
CreateMessageChannel(
	MESSAGE_CHANNEL *hMessageChannel
)
{
	PSMessageChannel psMessageChannel = malloc(sizeof(SMessageChannel));
	if (!psMessageChannel)
	{
		return FALSE;
	}

	psMessageChannel->hReceiveEvent = CreateEvent(
		NULL, FALSE,
		FALSE, NULL);
	if (psMessageChannel->hReceiveEvent == NULL)
	{
		free(psMessageChannel);

		return FALSE;
	}

	psMessageChannel->hSendEvent = CreateEvent(
		NULL, FALSE,
		FALSE, NULL);
	if (psMessageChannel->hSendEvent == NULL)
	{
		CloseHandle(psMessageChannel->hReceiveEvent);
		free(psMessageChannel);

		return FALSE;
	}

	psMessageChannel->hNextSend = CreateEvent(
		NULL, FALSE,
		FALSE, NULL);
	if (psMessageChannel->hNextSend == NULL)
	{
		CloseHandle(psMessageChannel->hSendEvent);
		CloseHandle(psMessageChannel->hReceiveEvent);
		free(psMessageChannel);

		return FALSE;
	}

	memset(&psMessageChannel->sCurrentData,
		0, sizeof(psMessageChannel->sCurrentData));

	*hMessageChannel = psMessageChannel;
	psMessageChannel->bWork = TRUE;

	return TRUE;
}


/**
* Отправить сообщение в канал
* @param[in]   hMessageChannel		Экземпляр канала сообщений.
* @param[in]   dwMessageId			ИД сообщения.
* @param[in]   pBuffer				Буфер.
* @param[in]   dwBufferSize			Размер буфера
* @param[in]   eBufferAllocatedIn	Где выделен буфер (Стек или куча).
* @return Результат работы функции.
*/
BOOL
SendMessageChannel(
	MESSAGE_CHANNEL		hMessageChannel,
	DWORD				dwMessageId,
	PVOID				pBuffer,
	DWORD				dwBufferSize,
	EMCBufferAllocated	eBufferAllocatedIn
)
{
	PSMessageChannel psMessageChannel = (PSMessageChannel)hMessageChannel;
	DWORD dwWaitRes = WaitForSingleObject(psMessageChannel->hReceiveEvent, INFINITE);
	if (!psMessageChannel->bWork || dwWaitRes != WAIT_OBJECT_0)
	{
		return FALSE;
	}

	psMessageChannel->sCurrentData.dwCurrentBufferSize = dwBufferSize;
	psMessageChannel->sCurrentData.dwMessageId = dwMessageId;
	psMessageChannel->sCurrentData.eBufferAllocatedIn = eBufferAllocatedIn;
	psMessageChannel->sCurrentData.pCurrentBuffer = pBuffer;
	psMessageChannel->sCurrentData.bSuccess = FALSE;

	BOOL bResult = SetEvent(psMessageChannel->hSendEvent);
	if (!bResult)
	{
		return FALSE;
	}

	dwWaitRes = WaitForSingleObject(psMessageChannel->hNextSend, INFINITE);
	if (!psMessageChannel->bWork || dwWaitRes != WAIT_OBJECT_0)
	{
		return FALSE;
	}

	return psMessageChannel->sCurrentData.bSuccess;
}


/**
* Принять сообщение из канала
* @param[in]   hMessageChannel		Экземпляр канала сообщений.
* @param[in]   pBuffer				Буфер.
* @param[in]   dwBufferSize			Размер буфера.
* @return ИД сообщения
*/
DWORD
ReceiveMessageChannel(
	MESSAGE_CHANNEL hMessageChannel,
	PVOID			pBuffer,
	PDWORD			pdwBufferSize
)
{
	PSMessageChannel psMessageChannel = (PSMessageChannel)hMessageChannel;

	BOOL bResult = SetEvent(psMessageChannel->hReceiveEvent);
	if (!bResult)
	{
		SetEvent(psMessageChannel->hNextSend);
		return MESSAGE_CHANNEL_BAD_RESULT;
	}

	DWORD dwWaitRes = WaitForSingleObject(psMessageChannel->hSendEvent, INFINITE);
	if (!psMessageChannel->bWork || dwWaitRes != WAIT_OBJECT_0)
	{
		SetEvent(psMessageChannel->hNextSend);
		return MESSAGE_CHANNEL_BAD_RESULT;
	}

	switch (psMessageChannel->sCurrentData.eBufferAllocatedIn)
	{
	case EMCBA_HEAP_ALLOCATED:
		pBuffer = psMessageChannel->sCurrentData.pCurrentBuffer;
		break;
	case EMCBA_STACK_ALLOCATED:
		memcpy(pBuffer, psMessageChannel->sCurrentData.pCurrentBuffer, 
			psMessageChannel->sCurrentData.dwCurrentBufferSize);
		break;
	default:
		SetEvent(psMessageChannel->hNextSend);
		return MESSAGE_CHANNEL_BAD_RESULT;
	}

	*pdwBufferSize = psMessageChannel->sCurrentData.dwCurrentBufferSize;
	DWORD dwMessageId = psMessageChannel->sCurrentData.dwMessageId;

	psMessageChannel->sCurrentData.dwCurrentBufferSize = MESSAGE_CHANNEL_BAD_RESULT;
	psMessageChannel->sCurrentData.bSuccess = TRUE;

	bResult = SetEvent(psMessageChannel->hNextSend);
	if (!bResult)
	{
		return MESSAGE_CHANNEL_BAD_RESULT;
	}

	return dwMessageId;
}


/**
* Получить размер текущего сообщения в канале
* @param[in]   hMessageChannel		Экземпляр канала сообщений.
* @return Размер
*/
DWORD
MCGetCurrentBufferSize(
	MESSAGE_CHANNEL hMessageChannel
)
{
	PSMessageChannel psMessageChannel = (PSMessageChannel)hMessageChannel;
	return psMessageChannel->sCurrentData.dwCurrentBufferSize;
}


/**
* Закрыть канал сообщений
* @param[in]   hMessageChannel		Экземпляр канала сообщений.
*/
VOID
CloseMessageChannel(
	MESSAGE_CHANNEL hMessageChannel
)
{
	PSMessageChannel psMessageChannel = (PSMessageChannel)hMessageChannel;

	psMessageChannel->bWork = FALSE;

	SetEvent(psMessageChannel->hReceiveEvent);
	SetEvent(psMessageChannel->hSendEvent);
	SetEvent(psMessageChannel->hNextSend);

	CloseHandle(psMessageChannel->hReceiveEvent);
	CloseHandle(psMessageChannel->hSendEvent);
	CloseHandle(psMessageChannel->hNextSend);

	free(psMessageChannel);
}