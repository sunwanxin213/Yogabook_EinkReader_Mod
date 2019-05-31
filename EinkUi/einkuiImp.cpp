/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


// Xui.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"

#include "CommonHeader.h"

#include "./Graphy/XgD2DEngine4Eink.h"

#include "einkuiimp.h"

#include "resource.h"
#include <Dwmapi.h >
#include "EinkIteAPI.h"

#define EINK_RENDER_INTERVAL 600
#define EINK_RENDER_TIMER	 100

DEFINE_BUILTIN_NAME(CEinkuiSystem)
CEinkuiSystem* CEinkuiSystem::gpXuiSystem = NULL;

CEinkuiSystem::CEinkuiSystem()
{
	mpXuiGraphics = NULL;
	mpElementManager = NULL;
	muOperationThreadID = 0;
	mpAllocator = NULL;
	mhOperationThread = NULL;
	miToRender = 1;
	miDisableRender = 0;
	//muAutoRenderInterval = 0;	// 0��ʾ���Զ�����
	miRefreshEink = 0;
	miLostToRender = 0;
	muRenderTick = 0;
	miMissedMouseTest =0;
	mdLazyUpdate.Updated = 0;
	msuWinCap = 0;
	mpImeContext = NULL;
	miDiscardRenderMessage = 0;

	mhExitDetermine = CreateEvent(NULL,TRUE,FALSE,NULL);

	mbFirstRun = true;
	mbLocked = false;
	mlWinCallBack = 0;
	//mbWindowHide = false;
	mbIsTouch = false;
	mpSmallPreView = NULL;
	mpBigPreView = NULL;

	mdEinkInfo.ulWidth = mdEinkInfo.ulHeight = 0;
	muAutoRotate = 0;
}

CEinkuiSystem::~CEinkuiSystem()
{
	CMM_SAFE_RELEASE(mpXuiGraphics);
	CMM_SAFE_RELEASE(mpElementManager);
	CMM_SAFE_RELEASE(mpAllocator);
	CMM_SAFE_RELEASE(mpImeContext);
	if(mhOperationThread != NULL)
		CloseHandle(mhOperationThread);
	if(mhExitDetermine != NULL)
		CloseHandle(mhExitDetermine);

	while(moTimers.Size() > 0)
	{
		STES_TIMER* lpTimer = moTimers[0];
		CMM_SAFE_DELETE(lpTimer);
		moTimers.RemoveByIndex(0);
	}

	if(mpSmallPreView != NULL)
		DeleteObject(mpSmallPreView);

	if(mpBigPreView != NULL)
		DeleteObject(mpBigPreView);

	//�˳�EINK
	EiEnd();
}


// �������仭ˢ
IEinkuiBrush* __stdcall CEinkuiSystem::CreateBrush(
	XuiBrushType niBrushType,
	D2D1_COLOR_F noColor
	)
{
	if (mpPaintResource == NULL)
		return NULL;

	return mpPaintResource->CreateBrush(niBrushType, noColor);

}

// ���仭ˢʱ����Ҫ��������ɫ��
IEinkuiBrush* __stdcall CEinkuiSystem::CreateBrush(
	XuiBrushType niBrushType, 
	D2D1_GRADIENT_STOP* npGradientStop, 
	ULONG nuCount, 
	D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES ndLinearGradientBrushProperties
	)
{
	if (mpPaintResource == NULL)
		return NULL;

	return 	mpPaintResource->CreateBrush(niBrushType, npGradientStop, nuCount, ndLinearGradientBrushProperties);
	

}



// ���Ψһ����
CEinkuiSystem* CEinkuiSystem::GetUniqueObject(void)
{
	if(gpXuiSystem ==NULL)
		CEinkuiSystem::CreateInstance();

	CMMASSERT(gpXuiSystem !=NULL);

	return gpXuiSystem;
}


// ����Ԫ�ع���
IXelManager* __stdcall CEinkuiSystem::GetElementManager(void)	// ���Բ���ʧ�ܣ������жϷ���ֵ�Ƿ�ΪNULL
{
	return mpElementManager;
}

// ��õ�ǰ΢���ӿڣ������ͷ�;���ĳ���߳�û��ʹ��CreateWidgetWorkThread��������ô���������߳��е���GetCurrentWidget�������쳣����
IXsWidget* __stdcall CEinkuiSystem::GetCurrentWidget(void)	// ���Բ���ʧ�ܣ������жϷ���ֵ�Ƿ�ΪNULL
{
	IXsWidget* lpWidget = NULL;
	int liPos;
	CEsThreadNode loFind;

	loFind.muThreadID = GetCurrentThreadId();

	moWidgetLock.Enter();

	if(loFind.muThreadID == muOperationThreadID)
	{
		// ��Widget�����Ķ�ջ��ȡջ��
		lpWidget = moWidgetContext.GetTopWidget();
	}
	else
	if(loFind.muThreadID == muWinThreadID)
	{
		lpWidget = moWinWgtContext.GetTopWidget();
	}
	else
	{
		// ��鵱ǰ�Ƿ�����ĳ��Widget�����߳���
		liPos = moWidgetWorkThreads.Find(loFind);
		if(liPos >= 0)
		{	// ȷʵ��һ�������߳�
			lpWidget = moWidgetWorkThreads[liPos].mpOwnerWidget;
		}
	}

	moWidgetLock.Leave();

	if(lpWidget == NULL)
		THROW_NULL;	// �������������̵߳��ñ�����

	return dynamic_cast<IXsWidget*>(lpWidget);
}

// ��÷������������������ڽ��������򷢲���Element���󣬷��صĶ��������ͷ�
IXelAllocator* __stdcall CEinkuiSystem::GetAllocator(void)
{
	return mpAllocator;
}


// ��ʼ������������ʵ�ָ��ֲ�ͬ�����ĳ�ʼ��������ע�⣬����������InitOnCreate������һ��Ҫ���û����InitOnCreate����
// ����0��ʾ�ɹ�������ֵ���λΪ1��ʾ�������ش���Ӧ����ֹ��ʼ�����̣����صľ��Ǵ����룻��������ֵ��ʾ�����Ǵ��󷵻���
ULONG CEinkuiSystem::InitOnCreate(void){
	if(gpXuiSystem != NULL)
		return ERESULT_OBJECT_EXISTED;

	gpXuiSystem = this;

	return ERESULT_SUCCESS;
}

ERESULT CEinkuiSystem::CreateMainWnd(
	IN int niX,		// ����������Ļ�ϵ����Ͻ�X����
	IN int niY,		// ����������Ļ�ϵ����Ͻ�Y����
	IN int niWidth,		// ����������Ļ�ϵĿ��
	IN int niHeight,	// ����������Ļ�ϵĸ߶�
	IN const wchar_t* nswWndTittle		// �����ڱ���
	)
{
	WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
	wcex.style         = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wcex.lpfnWndProc   = CEinkuiSystem::MainWindowProc;
	wcex.cbClsExtra    = 0;
	wcex.cbWndExtra    = sizeof(LONG_PTR);
	wcex.hInstance     = GetModuleHandle(NULL);
	wcex.hbrBackground = NULL;
	wcex.lpszMenuName  = NULL;
	wcex.hCursor       = NULL;//LoadCursor(NULL, IDC_ARROW);
	wcex.lpszClassName = L"ESIEINKV1";	// XUI core window class version 1
	wcex.hIconSm = LoadIcon(wcex.hInstance,(LPCWSTR)IDI_MAINWND_ICON); //���Ӷ�ICON��ָ��
	wcex.hIcon = LoadIcon(wcex.hInstance,(LPCWSTR)IDI_MAINWND_ICON);

	RegisterClassEx(&wcex);

	HWND lhWindow = CreateWindow(
		L"ESIEINKV1",
		nswWndTittle,
		WS_OVERLAPPEDWINDOW |WS_MINIMIZEBOX,//WS_OVERLAPPEDWINDOW,WS_POPUP|WS_VISIBLE|WS_MINIMIZEBOX|WS_SYSMENU,
		niX,
		niY,
		niWidth,
		niHeight,
		NULL,
		NULL, 
		GetModuleHandle(NULL),
		this
		);

	if(lhWindow==NULL)
	{
		return ERESULT_UNSUCCESSFUL;
	}

	RegisterTouchWindow(lhWindow, TWF_WANTPALM);

	//���ע�����ڽ���connect standby��Ϣ
	RegisterPowerSettingNotification(EinkuiGetSystem()->GetMainWindow(), &GUID_CONSOLE_DISPLAY_STATE, DEVICE_NOTIFY_WINDOW_HANDLE);
	//HRGN rgn = CreateRectRgn(0, 0, niX+niWidth, niY+niHeight); ������Ϊʲô�������е��ã����ܺ�d3d�й� Mar.2,2013
	//SetWindowRgn(lhWindow, rgn, FALSE);

	return ERESULT_SUCCESS;
}

// ��ʾ��������WindowsͨѶ����
void __stdcall CEinkuiSystem::ShowMainWindow(bool nbShown)
{
	::PostMessage(mhMainWnd,muiCustomWinMsg,'Show',nbShown?1:0);
}


// ��õ�ǰ�Ľ������������ַ������磺��������'chn'
const wchar_t* __stdcall CEinkuiSystem::GetCurrentLanguage(void)
{
#define MAX_LANGID_LENGTH 10

	const wchar_t* lswLang= L"enu";

	//LANGID langId = GetUserDefaultUILanguage();
	DWORD ldwLangID = GetUserDefaultUILanguage();
	EiGetUserLagID(ldwLangID);
	switch(ldwLangID)
	{
	case 0x0404: //Chinese (Taiwan)
	case 0x1004: // Chinese (Singapore)
	case 0x1404: // Chinese (Macao SAR)
		lswLang = L"zh_tw";
		break;
	case 0x0c04: //Chinese (Hong Kong SAR, PRC)
		lswLang = L"zh_hk";
		break;
	case 0x0804: //Chinese (PRC)
		lswLang = L"zh_cn";
		break; 
	case 0x0405: //Czech
		lswLang = L"cs";
		break; 
	case 0x0406: //Danish
		lswLang = L"da"; //dk
		break; 
	case 0x0809://United Kingdom
		lswLang = L"en_gb";
		break;
	case 0x0407: //German (Standard)
	case 0x0807: //German (Switzerland) 
	case 0x0c07: //German (Austria) 
	case 0x1007: //German (Luxembourg) 
	case 0x1407: //German (Liechtenstein) 
		lswLang = L"de"; //at
		break;
	case 0x040a: //Spanish (Spain, Traditional Sort) 
	case 0x080a: //Spanish (Mexican) 
	case 0x0c0a: //Spanish (Spain, Modern Sort) 
	case 0x100a: //Spanish (Guatemala) 
	case 0x140a: //Spanish (Costa Rica) 
	case 0x180a: //Spanish (Panama) 
	case 0x1c0a: //Spanish (Dominican Republic) 
	case 0x200a: //Spanish (Venezuela) 
	case 0x240a: //Spanish (Colombia) 
	case 0x280a: //Spanish (Peru) 
	case 0x2c0a: //Spanish (Argentina) 
	case 0x300a: //Spanish (Ecuador) 
	case 0x340a: //Spanish (Chile) 
	case 0x380a: //Spanish (Uruguay) 
	case 0x3c0a: //Spanish (Paraguay) 
	case 0x400a: //Spanish (Bolivia) 
	case 0x440a: //Spanish (El Salvador) 
	case 0x480a: //Spanish (Honduras) 
	case 0x4c0a: //Spanish (Nicaragua) 
	case 0x500a: //Spanish (Puerto Rico) 
		lswLang = L"es_es";
		break;
	case 0x540A: //Spanish (United States (US)) 
		lswLang = L"es_us";
		break;
	case 0x0413: //Dutch (Netherlands) 
	case 0x0813: //Dutch (Belgium) 
		lswLang = L"nl";
		break;
	case 0x040b: //Finnish 
		lswLang = L"fi";
		break;
	case 0x040c: //French (Standard) 
	case 0x080c: //French (Belgian) 
	case 0x100c: //French (Switzerland) 
	case 0x140c: //French (Luxembourg) 
		lswLang = L"fr";
		break;
	case 0x0c0c: //French (Canadian) 
		lswLang = L"fr_ca";
		break;
	case 0x0408: //Greek 
		lswLang = L"el";
		break;
	case 0x040e: //Hungarian 
		lswLang = L"hu";
		break;
	case 0x0410: //Italian (Standard) 
	case 0x0810: //Italian (Switzerland) 
		lswLang = L"it";
		break;
	case 0x0411: //Japanese 
		lswLang = L"ja";
		break;
	case 0x0412: //Korean 
		lswLang = L"ko";
		break;
	case 0x0414: //Norwegian (Bokmal) 
	case 0x0814: //Norwegian (Nynorsk) 
		lswLang = L"nb";
		break;
	case 0x0415: //Polish 
		lswLang = L"pl";
		break;
	case 0x0816: //Portuguese (Portugal) 
		lswLang = L"pt_pt";
		break;
	case 0x0416: //Portuguese (Brazil)  
		lswLang = L"pt_br";
		break;
	case 0x0419: //Russian 
		lswLang = L"ru";
		break;
	case 0x041d: //Swedish 
	case 0x081d: //Swedish (Finland) 
		lswLang = L"sv";
		break;
	case 0x041f: //Turkish
		lswLang = L"tr";
		break;
	case 0x0418://romanian
		lswLang = L"ro";
		break;
 	case 0x0402: //bulgarian
 		lswLang = L"bg";
 		break;
	case 0x041b://slovak
		lswLang = L"sk";
		break;
	case 0x0424://slovenian
		lswLang = L"sl";
		break;
 	case 0x040D://Hebrew (he) Israel (IL)			----HE
 		lswLang = L"iw";
 		break;
 	case 0x0401: // Arabic (ar) Saudi Arabia (SA)	----AR
 		lswLang = L"ar";
 		break;

	case 0x041A: // Croatian (hr) Croatia (HR)		----CR
		lswLang = L"cr";
		break;
	case 0x041E: // 	Thailand (TH)
		lswLang = L"th";
		break;
	case 0x0C1A: //Serbian (SR) Serbia and Montenegro, Former, Cyrillic (CS)
	case 0x081A: //Serbian (SR) Serbia and Montenegro, Former, Latin (CS)
	case 0x241A: //win8.1 �������ģ���MSDN��Ҳû�ж���
		lswLang = L"sr";
		break;
	default:
		lswLang = L"enu";

	}

	return lswLang;
}

FLOAT __stdcall CEinkuiSystem::GetDpiX(void)
{
	return mdDpi.width;
}
FLOAT __stdcall CEinkuiSystem::GetDpiY(void)
{
	return mdDpi.height;
}

// ���û��壬��������װ����ɺ󣬵�������������û��壻�ڴ�֮ǰ���岢������Ƴ�ʵ��ͼ���Ա�������������ʱ��Eink��Ļ�������
ERESULT __stdcall CEinkuiSystem::EnablePaintboard(void)
{
	ERESULT luResult = ERESULT_NOT_INITIALIZED;

	if (mpXuiGraphics != NULL)
	{
		mpXuiGraphics->StopPainting(false);
		luResult = ERESULT_SUCCESS;
	}

	EnableRender();

	return luResult;
}

// ���û��壬����Ļ��ת����ʱ�����ñ�������ʹϵͳ���û���
void __stdcall CEinkuiSystem::ResetPaintboard(void)
{
	//EINK��ȡEink�豸��Ϣ
	EiGetSystemInfo(&mdEinkInfo);

	mpXuiGraphics->ResetPaintboard();

}

// ��������ڵĴ�С
void __stdcall CEinkuiSystem::GetPaintboardSize(
	OUT EI_SIZE* npSize	// ��ȡ�����С
	)
{
	mpXuiGraphics->GetPaintboardSize(npSize);
}

// ���ñ�������Eink Panel�ϵ���ʾλ�ã�ֻ�����Ի����
void __stdcall CEinkuiSystem::SetPositionInPanel(
	ULONG nuX,
	ULONG nuY
	)
{
	mpElementManager->SetPositionInPanel(nuX,nuY);
}



// ע�����й�����Ϣ
bool CEinkuiSystem::RegisterCommonWindowsMessage(void)
{
	muiCustomWinMsg = RegisterWindowMessage(ES_WINUI_CUSTOM_MSG);

	return muiCustomWinMsg!=0;
}

// ����Xui
int CEinkuiSystem::Startup(
	STES_START_UP& nrStart
	)
{
	int liProcessResult = -1;
	wchar_t* lswErrorString = NULL;

	ERESULT luResult;

	try
	{
		//mbIsAutoSize = nbIsAutoSize;
		mdDpi.width = nrStart.Dpi.width;
		mdDpi.height = nrStart.Dpi.height;

		muWinThreadID = GetCurrentThreadId();

		//Trace_Point(25643);// ע�ṫ��Windows��Ϣ
		if(RegisterCommonWindowsMessage()==false)
			THROW_FALSE;

		//Trace_Point(19481);// ����ͨѶ����
		luResult = CreateMainWnd(0,0,100,100,nrStart.WindowTitle);
		if(ERESULT_FAILED(luResult))
		{
			THROW_UNKNOWN;
		}
		
		mpImeContext = CXsImeContext::CreateInstance();

		//EINK��ʼ��
		EiAppStart(mhMainWnd);

		//EINK��ȡEink�豸��Ϣ
		EiGetSystemInfo(&mdEinkInfo);

		if (nrStart.AutoRotate != 0)
		{
			muAutoRotate = nrStart.AutoRotate;

			EiSetScreenOrient(mdEinkInfo.ulOrient);
		}

		//Trace_Point(24937);// ׼��ϵͳWidget�Ĳ���
		nrStart.WaitingCaller = CreateEvent(NULL,true,false,NULL);
		if(nrStart.WaitingCaller == NULL)
		{
			THROW_NULL;
		}

		//Trace_Point(20499);// ���������߳�
		luResult = ExecuteOperationThread(&nrStart);
		if(luResult != ERESULT_SUCCESS)
		{
			THROW_FALSE;
		}

		//Trace_Point(19006);//�ȴ�ϵͳWidget������ɣ�����20��
		if(WaitForSingleObject(nrStart.WaitingCaller,2000000/*???20000*/) == WAIT_TIMEOUT)
		{
			CloseHandle(nrStart.WaitingCaller);
			THROW_UNKNOWN;
		}
		CloseHandle(nrStart.WaitingCaller);

		if(nrStart.Result != ERESULT_SUCCESS)
			THROW_UNKNOWN;


		if (nrStart.CustomDraw != NULL)
		{
			mpXuiGraphics->SetCustomDraw(nrStart.CustomDraw->CustomDrawFun);
		}

		//ExecuteDetermineThread(NULL);

		// ������������

		// ����ִ�в����߳�̽��
		moWidgetContext.EnableTickDetection(true);

		//Trace_Point(15975);// ��������Ϣѭ��
		liProcessResult = HostThreadLoop();

		// �ȴ������߳��˳�
		WaitForSingleObject(mhOperationThread,16000);
	}
	catch(...)
	{
		//Trace_Point(350);//  ???���ӶԻ�����ʾ��ֱ�����û�����
		//Trace_Flush();
		MessageBox(NULL,L"An unknown error has detected!",L"Error",MB_OK);
	}
	
	return liProcessResult;
}

// �������ƶ�ʱ��
void CEinkuiSystem::EnableRender(void)
{
	DisableRender();

	InterlockedExchange(&miDisableRender, 0);

	//mpElementManager->GetRootElement()->SetTimer(ELMGR_TIMERID_RENDER, MAXULONG32, 15, NULL);
	//mpElementManager->GetRootElement()->SetTimer(ELMGR_TIMERID_LAZY, MAXULONG32, 200, NULL);
	mpElementManager->GetRootElement()->SetTimer(ELMGR_TIMERID_RENDER, MAXULONG32, EINK_RENDER_TIMER, NULL);
	//mpElementManager->GetRootElement()->SetTimer(ELMGR_TIMERID_LAZY, MAXULONG32, 1000, NULL);

	// ���û�����ʱ�������ڻ������߳�;��Ҫ���ڼ������߳��Ƿ�ᱻ����
	//::SetTimer(mhMainWnd,0,1000,NULL); $ax$ Nov.27,2018 ��ʱ���Σ���Ϊ���ֲ���(�򿪵����飩��ʱ������û�в�ȡ����Ķ�����ʽ���ᵼ����Ϣ�������ʱ������
					// ��������������Ϣ������

}

// ��ֹ���ƶ�ʱ��
void CEinkuiSystem::DisableRender(void)
{
	InterlockedExchange(&miDisableRender, 1);

	mpElementManager->GetRootElement()->KillTimer(ELMGR_TIMERID_RENDER);
	//mpElementManager->GetRootElement()->KillTimer(ELMGR_TIMERID_LAZY);

	::KillTimer(mhMainWnd,0);	// ��Ϊ������Եĵ�ԴҪ�󣬶����Ӷ������ʱ�����޸�
}


// Windows�����̣߳���ѭ��
int CEinkuiSystem::HostThreadLoop()
{
	MSG msg;

	while (GetMessage(&msg, NULL, 0, 0))
	{
		//�ж�һ���Ƿ���û�д��ھ�����߳���Ϣ
		if(msg.hwnd == NULL && msg.message == WM_SYSCOMMAND && msg.wParam == SC_CLOSE)
			msg.hwnd = mhMainWnd;

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	
	return 0;
}

ERESULT CEinkuiSystem::DoCapture(
	HWND hWnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam,
	HRESULT& rWinResult
	)	// ??? û���ṩע������Ĺ���
{
	int liMatch;
	CEsWsgCapture loFind;
	ERESULT luReval = ERESULT_WINMSG_SENDTO_NEXT;
	TEsWsgCaptor loAppliers;
	CXuiIterator* lpItrObj;


	moCaptorLock.Enter();

	loFind.muiWsgID = message;
	liMatch = moCaptors.Find(loFind);
	if(liMatch >= 0)
	{
		while(moCaptors[liMatch].muiWsgID == message)
		{
			loAppliers.Insert(moCaptors[liMatch]);
			if(++liMatch >= moCaptors.Size())
				break;
		}
	}

	moCaptorLock.Leave();

	if(loAppliers.Size()<=0)
		return ERESULT_WINMSG_SENDTO_NEXT;

	for(liMatch = 0;liMatch < loAppliers.Size();liMatch++)
	{
		if(mpElementManager->VerifyIterator(loAppliers[liMatch].mpAppItr)!= ERESULT_SUCCESS)
			continue;

		lpItrObj = dynamic_cast<CXuiIterator*>(loAppliers[liMatch].mpAppItr);

		if(lpItrObj == NULL)
			continue;

		moWinWgtContext.PushWidget(lpItrObj->mpWidget);

		try	{
			luReval = (loAppliers[liMatch].mpApplicant->*(loAppliers[liMatch].mpProcedure))(hWnd,message,wParam,lParam,(LRESULT&)rWinResult);
		}
		catch(...)
		{
			luReval = ERESULT_WINMSG_SENDTO_NEXT;
		}

		moWinWgtContext.PopWidget();

		if(luReval != ERESULT_WINMSG_SENDTO_NEXT && luReval != ERESULT_ITERATOR_INVALID)
			break;
	}

	return luReval;
}

// ȷ�ϵ�ǰ�߳��Ƿ��ǲ����߳�
bool CEinkuiSystem::IsRunningInOperationThread(void)
{
	return muOperationThreadID == GetCurrentThreadId();
}

// ȷ�ϵ�ǰ�߳��Ƿ���Windows�����߳�
bool CEinkuiSystem::IsRunningInWindowsUiThread(void)
{
	return muWinThreadID == GetCurrentThreadId();
}

// ��鲢��ִ��Windows�̵߳�Callback���󣬱�����ֻ�ܱ�ִ����Windows�߳��еĻ�����סWindows�̵߳ĵط�����
bool CEinkuiSystem::RunWindowsUICallback(void)
{
	MSG msg;
	bool lbReval = true;

	if(IsRunningInWindowsUiThread()!=false)
	{
		while(PeekMessage(&msg,mhMainWnd,muiCustomWinMsg,muiCustomWinMsg,PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
			{
				PostMessage(mhMainWnd,WM_QUIT,msg.wParam,msg.lParam);
				lbReval = false;
				break;
			}
			//Trace_Point(8590);//ȷʵ������ԭ�������������̼߳��໥����

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return lbReval;
}

// ���������߳�
ERESULT CEinkuiSystem::ExecuteOperationThread(PSTES_START_UP npCreate)
{
	ULONG luThreadID;
	HANDLE lhThread = CreateThread(NULL,128*1024,(LPTHREAD_START_ROUTINE)CEinkuiSystem::OperationThread,npCreate,CREATE_SUSPENDED,&luThreadID);

	if(lhThread == NULL)
		return ERESULT_UNSUCCESSFUL;
	
	InterlockedExchange((LONG*)&muOperationThreadID,luThreadID);

	if(mhOperationThread != NULL)
		CloseHandle(mhOperationThread);

	mhOperationThread = lhThread;

	ResumeThread(lhThread);

	return ERESULT_SUCCESS;
}

// �����Ⱦ�׶�
CEinkuiSystem::ERENDER_STEP CEinkuiSystem::GetRenderStep(void)
{
	return (CEinkuiSystem::ERENDER_STEP)mpXuiGraphics->mlRenderStep;
}

// �����̣߳�XUIϵͳ�Ľ��潻���̣߳����ĳ��Widget��ס���̳߳���һ����ʱ�䣬���̼߳�ᱻHost�߳���ֹ
ULONG WINAPI CEinkuiSystem::OperationThread(PSTES_START_UP npStartup)
{
	ERESULT luResult;
	IEinkuiIterator* lpRootItr;

	//Trace_Time(6790);//�����߳������������һ�׶Σ���ʼ��ϵͳ

	CoInitialize(NULL);

	if(npStartup != NULL)
	{
		try
		{
			//Trace_Point(25884);// ����Ԫ�ع�����
			gpXuiSystem->mpElementManager = CXelManager::CreateInstance();
			if(gpXuiSystem->mpElementManager == NULL)
			{
				THROW_NULL;
			}

			//Trace_Point(20821);// ����ͼ���豸
			if(npStartup->CustomDraw != NULL)
				gpXuiSystem->mpXuiGraphics = CXD2dEngine::CreateInstance(npStartup->CustomDraw->Width, npStartup->CustomDraw->Height);
			else
				gpXuiSystem->mpXuiGraphics = CXD2dEngine::CreateInstance(0,0);

			if(gpXuiSystem->mpXuiGraphics == NULL)
			{
				THROW_NULL;
			}

			// ����ָ�븳ֵ������ָ��
			gpXuiSystem->mpPaintResource = gpXuiSystem->mpXuiGraphics;

			// ע���ݼ�
			lpRootItr = gpXuiSystem->mpElementManager->GetRootElement();
			CExHotkey::RegisterHotKey(lpRootItr,lpRootItr,EHOTKEY_COPY);
			CExHotkey::RegisterHotKey(lpRootItr,lpRootItr,EHOTKEY_CUT);
			CExHotkey::RegisterHotKey(lpRootItr,lpRootItr,EHOTKEY_PASTE);
			CExHotkey::RegisterHotKey(lpRootItr,lpRootItr,EHOTKEY_DELETE);
			CExHotkey::RegisterHotKey(lpRootItr,lpRootItr,EHOTKEY_SELECT_ALL);
			CExHotkey::RegisterHotKey(lpRootItr,lpRootItr,EHOTKEY_ENTER);
			CExHotkey::RegisterHotKey(lpRootItr,lpRootItr,EHOTKEY_ALTF4);
			CExHotkey::RegisterHotKey(lpRootItr,lpRootItr,EHOTKEY_ESC);
			//CExHotkey::RegisterHotKey(lpRootItr,lpRootItr,EHOTKEY);

			//Trace_Point(12679);// ׼������������
			gpXuiSystem->mpAllocator = CXelAllocator::CreateInstance(npStartup->ClassesRegPath);
			if(gpXuiSystem->mpAllocator == NULL)
			{
				THROW_NULL;
			}

			//Trace_Point(22798);//���ó�ʼ����ɱ�־
			gpXuiSystem->SetFlags(ESYS_FLAG_INITOK,true);

			//Trace_Point(21957);// ����ϵͳWidget
			luResult = gpXuiSystem->LaunchWidget(npStartup->ModulePathName,npStartup->HomeTempleteName,L"MainFrame",lpRootItr,NULL,NULL);

		}
		catch(...)
		{
			luResult = ERESULT_UNSUCCESSFUL;
		}

		npStartup->Result = luResult;
		SetEvent(npStartup->WaitingCaller);

		if(ERESULT_FAILED(luResult))
		{
			CoUninitialize();
			//Trace_Time(24705);// ʧ���ˣ��˳��߳�
			return -1;
		}

		Sleep(33);
	}

	//Trace_Time(19563);// �����߳̽���ڶ��׶Σ��ȴ�33���룬������Host�߳�

	gpXuiSystem->OpThreadMessageLoop(gpXuiSystem->moThreadBlock.AddBlock());

	CoUninitialize();
	//Trace_Time(22341);//�˳��߳�

	return 0;
}

// �����߳���Ϣ����ѭ��
ERESULT CEinkuiSystem::OpThreadMessageLoop(ULONG nuBlockID)
{
	ULONG luWait;
	ERESULT luResult;
	ULONG luCrtTick;

	do 
	{
		luCrtTick = GetTickCount();

		// ���Զ�ʱ���Ƿ���Ҫ����
		KickTimers(luCrtTick);

		try
		{
			// ��ȡһ����Ϣ���ַ�
			luResult = mpElementManager->ProcessNextMessage();

		}
		catch (...)
		{
			//Trace_Point(28888);//�����쳣
			luResult = ERESULT_UNSUCCESSFUL;
		}
		// �����Ϣ����Ϊ�գ����������Ĵ�������ʱ��������ȴ�������
		if(luResult == ERESULT_NO_MESSAGE)
		{
			luWait = GetTickCountToKickTimer(luCrtTick);

			luWait = mpElementManager->WaitMessageReach(luWait);
		}

		if(moThreadBlock.ReadBlockState(nuBlockID)!=ERESULT_BLOCK)
			break;

	} while (luResult != ERESULT_QUIT_XUI);

	//if(luResult == ERESULT_QUIT_XUI)
	//	PostQuitMessage(0);

	return luResult;
}


// ����һ���µ�Widget
ERESULT __stdcall CEinkuiSystem::LaunchWidget(
	IN const wchar_t* nswModulePathName,	// ��Widget��ģ���ļ���·��������ʵ�ִ�Widget��DLL����
	IN const wchar_t* nswHomeTempleteName,	// ��Widget��HomePage��Templete Key�����֣����Key������ProFile ��Root��
	IN const wchar_t* nswInstanceName,		// �������е�ʵ������ʵ����������ͬ�����������ͬ��ʵ������ϵͳ���᷵��ʧ��
	IN IEinkuiIterator* npFrame,	// ���ڶ�λ����ʾ��װ��Widget��Frame Element
	IN ICfKey* npInstConfig,	// Widgetʵ��ר������
	OUT IXsWidget** nppWidget	// ���Բ���д�����ڷ����½�����Widget�ӿڣ����صĽӿ���Ҫ�ͷ�
	)
{
	ERESULT luResult = ERESULT_UNSUCCESSFUL;
	IElementFactory* lpFactory = NULL;
	CXsWidget* lpWidget=NULL;
	IEinkuiIterator* lpHomePage = NULL;
	IConfigFile* lpProfile = NULL;
	ICfKey* lpTemplete = NULL;
	int liWidgetInserted=-1;

	do 
	{
		//Trace_Point(25893);// ����һ����Widget����
		lpWidget = CXsWidget::CreateInstance(nswModulePathName,nswInstanceName,npInstConfig);
		BREAK_ON_NULL(lpWidget);

		//Trace_Point(22020);// ��������Ƿ��ǽ���ϵͳWidget
		moWidgetLock.Enter();
		if(moAllWidgets.Size()==0)
		{
			//Trace_Point(9073);//����Root Element��Widget����
			mpElementManager->SetRootWidget(lpWidget);
		}

		//Trace_Point(11153);// ���½���Widget���뵽�б�
		liWidgetInserted = moAllWidgets.Insert(-1,lpWidget);
		moWidgetLock.Leave();
		if(liWidgetInserted<0)	{
			luResult = ERESULT_INSUFFICIENT_RESOURCES;
			break;
		}

		//Trace_Point(32591);// �л���ǰWidget
		PushWidget(lpWidget);

		if(moWinWgtContext.GetStackDepth()<=0)
		{
			//Trace_Point(15708);//��ʼ��Windowsϵͳ�̵߳�������
			moWinWgtContext.PushWidget(lpWidget);
		}

		//Trace_Point(12787);// װ�빤����
		lpFactory = mpAllocator->LoadFactory(/*lpWidget->GetModuleName()*/);
		BREAK_ON_NULL(lpFactory);

		// ����Widget�Ĺ�������
		lpWidget->SetFactory(lpFactory);

		//Trace_Point(12461);// ��ȡProfile
		lpProfile = lpFactory->GetTempleteFile();
		BREAK_ON_NULL(lpProfile);

		//Trace_Point(29298);// ���Templete
		lpTemplete = lpProfile->OpenKey(nswHomeTempleteName,false);
		BREAK_ON_NULL(lpTemplete);
		
		//Trace_Point(18621);// װ��Home Page
		lpHomePage = mpAllocator->CreateElement(npFrame,lpTemplete);
		BREAK_ON_NULL(lpHomePage);

		//Trace_Point(28099);//�ɹ��ˣ���moAllWidgets���Ӷ�Widget�����ã��������Ļ��մ�������ͷŵ�
		lpWidget->AddRefer();
		luResult = ERESULT_SUCCESS;

	} while (false);

	if(lpWidget!=NULL)
	{
		//Trace_Point(29054);// �����˳���ǰWidget��
		PopWidget();
	}

	if(ERESULT_SUCCEEDED(luResult))
	{
		if(nppWidget != NULL && lpWidget!=NULL)
		{
			*nppWidget = lpWidget;
			lpWidget->AddRefer();
		}
	}
	else
	if(lpWidget != NULL && liWidgetInserted >= 0)	// ���ʧ�ܣ���Ҫ�����
	{
		//Trace_Point(26011);// ����ǰWidget���б����Ƴ�
		moWidgetLock.Enter();
		moAllWidgets.RemoveByIndex(liWidgetInserted);
		moWidgetLock.Leave();
	}

	CMM_SAFE_RELEASE(lpFactory);
	CMM_SAFE_RELEASE(lpWidget);
	CMM_SAFE_RELEASE(lpHomePage);
	CMM_SAFE_RELEASE(lpTemplete);
	CMM_SAFE_RELEASE(lpProfile);

	return luResult;
}

// ��õ�ǰϵͳ�е�ĳ��Widget�ӿڣ��������NULL��ʾ�˱��֮��û��Widget��
// �˺���ֻ�ܱ�System Widget����
IXsWidget* __stdcall CEinkuiSystem::ObtainWidget(
	IN int niNumber		// ��0��ʼ��ŵ�Widegt�����û�����壬ֻ��Widget�Ĵ洢λ��
	)
{
	CXsWidget* lpWidgetObj = NULL;

	moWidgetLock.Enter();

	if(niNumber < moAllWidgets.Size())
		lpWidgetObj = moAllWidgets[niNumber];

	moWidgetLock.Leave();

	return dynamic_cast<IXsWidget*>(lpWidgetObj);
}

//// ����Widget
//IXsWidget* __stdcall CEinkuiSystem::FindWidget(
//	IN const wchar_t* nswInstanceName	// �������е�ʵ����
//	)
//{
//	return NULL;
//}

// ���ϵͳWidget
IXsWidget* __stdcall CEinkuiSystem::GetSystemWidget(void)
{
	CXsWidget* lpWidgetObj = NULL;

	moWidgetLock.Enter();

	if(0 < moAllWidgets.Size())
		lpWidgetObj = moAllWidgets[0];

	moWidgetLock.Leave();

	if(lpWidgetObj == NULL)
		THROW_NULL;

	return dynamic_cast<IXsWidget*>(lpWidgetObj);
}

ERESULT CEinkuiSystem::CloseWidget(
	IN IXsWidget* npWidget
	)
{
	CXsWidget* lpWidget;
	IEinkuiIterator* lpHome;

	if(npWidget == NULL)
		return ERESULT_WRONG_PARAMETERS;

	lpWidget = dynamic_cast<CXsWidget*>(npWidget);
	if(lpWidget == NULL)
		return ERESULT_WRONG_PARAMETERS;

	//Trace_Point(7113);// �رմ�Widget��HomePage
	lpHome = npWidget->GetHomePage();
	if(lpHome!=NULL)
		lpHome->Close();

	//// homepage ��Ȼ�����������ã�����ֱ�Ӹɵ���
	//auto lpToDestroy = dynamic_cast<CXuiIterator*>(lpHome);
	//while (lpToDestroy->KRelease() > 0);

	// ��ֹ��Widget�Ĺ����߳�

	//Trace_Point(22443);// ����Widget��widget�б���ɾ��
	moWidgetLock.Enter();

	for(int i=0;i< moAllWidgets.Size();i++)
	{
		if(moAllWidgets[i] == lpWidget)
		{
			moAllWidgets.RemoveByIndex(i);
			break;
		}
	}
	moWidgetLock.Leave();

	lpWidget->Release();

	return ERESULT_SUCCESS;
}


// Widgetѹջ����CXelManager���ã����ַ�һ����Ϣ��Ŀ��Elementʱ���ã���ʾ�л����µ�Widget״̬
void CEinkuiSystem::PushWidget(IXsWidget* npWidget)
{
	moWidgetContext.PushWidget(npWidget);
}

// Widget��ջ����ElementManager�����һ��Element����Ϣ�ǵ��ã��ָ���֮ǰ��Widget״̬
void CEinkuiSystem::PopWidget(void)
{
	moWidgetContext.PopWidget();
}

// ����һ�����̣߳����е�Widget��Ӧ��ʹ��������������Լ��Ķ����̣߳�����ֵ��Windows�߳̾�����������ڵ���SuspendThread/ResumeThread/GetExitCodeThread
// ���գ����صľ���������CloseHandle�رգ��������������ͬWindows API CreateThreadһ��
HANDLE __stdcall CEinkuiSystem::CreateWidgetWorkThread(
	LPSECURITY_ATTRIBUTES lpThreadAttributes,
	SIZE_T dwStackSize,
	LPTHREAD_START_ROUTINE lpStartAddress,
	LPVOID lpParameter,
	DWORD dwCreationFlags,
	LPDWORD lpThreadId
	)
{
	PSTES_CREATE_THREAD lpContext = new STES_CREATE_THREAD;

	if(lpContext == NULL)
		return NULL;

	lpContext->ThreadProc = lpStartAddress;
	lpContext->Context = lpParameter;
	lpContext->Widget = GetCurrentWidget();

	return CreateThread(lpThreadAttributes,dwStackSize,CEinkuiSystem::WidgetWorkThread,lpContext,dwCreationFlags,lpThreadId);
}

// �˳�Widget�����̣߳����������CreateWidgetWorkThread���߳��������˳�ʱ��ϵͳ���Զ�������ֹ�̲߳���
void __stdcall CEinkuiSystem::ExitWidgetWorkThread(DWORD dwExitCode)
{
	// ɾ����ǰ���̼߳�¼
	CEsThreadNode loAdd;

	loAdd.muThreadID = GetCurrentThreadId();

	moWidgetLock.Enter();

	moWidgetWorkThreads.Remove(loAdd);

	moWidgetLock.Leave();
}

// Widget�����̵߳����
ULONG WINAPI CEinkuiSystem::WidgetWorkThread(LPVOID Context)
{
	ULONG luResult = 0;
	PSTES_CREATE_THREAD lpContext = (PSTES_CREATE_THREAD)Context;

	if(Context==NULL)
		return ERROR_INVALID_PARAMETER;

	// ��ϵͳע��������߳�
	{
		CEsThreadNode loAdd;
		int liPos;

		loAdd.muThreadID = GetCurrentThreadId();
		loAdd.mpOwnerWidget = lpContext->Widget;

		gpXuiSystem->moWidgetLock.Enter();

		liPos = gpXuiSystem->moWidgetWorkThreads.Insert(loAdd);

		gpXuiSystem->moWidgetLock.Leave();

		if(liPos < 0)
			THROW_FALSE;
	}

	// ���ÿͻ����̹߳���
	try
	{
		luResult = lpContext->ThreadProc(lpContext->Context);
	}
	catch(...)
	{
	}

	gpXuiSystem->ExitWidgetWorkThread(luResult);

	return luResult;
}

// ��һ��Config�ļ������ڴ�һ��config�ļ���ĿǰӦ����Factory�ӿ�ʵ���У����ڴ�һ��Conponent��Ӧ��Profile
IConfigFile* __stdcall CEinkuiSystem::OpenConfigFile(
	IN const wchar_t* nszPathName,				// �ļ�������·����
	IN ULONG nuCreationDisposition	// ͬCreateFile API���ƣ���������CF_CREATE_ALWAYS��CF_CREATE_NEW��CF_OPEN_ALWAYS��CF_OPEN_EXISTING�������CfgIface.h
	)
{
	return CfCreateConfig(nszPathName,nuCreationDisposition);
}


LRESULT CALLBACK CEinkuiSystem::MainWindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = 0;
	
	ERESULT luXuiResult = gpXuiSystem->DoCapture(hwnd,message,wParam,lParam,(HRESULT&)result);

	if(luXuiResult != ERESULT_WINMSG_SENDTO_NEXT)
		return result;

	switch (message)
	{
	case WM_IME_NOTIFY:
	case WM_IME_STARTCOMPOSITION:
	case WM_IME_ENDCOMPOSITION:
	case WM_IME_COMPOSITION:
		if(gpXuiSystem->TestFlag(ESYS_FLAG_INITOK)!=false && gpXuiSystem->mpImeContext != NULL)
			result = gpXuiSystem->mpImeContext->OnImeMessage(hwnd,message,wParam,lParam);
		else
			result = DefWindowProc(hwnd, message, wParam, lParam);
		break;
	case WM_CREATE:
		{
			SetCursor(LoadCursor(NULL, IDC_ARROW));

			BOOL lbForceIconic = TRUE;
			BOOL lbHasIconicBitmap = TRUE;
			DwmSetWindowAttribute(
				hwnd,
				DWMWA_FORCE_ICONIC_REPRESENTATION,
				&lbForceIconic,
				sizeof(lbForceIconic));

			DwmSetWindowAttribute(
				hwnd,
				DWMWA_HAS_ICONIC_BITMAP,
				&lbHasIconicBitmap,
				sizeof(lbHasIconicBitmap));

			result = gpXuiSystem->WsgOnCreate(hwnd);
		}
		break;
	case WM_MOVE:
		{
			result = 0;
		}
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_CHAR:
//	case WM_SYSKEYDOWN:
		{
			STELEMGR_WINMSG_FORWARD ldInput;

			if(gpXuiSystem->TestFlag(ESYS_FLAG_INITOK)!=false)
			{
				ldInput.WinMsgID = message;
				ldInput.wParam = wParam;
				ldInput.lParam = lParam;

				gpXuiSystem->mpElementManager->SimplePostMessage(NULL,EMSG_KEYBOARD_FORWARD,&ldInput,sizeof(ldInput),EMSG_POSTTYPE_FAST);
			}
			result = 0;
		}
		break;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			BeginPaint(hwnd, &ps);
			EndPaint(hwnd, &ps);
		}
		result = 0;
		break;
	case WM_ERASEBKGND:
		result = 1;
		break;
	case WM_NCPAINT:
	case WM_NCCALCSIZE:
	//case WM_NCACTIVATE:
		result = 0;
		break;
	case WM_TIMER:
		{
			//$ax$ Nov.27, 2018 ��ʱ���Σ���Ϊ���ֲ���(�򿪵����飩��ʱ������û�в�ȡ����Ķ�����ʽ���ᵼ����Ϣ�������ʱ������
			// �����������˶�ʱ������
			if(wParam == 0 && gpXuiSystem->TestFlag(ESYS_FLAG_INITOK)!=false)
			{
				gpXuiSystem->DetermineOPRunming();
			}
		}
		result = 0;
		break;

	case WM_DESTROY:
		{
			//Trace_Point(19053);// �˳�����
			if(gpXuiSystem->TestFlag(ESYS_FLAG_INITOK)!=false && gpXuiSystem->mhOperationThread != NULL)
			{
				//Trace_Flush();

				gpXuiSystem->mpElementManager->EndMessageQueue();

				CExMessage::PostMessage(NULL,NULL,EMSG_QUIT_XUI,CExMessage::DataInvalid,EMSG_POSTTYPE_QUIT);

				WaitForSingleObject(gpXuiSystem->mhOperationThread,10000);
			}
			PostQuitMessage(0);
		}
		result = 1;
		break;
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
	case WM_LBUTTONDBLCLK:
	case WM_RBUTTONDBLCLK:
	case WM_XBUTTONDBLCLK:
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MOUSEWHEEL:
	case WM_MOUSELEAVE:
		{
			if(gpXuiSystem->TestFlag(ESYS_FLAG_INITOK)!=false)
				 gpXuiSystem->OnWinMouse(hwnd,message,wParam,lParam);
			result = 0;
		}
		break;
	case WM_EI_TOUCH:
		{
		if (gpXuiSystem->TestFlag(ESYS_FLAG_INITOK) != false)
			gpXuiSystem->OnEinkTouch(hwnd, message, wParam, lParam);
		result = 0;
		}
		break;
	case WM_EI_DRAW:
	{
		gpXuiSystem->ClearEinkBuffer();
		gpXuiSystem->UpdateView();
	}
	case WM_EI_ACTIVATE:
	{
		if (wParam != 0)
		{
			//˵���Ǳ��л���ǰ̨��
			SET_TP_AREA ldArea;
			ldArea.Rect.x = 0;
			ldArea.Rect.w = 1920;
			ldArea.Rect.y = 0;
			ldArea.Rect.h = 1080;
			ldArea.Index = 0;
			ldArea.Flag = 0;
			EiSetTpArea(ldArea);

			ldArea.Flag = SET_SP_AREA_TOUCH_PEN;
			EiSetTpArea(ldArea);

			//�л�����������
			EiSetWaveformMode(GI_WAVEFORM_DU2);
			EiCleanupScreen(0xff);
			Sleep(585); //DU260+15*5   ��16֡ˢ��ÿ֡���5ms
			EiSetWaveformMode(GI_WAVEFORM_GC16);
			gpXuiSystem->ClearEinkBuffer();

			gpXuiSystem->EnableRender();
			gpXuiSystem->UpdateView();

			gpXuiSystem->moWidgetContext.EnableTickDetection(true);
		}
		else
		{
			gpXuiSystem->DisableRender();

			gpXuiSystem->moWidgetContext.EnableTickDetection(false);
		}
		
	}
	break;
	case WM_EI_ROTATION:
	{
		if (gpXuiSystem->muAutoRotate != 0)
		{
			ULONG lulRoration = (ULONG)wParam;

			gpXuiSystem->mpElementManager->SimplePostMessage(NULL, EMSG_SCREEN_ROTATION,&lulRoration,sizeof(lulRoration), EMSG_POSTTYPE_FAST);

		}
	}
	break;
	case WM_SYSCOMMAND:
		switch(wParam)
		{
		case SC_CLOSE:
			DestroyWindow(gpXuiSystem->GetMainWindow());
			break;
		//case SC_MINIMIZE:
		//	gpXuiSystem->mpXuiGraphics->StopPainting(true);
		//	CExMessage::SendMessage(NULL,NULL,0,CExMessage::DataInvalid);	//Ϊ�˺ͻ����߳�ͬ������ֹ��ǰ��Ȼ�ڻ��ƣ�

		//	gpXuiSystem->mbWindowHide = true;
		default:
			return DefWindowProc(hwnd, message, wParam, lParam);
		}
		break;
	case WM_POWERBROADCAST:
		if(wParam == PBT_APMRESUMEAUTOMATIC)
		{
			//wark up
			gpXuiSystem->moWidgetContext.EnableTickDetection(true);
		}
		else
		if(wParam == PBT_APMSUSPEND)
		{
			//sleep
			gpXuiSystem->moWidgetContext.EnableTickDetection(false);
		}
		else if (wParam == PBT_POWERSETTINGCHANGE)
		{
			//connect standby
			POWERBROADCAST_SETTING* lpSetting = (POWERBROADCAST_SETTING*)lParam;
			BREAK_ON_NULL(lpSetting);
			if (lpSetting->PowerSetting == GUID_CONSOLE_DISPLAY_STATE)
			{
				DWORD ldwStatus = 0;
				memcpy_s(&ldwStatus, sizeof(DWORD), lpSetting->Data, sizeof(DWORD));
				if (ldwStatus == 0)
				{
					//����connect standby
					OutputDebugString(L"enter connect standby");
					gpXuiSystem->moWidgetContext.EnableTickDetection(false);
				}
				else if (ldwStatus == 1)
				{
					//�뿪connect standby
					OutputDebugString(L"leave connect standby");
					gpXuiSystem->moWidgetContext.EnableTickDetection(true);
				}
				else if (ldwStatus == 2)
				{
					//�ػ�
					OutputDebugString(L"Os shut down");
				}
			}
		}
		else
			DefWindowProc(hwnd, message, wParam, lParam);
		break;
	case WM_TOUCH:
		{
			gpXuiSystem->mbIsTouch = true;

			break;
		}
	case WM_DWMSENDICONICTHUMBNAIL:
		{
			// This window is being asked to provide its iconic bitmap. This indicates
			// a thumbnail is being drawn.
			//�������ϵ�СԤ��ͼ
			//HBITMAP lhBitmap = gpXuiSystem->mpXuiGraphics->GetCurrentBitmap(HIWORD(lParam), LOWORD(lParam)); 
			HBITMAP lhBitmap = NULL; 
			CExMessage::SendMessage(NULL,NULL,EMSG_MAIN_GET_CURRENT_BITMAP,lParam,&lhBitmap,sizeof(HBITMAP));

			if (lhBitmap != NULL)
			{
				if(gpXuiSystem->mpSmallPreView != NULL)
					DeleteObject(gpXuiSystem->mpSmallPreView);

				gpXuiSystem->mpSmallPreView = lhBitmap;
			}

			DwmSetIconicThumbnail(hwnd, gpXuiSystem->mpSmallPreView, 0);

		}
		break;
// 	case WM_DWMSENDICONICLIVEPREVIEWBITMAP:
// 		break;
	default:
		if(gpXuiSystem->TestFlag(ESYS_FLAG_INITOK)!=false && gpXuiSystem->muiCustomWinMsg == message)
		{
			result = gpXuiSystem->MessageFromXuiToWindowUi(wParam,lParam);
		}
		else
			result = DefWindowProc(hwnd, message, wParam, lParam);
	}

	return result;
}

// �����ڵ�Windows��Ϣ���̺���
LRESULT CEinkuiSystem::MessageFromXuiToWindowUi(
	WPARAM wParam,
	LPARAM lParam
	)
{
	switch((ULONG)wParam)
	{
	case 'WgCb':
		{
			// �ص�
			gpXuiSystem->CallClientProcedure((PSTES_WINTHREAD_CALLBACK)lParam);
		}
		break;
	case 'Exit':
		{
			DestroyWindow(gpXuiSystem->mhMainWnd);
		}
		break;
	case 'Show':
		{
			ShowWindow(gpXuiSystem->mhMainWnd,(ULONG)lParam!=0?SW_SHOW:SW_HIDE);
			if((ULONG)lParam!=0)
				UpdateWindow(gpXuiSystem->mhMainWnd);
		}
		break;
	case 'Idtt':
		{
			// ��ȡʶ����
		}
		break;
	//case 'UdWu':
	//	{
	//		if(lParam != NULL)
	//			InvalidateRect(mhMainWnd,NULL,FALSE);

	//		//��Ϊ���ǵĽ������»����ˣ�������Ҫ����ϵͳ�´���Ȼ��������Ҫ��ʾ��Ԥ��ͼ��
	//		DwmInvalidateIconicBitmaps(mhMainWnd);
	//	}
	//	break;
	//case 'CWSz':
	//	{
	//		SetWindowPos(mhMainWnd,NULL,mlNewPaintboardX,mlNewPaintboardY,mlNewPaintboardWidth,mlNewPaintboardHeight,SWP_NOZORDER);
	//	}
	//	break;
	default:;
	}

	return TRUE;
}


void CEinkuiSystem::RenderProcedule(ULONG nuCrtTick)
{
	if (miDisableRender != 0 || nuCrtTick - muRenderTick < EINK_RENDER_INTERVAL)	// ��ǰһ����Ⱦ�ļ���������Ҫ��
	{
		// ���ö�ʱ������Ⱦ��־�����󷵻�
		InterlockedExchange(&miToRender,1);
		return;
	}

	mpXuiGraphics->DoRender(nuCrtTick, InterlockedCompareExchange(&miRefreshEink, 0, 1) != 0);

	// ��ֹ��Ϣ�����ж����ˢ����Ϣ
	InterlockedExchange(&miDiscardRenderMessage, 1);
	ULONG flag = 1;
	mpElementManager->SimplePostMessage(NULL, EMSG_SYSTEM_RENDER,&flag,sizeof(flag));//��ϵͳ�����յ�����Ϣʱ���ָ�ˢ��ʱ�����miDiscardRenderMessage��־

	//InterlockedCompareExchange(&miMissedMouseTest, 1, 0);
	if (/*miMissedMouseTest > 0 &&*/ InterlockedIncrement(&miMissedMouseTest) >= 3)
	{
		InterlockedExchange(&miMissedMouseTest, 0);

		//ִ��һ������⣬ȷ������µ�Ԫ���ܵõ���ȷ���������
		mpElementManager->OnMsgMouseForward(NULL);
	}
}

// �յ�Windows Mouse��Ϣ
void CEinkuiSystem::OnWinMouse(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_LBUTTONDOWN:
		SetWinMouseCapture(EELMGR_WINCAP_LB);
		break;
	case WM_RBUTTONDOWN:
		SetWinMouseCapture(EELMGR_WINCAP_RB);
		break;
	case WM_MBUTTONDOWN:
		SetWinMouseCapture(EELMGR_WINCAP_MB);
		break;
	case WM_LBUTTONUP:
		ReleaseWinMouseCapture(EELMGR_WINCAP_LB);
		break;
	case WM_RBUTTONUP:
		ReleaseWinMouseCapture(EELMGR_WINCAP_RB);
		break;
	case WM_MBUTTONUP:
		ReleaseWinMouseCapture(EELMGR_WINCAP_MB);
		break;
	default:;
	}

	STELEMGR_WINMSG_FORWARD ldMouse;

	ldMouse.WinMsgID = message;
	ldMouse.wParam = wParam;
	ldMouse.lParam = lParam;

	mpElementManager->SimplePostMessage(NULL,EMSG_MOUSE_FORWARD,&ldMouse,sizeof(ldMouse),EMSG_POSTTYPE_FAST);
}

// �յ�Eink Touch��Ϣ
void CEinkuiSystem::OnEinkTouch(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
// �޸Ĵ˴��룬��������Ϣ���������Ϣת���������߼�δ������Ax.2017.08.16
// 	PSTEMS_TOUCH lpTouch = (PSTEMS_TOUCH)lParam;
// 
// 	if (lpTouch == NULL)
// 		return;
// 
// 	mpElementManager->SimplePostMessage(NULL, EMSG_TOUCH_FORWARD, lpTouch, sizeof(STEMS_TOUCH), EMSG_POSTTYPE_NORMAL);
	STELEMGR_WINMSG_FORWARD ldMouse;
	PEI_TOUCHINPUT_POINT lpTouch = (PEI_TOUCHINPUT_POINT)lParam;

	if (lpTouch == NULL)
		return;

	ldMouse.lParam = MAKELONG((SHORT)lpTouch->x, (SHORT)lpTouch->y);
	switch (lpTouch->Flags)
	{
	case EI_TOUCHEVENTF_DOWN:
		ldMouse.WinMsgID = WM_LBUTTONDOWN;
		ldMouse.wParam = MK_LBUTTON;
		break;
	case EI_TOUCHEVENTF_MOVE:
		ldMouse.WinMsgID = WM_MOUSEMOVE;
		ldMouse.wParam = MK_LBUTTON;
		break;
	case EI_TOUCHEVENTF_UP:
		ldMouse.WinMsgID = WM_LBUTTONUP;
		ldMouse.wParam = 0;
		break;
	default:
		return;
	}

	mpElementManager->SimplePostMessage(NULL, EMSG_MOUSE_FORWARD, &ldMouse, sizeof(ldMouse), EMSG_POSTTYPE_FAST);// EMSG_POSTTYPE_NORMAL);

}


// �ص������ߵĺ������ɹ���������̷߳��ͻָ���Ϣ
void CEinkuiSystem::CallClientProcedure(PSTES_WINTHREAD_CALLBACK npToCall)
{
	//ULONG ldUlongArr[2];

	//if(npToCall == NULL || npToCall->Signature != 'WgCc')
	//	ldUlongArr[1] = ERESULT_WRONG_PARAMETERS;
	//else
	//	ldUlongArr[1] = (npToCall->Applicant->*npToCall->ProcedureToCall)(npToCall->Flag,npToCall->ConText);

	//// ������Ϣ�������߳�
	//ldUlongArr[0] = npToCall->BlockID;

	//mpElementManager->SimplePostMessage(NULL,EMSG_WINCALL_COMPLETED,ldUlongArr,sizeof(ldUlongArr),EMSG_POSTTYPE_FAST);

	ERESULT luResult;

	if(npToCall == NULL || npToCall->Signature != 'WgCc')
		luResult = ERESULT_WRONG_PARAMETERS;
	else
		luResult = (npToCall->Applicant->*npToCall->ProcedureToCall)(npToCall->Flag,npToCall->ConText);

	moThreadBlock.SetBlockState(npToCall->BlockID,luResult);

	//if(npToCall->WaitEvent != NULL)
	//{
	//	SetEvent(npToCall->WaitEvent);
	//}

	delete npToCall;
}

// ����ʹ��Windows�����̻߳ص������Widget��������Ҫ����Windows�Ľ���ģ�飬��ʾ�����Windows�����������Windows������صı�����Windows�����߳���ִ�еĹ��ܣ�
// ���ù��ܴ�����ֲ���Լ�Element�����������IBaseObject�����һ�����������У�ͨ�����ñ�����������ʹ��Windows�����߳������ص��趨�Ķ������������ú����ķ���ֵҲ��
// ͨ��������ֱ�ӷ��ظ��������ĵ����ߡ�����������ܵĹ����У������Ĵ��뽫�����ڴ˷����У�����ȷ��������������ˢ�У�����Xui����Ľ��潫������Ӧ��
ERESULT __stdcall CEinkuiSystem::CallBackByWinUiThread(
	IBaseObject* npApplicant,		// �ص����󣬲���һ��Ҫ���ǵ��ñ������Ķ�����Ҳ�����Ǳ�Ķ���ָ��
	PXUI_CALLBACK npProcedureToCall,	//�ص����̣����̵ķ���ֵ���᷵�ظ�������
	ULONG nuFlag,			// ���ݸ��ص������Ĳ���
	LPVOID npConText,		// ���ݸ��ص������Ĳ���
	bool nbStall		// �ȴ������ڼ��Ƿ���Ҫ���������XUI��Ϣ�����nbStal��Ϊture����ֱ�ӵȴ����أ���������XUI��Ϣѭ��
						// ʹ�ô˷������Ա�����ñ������ڼ䣬�ٴ��������
	)
{
	STES_WINTHREAD_CALLBACK* lpCall;
	LRESULT luResult;
	ULONG luID = 0;
	//HANDLE lhEvent = NULL;
	bool lbOperationThread = IsRunningInOperationThread();

	lpCall = new STES_WINTHREAD_CALLBACK;
	if(lpCall == NULL)
		return ERESULT_INSUFFICIENT_RESOURCES;

	luID = moThreadBlock.AddBlock();

	lpCall->BlockID = luID;
	lpCall->Applicant = npApplicant;
	lpCall->Flag = nuFlag;
	lpCall->ConText = npConText;
	lpCall->ProcedureToCall = npProcedureToCall;
	lpCall->Signature = 'WgCc';
	lpCall->Size = sizeof(STES_WINTHREAD_CALLBACK);

	//if(IsRunningInOperationThread()==false || nbStall != false)
	//{
	//	lhEvent = CreateEvent(NULL,true,false,NULL);
	//	if(lhEvent == NULL)
	//		return ERESULT_INSUFFICIENT_RESOURCES;
	//}
	//lpCall->WaitEvent = lhEvent;
	if(lbOperationThread == false || nbStall != false)
		luResult = (::SendMessage(GetMainWindow(),muiCustomWinMsg,'WgCb',(LPARAM)lpCall)!=FALSE)?ERESULT_SUCCESS:ERESULT_UNSUCCESSFUL;
	else
		luResult = (::PostMessage(GetMainWindow(),muiCustomWinMsg,'WgCb',(LPARAM)lpCall)!=FALSE)?ERESULT_SUCCESS:ERESULT_UNSUCCESSFUL;

	if(luResult == ERESULT_SUCCESS)
	{
		if(lbOperationThread != false && nbStall == false)
		{
			// ������������
			//InterlockedIncrement(&mlWinCallBack);

			// �ֽ���Ϣ���ȴ�CallID��ͬ����Ϣ����
			luResult = OpThreadMessageLoop(luID);
			if(luResult == ERESULT_QUIT_XUI)
			{
				// ��������ʱ��㣬ǡ���յ����˳���Ϣ���ǣ�����һ���ǳ����õ������ֱ���˳�����
				ExitProcess(-1);
			}

			//InterlockedDecrement(&mlWinCallBack);
		}

		luResult = moThreadBlock.ReadBlockState(luID);

		moThreadBlock.RemoveBlock(luID);

	}

	//if(lhEvent != NULL)
	//	CloseHandle(lhEvent);

	return (ULONG)luResult;
}

// ע��Windows��Ϣ���أ�ͨ�������ܿ�����XUIϵͳ����Windows��Ϣ֮ǰ��ȡ��ע��Windows��Ϣ
// �����ȡ��Windows��Ϣ�Ĺ���Ҫ�����ܶ��ݣ���Ϊ�˿�XUIϵͳ��������Ҫ�̴߳���ͬ���ȴ�״̬��
// ����ERESULT_WINMSG_SENDTO_NEXT��XUI����Ϣ���ݸ���һ�������ߣ����߽���XUIϵͳ���ʹ�����������ֵ����ֹ��Windows��Ϣ�Ĵ��ݹ����Լ�XUI�Ը���Ϣ�Ĵ���
ERESULT __stdcall CEinkuiSystem::CaptureWindowsMessage(
	IN UINT nuiWinMsgID,	// ϣ�������Windows��Ϣ��ID
	IN IXsElement* npApplicant,	//����ص��Ķ���
	IN PWINMSG_CALLBACK npProcedure	//��Windows��Ϣ���͸��˺���
	)
{
	CEsWsgCapture loAdd;
	ERESULT luReval = ERESULT_UNSUCCESSFUL;

	if(npApplicant == NULL)
		return ERESULT_ELEMENT_INVALID;

	moCaptorLock.Enter();

	loAdd.muiWsgID = nuiWinMsgID;
	loAdd.mpApplicant= npApplicant;
	loAdd.mpAppItr = npApplicant->GetIterator();
	loAdd.mpProcedure = npProcedure;

	if(mpElementManager->VerifyIterator(loAdd.mpAppItr)!=ERESULT_SUCCESS)
		return ERESULT_ITERATOR_INVALID;

	if(moCaptors.Insert(loAdd,false)>=0)
		luReval = ERESULT_SUCCESS;

	moCaptorLock.Leave();

	return luReval;
}


// ����һ������˷���ͨ��ΪSystemWidget�Ĳ˵�ģ����ã�����ģ��һ���û�������ϼ���Ϊ��������ᱻ��������ǰ�ļ��̽������ϵľ���EITR_STYLE_COMMAND�Ķ���
ERESULT __stdcall CEinkuiSystem::GenerateCommand(
	nes_command::ESCOMMAND neCmd
	)
{
	STEMS_HOTKEY ldHotKey;
	ERESULT luResult = ERESULT_SUCCESS;

	switch(neCmd)
	{
	case nes_command::eCopy:
		ldHotKey.HotKeyID = EHOTKEY_COPY;
		break;
	case nes_command::eCut:
		ldHotKey.HotKeyID = EHOTKEY_CUT;
		break;
	case nes_command::ePaste:
		ldHotKey.HotKeyID = EHOTKEY_PASTE;
		break;
	case nes_command::eDelete:
		ldHotKey.HotKeyID = EHOTKEY_DELETE;
		break;
	case nes_command::eSelAll:
		ldHotKey.HotKeyID = EHOTKEY_SELECT_ALL;
		break;
	default:
		luResult = ERESULT_UNSUCCESSFUL;
	}

	if(luResult == ERESULT_SUCCESS)
		CExMessage::PostMessage(NULL,NULL,EMSG_HOTKEY_PRESSED,ldHotKey,EMSG_POSTTYPE_NORMAL);

	return luResult;
}


//// ��������߳�
//ERESULT CEinkuiSystem::ExecuteDetermineThread(LPVOID /*Context*/)
//{
//	ULONG luThreadID;
//	HANDLE lhThread = CreateThread(NULL,128*1024,(LPTHREAD_START_ROUTINE)CEinkuiSystem::DetermineThread,NULL,NULL,&luThreadID);
//
//	if(lhThread == NULL)
//		return ERESULT_UNSUCCESSFUL;
//
//	mhOperationThread = lhThread;
//
//	return ERESULT_SUCCESS;
//}


//// ����߳�
//ULONG WINAPI CEinkuiSystem::DetermineThread(LPVOID /*Context*/)
//{
//	do 
//	{
//		if(WaitForSingleObject(gpXuiSystem->mhExitDetermine,1000)!= WAIT_TIMEOUT)
//			break;
//
//		gpXuiSystem->DetermineOPRunming();
//
//	} while(1);
//
//	return 0;
//}
// �������߳�����״̬
void CEinkuiSystem::DetermineOPRunming(void)
{
	//$ax$ Nov.27, 2018 ��ʱ���Σ���Ϊ���ֲ���(�򿪵����飩��ʱ������û�в�ȡ����Ķ�����ʽ���ᵼ����Ϣ�������ʱ������
	// �����������˶Դ˴��ĵ��ã�����gpXuiSystem->DetermineOPRunming();
	ULONG luElapsed;
	bool lbPullOut = false;
	CFilePathName loFile;

	if(IsDebuggerPresent() != FALSE)
		return ;

	//Trace_ULONG(28132,GetTickCount());//����߳�����
	moWidgetLock.Enter();

	luElapsed = moWidgetContext.CheckElapsedTick();
	//Trace_ULONG(26368,luElapsed);//��ȥʱ��
	if(luElapsed >= 10000)	// ����10��û����Ӧ
	{
		if(moWidgetContext.HasTriedPulling()==false)
		{
			//Trace_Point(19018);//�����߳��쳣
			moWidgetContext.SetTriedPulling();
			lbPullOut = CThreadAbort::PullOut(mhOperationThread);
		}
		//else
		//if(luElapsed <= 10000)
		//{	
		//	Trace_Point(19067);// С��8��ʱ����ʱ������
		//	lbPullOut = true;
		//}
		else
		{
			//Trace_Point(23800);//�Ѿ����Թ����߳��ϳ���̶��ʧ�ܰ�����ô�رս�������������
		}
		//if(lbPullOut != false)
			//Trace_Flush();
	}

	moWidgetLock.Leave();

	return;
	// ���汾���ṩ�Զ�������������Ĺ���
	//if(luElapsed < 10000 || lbPullOut != false)
	//	return ;

	//Trace_Point(18027);//ֱ�ӻָ�ʧ�ܣ�ɱ���߳�
	//TerminateThread(mhOperationThread,-1);

	//// reload myself

	//loFile.SetByModulePathName();

	//Trace_Point(18923);//������������
	//ShellExecute(NULL,L"open",loFile.GetPathName(),L"Reload",NULL,SW_SHOW);

	////ExitProcess(-1);
	//Trace_Flush();

	//DestroyWindow(gpXuiSystem->mhMainWnd);
}


// ����Windows WM_CREATE��Ϣ
LRESULT CEinkuiSystem::WsgOnCreate(HWND nhWindow)
{
	mhMainWnd = nhWindow;

	return 0;
}

// ���붨ʱ�����������ô����Ķ�ʱ������Ҫע��
ERESULT CEinkuiSystem::SetTimer(
	IN CXuiIterator* npIterator,// ������
	IN ULONG nuID,	  // ��ʱ��ID
	IN ULONG nuRepeat,// ��Ҫ�ظ������Ĵ�����MAXULONG32��ʾ��Զ�ظ�
	IN ULONG nuDuration,	// ��������
	IN void* npContext//�����ģ������Ŷ�ʱ����Ϣ���͸�������
	)
{
	ERESULT luResult = ERESULT_WRONG_PARAMETERS;
	PSTES_TIMER lpTimer = NULL;

	moTimerListLock.Enter();

	do 
	{
		if(npIterator == NULL || nuID == 0 || nuDuration == 0 || nuRepeat == 0)
			break;

		if(FindTimer(npIterator,nuID)!=false)
		{
			luResult = ERESULT_OBJECT_EXISTED;
			break;
		}

		lpTimer = new STES_TIMER;
		if(lpTimer == NULL)
		{
			ERESULT_INSUFFICIENT_RESOURCES;
			break;
		}

		lpTimer->mpIterator = npIterator;
		lpTimer->muID = nuID;
		lpTimer->muBaseTick = GetTickCount();
		lpTimer->muRecentTick = lpTimer->muBaseTick;
		lpTimer->muEndTick = lpTimer->muBaseTick + nuDuration;
		lpTimer->muDuration = nuDuration;
		lpTimer->muKicked = 0;
		lpTimer->muRepeat = nuRepeat;
		lpTimer->mpContext = npContext;

		if(moTimers.Insert(lpTimer)<0)
			luResult = ERESULT_UNSUCCESSFUL;

		// �ɹ���
		luResult = ERESULT_SUCCESS;

	} while (false);

	moTimerListLock.Leave();

	// �ͷ���Ϣ����������ʹ�Ķ�ʱ����������������
	mpElementManager->SimplePostMessage(NULL,EMSG_WAKE_UP,NULL,0);

	return luResult;
}

// ���ٶ�ʱ��
ERESULT CEinkuiSystem::KillTimer(
	IN CXuiIterator* npIterator,// ������
	IN ULONG nuID	  // ��ʱ��ID
	)
{
	ERESULT luResult;

	moTimerListLock.Enter();

	luResult = FindTimer(npIterator,nuID,true)?ERESULT_SUCCESS:ERESULT_UNSUCCESSFUL;

	moTimerListLock.Leave();

	return luResult;
}

// ���һ���ɾ��һ����ʱ��������ʱ���������ڲ�����
bool CEinkuiSystem::FindTimer(
	IN CXuiIterator* npIterator,// ������
	IN ULONG nuID,	  // ��ʱ��ID
	IN bool nbDelete	// �Ƿ�ɾ��
	)
{
	bool lbResult = false;

	moTimerListLock.Enter();

	for (int i=0; i < moTimers.Size();i++)
	{
		if(moTimers[i]->mpIterator == npIterator && moTimers[i]->muID == nuID)
		{
			lbResult = true;
			if(nbDelete != false)
			{
 				STES_TIMER* lpTimer = moTimers[i];
 				CMM_SAFE_DELETE(lpTimer);
				lbResult = moTimers.RemoveByIndex(i);
			}
			break;
		}
	}

	moTimerListLock.Leave();

	return lbResult;
}


// �Ӷ�ʱ��������ȡ���һ�����ܼ�����ʱ������tick�����룩Ϊ��λ
ULONG CEinkuiSystem::GetTickCountToKickTimer(ULONG nuCrtTick)
{
	ULONG luTick = 33;	// �������ȡ��ֵ����û33���뻽�Ѳ����߳�һ��
	ULONG luCrtTick = nuCrtTick;

	moTimerListLock.Enter();

	if(moTimers.Size() > 0)
	{
		if(luCrtTick >= moTimers[0]->muEndTick)
			luTick = 0;	// �Ѿ���������ʱ����
		else
			luTick = moTimers[0]->muEndTick - luCrtTick;
	}

	moTimerListLock.Leave();

	return luTick;
}


// ��ʱ����������
void CEinkuiSystem::KickTimers(ULONG nuCrtTick)
{
	STEMS_TIMER ldTimerParam;
	PSTES_TIMER lpTimer;

	moTimerListLock.Enter();

	for (int i = moTimers.Size()-1;i>=0;)	// ����Զ��ʼ�ж�
	{
		if(moTimers[i]->muEndTick <= nuCrtTick)
		{
			// ������

			// ���ȰѶ�ʱ���Ӷ�����������
			lpTimer = moTimers[i];
			moTimers.RemoveByIndex(i);

			ldTimerParam.TimerID = lpTimer->muID;
			ldTimerParam.BaseTick = lpTimer->muBaseTick;
			ldTimerParam.CurrentTick = nuCrtTick;
			ldTimerParam.ElapsedTick = nuCrtTick - lpTimer->muRecentTick;
			ldTimerParam.Kicked = ++lpTimer->muKicked;
			ldTimerParam.Context = lpTimer->mpContext;

			if(ERESULT_SUCCEEDED(mpElementManager->SimplePostMessage(lpTimer->mpIterator,EMSG_TIMER,&ldTimerParam,sizeof(ldTimerParam))))
			{
				// �޸Ķ�ʱ����״ֵ̬
				lpTimer->muRecentTick = nuCrtTick;

				do 
				{	// ׼����һ�������㣬�������е�ǰ�������뵱ǰʱ��֮��δ������
					lpTimer->muEndTick += lpTimer->muDuration;
					if(lpTimer->muRepeat != MAXDWORD)
						lpTimer->muRepeat--;
				} while (lpTimer->muRepeat > 0 && lpTimer->muEndTick <= nuCrtTick);
			}
			else
			{
				//Trace_Point(16926);//���Ͷ�ʱ����Ϣʧ��
				lpTimer->muRepeat = 0;//����ʹ�øö�ʱ�����ر���
			}


			// �ж��Ƿ���Ҫ��ص���ʱ��������
			if(lpTimer->muRepeat > 0)
				moTimers.Insert(lpTimer);
			else 
				CMM_SAFE_DELETE(lpTimer);	//Ҫ�ͷ���Դ

			// ���´Ӷ�β��ⶨʱ��
			i = moTimers.Size()-1;

			continue;

		}
		i--;
	}

	moTimerListLock.Leave();
}

// ���ڽ���������Ϣ
ERESULT CEinkuiSystem::SystemMessageReceiver(IEinkuiMessage* npMsg)
{
	ERESULT luResult=ERESULT_UNEXPECTED_MESSAGE;

	try
	{
		switch (npMsg->GetMessageID())
		{
		case EMSG_SCREEN_ROTATION:
		{
			ULONG lulRoration = *(ULONG*)npMsg->GetInputData();
			ULONG luAccept = MAXULONG32;

			// ѯ��Widget�Ƿ�ת��
			CExMessage::SendMessage(gpXuiSystem->mpElementManager->GetDesktop(), NULL, EMSG_QUERY_SWGT_ROTATED, lulRoration, &luAccept, sizeof(luAccept));

			if(luAccept == MAXULONG32)
				break;

			EiSetScreenOrient(luAccept);
			gpXuiSystem->ResetPaintboard();

			CExMessage::SendMessage(gpXuiSystem->mpElementManager->GetDesktop(), NULL, EMSG_SWGT_ROTATED, luAccept);

			gpXuiSystem->UpdateView();
		}
		break;
		case EMSG_SYSTEM_RENDER:
		{
			ULONG luTick;

			if (*(ULONG*)npMsg->GetInputData() == 0)
			{
				if (miDiscardRenderMessage != 0)
				{
					luResult = ERESULT_SUCCESS;
					break;
				}
				luTick = GetTickCount();
				RenderProcedule(luTick);
			}
			else
			if (*(ULONG*)npMsg->GetInputData() == 1)
					InterlockedExchange(&miDiscardRenderMessage, 0);

			luResult = ERESULT_SUCCESS;
		}
			break;
		case EMSG_MOUSE_FORWARD:
			{
				// modified by ax ������ô�������ң�����Ĵ��뻹������Bug����һ��Windows�̻߳ص��ĵȴ������У�ִ��һ��ģ̬���ڣ��ͻ�������
				//if(mlWinCallBack != 0)	// �����ǰ���ڽ�������״̬��������
				//	break;

				if(npMsg->GetInputDataSize() != sizeof(STELEMGR_WINMSG_FORWARD))
				{
					luResult = ERESULT_WRONG_PARAMETERS;
					break;
				}

				const PSTELEMGR_WINMSG_FORWARD lpForward = (const PSTELEMGR_WINMSG_FORWARD)npMsg->GetInputData();
				InterlockedExchange(&miMissedMouseTest,0);
				luResult = mpElementManager->OnMsgMouseForward(lpForward);

				if(lpForward->WinMsgID == WM_LBUTTONUP)
					mbIsTouch = false;
			}
			break;
		case EMSG_KEYBOARD_FORWARD:
			{
				// modified by ax ������ô�������ң�����Ĵ��뻹������Bug����һ��Windows�̻߳ص��ĵȴ������У�ִ��һ��ģ̬���ڣ��ͻ�������
				//if(mlWinCallBack != 0)	// �����ǰ���ڽ�������״̬��������
				//	break;

				if(npMsg->GetInputDataSize() != sizeof(STELEMGR_WINMSG_FORWARD))
				{
					luResult = ERESULT_WRONG_PARAMETERS;
					break;
				}

				luResult = mpElementManager->OnMsgKeyboardForward((const PSTELEMGR_WINMSG_FORWARD)npMsg->GetInputData());
			}
			break;
		case EMSG_INPUT_ENABLE:
			{
				luResult = mpElementManager->OnMsgEnalbeInput();
			}
			break;
		// �޸Ĵ˴��룬��������Ϣ���������Ϣת���������߼�δ������Ax.2017.08.16
		//case EMSG_TOUCH_FORWARD:
		//	{
		//		if (npMsg->GetInputDataSize() != sizeof(STEMS_TOUCH))
		//		{
		//			luResult = ERESULT_WRONG_PARAMETERS;
		//			break;
		//		}

		//		const PSTEMS_TOUCH lpForward = (const PSTEMS_TOUCH)npMsg->GetInputData();
		//		InterlockedExchange(&miMissedMouseTest, 0);
		//		luResult = mpElementManager->OnMsgEinkTouchForward(lpForward);
		//	}
		//	break;
		case EMSG_TIMER:
			{
				if(npMsg->GetInputDataSize() != sizeof(STEMS_TIMER))
				{
					luResult = ERESULT_WRONG_PARAMETERS;
					break;
				}

				luResult = OnMsgTimer((const PSTEMS_TIMER)npMsg->GetInputData());
			}
			break;
		case  EMSG_QUIT_XUI:
			{
				EndXui();
				luResult = ERESULT_QUIT_XUI;	// ֪ͨ�߳��˳�
			}
			break;
		//case EMSG_WINCALL_COMPLETED:
		//	{
		//		if(npMsg->GetInputDataSize() == sizeof(ULONG)*2)
		//		{
		//			moThreadBlock.SetBlockState(*(ULONG*)npMsg->GetInputData(),*((ULONG*)npMsg->GetInputData()+1));
		//		}
		//		luResult = ERESULT_QUIT_OPLOOP;
		//	}
		//	break;
		//case EMSG_MODAL_COMPLETED:
		//	{
		//		if(npMsg->GetInputDataSize() == sizeof(ULONG))
		//		{
		//			muOpLoopResult = *(ULONG*)npMsg->GetInputData();
		//		}
		//		luResult = ERESULT_QUIT_OPLOOP;
		//	}
		//	break;
		case EMSG_WAKE_UP:
			luResult = ERESULT_SUCCESS;
			break;
		case EMSG_SET_KEYBOARD_FOCUS:
			{
				if(npMsg->GetInputDataSize() != sizeof(IEinkuiIterator*) || (*(IEinkuiIterator**)npMsg->GetInputData())->IsKindOf(GET_BUILTIN_NAME(CXuiIterator))==false)
				{
					luResult = ERESULT_WRONG_PARAMETERS;
					break;
				}

				luResult = mpElementManager->ApplyKeyBoard(*(IEinkuiIterator**)npMsg->GetInputData());
			}
			break;
		case EMSG_SET_ACTIVE:
			{
				if(npMsg->GetInputDataSize() != sizeof(IEinkuiIterator*) || (*(IEinkuiIterator**)npMsg->GetInputData())->IsKindOf(GET_BUILTIN_NAME(CXuiIterator))==false)
				{
					luResult = ERESULT_WRONG_PARAMETERS;
					break;
				}

				mpElementManager->AssignActivation(*(IEinkuiIterator**)npMsg->GetInputData());
				luResult = ERESULT_SUCCESS;
			}
			break;
		case EMSG_RELEASE_KEYBOARD_FOCUS:
			{
				if(npMsg->GetInputDataSize() != sizeof(ST_RELEASE_KEYFOCUS))
				{
					luResult = ERESULT_WRONG_PARAMETERS;
					break;
				}

				mpElementManager->ReleaseKeyBoard((PST_RELEASE_KEYFOCUS)npMsg->GetInputData());
				luResult =ERESULT_SUCCESS;
			}
			break;
		//case EMSG_MAIN_RESIZED:
		//	{
		//		if(npMsg->GetInputDataSize() != sizeof(ULONG)*3)
		//		{
		//			luResult = ERESULT_WRONG_PARAMETERS;
		//			break;
		//		}
		//		if(mpXuiGraphics != NULL)
		//		{
		//			if(*((ULONG*)npMsg->GetInputData()+2) != 0)
		//				mpXuiGraphics->Resize(-1,-1);

		//			ULONG luSize[2];
		//			Sleep(50);	//������Ϣһ��
		//			luSize[0] = *(ULONG*)npMsg->GetInputData();
		//			luSize[1] = *((ULONG*)npMsg->GetInputData()+1);

		//			mpXuiGraphics->Resize(luSize[0],luSize[1]);

		//			//ʹ��SendMessage�������ʹ���濴���������ıȽ���
		//			CExMessage::SendMessage(mpElementManager->GetDesktop(),NULL,EMSG_SWGT_MW_RESIZED,luSize);
		//		}
		//		luResult = ERESULT_SUCCESS;
		//	}
		//	break;
		case EMSG_OPTHREAD_LOCK:
			{
				if(npMsg->GetInputDataSize() != sizeof(STEMS_OPT_LOCK))
				{
					luResult = ERESULT_WRONG_PARAMETERS;
					break;
				}
				SetEvent(((PSTEMS_OPT_LOCK)npMsg->GetInputData())->OptToRelease);

				mbLocked = true;
				WaitForSingleObject(((PSTEMS_OPT_LOCK)npMsg->GetInputData())->OptToWait,INFINITE);

				mbLocked = false;
				SetEvent(((PSTEMS_OPT_LOCK)npMsg->GetInputData())->OptToRelease);
			}
			break;
		case EMSG_APPLY_DESTROY:
			{
				if(npMsg->GetMessageSender() == NULL)
				{
					luResult = ERESULT_WRONG_PARAMETERS;
					break;
				}

				luResult = mpElementManager->DestroyElement(npMsg->GetMessageSender());
			}
			break;
		case EMSG_CLOSE_WIDGET:
			{
				if(npMsg->GetInputDataSize() != sizeof(IXsWidget*))
				{
					luResult = ERESULT_WRONG_PARAMETERS;
					break;
				}
				luResult = CloseWidget(*(IXsWidget**)npMsg->GetInputData());
			}
			break;
		case EMSG_HOTKEY_PRESSED:
			{
				//��ݼ���Ϣ
				const STEMS_HOTKEY* lpHotKey;
				nes_command::ESCOMMAND leCmd;

				luResult = CExMessage::GetInputDataBuffer(npMsg,lpHotKey);
				if(luResult != ERESULT_SUCCESS)
					break;
				switch(lpHotKey->HotKeyID)
				{
				case EHOTKEY_COPY:
					leCmd = nes_command::eCopy;
					break;
				case EHOTKEY_CUT:
					leCmd = nes_command::eCut;
					break;
				case EHOTKEY_PASTE:
					leCmd = nes_command::ePaste;
					break;
				case EHOTKEY_DELETE:
					leCmd = nes_command::eDelete;
					break;
				case EHOTKEY_SELECT_ALL:
					leCmd = nes_command::eSelAll;
					break;
				case EHOTKEY_ENTER:
					leCmd = nes_command::eEnter;
					break;
				case EHOTKEY_ESC:
					leCmd = nes_command::eEsc;
					break;
				case EHOTKEY_ALTF4:
					leCmd = nes_command::eAltF4;
					break;
				default:
					leCmd = nes_command::eInvalid;
					luResult = ERESULT_KEY_ACCEPTED;
				}
				if(leCmd != nes_command::eInvalid)
					mpElementManager->SendCommand(leCmd);
			}
			break;
		case EMSG_MAIN_GET_CURRENT_BITMAP:
			{
				LPARAM llParam = *(LPARAM*)npMsg->GetInputData();
				HBITMAP* lppHbitmap = (HBITMAP*)npMsg->GetOutputBuffer();
				if(lppHbitmap != NULL)
					*lppHbitmap = gpXuiSystem->mpXuiGraphics->GetCurrentBitmap(HIWORD(llParam), LOWORD(llParam));

				luResult = ERESULT_SUCCESS;

				break;
			}
		default:;
		}
	}
	catch(...)
	{
		luResult = ERESULT_UNKNOWN_ERROR;
	}



	return luResult;
}

// ��ʱ����Ϣ
ERESULT CEinkuiSystem::OnMsgTimer(const PSTEMS_TIMER npTimerInput)
{
	//static cmmVector<ULONG> TimerLog;
	if (npTimerInput->TimerID == ELMGR_TIMERID_RENDER)
	{

		// �ж��Ƿ���Ҫ����
		//if (InterlockedCompareExchange(&miToRender, 0, 1) != 0 || InterlockedIncrement(&miLostToRender)*EINK_RENDER_TIMER >= 20)
		//{	// ��ǰ�л���Ҫ�󣬻�����10����(500ms*20)��ʱ��û��ˢ�½���
		if (InterlockedCompareExchange(&miToRender, 0, 1) != 0)
		{
			//TimerLog.Insert(-1,npTimerInput->CurrentTick);

			//InterlockedExchange(&miLostToRender, 0);

			RenderProcedule(npTimerInput->CurrentTick);
		}

		return ERESULT_SUCCESS;
	}
	else
		return ERESULT_UNEXPECTED_MESSAGE;
}

// ��ĳ��Element��������children���գ�???��ʱ��û�����߳�ͬ����Ҫ��ֹ�����̵߳���
IEinkuiBitmap* __stdcall CEinkuiSystem::TakeSnapshot(
	IEinkuiIterator* npToShot,
	const D2D1_RECT_F& crSourceRegion,	// ��������Ŀ��Ԫ�صľֲ�����ϵ
	const D2D_SIZE_F& crBriefSize,		// ����ͼ�ߴ磬���յĽ����һ������ͼ
	const FLOAT* ColorRGBA
	)
{

	return mpXuiGraphics->TakeSnapshot(npToShot,crSourceRegion,crBriefSize,ColorRGBA);
}

// �������»����Ը���������ͼ��Idealife��ģʽ��������ͼ�ػ棬ֻҪ����һ�ξͻ�������������ͼ��
// �ظ����ñ����������ᵼ���ظ��Ļ��ƣ�
void __stdcall CEinkuiSystem::UpdateView(
	IN bool nbRefresh	// �����ύȫ��
	)
{
	if(nbRefresh != false)
		InterlockedExchange(&miRefreshEink, 1);

	LONG flag = 0;
	mpElementManager->SimplePostMessage(NULL, EMSG_SYSTEM_RENDER,&flag,sizeof(flag));
}

// ����Eink���ƻص���ÿ��UIϵͳ��Eink�����ύʱ����ָ���Ļص�����
ERESULT __stdcall CEinkuiSystem::SetEinkUpdatingCallBack(
	IN PXUI_EINKUPDATING_CALLBACK npFunction,
	IN PVOID npContext
)
{
	if(mpXuiGraphics->mpEinkUpdatingCallBack!=NULL)
		return  ERESULT_OBJECT_EXISTED;

	mpXuiGraphics->mpEinkUpdatingCallBack = npFunction;
	mpXuiGraphics->mpEinkUpdatingContext = npContext;

	return ERROR_SUCCESS;
}


// ����Eink���棻����Ĺ���ԭ����ͨ��Eink�����е������������ȶԴ���ʾ���ݣ�ֻ����ͬ�Ĳ��ַ���Eink�����Eink��Ļ��Ҫȫ���ػ�
//    ����Ҫ����Eink���棬ʹ��ȫ�����ݻ��Ƶ�Eink����Ҫ����App���»Eink��Ļ�����
void __stdcall CEinkuiSystem::ClearEinkBuffer(void)
{
	mpXuiGraphics->ClearEinkBuffer(true);
}

// ���Direct2D�Ĺ����ӿڣ������ͷţ�ֻ�ܹ��ڻ�ñ�IXuiPaintBoard�ӿڵ���Ϣ��Ӧ�ڼ�ʹ�÷��ص�RenderTarget�ӿڣ���Ҫ���ڱ���˴����صĽӿڣ�����ʧЧ���δ֪����
// ͨ������CXuiGraphy���ͬ������ʵ�֣�ͬ����ȫ��ͬ
ID2D1Factory* __stdcall CEinkuiSystem::GetD2dFactory(void)
{
	return mpXuiGraphics->GetD2dFactory();
}

// ���WIC�����������ͷţ�ֻ�ܹ��ڻ�ñ�IXuiPaintBoard�ӿڵ���Ϣ��Ӧ�ڼ�ʹ�÷��ص�RenderTarget�ӿڣ���Ҫ���ڱ���˴����صĽӿڣ�����ʧЧ���δ֪����
// ͨ������CXuiGraphy���ͬ������ʵ�֣�ͬ����ȫ��ͬ
IWICImagingFactory* __stdcall CEinkuiSystem::GetWICFactory(void)
{
	return mpXuiGraphics->GetWICFactory();
}

// ���Direct Write�����������ͷţ�ֻ�ܹ��ڻ�ñ�IXuiPaintBoard�ӿڵ���Ϣ��Ӧ�ڼ�ʹ�÷��ص�RenderTarget�ӿڣ���Ҫ���ڱ���˴����صĽӿڣ�����ʧЧ���δ֪����
// ͨ������CXuiGraphy���ͬ������ʵ�֣�ͬ����ȫ��ͬ
IDWriteFactory* __stdcall CEinkuiSystem::GetDWriteFactory(void)
{
	return mpXuiGraphics->GetDWriteFactory();
}

void __stdcall CEinkuiSystem::SystemTest(ULONG nuValue)
{
	//Sleep(1000);

	//mbThreadKilled = true;

	//TerminateThread(mhOperationThread,0);
	GUID guid = EGUID_EMGR_DROP_TEST;

	//Trace_ULONG(19122,nuValue);//ϵͳ���Լ�¼
}

// ��������Ԥ����
ERESULT __stdcall CEinkuiSystem::EnterForLazyUpdate(IEinkuiIterator* npRecipient)
{
	CXuiIterator* lpRecipient = dynamic_cast<CXuiIterator*>(npRecipient);

	//if(lpRecipient->CheckStyle(EITR_STYLE_LAZY_UPDATE)!= false)
		//mpElementManager->SimplePostMessage(npRecipient,EMSG_LAZY_UPATE,&mdLazyUpdate,sizeof(mdLazyUpdate));
	if(lpRecipient->CheckStyle(EITR_STYLE_LAZY_UPDATE)!= false)
		mpElementManager->SimplePostMessage(npRecipient,EMSG_LAZY_UPATE,&mdLazyUpdate,sizeof(mdLazyUpdate));
		//mpElementManager->SimpleSendMessage(npRecipient,EMSG_LAZY_UPATE,&mdLazyUpdate,sizeof(mdLazyUpdate),NULL,0);

	return ERESULT_ENUM_CHILD;
}

// �����������
ERESULT __stdcall CEinkuiSystem::LeaveForLazyUpdate(IEinkuiIterator* npRecipient)
{
	return ERESULT_SUCCESS;
}

// ����ĳҳ��Ϊģ̬�Ի���ʽ�����û�������ɸöԻ�����ʱ�����ϵ��������ֶ��޷�ʹ�á�
// XUI��ģ̬�Ի���ʽ��ȫ�ֵģ�����ģ̬�Ի��£����е�Widget������System Widget)��������Ӧ�û����룻���У������ܱ���ʹ��ģʽ�Ի����������Ǳ���ġ�
// ʹ�÷����ǣ����ȴ���ģʽ�Ի���������Ĭ������)��Ȼ����ñ���������ģ̬�Ի���ʽ����ʱ��ģ̬�Ի�Ԫ�ض��󽫻��յ�һ��EMSG_MODAL_ENTER��Ϣ�����������������Լ���ʾ������
// ����ģ̬�Ի������ж��û�����˶Ի��������Լ�������ExitModal�˳�ģ̬�Ի���ʽ
// ע�⣬ģ̬�Ի���ʽ�ǿ����ص�����ģ���ģ̬�Ի��£����Դ���ģ̬�Ի�������һ����˳�
ERESULT __stdcall CEinkuiSystem::DoModal(
	IEinkuiIterator* npModalElement		// ��Ԫ����ģ̬�Ի���������
	)
{
	if(IsRunningInOperationThread()==false)
	{
		return ERESULT_WRONG_THREAD;
	}

	if(mpElementManager->VerifyIterator(npModalElement)!=ERESULT_SUCCESS)
		return ERESULT_ITERATOR_INVALID;

	if(((CXuiIterator*)npModalElement)->CheckStyle(EITR_STYLE_POPUP)==false)
		return ERETULT_WRONG_STYLE;

	// ��ģ̬������һ����Ϣ
	ERESULT luResult = mpElementManager->SimpleSendMessage(npModalElement,EMSG_MODAL_ENTER,NULL,0,NULL,0);
	if(luResult != ERESULT_SUCCESS)
		return luResult;

	// ����Ϊ����
	npModalElement->SetActive();

	// ����ģ̬�Ի�������
	CXuiModalState loState;

	loState.muBlock = moThreadBlock.AddBlock();
	loState.mpTarget = npModalElement;
	mpElementManager->EnterModal();
	moModalStack.AddModal(loState);
	
	// ������Ϣѭ��
	luResult = OpThreadMessageLoop(loState.muBlock);
	if(luResult == ERESULT_QUIT_XUI && IsRunningInOperationThread()!=false)
	{
		// ��������ʱ��㣬ǡ���յ����˳���Ϣ���ǣ�����һ���ǳ����õ������ֱ���˳�����
		ExitProcess(-1);
	}

	moModalStack.RemoveModal(npModalElement);

	luResult = moThreadBlock.ReadBlockState(loState.muBlock);
	moThreadBlock.RemoveBlock(loState.muBlock);

	return luResult;
}

// �˳�ģ̬�Ի���ʽ
void __stdcall CEinkuiSystem::ExitModal(
	IEinkuiIterator* npModalElement,	// ��Ԫ����ģ̬�Ի���������
	ERESULT nuResult
	)
{
	//// ��ϵͳ�����˳���Ϣ
	//mpElementManager->SimplePostMessage(NULL,EMSG_MODAL_COMPLETED,&nuResult,sizeof(nuResult),EMSG_POSTTYPE_FAST);

	// �����ǰ�����ȫ��Windowsԭʼ������Ϣ
	//mpElementManager->moFastMessages.RemoveMessages(LMSG_TP_WIN_INPUT,0,0,XSMSG_REMOVE_TYPE);
	mpElementManager->CleanHumanInput(false);

	// ͨ��Ԫ���ҵ����ţ�Ȼ����������״̬
	ULONG luBlockID = moModalStack.GetBlockIDOfModal(npModalElement);
	if(luBlockID != 0)
		moThreadBlock.SetBlockState(luBlockID,nuResult);
}

// ����windows��겶��
void CEinkuiSystem::SetWinMouseCapture(USHORT nsuFlag)
{
	if(InterlockedOr16((SHORT*)&msuWinCap,nsuFlag)==0)
		SetCapture(EinkuiGetSystem()->GetMainWindow());
}

// �ͷ�windows��겶��
void CEinkuiSystem::ReleaseWinMouseCapture(USHORT nsuFlag)
{
	if(msuWinCap == 0)
		return ;

	if(InterlockedAnd16((SHORT*)&msuWinCap,~nsuFlag)!=0 && msuWinCap==0)
		ReleaseCapture();
}

void CEinkuiSystem::WsgOnGetMinMaxInfo(MINMAXINFO *pMinMaxInfo)
{
	pMinMaxInfo->ptMinTrackSize.x = GetSystemMetrics(SM_CXMINTRACK);
	pMinMaxInfo->ptMinTrackSize.y = GetSystemMetrics(SM_CXMAXTRACK);
}

//void CEinkuiSystem::WsgOnSettingChanged()
//{
//	if (mbIsAutoSize != false)
//	{
//		RECT rcWorkArea;
//		SystemParametersInfo(SPI_GETWORKAREA, sizeof(rcWorkArea), &rcWorkArea, 0);
//		MoveWindow(mhMainWnd, rcWorkArea.left, rcWorkArea.top, rcWorkArea.right-rcWorkArea.left, rcWorkArea.bottom-rcWorkArea.top, TRUE);
//	}
//}

// Client���ã��˳�����XUIϵͳ
void __stdcall CEinkuiSystem::ExitXui(void)
{
	gpXuiSystem->mpElementManager->EndMessageQueue();

	::PostMessage(mhMainWnd,muiCustomWinMsg,'Exit',NULL);
}


// ʵ���˳�Xui��ʱ��
void CEinkuiSystem::EndXui(void)
{
	bool exception = false;
	__try {
		// �ر�ȫ����Widget
		for (int i = moAllWidgets.Size() - 1; i >= 0; i--)
		{
			CloseWidget(moAllWidgets[i]);
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		exception = true;
	}
}

//// ������˸Domodal�Ĵ���
//void CEinkuiSystem::FlashModalElement(IEinkuiIterator* npDodal){
//	mpXuiGraphics->FlashModalElement(npDodal);
//}

// �����ǰ���ģ̬����
IEinkuiIterator* CEinkuiSystem::GetTopModalElement(void)
{
	CXuiModalState loState;

	if(moModalStack.GetTopModel(loState)==false)
		return NULL;

	return loState.mpTarget;
}

//���β����Ƿ���TOUCH����
bool __stdcall CEinkuiSystem::IsTouch(void)
{
	return mbIsTouch;
}