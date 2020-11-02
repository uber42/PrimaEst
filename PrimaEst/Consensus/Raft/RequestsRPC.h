/**
 * @file RPCDefinitions.h
 *
 * @author Pavel Chursin
 * @date Oct 2020
 */

#ifndef RPC_REQUESTS_H
#define RPC_REQUESTS_H

/**
* Структура оправляемая лидером для репликации записей
*/
typedef struct _SAppendEntriesRequest
{
	/** Эпоха лидера */
	UINT64	ulLeaderTerm;

	/** ИД лидера */
	DWORD	dwLeaderID;

	/** Индекс предыдущей записи лога */
	UINT64	ulPrevLogIndex;

	/** Эпоха предыдущей записи лога */
	UINT64	ulPrevLogTerm;

	/** Индекс самой старшей закоммиченной записи */
	UINT64	ulLeaderCommit;

	/** Размер записей */
	DWORD	dwEntriesSize;

	/** Записи */
	BYTE	sEntries[0];
} SAppendEntriesRequest, *PSAppendEntriesRequest;

/**
* Структура отправляемая кандидатом для начала выборов
*/
typedef struct _SStartVoteRequest
{
	/** Эпоха кандидата */
	UINT64	ulCandidateTerm;

	/** ИД кандидата */
	DWORD	dwCandidateID;
	
	/** Индекс последней записи в логе */
	UINT64	ulLastLogIndex;

	/** Эпоха последней записи в логе */
	UINT64	ilLastLogTerm;
} StartVoteRequest, *PStartVoteRequest;




#endif