/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#include "StdAfx.h"
#include "EditToolbar.h"
#include "XCtl.h"
#include "stdio.h"
#include "cmmStrHandle.h"
#include "cmmPath.h"
#include "MsgDefine.h"

DEFINE_BUILTIN_NAME(EditToolbar)


CEditToolbar::CEditToolbar(void)
{
	mpIterBtUndo = NULL;
	mpIterBtRedo = NULL;

	mdwPenColorIndex = 0;
	mdwPenWidthIndex = 1;

	mpMenuPen = NULL;
	mpIterGroup = NULL;
	mbIsHand = false;

	mulSelectID = PEN_MODE_PEN;
}


CEditToolbar::~CEditToolbar(void)
{
}

//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
ERESULT CEditToolbar::OnElementCreate(IEinkuiIterator* npIterator)
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

ULONG CEditToolbar::InitOnCreate(
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
		mpIterBtUndo = mpIterator->GetSubElementByID(ET_BT_UNDO);
		BREAK_ON_NULL(mpIterBtUndo);
		mpIterBtUndo->SetEnable(false);
		//mpIterBtUndo->SetPosition(mpIterBtUndo->GetPositionX() - 100.0f, mpIterBtUndo->GetPositionY()); //����ѡ��ť��ɾ���ô���

		mpIterBtRedo = mpIterator->GetSubElementByID(ET_BT_REDO);
		BREAK_ON_NULL(mpIterBtRedo);
		mpIterBtRedo->SetEnable(false);
		//mpIterBtRedo->SetPosition(mpIterBtRedo->GetPositionX() - 100.0f, mpIterBtRedo->GetPositionY()); //����ѡ��ť��ɾ���ô���

		mpIterBtHand = mpIterator->GetSubElementByID(ET_BT_HAND);
		BREAK_ON_NULL(mpIterBtHand);
		//mpIterBtHand->SetPosition(mpIterBtHand->GetPositionX() - 100.0f, mpIterBtHand->GetPositionY()); //����ѡ��ť��ɾ���ô���
		
		mpIterLine2 = mpIterator->GetSubElementByID(1);
		BREAK_ON_NULL(mpIterLine2);
		//mpIterLine2->SetPosition(mpIterLine2->GetPositionX() - 100.0f, mpIterLine2->GetPositionY()); //����ѡ��ť��ɾ���ô���

		mpIterLine1 = mpIterator->GetSubElementByID(2);
		BREAK_ON_NULL(mpIterLine1);
		//mpIterLine1->SetPosition(mpIterLine1->GetPositionX() - 100.0f, mpIterLine1->GetPositionY()); //����ѡ��ť��ɾ���ô���

		//��ʱ����ѡ��ť
		mpIterGroup = mpIterator->GetSubElementByID(3);
		//mpIterGroup->GetSubElementByID(3)->SetVisible(false);


		leResult = ERESULT_SUCCESS;

	} while (false);

	CMM_SAFE_RELEASE(lpSubKey);

	// ��ϵͳע����Ҫ�յ�����Ϣ
	return leResult;
}


//��ť�����¼�
ERESULT CEditToolbar::OnCtlButtonClick(IEinkuiIterator* npSender)
{

	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		//CExMessage::PostMessage(EinkuiGetSystem()->GetCurrentWidget()->GetHomePage(), mpIterator, EEVT_RESET_HIDE_TIME, true);

		ULONG llBtnID = npSender->GetID();
		switch (llBtnID)
		{
		case ET_BT_UNDO:
		{
			CExMessage::SendMessage(EinkuiGetSystem()->GetCurrentWidget()->GetHomePage(), mpIterator, EEVT_UNDO,CExMessage::DataInvalid);
			mpIterBtRedo->SetEnable(true);

			break;
		}
		case ET_BT_REDO:
		{

			CExMessage::SendMessage(EinkuiGetSystem()->GetCurrentWidget()->GetHomePage(), mpIterator, EEVT_REDO, CExMessage::DataInvalid);
			mpIterBtUndo->SetEnable(true);

			break;
		}
		default:
			break;
		}


		lResult = ERESULT_SUCCESS;
	} while (false);

	return lResult;
}

//����redo,undo��ť״̬
void CEditToolbar::SetUndoRedoStatus(bool nbUndoEnable, bool nbRedoEnable)
{
	if(mpIterBtRedo->IsEnable() != nbRedoEnable)
		mpIterBtRedo->SetEnable(nbRedoEnable);

	if (mpIterBtUndo->IsEnable() != nbUndoEnable)
		mpIterBtUndo->SetEnable(nbUndoEnable);
}

// ���ó�ʼ������
void CEditToolbar::SetData(DWORD ndwPenWidthIndex, DWORD ndwPenColorIndex)
{
	mdwPenWidthIndex = ndwPenWidthIndex;
	mdwPenColorIndex = ndwPenColorIndex;
}

//���ð�ťѡ��״̬
void CEditToolbar::SetSelect(ULONG nulID)
{
	if (mulSelectID != nulID)
	{
		mulSelectID = nulID;
		CExMessage::PostMessage(mpIterGroup, mpIterator, EACT_RBG_SET_SELECT, mulSelectID);
	}
}

//��Ϣ������
ERESULT CEditToolbar::ParseMessage(IEinkuiMessage* npMsg)
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
	case EEVT_RBG_SELECTED_ITEM_CLICK:
	{
		//��ѡ�е�������ٴα����
		//CExMessage::PostMessage(EinkuiGetSystem()->GetCurrentWidget()->GetHomePage(), mpIterator, EEVT_RESET_HIDE_TIME, true);

		ULONG lulSelectID = 0;
		luResult = CExMessage::GetInputData(npMsg, lulSelectID);
		if (luResult != ERESULT_SUCCESS)
			break;

		if (lulSelectID == ET_BT_PEN)
		{
			//�����ʼ��˵�
			if (mpMenuPen != NULL)
			{
				mpMenuPen->ExitModal();
				mpMenuPen = NULL;
			}

			ICfKey* lpSubKey = mpTemplete->OpenKey(L"MenuPen");
			mpMenuPen = CMenuPen::CreateInstance(mpIterator, lpSubKey);
			mpMenuPen->SetData(mdwPenWidthIndex, mdwPenColorIndex);
			EI_SIZE ldPaintSize;
			EinkuiGetSystem()->GetPaintboardSize(&ldPaintSize);
			mpMenuPen->GetIterator()->SetSize(float(ldPaintSize.w), float(ldPaintSize.h));

			//�����˵�ʱ�͹ر�����
			CExMessage::SendMessage(EinkuiGetSystem()->GetCurrentWidget()->GetHomePage(), mpIterator, EEVT_PEN_MODE, PEN_MODE_NONE);
			mpMenuPen->DoModal();
			CExMessage::SendMessage(EinkuiGetSystem()->GetCurrentWidget()->GetHomePage(), mpIterator, EEVT_PEN_MODE, PEN_MODE_PEN);

			CMM_SAFE_RELEASE(lpSubKey);
			mpMenuPen = NULL;

			EinkuiGetSystem()->UpdateView(true);
		}
		break;
	}
	case EEVT_RBG_SELECTED_CHANGED:
	{
		//��ѡ��仯
		//CExMessage::PostMessage(EinkuiGetSystem()->GetCurrentWidget()->GetHomePage(), mpIterator, EEVT_RESET_HIDE_TIME, true);

		
		luResult = CExMessage::GetInputData(npMsg, mulSelectID);
		if (luResult != ERESULT_SUCCESS)
			break;

		//���ñ�״̬
		CExMessage::PostMessage(EinkuiGetSystem()->GetCurrentWidget()->GetHomePage(), mpIterator, EEVT_PEN_MODE, mulSelectID);

		break;
	}
	case EEVT_SET_PEN_WIDTH:
	{
		//���ñʿ�
		luResult = CExMessage::GetInputData(npMsg, mdwPenWidthIndex);
		if (luResult != ERESULT_SUCCESS)
			break;

		//PostMessageToParent(EEVT_SET_PEN_WIDTH, mdwPenWidthIndex);
		CExMessage::PostMessage(EinkuiGetSystem()->GetCurrentWidget()->GetHomePage(), mpIterator, EEVT_SET_PEN_WIDTH, mdwPenWidthIndex);
		break;
	}
	case EEVT_SET_PEN_COLOR:
	{
		//���ñ���ɫ
		luResult = CExMessage::GetInputData(npMsg, mdwPenColorIndex);
		if (luResult != ERESULT_SUCCESS)
			break;

		//PostMessageToParent(EEVT_SET_PEN_COLOR, mdwPenColorIndex);
		CExMessage::PostMessage(EinkuiGetSystem()->GetCurrentWidget()->GetHomePage(), mpIterator, EEVT_SET_PEN_COLOR, mdwPenColorIndex);
		break;
	}
	case EEVT_BUTTON_CHECKED:
	{
		if (npMsg->GetMessageSender()->GetID() == ET_BT_HAND) //�Ƿ��ֿɻ��߰�ť
		{
			mbIsHand = true;
			CExMessage::PostMessage(EinkuiGetSystem()->GetCurrentWidget()->GetHomePage(), mpIterator, EEVT_HAND_WRITE, mbIsHand);
		}
			
		break;
	}
	case EEVT_BUTTON_UNCHECK:
	{
		if (npMsg->GetMessageSender()->GetID() == ET_BT_HAND) //�Ƿ��ֿɻ��߰�ť
		{
			mbIsHand = false;
			CExMessage::PostMessage(EinkuiGetSystem()->GetCurrentWidget()->GetHomePage(), mpIterator, EEVT_HAND_WRITE, mbIsHand);
		}
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
void CEditToolbar::OnTimer(
	PSTEMS_TIMER npStatus
	)
{

}

//Ԫ�زο��ߴ緢���仯
ERESULT CEditToolbar::OnElementResized(D2D1_SIZE_F nNewSize)
{
	
	return ERESULT_SUCCESS;
}

//��ʼ����Ĭ�ϱ�״̬
void CEditToolbar::init(void)
{
	//Ĭ���Ǳ�дģʽ
	mulSelectID = PEN_MODE_PEN;
	CExMessage::PostMessage(EinkuiGetSystem()->GetCurrentWidget()->GetHomePage(), mpIterator, EEVT_PEN_MODE, mulSelectID);
	CExMessage::SendMessage(mpIterGroup, mpIterator, EACT_RBG_SET_SELECT, mulSelectID);
}

//֪ͨԪ�ء���ʾ/���ء������ı�
ERESULT CEditToolbar::OnElementShow(bool nbIsShow)
{
	//EiSetHomebarStatus(nbIsShow == false ? GI_HOMEBAR_SHOW : GI_HOMEBAR_HIDE);
	if (nbIsShow != false)
	{
		CExMessage::PostMessage(EinkuiGetSystem()->GetCurrentWidget()->GetHomePage(), mpIterator, EEVT_HAND_WRITE_MUTE, mbIsHand);
	}
	else
	{
		CExMessage::PostMessage(EinkuiGetSystem()->GetCurrentWidget()->GetHomePage(), mpIterator, EEVT_PEN_MODE, PEN_MODE_NONE);
		CExMessage::PostMessage(EinkuiGetSystem()->GetCurrentWidget()->GetHomePage(), mpIterator, EEVT_HAND_WRITE_MUTE, false);
	}

	return ERESULT_SUCCESS;
}

//����������
void CEditToolbar::ToolbarShow(bool nbIsShow)
{
	if (nbIsShow == false && mpMenuPen != NULL)
		mpMenuPen->ExitModal();
}