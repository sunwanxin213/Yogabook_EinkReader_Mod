/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#include "StdAfx.h"
#include "PageProgress.h"
#include "XCtl.h"
#include "stdio.h"
#include "cmmStrHandle.h"
#include "cmmPath.h"
#include "MsgDefine.h"
#include <math.h>

DEFINE_BUILTIN_NAME(PageProgress)


CPageProgress::CPageProgress(void)
{
	mpIterProgress = NULL;
	mpIterArrow = NULL;
	mfCurrentPage = 0.0f;
	mfPageCount = 0.0f;
}


CPageProgress::~CPageProgress(void)
{
}

//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
ERESULT CPageProgress::OnElementCreate(IEinkuiIterator* npIterator)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		if(CXuiElement::OnElementCreate(npIterator) != ERESULT_SUCCESS)
			break;

		mpIterator->ModifyStyles(EITR_STYLE_POPUP | EITR_STYLE_KEYBOARD);
		
		//mpIterPicture->SetRotation(90.0f);

		lResult = ERESULT_SUCCESS;

	}while(false);

	return lResult;
}

ULONG CPageProgress::InitOnCreate(
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
		mpIterProgress = mpIterator->GetSubElementByID(PP_PROGRESS);
		BREAK_ON_NULL(mpIterProgress);
		
		mpIterArrow = mpIterator->GetSubElementByID(3);
		BREAK_ON_NULL(mpIterArrow);
		
		leResult = ERESULT_SUCCESS;

	} while (false);

	CMM_SAFE_RELEASE(lpSubKey);

	// ��ϵͳע����Ҫ�յ�����Ϣ
	return leResult;
}

// ��������
void CPageProgress::SetData(FLOAT nfCurrentPage, FLOAT nfPageCount)
{
	mfCurrentPage = nfCurrentPage;
	mfPageCount = nfPageCount;

	if (nfPageCount > 1)
	{
		CExMessage::SendMessage(mpIterProgress, mpIterator, EACT_SLIDERBAR_SET_RANGE, mfPageCount - 1.0f);
		CExMessage::SendMessage(mpIterProgress, mpIterator, EACT_SLIDERBAR_SET_POSITION, mfCurrentPage - 1.0f);
		mpIterProgress->SetEnable(true);
	}
	else
	{
		CExMessage::SendMessage(mpIterProgress, mpIterator, EACT_SLIDERBAR_SET_RANGE, mfPageCount);
		CExMessage::SendMessage(mpIterProgress, mpIterator, EACT_SLIDERBAR_SET_POSITION, mfCurrentPage);

		mpIterProgress->SetEnable(false);
	}

}

//���ü�ͷλ��
void CPageProgress::SetArrowPos(D2D1_POINT_2F ndPos)
{
	mpIterArrow->SetPosition(ndPos.x - mpIterArrow->GetSizeX()/2.0f,ndPos.y);
}

//��ť�����¼�
ERESULT CPageProgress::OnCtlButtonClick(IEinkuiIterator* npSender)
{

	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		//CExMessage::PostMessage(EinkuiGetSystem()->GetCurrentWidget()->GetHomePage(), mpIterator, EEVT_RESET_HIDE_TIME, true);

		ULONG llBtnID = npSender->GetID();
		switch (llBtnID)
		{
		case PP_BT_CLOSE:
		{
			//�رս���
			mpIterator->SetVisible(false);
			
			break;
		}
		default:
			break;
		}


		lResult = ERESULT_SUCCESS;
	} while (false);

	return lResult;
}

//��Ϣ������
ERESULT CPageProgress::ParseMessage(IEinkuiMessage* npMsg)
{
	ERESULT luResult = ERESULT_UNEXPECTED_MESSAGE;

	switch (npMsg->GetMessageID())
	{
	case EACT_SLIDERBAR_DRAG_END:
	case EACT_SLIDERBAR_THUMB_CLICK:
	//case EACT_SLIDERBAR_DRAGING:
	{
		// �϶�������
		//CExMessage::PostMessage(EinkuiGetSystem()->GetCurrentWidget()->GetHomePage(), mpIterator, EEVT_RESET_HIDE_TIME, true);

		if (npMsg->GetInputDataSize() != sizeof(FLOAT))
		{
			luResult = ERESULT_WRONG_PARAMETERS;
			break;
		}
		FLOAT* lpValue = (FLOAT*)npMsg->GetInputData();
		int liCurrent = (int)round(*lpValue)+1;
		if (liCurrent < 1)
			liCurrent = 1;
		//PostMessageToParent(EEVT_ER_PAGE_JUMP, liCurrent);
		CExMessage::PostMessage(mpIterator->GetParent(), mpIterator, EEVT_ER_PAGE_JUMP, liCurrent);

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
void CPageProgress::OnTimer(
	PSTEMS_TIMER npStatus
	)
{

}

//Ԫ�زο��ߴ緢���仯
ERESULT CPageProgress::OnElementResized(D2D1_SIZE_F nNewSize)
{
	
	return ERESULT_SUCCESS;
}


//֪ͨԪ�ء���ʾ/���ء������ı�
ERESULT CPageProgress::OnElementShow(bool nbIsShow)
{
	//EiSetHomebarStatus(nbIsShow == false ? GI_HOMEBAR_SHOW : GI_HOMEBAR_HIDE);
	if (nbIsShow != false)
	{
		mpIterator->BringToTop();
	}
	return ERESULT_SUCCESS;
}

//��������뿪
void  CPageProgress::OnMouseFocus(PSTEMS_STATE_CHANGE npState)
{
	if (npState->State == 0)
	{
		mpIterator->BringToTop();
	}
}