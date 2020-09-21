/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#include "StdAfx.h"
#include "MenuPen.h"
#include "XCtl.h"
#include "stdio.h"
#include "cmmStrHandle.h"
#include "cmmPath.h"
#include "MsgDefine.h"
#include "PreNextButton.h"
#include "..\ECtl\EvButtonImp.h"

DEFINE_BUILTIN_NAME(MenuPen)

CMenuPen::CMenuPen(void)
{
	mpIterMenuBase = NULL;
	mpIterPenWidthGroup = NULL;
	mpIterPenColorGroup = NULL;
	mbIsPressed = false;
}


CMenuPen::~CMenuPen(void)
{
}

//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
ERESULT CMenuPen::OnElementCreate(IEinkuiIterator* npIterator)
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


ULONG CMenuPen::InitOnCreate(
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
		leResult = CXuiElement::InitOnCreate(npParent, npTemplete, nuEID);
		if (leResult != ERESULT_SUCCESS)
			break;


		//��ȡ������
		mpIterMenuBase = mpIterator->GetSubElementByID(1);
		BREAK_ON_NULL(mpIterMenuBase);

		mpIterPenWidthGroup = mpIterMenuBase->GetSubElementByID(4);
		BREAK_ON_NULL(mpIterPenWidthGroup);

		mpIterPenColorGroup = mpIterMenuBase->GetSubElementByID(5);
		BREAK_ON_NULL(mpIterPenColorGroup);

		leResult = ERESULT_SUCCESS;

	} while (false);

	CMM_SAFE_RELEASE(lpSubKey);

	// ��ϵͳע����Ҫ�յ�����Ϣ
	return leResult;
}

// ���ó�ʼ������
void CMenuPen::SetData(DWORD ndwPenWidthIndex, DWORD ndwPenColorIndex)
{
	CExMessage::SendMessage(mpIterPenWidthGroup, mpIterator, EACT_RBG_SET_SELECT, ndwPenWidthIndex + 1);
	CExMessage::SendMessage(mpIterPenColorGroup, mpIterator, EACT_RBG_SET_SELECT, ndwPenColorIndex + 100);

}


// ��������
ERESULT CMenuPen::OnMouseOwnerTest(const D2D1_POINT_2F& rPoint)
{
	ERESULT luResult = ERESULT_SUCCESS;

	if (!(rPoint.x < 0.0f || rPoint.x >= mpIterator->GetSizeX()
		|| rPoint.y < 0.0f || rPoint.y >= mpIterator->GetSizeY()))
	{
		luResult = ERESULT_MOUSE_OWNERSHIP;
	}
	else if (rPoint.x < 0.0f || rPoint.y < 0.0f)
	{
		//˵��������
		ExitModal();
	}
	else
	{
		luResult = ERESULT_SUCCESS;
	}

	return luResult;
}

//��갴��
ERESULT CMenuPen::OnMousePressed(const STEMS_MOUSE_BUTTON* npInfo)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		BREAK_ON_NULL(npInfo);
		if (mpIterator->IsEnable() == false)
			break;	//����ǽ���״̬���Ͳ���������

		if (MOUSE_LB(npInfo->ActKey) == false)  //��������������Ͳ�����
			break;

		if (npInfo->Presssed == false && mbIsPressed != false)
		{
			//���̧��

			ExitModal();

		}
		else if (npInfo->Presssed != false)
		{
			//��갴��
			mbIsPressed = true;
		}

		lResult = ERESULT_SUCCESS;

	} while (false);

	return lResult;
}

//��ť�����¼�
ERESULT CMenuPen::OnCtlButtonClick(IEinkuiIterator* npSender)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		//ULONG llBtnID = npSender->GetID();
		//switch (llBtnID)
		//{
		//case MT_BT_SHAPSHOT:
		//{
		//	//����
		//	PostMessageToParent(EEVT_ER_ENTER_SNAPSHOT,CExMessage::DataInvalid);
		//	
		//	//mpIterator->SetVisible(false);
		//	break;
		//}
		//default:
		//	break;
		//}

		

		lResult = ERESULT_SUCCESS;
	} while (false);

	return lResult;
}


void CMenuPen::DoModal()
{
	do
	{
		mpIterator->SetActive();
		mpIterator->BringToTop();
		EinkuiGetSystem()->EnablePaintboard(false);
		EinkuiGetSystem()->UpdateView(true);
		EinkuiGetSystem()->DoModal(mpIterator);


		mpIterator->Close();

	} while (false);
}

void CMenuPen::ExitModal()
{
	EinkuiGetSystem()->ExitModal(mpIterator, 0);
}


//��Ϣ������
ERESULT CMenuPen::ParseMessage(IEinkuiMessage* npMsg)
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
		//��ѡ��仯
		ULONG lulSelectID = 0;
		luResult = CExMessage::GetInputData(npMsg, lulSelectID);
		if (luResult != ERESULT_SUCCESS)
			break;
		ULONG ll = npMsg->GetMessageSender()->GetID();
		ULONG li = mpIterPenWidthGroup->GetID();
		DWORD ldwIndex = (DWORD)lulSelectID;
		if (ldwIndex < MP_CBT_RED)
		{
			//�����߿�
			PostMessageToParent(EEVT_SET_PEN_WIDTH, ldwIndex - 1);
		}
		else
		{
			//������ɫ
			PostMessageToParent(EEVT_SET_PEN_COLOR, ldwIndex - 100);
		}

		ExitModal();

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
void CMenuPen::OnTimer(
	PSTEMS_TIMER npStatus
	)
{

}

//Ԫ�زο��ߴ緢���仯
ERESULT CMenuPen::OnElementResized(D2D1_SIZE_F nNewSize)
{

	if (mpIterMenuBase != NULL)
	{
		//mpIterMenuBase->SetPosition(mpIterator->GetParent()->GetPositionX(), mpIterMenuBase->GetPositionY());
	}

	return ERESULT_SUCCESS;
}


//֪ͨԪ�ء���ʾ/���ء������ı�
ERESULT CMenuPen::OnElementShow(bool nbIsShow)
{
	//EiSetHomebarStatus(nbIsShow == false ? GI_HOMEBAR_SHOW : GI_HOMEBAR_HIDE);
	
	return ERESULT_SUCCESS;
}
