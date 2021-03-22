// https://github.com/Mahorori/FindPattern
// Thank to Mahorori
//
#include "stdafx.h"

#include "findpattern.hpp"
#include <memory>

static const BYTE s_cbNibble[0x100] =
{
	NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, // 0x08
	NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, // 0x10
	NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, // 0x18
	NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, // 0x20
	NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, // 0x28
	NB_ERR, NB_ERR, NB_WC, NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, // 0x30
	0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, // 0x38
	0x8, 0x9, NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_WC, // 0x40
	NB_ERR, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF, NB_ERR, // 0x48
	NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, // 0x50
	NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, // 0x58
	NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, // 0x60
	NB_ERR, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF, NB_ERR, // 0x68
	NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, // 0x70
	NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, // 0x78
	NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, // 0x80
	NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, // 0x88
	NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, // 0x90
	NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, // 0x98
	NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, // 0xA0
	NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, // 0xA8
	NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, // 0xB0
	NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, // 0xB8
	NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, // 0xC0
	NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, // 0xC8
	NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, // 0xD0
	NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, // 0xD8
	NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, // 0xE0
	NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, // 0xE8
	NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, // 0xF0
	NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, // 0xF8
	NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR, NB_ERR  // 0xFF
};

BOOL GenerateXPattern(__inout LPSRCH_CTX lpCTX, __in LPCWSTR lpPattern)
{
	USHORT  usCurrent;
	BYTE    bN1, bN2;
	WCHAR	*pwszTemp;

	lpCTX->vusXPattern.clear();
	lpCTX->nLength = 0;

	pwszTemp = (WCHAR *)lpPattern;
	do
	{
		// skip white-spaces
		while (*pwszTemp && isspace(*pwszTemp) != 0)
			pwszTemp++;

		if (*pwszTemp == L'\0')
			return FALSE;

		// grab first nibble [X?]
		bN1 = s_cbNibble[*pwszTemp++];
		if (*pwszTemp == L'\0')
			return FALSE;

		if (isspace(*pwszTemp) != 0)
		{
			// single
			if (bN1 == NB_ERR)
				return FALSE;

			// generate & store x-pattern
			usCurrent = bN1 == NB_WC ? (NB_WC_1 | NB_WC_2) : bN1;
		}
		else
		{
			// double
			// grab second nibble [?X]
			bN2 = s_cbNibble[*pwszTemp++];
			if (bN1 == NB_ERR || bN2 == NB_ERR)
				return FALSE;

			// generate & store x-pattern
			usCurrent = bN1 == NB_WC ? NB_WC_1 : bN1 << 4;
			usCurrent |= bN2 == NB_WC ? NB_WC_2 : bN2;
		}

		lpCTX->nLength++;
		lpCTX->vusXPattern.push_back(usCurrent);
	} while (*pwszTemp != L'\0');

	return TRUE;
}

__inline BOOL XEqual(__in WORD w1, __in WORD w2)
{
	switch ((w1 >> 8) | (w2 >> 8))
	{
		case 0:		return w1 == w2;
		case 1:		return (w1 & 0x0F) == (w2 & 0x0F);
		case 2:		return (w1 & 0xF0) == (w2 & 0xF0);
		case 3:		return TRUE;
		default:	return FALSE;
	}
}

VOID CalculateBC(__inout LPSRCH_CTX lpCTX)
{
	INT i, n, cLength;

	// パターンに無い文字のずらし幅
	for (i = 0; i < 0x100; i++)
		lpCTX->iBC[i] = lpCTX->nLength + 1;

	// パターンに含まれる文字のずらし幅
	for (i = 0; i < lpCTX->nLength; i++)
	{
		cLength = lpCTX->nLength - i;

		switch (lpCTX->vusXPattern[i] >> 8)
		{
			case 1:	// NB_WC_1 (?X)
				for (n = 0; n < 0x10; n++)
					lpCTX->iBC[(n << 4) | (lpCTX->vusXPattern[i] & 0x0F)] = cLength;
				break;

			case 2:	// NB_WC_2 (X?)
				for (n = 0; n < 0x10; n++)
					lpCTX->iBC[n | (lpCTX->vusXPattern[i] & 0xF0)] = cLength;
				break;

			case 3:	// NB_WC_1 | NB_WC_2
				for (n = 0; n < 0x100; n++)
					lpCTX->iBC[n] = cLength;
				break;

			default:
				lpCTX->iBC[lpCTX->vusXPattern[i]] = cLength;
		}
	}
}

LPVOID PerformSearch(__in LPSRCH_CTX lpCTX, __in_bcount(nSize) const PBYTE pcbData, __in INT nSize)
{
	INT i, j;

	if (nSize < lpCTX->nLength)
		return NULL;

	i = 0;

	while (i + lpCTX->nLength - 1 < nSize)
	{
		// 先頭から探索する。
		j = 0;

		while (j < lpCTX->nLength && XEqual(lpCTX->vusXPattern[j], pcbData[i + j]))
		{
			// 一致した場合1byte右にずらす。
			j++;
		}

		if (j == lpCTX->nLength)
			return pcbData + i;
		else
		{
			// テーブル分右にずらす
			if (i + lpCTX->nLength < nSize)
				i += lpCTX->iBC[pcbData[i + lpCTX->nLength]];
			else
				return NULL;
		}
	}

	return NULL;
}

BOOL InitializeSearch(__out LPSRCH_CTX lpCTX, __in LPCWSTR lpPattern)
{
	if (!GenerateXPattern(lpCTX, lpPattern))
		return FALSE;

	CalculateBC(lpCTX);
	return TRUE;
}

LPVOID WINAPI FindPatternA(__in_bcount(nSize) LPCVOID lpcv, __in INT nSize, __in LPCSTR lpPattern)
{
	INT nLength = MultiByteToWideChar(CP_ACP, 0, lpPattern, -1, NULL, 0);
	if (nLength == 0)
		return NULL;

	std::unique_ptr<WCHAR> wszBuffer(new WCHAR[nLength]);
	if (MultiByteToWideChar(CP_ACP, 0, lpPattern, nLength, wszBuffer.get(), nLength) == 0)
		return FALSE;

	return FindPatternW(lpcv, nSize, wszBuffer.get());
}

LPVOID WINAPI FindPatternW(__in_bcount(nSize) LPCVOID lpcv, __in INT nSize, __in LPCWSTR lpPattern)
{
	SRCH_CTX ctx;

	if (!InitializeSearch(&ctx, lpPattern))
		return FALSE;

	return PerformSearch(&ctx, (const PBYTE)lpcv, nSize);
}