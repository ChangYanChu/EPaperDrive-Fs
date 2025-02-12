#include "stdafx.h"
#include "IniHelper.h"

#include <windows.h>
#include <strsafe.h>

#define FROFILE_STRING_MAX_CHARACTERS_SIZE      (1024 * 64)
#define FROFILE_EXTERN_NAME                     _T(".ini")

CIniHelper::CIniHelper()
    :
    m_strFilePath(GetCurrentModuleDir())
{
    m_strFilePath += _T("\\");
    m_strFilePath += GetCurrentModuleName(false);
    m_strFilePath += FROFILE_EXTERN_NAME;
}

CIniHelper::CIniHelper(const _tstring& strFileName, bool isAbsolutePath)
{
    if (isAbsolutePath)
    {
        m_strFilePath = strFileName;
    }
    else
    {
        m_strFilePath = GetCurrentModuleDir();
        m_strFilePath += _T("\\");
        m_strFilePath += strFileName;
    }
}

CIniHelper::~CIniHelper()
{
}

void CIniHelper::SetPath(const _tstring& strFilePath, bool isAbsolutePath)
{
    if (isAbsolutePath)
    {
        m_strFilePath = strFilePath;
    }
    else
    {
        m_strFilePath = GetCurrentModuleDir();
        m_strFilePath += _T("\\");
        m_strFilePath += strFilePath;
    }
}

_tstring CIniHelper::GetPath() const
{
    return m_strFilePath;
}

bool CIniHelper::SetString(const _tstring& strAppName, const _tstring& strKeyName, const _tstring& strString)
{
    return ::WritePrivateProfileString(strAppName.c_str(), strKeyName.c_str(), strString.c_str(), m_strFilePath.c_str());
}

_tstring CIniHelper::GetString(const _tstring& strAppName, const _tstring& strKeyName, const _tstring& strDefault) const
{
    _tstring strResult;

    LPTSTR lpBuf = (LPTSTR)::HeapAlloc(::GetProcessHeap(), HEAP_ZERO_MEMORY, FROFILE_STRING_MAX_CHARACTERS_SIZE * sizeof(TCHAR));
    if (NULL != lpBuf)
    {
        ::GetPrivateProfileString(strAppName.c_str(), strKeyName.c_str(), strDefault.c_str(), lpBuf, FROFILE_STRING_MAX_CHARACTERS_SIZE, m_strFilePath.c_str());
        strResult = lpBuf;
    }

    if (NULL != lpBuf)
    {
        ::HeapFree(::GetProcessHeap(), 0, lpBuf);
    }

    return strResult;
}

bool CIniHelper::SetNumber(const _tstring& strAppName, const _tstring& strKeyName, int dwNumber)
{
    TCHAR szBuf[MAX_PATH] = { 0 };
    _itot_s(dwNumber, szBuf, 10);
    return this->SetString(strAppName, strKeyName, szBuf);
}

int CIniHelper::GetNumber(const _tstring& strAppName, const _tstring& strKeyName, int dwDefault) const
{
    TCHAR szBuf[MAX_PATH] = { 0 };
    _itot_s(dwDefault, szBuf, 10);
    _tstring strText = this->GetString(strAppName, strKeyName, szBuf);
    return _tcstoul(strText.c_str(), nullptr, 10);
}

bool CIniHelper::SetNumber64(const _tstring& strAppName, const _tstring& strKeyName, uint64_t nNumber)
{
    TCHAR szBuf[MAX_PATH] = { 0 };
    _i64tot_s(nNumber, szBuf, _countof(szBuf), 10);
    return this->SetString(strAppName, strKeyName, szBuf);
}

uint64_t CIniHelper::GetNumber64(const _tstring& strAppName, const _tstring& strKeyName, uint64_t nDefault/* = -1*/) const
{
    TCHAR szBuf[MAX_PATH] = { 0 };
    _i64tot_s(nDefault, szBuf, _countof(szBuf), 10);
    _tstring strText = this->GetString(strAppName, strKeyName, szBuf);
    return _tcstoull(strText.c_str(), nullptr, 10);
}

//¶ÁÐ´¸¡µãÊý
bool CIniHelper::SetFloat(const _tstring& strAppName, const _tstring& strKeyName, double dwNumber)
{
    TCHAR szBuf[MAX_PATH] = { 0 };
    StringCchPrintf(szBuf, _countof(szBuf), _T("%lf"), dwNumber);
    return this->SetString(strAppName, strKeyName, szBuf);
}

double CIniHelper::GetFloat(const _tstring& strAppName, const _tstring& strKeyName, double dwDefault/* = -1*/) const
{
    TCHAR szBuf[MAX_PATH] = { 0 };
    StringCchPrintf(szBuf, _countof(szBuf), _T("%lf"), dwDefault);
    _tstring strText = this->GetString(strAppName, strKeyName, szBuf);

    return _tstof(strText.c_str());
}

bool CIniHelper::SetHex(const _tstring& strAppName, const _tstring& strKeyName, int dwNumber)
{
    TCHAR szBuf[MAX_PATH] = { 0 };
    StringCchPrintf(szBuf, _countof(szBuf), _T("0x%X"), dwNumber);
    return this->SetString(strAppName, strKeyName, szBuf);
}

int CIniHelper::GetHex(const _tstring& strAppName, const _tstring& strKeyName, int dwDefault) const
{
    TCHAR szBuf[MAX_PATH] = { 0 };
    StringCchPrintf(szBuf, _countof(szBuf), _T("0x%X"), dwDefault);
    _tstring strText = this->GetString(strAppName, strKeyName, szBuf);
    return _tcstoul(strText.c_str(), nullptr, 16);
}

bool CIniHelper::DeleteValue(const _tstring& strAppName, const _tstring& strKeyName)
{
    if (strKeyName.empty())
    {
        return ::WritePrivateProfileString(strAppName.c_str(), nullptr, NULL, m_strFilePath.c_str());
    }
    else
    {
        return ::WritePrivateProfileString(strAppName.c_str(), strKeyName.c_str(), NULL, m_strFilePath.c_str());
    }
}

_tstring CIniHelper::GetCurrentModulePath()
{
    TCHAR szCurPath[MAX_PATH] = { 0 };
    ::GetModuleFileName(NULL, szCurPath, _countof(szCurPath));

    _tstring strDir = szCurPath;
    return strDir;
}

_tstring CIniHelper::GetCurrentModuleDir()
{
    _tstring strDir = GetCurrentModulePath();
    strDir.resize(strDir.find_last_of(_T('\\')));
    return strDir;
}

_tstring CIniHelper::GetCurrentModuleName(bool bHasExt/* = false*/)
{
    _tstring strDir = GetCurrentModulePath();
    size_t nIndex = strDir.find_last_of(_T('\\'));
    if (nIndex != _tstring::npos)
    {
        strDir = strDir.substr(nIndex + 1);
    }

    if (!bHasExt)
    {
        nIndex = strDir.find_last_of(_T('.'));
        if (nIndex != _tstring::npos)
        {
            strDir.resize(nIndex);
        }
    }

    return strDir;
}

bool CIniHelper::IsDirectory(const _tstring& strPath)
{
    DWORD dwAttr = ::GetFileAttributes(strPath.c_str());
    if (INVALID_FILE_ATTRIBUTES == dwAttr)
    {
        return false;
    }

    return FILE_ATTRIBUTE_DIRECTORY == (FILE_ATTRIBUTE_DIRECTORY & dwAttr);
}
