#include "StdAfx.h"
#include "BitFont.h"

CBitFont::CBitFont(void)
{
	HDC hdc;
	m_nOffsetX = 0;
	m_nOffsetY = 0;
	m_nWidth = 0;
	m_nHeight= 0;
	m_nBytesPerLine = 0;
	m_nCharWidth = 0;
	m_hBitmap = NULL;
	m_hFont = NULL;
	hdc = ::GetDC(NULL);
	m_hdc = ::CreateCompatibleDC(hdc);
	::ReleaseDC(NULL,hdc);
	::SetTextColor(m_hdc,RGB(255,255,255));
	::SetBkColor(m_hdc,RGB(0,0,0));
}

CBitFont::~CBitFont(void)
{
	if(m_hdc)
	{
		::DeleteDC(m_hdc);
	}
	if(m_hBitmap)
	{
		::DeleteObject(m_hBitmap);
	}
	if(m_hFont)
	{
		::DeleteObject(m_hFont);
	}
}

void CBitFont::CreateBitmap(void)
{
	HBITMAP hBitmap;
	BITMAPINFO bmi;
	ZeroMemory(&bmi,sizeof(bmi));
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = m_nWidth;
	bmi.bmiHeader.biHeight = m_nHeight;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;
	hBitmap = ::CreateDIBSection(m_hdc,&bmi,DIB_RGB_COLORS,(void**)&m_pPixels,NULL,0);
	if(hBitmap != NULL)
	{
		if(m_hBitmap != NULL)
		{
			::DeleteObject(m_hBitmap);
		}
		::SelectObject(m_hdc,hBitmap);
		m_hBitmap = hBitmap;
	}
}

void CBitFont::SetFont(WCHAR *name, INT size, INT style, INT bpp)
{
	LOGFONT lf;
	ZeroMemory(&lf,sizeof(lf));
	wcscpy_s(lf.lfFaceName, name);
	lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfQuality = (bpp == 1) ? NONANTIALIASED_QUALITY : ANTIALIASED_QUALITY;
	lf.lfHeight = size;
	switch(style)
	{
	case 0:
		lf.lfWeight = 400;
		break;
	case 1:
		lf.lfWeight = 700;
		break;
	case 2:
		lf.lfWeight = 400;
		lf.lfItalic = 1;
		break;
	}
	if(m_hFont != NULL)
	{
		DeleteObject(m_hFont);
	}
	m_nFontBpp = bpp;
	m_hFont = CreateFontIndirect(&lf);
	::SelectObject(m_hdc,m_hFont);
}

void CBitFont::SetOffset(INT dx, INT dy)
{
	m_nOffsetX = dx;
	m_nOffsetY = dy;
}

void CBitFont::SetSize(INT width, INT height)
{
	m_nWidth = width;
	m_nHeight= height;
	m_nBytesPerLine = width;
	CreateBitmap();
}

void CBitFont::GrayScale(void)
{
	DWORD* pLine;
	for(int y=0; y<m_nHeight; y++)
	{
		pLine = m_pPixels + m_nBytesPerLine * (m_nHeight - 1 - y);
		for(int x=0; x<m_nWidth; x++)
		{
			BYTE val = GetRValue(pLine[x]);
			switch(m_nFontBpp)
			{
			case 1:
				break;
			case 2:
				if(val != 0xFF)
				{
					val &= 0xC0;
					pLine[x] = RGB(val, val, val);
				}
				break;
			case 4:
				if(val != 0xFF)
				{
					val &= 0xF0;
					pLine[x] = RGB(val, val, val);
				}
				break;
			default:
				break;
			}
		}
	}
}

void CBitFont::PaintChar(WCHAR ch)
{
	RECT rc;
	SIZE sz;
	SetRect(&rc,0,0,m_nWidth,m_nHeight);
	::FillRect(m_hdc,&rc,(HBRUSH)GetStockObject(BLACK_BRUSH));
	::TextOut(m_hdc,m_nOffsetX,m_nOffsetY,&ch,1);
	::GetTextExtentPoint32W(m_hdc,&ch,1,&sz);
	GrayScale();
	m_nCharWidth = sz.cx;
}

HDC CBitFont::GetDC()
{
	return m_hdc;
}

BYTE CBitFont::GetPixel(int x, int y)
{
	int line;
	BYTE value;
	DWORD* pLine;
	line = m_nHeight - 1 - y;
	pLine = m_pPixels + m_nBytesPerLine * line;
	value = (BYTE)pLine[x];
	switch(m_nFontBpp)
	{
	case 1:
		return (value & 0x01);
	case 2:
		return (value & 0xC0) >> 6;
	case 4:
		return (value & 0xF0) >> 4;
	case 8:
		return (value & 0xFF);
	default:
		return 0;
	}
}

INT  CBitFont::GetBitsHorz(BYTE* pBits, INT size, BOOL msb, BOOL vw)
{
	INT x;
	INT y;
	INT ret;
	INT pos;
	BYTE val;
	ret = m_nHeight * ((m_nWidth * m_nFontBpp + 7) / 8);
	ret += vw ? 2 : 0;
	if(size < ret)
	{
		return ret;
	}
	if(vw)
	{
		pBits[0] = m_nCharWidth>>8;
		pBits[1] = m_nCharWidth;
		pBits += 2;
	}
	for(y=0; y<m_nHeight; y++)
	{
		pos = 0;
		for(x=0; x<m_nWidth; x++)
		{
			if(pos == 0)
			{
				*pBits = 0;
			}
			val = GetPixel(x, y);
			if(msb)
			{
				*pBits |= (val << (8 - m_nFontBpp - pos));
			}
			else
			{
				*pBits |= (val << pos);
			}
			pos += m_nFontBpp;
			if(pos == 8)
			{
				pos = 0;
				pBits++;
			}
		}
		if(pos != 0)
		{
			pBits++;
		}
	}
	return ret;
}

INT  CBitFont::GetBitsVert(BYTE* pBits, INT size, BOOL msb, BOOL vw)
{
	INT x;
	INT y;
	INT ret;
	INT pos;
	BYTE val;
	ret = m_nHeight * ((m_nWidth * m_nFontBpp + 7) / 8);
	ret += vw ? 2 : 0;
	if(size < ret)
	{
		return ret;
	}
	if(vw)
	{
		pBits[0] = m_nCharWidth >> 8;
		pBits[1] = m_nCharWidth;
		pBits += 2;
	}
	for(x=0; x<m_nWidth; x++)
	{
		pos = 0;
		for(y=0; y<m_nHeight; y++)
		{
			if(pos == 0)
			{
				*pBits = 0;
			}
			val = GetPixel(x, y);
			if(msb)
			{
				*pBits |= (val << (8 - m_nFontBpp - pos));
			}
			else
			{
				*pBits |= (val << pos);
			}
			pos += m_nFontBpp;
			if(pos == 8)
			{
				pos = 0;
				pBits++;
			}
		}
		if(pos != 0)
		{
			pBits++;
		}
	}
	return ret;
}

INT CBitFont::GetBits(BYTE* pBits, INT size, INT scan, BOOL msb, BOOL var)
{
	switch(scan)
	{
	case 0:
		return GetBitsHorz(pBits, size, msb, var);
	case 1:
		return GetBitsVert(pBits, size, msb, var);
	default:
		return 0;
	}
}
