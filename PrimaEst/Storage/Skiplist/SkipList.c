#include "../../global.h"

/**
 * ������������� ������
 * [Ret] ������
 */
static DWORD
RandomHeight();

/**
 * ������� ���� ������ � ����������
 * [In] dwHeight	������
 * [In] pKey		���� 
 * [In] pValue		��������
 * [Ret] ����
 */
PSSkipListNode
CreateSkipListNode(
	DWORD		dwHeight,
	PVOID		pKey,
	PVOID	    pValue);


/**
 * ������� ���� ������ � ����������
 * [In] dwHeight	������
 * [In] pKey		����
 * [In] pValue		��������
 * [Ret] ����
 */
PSSkipListNode
CreateSkipListNode(
	DWORD		dwHeight,
	PVOID		pKey,
	PVOID	    pValue)
{
	if (pKey == NULL || pValue == NULL)
	{
		return NULL;
	}

	DWORD dwSize = sizeof(SSkipListNode) + dwHeight * sizeof(SList);
	PSSkipListNode psSkipListNode = (PSSkipListNode)malloc(dwSize);
	if (psSkipListNode == NULL)
	{
		return NULL;
	}

	psSkipListNode->pKey = pKey;
	psSkipListNode->pValue = pValue;

	return psSkipListNode;
}

/**
 * ������� ������ � ����������
 * [In] pfComparator ����������
 * [Ret] ��������� ������
 */
PSSkipList
CreateSkipList(
	FComp* pfComparator
)
{
	if (pfComparator == NULL)
	{
		return NULL;
	}

	PSSkipList psSkipList = (PSSkipList)malloc(sizeof(SSkipList));
	if (psSkipList == NULL)
	{
		return NULL;
	}

	psSkipList->dwHeight = 1;
	psSkipList->dwCount = 0;
	psSkipList->pfComparator = pfComparator;

	for (size_t i = 0; i < ARRAYSIZE(psSkipList->pHead); i++)
	{
		ListHeadInit(&psSkipList->pHead[i]);
	}

	return psSkipList;
}

/**
 * �������� ���� ����-�������� � ������
 * ���� ���� ��� ���� � ���������, �� ������ ��������
 * [In] psSkipList	��������� ������
 * [In] pKey		����
 * [In] pValue		��������
 * [Ret] ����������� ����
 */
PSSkipListNode
SkipListAdd(
	PSSkipList	psSkipList,
	PVOID		pKey,
	PVOID		pValue
)
{
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
				psSkipListCurrentNode->pValue = pValue;
			}
		}

		psListCurrent = psListEnd->pBlink;
		if (dwIdx < dwHeight)
		{
			ListAdd(&psSkipListNode->pLink[dwIdx], psListCurrent);
		}

		psListCurrent--;
		psListEnd--;

		dwIdx--;
	}

	psSkipList->dwCount++;

	return psSkipListNode;
}

/**
 * ����� ���� � ������
 * [In] psSkipList	��������� ������
 * [In] pKey		����
 * [Ret] ��������� ����
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

			if (psSkipList->pfComparator(
				psSkipListCurrentNode->pKey, pKey) >= 0)
			{
				psListEnd = &psSkipListCurrentNode->pLink[dwIdx];
				break;
			}

			if (psSkipList->pfComparator(
				psSkipListCurrentNode->pKey, pKey) == 0)
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
 * ������� ���� � ������ �� �����
 * [In] psSkipList	��������� ������
 * [In] pKey		����
 */
VOID
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
				for (DWORD dwCurrentHeight = 0; dwCurrentHeight < dwIdx + 1; dwCurrentHeight++)
				{
					ListNodeDelete(psListCurrent);
					if (ListIsEmpty(&psSkipList->pHead[dwCurrentHeight]))
					{
						psSkipList->dwHeight--;
					}
				}

				free(psSkipListCurrentNode);
				psSkipList->dwCount--;

				return;
			}
		}

		psListCurrent = psListEnd->pBlink;

		psListCurrent--;
		psListEnd--;

		dwIdx--;
	}
}

/**
 * ����������� ������ � ��������� �������
 * [In] psSkipList			��������� ������
 * [In] pFSkipListPrinter	������� ����������� ����� ��� 
 */
VOID
SkipListPrint(
	PSSkipList			psSkipList,
	FSkipListPrinter*   pFSkipListPrinter
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
 * �������� ������ ���� ��������� ���������
 * [In] psSkipList ��������� ������
 * [Ret] ������ ���� ���������
 */
PSList
SkipListGetAll(
	PSSkipList			psSkipList
)
{
	return &psSkipList->pHead[0];
}

/**
 * ������� ��� �������� �� ������
 * [In] psSkipList ��������� ������
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

		free(psSkipListCurrentNode);
	}

	psSkipList->dwHeight = 1;
	psSkipList->dwCount = 0;

	for (size_t i = 0; i < ARRAYSIZE(psSkipList->pHead); i++)
	{
		ListHeadInit(&psSkipList->pHead[i]);
	}
}

/**
 * ������������� ������
 * [Ret] ������
 */
static DWORD
RandomHeight()
{
	DWORD dwRandom = NextRandom();
	DWORD dwBranching = 4;
	DWORD dwHeight = 1;

	while (dwHeight < SKIP_LIST_MAX_HEIGHT &&
			NextRandom() % dwBranching == 0)
	{
		dwHeight++;
	}

	return dwHeight;
}