/**
 * @file TimerTest.c
 *
 * @author Pavel Chursin
 * @date Nov 2020
 */

#include "../global.h"
#include <time.h>

#define EPS 25

VOID
TimerSetTest()
{
	MESSAGE_CHANNEL hChannel;
	BOOL bResult = CreateMessageChannel(&hChannel);
	assert(bResult);

	CHANNEL_TIMER hTimer;

	bResult = CreateChannelTimer(
		&hTimer, hChannel, 1);
	assert(bResult);


	UINT64 lTime = clock();
	bResult = SetChannelTimer(hTimer, 1000);
	assert(bResult);

	bResult = FALSE;
	DWORD dwSize = 0;
	bResult = ReceiveMessageChannel(
		hChannel,
		&bResult,
		&dwSize);
	assert(bResult);

	lTime = clock() - lTime;

	assert(bResult);
	assert(dwSize == sizeof(BOOL));

	assert(lTime + EPS > 1000 && lTime - EPS < 1000);

	DeleteChannelTimer(hTimer);
	CloseMessageChannel(hChannel);
}

VOID
TimerCancelTest()
{
	MESSAGE_CHANNEL hChannel;
	BOOL bResult = CreateMessageChannel(&hChannel);
	assert(bResult);

	CHANNEL_TIMER hTimer;

	bResult = CreateChannelTimer(
		&hTimer, hChannel, 1);
	assert(bResult);


	UINT64 lTime = clock();
	bResult = SetChannelTimer(hTimer, 1000);
	assert(bResult);

	ResetChannelTimer(hTimer);

	DeleteChannelTimer(hTimer);
	CloseMessageChannel(hChannel);
}