/**
 * @file SkipListDefinitions.h
 *
 * @author Pavel Chursin
 * @date Oct 2020
 */

#ifndef SKIP_LIST_DEFINITIONS_H
#define SKIP_LIST_DEFINITIONS_H

/** Максимальная высота */
#define SKIP_LIST_MAX_HEIGHT 12

/**
 * Компаратор ключей
 * @param[in] pFirst	Первый ключ
 * @param[in] pSecond	Второй ключ
 */
typedef int FSkipListComp(
	PVOID pFirst,
	PVOID pSecond
);

/**
 * Функция описывающая вывод для конкретного типа пары ключ-значение
 * @param[in] dwHeight	Высота
 * @param[in] fNewLevel	Был ли совершен переход на другой уровень
 * @param[in] pKey		Ключ
 * @param[in] pValue	Значение
 */
typedef int FSkipListPrinter(
	DWORD dwHeight,
	BOOL  fNewLevel,
	PVOID pKey,
	PVOID pValue
);

/**
 * Функция описывающая удаление узла структуры
 * @param[in] dwHeight	Высота
 */
typedef VOID FSkipListNodeEraser(
	PVOID	pNode
);

/**
 * Функция описывающая удаление узла структуры
 * @param[in] dwHeight	Высота
 */
typedef VOID FSkipListNodeValueChanger(
	PVOID	*pValueDest,
	PVOID	pValueSrc
);

/**
 * Сгенирировать высоту
 * @return Высота
 */
static 
DWORD
RandomHeight()
{
	DWORD dwRandom = LehmerRandom();
	DWORD dwBranching = 4;
	DWORD dwHeight = 1;

	while (dwHeight < SKIP_LIST_MAX_HEIGHT &&
		LehmerRandom() % dwBranching == 0)
	{
		dwHeight++;
	}

	return dwHeight;
}

#endif