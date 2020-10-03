/**
 * @file tests.c
 *
 * @author Pavel Chursin
 * @date Oct 2020
 */

#include "global.h"

int main()
{
	//SkipListAddTest();
	//SkipListSetTest();

	/** Тесты для лога */
	//LogEnumFnvTest();
	//ParseLogTest_1();
	//ParseLogTest_2();
	//LogWriteTest_1();
	LogMultiThreadWriteTest_1();

	system("PAUSE");
}