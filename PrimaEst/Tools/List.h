/**
 * @file List.h
 *
 * @author Pavel Chursin
 * @date Oct 2020
 */

#ifndef LIST_H
#define LIST_H

typedef struct _SList
{
	struct _SList* pFlink;
	struct _SList* pBlink;
} SList, * PSList;

static __inline void
ListHeadInit(PSList pHead)
{
	pHead->pBlink = pHead;
	pHead->pFlink = pHead;
}

static __inline void
ListAdd(
	PSList pNode,
	PSList pHead)
{
	PSList pNext = pHead->pFlink;

	pNext->pBlink = pNode;
	pNode->pFlink = pNext;
	pNode->pBlink = pHead;
	pHead->pFlink = pNode;
}

static __inline void
ListAddToEnd(
	PSList pNode,
	PSList pHead)
{
	PSList pTail = pHead->pBlink;

	pTail->pFlink = pNode;
	pNode->pBlink = pTail;
	pNode->pFlink = pHead;
	pHead->pBlink = pNode;
}

static __inline void
ListNodeDelete(PSList pNode)
{
	PSList pFlink = pNode->pFlink;
	PSList pBlink = pNode->pBlink;

	pFlink->pBlink = pBlink;
	pBlink->pFlink = pFlink;
}

static __inline void
ListDeleteAndInit(PSList pNode)
{
	ListNodeDelete(pNode);
	ListHeadInit(pNode);
}

static __inline signed int
ListIsEmpty(PSList pHead)
{
	return (pHead == pHead->pFlink);
}

static __inline signed int
ListIsSingular(PSList pHead)
{
	return (!ListIsEmpty(pHead) && pHead->pFlink == pHead->pBlink);
}

static __inline void
ListSplice(
	PSList pList,
	PSList pHead)
{
	PSList pHeadFirst = pHead->pFlink;
	PSList pListFirst = pList->pFlink;
	PSList pListLast = pList->pBlink;

	if (ListIsEmpty(pList))
	{
		return;
	}

	pHead->pFlink = pListFirst;
	pListFirst->pBlink = pHead;

	pListLast->pFlink = pHeadFirst;
	pHeadFirst->pBlink = pListLast;
}

static __inline void
ListSpliceWithEnd(
	PSList pList,
	PSList pHead)
{
	PSList pHeadLast = pHead->pBlink;
	PSList pListFirst = pList->pFlink;
	PSList pListLast = pList->pBlink;

	if (ListIsEmpty(pList))
	{
		return;
	}

	pHeadLast->pFlink = pListFirst;
	pListFirst->pBlink = pHeadLast;

	pListLast->pFlink = pHead;
	pHead->pBlink = pListLast;
}

static __inline void
ListSpliceInit(
	PSList pList,
	PSList pHead)
{
	ListSplice(pList, pHead);
	ListHeadInit(pList);
}

static __inline void
ListSpliceWithEndInit(
	PSList pList,
	PSList pHead
)
{
	ListSpliceWithEnd(pList, pHead);
	ListHeadInit(pList);
}

static __inline void
ListCutPosition(
	PSList pHeadTo,
	PSList pHeadFrom,
	PSList pNode
)
{
	PSList pHeadFromFirst = pHeadFrom->pFlink;

	if (ListIsEmpty(pHeadFrom))
	{
		return;
	}

	if (pHeadFrom == pNode)
	{
		ListHeadInit(pHeadTo);
		return;
	}

	pHeadFrom->pFlink = pNode->pFlink;
	pHeadFrom->pFlink->pBlink = pNode->pBlink;

	pHeadTo->pBlink = pNode;
	pNode->pFlink = pHeadTo;
	pHeadTo->pFlink = pHeadFromFirst;
	pHeadTo->pFlink->pBlink = pHeadTo;
}

#define LIST_FIRST(head) ((head)->pFlink)

#define LIST_FIRST_ENTRY(head, type, member)    \
        CONTAINING_RECORD((head)->pFlink, type, member)

#define LIST_LAST_ENTRY(head, type, member)     \
        CONTAINING_RECORD((head)->pBlink, type, member)

#endif // LIST_H