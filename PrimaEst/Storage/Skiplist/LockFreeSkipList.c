#include "../../global.h"

/**
 * ������� ���� ������ � ����������
 * @param[in] dwHeight	������
 * @param[in] pKey		����
 * @param[in] pValue	��������
 * @return				����
 */
PSLockFreeSkipListNode
CreateLockFreeSkipListNode(
	DWORD		dwHeight,
	PVOID		pKey,
	PVOID	    pValue);


/**
 * ������� ������ � ����������
 * @param[in] pfComparator		����������
 * @param[in] pfEraser			������� �������� ����
 * @param[in] pfValueChanger	������� ��������� �������� ����
 * @return ��������� ������
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
		LogError("[LockFreeSkipList] ������ ��������� ������");
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
 * �������� ���� ����-�������� � ������
 * ���� ���� ��� ���� � ���������, �� ������ ��������
 * @param[in] psSkipList	��������� ������
 * @param[in] pKey			����
 * @param[in] pValue		��������
 * @return ����������� ����
 */
PSLockFreeSkipListNode
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
				psSkipList->pfValueChanger(&psNextNode->pKey, pKey);
				return psNextNode;
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

	DWORD dwRndHeight = RandomHeight();
	psFoundNode = CreateLockFreeSkipListNode(
		dwRndHeight, pKey, pValue);
	if (!psFoundNode)
	{
		LogError("[LockFreeSkipList] ������ ��������� ������");
		return NULL;
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

	return psFoundNode;
}

/**
 * ����� ���� � ������
 * @param[in] psSkipList	��������� ������
 * @param[in] pKey		����
 * @return ��������� ����
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
 * ������� ��� �������� �� ������
 * @param[in] psSkipList ��������� ������
 */
VOID
LockFreeSkipListClear(
	PSLockFreeSkipList	psSkipList
)
{
	PSLockFreeSkipListNode psIter = psSkipList->psNode->pHead[0];
	for (DWORD dxIdx = 0; dxIdx < psSkipList->dwCount; dxIdx++)
	{
		PSLockFreeSkipListNode psDelete = psIter;
		psIter = psIter->pHead[0];

		psSkipList->pfEraser(psDelete);
		free(psDelete);
	}
}

/**
 * ���������� ��� ������� ������� ����������
 * @param[in] psSkipList ��������� ������
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
 * ������� ���� ������ � ����������
 * @param[in] dwHeight	������
 * @param[in] pKey		����
 * @param[in] pValue	��������
 * @return				����
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
		LogError("[SkipList] ������ ��������� ������");
		return NULL;
	}

	psSkipListNode->pKey = pKey;
	psSkipListNode->pValue = pValue;

	return psSkipListNode;
}


/**
 * �������� ��� ��������
 * @param[in] psSkipList	��������� ������
 * @param[out] pdwSize		���������� ���������
 * @return					������
 */
PSLockFreeSkipListNode*
LockFreeSkipListGetAll(
	PSLockFreeSkipList	psSkipList,
	PDWORD				pdwSize
)
{
	*pdwSize = psSkipList->dwCount;
	return psSkipList->psNode->pHead;
}