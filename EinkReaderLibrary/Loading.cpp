/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#include "StdAfx.h"
#include "Loading.h"
#include "XCtl.h"
#include "stdio.h"
#include "cmmStrHandle.h"
#include "cmmPath.h"
#include "MsgDefine.h"
#include <Shlobj.h>

DEFINE_BUILTIN_NAME(LoadingView)

CLoadingView::CLoadingView(void)
{
	mpIterFileName = NULL;
	mpIterDi = NULL;
	mpIterDd = NULL;
	mplStep = NULL;
	mWaitHandle = NULL;
	mpIterIndex = NULL;
}


CLoadingView::~CLoadingView(void)
{
}

//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
ERESULT CLoadingView::OnElementCreate(IEinkuiIterator* npIterator)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		if(CXuiElement::OnElementCreate(npIterator) != ERESULT_SUCCESS)
			break;

		mpIterator->ModifyStyles(EITR_STYLE_POPUP);
		
		//mpIterPicture->SetRotation(90.0f);

		lResult = ERESULT_SUCCESS;

	}while(false);

	return lResult;
}

ULONG CLoadingView::InitOnCreate(
	IN IEinkuiIterator* npParent,	// ������ָ��
	IN ICfKey* npTemplete,		// npTemplete��Key ID����EID��ֵ��������EType
	IN ULONG nuEID	// �����Ϊ0��MAXULONG32����ָ����Ԫ�ص�EID; ����ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��XUIϵͳ�Զ�����
	)
{
	ERESULT leResult = ERESULT_UNSUCCESSFUL;
	ICfKey* lpSubKey = NULL;

	do 
	{
		//���ȵ��û���
		leResult = 	CXuiElement::InitOnCreate(npParent,npTemplete,nuEID);
		if(leResult != ERESULT_SUCCESS)
			break;


		//��ȡ������
		mpIterDd = mpIterator->GetSubElementByID(2);
		BREAK_ON_NULL(mpIterDd);

		mpIterDi = mpIterator->GetSubElementByID(1);
		BREAK_ON_NULL(mpIterDi);

		mpIterFileName = mpIterator->GetSubElementByID(3);
		BREAK_ON_NULL(mpIterFileName);

		mpIterIndex = mpIterator->GetSubElementByID(10);
		BREAK_ON_NULL(mpIterIndex);
		mpIterIndex->SetVisible(false);

		leResult = ERESULT_SUCCESS;

	} while (false);

	CMM_SAFE_RELEASE(lpSubKey);

	// ��ϵͳע����Ҫ�յ�����Ϣ
	return leResult;
}

//�����Ѽ���ҳ��
void CLoadingView::SetPage(LONG niIndex)
{
	IConfigFile* lpProfile = NULL;
	wchar_t lszText[MAX_PATH] = { 0 };

	do 
	{
		if(mpIterIndex == NULL)
			break;

		wchar_t lszString[MAX_PATH] = { 0 };

		//��ȡ�������ַ���
		//Ϊ�˷��뷽�㣬�ַ��������root/string
		lpProfile = EinkuiGetSystem()->GetCurrentWidget()->GetDefaultFactory()->GetTempleteFile();
		ICfKey* lpCfKey = NULL;
		if (lpProfile != NULL)
		{
			lpCfKey = lpProfile->OpenKey(L"String2/LoadPageIndex");

			if (lpCfKey != NULL)
				lpCfKey->GetValue(lszText, MAX_PATH * sizeof(wchar_t));

		}
		CMM_SAFE_RELEASE(lpCfKey);

		swprintf_s(lszString, MAX_PATH, lszText, niIndex);
		CExMessage::PostMessageWithText(mpIterIndex, mpIterator, EACT_LABEL_SET_TEXT, lszString);
		
		if (mpIterIndex->IsVisible() == false && mpIterIndex->GetSizeX() > 50.0f)
		{
			mpIterIndex->SetPosition((mpIterator->GetSizeX() - mpIterIndex->GetSizeX()) / 2.0f, mpIterIndex->GetPositionY());
			mpIterIndex->SetVisible(true);
		}
			

	} while (false);
	
}

//��ť�����¼�
ERESULT CLoadingView::OnCtlButtonClick(IEinkuiIterator* npSender)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		

		lResult = ERESULT_SUCCESS;
	} while (false);

	return lResult;
}


void CLoadingView::DoModal(volatile LONG* nplStep, HANDLE nWaitHandle)
{
	do
	{
		mplStep = nplStep;
		mWaitHandle = nWaitHandle;

		mpIterator->SetVisible(true);
		mpIterator->SetActive();
		mpIterator->BringToTop();
		EinkuiGetSystem()->EnablePaintboard(false);
		EinkuiGetSystem()->UpdateView(true);
		mpIterator->SetTimer(LOAD_TIMER_MOVE, MAXULONG32, 1000, NULL);

		EinkuiGetSystem()->DoModal(mpIterator);


		mpIterator->Close();

	} while (false);
}

void CLoadingView::ExitModal()
{
	mpIterator->KillTimer(LOAD_TIMER_MOVE);
	//mpIterator->SetVisible(false);
	EinkuiGetSystem()->ExitModal(mpIterator, 0);
}

//��Ϣ������
ERESULT CLoadingView::ParseMessage(IEinkuiMessage* npMsg)
{
	ERESULT luResult = ERESULT_UNEXPECTED_MESSAGE;

	switch (npMsg->GetMessageID())
	{
	case EMSG_MODAL_ENTER:
	{
		//// ����Ҫ�����ĶԻ���
		//mpIterator->SetVisible(true);
		luResult = ERESULT_SUCCESS;
		break;
	}
	default:
		luResult = ERESULT_NOT_SET;
		break;
	}

	if (luResult == ERESULT_NOT_SET)
	{
		luResult = CXuiElement::ParseMessage(npMsg); // ���û����ͬ��������ע�⣺һ��Ҫ��������ֱ�ӻ���
	}

	return luResult;
}

//��ʱ��
void CLoadingView::OnTimer(
	PSTEMS_TIMER npStatus
	)
{
	if (npStatus->TimerID == LOAD_TIMER_MOVE)
	{
		//�ƶ�
		float lfMoved = mpIterDd->GetPositionX() + 51.0f;
		if ((lfMoved + mpIterDd->GetSizeX()) > (mpIterDi->GetPositionX() + mpIterDi->GetSizeX()))
			lfMoved = mpIterDi->GetPositionX()+2.0f;

		EiSetPartialUpdate(TRUE);
		EinkuiGetSystem()->ClearEinkBuffer();

		mpIterDd->SetPosition(lfMoved, mpIterDd->GetPositionY());

		if (mplStep != NULL && *mplStep == 0)
			ExitModal(); //�˳�

		if (mWaitHandle != NULL && WaitForSingleObject(mWaitHandle,100) == WAIT_OBJECT_0)
			ExitModal(); //�˳�
	}
}

//������Ϣ
ERESULT CLoadingView::OnPaint(IEinkuiPaintBoard* npPaintBoard)
{
	// ���Ʊ���
	if (mpBgBitmap != NULL)
		npPaintBoard->DrawBitmap(D2D1::RectF(0, 0, mpIterator->GetSizeX(), mpIterator->GetSizeY()),
			mpBgBitmap,
			ESPB_DRAWBMP_EXTEND);

	return ERESULT_SUCCESS;
}

//Ԫ�زο��ߴ緢���仯
ERESULT CLoadingView::OnElementResized(D2D1_SIZE_F nNewSize)
{
	//��λ
	if(mpIterFileName != NULL)
		mpIterFileName->SetPosition((nNewSize.width - mpIterFileName->GetSizeX()) / 2.0f, nNewSize.height/2.0f-100);

	if(mpIterDi != NULL)
		mpIterDi->SetPosition((nNewSize.width - mpIterDi->GetSizeX()) / 2.0f, mpIterFileName->GetPositionY() + 100.0f);
	if(mpIterDd != NULL)
		mpIterDd->SetPosition(mpIterDi->GetPositionX()+2.0f, mpIterDi->GetPositionY()+2.0f);

	if (mpIterIndex != NULL)
		mpIterIndex->SetPosition((nNewSize.width - mpIterIndex->GetSizeX()) / 2.0f, mpIterDi->GetPositionY() + 100.0f);

	return ERESULT_SUCCESS;
}


//�����ļ���
void CLoadingView::SetData(wchar_t* npszFileName)
{
	do 
	{
		BREAK_ON_NULL(npszFileName);
		CExMessage::SendMessageWithText(mpIterFileName, mpIterator, EACT_LABEL_SET_TEXT, npszFileName);

		//���¶�λ
		mpIterFileName->SetPosition((mpIterator->GetSizeX()-mpIterFileName->GetSizeX()) / 2.0f,mpIterFileName->GetPositionY());

	} while (false);
}

//֪ͨԪ�ء���ʾ/���ء������ı�
ERESULT CLoadingView::OnElementShow(bool nbIsShow)
{
	return ERESULT_SUCCESS;
}