#ifndef RESPONSES_RPC_H
#define RESPONSES_RPC_H

/**
* ������������ RPC �������� �� RAFT
*/
typedef enum _ERPCOperation
{
	/** ������ �� ����������� */
	ERPCO_REQUEST_VOTE,

	/** ������� ������ � ������ ���� */
	ERPCO_APPEND_ENTRIES,

	/** ���������� ������ ��������� */
	ERPCO_INSTALL_SNAPSHOT
} ERPCOperation, *PERPCOperation;

/**
* ��������� ����������� �������������� ������
* �� ������ ���������� �������
*/
typedef struct _SAppendEntriesResponse
{
	/** ������� ����� ������������� */
	UINT64	ulCurrentTerm;

	/** �������� �� ��� ������ */
	BOOL	bSuccess;
} SAppendEntriesResponse, *PSAppendEntriesResponse;


/**
* ��������� ������������ �������������� ���������
* �� ����� �������
*/
typedef struct _SStartVoteResponse
{
	/** ������� ����� ������������� */
	UINT64	ulCurrentTerm;

	/** ������������ �� ������������� �� ����� ��������� */
	UINT64	bVoteGranted;
} SStartVoteResponse, *PSStartVoteResponse;


#endif