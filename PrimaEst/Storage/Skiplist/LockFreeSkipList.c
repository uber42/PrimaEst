/**
 * @file LockFreeSkipList.c
 *
 * @author Pavel Chursin
 * @date Oct 2020
 */

#include "../../global.h"

 /**
  * Создать узел списка с пропусками
  * @param[in] dwHeight	Высота
  * @param[in] pKey		Ключ
  * @param[in] pValue	Значение
  * @return				Узел
  */
PSLockFreeSkipListNode
CreateLockFreeSkipListNode(
	DWORD		dwHeight,
	PVOID		pKey,
	PVOID	    pValue
);

/**
 * Найти ключ в списке с поиском предшественников и узлов для вставки
 * @param[in] psSkipList	Экземпляр списка
 * @param[in] psPredsNodes	Узлы предшественники
 * @param[in] psSuccsNodes	Узлы для вставки
 * @param[out] psFoundNode	Найденный узел
 * @param[in] pKey			Ключ
 * @return Найденный узел
 */
BOOL
InternalLockFreeSkipListFind(
	PSLockFreeSkipList					psSkipList,
	PSLockFreeSkipListNode*				psPredsNodes,
	PSLockFreeSkipListNode*				psSuccsNodes,
	PSLockFreeSkipListNode*				psFoundNode,
	PVOID								pKey
);

/**
 * Создать Lock-Free список с пропусками
 * @param[in] pfComparator		Компаратор
 * @param[in] pfEraser			Функция удаления узла
 * @param[in] pfValueChanger	Функция изменения значения узла
 * @return Созданный список
 */
PSSkipList
CreateLockFreeSkipList(
	FSkipListComp*				pfComparator,
	FSkipListNodeEraser*		pfEraser,
	FSkipListNodeValueChanger*	pfValueChanger
)
{
	PSLockFreeSkipList psLockFreeSkipList = malloc(sizeof(SLockFreeSkipList));
	if (!psLockFreeSkipList)
	{
		LogError("[LockFreeSkipList] Ошибка выделения памяти");
		return NULL;
	}

	psLockFreeSkipList->pfEraser = pfEraser;
	psLockFreeSkipList->pfValueChanger = pfValueChanger;
	psLockFreeSkipList->pfComparator = pfComparator;

	psLockFreeSkipList->psHead = CreateLockFreeSkipListNode(SKIP_LIST_MAX_HEIGHT, LLONG_MIN, NULL);
	if (!psLockFreeSkipList->psHead)
	{
		LogError("[LockFreeSkipList] Ошибка выделения памяти");

		free(psLockFreeSkipList);
		return NULL;
	}

	psLockFreeSkipList->psTail = CreateLockFreeSkipListNode(SKIP_LIST_MAX_HEIGHT, LLONG_MAX, NULL);
	if (!psLockFreeSkipList->psTail)
	{
		LogError("[LockFreeSkipList] Ошибка выделения памяти");

		free(psLockFreeSkipList->psHead);
		free(psLockFreeSkipList);
		return NULL;
	}

	for (DWORD dwIdx = 0; dwIdx < SKIP_LIST_MAX_HEIGHT; dwIdx++)
	{
		psLockFreeSkipList->psHead->pNext[dwIdx] = psLockFreeSkipList->psTail;
	}

	return psLockFreeSkipList;
}

/**
 * Добавить пару ключ-значение в список
 * Если ключ уже есть в структуре, то меняет значение
 * @param[in] psSkipList	Экземпляр списка
 * @param[in] pKey			Ключ
 * @param[in] pValue		Значение
 * @return Добавленный узел
 */
BOOL
LockFreeSkipListSet(
	PSLockFreeSkipList	psSkipList,
	PVOID				pKey,
	PVOID				pValue
)
{
	DWORD dwRndHeight = RandomHeight();

	PSLockFreeSkipListNode psPredecessors[SKIP_LIST_MAX_HEIGHT];
	PSLockFreeSkipListNode psSuccessors[SKIP_LIST_MAX_HEIGHT];

	do 
	{
		PSLockFreeSkipListNode psFoundNode = NULL;
		BOOL bFound = InternalLockFreeSkipListFind(
			psSkipList, psPredecessors, psSuccessors, &psFoundNode, pKey);

		if (bFound)
		{
			psSkipList->pfValueChanger(&psFoundNode->pValue, pValue);
			return TRUE;
		}

		PSLockFreeSkipListNode psNode = CreateLockFreeSkipListNode(
			dwRndHeight,
			pKey, pValue);

		for (DWORD dwLevel = 0; dwLevel < dwRndHeight; dwLevel++)
		{
			psNode->pNext[dwLevel] = *psSuccessors;
		}

		if (InterlockedCompareExchangePointer(
			&(*psPredecessors)->pNext[0],
			psNode, *psSuccessors
		) != *psSuccessors)
		{
			free(psNode);
			continue;
		}

		for (DWORD dwLevel = 1; dwLevel < dwRndHeight; dwLevel++)
		{
			while (TRUE)
			{
				PSLockFreeSkipListNode psSuccessor = psSuccessors[dwLevel];
				PSLockFreeSkipListNode psPredecessor = psPredecessors[dwLevel];

				if (psSuccessor != psNode->pNext[dwLevel])
				{
					PSLockFreeSkipListNode psCurrentSuccessor = psNode->pNext[dwLevel];

					if (InterlockedCompareExchangePointer(
						&psNode->pNext[dwLevel],
						psSuccessor, psCurrentSuccessor
						) != psCurrentSuccessor)
					{
						continue;
					}
				}

				if (InterlockedCompareExchangePointer(
					&psPredecessor->pNext[dwLevel],
					psNode, psSuccessor
					) == psSuccessor)
				{
					break;
				}

				memset(psPredecessors, 0, sizeof(psPredecessors));
				memset(psSuccessors, 0, sizeof(psSuccessors));
				InternalLockFreeSkipListFind(psSkipList, psPredecessors, psSuccessors, &psFoundNode, pKey);
			}
		}

		return TRUE;

	} while (TRUE);
}

/**
 * Найти ключ в списке
 * @param[in] psSkipList	Экземпляр списка
 * @param[in] pKey		Ключ
 * @return Найденный узел
 */
BOOL
LockFreeSkipListFind(
	PSLockFreeSkipList	psSkipList,
	PVOID				pKey
)
{
	DWORD dwHeight = SKIP_LIST_MAX_HEIGHT - 1;
	PSLockFreeSkipListNode psNode = psSkipList->psHead;

	while (TRUE)
	{
		_ReadWriteBarrier();
		PSLockFreeSkipListNode psNextNode = psNode->pNext[dwHeight];

		INT nCompare = 0xDEADBEEF;
		BOOL bCompare = psNextNode != NULL &&
			(nCompare = psSkipList->pfComparator(pKey, psNextNode->pKey)) > 0;
		if (bCompare)
		{
			psNode = psNextNode;
		}
		else
		{
			if (dwHeight == 0)
			{
				return !nCompare ? TRUE : FALSE;
			}

			dwHeight--;
		}
	}

	return FALSE;
}

/**
 * Удалить все значения из списка
 * @param[in] psSkipList Экземпляр списка
 */
VOID
LockFreeSkipListClear(
	PSLockFreeSkipList	psSkipList
)
{
	PSLockFreeSkipListNode psCurrentNode = psSkipList->psHead;
	PSLockFreeSkipListNode psNextNode;

	while (psCurrentNode != NULL) 
	{
		psNextNode = psCurrentNode->pNext[0];
		psSkipList->pfEraser(psCurrentNode);
		psCurrentNode = psNextNode;
	}
}

/**
 * Освободить все ресурсы занятые структурой
 * @param[in] psSkipList Экземпляр списка
 */
VOID
LockFreeSkipListClose(
	PSLockFreeSkipList	psSkipList
)
{
	LockFreeSkipListClear(psSkipList);
	free(psSkipList);
}


/**
 * Создать узел списка с пропусками
 * @param[in] dwHeight	Высота
 * @param[in] pKey		Ключ
 * @param[in] pValue	Значение
 * @return				Узел
 */
PSLockFreeSkipListNode
CreateLockFreeSkipListNode(
	DWORD		dwHeight,
	PVOID		pKey,
	PVOID	    pValue)
{
	DWORD dwSize = sizeof(SLockFreeSkipListNode) + 
		(dwHeight) * sizeof(PSLockFreeSkipListNode);
	PSLockFreeSkipListNode psSkipListNode = (PSLockFreeSkipListNode)malloc(dwSize);
	if (psSkipListNode == NULL)
	{
		LogError("[SkipList] Ошибка выделения памяти");
		return NULL;
	}

	psSkipListNode->pKey = pKey;
	psSkipListNode->pValue = pValue;
	psSkipListNode->dwHeight = dwHeight;

	for (DWORD dwIdx = 0; dwIdx < dwHeight; dwIdx++)
	{
		psSkipListNode->pNext[dwIdx] = NULL;
	}

	return psSkipListNode;
}

/**
 * Найти ключ в списке с поиском предшественников и узлов для вставки
 * @param[in] psSkipList	Экземпляр списка
 * @param[in] psPredsNodes	Узлы предшественники
 * @param[in] psSuccsNodes	Узлы для вставки
 * @param[out] psFoundNode	Найденный узел
 * @param[in] pKey			Ключ
 * @return Найденный узел
 */
BOOL
InternalLockFreeSkipListFind(
	PSLockFreeSkipList					psSkipList,
	PSLockFreeSkipListNode*				psPredsNodes,
	PSLockFreeSkipListNode*				psSuccsNodes,
	PSLockFreeSkipListNode*				psFoundNode,
	PVOID								pKey
)
{
	PVOID pCurrentKey = 0;

	PSLockFreeSkipListNode psCurrentNode = NULL;
	PSLockFreeSkipListNode psPredecessorNode = psSkipList->psHead;
	for (INT dwLevel = SKIP_LIST_MAX_HEIGHT - 1; dwLevel >= 0; dwLevel--)
	{
		psCurrentNode = psPredecessorNode->pNext[dwLevel];

		while (TRUE)
		{
			PSLockFreeSkipListNode psSuccessorNode = psCurrentNode->pNext[dwLevel];

			pCurrentKey = psCurrentNode->pKey;

			if (psSkipList->pfComparator(pCurrentKey, pKey) < 0)
			{
				psPredecessorNode = psCurrentNode;
				psCurrentNode = psSuccessorNode;
			}
			else
			{
				break;
			}
		}

		psPredsNodes[dwLevel] = psPredecessorNode;
		psSuccsNodes[dwLevel] = psCurrentNode;
	}

	*psFoundNode = psCurrentNode;

	return pCurrentKey == pKey;
}

/**
 * Вывод списка
 * @param[in] psSkipList Экземпляр списка
 */
VOID
LockFreeSkipListPrint(
	PSLockFreeSkipList	psSkipList
)
{
	for (INT dwHeight = SKIP_LIST_MAX_HEIGHT - 1; dwHeight >= 0; dwHeight--)
	{
		PSLockFreeSkipListNode psNode = psSkipList->psHead;
		while (psNode != psSkipList->psTail)
		{
			printf("%lld ", (LONG64)psNode->pKey);

			psNode = psNode->pNext[dwHeight];
		}
		printf("\n");
	}
}