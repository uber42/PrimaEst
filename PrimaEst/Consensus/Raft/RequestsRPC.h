/**
 * @file RPCDefinitions.h
 *
 * @author Pavel Chursin
 * @date Oct 2020
 */

#ifndef RPC_REQUESTS_H
#define RPC_REQUESTS_H

/**
* ��������� ����������� ������� ��� ���������� �������
*/
typedef struct _SAppendEntriesRequest
{
	/** ����� ������ */
	UINT64	ulLeaderTerm;

	/** �� ������ */
	DWORD	dwLeaderID;

	/** ������ ���������� ������ ���� */
	UINT64	ulPrevLogIndex;

	/** ����� ���������� ������ ���� */
	UINT64	ulPrevLogTerm;

	/** ������ ����� ������� ������������� ������ */
	UINT64	ulLeaderCommit;

	/** ������ ������� */
	DWORD	dwEntriesSize;

	/** ������ */
	BYTE	sEntries[0];
} SAppendEntriesRequest, *PSAppendEntriesRequest;

/**
* ��������� ������������ ���������� ��� ������ �������
*/
typedef struct _SStartVoteRequest
{
	/** ����� ��������� */
	UINT64	ulCandidateTerm;

	/** �� ��������� */
	DWORD	dwCandidateID;
	
	/** ������ ��������� ������ � ���� */
	UINT64	ulLastLogIndex;

	/** ����� ��������� ������ � ���� */
	UINT64	ilLastLogTerm;
} StartVoteRequest, *PStartVoteRequest;




#endif