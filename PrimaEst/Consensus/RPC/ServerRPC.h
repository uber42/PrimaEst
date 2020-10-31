/**
 * @file ServerRPC.h
 *
 * @author Pavel Chursin
 * @date Oct 2020
 */


#ifndef SERVER_RPC_H
#define SERVER_RPC_H

#include "DefinitionRPC.h"


 /**
 * Обработчик RPC запроса
 * @param[in] dwRequestCode		Идентификатор функции
 * @param[in] pDataBuffer		Входной буфер
 * @param[in] dwSize			Размер входных данных
 * @param[in] pOutBuffer		Выходной буфер
 * @param[in] pdwOutSize		Размер выходных данных
 * @return						Результат работы.
 */
typedef
BOOL
(*RPCRequestHandler)(
	DWORD			dwRequestCode,
	PBYTE			pDataBuffer,
	DWORD			dwSize,
	PBYTE			pOutBuffer,
	PDWORD			pdwOutSize
);


/**
* Структура RPC сервер
*/
typedef struct _SRpcServer
{
	/** Сокет сервера */
	SSocket			sSocket;

	/** Адрес сервера */
	CHAR			szAddress[IPV4_ADDRESS_MAX_LENGTH];

	/** Порт сервера */
	CHAR			szPort[PORT_MAX_SYMBOL_LENGTH];

	/** Поток прослушивания */
	HANDLE			hThread;

	/** Обработчик RPC запроса */
	RPCRequestHandler cbRpcHandler;

	/** Максимальное количество подключений  */
	DWORD			dwMaxConnections;

	/** Количество подключений */
	volatile DWORD	dwConnections;

	/** Работает ли сервер */
	volatile BOOL	bIsWork;
} SRpcServer, *PSRpcServer;



/**
* Инициализировать RPC сервер
* @param[in] psRpcServer		Экземпляр сервера
* @param[in] pszAddress			Адрес
* @param[in] pszPort			Порт
* @param[in] dwMaxConnections	Максимальное количество соединений
* @param[in] cbRequestHandler	Обработчик RPC
* @return						Результат работы.
*/
BOOL
InitializeRPCServer(
	PSRpcServer			psRpcServer,
	PCHAR				pszAddress,
	PCHAR				pszPort,
	DWORD				dwMaxConnections,
	RPCRequestHandler	cbRequestHandler
);

/**
* Запустить RPC сервер
* @param[in] psRpcServer		Экземпляр сервера
*/
BOOL
StartRPCServer(
	PSRpcServer	psRpcServer
);

/**
* Закрыть описатель RPC сервера
* @param[in] psRpcServer		Экземпляр сервера
* @return						Результат работы.
*/
VOID
CloseRPCServer(
	PSRpcServer	psRpcServer
);


#endif