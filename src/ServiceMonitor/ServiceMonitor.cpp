#include "stdafx.h"
#include "StringUtil.h"
#include "StlUtils.h"
#include "inifile.h"
#include "HttpNotificationService.h"

VOID CALLBACK Service_Monitor::NotifyCallBack(PVOID parameter)
{
	extern handlemap g_serviceEventMap;

	Service_Monitor sm;
    PSERVICE_NOTIFY pSNotify = (PSERVICE_NOTIFY)parameter;
    HANDLE hEvent = (HANDLE) pSNotify->pContext;
    _tprintf(L"\nService Change Status Notify Callback is called for service '%s' with status '%d'",
        pSNotify->pszServiceNames, pSNotify->ServiceStatus.dwCurrentState);
    /*if (hEvent != NULL) 
    {
		SetEvent(hEvent);
    }*/
	
	//sm.StartServiceByName(pSNotify->pszServiceNames);
}

HRESULT Service_Monitor::EnsureInitialized()
{
    HRESULT hr = S_OK;
    if (!_fInitialized)
    {
        AcquireSRWLockExclusive(&_srwLock);
        if (!_fInitialized)
        {
            _hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
            if (_hSCManager == NULL)
            {
                hr = HRESULT_FROM_WIN32(GetLastError());
                _tprintf(L"\nERROR:Could NOT open server control manager [%x]\n", hr);
            }
            else
            {
                _fInitialized = TRUE;
            }
        }
        ReleaseSRWLockExclusive(&_srwLock);
    }
    return hr;
}

HRESULT Service_Monitor::StartServiceByName(LPCTSTR pServiceName, DWORD dwTimeOutSeconds)
{
    HRESULT hr = S_OK;
    SC_HANDLE hService = NULL;
    DWORD dwSleepTime = 0;
    DWORD dwRemainTime = dwTimeOutSeconds;

    hr = GetServiceHandle(pServiceName, &hService);

    if (SUCCEEDED(hr))
    {

		SERVICE_STATUS_PROCESS sStatus0;
		DWORD     dwBytes0 = 0;
		if (!QueryServiceStatusEx(hService,
			SC_STATUS_PROCESS_INFO,
			(LPBYTE)&sStatus0,
			sizeof(SERVICE_STATUS_PROCESS),
			&dwBytes0))
		{
			hr = HRESULT_FROM_WIN32(GetLastError());
			goto Finished;
		}
		if (sStatus0.dwCurrentState == SERVICE_RUNNING)
		{
			goto FinishedEscape;
		}

		SendSmsMessage(Convert2String((LPTSTR)pServiceName));

		Sleep(5000);
        if (StartService(hService, 0, NULL) == FALSE)
        {
            DWORD dwError = GetLastError();
            if (dwError == ERROR_SERVICE_ALREADY_RUNNING)
            {
                dwError = ERROR_SUCCESS;
            }
            else
            {
                hr = HRESULT_FROM_WIN32(dwError);
            }
        }
        //
        // Query service status to make sure service is in running state
        //
        while(dwRemainTime >0)
        {
            DWORD     dwBytes = 0;
            SERVICE_STATUS_PROCESS sStatus;

            if (!QueryServiceStatusEx(hService,
                SC_STATUS_PROCESS_INFO,
                (LPBYTE)&sStatus,
                sizeof(SERVICE_STATUS_PROCESS),
                &dwBytes))
            {
                hr = HRESULT_FROM_WIN32(GetLastError());
                goto Finished;
            }

            if (sStatus.dwCurrentState == SERVICE_RUNNING)
            {
                goto Finished;
            }
            else if(sStatus.dwCurrentState == SERVICE_START_PENDING)
            {
                dwSleepTime = rand() % 10 + 1;
                dwSleepTime = dwSleepTime < dwRemainTime ? dwSleepTime : dwRemainTime;
                dwRemainTime -= dwSleepTime;
                Sleep(dwSleepTime * 1000);
            }
            else
            {
                //
                // Service fails to start 
                //
                hr = E_FAIL;
                goto Finished;
            }
        }
        //
        // Cannot start service within given time period
        //
        hr = HRESULT_FROM_WIN32(ERROR_TIMEOUT);
    }

    Finished:
    if(SUCCEEDED(hr))
    {
        _tprintf(L"\n Service '%s' started \n", pServiceName);
    }
    else
    {
        _tprintf(L"\nERROR: Failed to start or query status of service '%s' error [%x]\n", pServiceName, hr);
    }

    FinishedEscape:
    if (hService != NULL)
    {
        CloseServiceHandle(hService);
    }
    return hr;
}

HRESULT Service_Monitor::StopServiceByName(LPCTSTR pServiceName, DWORD dwTimeOutSeconds)
{
        HRESULT   hr = S_OK;
        SC_HANDLE hService = NULL;
        DWORD     dwBytes = 0;
        DWORD     dwSleepTime = 0;
        DWORD     dwRemainTime = dwTimeOutSeconds;
        SERVICE_STATUS_PROCESS sStatus;

        hr = GetServiceHandle(pServiceName, &hService);
        if (SUCCEEDED(hr)) 
        {
            while (dwRemainTime >0)
            {
                DWORD     dwBytes = 0;
                if (!QueryServiceStatusEx(hService,
                    SC_STATUS_PROCESS_INFO,
                    (LPBYTE)&sStatus,
                    sizeof(SERVICE_STATUS_PROCESS),
                    &dwBytes))
                {
                    hr = HRESULT_FROM_WIN32(GetLastError());
                    goto Finished;
                }
                if (sStatus.dwCurrentState == SERVICE_STOPPED)
                {
                    goto Finished;
                }
                else if (sStatus.dwCurrentState == SERVICE_STOP_PENDING || sStatus.dwCurrentState == SERVICE_START_PENDING || sStatus.dwCurrentState == SERVICE_PAUSE_PENDING || sStatus.dwCurrentState == SERVICE_CONTINUE_PENDING)
                {
                    dwSleepTime = rand() % 10 + 1;
                    dwSleepTime = dwSleepTime < dwRemainTime ? dwSleepTime : dwRemainTime;
                    dwRemainTime -= dwSleepTime;
                    Sleep(dwSleepTime * 1000);
                }
                else if (sStatus.dwCurrentState == SERVICE_RUNNING || sStatus.dwCurrentState == SERVICE_PAUSED)
                {
                    if (!ControlService(hService, SERVICE_CONTROL_STOP, (LPSERVICE_STATUS)&sStatus))
                    {
                        hr = HRESULT_FROM_WIN32(GetLastError());
                        goto Finished;
                    }
                    _tprintf(L"\nStopping service '%s'\n", pServiceName);
                }
                else
                {
                    //
                    // Service fails to stop 
                    //
                    hr = E_FAIL;
                    goto Finished;
                }
            }
            //
            // cannot stop service within given time period
            //
            hr = HRESULT_FROM_WIN32(ERROR_TIMEOUT);

        }

    Finished: 
        if (SUCCEEDED(hr))
        {
            _tprintf(L"\n Service '%s' has been stopped \n", pServiceName);
        }
        else
        {
            _tprintf(L"\nERROR: Failed to stop or query status of service '%s' error [%x]\n", pServiceName, hr);
        }

        if (hService != NULL)
        {
            CloseServiceHandle(hService);
        }
        return hr;
    }

HRESULT Service_Monitor::MonitoringService(handlemap hmap, DWORD dwStatus)
{
	HRESULT   hr = S_OK;
	DWORD     dwError = ERROR_SUCCESS;
	DWORD     dwWaitResult;
	SC_HANDLE hService;

	std::vector<std::string> svcVec;
	std::vector<HANDLE> handleVec;


	extern notifymap g_serviceNotifyMap;

	typedef std::unordered_map<std::string, HANDLE>::const_iterator map_iter_t;
	for (map_iter_t iter = hmap.begin(); iter != hmap.end(); ++iter)
	{
		std::string svcName = iter->first;
		LPCWSTR pServiceName = ConvertW(svcName.c_str());
		//hr = sm.MonitoringService(ConvertW(svcName.c_str()),
		//	SERVICE_NOTIFY_STOPPED | SERVICE_NOTIFY_STOP_PENDING | SERVICE_NOTIFY_PAUSED | SERVICE_NOTIFY_PAUSE_PENDING,
		//	g_serviceEventMap[svcName]);
		hr = GetServiceHandle(pServiceName, &hService);
		if (FAILED(hr))
		{
			_tprintf(L"\nERROR: Failed to get service handle %S service, with failed result %d\n", pServiceName, hr);
		}
		else
		{
			//FIX bug, if in else below, same address
			SERVICE_NOTIFY *sNotify = g_serviceNotifyMap[svcName];
			
			sNotify->dwVersion = SERVICE_NOTIFY_STATUS_CHANGE;
			sNotify->pfnNotifyCallback = (PFN_SC_NOTIFY_CALLBACK)NotifyCallBack;
			sNotify->pszServiceNames = (LPWSTR)pServiceName;
			sNotify->pContext = iter->second;

			dwError = NotifyServiceStatusChange(hService, dwStatus, sNotify);
			if (dwError != ERROR_SUCCESS)
			{
				hr = HRESULT_FROM_WIN32(dwError);
				_tprintf(L"\nERROR: fail to register status change callback [%x]\n", hr);
				if (hService != NULL)
				{
					CloseServiceHandle(hService);
				}
			}
		}
	}
	svcVec = Keys(hmap);
	handleVec = Values(hmap);
	dwWaitResult = WaitForMultipleObjectsEx(svcVec.size(), &handleVec[0], FALSE, INFINITE, TRUE);

	//by hand
	if (dwWaitResult == WAIT_IO_COMPLETION)
	{
		StartServices(svcVec);
	}
	else if (dwWaitResult >= WAIT_OBJECT_0 && dwWaitResult < (WAIT_OBJECT_0 + svcVec.size())) {
		StartServiceByName(ConvertW(svcVec.at(dwWaitResult).c_str()));
	}
	else
	{
		hr = HRESULT_FROM_WIN32(GetLastError());
		_tprintf(L"\nERROR: Monitoring service '%s' wait error \n", hr);
	}
	
	return S_OK;
}

HRESULT Service_Monitor::MonitoringService(LPCTSTR pServiceName, DWORD dwStatus, HANDLE hStopEvent)
{
    HRESULT   hr = S_OK;
    DWORD     dwError = ERROR_SUCCESS;
    DWORD     dwWaitResult;
    SC_HANDLE hService;
    SERVICE_NOTIFY sNotify;

    hr = GetServiceHandle(pServiceName, &hService);
    if(FAILED(hr))
    {
        goto Finished;
    }

    sNotify.dwVersion = SERVICE_NOTIFY_STATUS_CHANGE;
    sNotify.pfnNotifyCallback = (PFN_SC_NOTIFY_CALLBACK)NotifyCallBack;
    sNotify.pszServiceNames = (LPWSTR) pServiceName;
    sNotify.pContext = hStopEvent;

    dwError = NotifyServiceStatusChange(hService, dwStatus, &sNotify);
    if (dwError != ERROR_SUCCESS)
    {
        hr = HRESULT_FROM_WIN32(dwError);
        _tprintf(L"\nERROR: fail to register status change callback [%x]\n", hr);
        goto Finished;
    }

    dwWaitResult = WaitForSingleObjectEx(hStopEvent, INFINITE, TRUE);
    switch (dwWaitResult)
    {
        // Event object was signaled
    case WAIT_OBJECT_0:
    case WAIT_IO_COMPLETION:
        break;

        // An error occurred
    default:
        hr = HRESULT_FROM_WIN32(GetLastError());
        _tprintf(L"\nERROR: Monitoring service '%s' wait error [%x]\n", pServiceName, hr);
    }

Finished:
    if (hService != NULL)
    {
        CloseServiceHandle(hService);
    }
    return hr;
}

HRESULT Service_Monitor::GetServiceHandle(LPCTSTR pServiceName, SC_HANDLE* pHandle)
{
    HRESULT hr = S_OK;

    if (pServiceName == NULL)
    {
        _tprintf(L"\nERROR: Null parameter for GetServiceHandle()\n");
        hr = HRESULT_FROM_WIN32(ERROR_INVALID_PARAMETER);
        goto Finished;
    }

    hr = EnsureInitialized();
    if (SUCCEEDED(hr))
    {
        *pHandle = OpenService(_hSCManager, pServiceName, SERVICE_START | SERVICE_STOP | SERVICE_QUERY_STATUS);
        if (*pHandle == NULL)
        {
            hr = HRESULT_FROM_WIN32(GetLastError());
            goto Finished;
        }
    }

Finished:
    return hr;
}
HRESULT Service_Monitor::StartServices(std::vector<string>& svcVec) 
{
	typedef std::vector<std::string>::const_iterator sv_citer_t;
	for (sv_citer_t iter = svcVec.begin(); iter != svcVec.end(); ++iter)
	{
		StartServiceByName(ConvertW((*iter).c_str()));
	}
	return S_OK;
}

void Service_Monitor::SendSmsMessage(std::string svcName) 
{
	string serverUrl;
	bool serverUrlResult = CIniFile::GetKey("notification", "server_url", serverUrl);
	if (serverUrlResult) {
		CHttpNotificationService::Instance().Publish(serverUrl, svcName, "");
	}
}