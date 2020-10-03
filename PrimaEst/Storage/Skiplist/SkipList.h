/**
 * @file SkipList.h
 *
 * @author Pavel Chursin
 * @date Oct 2020
 */

#ifndef SKIP_LIST_H
#define SKIP_LIST_H

 /** Максимальная высота */
#define SKIP_LIST_MAX_HEIGHT 32

/**
 * Компаратор ключей
 * @param[in] pFirst	Первый ключ
 * @param[in] pSecond Второй ключ
 */
typedef int FComp(
	PVOID pFirst,
	PVOID pSecond
);

/**
 * Функция описывающая вывод для конкретного типа пары ключ-значение
 * @param[in] dwHeight	Высота
 * @param[in] fNewLevel	Был ли совершен переход на другой уровень
 * @param[in] pKey		Ключ
 * @param[in] pValue		Значение
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
 * @param[in] pfComparator Компаратор
 * @return Созданный список
 */
PSSkipList
CreateSkipList(
	FComp* pfComparator
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
 * @param[in] pKey		Ключ
 */
VOID
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
