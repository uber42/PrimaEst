/**
 * @file ServerRPC.c
 *
 * @author Pavel Chursin
 * @date Oct 2020
 */

#include "../../global.h"
#include "DefinitionRPC.h"


/**
* Структура аргумент клиентской сессии RPC
*/
typedef struct _SClientRoutineArg
{
	/** Клиентский сокет */
	SSocket				sClientSocket;

	/** Обработчик RPC */
	RPCRequestHandler	cbRpcHandler;

	/** Работает ли сервер */
	volatile BOOL*		bIsWork;

	/** Количество подключений */
	volatile DWORD*		dwConnections;
} SClientRoutineArg, * PSClientRoutineArg;


/**
* Обработчик клиентской сессии RPC
* @param[in] lpThreadParameter	Аргумент обработчика
* @return						Результат работы.
*/
static
DWORD
WINAPI
RPCClientRoutine(
	LPVOID	lpThreadParameter
);

/**
* Обработчик принятия подключений
* @param[in] lpThreadParameter	Аргумент обработчика
* @return						Результат работы.
*/
static
DWORD
WINAPI
RPCServerRoutine(
	LPVOID	lpThreadParameter
);

/**
* Обработчик принятия подключений
* @param[in] lpThreadParameter	Аргумент обработчика
* @return						Результат работы.
*/
static
DWORD
WINAPI
RPCServerRoutine(
	LPVOID	lpThreadParameter
)
{
	PSRpcServer psRpcServer = (PSRpcServer)lpThreadParameter;

	while (psRpcServer->bIsWork)
	{
		SSocket sClientSocket;
		BOOL bResult = AcceptSocket(
			psRpcServer->sSocket,
			&sClientSocket);

		if (!bResult)
		{
			if (!psRpcServer->bIsWork)
			{
				LogInfo("[RPCServer] Работа сервера завершена");
				break;
			}

			bResult = SocketIsClosed(psRpcServer->sSocket);
			if (bResult)
			{
				CloseSocket(&psRpcServer->sSocket);

				bResult = 
					CreateSocket(
						&psRpcServer->sSocket,
						psRpcServer->szAddress,
						psRpcServer->szPort) &&
					BindSocket(psRpcServer->sSocket) &&
					ListenSocket(psRpcServer->sSocket,
						psRpcServer->dwMaxConnections);
				if (!bResult)
				{
					psRpcServer->bIsWork = FALSE;
					LogExtra("[RPCServer] Аварийное завершение");
					break;
				}
			}

			LogDebug("[RPCServer] RPCServerRoutine -> AcceptSocket");
			continue;
		}

		PSClientRoutineArg arg = malloc(sizeof(SClientRoutineArg));
		if (!arg)
		{

			CloseSocket(&sClientSocket);
			continue;
		}

		arg->sClientSocket = sClientSocket;
		arg->bIsWork = &psRpcServer->bIsWork;
		arg->cbRpcHandler = psRpcServer->cbRpcHandler;
		arg->dwConnections = &psRpcServer->dwConnections;

		HANDLE hClientWorker = CreateThread(
			NULL, 0,
			RPCClientRoutine,
			(LPVOID)arg,
			0, NULL);
		if (hClientWorker == NULL)
		{
			free(arg);

			CloseSocket(&sClientSocket);
			continue;
		}
		
		CloseHandle(hClientWorker);

		InterlockedIncrement(&psRpcServer->dwConnections);
	}

	return 0;
}

/**
* Обработчик клиентской сессии RPC
* @param[in] lpThreadParameter	Аргумент обработчика
* @return						Результат работы.
*/
static
DWORD
WINAPI
RPCClientRoutine(
	LPVOID	lpThreadParameter
)
{
	PSClientRoutineArg sArg = (PSClientRoutineArg)lpThreadParameter;

	while ((*sArg->bIsWork))
	{
		PBYTE pBuffer = NULL;
		DWORD dwSize = 0;
		BOOL bResult = SocketReceive(
			sArg->sClientSocket,
			&pBuffer,
			&dwSize, 10000);
		if (!bResult)
		{			
			if (WSAGetLastError() == TIMEOUT_EXPIRED)
			{
				continue;
			}
			else
			{
				break;
			}
		}

		DWORD dwOutSize = 0;
		PSRpcRequest psRpcRequest = pBuffer;
		SRpcResponse sRpcResponse = { 0 };
		bResult = sArg->cbRpcHandler(
			psRpcRequest->dwRequestID,
			psRpcRequest->pbBuffer,
			dwSize - sizeof(psRpcRequest->dwRequestID),
			sRpcResponse.pbBuffer, &dwOutSize);
		sRpcResponse.dwRequestID = psRpcRequest->dwRequestID;
		sRpcResponse.bSuccess = bResult;

		free(pBuffer);

		bResult = SocketSend(
			sArg->sClientSocket,
			&sRpcResponse, 
			RPC_RESPONSE_SIZE + dwOutSize);
		if (!bResult)
		{
			continue;
		}
	}

	InterlockedDecrement(sArg->dwConnections);

	CloseSocket(&sArg->sClientSocket);
	free(sArg);
	
	return 0;
}

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
)
{
	BOOL bResult = CreateSocket(
		&psRpcServer->sSocket,
		pszAddress, pszPort);
	if (!bResult)
	{
		return FALSE;
	}

	psRpcServer->bIsWork = FALSE;
	psRpcServer->dwMaxConnections = dwMaxConnections;
	psRpcServer->dwConnections = 0;
	psRpcServer->cbRpcHandler = cbRequestHandler;
	strcpy(psRpcServer->szAddress, pszAddress);
	strcpy(psRpcServer->szPort, pszPort);
	
	psRpcServer->hThread = CreateThread(
		NULL, 0,
		RPCServerRoutine,
		psRpcServer,
		CREATE_SUSPENDED,
		NULL
	);
	if (psRpcServer->hThread == NULL)
	{
		LogError("[ServerRPC] Ошибка создания прослушивающей нити");

		return FALSE;
	}

	return TRUE;
}


/**
* Запустить RPC сервер
* @param[in] psRpcServer		Экземпляр сервера
*/
BOOL
StartRPCServer(
	PSRpcServer	psRpcServer
)
{
	BOOL bResult = BindSocket(psRpcServer->sSocket);
	if (!bResult)
	{
		LogDebug("[ServerRPC] StartRPCServer -> BindSocket");
		return FALSE;
	}

	bResult = ListenSocket(psRpcServer->sSocket,
		psRpcServer->dwMaxConnections);
	if (!bResult)
	{
		LogDebug("[ServerRPC] StartRPCServer -> ListenSocket");
		return FALSE;
	}

	psRpcServer->bIsWork = TRUE;
	DWORD dwResult = ResumeThread(psRpcServer->hThread);
	if (dwResult == (DWORD)(-1))
	{
		LogError("[RpcServer] Ошибка запуска нити");
		return FALSE;
	}

	return TRUE;
}

/**
* Закрыть описатель RPC сервера
* @param[in] psRpcServer		Экземпляр сервера
* @return						Результат работы.
*/
VOID
CloseRPCServer(
	PSRpcServer	psRpcServer
)
{
	psRpcServer->bIsWork = FALSE;
	CloseSocket(&psRpcServer->sSocket);

	WaitForSingleObject(psRpcServer->hThread, INFINITE);
	while (psRpcServer->dwConnections != 0);

	CloseHandle(psRpcServer->hThread);
}