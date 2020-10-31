/**
 * @file RpcTest.c
 *
 * @author Pavel Chursin
 * @date Oct 2020
 */

#include "../global.h"


typedef enum _ECalcRpcFunc
{
	ECRF_ADD,
	ECRF_SUB,
	ECRF_MUL,
	ECRF_DIV
} ECalcRpcFunc, *PECalcRpcFunc;

typedef struct _STestRpcRequest
{
	DWORD	dwFirst;
	DWORD	dwSecond;
} STestRpcRequest, *PSTestRpcRequest;

typedef struct _STestRpcResponse
{
	DWORD	dwResult;
} STestRpcResponse, *PSTestRpcResponse;

DWORD
AddTestFunc(DWORD dwFirst, DWORD dwSecond);

DWORD
SubTestFunc(DWORD dwFirst, DWORD dwSecond);

DWORD
MulTestFunc(DWORD dwFirst, DWORD dwSecond);

DWORD
DivTestFunc(DWORD dwFirst, DWORD dwSecond);

static
BOOL
TestRPCHandler(
	DWORD			dwRequestCode,
	PBYTE			pDataBuffer,
	DWORD			dwSize,
	PBYTE			pOutBuffer,
	PDWORD			pdwOutSize
);

static
BOOL
TestRPCHandler(
	DWORD			dwRequestCode,
	PBYTE			pDataBuffer,
	DWORD			dwSize,
	PBYTE			pOutBuffer,
	PDWORD			pdwOutSize
)
{
	PSTestRpcRequest psRpcRequest = pDataBuffer;
	DWORD dwResult = 0;

	switch (dwRequestCode)
	{
	case ECRF_ADD:
		dwResult = AddTestFunc(
			psRpcRequest->dwFirst,
			psRpcRequest->dwSecond
		);
		break;
	case ECRF_SUB:
		dwResult = SubTestFunc(
			psRpcRequest->dwFirst,
			psRpcRequest->dwSecond
		);
		break;
	case ECRF_MUL:
		dwResult = MulTestFunc(
			psRpcRequest->dwFirst,
			psRpcRequest->dwSecond
		);
		break;
	case ECRF_DIV:
		dwResult = DivTestFunc(
			psRpcRequest->dwFirst,
			psRpcRequest->dwSecond
		);
		break;
	default:
		break;
	}

	PBYTE pResult = &dwResult;
	memcpy(pOutBuffer, pResult, sizeof(STestRpcResponse));
	*pdwOutSize = sizeof(STestRpcResponse);

	return TRUE;
}

DWORD
AddTestFunc(DWORD dwFirst, DWORD dwSecond)
{
	return dwFirst + dwSecond;
}

DWORD
SubTestFunc(DWORD dwFirst, DWORD dwSecond)
{
	return dwFirst - dwSecond;
}

DWORD
MulTestFunc(DWORD dwFirst, DWORD dwSecond)
{
	return dwFirst * dwSecond;
}

DWORD
DivTestFunc(DWORD dwFirst, DWORD dwSecond)
{
	return dwFirst / dwSecond;
}


VOID
RpcTest()
{
	InitializeWSA();

	SRpcServer sRpcServer;

	BOOL bResult = InitializeRPCServer(
		&sRpcServer,
		"localhost", 
		"2222", 5,
		TestRPCHandler);
	assert(bResult);

	bResult = StartRPCServer(&sRpcServer);
	assert(bResult);

	STestRpcRequest sReq = {
		.dwFirst = 12,
		.dwSecond = 6
	};

	SRpcClient sRpcClient;
	bResult = CreateRPCClient(
		&sRpcClient,
		"localhost", "2222");
	assert(bResult);

	BOOL bSuccess = FALSE;
	STestRpcResponse sRpcResponse = { 0 };
	bResult = CallRPC(
		&sRpcClient,
		ECRF_ADD,
		&sReq, sizeof(STestRpcRequest),
		&bSuccess, &sRpcResponse);
	assert(bResult);
	
	assert(((sReq.dwFirst + sReq.dwSecond) == sRpcResponse.dwResult));

	bResult = CallRPC(
		&sRpcClient,
		ECRF_SUB,
		&sReq, sizeof(STestRpcRequest),
		&bSuccess, &sRpcResponse);
	assert(bResult);

	assert(((sReq.dwFirst - sReq.dwSecond) == sRpcResponse.dwResult));

	bResult = CallRPC(
		&sRpcClient,
		ECRF_MUL,
		&sReq, sizeof(STestRpcRequest),
		&bSuccess, &sRpcResponse);
	assert(bResult);

	assert(((sReq.dwFirst * sReq.dwSecond) == sRpcResponse.dwResult));

	bResult = CallRPC(
		&sRpcClient,
		ECRF_DIV,
		&sReq, sizeof(STestRpcRequest),
		&bSuccess, &sRpcResponse);
	assert(bResult);

	assert(((sReq.dwFirst / sReq.dwSecond) == sRpcResponse.dwResult));

	CloseRPCServer(&sRpcServer);
	CloseRPCClient(&sRpcClient);

	DeinitializeWSA();
}

#define RPC_TEST_THREAD_COUNT 10
#define RPC_TEST_REPEAT_COUNT 100

static
DWORD
WINAPI
RpcTestRoutine(
	LPVOID	lpThreadParameter
)
{
	SRpcClient sRpcClient;
	BOOL bResult = CreateRPCClient(
		&sRpcClient,
		"localhost", "2222");
	assert(bResult);

	for (int i = 1; i <= RPC_TEST_REPEAT_COUNT; i++)
	{
		STestRpcRequest sReq = {
			.dwFirst = i * 4,
			.dwSecond = i * 2
		};

		BOOL bSuccess = FALSE;
		STestRpcResponse sRpcResponse = { 0 };
		bResult = CallRPC(
			&sRpcClient,
			ECRF_ADD,
			&sReq, sizeof(STestRpcRequest),
			&bSuccess, &sRpcResponse);
		assert(bResult);

		assert(((sReq.dwFirst + sReq.dwSecond) == sRpcResponse.dwResult));

		bResult = CallRPC(
			&sRpcClient,
			ECRF_SUB,
			&sReq, sizeof(STestRpcRequest),
			&bSuccess, &sRpcResponse);
		assert(bResult);

		assert(((sReq.dwFirst - sReq.dwSecond) == sRpcResponse.dwResult));

		bResult = CallRPC(
			&sRpcClient,
			ECRF_MUL,
			&sReq, sizeof(STestRpcRequest),
			&bSuccess, &sRpcResponse);
		assert(bResult);

		assert(((sReq.dwFirst * sReq.dwSecond) == sRpcResponse.dwResult));

		bResult = CallRPC(
			&sRpcClient,
			ECRF_DIV,
			&sReq, sizeof(STestRpcRequest),
			&bSuccess, &sRpcResponse);
		assert(bResult);

		assert(((sReq.dwFirst / sReq.dwSecond) == sRpcResponse.dwResult));
	}	

	CloseRPCClient(&sRpcClient);

	return 0;
}


VOID
RpcMultiThreadTest()
{
	InitializeWSA();

	SRpcServer sRpcServer;

	BOOL bResult = InitializeRPCServer(
		&sRpcServer,
		"localhost",
		"2222", 15,
		TestRPCHandler);
	assert(bResult);

	bResult = StartRPCServer(&sRpcServer);
	assert(bResult);

	HANDLE hThreads[RPC_TEST_THREAD_COUNT] = { 0 };
	for (int i = 0; i < RPC_TEST_THREAD_COUNT; i++)
	{
		hThreads[i] = CreateThread(
			NULL, 0,
			RpcTestRoutine,
			NULL, 0, NULL
		);
	}

	WaitForMultipleObjects(
		RPC_TEST_THREAD_COUNT,
		hThreads, TRUE, INFINITE);

	for (int i = 0; i < RPC_TEST_THREAD_COUNT; i++)
	{
#pragma warning(suppress : 6001)
#pragma warning(suppress : 6387)
		CloseHandle(hThreads[i]);
	}

	CloseRPCServer(&sRpcServer);

	DeinitializeWSA();
}