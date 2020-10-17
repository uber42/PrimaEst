/**
 * @file LockFreeSkipList.h
 *
 * @author Pavel Chursin
 * @date Oct 2020
 */

#ifndef LOCK_FREE_SKIP_LIST_H
#define LOCK_FREE_SKIP_LIST_H

#include "SkipListDefinitions.h"

 /**
  * ��������� ����� ������ � ����������
  */
typedef struct _SLockFreeSkipListNode
{
	/** ���� */
	PVOID pKey;

	/** �������� */
	PVOID pValue;

	/** ������ ����� */
	volatile struct _SLockFreeSkipListNode* pHead[1];
} SLockFreeSkipListNode, * PSLockFreeSkipListNode;

/**
 * ��������� ������ � ����������
 */
typedef struct _SLockFreeSkipList 
{
	/** ���������� ������� */
	volatile LONG64 dwCount;

	/** ������ */
	volatile LONG  dwHeight;

	/** ���������� */
	FSkipListComp* pfComparator;

	/** ������� �������� ���� */
	FSkipListNodeEraser* pfEraser;

	/** ������� ��������� �������� ���� */
	FSkipListNodeValueChanger* pfValueChanger;

	/** ������ ����� */
	PSLockFreeSkipListNode	psNode;
} SLockFreeSkipList, *PSLockFreeSkipList;


/**
 * ������� ������ � ����������
 * @param[in] pfComparator		����������
 * @param[in] pfEraser			������� �������� ����
 * @param[in] pfValueChanger	������� ��������� �������� ����
 * @return ��������� ������
 */
PSLockFreeSkipList
CreateLockFreeSkipList(
	FSkipListComp* pfComparator,
	FSkipListNodeEraser* pfEraser,
	FSkipListNodeValueChanger* pfValueChanger
);

/**
 * �������� ���� ����-�������� � ������
 * ���� ���� ��� ���� � ���������, �� ������ ��������
 * @param[in] psSkipList	��������� ������
 * @param[in] pKey		����
 * @param[in] pValue		��������
 * @return ����������� ����
 */
PSLockFreeSkipListNode
LockFreeSkipListSet(
	PSLockFreeSkipList	psSkipList,
	PVOID				pKey,
	PVOID				pValue
);

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
);

/**
 * ������� ��� �������� �� ������
 * @param[in] psSkipList ��������� ������
 */
VOID
LockFreeSkipListClear(
	PSLockFreeSkipList	psSkipList
);

/**
 * ���������� ��� ������� ������� ����������
 * @param[in] psSkipList ��������� ������
 */
VOID
LockFreeSkipListClose(
	PSLockFreeSkipList	psSkipList
);

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
);



#endif