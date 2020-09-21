/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */
#include "stdafx.h"
#include "EiAppCenter.h"
#include "EinkIteAPI.h"
#include "EinkInternal.h"
#include <Objbase.h>
#include "cmmBaseObj.h"
#include <Shlobj.h>
#include "InstallDriver.h"

CEiAppCenter::CEiAppCenter()
{
	muAppID = 0;

	muSendBufferLength = 0;
	mSendBuffer = NULL;

	muReciveBufferLength = 0;
	mpReciveBuffer = NULL;

	mhWnd = NULL;
}

CEiAppCenter::~CEiAppCenter()
{
	//ֹͣ��Ϣͨ��
	moAppListener.Stop();

	ReleaseAppBuffer();
}

//��ȡGUID�ַ���
void CEiAppCenter::GetGUIDString(wchar_t* npszBuffer, int niLen)
{
	GUID ldGuid;
	HRESULT lhResult = S_FALSE;
	while (lhResult != S_OK)
	{
		lhResult = CoCreateGuid(&ldGuid);
		Sleep(1);
	}

	StringFromGUID2(ldGuid, (LPOLESTR)npszBuffer, niLen);
}

// ��ʼ��ִ����
ULONG CEiAppCenter::Initialize(TRSP_SYSTEM_INFO_DATA& rSystemInfoData)
{
	ULONG luResult;

	//���ɹܵ�����
	wchar_t lszTempBuffer[MAX_PATH] = { 0 };
	GetGUIDString(lszTempBuffer, MAX_PATH);
	swprintf_s(mdRegAppInfo.mszAppFilePath, MAX_PATH, L"\\\\.\\pipe\\%s.eink", lszTempBuffer);

	//��������
	CreateAppBuffer();

	luResult = moAppListener.CreateListener(
		mdRegAppInfo.mszAppFilePath, //�ܵ�����
		(LPVOID)mpReciveBuffer,
		muReciveBufferLength,
		AppReciveCallBack,
		AppCenterCallBack,
		this
	);

	if (luResult != ERROR_SUCCESS)
		return luResult;

	// �򿪶�Host����Ϣ����
	wchar_t lszPipeName[MAX_PATH] = { 0 };
	luResult = GetRegSZ(L"PipeName", lszPipeName);
	if (luResult != ERROR_SUCCESS)
		return luResult;

	luResult = moConnectToHost.CreateConnector(lszPipeName);
	if (luResult != ERROR_SUCCESS)
		return luResult;

	// ע��App
	CEiSvrMsgItem loMsg;
	loMsg.Data.Item.AppId = muAppID = GetCurrentProcessId(); // AppIDֱ��ȡ�������ID
	loMsg.Data.Item.MsgId = EMHOSTID_REG_APP;
	ULONG luMsgBufSize = (ULONG)sizeof(REG_APP_INFO);
	memcpy_s(loMsg.Data.Item.MsgBuf, luMsgBufSize, &mdRegAppInfo, luMsgBufSize);
	loMsg.Data.Item.BufSize = luMsgBufSize;

	luResult = SendMessageToService(loMsg);
	if (luResult != ERROR_SUCCESS)
		return luResult;

	//��ȡ����ʾ�豸��Ϣ
	memcpy_s(&rSystemInfoData, sizeof(TRSP_SYSTEM_INFO_DATA), loMsg.Data.Item.MsgBuf, sizeof(TRSP_SYSTEM_INFO_DATA));

	return luResult;
}

// �ͷ�
void CEiAppCenter::Release(void)
{
	// �ر����ж�App������

	// �ر�Service�ļ���
	moAppListener.Stop();
}

// �������ݻص���ں���
void __stdcall CEiAppCenter::AppReciveCallBack(const char* npData, ULONG nSize, void* npContext)
{
	CEiAppCenter* lpThis = (CEiAppCenter*)npContext;
	lpThis->AppPushReciveDataToQueue(npData, nSize);
}

// ��������Ԥ������
void CEiAppCenter::AppPushReciveDataToQueue(const char* npData, ULONG nSize)
{
	DWORD dwMsgBufferSize = sizeof(CEiSvrMsgItem);
	CEiSvrMsgItem* pMsg = (CEiSvrMsgItem*)npData;
	moAppListener.PostMsgToListener(*pMsg);
}

// �ص���ں���
void __stdcall CEiAppCenter::AppCenterCallBack(CEiSvrMsgItem& nrMsg, void* npContext)
{
	CEiAppCenter* lpThis = (CEiAppCenter*)npContext;

	lpThis->AppDispatch(nrMsg);
}

// ���ַ�����
void CEiAppCenter::AppDispatch(CEiSvrMsgItem& nrMsg)
{
	switch (nrMsg.Data.Item.MsgId)
	{
	case EMAPPID_RESULT:
		MsgBack(nrMsg);
		break;
	case EMAPPID_FINGER_MOVE:
		//��ָ������Ϣ
		InputMsg(nrMsg);
		break;
	case EMAPPID_RE_DRAW:
		//����Ҫ��APPȫ���ػ�
		ReDraw(nrMsg);
		break;
	case EMAPPID_ACTIVATE:
		//˵��Z�ᷢ���仯
		ZOrderChange(nrMsg);
		break;
	case EMAPPID_ORIENTATION_CHANGED:
		//����֪ͨEINK��Ļ�����仯
		EinkScreenOrientationChange(nrMsg);
		break;
	case EMAPPID_LATTOP_CHANGED:
		//������̬�����仯
		LaptopModeChange(nrMsg);
		break;
	case EMAPPID_EVENT:
		//�����¼�
		ServiceMsg(nrMsg);
		break;
	case EMAPPID_HOMEBAR_CHANGED:
		//homebar״̬�����仯
		HomebarChanged(nrMsg);
		break;
	case EMAPPID_KEYBOARD_CHANGED:
		KeyboardStyleChangeComplete(nrMsg);
		break;
	case EMAPPID_RESET_TP_AREA:
		//��ҪӦ����������tp area
		ResetTPArea(nrMsg);
		break;
	case EMAPPID_PRIVACY_STATUS_CHANGED:
		//��˽����״̬�����仯
		PrivacyStatusChanged(nrMsg);
		break;
	case EMAPPID_CHECK_APP_ALIVE:
		CheckAppAlive(nrMsg);
		break;
	case EMAPPID_SMARTINFO_SETTINGS_CHANGE:
		SmartInfoSettingChange(nrMsg);
		break;
	default:
		break;
	}
}

// ������Ϣ��Service�����ȴ�
ULONG CEiAppCenter::PostMessageToService(CEiSvrMsgItem& nrMsg)
{
	ULONG uMsgLen = sizeof(nrMsg.Data.Item) + nrMsg.Data.Item.BufSize - 1;
	return moConnectToHost.PostMsg(nrMsg, uMsgLen);
}

// ������Ϣ��Service�����ȴ�
ULONG CEiAppCenter::PostMessageToService(CEiSvrMsgItem& nrMsg, EI_BUFFER* npBuffer)
{
	ULONG uMsgLen = sizeof(nrMsg.Data.Item) + nrMsg.Data.Item.BufSize - 1;
	memcpy_s(mSendBuffer, uMsgLen, &nrMsg, uMsgLen);

	ULONG uBufferLen = sizeof(EI_BUFFER) + npBuffer->ulBufferSize - 1;
	memcpy_s(mSendBuffer + uMsgLen, uBufferLen, npBuffer, uBufferLen);

	moConnectToHost.PostMsg(mSendBuffer, uMsgLen + uBufferLen);

	return ERROR_SUCCESS;
}

//������״̬
bool CEiAppCenter::ProtectService(void)
{
	HANDLE lhEventHandle = NULL;

	do
	{
		//�����װ���������ˣ����˳�
		lhEventHandle = OpenEvent(READ_CONTROL, FALSE, MUTEX_EVENT_INSTALL);
		if (lhEventHandle != NULL)
		{
			CloseHandle(lhEventHandle);
			break;
		}

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

			Sleep(1000 * 10);
		}
		else
		{
			CloseHandle(lhEventHandle);
			lhEventHandle = NULL;
		}

	} while (false);

	return 0;
}

// ������Ϣ��Service�����ȴ�
ULONG CEiAppCenter::SendMessageToService(CEiSvrMsgItem& nrMsg)
{
	ULONG luResult = ERROR_NOT_READY;


	nrMsg.Data.Item.WaitHandle = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (nrMsg.Data.Item.WaitHandle == NULL)
		return ERROR_NOT_ENOUGH_MEMORY;

	nrMsg.Data.Item.OrgItem = &nrMsg;

	PostMessageToService(nrMsg);

	DWORD ldwObject = WaitForSingleObject(nrMsg.Data.Item.WaitHandle, 1000 * 30);	// �ȴ���Լ30��
	if (ldwObject == WAIT_OBJECT_0)
	{
		luResult = nrMsg.Data.Item.Result;
		CloseHandle(nrMsg.Data.Item.WaitHandle);
	}
	else
	{
		luResult = ERROR_TIMEOUT;

		//�������ʱ�ˣ��ͼ��һ�·���״̬
		ProtectService();
	}

	return luResult;
}

// ������Ϣ�ͻ������ݸ�Service�����ȴ�
ULONG CEiAppCenter::SendMessageToService(CEiSvrMsgItem& nrMsg, EI_BUFFER* npBuffer)
{
	ULONG luResult = ERROR_NOT_READY;

	nrMsg.Data.Item.WaitHandle = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (nrMsg.Data.Item.WaitHandle == NULL)
		return ERROR_NOT_ENOUGH_MEMORY;

	nrMsg.Data.Item.OrgItem = &nrMsg;

	PostMessageToService(nrMsg, npBuffer);

	DWORD ldwObject = WaitForSingleObject(nrMsg.Data.Item.WaitHandle, 1000 * 30);	// �ȴ���Լ30��
	if (ldwObject == WAIT_OBJECT_0)
	{
		luResult = nrMsg.Data.Item.Result;
		CloseHandle(nrMsg.Data.Item.WaitHandle);
	}
	else
	{
		luResult = ERROR_TIMEOUT;

		//�������ʱ�ˣ��ͼ��һ�·���״̬
		ProtectService();
	}

	return luResult;
}

// ����һ����Ϣ���������д�����Ϣȫ������
void CEiAppCenter::RecallMessage(const CEiSvrMsgItem& nrMsg)
{
	//moConnectToHost.Recall(nrMsg);

	ULONG rcMsgId;
	memcpy_s(&rcMsgId, sizeof(ULONG), nrMsg.Data.Item.MsgBuf, sizeof(ULONG));
	
	ULONG uMsgLen = sizeof(nrMsg.Data.Item) + nrMsg.Data.Item.BufSize - 1;
	moConnectToHost.PostMsg(nrMsg, uMsgLen);
}


//////////////////////////////////////////////////////////////////////////
// ����Ϊ�����������Ӧ����

// ע��һ��App
void CEiAppCenter::MsgBack(CEiSvrMsgItem& nrMsg)
{
	// ֻ�лص������̲��ܷ��ʶ�Ӧ���ڴ�
	CEiSvrMsgItem* lpOrgMsg = nrMsg.Data.Item.OrgItem;

	if (lpOrgMsg == NULL || lpOrgMsg->Data.Item.WaitHandle == NULL)
		return;

	// ���Ʒ���ֵ
	lpOrgMsg->Data.Item.Result = nrMsg.Data.Item.Result;

	// ���Ʒ�������
	if (nrMsg.Data.Item.BufSize > 0)
	{
		memcpy_s(lpOrgMsg->Data.Item.MsgBuf, nrMsg.Data.Item.BufSize, nrMsg.Data.Item.MsgBuf, nrMsg.Data.Item.BufSize);
		lpOrgMsg->Data.Item.BufSize = nrMsg.Data.Item.BufSize;
	}

	// �ͷŷ��Ͷ˵ĵȴ�
	SetEvent(lpOrgMsg->Data.Item.WaitHandle);
}

//����ֵΪ��ַ��ʼ��ַ��rulBufferSizeΪBuffer��С
BYTE* CEiAppCenter::GetBufferBase(ULONG& rulBufferSize)
{
	BYTE* lpRetBuffer = NULL;

	do
	{
		if (mpReciveBuffer == NULL)
			break;
		lpRetBuffer = (BYTE*)mpReciveBuffer + EAC_MSG_BUFFER_SIZE;
		rulBufferSize = EAC_APP_BUFFER_SIZE - EAC_MSG_BUFFER_SIZE;
	} while (false);

	return lpRetBuffer;
}

VOID CALLBACK WinMsgMemFreeCallback(
	_In_ HWND      hwnd,
	_In_ UINT      uMsg,
	_In_ ULONG_PTR dwData,
	_In_ LRESULT   lResult
)
{
	HeapFree(GetProcessHeap(), 0, (void*)dwData);
}


void SendWinMsgWithData(
	HWND		  hWnd,
	UINT          Msg,
	WPARAM        wParam,
	const void*	  pData,
	int			  iSize
)
{
	void* lpData = HeapAlloc(GetProcessHeap(), 0, iSize);
	if (lpData == NULL)
		return;
	memcpy_s(lpData, iSize, pData, iSize);

	SendMessageCallback(hWnd, Msg, wParam, (LPARAM)lpData, WinMsgMemFreeCallback, (ULONG_PTR)lpData);
}

// ��ָ������Ϣ
void CEiAppCenter::InputMsg(CEiSvrMsgItem& nrMsg)
{
	do
	{
		BREAK_ON_NULL(mhWnd);

		PEI_TOUCHINPUT_POINT lpInput = (PEI_TOUCHINPUT_POINT)nrMsg.Data.Item.MsgBuf;
		BREAK_ON_NULL(lpInput);

		//����ϵͳsleep��ʱ
		SetThreadExecutionState(ES_DISPLAY_REQUIRED | ES_SYSTEM_REQUIRED);

		SendWinMsgWithData(mhWnd, WM_EI_TOUCH, 1, lpInput, sizeof(EI_TOUCHINPUT_POINT));

	} while (false);
}

//���ý���windows��Ϣ�Ĵ��ھ��
void CEiAppCenter::SetHwnd(HWND nHwnd)
{
	mhWnd = nHwnd;
}

// ȫ���ػ�
void CEiAppCenter::ReDraw(CEiSvrMsgItem& nrMsg)
{
	do
	{
		BREAK_ON_NULL(mhWnd);

		SendWinMsgWithData(mhWnd, WM_EI_DRAW, 0, nrMsg.Data.Item.MsgBuf, sizeof(EI_RECT));

	} while (false);
}

// Z�ᷢ���仯
void CEiAppCenter::ZOrderChange(CEiSvrMsgItem& nrMsg)
{
	do
	{
		BREAK_ON_NULL(mhWnd);
		ULONG lulType = sizeof(ULONG);

		memcpy_s(&lulType, nrMsg.Data.Item.BufSize, nrMsg.Data.Item.MsgBuf, nrMsg.Data.Item.BufSize);

		PostMessage(mhWnd, WM_EI_ACTIVATE, (WPARAM)lulType, 0);

	} while (false);
}

// Eink��Ļ�������仯
void CEiAppCenter::EinkScreenOrientationChange(CEiSvrMsgItem& nrMsg)
{
	do
	{
		BREAK_ON_NULL(mhWnd);
		ULONG lulOrientation = sizeof(ULONG);

		memcpy_s(&lulOrientation, nrMsg.Data.Item.BufSize, nrMsg.Data.Item.MsgBuf, nrMsg.Data.Item.BufSize);

		PostMessage(mhWnd, WM_EI_ROTATION, (WPARAM)lulOrientation, 0);

	} while (false);
}

// ������̬�����仯
void CEiAppCenter::LaptopModeChange(CEiSvrMsgItem& nrMsg)
{
	do
	{
		BREAK_ON_NULL(mhWnd);
		ULONG lulMode = sizeof(ULONG);

		memcpy_s(&lulMode, nrMsg.Data.Item.BufSize, nrMsg.Data.Item.MsgBuf, nrMsg.Data.Item.BufSize);

		PostMessage(mhWnd, WM_EI_LAPTOP_MODE_CHANGE, (WPARAM)lulMode, 0);

	} while (false);
}

// ����֪ͨ�¼�
void CEiAppCenter::ServiceMsg(CEiSvrMsgItem& nrMsg)
{
	do
	{
		BREAK_ON_NULL(mhWnd);
		ULONG lulEventID = sizeof(ULONG);

		memcpy_s(&lulEventID, nrMsg.Data.Item.BufSize, nrMsg.Data.Item.MsgBuf, nrMsg.Data.Item.BufSize);

		PostMessage(mhWnd, WM_EI_SERVICE_EVENT, (WPARAM)lulEventID, 0);

	} while (false);
}

// homebar״̬�����仯
void CEiAppCenter::HomebarChanged(CEiSvrMsgItem& nrMsg)
{
	do
	{
		BREAK_ON_NULL(mhWnd);
		ULONG lulModeID = sizeof(ULONG);

		memcpy_s(&lulModeID, nrMsg.Data.Item.BufSize, nrMsg.Data.Item.MsgBuf, nrMsg.Data.Item.BufSize);

		PostMessage(mhWnd, WM_EI_HOMEBAR_MODE_CHANGE, (WPARAM)lulModeID, 0);

	} while (false);
}

// ������ʽ�л����
void CEiAppCenter::KeyboardStyleChangeComplete(CEiSvrMsgItem& nrMsg)
{
	do
	{
		BREAK_ON_NULL(mhWnd);
		BOOL lbRet = false;

		memcpy_s(&lbRet, nrMsg.Data.Item.BufSize, nrMsg.Data.Item.MsgBuf, nrMsg.Data.Item.BufSize);

		PostMessage(mhWnd, WM_EI_CHANGE_KEYBOARD_STYLE_COMPLETE, (WPARAM)lbRet, 0);

	} while (false);
}

// ��������tp area
void CEiAppCenter::ResetTPArea(CEiSvrMsgItem& nrMsg)
{
	do
	{
		BREAK_ON_NULL(mhWnd);

		PostMessage(mhWnd, WM_EI_RESET_TP_AREA, 0, 0);

	} while (false);
}

//��˽����״̬�����仯
void CEiAppCenter::PrivacyStatusChanged(CEiSvrMsgItem& nrMsg)
{
	do
	{
		BREAK_ON_NULL(mhWnd);
		DWORD ldwStatus = 0;
		memcpy_s(&ldwStatus, nrMsg.Data.Item.BufSize, nrMsg.Data.Item.MsgBuf, nrMsg.Data.Item.BufSize);
		PostMessage(mhWnd, WM_EI_PRIVACY_CHANGE, (WPARAM)ldwStatus, 0);

	} while (false);
}

// ��������
void CEiAppCenter::CreateAppBuffer()
{
	if (muSendBufferLength == 0)
	{
		muSendBufferLength = 1024 * 1024 * 3;
		mSendBuffer = new char[muSendBufferLength];
	}

	if (muReciveBufferLength == 0)
	{
		muReciveBufferLength = EAC_APP_BUFFER_SIZE;
		mpReciveBuffer = new char[muReciveBufferLength];
	}
}

// �ͷŻ���
void CEiAppCenter::ReleaseAppBuffer()
{
	if (muSendBufferLength > 0)
	{
		muSendBufferLength = 0;
		delete[] mSendBuffer;
	}

	if (muReciveBufferLength > 0)
	{
		muReciveBufferLength = 0;
		delete[] mpReciveBuffer;
	}
}

// ��ӦService�˵ļ��
void CEiAppCenter::CheckAppAlive(CEiSvrMsgItem& nrMsg)
{
	if (nrMsg.Data.Item.WaitHandle == NULL || nrMsg.Data.Item.OrgItem == NULL)
		return;
	if (nrMsg.Data.Item.AppId == 0)
		return;

	// ���ؽ����App
	CEiSvrMsgItem loMsgBack;
	loMsgBack.Data.Item.AppId = nrMsg.Data.Item.AppId;
	loMsgBack.Data.Item.OrgItem = nrMsg.Data.Item.OrgItem;
	loMsgBack.Data.Item.MsgId = EMHOSTID_RESULT;
	loMsgBack.Data.Item.Result = ERROR_SUCCESS;
	loMsgBack.Data.Item.BufSize = 0;

	PostMessageToService(loMsgBack);
}

// ��鷢��Ϣ���ʱ���Ƿ��㹻��������20�뷢��������������Ϣ
void CEiAppCenter::CheckAndSendNormalRunMsg()
{
	if (moConnectToHost.GetMaxElapsedTimeOfMsg() > 1000 * 15)
	{
		CEiSvrMsgItem loMsg;
		loMsg.Data.Item.AppId = muAppID;
		loMsg.Data.Item.MsgId = EMHOSTID_NORMAL_RUN_NOTICE;
		loMsg.Data.Item.BufSize = 0;

		PostMessageToService(loMsg);
	}
}

LSTATUS CEiAppCenter::GetRegSZ(LPCWSTR lpValueName, wchar_t* value)
{
	HKEY h_reg = 0;
	LSTATUS status = ERROR_SUCCESS;
	do
	{
		status = RegOpenKeyW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Lenovo\\EinkSDK", &h_reg);
		if (status != ERROR_SUCCESS)
			break;

		DWORD data_type = REG_DWORD;
		DWORD data_size = 0;
		status = RegQueryValueExW(h_reg, lpValueName, 0, &data_type, nullptr, &data_size);
		if (status == ERROR_SUCCESS && data_size > 0)
		{
			LPBYTE buf = new BYTE[data_size];
			status = RegQueryValueExW(h_reg, lpValueName, 0, &data_type, buf, &data_size);
			memcpy_s((char*)value, data_size, buf, data_size);
			delete[] buf;
		}

	} while (false);

	if (h_reg != 0)
		RegCloseKey(h_reg);

	return status;
}

// smartinfo���÷����˱仯
void CEiAppCenter::SmartInfoSettingChange(CEiSvrMsgItem& nrMsg)
{
	do
	{
		BREAK_ON_NULL(mhWnd);
		SendWinMsgWithData(mhWnd, WM_EI_SMARTINFO_SETTING_CHANGE, 0, nrMsg.Data.Item.MsgBuf, nrMsg.Data.Item.BufSize);
	} while (false);
}
