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
  * Структура узела списка с пропусками
  */
typedef struct _SLockFreeSkipListNode
{
	/** Ключ */
	PVOID pKey;

	/** Значение */
	PVOID pValue;

	/** Список узлов */
	volatile struct _SLockFreeSkipListNode* pHead[1];
} SLockFreeSkipListNode, * PSLockFreeSkipListNode;

/**
 * Структура список с пропусками
 */
typedef struct _SLockFreeSkipList 
{
	/** Количество записей */
	volatile LONG64 dwCount;

	/** Высота */
	volatile LONG  dwHeight;

	/** Компаратор */
	FSkipListComp* pfComparator;

	/** Функция удаления узла */
	FSkipListNodeEraser* pfEraser;

	/** Функция изменения значения узла */
	FSkipListNodeValueChanger* pfValueChanger;

	/** Список узлов */
	PSLockFreeSkipListNode	psNode;
} SLockFreeSkipList, *PSLockFreeSkipList;


/**
 * Создать список с пропусками
 * @param[in] pfComparator		Компаратор
 * @param[in] pfEraser			Функция удаления узла
 * @param[in] pfValueChanger	Функция изменения значения узла
 * @return Созданный список
 */
PSLockFreeSkipList
CreateLockFreeSkipList(
	FSkipListComp* pfComparator,
	FSkipListNodeEraser* pfEraser,
	FSkipListNodeValueChanger* pfValueChanger
);

/**
 * Добавить пару ключ-значение в список
 * Если ключ уже есть в структуре, то меняет значение
 * @param[in] psSkipList	Экземпляр списка
 * @param[in] pKey		Ключ
 * @param[in] pValue		Значение
 * @return Добавленный узел
 */
BOOL
LockFreeSkipListSet(
	PSLockFreeSkipList	psSkipList,
	PVOID				pKey,
	PVOID				pValue
);

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
);

/**
 * Удалить все значения из списка
 * @param[in] psSkipList Экземпляр списка
 */
VOID
LockFreeSkipListClear(
	PSLockFreeSkipList	psSkipList
);

/**
 * Освободить все ресурсы занятые структурой
 * @param[in] psSkipList Экземпляр списка
 */
VOID
LockFreeSkipListClose(
	PSLockFreeSkipList	psSkipList
);

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
);



#endif