#ifndef RESPONSES_RPC_H
#define RESPONSES_RPC_H

/**
* Перечисление RPC операций по RAFT
*/
typedef enum _ERPCOperation
{
	/** Запрос на голосование */
	ERPCO_REQUEST_VOTE,

	/** Добавть записи к своему логу */
	ERPCO_APPEND_ENTRIES,

	/** Установить снимок состояния */
	ERPCO_INSTALL_SNAPSHOT
} ERPCOperation, *PERPCOperation;

/**
* Структура оправляемая последователем лидеру
* на запрос репликации записей
*/
typedef struct _SAppendEntriesResponse
{
	/** Текущая эпоха последователя */
	UINT64	ulCurrentTerm;

	/** Содержит ли все записи */
	BOOL	bSuccess;
} SAppendEntriesResponse, *PSAppendEntriesResponse;


/**
* Структура отправляемая последователем кандидату
* во время выборов
*/
typedef struct _SStartVoteResponse
{
	/** Текущая эпоха последователя */
	UINT64	ulCurrentTerm;

	/** Проголосовал ли последователь за этого кандидата */
	UINT64	bVoteGranted;
} SStartVoteResponse, *PSStartVoteResponse;


#endif