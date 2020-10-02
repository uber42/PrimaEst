#pragma once

#ifndef SKIP_LIST_H
#define SKIP_LIST_H

/** ������������ ������ */
#define SKIP_LIST_MAX_HEIGHT 32

/**
 * ���������� ������
 * [In] pFirst	������ ����
 * [In] pSecond ������ ����
 */
typedef int FComp(
	PVOID pFirst,
	PVOID pSecond
);

/**
 * ������� ����������� ����� ��� ����������� ���� ���� ����-��������
 * [In] dwHeight	������
 * [In] fNewLevel	��� �� �������� ������� �� ������ �������
 * [In] pKey		����
 * [In] pValue		��������
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
} SSkipList, *PSSkipList;

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
 * [In] pfComparator ����������
 * [Ret] ��������� ������
 */
PSSkipList
CreateSkipList(
	FComp *pfComparator
);

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
);

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
);

/**
 * ������� ���� � ������ �� �����
 * [In] psSkipList	��������� ������
 * [In] pKey		����
 */
VOID
SkipListRemove(
	PSSkipList	psSkipList,
	PVOID		pKey
);

/**
 * ����������� ������ � ��������� �������
 * [In] psSkipList			��������� ������
 * [In] pFSkipListPrinter	������� ����������� ����� ���
 */
VOID
SkipListPrint(
	PSSkipList			psSkipList,
	FSkipListPrinter*	pFSkipListPrinter
);

/**
 * �������� ������ ���� ��������� ���������
 * [In] psSkipList ��������� ������
 * [Ret] ������ ���� ���������
 */
PSList
SkipListGetAll(
	PSSkipList			psSkipList
);

/**
 * ������� ��� �������� �� ������
 * [In] psSkipList ��������� ������
 */
VOID
SkipListClear(
	PSSkipList			psSkipList
);

#endif // !SKIP_LIST_H
