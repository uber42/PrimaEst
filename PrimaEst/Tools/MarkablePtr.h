/**
 * @file MarkablePtr.h
 *
 * @author Pavel Chursin
 * @date Oct 2020
 */

typedef SIZE_T MarkablePtr, *PMarkablePtr;


/**
* Создать помечаемый указатель
* @param[in] pMarkable		Экземпляр
* @param[in] bIsMarked		Помечен ли ?
*/
__forceinline
static
MarkablePtr
MakeMarkablePtr(
	MarkablePtr		pMarkable,
	BOOL			bIsMarked
)
{
	return (SIZE_T)(pMarkable) | bIsMarked;
}


/**
* Установить метку на указатель
* @param[in] pMarkable		Экземпляр
*/
__forceinline
static
VOID
MarkablePtrSet(
	PMarkablePtr	ppMarkable,
	MarkablePtr		pSource,
	BOOL			bMarked
)
{
	*ppMarkable = (MarkablePtr)(((SIZE_T)pSource & (~0x1)) | bMarked);
}


/**
* Проверить метку у указателя
* @param[in] pMarkable		Экземпляр
*/
__forceinline
static
BOOL
MarkablePtrCheck(
	PMarkablePtr	pMarkable
)
{
	return ((SIZE_T)pMarkable & 0x1);
}

/**
* Получить ссылку
* @param[in] pMarkable		Экземпляр
*/
__forceinline
static
PVOID
MarkablePtrGetRef(
	MarkablePtr	pMarkable
)
{
	return (PVOID)((SIZE_T)pMarkable & (~0x1));
}

/**
* Получить ссылку и маркер
* @param[in] pMarkable		Экземпляр
* @param[out] pbMarker		Маркер
*/
__forceinline
static
PVOID
MarkablePtSplit(
	MarkablePtr	pMarkable,
	PBOOL		pbMarker
)
{
	*pbMarker = (SIZE_T)pMarkable & 0x1;
	return (PVOID)((SIZE_T)pMarkable & (~0x1));
}

/**
* CAS операция для помечаемой переменной
* @param[in] pMarkable			Экземпляр
* @param[in] pExceptedAddr		Ожидаемый адрес
* @param[in] pExceptedMark		Помечен ли ожидаемый адрес
* @param[in] pDesiredAddr		Желаемый адрес
* @param[in] pDesiredMark		Помечен ли желаемый адрес
*/
__forceinline
static
BOOL
MarkablePtrCAS(
	volatile PMarkablePtr	pMarkable,
	MarkablePtr				pExceptedAddr,
	BOOL					bExceptedMark,
	MarkablePtr				pDesiredAddr,
	BOOL					bDesiredMark
)
{
	pExceptedAddr = MakeMarkablePtr(pExceptedAddr, bExceptedMark);
	pDesiredAddr = MakeMarkablePtr(pDesiredAddr, bDesiredMark);

	return (BOOL)InterlockedCompareExchange(pMarkable, pDesiredAddr, pExceptedAddr);
}