#include"stdafx.h"
#include <unordered_map>
#include "IniFile.h"
#include "StringUtil.h"
#include "Thread.h"
#include "Mutex.h"

#include "HttpNotificationService.h"

using namespace std;
HANDLE g_hStopEvent = INVALID_HANDLE_VALUE;

handlemap g_serviceEventMap;
notifymap g_serviceNotifyMap;
handlemap g_poolEventMap;
handlemap g_poolNotifyMap;

string services;
string iisPools;

std::vector<std::string> svcVector;
std::vector<std::string> poolVector;

void CloseEvent();

void StartServiceMonitor(void* arg);
void StartPoolMonitor(void* arg);

int RefreshTimetout = 5;

VOID CALLBACK TimerProc(HWND, UINT, UINT_PTR, DWORD)//»Øµ÷º¯Êý 
{
	printf("Hello\n");
}
BOOL WINAPI CtrlHandle(DWORD dwCtrlType)
{
    switch (dwCtrlType)
    {
    case CTRL_C_EVENT:
    case CTRL_CLOSE_EVENT:
    case CTRL_BREAK_EVENT:
    case CTRL_LOGOFF_EVENT:
    case CTRL_SHUTDOWN_EVENT:
        _tprintf(L"\nCTRL signal received. The process will now terminate.\n");
        SetEvent(g_hStopEvent);
        g_hStopEvent = INVALID_HANDLE_VALUE;
        break;

    default:
        break;
    }

    return TRUE;
}
Service_Monitor sm;
int __cdecl _tmain(int argc, _TCHAR* argv[])
{
	HRESULT hr = S_OK;
	sm = Service_Monitor();

	bool servicesResult = CIniFile::GetKey("system", "services", services);
	bool iisResult = CIniFile::GetKey("system", "iis_pool", iisPools);
	CIniFile::GetKeyInt("appsettings", "timeout",5, RefreshTimetout);
	if (!iisResult && !servicesResult)
	{
		_tprintf(L"\n services and iis_pool could not be found in your config, please check.\n");
		CloseEvent();
		return hr;
	}
	string token = ",";
	if (servicesResult)
	{
		Tokenize(services, token, svcVector);
	}
	if (iisResult)
	{
		Tokenize(iisPools, token, poolVector);
	}

	IISConfigUtil configHelper = IISConfigUtil();
	if (FAILED(hr = configHelper.Initialize()))
	{
		_tprintf(L"\nFailed to update IIS configuration\n");
		CloseEvent();
		return hr;
	}
	typedef std::vector<std::string>::const_iterator sv_citer_t;
	for (sv_citer_t iter = svcVector.begin(); iter != svcVector.end(); ++iter)
	{
		LPCTSTR svcName = ConvertW((*iter).c_str());
		HANDLE hnd = INVALID_HANDLE_VALUE;
		hnd = CreateEvent(
			NULL,               // default security attributes
			TRUE,               // manual-reset event
			FALSE,              // initial state is nonsignaled
			svcName     // object name
		);
		if (hnd == NULL || hnd == INVALID_HANDLE_VALUE)
		{
			hr = HRESULT_FROM_WIN32(GetLastError());
			_tprintf(L"\nERROR: Failed to create event of %S service, with failed result %d\n", (*iter).c_str(), hr);
		}
		else
		{
			g_serviceEventMap[(*iter)] = hnd;
			SERVICE_NOTIFY *sNotify = new SERVICE_NOTIFY();
			g_serviceNotifyMap[(*iter)] = sNotify;
			hr = sm.StartServiceByName(svcName);
			if (FAILED(hr))
			{
				_tprintf(L"\nERROR: Failed to start service %S , with failed result %d\n", (*iter).c_str(), hr);
			}
		}
	}
	for (sv_citer_t iter = poolVector.begin(); iter != poolVector.end(); ++iter)
	{
		HANDLE hnd = INVALID_HANDLE_VALUE;
		hnd = CreateEvent(
			NULL,               // default security attributes
			TRUE,               // manual-reset event
			FALSE,              // initial state is nonsignaled
			ConvertW((*iter).c_str())     // object name
		);
		if (hnd == NULL || hnd == INVALID_HANDLE_VALUE)
		{
			_tprintf(L"\nERROR: Failed to create event of %S service, with failed result %d\n", (*iter).c_str(), hr);
		}
		else
		{
			g_poolEventMap[(*iter)] = hnd;
		}
	}

	if (!SetConsoleCtrlHandler(CtrlHandle, TRUE))
	{
		hr = HRESULT_FROM_WIN32(GetLastError());
		_tprintf(L"\nERROR: Failed to set control handle with error [%x]\n", hr);

		CloseEvent();
		return hr;
	}


	//hr = sm.MonitoringService(g_serviceEventMap, SERVICE_NOTIFY_STOPPED | SERVICE_NOTIFY_STOP_PENDING | SERVICE_NOTIFY_PAUSED | SERVICE_NOTIFY_PAUSE_PENDING);
	/*while (SUCCEEDED(hr))
	{
		hr = sm.MonitoringService(g_serviceEventMap, SERVICE_NOTIFY_STOPPED | SERVICE_NOTIFY_STOP_PENDING | SERVICE_NOTIFY_PAUSED | SERVICE_NOTIFY_PAUSE_PENDING);
	}*/

	CThread serviceThread;
	CThread poolThread;
	serviceThread.Start(StartServiceMonitor, NULL);
	poolThread.Start(StartPoolMonitor, NULL);
	serviceThread.Join();
	poolThread.Join();

	CConditionalEvent event1;
	event1.Wait();
}

void CloseEvent() {
	if (g_hStopEvent != INVALID_HANDLE_VALUE)
	{
		CloseHandle(g_hStopEvent);
		g_hStopEvent = INVALID_HANDLE_VALUE;
	}
}

void StartServiceMonitor(void* arg)
{
	HRESULT hr = S_OK;
	hr = sm.MonitoringService(g_serviceEventMap, SERVICE_NOTIFY_STOPPED | SERVICE_NOTIFY_STOP_PENDING | SERVICE_NOTIFY_PAUSED | SERVICE_NOTIFY_PAUSE_PENDING);
	while (SUCCEEDED(hr))
	{
		hr = sm.MonitoringService(g_serviceEventMap, SERVICE_NOTIFY_STOPPED | SERVICE_NOTIFY_STOP_PENDING | SERVICE_NOTIFY_PAUSED | SERVICE_NOTIFY_PAUSE_PENDING);
	}	
}
void SendSmsMessage(std::string poolName)
{
	string serverUrl;
	bool serverUrlResult = CIniFile::GetKey("notification", "server_url", serverUrl);
	if (serverUrlResult) {
		CHttpNotificationService::Instance().Publish(serverUrl, "", poolName);
	}
}

void StartPoolMonitor(void* arg)
{
	IISConfigUtil configHelper = IISConfigUtil();
	configHelper.Initialize();

	typedef std::vector<std::pair<std::string, bool>>::iterator kvp_t;
	while (true)
	{
		std::string result = configHelper.GetRunningPools();
		std::vector<std::pair<std::string, bool>> result1 = configHelper.ParseRunningPools(result);

		for (kvp_t it = result1.begin(); it != result1.end(); ++it) 
		{
			if (it->second)
				continue;

			SendSmsMessage(it->first);

			if (Contails(poolVector, it->first))
				configHelper.StartPools(it->first);
		}

		Sleep(RefreshTimetout*1000);
	}
}