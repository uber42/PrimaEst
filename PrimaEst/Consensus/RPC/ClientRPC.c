/**
 * @file ClientRPC.c
 *
 * @author Pavel Chursin
 * @date Oct 2020
 */

#include "../../global.h"
#include "DefinitionRPC.h"


BOOL
RPCReconnect(
	PSRpcClient	sRpcClient
);

BOOL
RPCReconnect(
	PSRpcClient	sRpcClient
)
{
	CloseSocket(&sRpcClient->sSocket);

	BOOL bResult = CreateSocket(
		&sRpcClient->sSocket,
		sRpcClient->szAddress, 
		sRpcClient->szPort);
	if (!bResult)
	{
		free(sRpcClient);
		return FALSE;
	}

	bResult = ConnectSocket(sRpcClient->sSocket);
	if (bResult)
	{
		CloseSocket(&sRpcClient->sSocket);
		free(sRpcClient);
		return FALSE;
	}

	return TRUE;
}


BOOL
CreateRPCClient(
	PSRpcClient	sRpcClient,
	PCHAR		szAddress,
	PCHAR		szPort
)
{
	BOOL bResult = CreateSocket(
		&sRpcClient->sSocket,
		szAddress, szPort);
	if (!bResult)
	{
		return FALSE;
	}

	strcpy(sRpcClient->szAddress, szAddress);
	strcpy(sRpcClient->szPort, szPort);

	bResult = ConnectSocket(sRpcClient->sSocket);
	if (!bResult)
	{
		CloseSocket(&sRpcClient->sSocket);
		return FALSE;
	}

	return TRUE;
}


BOOL
CallRPC(
	PSRpcClient	sRpcClient,
	DWORD		dwRpcCall,
	PBYTE		pbBuffer,
	DWORD		dwSize,
	PDWORD		pdwResult,
	PBYTE		pbResultBuffer
)
{
	PSRpcRequest psRpcRequest = malloc(sizeof(SRpcRequest) + sizeof(BYTE) * dwSize);
	if (!psRpcRequest)
	{
		LogError("[Client RPC] Ошибка выделения памяти");
		return FALSE;
	}

	psRpcRequest->dwRequestID = dwRpcCall;
	memcpy(psRpcRequest->pbBuffer, pbBuffer, dwSize);

	BOOL bResult = SocketSend(
		sRpcClient->sSocket,
		psRpcRequest,
		sizeof(SRpcRequest) + dwSize
	);
	if (!bResult)
	{
		bResult = RPCReconnect(sRpcClient);
		if (!bResult)
		{
			free(psRpcRequest);
			return FALSE;
		}

		bResult = SocketSend(
			sRpcClient->sSocket,
			psRpcRequest,
			sizeof(SRpcRequest) + dwSize
		);
		if (bResult)
		{
			free(psRpcRequest);
			return FALSE;
		}
	}

	free(psRpcRequest);

	DWORD dwRecvSize = 0;
	PBYTE pbRecvBuffer = NULL;
	bResult = SocketReceive(
		sRpcClient->sSocket,
		&pbRecvBuffer,
		&dwRecvSize, 0);
	if (!bResult)
	{
		return FALSE;
	}

	SRpcResponse sRpcResponse;
	memcpy(&sRpcResponse, pbRecvBuffer, dwRecvSize);
	
	*pdwResult = sRpcResponse.bSuccess;
	memcpy(pbResultBuffer, sRpcResponse.pbBuffer, dwRecvSize - RPC_RESPONSE_SIZE);

	free(pbRecvBuffer);

	return TRUE;
}

VOID
CloseRPCClient(
	PSRpcClient	sRpcClient
)
{
	CloseSocket(&sRpcClient->sSocket);
}