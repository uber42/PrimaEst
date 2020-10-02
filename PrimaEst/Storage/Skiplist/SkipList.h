#pragma once

#ifndef SKIP_LIST_H
#define SKIP_LIST_H

/** Максимальная высота */
#define SKIP_LIST_MAX_HEIGHT 32

/**
 * Компаратор ключей
 * [In] pFirst	Первый ключ
 * [In] pSecond Второй ключ
 */
typedef int FComp(
	PVOID pFirst,
	PVOID pSecond
);

/**
 * Функция описывающая вывод для конкретного типа пары ключ-значение
 * [In] dwHeight	Высота
 * [In] fNewLevel	Был ли совершен переход на другой уровень
 * [In] pKey		Ключ
 * [In] pValue		Значение
 */
typedef int FSkipListPrinter(
	DWORD dwHeight,
	BOOL  fNewLevel,
	PVOID pKey,
	PVOID pValue
);

/**
 * Структура список с пропусками
 */
typedef struct _SkipList {
	/** Количество записей */
	DWORD  dwCount;

	/** Высота */
	DWORD  dwHeight;

	/** Компаратор */
	FComp* pfComparator;

	/** Список уровней */
	SList  pHead[SKIP_LIST_MAX_HEIGHT];
} SSkipList, *PSSkipList;

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
 * [In] pfComparator Компаратор
 * [Ret] Созданный список
 */
PSSkipList
CreateSkipList(
	FComp *pfComparator
);

/**
 * Добавить пару ключ-значение в список
 * Если ключ уже есть в структуре, то меняет значение
 * [In] psSkipList	Экземпляр списка
 * [In] pKey		Ключ
 * [In] pValue		Значение
 * [Ret] Добавленный узел
 */
PSSkipListNode
SkipListAdd(
	PSSkipList	psSkipList,
	PVOID		pKey,
	PVOID		pValue
);

/**
 * Найти ключ в списке
 * [In] psSkipList	Экземпляр списка
 * [In] pKey		Ключ
 * [Ret] Найденный узел
 */
PSSkipListNode
SkipListFind(
	PSSkipList	psSkipList,
	PVOID		pKey
);

/**
 * Удалить узел в списке по ключу
 * [In] psSkipList	Экземпляр списка
 * [In] pKey		Ключ
 */
VOID
SkipListRemove(
	PSSkipList	psSkipList,
	PVOID		pKey
);

/**
 * Распечатать список с иерархией уровней
 * [In] psSkipList			Экземпляр списка
 * [In] pFSkipListPrinter	Функция описывающая вывод для
 */
VOID
SkipListPrint(
	PSSkipList			psSkipList,
	FSkipListPrinter*	pFSkipListPrinter
);

/**
 * Получить список всех элементов структуры
 * [In] psSkipList Экземпляр списка
 * [Ret] Список всех элементов
 */
PSList
SkipListGetAll(
	PSSkipList			psSkipList
);

/**
 * Удалить все значения из списка
 * [In] psSkipList Экземпляр списка
 */
VOID
SkipListClear(
	PSSkipList			psSkipList
);

#endif // !SKIP_LIST_H
