/**
 * @file MessageQueue.h
 *
 * @author Pavel Chursin
 * @date Oct 2020
 */

#ifndef MESSAGE_QUEUE_H
#define MESSAGE_QUEUE_H

#include "List.h"

#define MESSAGE_QUEUE_STATUS_OK					0xFFFF0000
#define MESSAGE_QUEUE_BUFFER_OVERFLOW			(MESSAGE_QUEUE_STATUS_OK + 1)
#define MESSAGE_QUEUE_EVENT_ERROR				(MESSAGE_QUEUE_STATUS_OK + 2)
#define MESSAGE_QUEUE_ALLOC_ERROR				(MESSAGE_QUEUE_STATUS_OK + 3)
#define MESSAGE_QUEUE_CLOSED					(MESSAGE_QUEUE_STATUS_OK + 4)
#define MESSAGE_QUEUE_BUFFER_NOT_ALLOW			(MESSAGE_QUEUE_STATUS_OK + 5)
#define MESSAGE_QUEUE_BAD_HANDLER				(MESSAGE_QUEUE_STATUS_OK + 6)

 /** Описатель очереди сообщений */
typedef PVOID MESSAGE_QUEUE;

/** Данные сообщения */
typedef struct _SMessageData
{
	/** Размер сообщения */
	DWORD	dwBufferSize;

	/** Буфер сообщения */
	PVOID	pBuffer;
} SMessageData, * PSMessageData;

/**
* Создать очередь сообщений
* @param[out]		ppsMessageQueue		Экземпляр очереди сообщений
* @param@param[in]	dwMessagesMaxCount	Максимальное количество сообщений в очереди
* @param@param[in]	dwMessageBufferSize	Размер передаваемого сообщения
* @param[out]		dwError				Ошибка
* @return Результат работы функции.
*/
_Success_(return != FALSE)
BOOL
InitializeMessageQueue(
	MESSAGE_QUEUE* hMessageQueue,
	DWORD		  dwMessagesMaxCount,
	DWORD		  dwMessageBufferSize,
	PDWORD		  dwError
);

/**
* Удалить очередь сообщений
* @param[inout]   hMessageQueue		Экземпляр очереди сообщений
* @return							Результат работы функции
*/
VOID
DeinitializeMessageQueue(
	MESSAGE_QUEUE hMessageQueue
);

/**
* Добавить сообщение в очередь
* @param@param[in]  hMessageQueue	Экземпляр очереди сообщений
* @param[out]		sMessageData	Сообщение
* @param[out]		dwError		    Ошибка
* @return							Результат работы функции
*/
BOOL
MessageQueuePush(
	MESSAGE_QUEUE hMessageQueue,
	SMessageData  sMessageData,
	PDWORD		  dwError
);

/**
* Вытащить из очереди первый элемент
* @param@param[in]  hMessageQueue		Экземпляр очереди сообщений
* @param[out]		pMessageData		Данные
* @param[out]		dwError		        Ошибка
* @return Результат работы функции
*/
BOOL
MessageQueuePop(
	MESSAGE_QUEUE hMessageQueue,
	PSMessageData pMessageData,
	PDWORD		  dwError
);

/**
* Очистить очередь сообщений
* @param[out]   hMessageQueue		Экземпляр очереди сообщений
*/
VOID
MessageQueueClear(
	MESSAGE_QUEUE hMessageQueue
);

/**
* Получить количество элементов в очереди
* @param[out]   hMessageQueue		Экземпляр очереди сообщений
* @return Количество элементов в очереди
*/
DWORD
MessageQueueGetCount(
	MESSAGE_QUEUE hMessageQueue
);

#endif // MESSAGE_QUEUE_H