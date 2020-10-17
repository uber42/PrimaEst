/**
 * @file SkipList.h
 *
 * @author Pavel Chursin
 * @date Oct 2020
 */

#ifndef SKIP_LIST_H
#define SKIP_LIST_H

#include "SkipListDefinitions.h"

/**
 * Структура список с пропусками
 */
typedef struct _SkipList
{
	/** Количество записей */
	DWORD  dwCount;

	/** Высота */
	DWORD  dwHeight;

	/** Компаратор */
	FSkipListComp* pfComparator;

	/** Функция удаления узла */
	FSkipListNodeEraser* pfEraser;

	/** Функция изменения значения узла */
	FSkipListNodeValueChanger* pfValueChanger;

	/** Список уровней */
	SList  pHead[SKIP_LIST_MAX_HEIGHT];
} SSkipList, * PSSkipList;

/**
 * Структура узела списка с пропусками
 */
typedef struct _SSkipListNode
{
	/** Ключ */
	PVOID pKey;

	/** Значение */
	PVOID pValue;

	/** Список узлов */
	SList pLink[];
} SSkipListNode, * PSSkipListNode;

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
);

/**
 * Добавить пару ключ-значение в список
 * Если ключ уже есть в структуре, то меняет значение
 * @param[in] psSkipList	Экземпляр списка
 * @param[in] pKey		Ключ
 * @param[in] pValue		Значение
 * @return Добавленный узел
 */
PSSkipListNode
SkipListSet(
	PSSkipList	psSkipList,
	PVOID		pKey,
	PVOID		pValue
);

/**
 * Найти ключ в списке
 * @param[in] psSkipList	Экземпляр списка
 * @param[in] pKey		Ключ
 * @return Найденный узел
 */
PSSkipListNode
SkipListFind(
	PSSkipList	psSkipList,
	PVOID		pKey
);

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
);

/**
 * Распечатать список с иерархией уровней
 * @param[in] psSkipList			Экземпляр списка
 * @param[in] pFSkipListPrinter	Функция описывающая вывод для
 */
VOID
SkipListPrint(
	PSSkipList			psSkipList,
	FSkipListPrinter* pFSkipListPrinter
);

/**
 * Получить список всех элементов структуры
 * @param[in] psSkipList Экземпляр списка
 * @return Список всех элементов
 */
PSList
SkipListGetAll(
	PSSkipList			psSkipList
);

/**
 * Удалить все значения из списка
 * @param[in] psSkipList Экземпляр списка
 */
VOID
SkipListClear(
	PSSkipList			psSkipList
);

/**
 * Освободить все ресурсы занятые структурой
 * @param[in] psSkipList Экземпляр списка
 */
VOID
SkipListClose(
	PSSkipList			psSkipList
);

#endif // !SKIP_LIST_H
