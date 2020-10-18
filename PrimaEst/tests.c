/**
 * @file tests.c
 *
 * @author Pavel Chursin
 * @date Oct 2020
 */

#include "global.h"

#ifdef TESTS

int main()
{
	BitsetSetBitTest();
	BitsetSet8MaskTest();
	BitsetSet16MaskTest();
	BitsetSet64Mask_x86Test();
	BitsetSet64Mask_x64Test();
	BitsetSet128Mask_x86Test();
	BitsetSet128Mask_x64Test();
	BitsetCheckBitTest();
	BitsetCheck8MaskTest();
	BitsetCheck64Mask_x86Test();
	BitsetCheck64Mask_x64Test();
	BitsetCheck128Mask_x86Test();
	BitsetCheck128Mask_x64Test();

	//LockFreeSkipListAddTest();
	//LockFreeSkipListFintTest();
	//LockFreeSkipListMultiThreadAddTest();
	//LockFreeSkipListMultiThreadFind();
	//LockFreeSkipListMultiThreadAddAndFind();
	//LockFreeSkipListResetTest();

	//SkipListAddTest();
	//SkipListSetTest();
	//SkipListFindTest();
	//SkipListPrintTest();
	//SkipListRemoveTest();

	//LogEnumFnvTest();
	//ParseLogTest_1();
	//ParseLogTest_2();
	//LogWriteTest_1();
	//LogMultiThreadWriteTest_1();

	system("PAUSE");
}

#endif