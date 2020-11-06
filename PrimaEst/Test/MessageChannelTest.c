#include "../global.h"

#define TEST_MC_ID 1244

BYTE	g_bTestBuffer[] = { 0xD1, 0x22, 0x11, 0x55, 0x55, 0xDF, 0xA4 };
DWORD	g_dwTestBufferSize = sizeof(g_bTestBuffer);

typedef enum _EMCTestThreadId
{
	ADD_1_TEST,
	SUB_1_TEST,
	ADD_5_TEST,
	SUB_5_TEST,
} EMCTestThreadId, *PEMCTestThreadId;

typedef struct _SMTTestArg
{
	MESSAGE_CHANNEL hChannel;

	EMCTestThreadId	eMsgId;
} SMTTestArg, *PSMTTestArg;

static
DWORD
WINAPI
ReceiveThreadRoutine(
	LPVOID	lpThreadParameter
)
{
	MESSAGE_CHANNEL hMessageChannel = lpThreadParameter;

	DWORD dwSize = 0;
	BYTE bBuffer[1024];
	DWORD dwId = ReceiveMessageChannel(
		hMessageChannel,
		bBuffer,
		&dwSize);
	assert(dwId == TEST_MC_ID);

	assert(!memcmp(bBuffer, g_bTestBuffer, g_dwTestBufferSize));

	return 0;
}


static
DWORD
WINAPI
SendThreadRoutine(
	LPVOID	lpThreadParameter
)
{
	MESSAGE_CHANNEL hMessageChannel = lpThreadParameter;

	BOOL bResult = SendMessageChannel(
		hMessageChannel,
		TEST_MC_ID,
		g_bTestBuffer,
		g_dwTestBufferSize,
		EMCBA_STACK_ALLOCATED);
	assert(bResult);
	
	return 0;
}

static
DWORD
WINAPI
MTSendThreadRoutine(
	LPVOID	lpThreadParameter
)
{
	PSMTTestArg arg = lpThreadParameter;

	BOOL bResult = SendMessageChannel(
		arg->hChannel,
		arg->eMsgId,
		g_bTestBuffer,
		g_dwTestBufferSize,
		EMCBA_STACK_ALLOCATED);

	return 0;
}

static
DWORD
WINAPI
MTReceiveThreadRoutine(
	LPVOID	lpThreadParameter
)
{
	MESSAGE_CHANNEL hMessageChannel = lpThreadParameter;

	int nResult = 0;

	while (TRUE)
	{
		DWORD dwSize = 0;
		BYTE bBuffer[1024];
		DWORD dwId = ReceiveMessageChannel(
			hMessageChannel,
			bBuffer,
			&dwSize);
		if (dwId == MESSAGE_CHANNEL_BAD_RESULT)
		{
			break;
		}

		assert(!memcmp(bBuffer, g_bTestBuffer, g_dwTestBufferSize));

		switch (dwId)
		{
		case ADD_1_TEST:
			nResult++;
			break;
		case ADD_5_TEST:
			nResult += 5;
			break;
		case SUB_1_TEST:
			nResult--;
			break;
		case SUB_5_TEST:
			nResult -= 5;
			break;
		default:
			assert(FALSE);
		}
	}

	assert(nResult == 10);

	return 0;
}


VOID
MCReceiveSendTest()
{
	MESSAGE_CHANNEL hMessageChannel;
	BOOL bResult = CreateMessageChannel(&hMessageChannel);
	assert(bResult);

	HANDLE hThread = CreateThread(
		NULL, 0,
		ReceiveThreadRoutine,
		hMessageChannel,
		0, NULL);
	assert(hThread != NULL);

	Sleep(100);

	bResult = SendMessageChannel(
		hMessageChannel,
		TEST_MC_ID,
		g_bTestBuffer,
		g_dwTestBufferSize,
		EMCBA_STACK_ALLOCATED);
	assert(bResult);

	WaitForSingleObject(hThread, INFINITE);

	CloseHandle(hThread);

	CloseMessageChannel(hMessageChannel);
}

VOID
MCSendReceiveTest()
{
	MESSAGE_CHANNEL hMessageChannel;
	BOOL bResult = CreateMessageChannel(&hMessageChannel);
	assert(bResult);

	HANDLE hThread = CreateThread(
		NULL, 0,
		SendThreadRoutine,
		hMessageChannel,
		0, NULL);
	assert(hThread != NULL);

	Sleep(100);


	DWORD dwSize = 0;
	BYTE bBuffer[1024];
	DWORD dwId = ReceiveMessageChannel(
		hMessageChannel,
		bBuffer,
		&dwSize);
	assert(dwId == TEST_MC_ID);

	assert(!memcmp(bBuffer, g_bTestBuffer, g_dwTestBufferSize));

	WaitForSingleObject(hThread, INFINITE);

	CloseHandle(hThread);

	CloseMessageChannel(hMessageChannel);
}

VOID
MCMultiThreadTest()
{
	MESSAGE_CHANNEL hMessageChannel;
	BOOL bResult = CreateMessageChannel(&hMessageChannel);
	assert(bResult);

	SMTTestArg arg1 =
	{
		.eMsgId = ADD_1_TEST,
		.hChannel = hMessageChannel
	};
	HANDLE hThread = CreateThread(
		NULL, 0,
		MTSendThreadRoutine,
		&arg1,
		0, NULL);
	assert(hThread != NULL);
	CloseHandle(hThread);

	SMTTestArg arg2 =
	{
		.eMsgId = SUB_1_TEST,
		.hChannel = hMessageChannel
	};
	hThread = CreateThread(
		NULL, 0,
		MTSendThreadRoutine,
		&arg2,
		0, NULL);
	assert(hThread != NULL);
	CloseHandle(hThread);

	SMTTestArg arg3 =
	{
		.eMsgId = ADD_5_TEST,
		.hChannel = hMessageChannel
	};
	hThread = CreateThread(
		NULL, 0,
		MTSendThreadRoutine,
		&arg3,
		0, NULL);
	assert(hThread != NULL);
	CloseHandle(hThread);

	SMTTestArg arg4 =
	{
		.eMsgId = ADD_5_TEST,
		.hChannel = hMessageChannel
	};
	hThread = CreateThread(
		NULL, 0,
		MTSendThreadRoutine,
		&arg4,
		0, NULL);
	assert(hThread != NULL);
	CloseHandle(hThread);

	hThread = CreateThread(
		NULL, 0,
		MTReceiveThreadRoutine,
		hMessageChannel,
		0, NULL);
	assert(hThread != NULL);

	Sleep(1000);
	CloseMessageChannel(hMessageChannel);

	WaitForSingleObject(hThread, INFINITE);
	CloseHandle(hThread);
}