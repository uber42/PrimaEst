#ifndef DEFENITION_RPC_H
#define DEFENITION_RPC_H

/**
* RPC Запрос
*/
typedef struct _SRpcRequest
{
	/** Тип запроса */
	DWORD	dwRequestID;

	/** Данные */
	BYTE	pbBuffer[0];
} SRpcRequest, *PSRpcRequest;

#define RPC_RESPONSE_BUFFER_SIZE 1024
#define RPC_RESPONSE_SIZE (sizeof(SRpcResponse) - RPC_RESPONSE_BUFFER_SIZE)

/**
* RPC Запрос
*/
typedef struct _SRpcResponse
{
	/** Тип запроса */
	DWORD	dwRequestID;

	/** Успешно ли ? */
	BOOL	bSuccess;

	/** Данные */
	BYTE	pbBuffer[RPC_RESPONSE_BUFFER_SIZE];
} SRpcResponse, * PSRpcResponse;


#endif