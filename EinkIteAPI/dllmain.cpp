/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */
// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "tconcmd.h"
#include "itetcon.h"
#include "einkiteapi.h"
#include "EinkInternal.h"
#include "EiAppCenter.h"
#include "BufferMgr.h"

//HANDLE glhDevice = INVALID_HANDLE_VALUE;
HWND glhAppWindow = NULL;
TRSP_SYSTEM_INFO_DATA gldSysData;
CEiAppCenter gloCenter;
CBufferMgr glAppBufferMgr;
//APP��Ļ��ʾ����,�Լ���������ʡ��ÿ���ҷ����ȡ
DWORD gldwOrientation = GIR_NONE;
//��������������Ϣ���߳�
HANDLE ghNoticeNormalRunThread = 0;

// Eui���������Ϣ�������ݽ�����
#pragma data_seg("MSGBUF")
#define EI_EXBUF_SIZE 1024*4//1024 * 1024 * 2
char gldHostExBuffer[EI_EXBUF_SIZE] = { 0 };
char gldAppExBuffer[EI_EXBUF_SIZE] = { 0 }; // �����ã������ڴ�ӳ���ļ���ɾ��,����ֻ��֧��һ��App
#pragma data_seg()

#pragma comment(linker, "/SECTION:MSGBUF,RWS")



BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

// ��ȡDLL�Ĺ����ڴ��
LPVOID EixGetSharedData(ULONG nuIndex)
{
	if (nuIndex == 0)
		return gldHostExBuffer;
	if (nuIndex == 1)
		return gldAppExBuffer;

	return NULL;
}

// ���DLL�Ĺ����ڴ�δ�С
ULONG EixGetSharedDataSize(void)
{
	return EI_EXBUF_SIZE;
}


void EiPostMessage(
	UINT Msg,
	WPARAM wParam,
	LPARAM lParam)
{
	if (glhAppWindow == NULL)
		return;
	PostMessage(glhAppWindow, Msg, wParam, lParam);
}

// ��ʼ��
// ��Ӧ����������Ҫ���ñ��ӿں�����Einkϵͳע�᱾Ӧ�á�
// ���أ� �����㣬��ʾ�ɹ������ط��㣬��ʾ�����룻
// return
//		zero: success
//		non-zero: error code
DWORD EiAppStart(
	HWND hInforWindow	// ���ڽ��ձ�ϵͳ��Ϣ��Windows���ھ��
) {
	ULONG luResult = ERROR_NOT_READY;

	do
	{
		// ����ϵͳ������
		//Sleep(1000 * 20);
		luResult = gloCenter.Initialize(gldSysData);
		if (luResult != ERROR_SUCCESS || gldSysData.uiWidth == 0 || gldSysData.uiHeight == 0)
			break;
		gloCenter.SetHwnd(hInforWindow);

		// ����service�˼��app�Ƿ��ڻ�Ծ״̬
		/*
		luResult = CreateNormalRunNoticeTimer(hInforWindow);
		if (luResult != ERROR_SUCCESS)
			break;
		*/

		//Sleep(1000 * 10);
		glhAppWindow = hInforWindow;

		//��ȡ��ʾ��Buffer
		ULONG lulBufferSize = 0;
		BYTE* lpBuffer = gloCenter.GetBufferBase(lulBufferSize);
		if (lpBuffer == NULL)
		{
			luResult = ERROR_OUTOFMEMORY;
			break;
		}

		glAppBufferMgr.Initialize(lpBuffer, lulBufferSize, gldSysData.uiWidth, gldSysData.uiHeight);

		luResult = ERROR_SUCCESS;

	} while (false);


	return luResult;
}

// ���Eink�Ļ�����Ϣ
// ���أ� �����㣬��ʾ�ɹ������ط��㣬��ʾ�����룻
// return
//		zero: success
//		non-zero: error code
DWORD EiGetSystemInfo(PEI_SYSTEM_INFO pstSystemInfo)
{
	ULONG luResult = ERROR_NOT_READY;

	do
	{
		//�������ͻ�ȡ��Ϣ
		CEiSvrMsgItem loMsg;
		loMsg.Data.Item.AppId = GetCurrentProcessId(); // AppIDֱ��ȡ�������ID
		loMsg.Data.Item.MsgId = EMHOSTID_GET_EINK_INFO;
		loMsg.Data.Item.BufSize = 0;

		luResult = gloCenter.SendMessageToService(loMsg);
		if (luResult != ERROR_SUCCESS)
			return luResult;

		//��ȡ����ʾ�豸��Ϣ
		memcpy_s(pstSystemInfo, sizeof(EI_SYSTEM_INFO), loMsg.Data.Item.MsgBuf, sizeof(EI_SYSTEM_INFO));
		pstSystemInfo->ulDpiX = 227;	// ���������Ҫ���ݵ�ǰ������� ???niu
		pstSystemInfo->ulDpiY = 227;

		luResult = ERROR_SUCCESS;

	} while (false);

	return luResult;
}

// ���APP����Ļ�趨��Ϣ
// get APP context indicated current screen settings
// ���أ� �����㣬��ʾ�ɹ������ط��㣬��ʾ�����룻
DWORD EiGetAppContext(PEI_APP_CONTEXT pstAppContext)
{
	ULONG luResult = ERROR_NOT_READY;

	do
	{
		//�������ͻ�ȡ��Ϣ
		CEiSvrMsgItem loMsg;
		loMsg.Data.Item.AppId = GetCurrentProcessId(); // AppIDֱ��ȡ�������ID
		loMsg.Data.Item.MsgId = EMHOSTID_GET_APP_CONTEXT;
		loMsg.Data.Item.BufSize = 0;

		luResult = gloCenter.SendMessageToService(loMsg);
		if (luResult != ERROR_SUCCESS)
			return luResult;

		//��ȡ����ʾ�豸��Ϣ
		memcpy_s(pstAppContext, sizeof(EI_APP_CONTEXT), loMsg.Data.Item.MsgBuf, sizeof(EI_APP_CONTEXT));

		luResult = ERROR_SUCCESS;

	} while (false);

	return luResult;
}


// ��û��ƻ���
// һ��Ӧ��ʵ���������������ƻ��棬ͨ�����ñ��������һ�����ƻ��棬�ڻ��ƻ����ύ��ϵͳˢ�»���֮ǰ���޸Ļ��ƻ��治�ᵼ��Eink��Ļ��ʾ������
// ���أ����ؿ�ָ��NULL����ʾʧ�ܣ���Ϊ��Դ�ľ������ط�NULLL����ʾ�ɹ�������ֵ�����½��Ļ���
EI_BUFFER* EiGetDrawBuffer(
	BOOL bInit,		// �Ƿ񽫻�������
	BOOL bCopy		// �Ƿ񽫵�ǰEink�����ݸ��Ƶ�����
) {
	EI_BUFFER* lpBuffer = NULL;

	do
	{
		//��ȡbuffer
		lpBuffer = glAppBufferMgr.GetBuffer();
		if (lpBuffer == NULL)
			break;

		if (bInit != FALSE)
		{
			//���
			ZeroMemory(lpBuffer->Buf, lpBuffer->ulBufferSize);
		}

		//���ÿ��
		ULONG lulWidth = lpBuffer->ulWidth;
		ULONG lulHeight = lpBuffer->ulHeight;
		if (gldwOrientation == GIR_90 || gldwOrientation == GIR_270)
		{
			lpBuffer->ulWidth = min(lulWidth, lulHeight);
			lpBuffer->ulHeight = max(lulWidth, lulHeight);
		}
		else
		{
			lpBuffer->ulWidth = max(lulWidth, lulHeight);
			lpBuffer->ulHeight = min(lulWidth, lulHeight);
		}
	} while (false);

	return lpBuffer;
}

// �ͷŻ��ƻ���
// û���ύ��ϵͳ���ƵĻ��ƻ��棬������ʹ��ʱ����Ҫ���ñ������ͷ���
void EiReleaseDrawBuffer(
	EI_BUFFER* pstBuffer		// ָ����ƻ���
)
{
	glAppBufferMgr.FreeBuffer(pstBuffer);

	//HeapFree(GetProcessHeap(), 0, pstBuffer);
}


// ���ͻ������󵽷�����
DWORD EiSendDrawRequest(
	EI_RECT* pstArea,	// indicates the area to draw
	EI_BUFFER* pstBuffer,		// ָ����ƻ���
	DWORD nuMsgID
)
{
	DWORD luResult = 0;
	EI_RECT ldUpdate = { 0,0,0,0 };

	do
	{
		if (pstArea != NULL)
			ldUpdate = *pstArea;

		if (ldUpdate.w == 0) ldUpdate.w = gldSysData.uiWidth;
		if (ldUpdate.h == 0) ldUpdate.h = gldSysData.uiHeight;

		EI_MSG_APP_DRAW ldRegAppDraw;
		ldRegAppDraw.Area = ldUpdate;
		ldRegAppDraw.BufferOffset = glAppBufferMgr.GetBufferOffset(pstBuffer);

		// ���ͻ�����Ϣ������
		CEiSvrMsgItem loMsg;
		loMsg.Data.Item.AppId = GetCurrentProcessId(); // AppIDֱ��ȡ�������ID
		loMsg.Data.Item.MsgId = nuMsgID;
		ULONG luMsgBufSize = sizeof(EI_MSG_APP_DRAW);
		memcpy_s(loMsg.Data.Item.MsgBuf, luMsgBufSize, &ldRegAppDraw, luMsgBufSize);
		loMsg.Data.Item.BufSize = luMsgBufSize;
		//�Ȱѵ�ǰ���ڵ��л�������
		//gloCenter.RecallMessage(loMsg);
		luResult = gloCenter.SendMessageToService(loMsg, pstBuffer);
		if (luResult != ERROR_SUCCESS)
			break;

	} while (false);

	return luResult;
}

// �������ݵ�Eink��
// �������е����ݻ��Ƶ�Eink��Ļ�ϣ����ô˺����󣬴���Ļ��ƻ���ᱻ�Զ��ͷţ�Ӧ�ò��ܼ���ʹ����黺��
// ����x,y,w,h��ʾ��Ҫ���µ�����������ȫ�����ƻ��Ǿֲ����ƣ����ƻ����е�����ʼ�ն�Ӧ����������Ļ
// ���أ� ���ط��㣬��ʾ�ɹ��������㣬��ʾʧ�ܣ�
DWORD EiDraw(
	EI_RECT* pstArea,	// indicates the area to draw
	EI_BUFFER* pstBuffer		// ָ����ƻ���
)
{
	return EiSendDrawRequest(pstArea, pstBuffer, EMHOSTID_DRAW);
}

// �ϴ�����ͼ
DWORD EiStoreCoverImage(
	EI_RECT* pstArea,	// indicates the area to draw
	EI_BUFFER* pstBuffer		// ָ����ƻ���
)
{
	return EiSendDrawRequest(pstArea, pstBuffer, EMHOSTID_STORE_COVER_IMAGE);
}

// ֪ͨservice���Ʒ���
DWORD EiDrawCoverImage(BOOL bCleanup)
{
	CEiSvrMsgItem loMsg;
	loMsg.Data.Item.AppId = GetCurrentProcessId(); // AppIDֱ��ȡ�������ID
	loMsg.Data.Item.MsgId = EMHOSTID_DRAW_COVER;

	ULONG luMsgBufSize = (ULONG)sizeof(BOOL);
	memcpy_s(loMsg.Data.Item.MsgBuf, luMsgBufSize, &bCleanup, luMsgBufSize);
	loMsg.Data.Item.BufSize = luMsgBufSize;

	return gloCenter.SendMessageToService(loMsg);
}

// ���Eink��Ļ
// Clean up E-ink screen
// return:
//		na
void EiCleanupScreen(
	unsigned char ucBackgroundColor		// the background color of E-ink screen after cleaning up
)
{
	CEiSvrMsgItem loMsg;
	loMsg.Data.Item.AppId = GetCurrentProcessId(); // AppIDֱ��ȡ�������ID
	loMsg.Data.Item.MsgId = EMHOSTID_CLEANUP_SCREEN;
	loMsg.Data.Item.MsgBuf[0] = ucBackgroundColor;
	loMsg.Data.Item.BufSize = 1;

	gloCenter.SendMessageToService(loMsg);
}


// Fuction Switcherʹ�õĻ��ƽӿ�
// ��ͬ��EiDraw�������˴���pstBuffer������������������Ļ�����������ǽ�������Switcher��������
//    ���ң��˴���Buffer��ÿһ���ֽڵ����λ��ʾ�������Ƿ���ʾ����
DWORD EiSwitcherDraw(
	EI_BUFFER* pstBuffer		// image buffer 
)
{
	EI_RECT ldDummy = { 0,0,0,0 };

	return EiSendDrawRequest(&ldDummy, pstBuffer, EMHOSTID_SWITCHER_DRAW);
}

// ����Switcher�Ĵ�С
void SetSwitcherLocation(
	const EI_RECT* npLocation // ����x��ŵ���������Ҳ�ľ���
)
{
	CEiSvrMsgItem loMsg;
	loMsg.Data.Item.AppId = GetCurrentProcessId(); // AppIDֱ��ȡ�������ID
	loMsg.Data.Item.MsgId = EMHOSTID_SWITCHER_LOCATION;
	ULONG luMsgBufSize = sizeof(EI_RECT);
	memcpy_s(loMsg.Data.Item.MsgBuf, luMsgBufSize, npLocation, luMsgBufSize);
	loMsg.Data.Item.BufSize = luMsgBufSize;

	gloCenter.SendMessageToService(loMsg);

}

// ����Switcher�Ŀ�������
void SetSwitcherShowArea(
	const EI_RECT* npArea // ����x��ŵ���������Ҳ�ľ���
)
{
	CEiSvrMsgItem loMsg;
	loMsg.Data.Item.AppId = GetCurrentProcessId(); // AppIDֱ��ȡ�������ID
	loMsg.Data.Item.MsgId = EMHOSTID_SWITCHER_SHOW_AREA;
	ULONG luMsgBufSize = sizeof(EI_RECT);
	memcpy_s(loMsg.Data.Item.MsgBuf, luMsgBufSize, npArea, luMsgBufSize);
	loMsg.Data.Item.BufSize = luMsgBufSize;

	gloCenter.SendMessageToService(loMsg);
}

// ֪ͨϵͳ�ػ�����ͼƬ��·��
// ���أ� ���ط��㣬��ʾ�ɹ��������㣬��ʾʧ�ܣ�
DWORD EiSetShutdownCover(
	const wchar_t* npszFilePath	// Ŀ���ļ���
)
{
	DWORD luResult = 0;

	do
	{
		if (npszFilePath == NULL)
			break;

		CEiSvrMsgItem loMsg;
		loMsg.Data.Item.AppId = GetCurrentProcessId(); // AppIDֱ��ȡ�������ID
		loMsg.Data.Item.MsgId = EMHOSTID_SET_SHUTDOWN_COVER;

		ULONG luMsgBufSize = (ULONG)(wcslen(npszFilePath) + 1) * sizeof(wchar_t);
		memcpy_s(loMsg.Data.Item.MsgBuf, luMsgBufSize, npszFilePath, luMsgBufSize);
		loMsg.Data.Item.BufSize = luMsgBufSize;

		luResult = gloCenter.SendMessageToService(loMsg);
		if (luResult != ERROR_SUCCESS)
			break;

	} while (false);

	return luResult;
}

// ��ʹϵͳ����һ��WM_EI_DRAW��Ϣ����ǰAPP
// ��дAPPʱ�����Խ����еĻ��ƶ����ڴ���WM_EI_DRAW��Ϣ�Ĺ����У����APP�ڴ���������һ�λ��Ʋ�����ֻ��Ҫ���ñ���������ʹϵͳ����һ��WM_EI_DRAW
// ��Ϣ��APP���Ӷ����ܹ������Ʋ���������һ���ط����У�WM_EI_DRAW������̣�
void EiInvalidPanel(
	EI_RECT* pstArea	// indicates the area to update
)
{
	// ���ͻ�����Ϣ�������ɷ����������Ƿ��ػ�
	CEiSvrMsgItem loMsg;
	loMsg.Data.Item.AppId = GetCurrentProcessId(); // AppIDֱ��ȡ�������ID
	loMsg.Data.Item.MsgId = EMHOSTID_INVALID_PANEL;
	ULONG luMsgBufSize = sizeof(EI_RECT);
	if (pstArea != NULL)
		memcpy_s(loMsg.Data.Item.MsgBuf, luMsgBufSize, pstArea, luMsgBufSize);
	else
		RtlZeroMemory(loMsg.Data.Item.MsgBuf, luMsgBufSize);
	loMsg.Data.Item.BufSize = luMsgBufSize;

	gloCenter.PostMessageToService(loMsg);
}


// ����Handwritingģʽ
// ���أ� �����㣬��ʾ�ɹ������ط��㣬��ʾ�����룻
// return
//		zero: success
//		non-zero: error code
DWORD EiSetHandWritingMode(
	DWORD eMode // ������˳���дģʽ���ο�GIHW_AUTO
) {

	DWORD luResult = ERROR_SUCCESS;

	do
	{
		// ע��App
		CEiSvrMsgItem loMsg;
		loMsg.Data.Item.AppId = GetCurrentProcessId(); // AppIDֱ��ȡ�������ID
		loMsg.Data.Item.MsgId = EMHOSTID_SET_SCENARIO;


		ULONG luMsgBufSize = (ULONG)sizeof(DWORD);
		memcpy_s(loMsg.Data.Item.MsgBuf, luMsgBufSize, &eMode, luMsgBufSize);
		loMsg.Data.Item.BufSize = luMsgBufSize;

		luResult = gloCenter.SendMessageToService(loMsg);
		if (luResult != ERROR_SUCCESS)
			break;

	} while (false);

	return luResult;
}

// ���ñʿ���δ֧��
// set width of pen in handwriting mode.
// ���أ� �����㣬��ʾ�ɹ������ط��㣬��ʾ�����룻
// return
//		zero: success
//		non-zero: error code
DWORD EiSetHandwritingPen(
	DWORD dwMin,	// ��С�ıʿ�	// min pen width
	DWORD dwMax	// ���ıʿ�	// max pen width
)
{
	return ERROR_NOT_READY;
}

// ������Ļ��ʾ����
// set the screen orientation for current app
// ���أ� �����㣬��ʾ�ɹ������ط��㣬��ʾ�����룻
// return
//		zero: success
//		non-zero: error code
DWORD EiSetScreenOrient(
	DWORD eOrientation // refer to GIR_NONE
)
{
	DWORD luResult = ERROR_NOT_READY;

	do
	{
		CEiSvrMsgItem loMsg;
		loMsg.Data.Item.AppId = GetCurrentProcessId(); // AppIDֱ��ȡ�������ID
		loMsg.Data.Item.MsgId = EMHOSTID_SET_DISPLAY_ORIENTATION;


		ULONG luMsgBufSize = (ULONG)sizeof(eOrientation);
		memcpy_s(loMsg.Data.Item.MsgBuf, luMsgBufSize, &eOrientation, luMsgBufSize);
		loMsg.Data.Item.BufSize = luMsgBufSize;

		luResult = gloCenter.SendMessageToService(loMsg);
		if (luResult != ERROR_SUCCESS)
			break;

		//�Լ���������ʡ��ÿ���ҷ���Ҫ
		gldwOrientation = eOrientation;

	} while (false);

	return luResult;
}

// ֹͣ����
// ��һ��Ӧ��ʵ����Ҫ�˳�ʱ�������������
void EiEnd(void) {
	do
	{
		// ֹͣ��������״̬��������Ϣ
		if (ghNoticeNormalRunThread != 0)
		{
			CloseHandle(ghNoticeNormalRunThread);
			ghNoticeNormalRunThread = 0;
		}

		// ע��App
		CEiSvrMsgItem loMsg;
		loMsg.Data.Item.AppId = GetCurrentProcessId(); // AppIDֱ��ȡ�������ID
		loMsg.Data.Item.MsgId = EMHOSTID_UNREG_APP;
		gloCenter.PostMessageToService(loMsg);

		//�ȴ�һ�£��������û�йر��ļ������ɾ�������ļ���
		Sleep(50);

	} while (false);

}

// �ڲ�����������һ��APP
// ֪ͨϵͳ����һ��ָ����APP
// ���أ� ���ط��㣬��ʾ�ɹ��������㣬��ʾʧ�ܣ�
DWORD EiRunAPP(
	const wchar_t* npszFilePath,	// Ŀ���ļ���
	int niSession				//����ָ���������ĸ�session,0��ʾsystem�û�
) {
	DWORD luResult = 0;

	do
	{
		if (npszFilePath == NULL)
			break;

		//������ǰ���ڵ� EMHOSTID_RUN_APP ����
		CEiSvrMsgItem loRecallMsg;
		loRecallMsg.Data.Item.AppId = GetCurrentProcessId(); // AppIDֱ��ȡ�������ID
		loRecallMsg.Data.Item.MsgId = EMHOSTID_RECALL_MSG;
		ULONG luRecallMsgId = EMHOSTID_RUN_APP;
		memcpy_s(loRecallMsg.Data.Item.MsgBuf, sizeof(ULONG), &luRecallMsgId, sizeof(ULONG));
		loRecallMsg.Data.Item.BufSize = sizeof(ULONG);

		gloCenter.RecallMessage(loRecallMsg);

		// ע��App
		CEiSvrMsgItem loMsg;
		loMsg.Data.Item.AppId = GetCurrentProcessId(); // AppIDֱ��ȡ�������ID
		loMsg.Data.Item.MsgId = EMHOSTID_RUN_APP;

		EI_MSG_RUN_APP ldRunInfo;
		wcscpy_s(ldRunInfo.FilePath, MAX_PATH, npszFilePath);
		ldRunInfo.SessionID = niSession;
		ULONG luMsgBufSize = sizeof(EI_MSG_RUN_APP);
		memcpy_s(loMsg.Data.Item.MsgBuf, luMsgBufSize, &ldRunInfo, luMsgBufSize);
		loMsg.Data.Item.BufSize = luMsgBufSize;

		//��������
		luResult = gloCenter.SendMessageToService(loMsg);

		if (luResult != ERROR_SUCCESS)
			break;

	} while (false);

	return luResult;
}

// ����������ϵת������ʾ����ϵ
// Convert a point from physical-coordinate to display-coordinate
void EiPhysicalToDisplay(
	IN EI_POINT* pstPointP,
	OUT EI_POINT* pstPointD
)
{
	// ��ȡ��ǰ��Ļ��ʾ���򣬶����Դ˼�������ת�� to ???ax			
	pstPointD->x = pstPointP->x;
	pstPointD->y = pstPointP->y;
}

// ����ʾ����ϵת������������ϵ
// Convert a point from display-coordinate to physical-coordinate
void EiDisplayToPhysical(
	IN EI_POINT* pstPointD,
	OUT EI_POINT* pstPointP
)
{
	// ��ȡ��ǰ��Ļ��ʾ���򣬶����Դ˼�������ת�� to ???ax
	pstPointD->x = pstPointP->x;
	pstPointD->y = pstPointP->y;
}

// ����FWģʽ
DWORD EiSetWaveformMode(
	DWORD dwMode
) {

	DWORD luResult = ERROR_NOT_READY;

	do
	{
		// ע��App
		CEiSvrMsgItem loMsg;
		loMsg.Data.Item.AppId = GetCurrentProcessId(); // AppIDֱ��ȡ�������ID
		loMsg.Data.Item.MsgId = EMHOSTID_SET_FW_MODE;


		ULONG luMsgBufSize = (ULONG)sizeof(DWORD);
		memcpy_s(loMsg.Data.Item.MsgBuf, luMsgBufSize, &dwMode, luMsgBufSize);
		loMsg.Data.Item.BufSize = luMsgBufSize;

		luResult = gloCenter.SendMessageToService(loMsg);
		if (luResult != ERROR_SUCCESS)
			break;

	} while (false);

	return luResult;
}

// ֪ͨϵͳ����ͼƬ��·��
// ���أ� ���ط��㣬��ʾ�ɹ��������㣬��ʾʧ�ܣ�
DWORD EiSetKeyboardImg(
	const wchar_t* npszFilePath	// Ŀ���ļ���
) {
	DWORD luResult = 0;

	do
	{
		if (npszFilePath == NULL)
			break;

		CEiSvrMsgItem loMsg;
		loMsg.Data.Item.AppId = GetCurrentProcessId(); // AppIDֱ��ȡ�������ID
		loMsg.Data.Item.MsgId = EMHOSTID_SET_KEYBOARD_PATH;

		ULONG luMsgBufSize = (ULONG)(wcslen(npszFilePath) + 1) * sizeof(wchar_t);
		memcpy_s(loMsg.Data.Item.MsgBuf, luMsgBufSize, npszFilePath, luMsgBufSize);
		loMsg.Data.Item.BufSize = luMsgBufSize;

		luResult = gloCenter.PostMessageToService(loMsg);
		if (luResult != ERROR_SUCCESS)
			break;

	} while (false);

	return luResult;
}

// ֪ͨϵͳ��ʾ״̬Ҫ�����仯
// ���أ� ���ط��㣬��ʾ�ɹ��������㣬��ʾʧ�ܣ�
// 1��ʾ���� 2��ʾ����A 3��ʾ����B
DWORD EiSetShowStatus(
	ULONG nulStatus	// ״̬
) {
	DWORD luResult = 0;

	do
	{
		CEiSvrMsgItem loMsg;
		loMsg.Data.Item.AppId = GetCurrentProcessId(); // AppIDֱ��ȡ�������ID
		loMsg.Data.Item.MsgId = EMHOSTID_SET_SHOW_STATUS;

		ULONG luMsgBufSize = (ULONG)sizeof(ULONG);
		memcpy_s(loMsg.Data.Item.MsgBuf, luMsgBufSize, &nulStatus, luMsgBufSize);
		loMsg.Data.Item.BufSize = luMsgBufSize;

		luResult = gloCenter.SendMessageToService(loMsg);
		if (luResult != ERROR_SUCCESS)
			break;

	} while (false);

	return luResult;
}

// Enable / disable direct handwriting region, which means handwriting is handled by tcon.
// To disable it, set both width and height = 0.
// Direct handwriting can only be used in GIHW_HANDWRITING mode.
//
// ���أ� �����㣬��ʾ�ɹ������ط��㣬��ʾ�����룻
// return
//		zero: success
//		non-zero: error code
DWORD EiSetHandwritingRect(
	EI_RECT dRect
) {
	DWORD luResult = 0;

	do
	{
		CEiSvrMsgItem loMsg;
		loMsg.Data.Item.AppId = GetCurrentProcessId(); // AppIDֱ��ȡ�������ID
		loMsg.Data.Item.MsgId = EMHOSTID_SET_HANDWRITING_RECT;

		ULONG luMsgBufSize = (ULONG)sizeof(EI_RECT);
		memcpy_s(loMsg.Data.Item.MsgBuf, luMsgBufSize, &dRect, luMsgBufSize);
		loMsg.Data.Item.BufSize = luMsgBufSize;

		luResult = gloCenter.SendMessageToService(loMsg);
		if (luResult != ERROR_SUCCESS)
			break;

	} while (false);

	return luResult;
}

// Check DisplayArea engine status.
//
// ���أ� ����TRUE����ʾ���С�����FALSE����ʾ���Ʊ�ռ��;
// return
//		TRUE: ready
//		FALSE: not ready
BOOL EiCheckDpyReady()
{
	DWORD luResult = 0;
	BOOL lbRet = FALSE;

	do
	{
		CEiSvrMsgItem loMsg;
		loMsg.Data.Item.AppId = GetCurrentProcessId(); // AppIDֱ��ȡ�������ID
		loMsg.Data.Item.MsgId = EMHOSTID_GET_DRAW_READY;

		loMsg.Data.Item.BufSize = 0;

		luResult = gloCenter.SendMessageToService(loMsg);
		if (luResult != ERROR_SUCCESS)
			break;

		memcpy_s(&lbRet, sizeof(BOOL), loMsg.Data.Item.MsgBuf, sizeof(BOOL));

	} while (false);

	return lbRet;
}

// Set PartialUpdate feature
//
// ���أ� �����㣬��ʾ�ɹ������ط��㣬��ʾ�����룻
// return
//		zero: success
//		non-zero: error code
DWORD EiSetPartialUpdate(
	BOOL enable
)
{
	DWORD luResult = 0;

	do
	{
		CEiSvrMsgItem loMsg;
		loMsg.Data.Item.AppId = GetCurrentProcessId(); // AppIDֱ��ȡ�������ID
		loMsg.Data.Item.MsgId = EMHOSTID_SET_PARTIAL_ENABLE;

		ULONG luMsgBufSize = (ULONG)sizeof(BOOL);
		memcpy_s(loMsg.Data.Item.MsgBuf, luMsgBufSize, &enable, luMsgBufSize);
		loMsg.Data.Item.BufSize = luMsgBufSize;

		luResult = gloCenter.SendMessageToService(loMsg);
		if (luResult != ERROR_SUCCESS)
			break;

	} while (false);

	return luResult;
}

DWORD EiSetScenario(DWORD dwScenario)
{
	CEiSvrMsgItem msg;
	msg.Data.Item.AppId = GetCurrentProcessId();
	msg.Data.Item.MsgId = EMHOSTID_SET_SCENARIO;
	msg.Data.Item.BufSize = sizeof(DWORD);
	memcpy_s(msg.Data.Item.MsgBuf, sizeof(DWORD), &dwScenario, sizeof(DWORD));
	auto result = gloCenter.SendMessageToService(msg);
	return result;
}

// Get scenario
//
// ���أ� �����㣬��ʾ�ɹ������ط��㣬��ʾ�����룻
// return
//		zero: success
//		non-zero: error code
DWORD EiGetScenario(DWORD& rdwCurrentMode)
{
	DWORD luResult = 0;

	do
	{
		CEiSvrMsgItem loMsg;
		loMsg.Data.Item.AppId = GetCurrentProcessId(); // AppIDֱ��ȡ�������ID
		loMsg.Data.Item.MsgId = EMHOSTID_GET_SCENARIO_MODE;

		loMsg.Data.Item.BufSize = 0;

		luResult = gloCenter.SendMessageToService(loMsg);
		if (luResult != ERROR_SUCCESS)
			break;

		memcpy_s(&rdwCurrentMode, sizeof(DWORD), loMsg.Data.Item.MsgBuf, sizeof(DWORD));

	} while (false);

	return luResult;
}

// ֪ͨϵͳ������̬�����仯
// ���أ� ���ط��㣬��ʾ�ɹ��������㣬��ʾʧ�ܣ�
// 2�ʼǱ���̬ 3tent��̬
DWORD EiSetLaptopMode(
	ULONG nulMode	// ģʽ
)
{
	DWORD luResult = 0;

	do
	{
		//������ǰ���ڵ� EMHOSTID_LAPTOP_MODE_CHANGED ����
		CEiSvrMsgItem loRecallMsg;
		loRecallMsg.Data.Item.AppId = GetCurrentProcessId(); // AppIDֱ��ȡ�������ID
		loRecallMsg.Data.Item.MsgId = EMHOSTID_RECALL_MSG;
		ULONG luRecallMsgId = EMHOSTID_LAPTOP_MODE_CHANGED;
		memcpy_s(loRecallMsg.Data.Item.MsgBuf, sizeof(ULONG), &luRecallMsgId, sizeof(ULONG));
		loRecallMsg.Data.Item.BufSize = sizeof(ULONG);

		gloCenter.RecallMessage(loRecallMsg);

		//���� EMHOSTID_LAPTOP_MODE_CHANGED ����
		CEiSvrMsgItem loMsg;
		loMsg.Data.Item.AppId = GetCurrentProcessId(); // AppIDֱ��ȡ�������ID
		loMsg.Data.Item.MsgId = EMHOSTID_LAPTOP_MODE_CHANGED;

		ULONG luMsgBufSize = sizeof(ULONG);
		memcpy_s(loMsg.Data.Item.MsgBuf, luMsgBufSize, &nulMode, luMsgBufSize);
		loMsg.Data.Item.BufSize = luMsgBufSize;

		luResult = gloCenter.SendMessageToService(loMsg);
		if (luResult != ERROR_SUCCESS)
			break;

	} while (false);

	return luResult;
}

// ֪ͨϵͳ���̰���������״̬
// ���أ� ���ط��㣬��ʾ�ɹ��������㣬��ʾʧ�ܣ�
// -1������ 0�ر� 1����
DWORD EiSetKeyboardDownSounds(
	bool nbIsSet
) {
	DWORD luResult = 0;

	do
	{
		//������ǰ���ڵ� EMHOSTID_SET_KEYBOARD_DOWN ����
		CEiSvrMsgItem loRecallMsg;
		loRecallMsg.Data.Item.AppId = GetCurrentProcessId(); // AppIDֱ��ȡ�������ID
		loRecallMsg.Data.Item.MsgId = EMHOSTID_RECALL_MSG;
		ULONG luRecallMsgId = EMHOSTID_LAPTOP_MODE_CHANGED;
		memcpy_s(loRecallMsg.Data.Item.MsgBuf, sizeof(ULONG), &luRecallMsgId, sizeof(ULONG));
		loRecallMsg.Data.Item.BufSize = sizeof(ULONG);

		gloCenter.RecallMessage(loRecallMsg);

		//���� EMHOSTID_SET_KEYBOARD_DOWN ����
		CEiSvrMsgItem loMsg;
		loMsg.Data.Item.AppId = GetCurrentProcessId(); // AppIDֱ��ȡ�������ID
		loMsg.Data.Item.MsgId = EMHOSTID_SET_KEYBOARD_DOWN;

		ULONG luMsgBufSize = sizeof(bool);
		memcpy_s(loMsg.Data.Item.MsgBuf, luMsgBufSize, &nbIsSet, luMsgBufSize);
		loMsg.Data.Item.BufSize = luMsgBufSize;

		luResult = gloCenter.SendMessageToService(loMsg);
		if (luResult != ERROR_SUCCESS)
			break;

	} while (false);

	return luResult;
}


// ֪ͨϵͳ���̰���������״̬
// ���أ� ���ط��㣬��ʾ�ɹ��������㣬��ʾʧ�ܣ�
// -1������ 0�ر� 1����
DWORD EiSetKeyboardUpSounds(
	bool nbIsSet
) {
	DWORD luResult = 0;

	do
	{
		//������ǰ���ڵ� EMHOSTID_SET_KEYBOARD_UP ����
		CEiSvrMsgItem loRecallMsg;
		loRecallMsg.Data.Item.AppId = GetCurrentProcessId(); // AppIDֱ��ȡ�������ID
		loRecallMsg.Data.Item.MsgId = EMHOSTID_RECALL_MSG;
		ULONG luRecallMsgId = EMHOSTID_SET_KEYBOARD_UP;
		memcpy_s(loRecallMsg.Data.Item.MsgBuf, sizeof(ULONG), &luRecallMsgId, sizeof(ULONG));
		loRecallMsg.Data.Item.BufSize = sizeof(ULONG);

		gloCenter.RecallMessage(loRecallMsg);

		//���� EMHOSTID_SET_KEYBOARD_UP ����
		CEiSvrMsgItem loMsg;
		loMsg.Data.Item.AppId = GetCurrentProcessId(); // AppIDֱ��ȡ�������ID
		loMsg.Data.Item.MsgId = EMHOSTID_SET_KEYBOARD_UP;

		ULONG luMsgBufSize = sizeof(bool);
		memcpy_s(loMsg.Data.Item.MsgBuf, luMsgBufSize, &nbIsSet, luMsgBufSize);
		loMsg.Data.Item.BufSize = luMsgBufSize;

		luResult = gloCenter.PostMessageToService(loMsg);
		if (luResult != ERROR_SUCCESS)
			break;

	} while (false);

	return luResult;
}

// Set Homebar status
//
// ���أ� �����㣬��ʾ�ɹ������ط��㣬��ʾ�����룻
// return
//		zero: success
//		non-zero: error code
DWORD EiSetHomebarStatus(
	ULONG lulStatus   //GI_HOMEBAR_HIDE/ GI_HOMEBAR_SHOW / GI_HOMEBAR_EXPAND / GI_HOMEBAR_COLLAPSE
) {
	DWORD luResult = 0;

	do
	{
		CEiSvrMsgItem loMsg;
		loMsg.Data.Item.AppId = GetCurrentProcessId(); // AppIDֱ��ȡ�������ID
		loMsg.Data.Item.MsgId = EMHOSTID_SET_HOMEBAR_STATUS;

		ULONG luMsgBufSize = sizeof(ULONG);
		memcpy_s(loMsg.Data.Item.MsgBuf, luMsgBufSize, &lulStatus, luMsgBufSize);
		loMsg.Data.Item.BufSize = luMsgBufSize;

		luResult = gloCenter.SendMessageToService(loMsg);
		if (luResult != ERROR_SUCCESS)
			break;

	} while (false);

	return luResult;
}

// OOBE complete
//
// ���أ� �����㣬��ʾ�ɹ������ط��㣬��ʾ�����룻
// return
//		zero: success
//		non-zero: error code
DWORD EiOOBEComplete()
{
	DWORD luResult = 0;

	do
	{
		CEiSvrMsgItem loMsg;
		loMsg.Data.Item.AppId = GetCurrentProcessId(); // AppIDֱ��ȡ�������ID
		loMsg.Data.Item.MsgId = EMHOSTID_OOBE_COMPLETE;

		ULONG luMsgBufSize = 0;

		luResult = gloCenter.PostMessageToService(loMsg);
		if (luResult != ERROR_SUCCESS)
			break;

	} while (false);

	return luResult;
}

// OOBE cstart
//
// ���أ� �����㣬��ʾ�ɹ������ط��㣬��ʾ�����룻
// return
//		zero: success
//		non-zero: error code
DWORD EiOOBEStart()
{
	DWORD luResult = 0;

	do
	{
		CEiSvrMsgItem loMsg;
		loMsg.Data.Item.AppId = GetCurrentProcessId(); // AppIDֱ��ȡ�������ID
		loMsg.Data.Item.MsgId = EMHOSTID_OOBE_START;

		ULONG luMsgBufSize = 0;

		luResult = gloCenter.PostMessageToService(loMsg);
		if (luResult != ERROR_SUCCESS)
			break;

	} while (false);

	return luResult;
}

// ��ȡϵͳ������������С
// ���أ�������С(0-100)
DWORD EiGetKeySoundsVolume(void)
{
	ULONG luResult = ERROR_NOT_READY;
	LONG Volume = 0;

	do
	{
		//������ǰ���ڵ� EMHOSTID_GET_KEYBOARD_VOLUME ����
		CEiSvrMsgItem loRecallMsg;
		loRecallMsg.Data.Item.AppId = GetCurrentProcessId(); // AppIDֱ��ȡ�������ID
		loRecallMsg.Data.Item.MsgId = EMHOSTID_RECALL_MSG;
		ULONG luRecallMsgId = EMHOSTID_GET_KEYBOARD_VOLUME;
		memcpy_s(loRecallMsg.Data.Item.MsgBuf, sizeof(ULONG), &luRecallMsgId, sizeof(ULONG));
		loRecallMsg.Data.Item.BufSize = sizeof(ULONG);

		gloCenter.RecallMessage(loRecallMsg);

		//�������ͻ�ȡ��Ϣ
		CEiSvrMsgItem loMsg;
		loMsg.Data.Item.AppId = GetCurrentProcessId(); // AppIDֱ��ȡ�������ID
		loMsg.Data.Item.MsgId = EMHOSTID_GET_KEYBOARD_VOLUME;
		loMsg.Data.Item.BufSize = 0;

		luResult = gloCenter.SendMessageToService(loMsg);
		if (luResult != ERROR_SUCCESS)
			return 0;

		//��ȡֵ
		Volume = *(LONG*)loMsg.Data.Item.MsgBuf;
	} while (false);

	return Volume;
}

// ����ϵͳ������������С
// ���أ���
void EiSetKeySoundsVolume(LONG nlVolume)//(0-100)
{
	DWORD luResult = 0;

	do
	{
		//������ǰ���ڵ� EMHOSTID_SET_KEYBOARD_VOLUME ����
		CEiSvrMsgItem loRecallMsg;
		loRecallMsg.Data.Item.AppId = GetCurrentProcessId(); // AppIDֱ��ȡ�������ID
		loRecallMsg.Data.Item.MsgId = EMHOSTID_RECALL_MSG;
		ULONG luRecallMsgId = EMHOSTID_SET_KEYBOARD_VOLUME;
		memcpy_s(loRecallMsg.Data.Item.MsgBuf, sizeof(ULONG), &luRecallMsgId, sizeof(ULONG));
		loRecallMsg.Data.Item.BufSize = sizeof(ULONG);

		gloCenter.RecallMessage(loRecallMsg);

		//���� EMHOSTID_SET_KEYBOARD_VOLUME ����
		CEiSvrMsgItem loMsg;
		loMsg.Data.Item.AppId = GetCurrentProcessId(); // AppIDֱ��ȡ�������ID
		loMsg.Data.Item.MsgId = EMHOSTID_SET_KEYBOARD_VOLUME;

		ULONG luMsgBufSize = sizeof(nlVolume);
		memcpy_s(loMsg.Data.Item.MsgBuf, luMsgBufSize, &nlVolume, luMsgBufSize);
		loMsg.Data.Item.BufSize = luMsgBufSize;

		luResult = gloCenter.SendMessageToService(loMsg);
		if (luResult != ERROR_SUCCESS)
			break;

	} while (false);
}

// Set direct handwriting setting, e.g. line width, eraser
//
// Return value
//		zero: success
//		non-zero: error code
DWORD EiSetHandwritingSetting(BYTE lineWidth)
{
	DWORD luResult = 0;

	do
	{
		CEiSvrMsgItem loMsg;
		loMsg.Data.Item.AppId = GetCurrentProcessId(); // AppIDֱ��ȡ�������ID
		loMsg.Data.Item.MsgId = EMHOSTID_SET_HANDWRITING_LINE_WIDTH;

		ULONG luMsgBufSize = sizeof(BYTE);
		memcpy_s(loMsg.Data.Item.MsgBuf, luMsgBufSize, &lineWidth, luMsgBufSize);
		loMsg.Data.Item.BufSize = luMsgBufSize;

		luResult = gloCenter.PostMessageToService(loMsg);
		if (luResult != ERROR_SUCCESS)
			break;

	} while (false);

	return luResult;
}

// ��ȡ��ǰ������̬
// Get tablet mode
//
// ���أ� �����㣬��ʾ�ɹ������ط��㣬��ʾ�����룻
// return
//		zero: success
//		non-zero: error code
//rdwCurrentMode : GIR_MODE_LAPTOP/GIR_MODE_TENT/GIR_MODE_TABLET
DWORD EiGetTabletMode(DWORD& rdwCurrentMode)
{
	DWORD luResult = 0;

	do
	{
		CEiSvrMsgItem loMsg;
		loMsg.Data.Item.AppId = GetCurrentProcessId(); // AppIDֱ��ȡ�������ID
		loMsg.Data.Item.MsgId = EMHOSTID_GET_TABLET_MODE;

		loMsg.Data.Item.BufSize = 0;

		luResult = gloCenter.SendMessageToService(loMsg);
		if (luResult != ERROR_SUCCESS)
			break;

		memcpy_s(&rdwCurrentMode, sizeof(DWORD), loMsg.Data.Item.MsgBuf, sizeof(DWORD));

	} while (false);

	return luResult;
}

// ֪ͨϵͳhomebar��̬�����仯
// ���أ� ���ط��㣬��ʾ�ɹ��������㣬��ʾʧ�ܣ�
DWORD EiHomebarChanged(
	ULONG nulMode	// ģʽ
)
{
	DWORD luResult = 0;

	do
	{
		CEiSvrMsgItem loMsg;
		loMsg.Data.Item.AppId = GetCurrentProcessId(); // AppIDֱ��ȡ�������ID
		loMsg.Data.Item.MsgId = EMHOSTID_HOMEBAR_CHANGE;

		ULONG luMsgBufSize = sizeof(ULONG);
		memcpy_s(loMsg.Data.Item.MsgBuf, luMsgBufSize, &nulMode, luMsgBufSize);
		loMsg.Data.Item.BufSize = luMsgBufSize;

		luResult = gloCenter.PostMessageToService(loMsg);
		if (luResult != ERROR_SUCCESS)
			break;

	} while (false);

	return luResult;
}

// ֪ͨϵͳ��Ļ����
// ���أ� ���ط��㣬��ʾ�ɹ��������㣬��ʾʧ�ܣ�
DWORD EiSetOrientation(
	DWORD ndwOrientation	// ģʽ
)
{
	DWORD luResult = 0;

	do
	{
		//������ǰ���ڵ� EMHOSTID_ORIENTATION_CHANGED ����
		CEiSvrMsgItem loRecallMsg;
		loRecallMsg.Data.Item.AppId = GetCurrentProcessId(); // AppIDֱ��ȡ�������ID
		loRecallMsg.Data.Item.MsgId = EMHOSTID_RECALL_MSG;
		ULONG luRecallMsgId = EMHOSTID_ORIENTATION_CHANGED;
		memcpy_s(loRecallMsg.Data.Item.MsgBuf, sizeof(ULONG), &luRecallMsgId, sizeof(ULONG));
		loRecallMsg.Data.Item.BufSize = sizeof(ULONG);

		gloCenter.RecallMessage(loRecallMsg);

		//���� EMHOSTID_ORIENTATION_CHANGED ����
		CEiSvrMsgItem loMsg;
		loMsg.Data.Item.AppId = GetCurrentProcessId(); // AppIDֱ��ȡ�������ID
		loMsg.Data.Item.MsgId = EMHOSTID_ORIENTATION_CHANGED;

		DWORD luMsgBufSize = sizeof(DWORD);
		memcpy_s(loMsg.Data.Item.MsgBuf, luMsgBufSize, &ndwOrientation, luMsgBufSize);
		loMsg.Data.Item.BufSize = luMsgBufSize;

		luResult = gloCenter.PostMessageToService(loMsg);
		if (luResult != ERROR_SUCCESS)
			break;

	} while (false);

	return luResult;
}

// ֪ͨϵͳ
// ���أ� ���ط��㣬��ʾ�ɹ��������㣬��ʾʧ�ܣ�
void EiSetBCover(
	char* npszPath	// ģʽ
)
{
	do
	{
		if (npszPath == NULL)
			break;

		CEiSvrMsgItem loMsg;
		loMsg.Data.Item.AppId = GetCurrentProcessId(); // AppIDֱ��ȡ�������ID
		loMsg.Data.Item.MsgId = EMHOSTID_SET_BCOVER_PATH;

		ULONG luMsgBufSize = ULONG(sizeof(char) * (strlen(npszPath) + 1));
		memcpy_s(loMsg.Data.Item.MsgBuf, luMsgBufSize, npszPath, luMsgBufSize);
		loMsg.Data.Item.BufSize = luMsgBufSize;

		//��������
		gloCenter.SendMessageToService(loMsg);

	} while (false);

}

// ֪ͨϵͳB�������˳�˫��ģʽ
// ���أ� ���ط��㣬��ʾ�ɹ��������㣬��ʾʧ�ܣ�
void EiBCoverEnterDbTap(
	bool nbIsEnter
)
{
	do
	{
		CEiSvrMsgItem loMsg;
		loMsg.Data.Item.AppId = GetCurrentProcessId(); // AppIDֱ��ȡ�������ID
		loMsg.Data.Item.MsgId = EMHOSTID_B_ENTER_DBTAP;

		ULONG luMsgBufSize = sizeof(bool);
		memcpy_s(loMsg.Data.Item.MsgBuf, luMsgBufSize, &nbIsEnter, luMsgBufSize);
		loMsg.Data.Item.BufSize = luMsgBufSize;

		//��������
		gloCenter.SendMessageToService(loMsg);

	} while (false);
}

// ���ü�����ʽ
// Set keyboard style
//
// ���أ� �����㣬��ʾ�ɹ������ط��㣬��ʾ�����룻
// return
//		zero: success
//		non-zero: error code
DWORD EiSetKeyboardStyle(EI_KEYBOARD_STYLE ndStyle)
{
	DWORD luResult = 0;

	do
	{
		CEiSvrMsgItem loMsg;
		loMsg.Data.Item.AppId = GetCurrentProcessId(); // AppIDֱ��ȡ�������ID
		loMsg.Data.Item.MsgId = EMHOSTID_SET_KEYBOARD_STYLE;

		ULONG luMsgBufSize = (ULONG)sizeof(EI_KEYBOARD_STYLE);
		memcpy_s(loMsg.Data.Item.MsgBuf, luMsgBufSize, &ndStyle, luMsgBufSize);
		loMsg.Data.Item.BufSize = luMsgBufSize;

		luResult = gloCenter.PostMessageToService(loMsg);
		if (luResult != ERROR_SUCCESS)
			break;

	} while (false);

	return luResult;
}

// ��������������
// ���أ� ���ط��㣬��ʾ�ɹ��������㣬��ʾʧ�ܣ�
DWORD EiSetKeyboardSensitivity(
	LONG nlLevel
) {
	DWORD luResult = 0;

	do
	{
		CEiSvrMsgItem loMsg;
		loMsg.Data.Item.AppId = GetCurrentProcessId(); // AppIDֱ��ȡ�������ID
		loMsg.Data.Item.MsgId = EMHOSTID_SET_KEYBOARD_SENSITIVITY;

		ULONG luMsgBufSize = (ULONG)sizeof(LONG);
		memcpy_s(loMsg.Data.Item.MsgBuf, luMsgBufSize, &nlLevel, luMsgBufSize);
		loMsg.Data.Item.BufSize = luMsgBufSize;

		luResult = gloCenter.PostMessageToService(loMsg);
		if (luResult != ERROR_SUCCESS)
			break;

	} while (false);

	return luResult;
}

// ֪ͨhomebar��B��������Ϊ�˽���ϸ���Ļ�Զ������bug
// ���أ� ���ط��㣬��ʾ�ɹ��������㣬��ʾʧ�ܣ�
DWORD EiCloseBCover(
	BOOL nbClose
)
{
	DWORD luResult = 0;

	do
	{
		CEiSvrMsgItem loMsg;
		loMsg.Data.Item.AppId = GetCurrentProcessId(); // AppIDֱ��ȡ�������ID
		loMsg.Data.Item.MsgId = EMHOSTID_SET_CLOSE_B_COVER;

		ULONG luMsgBufSize = (ULONG)sizeof(BOOL);
		memcpy_s(loMsg.Data.Item.MsgBuf, luMsgBufSize, &nbClose, luMsgBufSize);
		loMsg.Data.Item.BufSize = luMsgBufSize;

		luResult = gloCenter.SendMessageToService(loMsg);
		if (luResult != ERROR_SUCCESS)
			break;

	} while (false);

	return luResult;
}

// ͨ��Service��¼����
// ���أ� ���ط��㣬��ʾ�ɹ��������㣬��ʾʧ��
DWORD EiRecordAppData(AppData& appData)
{
	DWORD luResult = 0;

	do
	{
		CEiSvrMsgItem loMsg;
		loMsg.Data.Item.AppId = GetCurrentProcessId(); // AppIDֱ��ȡ�������ID
		loMsg.Data.Item.MsgId = EMHOSTID_RECORD_APP_DATA;

		ULONG luMsgBufSize = (ULONG)sizeof(AppData);
		memcpy_s(loMsg.Data.Item.MsgBuf, luMsgBufSize, &appData, luMsgBufSize);
		loMsg.Data.Item.BufSize = luMsgBufSize;

		luResult = gloCenter.SendMessageToService(loMsg);
		if (luResult != ERROR_SUCCESS)
			break;

	} while (false);

	return luResult;
}

// ֪ͨ���񲥷ż���������Ϊ������
// ���أ� ���ط��㣬��ʾ�ɹ��������㣬��ʾʧ�ܣ�
DWORD EiPlayKeyboardSound(
	ULONG nlType
)
{
	DWORD luResult = 0;

	do
	{
		//������ǰ���ڵ� EMHOSTID_PLAY_KEYBOARD_SOUND ����
		CEiSvrMsgItem loRecallMsg;
		loRecallMsg.Data.Item.AppId = GetCurrentProcessId(); // AppIDֱ��ȡ�������ID
		loRecallMsg.Data.Item.MsgId = EMHOSTID_RECALL_MSG;
		ULONG luRecallMsgId = EMHOSTID_PLAY_KEYBOARD_SOUND;
		memcpy_s(loRecallMsg.Data.Item.MsgBuf, sizeof(ULONG), &luRecallMsgId, sizeof(ULONG));
		loRecallMsg.Data.Item.BufSize = sizeof(ULONG);

		gloCenter.RecallMessage(loRecallMsg);

		//���� EMHOSTID_PLAY_KEYBOARD_SOUND ����
		CEiSvrMsgItem loMsg;
		loMsg.Data.Item.AppId = GetCurrentProcessId(); // AppIDֱ��ȡ�������ID
		loMsg.Data.Item.MsgId = EMHOSTID_PLAY_KEYBOARD_SOUND;

		ULONG luMsgBufSize = (ULONG)sizeof(ULONG);
		memcpy_s(loMsg.Data.Item.MsgBuf, luMsgBufSize, &nlType, luMsgBufSize);
		loMsg.Data.Item.BufSize = luMsgBufSize;

		luResult = gloCenter.SendMessageToService(loMsg);
		if (luResult != ERROR_SUCCESS)
			break;

	} while (false);

	return luResult;
}

// ֪ͨ����ѻ�����̬�л���ƽ��ģʽ
// ���أ� ���ط��㣬��ʾ�ɹ��������㣬��ʾʧ�ܣ�
DWORD EiChangeTabletMode(
	bool nbIsTablet  //Ϊ���ʾ�л���ƽ�壬�����л����ʼǱ�
)
{
	DWORD luResult = 0;

	do
	{
		CEiSvrMsgItem loMsg;
		loMsg.Data.Item.AppId = GetCurrentProcessId(); // AppIDֱ��ȡ�������ID
		loMsg.Data.Item.MsgId = EMHOSTID_CHANGE_TABLET_MODE;

		ULONG luMsgBufSize = (ULONG)sizeof(bool);
		memcpy_s(loMsg.Data.Item.MsgBuf, luMsgBufSize, &nbIsTablet, luMsgBufSize);
		loMsg.Data.Item.BufSize = luMsgBufSize;

		luResult = gloCenter.SendMessageToService(loMsg);
		if (luResult != ERROR_SUCCESS)
			break;

	} while (false);

	return luResult;
}

// ����/�ر�knock knock����
// ���أ� ���ط��㣬��ʾ�ɹ��������㣬��ʾʧ�ܣ�
DWORD EiOpenKnockKnock(
	bool nbIsOpen  //Ϊ���ʾ��
)
{
	DWORD luResult = 0;

	do
	{
		CEiSvrMsgItem loMsg;
		loMsg.Data.Item.AppId = GetCurrentProcessId(); // AppIDֱ��ȡ�������ID
		loMsg.Data.Item.MsgId = EMHOSTID_OPEN_KNOCK_KNOCK;

		ULONG luMsgBufSize = (ULONG)sizeof(bool);
		memcpy_s(loMsg.Data.Item.MsgBuf, luMsgBufSize, &nbIsOpen, luMsgBufSize);
		loMsg.Data.Item.BufSize = luMsgBufSize;

		luResult = gloCenter.PostMessageToService(loMsg);
		if (luResult != ERROR_SUCCESS)
			break;

	} while (false);

	return luResult;
}

// ����C���ֻ������
// Set TP Area
//
// ���أ� �����㣬��ʾ�ɹ������ط��㣬��ʾ�����룻
// return
//		zero: success
//		non-zero: error code
DWORD EiSetTpArea(SET_TP_AREA ndTpArea)
{
	DWORD luResult = 0;

	do
	{
		CEiSvrMsgItem loMsg;
		loMsg.Data.Item.AppId = GetCurrentProcessId(); // AppIDֱ��ȡ�������ID
		loMsg.Data.Item.MsgId = EMHOSTID_SET_TP_AREA;

		ULONG luMsgBufSize = (ULONG)sizeof(SET_TP_AREA);
		memcpy_s(loMsg.Data.Item.MsgBuf, luMsgBufSize, &ndTpArea, luMsgBufSize);
		loMsg.Data.Item.BufSize = luMsgBufSize;

		luResult = gloCenter.SendMessageToService(loMsg);
		if (luResult != ERROR_SUCCESS)
			break;

	} while (false);

	return luResult;
}

// ֪ͨ������һ�µ�ǰ�л���Ӧ�������л�����û
// ���أ� ���ط��㣬��ʾ�ɹ��������㣬��ʾʧ�ܣ�
DWORD EiCheckAppStatus()
{
	DWORD luResult = 0;

	do
	{
		CEiSvrMsgItem loMsg;
		loMsg.Data.Item.AppId = GetCurrentProcessId(); // AppIDֱ��ȡ�������ID
		loMsg.Data.Item.MsgId = EMHOSTID_CHECK_APP_STATUS;

		loMsg.Data.Item.BufSize = 0;

		luResult = gloCenter.SendMessageToService(loMsg);
		if (luResult != ERROR_SUCCESS)
			break;

	} while (false);

	return luResult;
}

// ֪ͨ����֪ͨ��ǰӦ����������tp area
// ���أ� ���ط��㣬��ʾ�ɹ��������㣬��ʾʧ�ܣ�
DWORD EiResetTpArea()
{
	DWORD luResult = 0;

	do
	{
		CEiSvrMsgItem loMsg;
		loMsg.Data.Item.AppId = GetCurrentProcessId(); // AppIDֱ��ȡ�������ID
		loMsg.Data.Item.MsgId = EMHOSTID_RESET_TP_AREA;

		loMsg.Data.Item.BufSize = 0;

		luResult = gloCenter.PostMessageToService(loMsg);
		if (luResult != ERROR_SUCCESS)
			break;

	} while (false);

	return luResult;
}

// ��ȡ��˽Э�鿪��״̬
// Get the privacy protocol switch status.
//
// ���أ� �����㣬��ʾ�ɹ������ط��㣬��ʾ�����룻
// return
//		zero: success
//		non-zero: error code
//rdwStatus : 0 off,1 on
DWORD EiGetPrivacyStatus(DWORD& rdwStatus)
{
	DWORD luResult = 0;

	do
	{
		CEiSvrMsgItem loMsg;
		loMsg.Data.Item.AppId = GetCurrentProcessId(); // AppIDֱ��ȡ�������ID
		loMsg.Data.Item.MsgId = EMHOSTID_GET_PRIVACY_STATUS;

		loMsg.Data.Item.BufSize = 0;

		luResult = gloCenter.SendMessageToService(loMsg);
		if (luResult != ERROR_SUCCESS)
			break;

		memcpy_s(&rdwStatus, sizeof(DWORD), loMsg.Data.Item.MsgBuf, sizeof(DWORD));

	} while (false);

	return luResult;
}


// ������˽Э�鿪��״̬
// Set the privacy protocol switch status.
//
// ���أ� �����㣬��ʾ�ɹ������ط��㣬��ʾ�����룻
// return
//		zero: success
//		non-zero: error code
//rdwStatus : 0 off,1 on
DWORD EiSetPrivacyStatus(DWORD& rdwStatus)
{
	DWORD luResult = 0;

	do
	{
		CEiSvrMsgItem loMsg;
		loMsg.Data.Item.AppId = GetCurrentProcessId(); // AppIDֱ��ȡ�������ID
		loMsg.Data.Item.MsgId = EMHOSTID_SET_PRIVACY_STATUS;

		DWORD luMsgBufSize = (DWORD)sizeof(DWORD);
		memcpy_s(loMsg.Data.Item.MsgBuf, luMsgBufSize, &rdwStatus, luMsgBufSize);
		loMsg.Data.Item.BufSize = luMsgBufSize;

		luResult = gloCenter.SendMessageToService(loMsg);
		if (luResult != ERROR_SUCCESS)
			break;

	} while (false);

	return luResult;
}

// ֪ͨϵͳ����һ����ǰ�û��µ�Ӧ�ã������ĳ���ļ��л�ĳ����ҳ
// ���أ� ���ط��㣬��ʾ�ɹ��������㣬��ʾʧ�ܣ�
DWORD EiRunExeForCurrentUser(
	const wchar_t* npszFilePath,	// Ŀ���ļ���
	const wchar_t* npszCommandLine
)
{
	DWORD luResult = 0;

	do
	{
		if (npszFilePath == NULL)
			break;

		//������ǰ���ڵ� EMHOSTID_RECALL_MSG ����
		CEiSvrMsgItem loRecallMsg;
		loRecallMsg.Data.Item.AppId = GetCurrentProcessId(); // AppIDֱ��ȡ�������ID
		loRecallMsg.Data.Item.MsgId = EMHOSTID_RECALL_MSG;
		ULONG luRecallMsgId = EMHOSTID_RUN_EXE_FOR_CURRENT_USER;
		memcpy_s(loRecallMsg.Data.Item.MsgBuf, sizeof(ULONG), &luRecallMsgId, sizeof(ULONG));
		loRecallMsg.Data.Item.BufSize = sizeof(ULONG);

		gloCenter.RecallMessage(loRecallMsg);

		//���� ע��App ����
		CEiSvrMsgItem loMsg;
		loMsg.Data.Item.AppId = GetCurrentProcessId(); // AppIDֱ��ȡ�������ID
		loMsg.Data.Item.MsgId = EMHOSTID_RUN_EXE_FOR_CURRENT_USER;

		EI_MSG_RUN_APP ldRunInfo;
		wcscpy_s(ldRunInfo.FilePath, MAX_PATH, npszFilePath);
		wcscpy_s(ldRunInfo.CommandLine, MAX_PATH, npszCommandLine);
		ldRunInfo.SessionID = -1;
		ULONG luMsgBufSize = sizeof(EI_MSG_RUN_APP);
		memcpy_s(loMsg.Data.Item.MsgBuf, luMsgBufSize, &ldRunInfo, luMsgBufSize);
		loMsg.Data.Item.BufSize = luMsgBufSize;

		//��������
		luResult = gloCenter.SendMessageToService(loMsg);
		if (luResult != ERROR_SUCCESS)
			break;

	} while (false);

	return luResult;
}

// ֪ͨ����C��������仯
// ���أ� ���ط��㣬��ʾ�ɹ��������㣬��ʾʧ�ܣ�
DWORD EiSetCCoverOri(
	ULONG nulOri
)
{
	DWORD luResult = 0;

	do
	{
		CEiSvrMsgItem loMsg;
		loMsg.Data.Item.AppId = GetCurrentProcessId(); // AppIDֱ��ȡ�������ID
		loMsg.Data.Item.MsgId = EMHOSTID_SET_CCOVER_ORI;

		DWORD luMsgBufSize = (DWORD)sizeof(ULONG);
		memcpy_s(loMsg.Data.Item.MsgBuf, luMsgBufSize, &nulOri, luMsgBufSize);
		loMsg.Data.Item.BufSize = luMsgBufSize;

		luResult = gloCenter.SendMessageToService(loMsg);
		if (luResult != ERROR_SUCCESS)
			break;

	} while (false);

	return luResult;
}

// ֪ͨ�����Դ״̬�仯
// ���أ� ���ط��㣬��ʾ�ɹ��������㣬��ʾʧ�ܣ�
DWORD EiSetPowerStatus(
	DWORD ndwStatus
)
{
	DWORD luResult = 0;

	do
	{
		CEiSvrMsgItem loMsg;
		loMsg.Data.Item.AppId = GetCurrentProcessId(); // AppIDֱ��ȡ�������ID
		loMsg.Data.Item.MsgId = EMHOSTID_POWER_CHANGE;

		DWORD luMsgBufSize = (DWORD)sizeof(DWORD);
		memcpy_s(loMsg.Data.Item.MsgBuf, luMsgBufSize, &ndwStatus, luMsgBufSize);
		loMsg.Data.Item.BufSize = luMsgBufSize;
		//�Ȱѵ�ǰ���ڵ��л�������
		//gloCenter.RecallMessage(loMsg);
		luResult = gloCenter.PostMessageToService(loMsg);
		if (luResult != ERROR_SUCCESS)
			break;

	} while (false);

	return luResult;
}

// ѯ���Ƿ���Ҫ��ʾoobe,ͬһ�û�ֻ��ʾһ��
// ���أ� ���ط��㣬��ʾ�ɹ��������㣬��ʾʧ�ܣ�
ULONG EiIsShowOOBE(
	bool& rbFlag
)
{
	ULONG luResult = ERROR_NOT_READY;

	do
	{
		//�������ͻ�ȡ��Ϣ
		CEiSvrMsgItem loMsg;
		loMsg.Data.Item.AppId = GetCurrentProcessId(); // AppIDֱ��ȡ�������ID
		loMsg.Data.Item.MsgId = EMHOSTID_GET_IS_SHOW_OOBE;
		loMsg.Data.Item.BufSize = 0;

		luResult = gloCenter.SendMessageToService(loMsg);
		if (luResult != ERROR_SUCCESS)
			return 0;

		//��ȡֵ
		rbFlag = *(bool*)loMsg.Data.Item.MsgBuf;

	} while (false);

	return luResult;
}
ULONG EiIsShowUpdateOOBE(
	bool& rbFlag
)
{
	ULONG luResult = ERROR_NOT_READY;

	do
	{
		//�������ͻ�ȡ��Ϣ
		CEiSvrMsgItem loMsg;
		loMsg.Data.Item.AppId = GetCurrentProcessId(); // AppIDֱ��ȡ�������ID
		loMsg.Data.Item.MsgId = EMHOSTID_GET_IS_SHOW_UPDATEOOBE;
		loMsg.Data.Item.BufSize = 0;

		luResult = gloCenter.SendMessageToService(loMsg);
		if (luResult != ERROR_SUCCESS)
			return 0;

		//��ȡֵ
		rbFlag = *(bool*)loMsg.Data.Item.MsgBuf;

	} while (false);

	return luResult;
}
// ѯ��8951�Ƿ���sleep״̬
// ���أ� ���ط��㣬��ʾ�ɹ��������㣬��ʾʧ�ܣ�
ULONG Ei8951IsSleep(
	bool& rbSleep
)
{
	ULONG luResult = ERROR_NOT_READY;

	do
	{
		//�������ͻ�ȡ��Ϣ
		CEiSvrMsgItem loMsg;
		loMsg.Data.Item.AppId = GetCurrentProcessId(); // AppIDֱ��ȡ�������ID
		loMsg.Data.Item.MsgId = EMHOSTID_GET_8951_IS_SLEEP;
		loMsg.Data.Item.BufSize = 0;

		luResult = gloCenter.SendMessageToService(loMsg);
		if (luResult != ERROR_SUCCESS)
			return 0;

		//��ȡֵ
		rbSleep = *(bool*)loMsg.Data.Item.MsgBuf;

	} while (false);

	return luResult;
}

// ��ȡ�û���ʾ����
// ���أ� ���ط��㣬��ʾ�ɹ��������㣬��ʾʧ�ܣ�
ULONG EiGetUserLagID(
	DWORD& rdwLagID
)
{
	ULONG luResult = ERROR_NOT_READY;

	do
	{
		//�������ͻ�ȡ��Ϣ
		CEiSvrMsgItem loMsg;
		loMsg.Data.Item.AppId = GetCurrentProcessId(); // AppIDֱ��ȡ�������ID
		loMsg.Data.Item.MsgId = EMHOSTID_GET_USER_LAGID;
		loMsg.Data.Item.BufSize = 0;

		luResult = gloCenter.SendMessageToService(loMsg);
		if (luResult != ERROR_SUCCESS)
			return 0;

		//��ȡֵ
		rdwLagID = *(DWORD*)loMsg.Data.Item.MsgBuf;

	} while (false);

	return luResult;
}

DWORD EiSetSmartInfoStatus(SmartInfoOptionMsgBody &SIOMB)
{
	ULONG luResult = 0;

	do
	{
		//�������ͻ�ȡ��Ϣ
		CEiSvrMsgItem loMsg;
		loMsg.Data.Item.AppId = GetCurrentProcessId(); // AppIDֱ��ȡ�������ID
		loMsg.Data.Item.MsgId = EMHOSTID_SET_SMARTINFO_MSG;
		ULONG luMsgBufSize = sizeof(SmartInfoOptionMsgBody);
		loMsg.Data.Item.BufSize = luMsgBufSize;

		memcpy_s(loMsg.Data.Item.MsgBuf, luMsgBufSize, &SIOMB, luMsgBufSize);

		luResult = gloCenter.SendMessageToService(loMsg);
		if (luResult != ERROR_SUCCESS)
			return luResult;

		luResult = ERROR_SUCCESS;

	} while (false);

	return luResult;
}

// ֪ͨ����8951״̬�仯
// ���أ� ���ط��㣬��ʾ�ɹ��������㣬��ʾʧ�ܣ�
DWORD Ei8951StatusChanged(
	bool nbIsConnect
)
{
	DWORD luResult = 0;

	do
	{
		CEiSvrMsgItem loMsg;
		loMsg.Data.Item.AppId = GetCurrentProcessId(); // AppIDֱ��ȡ�������ID
		loMsg.Data.Item.MsgId = EMHOSTID_8951_CONNECT_OR_REMOVE;

		DWORD luMsgBufSize = (DWORD)sizeof(nbIsConnect);
		memcpy_s(loMsg.Data.Item.MsgBuf, luMsgBufSize, &nbIsConnect, luMsgBufSize);
		loMsg.Data.Item.BufSize = luMsgBufSize;

		luResult = gloCenter.SendMessageToService(loMsg);
		if (luResult != ERROR_SUCCESS)
			break;

	} while (false);

	return luResult;
}


// ֪ͨ����5182״̬�仯
// ���أ� ���ط��㣬��ʾ�ɹ��������㣬��ʾʧ�ܣ�
DWORD Ei5182StatusChanged(
	bool nbIsConnect
)
{
	DWORD luResult = 0;

	do
	{
		CEiSvrMsgItem loMsg;
		loMsg.Data.Item.AppId = GetCurrentProcessId(); // AppIDֱ��ȡ�������ID
		loMsg.Data.Item.MsgId = EMHOSTID_5182_CONNECT_OR_REMOVE;

		DWORD luMsgBufSize = (DWORD)sizeof(nbIsConnect);
		memcpy_s(loMsg.Data.Item.MsgBuf, luMsgBufSize, &nbIsConnect, luMsgBufSize);
		loMsg.Data.Item.BufSize = luMsgBufSize;

		luResult = gloCenter.PostMessageToService(loMsg);
		if (luResult != ERROR_SUCCESS)
			break;

	} while (false);

	return luResult;
}

// �ж��Լ��Ƿ���ǰ̨����
// Is it a foreground window
//
// ���أ� �����㣬��ʾ�ɹ������ط��㣬��ʾ�����룻
// return
//		zero: success
//		non-zero: error code
DWORD EiIsForeground(bool& rbIsForeground)
{
	ULONG luResult = ERROR_NOT_READY;
	rbIsForeground = false;

	do
	{
		//�������ͻ�ȡ��Ϣ
		CEiSvrMsgItem loMsg;
		loMsg.Data.Item.AppId = GetCurrentProcessId(); // AppIDֱ��ȡ�������ID
		loMsg.Data.Item.MsgId = EMHOSTID_GET_IS_FOREGROUND_WINDOW;
		loMsg.Data.Item.BufSize = 0;

		luResult = gloCenter.SendMessageToService(loMsg);
		if (luResult != ERROR_SUCCESS)
			return 0;

		//��ȡֵ
		rbIsForeground = *(bool*)loMsg.Data.Item.MsgBuf;

	} while (false);

	return luResult;
}

// ֪ͨHomebar�л�Ӧ��
// ���أ� ���ط��㣬��ʾ�ɹ��������㣬��ʾʧ�ܣ�
ULONG EiChangeApp(
	DWORD ldwID
)
{
	DWORD luResult = 0;

	do
	{
		CEiSvrMsgItem loMsg;
		loMsg.Data.Item.AppId = GetCurrentProcessId(); // AppIDֱ��ȡ�������ID
		loMsg.Data.Item.MsgId = EMHOSTID_CHANGE_APP;

		DWORD luMsgBufSize = (DWORD)sizeof(ldwID);
		memcpy_s(loMsg.Data.Item.MsgBuf, luMsgBufSize, &ldwID, luMsgBufSize);
		loMsg.Data.Item.BufSize = luMsgBufSize;

		luResult = gloCenter.PostMessageToService(loMsg);
		if (luResult != ERROR_SUCCESS)
			break;

	} while (false);

	return luResult;
}

// ��ȡ��ǰ�û������б�
// ���أ� ���ط��㣬��ʾ�ɹ��������㣬��ʾʧ�ܣ�
DWORD EiGetUserDiskList(
)
{
	ULONG luResult = ERROR_NOT_READY;

	do
	{
		//�������ͻ�ȡ��Ϣ
		CEiSvrMsgItem loMsg;
		loMsg.Data.Item.AppId = GetCurrentProcessId(); // AppIDֱ��ȡ�������ID
		loMsg.Data.Item.MsgId = EMHOSTID_GET_USER_DIST_LIST;
		loMsg.Data.Item.BufSize = 0;

		luResult = gloCenter.SendMessageToService(loMsg);
		if (luResult != ERROR_SUCCESS)
			return luResult;

		luResult = ERROR_SUCCESS;

	} while (false);

	return luResult;
}

// ���ͻ�Ծ��Ϣ�Ķ�ʱ���ص�����
VOID WINAPI NormalRunNoticeCallBack(HWND hwnd, UINT message, UINT idTimer, DWORD dwTime)
{
	while (true)
	{
		Sleep(1000 * 10);
		gloCenter.CheckAndSendNormalRunMsg();
	}
}

DWORD EiIsFileCertificateValidation(FileCVCheckVaule& checkvalue)
{
	ULONG luResult = 0;

	do
	{
		//�������ͻ�ȡ��Ϣ
		CEiSvrMsgItem loMsg;
		loMsg.Data.Item.AppId = GetCurrentProcessId(); // AppIDֱ��ȡ�������ID
		loMsg.Data.Item.MsgId = EMHOSTID_CHECK_FILE_ISVALID;
		ULONG luMsgBufSize = sizeof(FileCVCheckVaule);
		loMsg.Data.Item.BufSize = luMsgBufSize;

		memcpy_s(loMsg.Data.Item.MsgBuf, luMsgBufSize, &checkvalue, luMsgBufSize);

		luResult = gloCenter.SendMessageToService(loMsg);
		if (luResult != ERROR_SUCCESS)
			return 0;

		memcpy_s(&checkvalue, sizeof(FileCVCheckVaule), loMsg.Data.Item.MsgBuf, sizeof(FileCVCheckVaule));
		//��������ֵ����Ϣ���ظ��ṹ��

	} while (false);

	return luResult;
}

// ������������������Ϣ�Ķ�ʱ��
ULONG CreateNormalRunNoticeTimer(HWND hwnd)
{
	if (hwnd == NULL)
		return ERROR_INVALID_HANDLE;

	if (ghNoticeNormalRunThread != 0)
	{
		CloseHandle(ghNoticeNormalRunThread);
		ghNoticeNormalRunThread = 0;
	}

	int count = 0;
	while (true)
	{
		DWORD lpThreadId;
		ghNoticeNormalRunThread = CreateThread(
			NULL,
			0,
			(LPTHREAD_START_ROUTINE)NormalRunNoticeCallBack,
			NULL,
			0,
			&lpThreadId
		);

		if (ghNoticeNormalRunThread == 0)
		{
			if (++count > 5)
				return ERROR_BAD_UNIT;
			Sleep(100);
			continue;
		}
		break;
	}

	return ERROR_SUCCESS;
}

// zhuhl5
DWORD EiTconBoolSetting(APITconBoolValues& values)
{
	DWORD luResult = 0;
	do {
		CEiSvrMsgItem msg;
		msg.Data.Item.AppId = GetCurrentProcessId();
		msg.Data.Item.MsgId = EMHOSTID_SETTCONBOOLSETTING;
		msg.Data.Item.BufSize = sizeof(values);
		memcpy_s(msg.Data.Item.MsgBuf, msg.Data.Item.BufSize, &values, msg.Data.Item.BufSize);

		luResult = gloCenter.PostMessageToService(msg);
	} while (false);
	return luResult;
}

//xingej1
void EiBackToKeyboard(void)
{
	DWORD luResult = 0;
	do
	{
		CEiSvrMsgItem loMsg;
		loMsg.Data.Item.AppId = GetCurrentProcessId(); // AppIDֱ��ȡ�������ID
		loMsg.Data.Item.MsgId = EMHOSTID_TO_KEYBOARD;

		ULONG luMsgBufSize = 0;

		luResult = gloCenter.PostMessageToService(loMsg);
		if (luResult != ERROR_SUCCESS)
			break;

	} while (false);
}

// ��ȡ��ǰ�豸״̬��������أ�����etc��
DWORD EiGetDeviceStatus(PEinkDeviceStatus pstEinkDeviceStatus)
{
	ULONG luResult = ERROR_NOT_READY;

	do
	{
		//�������ͻ�ȡ��Ϣ
		CEiSvrMsgItem loMsg;
		loMsg.Data.Item.AppId = GetCurrentProcessId(); // AppIDֱ��ȡ�������ID
		loMsg.Data.Item.MsgId = EMHOSTID_GET_DEVICE_STATUS;
		loMsg.Data.Item.BufSize = 0;

		luResult = gloCenter.SendMessageToService(loMsg);
		if (luResult != ERROR_SUCCESS)
			return luResult;

		//��ȡ����ʾ�豸��Ϣ
		memcpy_s(pstEinkDeviceStatus, sizeof(EinkDeviceStatus), loMsg.Data.Item.MsgBuf, sizeof(EinkDeviceStatus));

		luResult = ERROR_SUCCESS;

	} while (false);

	return luResult;
}

// EOF