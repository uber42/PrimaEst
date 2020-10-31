/**
 * @file ServerRPC.h
 *
 * @author Pavel Chursin
 * @date Oct 2020
 */


#ifndef SERVER_RPC_H
#define SERVER_RPC_H

#include "DefinitionRPC.h"


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
* 
*/
BOOL
InitializeRPCServer(
	PSRpcServer			psRpcServer,
	PCHAR				pszAddress,
	PCHAR				pszPort,
	DWORD				dwMaxConnections,
	RPCRequestHandler	cbRequestHandler
);

BOOL
StartRPCServer(
	PSRpcServer	psRpcServer
);


BOOL
StartRPCServer(
	PSRpcServer	psRpcServer
);


VOID
CloseRPCServer(
	PSRpcServer	psRpcServer
);


#endif