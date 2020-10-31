/**
 * @file SocketWrapperTest.c
 *
 * @author Pavel Chursin
 * @date Oct 2020
 */

#include "../global.h"

BYTE g_bTestBuffer_1[5555] = { 0 };
BYTE g_bTestBuffer_2[777] = { 0 };

HANDLE g_hFirstReadEvent = { 0 };

#define SOCKET_WRAPPER_TEST_HOST "localhost"
#define SOCKET_WRAPPER_TEST_PORT "27777"

static
DWORD
WINAPI
ServerTestRoutine(
	LPVOID lpThreadParameter
)
{
	UNREFERENCED_PARAMETER(lpThreadParameter);

	SSocket sServerSocket;
	BOOL bResult = CreateSocket(
		&sServerSocket,
		SOCKET_WRAPPER_TEST_HOST,
		SOCKET_WRAPPER_TEST_PORT);
	if (!bResult)
	{
		assert(FALSE);
	}

	bResult = BindSocket(sServerSocket);
	if (!bResult)
	{
		CloseSocket(&sServerSocket);
		assert(FALSE);
	}

	bResult = ListenSocket(sServerSocket, 2);
	if (!bResult)
	{
		CloseSocket(&sServerSocket);
		assert(FALSE);
	}

	SSocket sClientSocket;
	bResult = AcceptSocket(sServerSocket, &sClientSocket);
	if (!bResult)
	{
		CloseSocket(&sServerSocket);
		assert(FALSE);
	}
	
	DWORD dwSize = 1024;
	PBYTE pbBuffer = malloc(dwSize);

	bResult = SocketReceive(
		sClientSocket,
		&pbBuffer,
		&dwSize, 0);
	if (!bResult)
	{
		CloseSocket(&sServerSocket);
		assert(FALSE);
	}

	assert(dwSize == sizeof(g_bTestBuffer_1));
	for (DWORD dwIdx = 0; dwIdx < sizeof(g_bTestBuffer_1); dwIdx++)
	{
		assert(pbBuffer[dwIdx] == g_bTestBuffer_1[dwIdx]);
	}

	CloseSocket(&sClientSocket);
	SetEvent(g_hFirstReadEvent);

	bResult = AcceptSocket(sServerSocket, &sClientSocket);
	if (!bResult)
	{
		CloseSocket(&sServerSocket);
		assert(FALSE);
	}

	dwSize = 1024;
	PBYTE pbBuffer_2 = malloc(dwSize);
	bResult = SocketReceive(
		sClientSocket,
		&pbBuffer_2,
		&dwSize, 0);
	if (!bResult)
	{
		CloseSocket(&sServerSocket);
		assert(FALSE);
	}

	assert(dwSize == sizeof(g_bTestBuffer_2));
	for (DWORD dwIdx = 0; dwIdx < sizeof(g_bTestBuffer_2); dwIdx++)
	{
		assert(pbBuffer_2[dwIdx] == g_bTestBuffer_2[dwIdx]);
	}

	free(pbBuffer);
	free(pbBuffer_2);

	CloseSocket(&sServerSocket);
	CloseSocket(&sClientSocket);
}

static
DWORD
WINAPI
ClientTestRoutine(
	LPVOID lpThreadParameter
)
{
	UNREFERENCED_PARAMETER(lpThreadParameter);

	for (DWORD dwIdx = 0; dwIdx < sizeof(g_bTestBuffer_1); dwIdx++)
	{
		g_bTestBuffer_1[dwIdx] = rand() % 1000;
	}

	for (DWORD dwIdx = 0; dwIdx < sizeof(g_bTestBuffer_2); dwIdx++)
	{
		g_bTestBuffer_2[dwIdx] = rand() % 1000;
	}

	SSocket sClientSocket;
	BOOL bResult = CreateSocket(
		&sClientSocket,
		SOCKET_WRAPPER_TEST_HOST,
		SOCKET_WRAPPER_TEST_PORT);
	if (!bResult)
	{
		assert(FALSE);
	}

	bResult = ConnectSocket(sClientSocket);
	if (!bResult)
	{
		CloseSocket(&sClientSocket);
		assert(FALSE);
	}

	Sleep(300);

	bResult = SocketSend(sClientSocket, g_bTestBuffer_1, sizeof(g_bTestBuffer_1));
	if (!bResult)
	{
		CloseSocket(&sClientSocket);
		assert(FALSE);
	}

	WaitForSingleObject(g_hFirstReadEvent, INFINITE);

	bResult = CreateSocket(
		&sClientSocket,
		SOCKET_WRAPPER_TEST_HOST,
		SOCKET_WRAPPER_TEST_PORT);
	if (!bResult)
	{
		assert(FALSE);
	}
	bResult = ConnectSocket(sClientSocket);
	if (!bResult)
	{
		CloseSocket(&sClientSocket);
		assert(FALSE);
	}

	Sleep(300);

	bResult = SocketSend(sClientSocket, g_bTestBuffer_2, sizeof(g_bTestBuffer_2));
	if (!bResult)
	{
		CloseSocket(&sClientSocket);
		assert(FALSE);
	}

	CloseSocket(&sClientSocket);
}

static
DWORD
WINAPI
ServerWithCheckSumTestRoutine(
	LPVOID lpThreadParameter
)
{
	UNREFERENCED_PARAMETER(lpThreadParameter);

	SSocket sServerSocket;
	BOOL bResult = CreateSocket(
		&sServerSocket,
		SOCKET_WRAPPER_TEST_HOST,
		SOCKET_WRAPPER_TEST_PORT);
	if (!bResult)
	{
		assert(FALSE);
	}

	bResult = BindSocket(sServerSocket);
	if (!bResult)
	{
		CloseSocket(&sServerSocket);
		assert(FALSE);
	}

	bResult = ListenSocket(sServerSocket, 2);
	if (!bResult)
	{
		CloseSocket(&sServerSocket);
		assert(FALSE);
	}

	SSocket sClientSocket;
	bResult = AcceptSocket(sServerSocket, &sClientSocket);
	if (!bResult)
	{
		CloseSocket(&sServerSocket);
		assert(FALSE);
	}

	DWORD dwSize = 1024;
	PBYTE pbBuffer = malloc(dwSize);

	bResult = SocketReceiveWithCheck(
		sClientSocket,
		&pbBuffer,
		&dwSize, 0);
	if (!bResult)
	{
		CloseSocket(&sServerSocket);
		assert(FALSE);
	}

	assert(dwSize == sizeof(g_bTestBuffer_1));
	for (DWORD dwIdx = 0; dwIdx < sizeof(g_bTestBuffer_1); dwIdx++)
	{
		assert(pbBuffer[dwIdx] == g_bTestBuffer_1[dwIdx]);
	}

	CloseSocket(&sClientSocket);
	SetEvent(g_hFirstReadEvent);

	bResult = AcceptSocket(sServerSocket, &sClientSocket);
	if (!bResult)
	{
		CloseSocket(&sServerSocket);
		assert(FALSE);
	}


	dwSize = 1024;
	PBYTE pbBuffer_2 = malloc(dwSize);
	bResult = SocketReceiveWithCheck(
		sClientSocket,
		&pbBuffer_2,
		&dwSize, 0);

	assert(dwSize == sizeof(g_bTestBuffer_2));
	for (DWORD dwIdx = 0; dwIdx < sizeof(g_bTestBuffer_2); dwIdx++)
	{
		assert(pbBuffer_2[dwIdx] == g_bTestBuffer_2[dwIdx]);
	}

	free(pbBuffer);
	free(pbBuffer_2);

	CloseSocket(&sServerSocket);
	CloseSocket(&sClientSocket);
}

static
DWORD
WINAPI
ClientWithCheckSumTestRoutine(
	LPVOID lpThreadParameter
)
{
	UNREFERENCED_PARAMETER(lpThreadParameter);

	for (DWORD dwIdx = 0; dwIdx < sizeof(g_bTestBuffer_1); dwIdx++)
	{
		g_bTestBuffer_1[dwIdx] = rand() % 1000;
	}

	for (DWORD dwIdx = 0; dwIdx < sizeof(g_bTestBuffer_2); dwIdx++)
	{
		g_bTestBuffer_2[dwIdx] = rand() % 1000;
	}

	SSocket sClientSocket;
	BOOL bResult = CreateSocket(
		&sClientSocket,
		SOCKET_WRAPPER_TEST_HOST,
		SOCKET_WRAPPER_TEST_PORT);
	if (!bResult)
	{
		assert(FALSE);
	}

	bResult = ConnectSocket(sClientSocket);
	if (!bResult)
	{
		CloseSocket(&sClientSocket);
		assert(FALSE);
	}

	Sleep(300);

	bResult = SocketSendWithCheck(sClientSocket, g_bTestBuffer_1, sizeof(g_bTestBuffer_1));
	if (!bResult)
	{
		CloseSocket(&sClientSocket);
		assert(FALSE);
	}

	WaitForSingleObject(g_hFirstReadEvent, INFINITE);

	bResult = CreateSocket(
		&sClientSocket,
		SOCKET_WRAPPER_TEST_HOST,
		SOCKET_WRAPPER_TEST_PORT);
	if (!bResult)
	{
		assert(FALSE);
	}
	bResult = ConnectSocket(sClientSocket);
	if (!bResult)
	{
		CloseSocket(&sClientSocket);
		assert(FALSE);
	}

	Sleep(300);

	bResult = SocketSendWithCheck(sClientSocket, g_bTestBuffer_2, sizeof(g_bTestBuffer_2));
	if (!bResult)
	{
		CloseSocket(&sClientSocket);
		assert(FALSE);
	}

	CloseSocket(&sClientSocket);
}

VOID
SocketWrapperTest()
{
	InitializeWSA();

	HANDLE hThreads[2] = { 0 };

	g_hFirstReadEvent = CreateEvent(
		NULL, FALSE, FALSE, NULL
	);

	hThreads[0] = CreateThread(
		NULL, 0,
		ServerTestRoutine,
		NULL, 0, NULL
	);

	hThreads[1] = CreateThread(
		NULL, 0,
		ClientTestRoutine,
		NULL, 0, NULL
	);

	WaitForMultipleObjects(2, hThreads, TRUE, INFINITE);

	DeinitializeWSA();
}

VOID
SocketWrapperTimeoutTest()
{
	InitializeWSA();

	SSocket sServerSocket;

	BOOL bResult = CreateSocket(
		&sServerSocket,
		SOCKET_WRAPPER_TEST_HOST,
		SOCKET_WRAPPER_TEST_PORT
	);
	if (!bResult)
	{
		assert(FALSE);
	}

	bResult = BindSocket(sServerSocket);
	if (!bResult)
	{
		CloseSocket(&sServerSocket);
		assert(FALSE);
	}

	bResult = ListenSocket(sServerSocket, 1);
	if (!bResult)
	{
		CloseSocket(&sServerSocket);
		assert(FALSE);
	}

	PBYTE pbBuffer = NULL;
	DWORD dwSize = 0;
	BOOL bReallocated = FALSE;
	bResult = SocketReceive(
		sServerSocket,
		&pbBuffer, dwSize,
		250);
	
	assert(!bResult);
	assert(dwSize == 0);
	assert(WSAGetLastError() == TIMEOUT_EXPIRED);

	CloseSocket(&sServerSocket);

	free(pbBuffer);

	DeinitializeWSA();
}


VOID
SocketWrapperWithCheckSumTest()
{
	InitializeWSA();

	HANDLE hThreads[2] = { 0 };

	g_hFirstReadEvent = CreateEvent(
		NULL, FALSE, FALSE, NULL
	);

	hThreads[0] = CreateThread(
		NULL, 0,
		ServerWithCheckSumTestRoutine,
		NULL, 0, NULL
	);

	hThreads[1] = CreateThread(
		NULL, 0,
		ClientWithCheckSumTestRoutine,
		NULL, 0, NULL
	);

	WaitForMultipleObjects(2, hThreads, TRUE, INFINITE);

	DeinitializeWSA();
}