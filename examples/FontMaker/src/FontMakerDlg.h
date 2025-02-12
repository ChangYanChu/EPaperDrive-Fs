// FontMakerDlg.h : 头文件
#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "BitFont.h"
#include "Charset.h"
#include "EditBox.h"
#include "PreviewWnd.h"


#define CONFIG_NAME_ROOT				_T("config")

#define CONFIG_FONT_SIZE_W				_T("font_width")
#define CONFIG_FONT_SIZE_H				_T("font_height")

#define CONFIG_POS_OFFSET_H				_T("pos_offset_horz")
#define CONFIG_POS_OFFSET_V				_T("pos_offset_vert")

#define CONFIG_NAME_SCAN_TYPE			_T("scan_type")
#define CONFIG_SCAN_TYPE_H				(0)
#define CONFIG_SCAN_TYPE_V				(1)

#define CONFIG_NAME_BIT_ORDER			_T("bit_order")
#define CONFIG_BIT_ORDER_BIG			(0)
#define CONFIG_BIT_ORDER_LITTLE			(1)

#define CONFIG_NAME_CHAR_WIDTH			_T("char_width")
#define CONFIG_CHAR_WIDTH_FIXED			(0)
#define CONFIG_CHAR_WIDTH_VARIABLE		(1)

#define CONFIG_NAME_FILE_TYPE			_T("save_type")
#define CONFIG_FILE_TYPE_CPP			(0)
#define CONFIG_FILE_TYPE_BIN			(1)

#define CONFIG_NAME_GRAY_LEVEL			_T("gray_level")
#define CONFIG_GRAY_LEVEL_1				(0)
#define CONFIG_GRAY_LEVEL_2				(1)
#define CONFIG_GRAY_LEVEL_4				(2)
#define CONFIG_GRAY_LEVEL_8				(3)

// CFontMakerDlg 对话框
class CFontMakerDlg : public CDialog
{
// 构造
public:
	CFontMakerDlg(CWnd* pParent = NULL);
	enum { IDD = IDD_MAIN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

private:
	CPreviewWnd m_draw;
	CEditBox m_ebox;
	CBitFont m_bitfont;
	CCharset m_charset;
	HANDLE m_hTask;
	HFONT m_hFont;
	WCHAR m_wChar;
	UINT  m_nCharIndex;
	BOOL m_bInitOK;
	int m_nFontSize;
	int m_nFontWidth;
	int m_nFontHeight;
	int m_nOffsetX;
	int m_nOffsetY;
	int m_nBpp;
	bool m_fBusy;
	bool m_fQuit;

	CString m_szCharsetPath;
	CComboBox m_listFontName;
	CComboBox m_listFontStyle;
	CComboBox m_listCharset;
	CButton m_btnEdit;
	CSpinButtonCtrl m_spFontSize;
	CSpinButtonCtrl m_spFontWidth;
	CSpinButtonCtrl m_spFontHeight;
	CSpinButtonCtrl m_spOffsetX;
	CSpinButtonCtrl m_spOffsetY;
	HICON m_hIcon;
public:
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	virtual void PostNcDestroy();
	void RedrawPreview();
	void OnFontChange();
	void OnCharChange();
	void OnCharsetChange();
	void OnCharTableChange();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedBtnEdit();
	afx_msg void OnBnClickedBtnSave();
	afx_msg void OnBnClickedBtnStd();
	afx_msg void OnBnClickedBtnUser();
	afx_msg void OnCbnSelchangeListFontName();
	afx_msg void OnCbnSelchangeListFontStyle();
	afx_msg void OnCbnSelchangeListCharset();
	afx_msg void OnEnChangeEditFontSize();
	afx_msg void OnEnChangeEditWidth();
	afx_msg void OnEnChangeEditHeight();
	afx_msg void OnEnChangeEditHorz();
	afx_msg void OnEnChangeEditVert();
	afx_msg void OnDeltaposSpin6(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtn1bpp();
	afx_msg void OnBnClickedBtn2bpp();
	afx_msg void OnBnClickedBtn4bpp();
	afx_msg void OnBnClickedBtn8bpp();
	afx_msg void OnClose();

private:

	// 任务线程处理
	static void TaskProc(void* pData);
	static bool ProgressCallback(void* pData, double progress, size_t cur, size_t total, double speed, double costTime, double remainingTime);
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
