/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "cmmstruct.h"
#include "cmmBaseObj.h"
#include "FactoryImp.h"








//��try����


//�����Ϲ滮
//BOOL APIENTRY DllMain( HMODULE hModule,
//                       DWORD  ul_reason_for_call,
//                       LPVOID lpReserved
//					 )
//{
//	switch (ul_reason_for_call)
//	{
//	case DLL_PROCESS_ATTACH:
//	case DLL_THREAD_ATTACH:
//	case DLL_THREAD_DETACH:
//	case DLL_PROCESS_DETACH:
//		break;
//	}
//	return TRUE;
//}


// ʵ���������࣬�����ӿ�ָ��
IElementFactory* XuiGetFactory()
{
	return CFactoryImp::GetUniqueObject();

}

void GetWidgetInformation(OUT wchar_t* npIconName,				//�����ҳ����ʾ��ͼ������
	OUT wchar_t* npPreviewPicName,		//��ӵ�ʱ����ʾ��Ԥ��ͼ����
	OUT LONG& rlWidth,					//ҳ��
	OUT LONG& rlHeight					//ҳ��
	)
{

}