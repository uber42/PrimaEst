#pragma once

#ifndef RANDOM_H
#define RANDOM_H

static DWORD NextRandom()
{
	static DWORD dwSeed = 0xDEADBEEF;

	static const DWORD dwMod				= 2147483647L;
	static const unsigned __int64 uiFactor	= 16807;		

	unsigned __int64 uiProduct = dwSeed * uiFactor;

	dwSeed = (DWORD)((uiProduct >> 31) + (uiProduct & dwMod));
	if (dwSeed > dwMod)
	{
		dwSeed -= dwMod;
	}

	return dwSeed;
}

#endif // RANDOM_H
