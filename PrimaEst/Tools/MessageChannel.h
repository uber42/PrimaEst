#ifndef MESSAGE_CHANNEL_H
#define MESSAGE_CHANNEL_H

#define MESSAGE_CHANNEL_BAD_RESULT ((DWORD)(-1))

typedef PVOID MESSAGE_CHANNEL;

/**
* В какой памяти находится буфер передаваемый по каналу
*/
typedef enum _EMCBufferAllocated
{
	EMCBA_STACK_ALLOCATED,

	EMCBA_HEAP_ALLOCATED
} EMCBufferAllocated, *PEMCBufferAllocated;

/**
* Создать канал сообщений
* @param[out]   hMessageChannel		Экземпляр канала сообщений.
* @return Результат работы функции.
*/
BOOL
CreateMessageChannel(
	MESSAGE_CHANNEL *hMessageChannel
);

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
);

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
);

/**
* Получить размер текущего сообщения в канале
* @param[in]   hMessageChannel		Экземпляр канала сообщений.
* @return Размер
*/
DWORD
MCGetCurrentBufferSize(
	MESSAGE_CHANNEL hMessageChannel
);

/**
* Закрыть канал сообщений
* @param[in]   hMessageChannel		Экземпляр канала сообщений.
*/
VOID
CloseMessageChannel(
	MESSAGE_CHANNEL hMessageChannel
);




#endif