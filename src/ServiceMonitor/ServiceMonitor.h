#pragma once

#include <Windows.h>

#include <string>
#include <vector>
#include <unordered_map>
typedef std::unordered_map<std::string, PSERVICE_NOTIFY> notifymap;
typedef std::unordered_map<std::string, HANDLE> handlemap;
class Service_Monitor
{

public:
    Service_Monitor() :_hSCManager(NULL)
    {
        InitializeSRWLock(&_srwLock);
    }

    ~Service_Monitor()
    {
        if (_hSCManager != NULL)
        {
            CloseServiceHandle(_hSCManager);
        }
    }

    HRESULT EnsureInitialized();
    HRESULT StartServiceByName(LPCTSTR pServiceName, DWORD dwTimeOutSeconds = 30);
    HRESULT StopServiceByName(LPCTSTR pServiceName, DWORD dwTimeOutSeconds = 30);
	HRESULT MonitoringService(handlemap hmap, DWORD dwStatus);
    HRESULT MonitoringService(LPCTSTR pServiceName, DWORD dwStatus, HANDLE hStopEvent);
    static VOID  CALLBACK  NotifyCallBack(PVOID parameter);
	HRESULT StartServices(std::vector<std::string>& svcVec);

private:
    HRESULT GetServiceHandle(LPCTSTR pServiceName, SC_HANDLE* pHandle);
    SC_HANDLE _hSCManager;
    BOOL      _fInitialized = FALSE;
    SRWLOCK   _srwLock;

private:
	void SendSmsMessage(std::string svcName);
};
