#pragma once
class ProcessUtil
{
public:
	ProcessUtil() 
	{
	}
	
	~ProcessUtil() {}
	int RunCommand(std::string& pstrCmd, std::string & result);
private:
	HANDLE g_hChildStd_IN_Rd = NULL;
	HANDLE g_hChildStd_IN_Wr = NULL;
	HANDLE g_hChildStd_OUT_Rd = NULL;
	HANDLE g_hChildStd_OUT_Wr = NULL;

	HRESULT CreateChildProcess(std::string psCmd);
	std::string ReadFromPipe(void);
};

