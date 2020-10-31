/**
 * @file ClientRPC.h
 *
 * @author Pavel Chursin
 * @date Oct 2020
 */

#ifndef CLIENT_RPC_H
#define CLIENT_RPC_H

typedef struct _SRpcClient
{
	/** Сокет */
	SSocket			sSocket;

	/** Адрес сервера */
	CHAR			szAddress[IPV4_ADDRESS_MAX_LENGTH];

	/** Порт сервера */
	CHAR			szPort[PORT_MAX_SYMBOL_LENGTH];
} SRpcClient, *PSRpcClient;


BOOL
CreateRPCClient(
	PSRpcClient	sRpcClient,
	PCHAR		szAddress,
	PCHAR		szPort
);


BOOL
CallRPC(
	PSRpcClient	sRpcClient,
	DWORD		dwRpcCall,
	PBYTE		pbBuffer,
	DWORD		dwSize,
	PDWORD		pdwResult,
	PBYTE		pbResultBuffer
);


VOID
CloseRPCClient(
	PSRpcClient	sRpcClient
);


#endif