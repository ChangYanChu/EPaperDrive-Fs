#pragma once
#include "BitFont.h"
#include "Charset.h"

typedef bool (__cdecl* _ProgressCallback)(void* pData, double progress, size_t cur, size_t total, double speed, double costTime, double remainingTime);

class CFileMaker
{
public:
	static BOOL MakeCppFile(CBitFont* pBitFont, CCharset* pCharset, CFile *file, BOOL vert_scan, BOOL msb, BOOL monospace, _ProgressCallback cbProcess = nullptr, void* cbData = nullptr);
	static BOOL MakeBinFile(CBitFont* pBitFont, CCharset* pCharset, CFile *file, BOOL vert_scan, BOOL msb, BOOL monospace, _ProgressCallback cbProcess = nullptr, void* cbData = nullptr);
};
