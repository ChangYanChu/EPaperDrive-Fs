#pragma once

class CBitFont
{
public:
	CBitFont(void);
	~CBitFont(void);
	void SetFont(WCHAR *name, INT size, INT style, INT bpp);
	void SetOffset(INT dx, INT dy);
	void SetSize(INT width, INT height);
	void PaintChar(WCHAR ch);
	INT  GetBits(BYTE* pBits, INT size, INT scan, BOOL msb, BOOL var);
	HDC  GetDC();
private:
	void CreateBitmap();
	BYTE GetPixel(int x, int y);
	INT  GetBitsHorz(BYTE* pBits, INT size, BOOL msb, BOOL vw);
	INT  GetBitsVert(BYTE* pBits, INT size, BOOL msb, BOOL vw);
	void GrayScale(void);

public:
	HDC m_hdc;
	HFONT m_hFont;
	HBITMAP m_hBitmap;
	DWORD* m_pPixels;
	INT m_nBytesPerLine;
	INT m_nOffsetX;
	INT m_nOffsetY;
	INT m_nWidth;
	INT m_nHeight;
	INT m_nCharWidth;
	INT m_nFontBpp;
};
