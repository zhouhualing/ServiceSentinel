#include "StdAfx.h"
#include "Mutex.h"
#include "HttpNotificationService.h"
#include "StringUtil.h"
#include <iostream>
using namespace std;

CHttpNotificationService& CHttpNotificationService::Instance()
{
    static CHttpNotificationService instance;
    return instance;
}

CHttpNotificationService::CHttpNotificationService()
{
}

void CHttpNotificationService::Initialize()
{
}

CHttpNotificationService::~CHttpNotificationService()
{
}

void CHttpNotificationService::Publish(const std::string& url, const std::string& svcName, const std::string poolName)
{	
	char tmp[1024];

	memset(tmp,0,1024);

	sprintf(tmp, "curl -H \"Content-Type:application/json\" -X POST -d  \"{\\\"SvcName\\\":\\\"%s\\\",\\\"PoolName\\\":\\\"%s\\\"}\" %s",svcName.c_str(),poolName.c_str(),url.c_str());
	

	HRESULT     hr = S_OK;
	STARTUPINFO si = { sizeof(STARTUPINFO) };
	DWORD       dwStatus = 0;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	si.dwFlags |= STARTF_USESTDHANDLES;
	if (!CreateProcess(NULL,
		(LPWSTR)ConvertW(tmp),
		NULL,
		NULL,
		false,
		0,
		NULL,
		NULL,
		&si,
		&pi))
	{
		hr = HRESULT_FROM_WIN32(GetLastError());
		return;
	}
}





