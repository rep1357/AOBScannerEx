// https://github.com/Mahorori/FindPattern
// Thank to Mahorori
//
#pragma once

#include <vector>

typedef struct _SEARCH_CONTEXT
{
	std::vector<USHORT> vusXPattern;
	INT nLength;
	INT iBC[0x100];
} SEARCH_CONTEXT, far *LPSEARCH_CONTEXT, near *PSEARCH_CONTEXT, SRCH_CTX, far *LPSRCH_CTX, near *PSRCH_CTX;

#define NB_ERR  0xFF
#define NB_WC   0xCC
#define NB_WC_1 0x100
#define NB_WC_2 0x200

LPVOID WINAPI FindPatternA(__in_bcount(nSize) LPCVOID lpcv, __in INT nSize, __in LPCSTR lpPattern);
LPVOID WINAPI FindPatternW(__in_bcount(nSize) LPCVOID lpcv, __in INT nSize, __in LPCWSTR lpPattern);

#ifdef _UNICODE
#define FindPattern FindPatternW
#else
#define FindPattern FindPatternA
#endif