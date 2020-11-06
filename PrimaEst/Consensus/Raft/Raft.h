#ifndef RAFT_H
#define RAFT_H


typedef struct _SRaftServer
{
	/** Текущая эпоха */
	UINT64	ulCurrentTerm;

	/** ИД узела которому был отдан голос в текущей эпохе */
	DWORD	dwVotedFor;

	/** ВРЕМЕННЫЙ ЛОГ */
	BYTE	pbLog[16384];

	/** Индекс последней зафиксированной записи */
	volatile UINT64	ulCommitIndex;

	/** Индекс последней принятой записи */
	volatile UINT64	ulLastApplied;


	/** Таймаут до выборов */
	DWORD	dwElectionTimeout;
} SRaftServer, *PSRaftServer;


BOOL
CreateRaftServer(
	PSRaftServer	psRaftServer
);

#endif