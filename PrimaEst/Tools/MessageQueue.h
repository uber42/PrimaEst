/**
 * @file MessageQueue.h
 *
 * @author Pavel Chursin
 * @date Oct 2020
 */

#ifndef MESSAGE_QUEUE_H
#define MESSAGE_QUEUE_H

#include "List.h"

#define MESSAGE_QUEUE_STATUS_OK					0xFFFF0000
#define MESSAGE_QUEUE_BUFFER_OVERFLOW			(MESSAGE_QUEUE_STATUS_OK + 1)
#define MESSAGE_QUEUE_EVENT_ERROR				(MESSAGE_QUEUE_STATUS_OK + 2)
#define MESSAGE_QUEUE_ALLOC_ERROR				(MESSAGE_QUEUE_STATUS_OK + 3)
#define MESSAGE_QUEUE_CLOSED					(MESSAGE_QUEUE_STATUS_OK + 4)
#define MESSAGE_QUEUE_BUFFER_NOT_ALLOW			(MESSAGE_QUEUE_STATUS_OK + 5)
#define MESSAGE_QUEUE_BAD_HANDLER				(MESSAGE_QUEUE_STATUS_OK + 6)

/** ��������� ������� ��������� */
typedef PVOID MESSAGE_QUEUE;

/** ������ ��������� */
typedef struct _SMessageData
{
	/** ������ ��������� */
	DWORD	dwBufferSize;

	/** ����� ��������� */
	PVOID	pBuffer;
} SMessageData, * PSMessageData;

/**
* ������� ������� ���������
* @param[out]		ppsMessageQueue		��������� ������� ���������
* @param@param[in]	dwMessagesMaxCount	������������ ���������� ��������� � �������
* @param@param[in]	dwMessageBufferSize	������ ������������� ���������
* @param[out]		dwError				������
* @return ��������� ������ �������.
*/
_Success_(return != FALSE)
BOOL
InitializeMessageQueue(
	MESSAGE_QUEUE* hMessageQueue,
	DWORD		  dwMessagesMaxCount,
	DWORD		  dwMessageBufferSize,
	PDWORD		  dwError
);

/**
* ������� ������� ���������
* @param[inout]   hMessageQueue		��������� ������� ���������
* @return							��������� ������ �������
*/
VOID
DeinitializeMessageQueue(
	MESSAGE_QUEUE hMessageQueue
);

/**
* �������� ��������� � �������
* @param@param[in]  hMessageQueue	��������� ������� ���������
* @param[out]		sMessageData	���������
* @param[out]		dwError		    ������
* @return							��������� ������ �������
*/
BOOL
MessageQueuePush(
	MESSAGE_QUEUE hMessageQueue,
	SMessageData  sMessageData,
	PDWORD		  dwError
);

/**
* �������� �� ������� ������ �������
* @param@param[in]  hMessageQueue		��������� ������� ���������
* @param[out]		pMessageData		������
* @param[out]		dwError		        ������
* @return ��������� ������ �������
*/
BOOL
MessageQueuePop(
	MESSAGE_QUEUE hMessageQueue,
	PSMessageData pMessageData,
	PDWORD		  dwError
);

/**
* �������� ������� ���������
* @param[out]   hMessageQueue		��������� ������� ���������
*/
VOID
MessageQueueClear(
	MESSAGE_QUEUE hMessageQueue
);

#endif // MESSAGE_QUEUE_H