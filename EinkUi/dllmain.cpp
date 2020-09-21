// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

#include "CommonHeader.h"
#include "EinkIteAPI.h"


// {EA3270AA-3D8B-4246-AE7F-0D65F3D7BD60}
static const GUID EGUID_EMGR_DROP_TEST =
{ 0xea3270aa, 0x3d8b, 0x4246, { 0xae, 0x7f, 0xd, 0x65, 0xf3, 0xd7, 0xbd, 0x60 } };



BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
	}
	break;
	case DLL_PROCESS_DETACH:
	{
	}
	break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;;
	}
	return TRUE;
}

// XUI�������������ϣ���޸�DPI��������ڵ��ô˺���ǰ����Win API SetProcessDPIAware
int __stdcall EinkuiStart(
	IN const wchar_t* nswModulePath,	// System Widget��ʵ��ģ����ļ�·����
	IN const wchar_t* nswHomeTempleteName,	// System Widget��Home Page��templete key������
	IN const wchar_t* nswClassesRegPath,	// Xuiע�������ڵ�ע���·������:Software\\Lenovo\\Veriface5\\PublicClasses
	IN ULONG nuAutoRotate,				// ����֧���Զ���ת
	IN PEINKUI_CUSTOMDRAW_SETTINGS npCustomDraw,	// �Ի�Eink
	IN const wchar_t* nswWndTittle		// �����ڱ���
)
{
	int liResult = -1;
	static bool glbStarted = false;
	STES_START_UP ldStart;

	wchar_t lszTemp[MAX_PATH] = { 0 };
	GetTempPath(MAX_PATH, lszTemp);
	wcscat_s(lszTemp, MAX_PATH, L".\\Xui.trf");
	//Trace_Init(lszTemp,L"XUI.DLL",2,200,L"Xui",L"35EEB40700AF554DB8CCEF09984873EF",0x402CE600,0x1CF8FC4);

	HRESULT hr;
	ID2D1Factory* lpFactory = NULL;

	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &lpFactory);
	if (FAILED(hr))
		return -1;

	lpFactory->GetDesktopDpi(&ldStart.Dpi.width, &ldStart.Dpi.height);

	lpFactory->Release();

	if (glbStarted != false)
	{
		//Trace_Point(22233);// �ظ�������Ŀǰ�Ѿ�������
		return -1;
	}

	CoInitialize(NULL);

	CEinkuiSystem* lpXuiSystem = CEinkuiSystem::GetUniqueObject();

	//Trace_Time(29955);//����XUIϵͳ
	if (lpXuiSystem != NULL)
	{
		ldStart.HomeTempleteName = nswHomeTempleteName;
		ldStart.ModulePathName = nswModulePath;
		ldStart.ClassesRegPath = nswClassesRegPath;
		ldStart.WindowTitle = nswWndTittle;
		ldStart.CustomDraw = npCustomDraw;
		ldStart.WaitingCaller = NULL;
		ldStart.AutoRotate = nuAutoRotate;

		liResult = lpXuiSystem->Startup(ldStart);
		lpXuiSystem->Release();
	}

	CoUninitialize();

	//Trace_Exit();

	return liResult;
}

// ���XUIϵͳ�ӿ�
IEinkuiSystem* __stdcall EinkuiGetSystem(void)
{
	return CEinkuiSystem::gpXuiSystem;
}

