/**
 * @file ClientRPC.h
 *
 * @author Pavel Chursin
 * @date Oct 2020
 */

#ifndef CLIENT_RPC_H
#define CLIENT_RPC_H

/**
* Структура RPC клинет
*/
typedef struct _SRpcClient
{
	/** Сокет */
	SSocket			sSocket;

	/** Адрес сервера */
	CHAR			szAddress[IPV4_ADDRESS_MAX_LENGTH];

	/** Порт сервера */
	CHAR			szPort[PORT_MAX_SYMBOL_LENGTH];
} SRpcClient, *PSRpcClient;


/**
* Создать RPC соединение
* @param[in] sRpcClient	Экземпляр клинета
* @param[in] szAddress	Адрес RPC сервера
* @param[in] szPort		Порт RPC сервера
* @return				Результат работы.
*/
BOOL
CreateRPCClient(
	PSRpcClient	sRpcClient,
	PCHAR		szAddress,
	PCHAR		szPort
);

/**
* Вызвать RPC функцию
* @param[in] sRpcClient		Экземпляр клинета
* @param[in] dwRpcCall		Идентификатор функции
* @param[in] pbBuffer		Входной буфер
* @param[in] dwSize			Размер входных данных
* @param[in] pdwResult		Результат вызова
* @param[in] pbResultBuffer	Выходные данные
* @return					Результат работы.
*/
BOOL
CallRPC(
	PSRpcClient	sRpcClient,
	DWORD		dwRpcCall,
	PBYTE		pbBuffer,
	DWORD		dwSize,
	PDWORD		pdwResult,
	PBYTE		pbResultBuffer
);

/**
* Закрыть RPC соединение
* @param[in] sRpcClient		Экземпляр клинета
*/
VOID
CloseRPCClient(
	PSRpcClient	sRpcClient
);


#endif