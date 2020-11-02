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
	PVOID				pKey;

	/** Значение */
	PVOID				pValue;

	/** Высота узла */
	DWORD				dwHeight;

	/** Список узлов */
	struct _SLockFreeSkipListNode* pNext[];
} SLockFreeSkipListNode, * PSLockFreeSkipListNode;

/**
 * Структура список с пропусками
 */
typedef struct _SLockFreeSkipList 
{

	/** Функция удаления узла */
	FSkipListNodeEraser*		pfEraser;

	/** Функция изменения значения узла */
	FSkipListNodeValueChanger*	pfValueChanger;

	/** Компаратор */
	FSkipListComp*				pfComparator;

	/** Голова списка */
	PSLockFreeSkipListNode		psHead;

	/** Хвост списка */
	PSLockFreeSkipListNode		psTail;
} SLockFreeSkipList, *PSLockFreeSkipList;


/**
 * Создать Lock-Free список с пропусками
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
 * @return Есть ли элемент по ключу ?
 */
BOOL
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
 * Вывод списка
 * @param[in] psSkipList Экземпляр списка
 */
VOID
LockFreeSkipListPrint(
	PSLockFreeSkipList	psSkipList
);

#endif