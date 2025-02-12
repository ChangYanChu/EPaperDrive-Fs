#pragma once

#include <string>
#include <tchar.h>
#include <stdint.h>

#ifdef _UNICODE
using _tstring = std::wstring;
#else
using _tstring = std::string;
#endif

class CIniHelper
{
public:
    CIniHelper();
    CIniHelper(const _tstring& strFileName, bool isAbsolutePath = false);
    ~CIniHelper();

    //���û�ȡ�����ļ�·��
    void SetPath(const _tstring& strFilePath, bool isAbsolutePath = false);
    _tstring GetPath() const;

    //��д�ַ���
    bool SetString(const _tstring& strAppName, const _tstring& strKeyName, const _tstring& strString);
    _tstring GetString(const _tstring& strAppName, const _tstring& strKeyName, const _tstring& strDefault = _T("")) const;

    //��д����
    bool SetNumber(const _tstring& strAppName, const _tstring& strKeyName, int dwNumber);
    int GetNumber(const _tstring& strAppName, const _tstring& strKeyName, int dwDefault = -1) const;

    bool SetNumber64(const _tstring& strAppName, const _tstring& strKeyName, uint64_t nNumber);
    uint64_t GetNumber64(const _tstring& strAppName, const _tstring& strKeyName, uint64_t nDefault = -1) const;

    //��д������
    bool SetFloat(const _tstring& strAppName, const _tstring& strKeyName, double dwNumber);
    double GetFloat(const _tstring& strAppName, const _tstring& strKeyName, double dwDefault = -1) const;

    //��д����(ʮ������)
    bool SetHex(const _tstring& strAppName, const _tstring& strKeyName, int dwNumber);
    int GetHex(const _tstring& strAppName, const _tstring& strKeyName, int dwDefault = -1) const;

    //ɾ������ֵ
    bool DeleteValue(const _tstring& strAppName, const _tstring& strKeyName = _T(""));

    static _tstring GetCurrentModulePath();
    static _tstring GetCurrentModuleDir();
    static _tstring GetCurrentModuleName(bool bHasExt = false);
    static bool IsDirectory(const _tstring& strPath);

private:
    _tstring m_strFilePath;  //�����ļ�·��
};