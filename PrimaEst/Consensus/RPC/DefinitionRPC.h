#ifndef DEFENITION_RPC_H
#define DEFENITION_RPC_H

/**
* RPC ������
*/
typedef struct _SRpcRequest
{
	/** ��� ������� */
	DWORD	dwRequestID;

	/** ������ */
	BYTE	pbBuffer[0];
} SRpcRequest, *PSRpcRequest;

#define RPC_RESPONSE_BUFFER_SIZE 1024
#define RPC_RESPONSE_SIZE (sizeof(SRpcResponse) - RPC_RESPONSE_BUFFER_SIZE)

/**
* RPC ������
*/
typedef struct _SRpcResponse
{
	/** ��� ������� */
	DWORD	dwRequestID;

	/** ������� �� ? */
	BOOL	bSuccess;

	/** ������ */
	BYTE	pbBuffer[RPC_RESPONSE_BUFFER_SIZE];
} SRpcResponse, * PSRpcResponse;


#endif