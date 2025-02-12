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

    //设置获取配置文件路径
    void SetPath(const _tstring& strFilePath, bool isAbsolutePath = false);
    _tstring GetPath() const;

    //读写字符串
    bool SetString(const _tstring& strAppName, const _tstring& strKeyName, const _tstring& strString);
    _tstring GetString(const _tstring& strAppName, const _tstring& strKeyName, const _tstring& strDefault = _T("")) const;

    //读写数字
    bool SetNumber(const _tstring& strAppName, const _tstring& strKeyName, int dwNumber);
    int GetNumber(const _tstring& strAppName, const _tstring& strKeyName, int dwDefault = -1) const;

    bool SetNumber64(const _tstring& strAppName, const _tstring& strKeyName, uint64_t nNumber);
    uint64_t GetNumber64(const _tstring& strAppName, const _tstring& strKeyName, uint64_t nDefault = -1) const;

    //读写浮点数
    bool SetFloat(const _tstring& strAppName, const _tstring& strKeyName, double dwNumber);
    double GetFloat(const _tstring& strAppName, const _tstring& strKeyName, double dwDefault = -1) const;

    //读写数字(十六进制)
    bool SetHex(const _tstring& strAppName, const _tstring& strKeyName, int dwNumber);
    int GetHex(const _tstring& strAppName, const _tstring& strKeyName, int dwDefault = -1) const;

    //删除配置值
    bool DeleteValue(const _tstring& strAppName, const _tstring& strKeyName = _T(""));

    static _tstring GetCurrentModulePath();
    static _tstring GetCurrentModuleDir();
    static _tstring GetCurrentModuleName(bool bHasExt = false);
    static bool IsDirectory(const _tstring& strPath);

private:
    _tstring m_strFilePath;  //配置文件路径
};