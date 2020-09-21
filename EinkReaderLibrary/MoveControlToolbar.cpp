/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#include "StdAfx.h"
#include "MoveControlToolbar.h"
#include "XCtl.h"
#include "stdio.h"
#include "cmmStrHandle.h"
#include "cmmPath.h"
#include "MsgDefine.h"

DEFINE_BUILTIN_NAME(MoveControlToolbar)

CMoveControlToolbar::CMoveControlToolbar(void)
{
}

CMoveControlToolbar::~CMoveControlToolbar(void)
{
}

//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
ERESULT CMoveControlToolbar::OnElementCreate(IEinkuiIterator* npIterator)
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

ULONG CMoveControlToolbar::InitOnCreate(
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
		mpIterBtUP = mpIterator->GetSubElementByID(ZC_BT_UP);
		BREAK_ON_NULL(mpIterBtUP);

		mpIterBtDown = mpIterator->GetSubElementByID(ZC_BT_DOWN);
		BREAK_ON_NULL(mpIterBtDown);

		mpIterBtLeft = mpIterator->GetSubElementByID(ZC_BT_LEFT);
		BREAK_ON_NULL(mpIterBtLeft);

		mpIterBtRight = mpIterator->GetSubElementByID(ZC_BT_RIGHT);
		BREAK_ON_NULL(mpIterBtRight);

		leResult = ERESULT_SUCCESS;

	} while (false);

	CMM_SAFE_RELEASE(lpSubKey);

	// ��ϵͳע����Ҫ�յ�����Ϣ
	return leResult;
}

//���������ƶ�
void CMoveControlToolbar::MovePage(ULONG nulID)
{
	POINT ldPos;
	RECT ldRect;
	if (nulID == ZC_BT_UP)
	{
		ldPos.y = 300;
		ldPos.x = 0;
	}
	else if (nulID == ZC_BT_DOWN)
	{
		ldPos.y = -300;
		ldPos.x = 0;
	}
	if (nulID == ZC_BT_LEFT)
	{
		ldPos.x = 300;
		ldPos.y = 0;
	}
	if (nulID == ZC_BT_RIGHT)
	{
		ldPos.x = -300;
		ldPos.y = 0;
	}

	CExMessage::SendMessage(mpIterator->GetParent()->GetParent(), mpIterator, EEVT_ER_SET_PAGE_MOVE, ldPos, &ldRect, sizeof(RECT));
	ShowMoveButton(ldRect);
}

//����4���ƶ���ť��״̬
void CMoveControlToolbar::ShowMoveButton(RECT ldRect)
{
	if (mMoveForward == MoveForward::HORIZONTAL)
	{
		ldRect.top = 0;
		ldRect.bottom = 0;
	}
	else if (mMoveForward == MoveForward::VERTICAL)
	{
		ldRect.left = 0;
		ldRect.right = 0;
	}
	
	mpIterBtLeft->SetEnable(ldRect.left != 0);
	mpIterBtRight->SetEnable(ldRect.right != 0);
	mpIterBtUP->SetEnable(ldRect.top != 0);
	mpIterBtDown->SetEnable(ldRect.bottom != 0);
}

//���û�������
void CMoveControlToolbar::SetMoveForward(MoveForward forward)
{
	mMoveForward = forward;
}

//��ť�����¼�
ERESULT CMoveControlToolbar::OnCtlButtonClick(IEinkuiIterator* npSender)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		CExMessage::PostMessage(EinkuiGetSystem()->GetCurrentWidget()->GetHomePage(), mpIterator, EEVT_RESET_HIDE_TIME, true);

		ULONG llBtnID = npSender->GetID();
		switch (llBtnID)
		{
		case ZC_BT_UP:
		case ZC_BT_DOWN:
		case ZC_BT_LEFT:
		case ZC_BT_RIGHT:
		{
			//���������ƶ�
			MovePage(llBtnID);

			break;
		}
		default:
			break;
		}

		lResult = ERESULT_SUCCESS;
	} while (false);

	return lResult;
}

//��ʼ���Լ�
void CMoveControlToolbar::initData(void)
{
}

//��Ϣ������
ERESULT CMoveControlToolbar::ParseMessage(IEinkuiMessage* npMsg)
{
	ERESULT luResult = ERESULT_UNEXPECTED_MESSAGE;

	switch (npMsg->GetMessageID())
	{
	case EMSG_MODAL_ENTER:
	{
		// ����Ҫ�����ĶԻ���
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
void CMoveControlToolbar::OnTimer(
	PSTEMS_TIMER npStatus
	)
{
}

//Ԫ�زο��ߴ緢���仯
ERESULT CMoveControlToolbar::OnElementResized(D2D1_SIZE_F nNewSize)
{
	return ERESULT_SUCCESS;
}

//֪ͨԪ�ء���ʾ/���ء������ı�
ERESULT CMoveControlToolbar::OnElementShow(bool nbIsShow)
{
	return ERESULT_SUCCESS;
}