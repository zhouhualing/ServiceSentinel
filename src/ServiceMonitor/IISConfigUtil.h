// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

#pragma once

#include <Windows.h>
#include <unordered_map>

#include <vector>
#include <deque>

enum APPCMD_CMD_TYPE
{
    APPCMD_ADD = 0,
    APPCMD_RM = 1
};

class IISConfigUtil
{
public:

    IISConfigUtil();
    ~IISConfigUtil();
    HRESULT Initialize();
    HRESULT UpdateEnvironmentVarsToConfig(WCHAR* pstrAppPoolName);
	std::string GetRunningPools();
	std::vector<std::pair<std::string, bool>> ParseRunningPools(std::string input);
	std::string StartPools(std::string pool);
private:
    HRESULT RunCommand(std::wstring& pstrCmd, BOOL fIgnoreError = TRUE);
    void    Replace(std::wstring& str, std::wstring oldValue, std::wstring newValue);
    HRESULT BuildAppCmdCommand(const std::vector<std::pair<std::wstring, std::wstring>>& vecSet, std::vector<std::pair<std::wstring, std::wstring>>::iterator& envVecIter, WCHAR* pstrAppPoolName, std::wstring& pStrCmd, APPCMD_CMD_TYPE appcmdType);
	HRESULT BuildStartedAppCmdCommand(std::wstring& pStrCmd);

    BOOL    FilterEnv(const std::unordered_map<std::wstring, LPTSTR>& filter, LPCTSTR strEnvName, LPCTSTR strEnvValue);
    TCHAR*  m_pstrSysDirPath;
};

