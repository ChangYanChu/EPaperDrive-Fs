// AboutBox.cpp : 实现文件
//

#include "stdafx.h"
#include "FontMakerApp.h"
#include "AboutBox.h"

// CAboutBox 对话框
IMPLEMENT_DYNAMIC(CAboutBox, CDialog)

CAboutBox::CAboutBox(CWnd* pParent /*=NULL*/)
	: CDialog(CAboutBox::IDD, pParent)
{

}

CAboutBox::~CAboutBox()
{
}

void CAboutBox::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutBox, CDialog)
END_MESSAGE_MAP()

// CAboutBox 消息处理程序
static WCHAR szLicense[]=
{
	L"本软件是免费软件，你可以随意使用和传播本软件.\r\n",
};

static WCHAR szVersion[]=
{
	L"软件名称: 点阵字库生成工具\r\n"
	L"软件版本: 1.4.0\r\n"
	L"编译日期: 2023.11.18\r\n"
	L"软件作者: 蒋晓岗\r\n"
	L"电子邮箱: kerndev@foxmail.com\r\n"
};

BOOL CAboutBox::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetWindowText(L"关于");
	SetDlgItemText(IDC_EDIT1, szVersion);
	SetDlgItemText(IDC_EDIT2, szLicense);
	return TRUE;
}
