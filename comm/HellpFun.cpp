#include "stdafx.h"
#include "HellpFun.h"
#include "InstallDriver.h"
#include <Wtsapi32.h>
#include "Compile.h"
#include "../inc/ludp.h"

HANDLE ghProcThread = NULL;

CHellpFun::CHellpFun()
{
}


CHellpFun::~CHellpFun()
{
}
//������״̬
void CHellpFun::CheckService(void)
{
	HANDLE lhEventHandle = NULL;

	do
	{
		//�������Ƿ�����
		lhEventHandle = OpenEvent(READ_CONTROL, FALSE, MUTEX_EVENT_SERVICE);
		if (lhEventHandle == NULL)
		{
			//��������
			CInstallDriver InstallDriver;
			InstallDriver.ManageDriver(L"EinkSvr",
				L"EinkSvr",
				SC_MANAGER_ALL_ACCESS | SERVICE_ALL_ACCESS,
				SERVICE_WIN32_OWN_PROCESS,
				SERVICE_AUTO_START,
				DRIVER_FUNC_START);
		}
		else
		{
			CloseHandle(lhEventHandle);
			lhEventHandle = NULL;
		}


	} while (false);

}


//ͨ���������ƻ�ȡ����PID
DWORD CHellpFun::GetPIDByName(wchar_t* npszName)
{
	DWORD ldwPID = 0;

	//��ȡ���������
	TCHAR szComputerName[MAX_COMPUTERNAME_LENGTH + 1] = { 0 };
	DWORD	ldwSize = MAX_COMPUTERNAME_LENGTH + 1;
	GetComputerName(szComputerName, &ldwSize);
	HANDLE lHandle = WTSOpenServer(szComputerName);
	WTS_PROCESS_INFO* pWtspi = NULL;
	//Opens a handle to the specified server
	do
	{
		if (lHandle == NULL)
			break;

		//ö�ټ�����ϵĽ���

		DWORD dwCount;
		if (!WTSEnumerateProcesses(lHandle,
			0,
			1,
			&pWtspi,
			&dwCount))
		{
			break;
		};

		LPWSTR pProcessName;
		DWORD ProcessId;
		for (int i = 0; i < dwCount; i++)
		{
			pProcessName = pWtspi[i].pProcessName;
			ProcessId = pWtspi[i].ProcessId;

			if (_tcsicmp(pProcessName, npszName) == 0)
			{
				ldwPID = ProcessId;
				break;
			}
		}


	} while (false);

	WTSCloseServer(lHandle);
	WTSFreeMemory(pWtspi);

	return ldwPID;
}

//�ػ��߳�
bool CHellpFun::ProtectThread(LPVOID npData)
{
	HANDLE lhEventHandle = NULL;

	do
	{
		Sleep(1000 * 3);

		//�����װ���������ˣ����˳�
		/*lhEventHandle = OpenEvent(READ_CONTROL, FALSE, MUTEX_EVENT_INSTALL);
		if (lhEventHandle != NULL)
		{
			CloseHandle(lhEventHandle);
			Sleep(1000*20);
		}*/

		//�������Ƿ�����
		lhEventHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, CHellpFun::GetPIDByName(L"EinkSvr.exe"));
		if (lhEventHandle != NULL)
		{
			WaitForSingleObject(lhEventHandle, INFINITE);
			CloseHandle(lhEventHandle);
		}


		//��������
		CInstallDriver InstallDriver;
		InstallDriver.ManageDriver(L"EinkSvr",
			L"EinkSvr",
			SC_MANAGER_ALL_ACCESS | SERVICE_ALL_ACCESS,
			SERVICE_WIN32_OWN_PROCESS,
			SERVICE_AUTO_START,
			DRIVER_FUNC_START);


	} while (true);

	return 0;
}

//�����̼߳�ط���״̬���������û�ˣ�����������
void CHellpFun::ProcService(void)
{
	// �ػ��߳�
	DWORD ldwProtectThreadID = 0;
	ghProcThread = CreateThread(
		NULL,
		0,
		(LPTHREAD_START_ROUTINE)&CHellpFun::ProtectThread,
		NULL,
		0,
		&ldwProtectThreadID
	);
}


//ɱ��ָ������
BOOL CHellpFun::KillProc(DWORD ProcessID)
{
	HANDLE hProc = NULL;

	//����ProcessVXER()���ݵĽ���PID
	hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcessID);

	if (hProc != NULL)
	{
		//��ֹ����
		if (!(TerminateProcess(hProc, 2)))
		{
			CloseHandle(hProc);
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}

	if (hProc != NULL)
		CloseHandle(hProc);

	return TRUE;
}

//ɱ��ĳЩ����
void CHellpFun::KillProcByName(wchar_t* npszName)
{
	//��ȡ���������
	TCHAR szComputerName[MAX_COMPUTERNAME_LENGTH + 1] = { 0 };
	DWORD	ldwSize = MAX_COMPUTERNAME_LENGTH + 1;
	GetComputerName(szComputerName, &ldwSize);
	//Opens a handle to the specified server
	HANDLE lHandle = WTSOpenServer(szComputerName);
	//ö�ټ�����ϵĽ���
	WTS_PROCESS_INFO* pWtspi;
	DWORD dwCount;
	if (!WTSEnumerateProcesses(lHandle,
		0,
		1,
		&pWtspi,
		&dwCount))
	{
		return;
	};

	DWORD ldwProID = GetCurrentProcessId();

	LPWSTR pProcessName;
	DWORD ProcessId;
	for (int i = 0; i < dwCount; i++)
	{
		pProcessName = pWtspi[i].pProcessName;
		ProcessId = pWtspi[i].ProcessId;

		if (_tcsicmp(pProcessName, npszName) == 0)
		{
			if (ldwProID != ProcessId)
				KillProc(ProcessId); //����ɱ���Լ�
		}
	}
	WTSCloseServer(lHandle);
	WTSFreeMemory(pWtspi);

}