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
 * Создать список с пропусками
 * @param[in] pfComparator		Компаратор
 * @param[in] pfEraser			Функция удаления узла
 * @param[in] pfValueChanger	Функция изменения значения узла
 * @return Созданный список
 */
PSLockFreeSkipList
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

	psLockFreeSkipList->dwCount = 0;
	psLockFreeSkipList->dwHeight = 1;
	psLockFreeSkipList->pfComparator = pfComparator;
	psLockFreeSkipList->pfEraser = pfEraser;
	psLockFreeSkipList->pfValueChanger = pfValueChanger;
	psLockFreeSkipList->psNode = CreateLockFreeSkipListNode(SKIP_LIST_MAX_HEIGHT, NULL, NULL);
	if (!psLockFreeSkipList->psNode)
	{
		free(psLockFreeSkipList);
		return NULL;
	}

	for (DWORD dwIdx = 0; dwIdx < SKIP_LIST_MAX_HEIGHT; dwIdx++)
	{
		psLockFreeSkipList->psNode->pHead[dwIdx] = NULL;
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
	PSLockFreeSkipListNode sListInsertion[SKIP_LIST_MAX_HEIGHT] = { 0 };

	PSLockFreeSkipListNode psFoundNode = NULL;
	PSLockFreeSkipListNode psNode = psSkipList->psNode;
	DWORD dwHeight = psSkipList->dwHeight - 1;

	while (TRUE)
	{
		_ReadWriteBarrier();
		PSLockFreeSkipListNode psNextNode = psNode->pHead[dwHeight];

		INT nCompare = 0;
		BOOL bCompare = psNextNode != NULL && 
			(nCompare = psSkipList->pfComparator(pKey, psNextNode->pKey)) >= 0;
		if (bCompare)
		{
			if (!nCompare)
			{
				psSkipList->pfValueChanger(&psNextNode->pValue, pValue);
				return TRUE;
			}
			psNode = psNextNode;
		}
		else
		{
			sListInsertion[dwHeight] = psNode;
			if (dwHeight == 0)
			{
				psFoundNode = psNextNode;
				break;
			}
			
			dwHeight--;
		}
	}

	LONG dwRndHeight = RandomHeight();
	psFoundNode = CreateLockFreeSkipListNode(
		dwRndHeight, pKey, pValue);
	if (!psFoundNode)
	{
		LogError("[LockFreeSkipList] Ошибка выделения памяти");
		return FALSE;
	}

	if (dwRndHeight > psSkipList->dwHeight)
	{
		for (DWORD dwHeightIdx = psSkipList->dwHeight;
			dwHeightIdx < dwRndHeight; dwHeightIdx++)
		{
			sListInsertion[dwHeightIdx] = psSkipList->psNode;
		}

		psSkipList->dwHeight = dwRndHeight;
	}

	for (DWORD dwIdx = 0; dwIdx < dwRndHeight; dwIdx++)
	{
		psFoundNode->pHead[dwIdx] = sListInsertion[dwIdx]->pHead[dwIdx];

		sListInsertion[dwIdx]->pHead[dwIdx] = psFoundNode;
		_ReadWriteBarrier();
	}

	InterlockedAdd64(&psSkipList->dwCount, 1);

	return TRUE;
}

/**
 * Найти ключ в списке
 * @param[in] psSkipList	Экземпляр списка
 * @param[in] pKey		Ключ
 * @return Найденный узел
 */
PSLockFreeSkipListNode
LockFreeSkipListFind(
	PSLockFreeSkipList	psSkipList,
	PVOID				pKey
)
{
	PSLockFreeSkipListNode psNode = psSkipList->psNode;
	DWORD dwHeight = psSkipList->dwHeight - 1;

	while (TRUE)
	{
		_ReadWriteBarrier();
		PSLockFreeSkipListNode psNextNode = psNode->pHead[dwHeight];

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
				return !nCompare ?
					psNextNode : NULL;
			}

			dwHeight--;
		}
	}

	return NULL;
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
	volatile PSLockFreeSkipListNode psIter = psSkipList->psNode->pHead[0];
	for (DWORD dxIdx = 0; dxIdx < psSkipList->dwCount; dxIdx++)
	{
		volatile PSLockFreeSkipListNode psDelete = psIter;
		psIter = psIter->pHead[0];

		psSkipList->pfEraser(psDelete);
		free(psDelete);
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
		(dwHeight - 1) * sizeof(volatile PSLockFreeSkipListNode);
	PSLockFreeSkipListNode psSkipListNode = (PSLockFreeSkipListNode)malloc(dwSize);
	if (psSkipListNode == NULL)
	{
		LogError("[SkipList] Ошибка выделения памяти");
		return NULL;
	}

	psSkipListNode->pKey = pKey;
	psSkipListNode->pValue = pValue;

	return psSkipListNode;
}


/**
 * Получить все элементы
 * @param[in] psSkipList	Экземпляр списка
 * @param[out] pdwSize		Количество элементов
 * @return					Список
 */
PSLockFreeSkipListNode*
LockFreeSkipListGetAll(
	PSLockFreeSkipList	psSkipList,
	PDWORD				pdwSize
)
{
	*pdwSize = (DWORD)psSkipList->dwCount;
	return psSkipList->psNode->pHead;
}