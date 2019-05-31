/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#include "stdafx.h"

#include "CommonHeader.h"
#include "XsWgtContext.h"

#include "ImeContext.h"


DEFINE_BUILTIN_NAME(CXsImeContext)



CXsImeContext::CXsImeContext()
{
	mhPreviousImc = NULL;
	mhImcToHaltIme = NULL;
	mbAssociated = false;
	mdComInfo.dwStyle = CFS_POINT;
	mdComInfo.ptCurrentPos.x = 0;
	mdComInfo.ptCurrentPos.y = 0;
}

CXsImeContext::~CXsImeContext()
{
	if(mbAssociated != false)
		ImmAssociateContext(EinkuiGetSystem()->GetMainWindow(),mhPreviousImc);
	if(mhImcToHaltIme != NULL)
		ImmDestroyContext(mhImcToHaltIme);
}

ERESULT CXsImeContext::InitOnCreate()
{
	mhImcToHaltIme = ImmCreateContext();

	ImmSetOpenStatus(mhImcToHaltIme,FALSE);

	return ERESULT_SUCCESS;
}


LRESULT CXsImeContext::OnImeMessage(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_IME_NOTIFY:	// ���û��л����뷨����IME��ʱ�����е�Context����ִ�п������������ԣ�ֻҪ�յ�������Ϣ����Ҫ������ǵĽ���IME��Context�Ƿ񱻿����ˡ�
		if(mhImcToHaltIme != NULL && wParam == IMN_SETOPENSTATUS && ImmGetOpenStatus(mhImcToHaltIme)!=FALSE)	
			ImmSetOpenStatus(mhImcToHaltIme,FALSE);		// ����ر�
		break;
	case WM_IME_STARTCOMPOSITION:
		{
			HIMC lhImc;

			lhImc = ImmGetContext(EinkuiGetSystem()->GetMainWindow());

			ImmSetCompositionWindow(lhImc,&mdComInfo);

			ImmReleaseContext(EinkuiGetSystem()->GetMainWindow(),lhImc);
		}
		break;
	default:;
	}

	return DefWindowProc(hwnd,message,wParam,lParam);
}

void CXsImeContext::EnableIme(void)
{
	if(mhImcToHaltIme == NULL || mbAssociated == false)
		return ;

	EinkuiGetSystem()->CallBackByWinUiThread(this,(PXUI_CALLBACK)&CXsImeContext::ChangeImeContextCallback,FALSE,NULL,true);
}

void CXsImeContext::DisableIme(void)
{
	if(mhImcToHaltIme == NULL || mbAssociated != false)
		return ;

	EinkuiGetSystem()->CallBackByWinUiThread(this,(PXUI_CALLBACK)&CXsImeContext::ChangeImeContextCallback,TRUE,NULL,true);
}

void CXsImeContext::SetImeCompositionWnd(D2D1_POINT_2F ndPosition)
{
	mdComInfo.dwStyle = CFS_FORCE_POSITION;//CFS_POINT;
	mdComInfo.ptCurrentPos.x = (LONG)(ndPosition.x+0.5f);
	mdComInfo.ptCurrentPos.y = (LONG)(ndPosition.y+0.5f);
}

ERESULT __stdcall CXsImeContext::ChangeImeContextCallback(ULONG nuFlag,LPVOID npContext)
{
	if(nuFlag != FALSE)
	{	// ����IME�������Խ���IME������
		if(mhImcToHaltIme == NULL || mbAssociated != false)
			return ERESULT_SUCCESS;

		mbAssociated = true;
		mhPreviousImc = ImmAssociateContext(EinkuiGetSystem()->GetMainWindow(),mhImcToHaltIme);
	}
	else
	{
		// ȡ�����ã�����Ĭ�ϵ�IME
		if(mhImcToHaltIme == NULL || mbAssociated == false)
			return ERESULT_SUCCESS;

		mbAssociated = false;
		ImmAssociateContext(EinkuiGetSystem()->GetMainWindow(),mhPreviousImc);
		mhPreviousImc = NULL;
	}

	return ERESULT_SUCCESS;
}



