/**
 * @file SkipList.h
 *
 * @author Pavel Chursin
 * @date Oct 2020
 */

#ifndef SKIP_LIST_H
#define SKIP_LIST_H

/** ������������ ������ */
#define SKIP_LIST_MAX_HEIGHT 32

/**
 * ���������� ������
 * @param[in] pFirst	������ ����
 * @param[in] pSecond ������ ����
 */
typedef int FComp(
	PVOID pFirst,
	PVOID pSecond
);

/**
 * ������� ����������� ����� ��� ����������� ���� ���� ����-��������
 * @param[in] dwHeight	������
 * @param[in] fNewLevel	��� �� �������� ������� �� ������ �������
 * @param[in] pKey		����
 * @param[in] pValue		��������
 */
typedef int FSkipListPrinter(
	DWORD dwHeight,
	BOOL  fNewLevel,
	PVOID pKey,
	PVOID pValue
);

/**
 * ��������� ������ � ����������
 */
typedef struct _SkipList {
	/** ���������� ������� */
	DWORD  dwCount;

	/** ������ */
	DWORD  dwHeight;

	/** ���������� */
	FComp* pfComparator;

	/** ������ ������� */
	SList  pHead[SKIP_LIST_MAX_HEIGHT];
} SSkipList, * PSSkipList;

/**
 * ��������� ����� ������ � ����������
 */
typedef struct _SSkipListNode
{
	/** ���� */
	PVOID pKey;

	/** �������� */
	PVOID pValue;

	/** ������ ����� */
	SList pLink[];
} SSkipListNode, * PSSkipListNode;

/**
 * ������� ������ � ����������
 * @param[in] pfComparator ����������
 * @return ��������� ������
 */
PSSkipList
CreateSkipList(
	FComp* pfComparator
);

/**
 * �������� ���� ����-�������� � ������
 * ���� ���� ��� ���� � ���������, �� ������ ��������
 * @param[in] psSkipList	��������� ������
 * @param[in] pKey		����
 * @param[in] pValue		��������
 * @return ����������� ����
 */
PSSkipListNode
SkipListSet(
	PSSkipList	psSkipList,
	PVOID		pKey,
	PVOID		pValue
);

/**
 * ����� ���� � ������
 * @param[in] psSkipList	��������� ������
 * @param[in] pKey		����
 * @return ��������� ����
 */
PSSkipListNode
SkipListFind(
	PSSkipList	psSkipList,
	PVOID		pKey
);

/**
 * ������� ���� � ������ �� �����
 * @param[in] psSkipList	��������� ������
 * @param[in] pKey		����
 */
VOID
SkipListRemove(
	PSSkipList	psSkipList,
	PVOID		pKey
);

/**
 * ����������� ������ � ��������� �������
 * @param[in] psSkipList			��������� ������
 * @param[in] pFSkipListPrinter	������� ����������� ����� ���
 */
VOID
SkipListPrint(
	PSSkipList			psSkipList,
	FSkipListPrinter* pFSkipListPrinter
);

/**
 * �������� ������ ���� ��������� ���������
 * @param[in] psSkipList ��������� ������
 * @return ������ ���� ���������
 */
PSList
SkipListGetAll(
	PSSkipList			psSkipList
);

/**
 * ������� ��� �������� �� ������
 * @param[in] psSkipList ��������� ������
 */
VOID
SkipListClear(
	PSSkipList			psSkipList
);

/**
 * ���������� ��� ������� ������� ����������
 * @param[in] psSkipList ��������� ������
 */
VOID
SkipListClose(
	PSSkipList			psSkipList
);

#endif // !SKIP_LIST_H
