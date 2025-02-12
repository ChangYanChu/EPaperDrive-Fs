// FontMakerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "FontMakerApp.h"
#include "FontMakerDlg.h"
#include "FileMaker.h"
#include "IniHelper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CFontMakerDlg 对话框
CFontMakerDlg::CFontMakerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFontMakerDlg::IDD, pParent)
{
	m_bInitOK = 0;
	m_nCharIndex = 0;
	m_wChar = L'0';
	m_nBpp = 1;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_fBusy = false;
	m_hTask = nullptr;
	m_fQuit = false;
}

void CFontMakerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_FONT_NAME, m_listFontName);
	DDX_Control(pDX, IDC_LIST_FONT_STYLE, m_listFontStyle);
	DDX_Control(pDX, IDC_LIST_CHARSET, m_listCharset);
	DDX_Control(pDX, IDC_BTN_EDIT, m_btnEdit);
	DDX_Control(pDX, IDC_SPIN1, m_spFontWidth);
	DDX_Control(pDX, IDC_SPIN3, m_spFontHeight);
	DDX_Control(pDX, IDC_SPIN2, m_spOffsetX);
	DDX_Control(pDX, IDC_SPIN4, m_spOffsetY);
	DDX_Control(pDX, IDC_SPIN5, m_spFontSize);
}

BEGIN_MESSAGE_MAP(CFontMakerDlg, CDialog)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_EDIT, &CFontMakerDlg::OnBnClickedBtnEdit)
	ON_BN_CLICKED(IDC_BTN_SAVE, &CFontMakerDlg::OnBnClickedBtnSave)
	ON_BN_CLICKED(IDC_BTN_STD, &CFontMakerDlg::OnBnClickedBtnStd)
	ON_BN_CLICKED(IDC_BTN_USER, &CFontMakerDlg::OnBnClickedBtnUser)
	ON_CBN_SELCHANGE(IDC_LIST_FONT_NAME, &CFontMakerDlg::OnCbnSelchangeListFontName)
	ON_CBN_SELCHANGE(IDC_LIST_FONT_STYLE, &CFontMakerDlg::OnCbnSelchangeListFontStyle)
	ON_CBN_SELCHANGE(IDC_LIST_CHARSET, &CFontMakerDlg::OnCbnSelchangeListCharset)
	ON_EN_CHANGE(IDC_EDIT_FONT_SIZE, &CFontMakerDlg::OnEnChangeEditFontSize)
	ON_EN_CHANGE(IDC_EDIT_WIDTH, &CFontMakerDlg::OnEnChangeEditWidth)
	ON_EN_CHANGE(IDC_EDIT_HEIGHT, &CFontMakerDlg::OnEnChangeEditHeight)
	ON_EN_CHANGE(IDC_EDIT_HORZ, &CFontMakerDlg::OnEnChangeEditHorz)
	ON_EN_CHANGE(IDC_EDIT_VERT, &CFontMakerDlg::OnEnChangeEditVert)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN6, &CFontMakerDlg::OnDeltaposSpin6)
	ON_BN_CLICKED(IDC_BTN_1BPP, &CFontMakerDlg::OnBnClickedBtn1bpp)
	ON_BN_CLICKED(IDC_BTN_2BPP, &CFontMakerDlg::OnBnClickedBtn2bpp)
	ON_BN_CLICKED(IDC_BTN_4BPP, &CFontMakerDlg::OnBnClickedBtn4bpp)
	ON_BN_CLICKED(IDC_BTN_8BPP, &CFontMakerDlg::OnBnClickedBtn8bpp)
	ON_WM_CLOSE()
END_MESSAGE_MAP()

//枚举系统字体
static int CALLBACK FontEnumProc(CONST ENUMLOGFONT *lpelfe,CONST TEXTMETRIC *lpntme,DWORD FontType,LPARAM lParam)
{
	CComboBox* pComboBox;
	pComboBox = (CComboBox*)lParam;
	if(lpelfe->elfFullName[0]=='@')
	{
		return 1;
	}
	if(pComboBox->FindString(0,lpelfe->elfFullName)==CB_ERR)
	{
		pComboBox->AddString(lpelfe->elfFullName);
	}
	return 1;
}

static void InitFontFamily(CComboBox* pComboBox)
{
	HDC hdc;
	LOGFONT lf;
	ZeroMemory(&lf,sizeof(lf));
	lf.lfCharSet = DEFAULT_CHARSET;
	hdc = GetDC(NULL);
	::EnumFontFamiliesEx(hdc,&lf,(FONTENUMPROCW)::FontEnumProc,(LPARAM)pComboBox,0);
	ReleaseDC(NULL,hdc);
	pComboBox->SetCurSel(0);
}

//返回文件标题,不含后缀名
static int GetFileTitle(LPTSTR lpFile, LPTSTR lpName, int count)
{
	int ret;
	TCHAR* dot;
	ZeroMemory(lpName,count*sizeof(TCHAR));
	dot = wcsrchr(lpFile,_T('.'));
	if(dot != NULL)
	{
		ret = (int)(dot-lpFile);
		wcsncpy_s(lpName,count,lpFile,ret);
	}
	else
	{
		ret = count;
		wcsncpy_s(lpName,count,lpFile,ret);
	}
	return ret;
}

//遍历文件夹,搜索*.cst
static UINT InitCharset(CComboBox* pComboBox)
{
	UINT count;
	HANDLE hFind;
	CString szPath;
	WCHAR title[MAX_PATH];
	WIN32_FIND_DATA wfd;
	count = 0;
	theApp.GetPath(szPath);
	szPath.Append(L"charset\\*.cst");
	hFind = FindFirstFile(szPath,&wfd);
	if(hFind!=INVALID_HANDLE_VALUE)
	{
		count++;
		GetFileTitle(wfd.cFileName,title,MAX_PATH);
		pComboBox->AddString(title);
		while(FindNextFile(hFind,&wfd))
		{
			count++;
			GetFileTitle(wfd.cFileName,title,MAX_PATH);
			pComboBox->AddString(title);
		}
		FindClose(hFind);
		pComboBox->SetCurSel(0);
	}
	return count;
}

void CFontMakerDlg::OnFontChange()
{
	int style;
	WCHAR name[32];
	m_listFontName.GetWindowText(name,32);
	style = m_listFontStyle.GetCurSel();
	m_bitfont.SetFont(name, m_nFontSize, style, m_nBpp);
	RedrawPreview();
}

void CFontMakerDlg::OnCharChange()
{
	CString str;
	m_wChar = m_charset.GetChar(m_nCharIndex);
	str.Format(L"当前字符: U+%04X(%d/%d)",m_wChar,m_nCharIndex+1,m_charset.GetCharCount());
	SetDlgItemText(IDC_EDIT_INDEX,str);
	RedrawPreview();
}

//标准字符集变更
void CFontMakerDlg::OnCharsetChange()
{
	UINT count;
	CString name;
	m_listCharset.GetWindowText(name);
	name.Append(L".cst");
	name = m_szCharsetPath + name;
	m_charset.Delete();
	if(!m_charset.LoadFromFile(name))
	{
		m_nCharIndex = 0;
		MessageBox(L"加载字符集文件失败!",L"错误提示",MB_OK|MB_ICONWARNING);
		return;
	}
	count = m_charset.GetCharCount();
	m_nCharIndex = 0;
	OnCharChange();
}

//用户码表变更
void CFontMakerDlg::OnCharTableChange()
{
	UINT count;
	if(m_ebox.m_pzTable != NULL)
	{
		m_charset.Delete();
		m_charset.Create(m_ebox.m_pzTable);
		count = m_charset.GetCharCount();
		m_nCharIndex = 0;
		OnCharChange();
	}
}

// CFontMakerDlg 消息处理程序
BOOL CFontMakerDlg::OnInitDialog()
{
	int count;
	CDialog::OnInitDialog();
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标
	SetWindowText(L"通用点阵字库生成工具");
	InitFontFamily(&m_listFontName);
	
	m_draw.SubclassDlgItem(IDC_BITMAP,this);
	m_draw.SetDC(m_bitfont.GetDC());
	
	theApp.GetPath(m_szCharsetPath);
	m_szCharsetPath.Append(L"charset\\");
	count=InitCharset(&m_listCharset);
	if(count==0)
	{
		GetDlgItem(IDC_BTN_STD)->EnableWindow(FALSE);
		CheckDlgButton(IDC_BTN_USER,1);
		OnBnClickedBtnUser();
		OnCharTableChange();
	}
	else
	{
		CheckDlgButton(IDC_BTN_STD,1);
		OnBnClickedBtnStd();
		OnCharsetChange();
	}
	m_bInitOK = TRUE;
	m_listFontStyle.SetCurSel(0);
	m_spFontSize.SetRange(1,+999);
	m_spFontWidth.SetRange(1,999);
	m_spFontHeight.SetRange(1,999);
	m_spOffsetX.SetRange(-999,+999);
	m_spOffsetY.SetRange(-999,+999);

	SetDlgItemInt(IDC_EDIT_HEIGHT,16);
	SetDlgItemInt(IDC_EDIT_HORZ,0);
	SetDlgItemInt(IDC_EDIT_VERT,0);
	SetDlgItemInt(IDC_EDIT_FONT_SIZE,16);


	CIniHelper config;

	// 点阵大小
	int fontWidth = config.GetNumber(CONFIG_NAME_ROOT, CONFIG_FONT_SIZE_W, 16);
	int fontHeight = config.GetNumber(CONFIG_NAME_ROOT, CONFIG_FONT_SIZE_H, 16);
	SetDlgItemInt(IDC_EDIT_WIDTH, fontWidth);
	SetDlgItemInt(IDC_EDIT_HEIGHT, fontHeight);

	// 位置偏移
	int offsetHorz = config.GetNumber(CONFIG_NAME_ROOT, CONFIG_POS_OFFSET_H, 0);
	int offsetVert = config.GetNumber(CONFIG_NAME_ROOT, CONFIG_POS_OFFSET_V, 0);
	SetDlgItemInt(IDC_EDIT_HORZ, offsetHorz);
	SetDlgItemInt(IDC_EDIT_VERT, offsetVert);

	// 扫描方式
	int scanType = config.GetNumber(CONFIG_NAME_ROOT, CONFIG_NAME_SCAN_TYPE, CONFIG_SCAN_TYPE_H);
	if (CONFIG_SCAN_TYPE_H == scanType)
	{
		CheckDlgButton(IDC_BTN_SCAN1, 1);
	}
	else
	{
		CheckDlgButton(IDC_BTN_SCAN2, 1);
	}

	// 比特顺序
	int bitOrder = config.GetNumber(CONFIG_NAME_ROOT, CONFIG_NAME_BIT_ORDER, CONFIG_BIT_ORDER_BIG);
	if (CONFIG_SCAN_TYPE_H == bitOrder)
	{
		CheckDlgButton(IDC_BTN_MSB, 1);
	}
	else
	{
		CheckDlgButton(IDC_BTN_LSB, 1);
	}

	// 字符宽度
	int charWidth = config.GetNumber(CONFIG_NAME_ROOT, CONFIG_NAME_CHAR_WIDTH, CONFIG_CHAR_WIDTH_FIXED);
	if (CONFIG_CHAR_WIDTH_FIXED == charWidth)
	{
		CheckDlgButton(IDC_BTN_FW, 1);
	}
	else
	{
		CheckDlgButton(IDC_BTN_VW, 1);
	}

	// 文件类型
	int fileType = config.GetNumber(CONFIG_NAME_ROOT, CONFIG_NAME_FILE_TYPE, CONFIG_FILE_TYPE_BIN);
	if (CONFIG_FILE_TYPE_CPP == fileType)
	{
		CheckDlgButton(IDC_BTN_C, 1);
	}
	else
	{
		CheckDlgButton(IDC_BTN_BIN, 1);
	}

	// 灰度等级
	int grayLevel = config.GetNumber(CONFIG_NAME_ROOT, CONFIG_NAME_GRAY_LEVEL, CONFIG_GRAY_LEVEL_1);
	if (grayLevel > CONFIG_GRAY_LEVEL_8) grayLevel = CONFIG_GRAY_LEVEL_1;
	if (CONFIG_GRAY_LEVEL_1 == grayLevel) CheckDlgButton(IDC_BTN_1BPP,1);
	if (CONFIG_GRAY_LEVEL_2 == grayLevel) CheckDlgButton(IDC_BTN_2BPP,1);
	if (CONFIG_GRAY_LEVEL_4 == grayLevel) CheckDlgButton(IDC_BTN_4BPP,1);
	if (CONFIG_GRAY_LEVEL_8 == grayLevel) CheckDlgButton(IDC_BTN_8BPP,1);

	return TRUE;
}

void CFontMakerDlg::OnClose()
{
	m_fQuit = true;

	if (nullptr != m_hTask)
	{
		::WaitForSingleObject(m_hTask, 5000);
	}

	CIniHelper config;

	// 点阵大小
	config.SetNumber(CONFIG_NAME_ROOT, CONFIG_FONT_SIZE_W, GetDlgItemInt(IDC_EDIT_WIDTH, nullptr));
	config.SetNumber(CONFIG_NAME_ROOT, CONFIG_FONT_SIZE_H, GetDlgItemInt(IDC_EDIT_HEIGHT, nullptr));

	// 位置偏移
	config.SetNumber(CONFIG_NAME_ROOT, CONFIG_POS_OFFSET_H, GetDlgItemInt(IDC_EDIT_HORZ, nullptr));
	config.SetNumber(CONFIG_NAME_ROOT, CONFIG_POS_OFFSET_V, GetDlgItemInt(IDC_EDIT_VERT, nullptr));

	// 扫描方式
	if (IsDlgButtonChecked(IDC_BTN_SCAN1))
	{
		config.SetNumber(CONFIG_NAME_ROOT, CONFIG_NAME_SCAN_TYPE, CONFIG_SCAN_TYPE_H);
	}
	else
	{
		config.SetNumber(CONFIG_NAME_ROOT, CONFIG_NAME_SCAN_TYPE, CONFIG_SCAN_TYPE_V);
	}

	// 比特顺序
	if (IsDlgButtonChecked(IDC_BTN_MSB))
	{
		config.SetNumber(CONFIG_NAME_ROOT, CONFIG_NAME_BIT_ORDER, CONFIG_BIT_ORDER_BIG);
	}
	else
	{
		config.SetNumber(CONFIG_NAME_ROOT, CONFIG_NAME_BIT_ORDER, CONFIG_BIT_ORDER_LITTLE);
	}

	// 字符宽度
	if (IsDlgButtonChecked(IDC_BTN_FW))
	{
		config.SetNumber(CONFIG_NAME_ROOT, CONFIG_NAME_CHAR_WIDTH, CONFIG_CHAR_WIDTH_FIXED);
	}
	else
	{
		config.SetNumber(CONFIG_NAME_ROOT, CONFIG_NAME_CHAR_WIDTH, CONFIG_CHAR_WIDTH_VARIABLE);
	}

	// 文件类型
	if (IsDlgButtonChecked(IDC_BTN_C))
	{
		config.SetNumber(CONFIG_NAME_ROOT, CONFIG_NAME_FILE_TYPE, CONFIG_FILE_TYPE_CPP);
	}
	else
	{
		config.SetNumber(CONFIG_NAME_ROOT, CONFIG_NAME_FILE_TYPE, CONFIG_FILE_TYPE_BIN);
	}

	// 灰度等级
	if (IsDlgButtonChecked(IDC_BTN_1BPP))
	{
		config.SetNumber(CONFIG_NAME_ROOT, CONFIG_NAME_GRAY_LEVEL, CONFIG_GRAY_LEVEL_1);
	}

	if (IsDlgButtonChecked(IDC_BTN_2BPP))
	{
		config.SetNumber(CONFIG_NAME_ROOT, CONFIG_NAME_GRAY_LEVEL, CONFIG_GRAY_LEVEL_2);
	}

	if (IsDlgButtonChecked(IDC_BTN_4BPP))
	{
		config.SetNumber(CONFIG_NAME_ROOT, CONFIG_NAME_GRAY_LEVEL, CONFIG_GRAY_LEVEL_4);
	}

	if (IsDlgButtonChecked(IDC_BTN_8BPP))
	{
		config.SetNumber(CONFIG_NAME_ROOT, CONFIG_NAME_GRAY_LEVEL, CONFIG_GRAY_LEVEL_8);
	}

	CDialog::OnClose();
}

//更新预览图
void CFontMakerDlg::RedrawPreview()
{
	m_bitfont.PaintChar(m_wChar);
	m_draw.Invalidate(FALSE);
}

//编辑码表
void CFontMakerDlg::OnBnClickedBtnEdit()
{
	m_ebox.DoModal();
	OnCharTableChange();
}

//字体变更
void CFontMakerDlg::OnCbnSelchangeListFontName()
{
	OnFontChange();
}

//字形变更
void CFontMakerDlg::OnCbnSelchangeListFontStyle()
{
	OnFontChange();
}

//字号变更
void CFontMakerDlg::OnEnChangeEditFontSize()
{
	if(m_bInitOK)
	{
		m_nFontSize = GetDlgItemInt(IDC_EDIT_FONT_SIZE);
		OnFontChange();
	}
}

//字符集变更
void CFontMakerDlg::OnCbnSelchangeListCharset()
{
	OnCharsetChange();
}

//选中标准字库
void CFontMakerDlg::OnBnClickedBtnStd()
{
	m_listCharset.EnableWindow(1);
	m_btnEdit.EnableWindow(0);
	OnCharsetChange();
}

//选中用户字库
void CFontMakerDlg::OnBnClickedBtnUser()
{
	m_listCharset.EnableWindow(0);
	m_btnEdit.EnableWindow(1);
	OnCharTableChange();
}

//点阵宽度变更
void CFontMakerDlg::OnEnChangeEditWidth()
{
	if(m_bInitOK)
	{
		m_nFontWidth = GetDlgItemInt(IDC_EDIT_WIDTH);
		m_bitfont.SetSize(m_nFontWidth,m_nFontHeight);
		m_draw.SetSize(m_nFontWidth,m_nFontHeight);
		RedrawPreview();
	}
}

//点阵高度变更
void CFontMakerDlg::OnEnChangeEditHeight()
{
	if(m_bInitOK)
	{
		m_nFontHeight = GetDlgItemInt(IDC_EDIT_HEIGHT);
		m_bitfont.SetSize(m_nFontWidth,m_nFontHeight);
		m_draw.SetSize(m_nFontWidth,m_nFontHeight);
		RedrawPreview();
	}
}

//水平偏移变更
void CFontMakerDlg::OnEnChangeEditHorz()
{
	if(m_bInitOK)
	{
		m_nOffsetX = GetDlgItemInt(IDC_EDIT_HORZ);
		m_bitfont.SetOffset(m_nOffsetX,m_nOffsetY);
		RedrawPreview();
	}
}

//垂直偏移变更
void CFontMakerDlg::OnEnChangeEditVert()
{
	if(m_bInitOK)
	{
		m_nOffsetY = GetDlgItemInt(IDC_EDIT_VERT);
		m_bitfont.SetOffset(m_nOffsetX,m_nOffsetY);
		RedrawPreview();
	}
}

//序号变更
void CFontMakerDlg::OnDeltaposSpin6(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	*pResult = 0;
	if(pNMUpDown->iDelta > 0)
	{
		if(m_nCharIndex < (m_charset.GetCharCount() - 1))
		{
			m_nCharIndex++;
			OnCharChange();
		}
	}
	else
	{
		if(m_nCharIndex > 0)
		{
			m_nCharIndex--;
			OnCharChange();
		}
	}
}

bool CFontMakerDlg::ProgressCallback(void* pData, double progress, size_t cur, size_t total, double speed, double costTime, double remainingTime)
{
	CFontMakerDlg* pThis = (CFontMakerDlg*)pData;
	if (pThis->m_fQuit)
	{
		return false;
	}

	CWnd* pWnd = ::AfxGetApp()->GetMainWnd();

	CString strProgress;
	// 格式化进度文本
	strProgress.Format(L"进度: %.1lf%% %zu/%zu 速度: %.0lf/s 耗时: %.1lfs 剩余: %.1lfs", 
		(double)(cur * 100) / (double)total, 
		cur, total,
		speed,
		costTime,
		remainingTime
	);

	pThis->SetWindowText(strProgress);

	return true;
}

void CFontMakerDlg::TaskProc(void* pData)
{
	BOOL vert;
	BOOL msb;
	BOOL mono;
	BOOL ret;
	CFile file;
	CString name;
	CFileDialog fbox(0);

	CFontMakerDlg* pThis = (CFontMakerDlg*)pData;

	CString strTitle;
	pThis->GetWindowText(strTitle);

	do
	{
		vert = pThis->IsDlgButtonChecked(IDC_BTN_SCAN2);
		msb  = pThis->IsDlgButtonChecked(IDC_BTN_MSB);
		mono = pThis->IsDlgButtonChecked(IDC_BTN_FW);
		if(fbox.DoModal()==IDOK)
		{
			name = fbox.GetPathName();
			if(!file.Open(name,CFile::modeCreate|CFile::modeReadWrite|CFile::shareDenyNone))
			{
				pThis->MessageBox(L"创建文件失败!",L"操作失败",MB_OK|MB_ICONWARNING);
				break;
			}

			clock_t timeBegin = ::clock();
			if(pThis->IsDlgButtonChecked(IDC_BTN_C))
			{
				ret = CFileMaker::MakeCppFile(&pThis->m_bitfont, &pThis->m_charset, &file, vert, msb, mono, ProgressCallback, pThis);
			}
			else
			{
				ret = CFileMaker::MakeBinFile(&pThis->m_bitfont, &pThis->m_charset, &file, vert, msb, mono, ProgressCallback, pThis);
			}
			clock_t timeEnd = ::clock();

			if (pThis->m_fQuit)
			{
				break;
			}

			if(ret)
			{
				CString strInfo;
				strInfo.Format(_T("保存文件成功, 耗时: %.3f秒"), (double)(timeEnd - timeBegin)/1000.0f);
				pThis->MessageBox(strInfo,L"操作完成",MB_OK|MB_ICONINFORMATION);
			}
			else
			{
				pThis->MessageBox(L"保存文件失败!",L"操作失败",MB_OK|MB_ICONINFORMATION);
			}
		}
	} while (false);

	if (!file.GetFilePath().IsEmpty())
	{
		file.Close();
	}

	pThis->SetWindowText(strTitle);

	pThis->m_fBusy = false;
}

//保存文件
void CFontMakerDlg::OnBnClickedBtnSave()
{
	if (m_fBusy)
	{
		return;
	}

	m_fBusy = true;

	m_hTask = (HANDLE)::_beginthread(TaskProc, 0, this);
}

void CFontMakerDlg::PostNcDestroy()
{
	m_charset.Delete();
	CDialog::PostNcDestroy();
}

void CFontMakerDlg::OnBnClickedBtn1bpp()
{
	m_nBpp = 1;
	OnFontChange();
}

void CFontMakerDlg::OnBnClickedBtn2bpp()
{
	m_nBpp = 2;
	OnFontChange();
}

void CFontMakerDlg::OnBnClickedBtn4bpp()
{
	m_nBpp = 4;
	OnFontChange();
}

void CFontMakerDlg::OnBnClickedBtn8bpp()
{
	m_nBpp = 8;
	OnFontChange();
}


BOOL CFontMakerDlg::PreTranslateMessage(MSG* pMsg)
{
	// 繁忙时段不处理客户区鼠标消息
	if (m_fBusy)
	{
		if (pMsg->message >= WM_LBUTTONDOWN && pMsg->message <= WM_XBUTTONDBLCLK)
		{
			return TRUE;
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}
