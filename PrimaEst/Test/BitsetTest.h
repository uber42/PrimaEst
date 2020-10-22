/**
 * @file BitsetTest.h
 *
 * @author Pavel Chursin
 * @date Oct 2020
 */

#ifndef BITSET_TEST_H
#define BITSET_TEST_H


VOID
BitsetSetBitTest();

VOID
BitsetSet8MaskTest();

VOID
BitsetSet16MaskTest();

VOID
BitsetSet64Mask_x86Test();

VOID
BitsetSet128Mask_x86Test();

VOID
BitsetSet64Mask_x64Test();

VOID
BitsetSet128Mask_x64Test();

VOID
BitsetCheckBitTest();

VOID
BitsetCheck8MaskTest();

VOID
BitsetCheck64Mask_x86Test();

VOID
BitsetCheck128Mask_x86Test();

VOID
BitsetCheck64Mask_x64Test();

VOID
BitsetCheck128Mask_x64Test();

VOID
BitsetCheck128MaskAligned_x86Test();

VOID
BitsetCheck128MaskAligned_x64Test();


#endif