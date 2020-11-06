/**
 * @file SkipList.c
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
PSSkipListNode
CreateSkipListNode(
	DWORD		dwHeight,
	PVOID		pKey,
	PVOID	    pValue);

/**
 * Создать узел списка с пропусками
 * @param[in] dwHeight	Высота
 * @param[in] pKey		Ключ
 * @param[in] pValue	Значение
 * @return				Узел
 */ 
PSSkipListNode
CreateSkipListNode(
	DWORD		dwHeight,
	PVOID		pKey,
	PVOID	    pValue
)
{
	DWORD dwSize = sizeof(SSkipListNode) + dwHeight * sizeof(SList);
	PSSkipListNode psSkipListNode = (PSSkipListNode)malloc(dwSize);
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
 * Создать список с пропусками
 * @param[in] pfComparator		Компаратор
 * @param[in] pfEraser			Функция удаления узла
 * @param[in] pfValueChanger	Функция изменения значения узла
 * @return Созданный список
 */
PSSkipList
CreateSkipList(
	FSkipListComp*				pfComparator,
	FSkipListNodeEraser*		pfEraser,
	FSkipListNodeValueChanger*	pfValueChanger
)
{
	PSSkipList psSkipList = (PSSkipList)malloc(sizeof(SSkipList));
	if (psSkipList == NULL)
	{
		LogError("[SkipList] Ошибка выделения памяти");
		return NULL;
	}

	psSkipList->dwHeight = 1;
	psSkipList->dwCount = 0;
	psSkipList->pfEraser = pfEraser;
	psSkipList->pfValueChanger = pfValueChanger;
	psSkipList->pfComparator = pfComparator;

	for (size_t i = 0; i < ARRAYSIZE(psSkipList->pHead); i++)
	{
		ListHeadInit(&psSkipList->pHead[i]);
	}

	return psSkipList;
}

/**
 * Добавить пару ключ-значение в список
 * Если ключ уже есть в структуре, то меняет значение
 * @param[in] psSkipList	Экземпляр списка
 * @param[in] pKey			Ключ
 * @param[in] pValue		Значение
 * @return					Добавленный узел
 */
PSSkipListNode
SkipListSet(
	PSSkipList	psSkipList,
	PVOID		pKey,
	PVOID		pValue
)
{
	PSList sListInsertion[SKIP_LIST_MAX_HEIGHT] = { 0 };

	DWORD dwHeight = RandomHeight();
	if (dwHeight > psSkipList->dwHeight)
	{
		psSkipList->dwHeight = dwHeight;
	}

	PSSkipListNode psSkipListNode = CreateSkipListNode(dwHeight, pKey, pValue);
	if (psSkipListNode == NULL)
	{
		return NULL;
	}

	INT dwIdx = (INT)psSkipList->dwHeight - 1;
	PSList psListCurrent = &psSkipList->pHead[dwIdx];
	PSList psListEnd = &psSkipList->pHead[dwIdx];

	while (dwIdx >= 0)
	{
		psListCurrent = psListCurrent->pFlink;
		for (; psListCurrent != psListEnd;
			psListCurrent = psListCurrent->pFlink)
		{
			PSSkipListNode psSkipListCurrentNode = CONTAINING_RECORD(
				psListCurrent,
				SSkipListNode,
				pLink[dwIdx]
			);

			int nCompareResult = psSkipList->pfComparator(
				psSkipListCurrentNode->pKey, pKey);
			if (nCompareResult > 0)
			{
				psListEnd = &psSkipListCurrentNode->pLink[dwIdx];
				break;
			}
			else if (nCompareResult == 0)
			{
				free(psSkipListNode);

				psSkipList->pfValueChanger(&psSkipListCurrentNode->pValue, pValue);
				return psSkipListCurrentNode;
			}
		}

		psListCurrent = psListEnd->pBlink;
		if ((DWORD)dwIdx < dwHeight)
		{
			sListInsertion[dwIdx] = psListCurrent;
		}

		psListCurrent--;
		psListEnd--;

		dwIdx--;
	}

	for (DWORD dwIdx = 0; dwIdx < dwHeight; dwIdx++)
	{
		ListAdd(&psSkipListNode->pLink[dwIdx],
			sListInsertion[dwIdx]);
	}
	
	psSkipList->dwCount++;

	return psSkipListNode;
}

/**
 * Найти ключ в списке
 * @param[in] psSkipList	Экземпляр списка
 * @param[in] pKey			Ключ
 * @return					Найденный узел
 */
PSSkipListNode
SkipListFind(
	PSSkipList	psSkipList,
	PVOID		pKey
)
{
	INT dwIdx = (INT)psSkipList->dwHeight - 1;
	PSList psListCurrent = &psSkipList->pHead[dwIdx];
	PSList psListEnd = &psSkipList->pHead[dwIdx];

	while (dwIdx >= 0)
	{
		psListCurrent = psListCurrent->pFlink;
		for (; psListCurrent != psListEnd;
			psListCurrent = psListCurrent->pFlink)
		{
			PSSkipListNode psSkipListCurrentNode = CONTAINING_RECORD(
				psListCurrent,
				SSkipListNode,
				pLink[dwIdx]
			);

			int nCompareResult = 
				psSkipList->pfComparator(psSkipListCurrentNode->pKey, pKey);

			if (nCompareResult > 0)
			{
				psListEnd = &psSkipListCurrentNode->pLink[dwIdx];
				break;
			}
			else if (nCompareResult == 0)
			{
				return psSkipListCurrentNode;
			}
		}

		psListCurrent = psListEnd->pBlink;

		psListCurrent--;
		psListEnd--;

		dwIdx--;
	}

	return NULL;
}

/**
 * Удалить узел в списке по ключу
 * @param[in] psSkipList	Экземпляр списка
 * @param[in] pKey			Ключ
 * @return					Удален ли узел ? (Может быть не удален только в случае если его нет)
 */
BOOL
SkipListRemove(
	PSSkipList	psSkipList,
	PVOID		pKey
)
{
	INT dwIdx = (INT)psSkipList->dwHeight - 1;
	PSList psListCurrent = &psSkipList->pHead[dwIdx];
	PSList psListEnd = &psSkipList->pHead[dwIdx];

	while (dwIdx >= 0)
	{
		psListCurrent = psListCurrent->pFlink;
		PSList psListSafeIterator = psListCurrent->pFlink;

		for (; psListCurrent != psListEnd;
			psListCurrent = psListSafeIterator,
			psListSafeIterator = psListCurrent->pFlink)
		{
			PSSkipListNode psSkipListCurrentNode = CONTAINING_RECORD(
				psListCurrent,
				SSkipListNode,
				pLink[dwIdx]
			);

			int nComparationResult = psSkipList->pfComparator(
				psSkipListCurrentNode->pKey, pKey);

			if (nComparationResult > 0)
			{
				psListEnd = &psSkipListCurrentNode->pLink[dwIdx];
				break;
			}
			else if (nComparationResult == 0)
			{
				for (INT dwCurrentHeight = 0; dwCurrentHeight <= dwIdx; dwCurrentHeight++)
				{
					ListNodeDelete(&psSkipListCurrentNode->pLink[dwCurrentHeight]);
					if (ListIsEmpty(&psSkipList->pHead[dwCurrentHeight]))
					{
						psSkipList->dwHeight--;
					}
				}

				psSkipList->pfEraser(psSkipListCurrentNode);
				free(psSkipListCurrentNode);
				psSkipList->dwCount--;

				return TRUE;
			}
		}

		psListCurrent = psListEnd->pBlink;

		psListCurrent--;
		psListEnd--;

		dwIdx--;
	}

	return FALSE;
}

/**
 * Распечатать список с иерархией уровней
 * @param[in] psSkipList			Экземпляр списка
 * @param[in] pFSkipListPrinter		Функция описывающая вывод для
 */
VOID
SkipListPrint(
	PSSkipList			psSkipList,
	FSkipListPrinter*	pFSkipListPrinter
)
{
	INT dwIdx = (INT)psSkipList->dwHeight - 1;
	PSList psListCurrent = &psSkipList->pHead[dwIdx];
	PSList psListEnd = &psSkipList->pHead[dwIdx];

	while (dwIdx >= 0)
	{
		psListCurrent = psListCurrent->pFlink;

		if (psListCurrent != psListEnd)
		{
			PSSkipListNode psSkipListCurrentNode = CONTAINING_RECORD(
				psListCurrent,
				SSkipListNode,
				pLink[dwIdx]
			);

			pFSkipListPrinter(
				dwIdx, TRUE,
				psSkipListCurrentNode->pKey,
				psSkipListCurrentNode->pValue);

			psListCurrent = psListCurrent->pFlink;
		}

		for (; psListCurrent != psListEnd;
			psListCurrent = psListCurrent->pFlink)
		{
			PSSkipListNode psSkipListCurrentNode = CONTAINING_RECORD(
				psListCurrent,
				SSkipListNode,
				pLink[dwIdx]
			);

			pFSkipListPrinter(
				dwIdx, FALSE,
				psSkipListCurrentNode->pKey,
				psSkipListCurrentNode->pValue);
		}

		psListCurrent--;
		psListEnd--;

		dwIdx--;
	}
}

/**
 * Получить список всех элементов структуры
 * @param[in] psSkipList Экземпляр списка
 * @return				 Список всех элементов
 */
PSList
SkipListGetAll(
	PSSkipList			psSkipList
)
{
	return &psSkipList->pHead[0];
}

/**
 * Удалить все значения из списка
 * @param[in] psSkipList Экземпляр списка
 */
VOID
SkipListClear(
	PSSkipList			psSkipList
)
{
	PSList psListCurrent = &psSkipList->pHead[0];
	PSList psListEnd = &psSkipList->pHead[0];

	PSList psListSafeIterator = NULL;

	for (psListCurrent = psListCurrent->pFlink,
		psListSafeIterator = psListCurrent->pFlink;
		psListCurrent != psListEnd;
		psListCurrent = psListSafeIterator,
		psListSafeIterator = psListCurrent->pFlink)
	{
		PSSkipListNode psSkipListCurrentNode = CONTAINING_RECORD(
			psListCurrent,
			SSkipListNode,
			pLink
		);

		psSkipList->pfEraser(psSkipListCurrentNode);

		free(psSkipListCurrentNode);
	}

	for (size_t i = 0; i < psSkipList->dwHeight; i++)
	{
		ListHeadInit(&psSkipList->pHead[i]);
	}

	psSkipList->dwHeight = 1;
	psSkipList->dwCount = 0;
}

/**
 * Освободить все ресурсы занятые структурой
 * @param[in] psSkipList Экземпляр списка
 */
VOID
SkipListClose(
	PSSkipList			psSkipList
)
{
	SkipListClear(psSkipList);
	free(psSkipList);
}

