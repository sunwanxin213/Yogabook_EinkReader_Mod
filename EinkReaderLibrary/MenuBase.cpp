/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#include "StdAfx.h"
#include "MenuBase.h"
#include "XCtl.h"
#include "stdio.h"
#include "cmmStrHandle.h"
#include "cmmPath.h"
#include "MsgDefine.h"


DEFINE_BUILTIN_NAME(MenuBase)

CMenuBase::CMenuBase(void)
{
	
}


CMenuBase::~CMenuBase(void)
{
	CExMessage::PostMessage(EinkuiGetSystem()->GetCurrentWidget()->GetHomePage(), mpIterator, EEVT_RESET_HIDE_TIME, true);
}

//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
ERESULT CMenuBase::OnElementCreate(IEinkuiIterator* npIterator)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		if(CXuiElement::OnElementCreate(npIterator) != ERESULT_SUCCESS)
			break;

		mpIterator->ModifyStyles(EITR_STYLE_POPUP);
		
		

		lResult = ERESULT_SUCCESS;

	}while(false);

	return lResult;
}

ULONG CMenuBase::InitOnCreate(
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
	
		CExMessage::PostMessage(EinkuiGetSystem()->GetCurrentWidget()->GetHomePage(), mpIterator, EEVT_RESET_HIDE_TIME, false);

		leResult = ERESULT_SUCCESS;

	} while (false);

	CMM_SAFE_RELEASE(lpSubKey);

	// ��ϵͳע����Ҫ�յ�����Ϣ
	return leResult;
}

//��ť�����¼�
ERESULT CMenuBase::OnCtlButtonClick(IEinkuiIterator* npSender)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{

		SendMessageToParent(EEVT_MENU_ITEM_CLICKED, npSender->GetID(),NULL,0);

		lResult = ERESULT_SUCCESS;
	} while (false);

	return lResult;
}

// ��������
ERESULT CMenuBase::OnMouseOwnerTest(const D2D1_POINT_2F& rPoint)
{
	ERESULT luResult = ERESULT_SUCCESS;

	if (!(rPoint.x < 0.0f || rPoint.x >= mpIterator->GetSizeX()
		|| rPoint.y < 0.0f || rPoint.y >= mpIterator->GetSizeY()))
	{
		luResult = ERESULT_MOUSE_OWNERSHIP;
	}
	else
	{
		luResult = ERESULT_SUCCESS;
	}



	return luResult;
}


//��Ϣ������
ERESULT CMenuBase::ParseMessage(IEinkuiMessage* npMsg)
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
	case EEVT_RBG_SELECTED_CHANGED:
	{
		ULONG lulSelectID = 0;
		luResult = CExMessage::GetInputData(npMsg, lulSelectID);
		if (luResult != ERESULT_SUCCESS)
			break;

		SendMessageToParent(EEVT_RBG_SELECTED_CHANGED, lulSelectID, NULL, 0);

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

