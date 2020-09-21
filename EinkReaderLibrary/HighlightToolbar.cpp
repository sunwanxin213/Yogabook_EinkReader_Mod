/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#include "StdAfx.h"
#include "HighlightToolbar.h"
#include "XCtl.h"
#include "stdio.h"
#include "cmmStrHandle.h"
#include "cmmPath.h"
#include "MsgDefine.h"
#include <shellapi.h>

DEFINE_BUILTIN_NAME(HighlightToolbar)

CHighlightToolbar::CHighlightToolbar(void)
{
	mpIterBtCopy = NULL;
	mpIterBtHighlight = NULL;
	mpIterBtDeleteLine = NULL;
	mpIterBtLine = NULL;
	mpIterBtDelete = NULL;
}


CHighlightToolbar::~CHighlightToolbar(void)
{
}

//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
ERESULT CHighlightToolbar::OnElementCreate(IEinkuiIterator* npIterator)
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

ULONG CHighlightToolbar::InitOnCreate(
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
		mpIterBtDelete = mpIterator->GetSubElementByID(HIGHLIGHT_BT_DELETE);
		BREAK_ON_NULL(mpIterBtDelete);
		//
		//mpIterBtAdd = mpIterator->GetSubElementByID(ZC_BT_ADD);
		//BREAK_ON_NULL(mpIterBtAdd);

		//mpIterBtSub = mpIterator->GetSubElementByID(ZC_BT_SUB);
		//BREAK_ON_NULL(mpIterBtSub);


		////��ȡ������
		//mpIterBtUP = mpIterator->GetSubElementByID(ZC_BT_UP);
		//BREAK_ON_NULL(mpIterBtUP);

		//mpIterBtDown = mpIterator->GetSubElementByID(ZC_BT_DOWN);
		//BREAK_ON_NULL(mpIterBtDown);

		//mpIterBtLeft = mpIterator->GetSubElementByID(ZC_BT_LEFT);
		//BREAK_ON_NULL(mpIterBtLeft);

		//mpIterBtRight = mpIterator->GetSubElementByID(ZC_BT_RIGHT);
		//BREAK_ON_NULL(mpIterBtRight);

		/*mpIterBtDefault = mpIterator->GetSubElementByID(ZC_BT_DEFAULT);
		BREAK_ON_NULL(mpIterBtDefault);*/

		leResult = ERESULT_SUCCESS;

	} while (false);

	CMM_SAFE_RELEASE(lpSubKey);

	// ��ϵͳע����Ҫ�յ�����Ϣ
	return leResult;
}

//��ť�����¼�
ERESULT CHighlightToolbar::OnCtlButtonClick(IEinkuiIterator* npSender)
{

	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		int liBtnID = (int)npSender->GetID();
		switch (liBtnID)
		{
		case HIGHLIGHT_BT_COPY:
		case HIGHLIGHT_BT_HIGHLIGHT:
		case HIGHLIGHT_BT_DELETE_LINE:
		case HIGHLIGHT_BT_UNDER_LINE:
		case HIGHLIGHT_BT_DELETE:
		case HIGHLIGHT_BT_TRANSLATE:
		{
			//���Ƶ����а�
			//PostMessageToParent(EEVT_COPY_TEXT, CExMessage::DataInvalid);
			CExMessage::SendMessage(mpIterator->GetParent(), mpIterator, EEVT_HIGHLIGHT_BT_EVENT, liBtnID);

			mpIterator->SetVisible(false);
			//initData();

			break;
		}
	
		default:
			break;
		}


		lResult = ERESULT_SUCCESS;
	} while (false);

	return lResult;
}

//����highlight����
void CHighlightToolbar::SetHighlightCount(int niCount)
{
	if (niCount > 0)
	{
		mpIterBtDelete->SetEnable(true);
	}
	else
	{
		mpIterBtDelete->SetEnable(false);
	}
}

//��ʼ���Լ�
void CHighlightToolbar::initData(void)
{

}


//��Ϣ������
ERESULT CHighlightToolbar::ParseMessage(IEinkuiMessage* npMsg)
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
void CHighlightToolbar::OnTimer(
	PSTEMS_TIMER npStatus
	)
{

}

//Ԫ�زο��ߴ緢���仯
ERESULT CHighlightToolbar::OnElementResized(D2D1_SIZE_F nNewSize)
{
	
	return ERESULT_SUCCESS;
}


//֪ͨԪ�ء���ʾ/���ء������ı�
ERESULT CHighlightToolbar::OnElementShow(bool nbIsShow)
{
	//EiSetHomebarStatus(nbIsShow == false ? GI_HOMEBAR_SHOW : GI_HOMEBAR_HIDE);
	if (nbIsShow != false)
	{
		mpIterator->BringToTop();
	}
	return ERESULT_SUCCESS;
}