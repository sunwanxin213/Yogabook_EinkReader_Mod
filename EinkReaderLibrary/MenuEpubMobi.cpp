/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#include "StdAfx.h"
#include "MenuEpubMobi.h"
#include "XCtl.h"
#include "stdio.h"
#include "cmmStrHandle.h"
#include "cmmPath.h"
#include "MsgDefine.h"
#include "PreNextButton.h"
#include "..\ECtl\EvButtonImp.h"

DEFINE_BUILTIN_NAME(MenuEpubMobi)

CMenuEpubMobi::CMenuEpubMobi(void)
{
	mpIterMenuBase = NULL;
	mpIterPageGroup = NULL;
	mbIsPressed = false;
}


CMenuEpubMobi::~CMenuEpubMobi(void)
{
}

//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
ERESULT CMenuEpubMobi::OnElementCreate(IEinkuiIterator* npIterator)
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


ULONG CMenuEpubMobi::InitOnCreate(
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

		mpIterPageGroup = mpIterMenuBase->GetSubElementByID(4);
		BREAK_ON_NULL(mpIterPageGroup);

		leResult = ERESULT_SUCCESS;

	} while (false);

	CMM_SAFE_RELEASE(lpSubKey);

	// ��ϵͳע����Ҫ�յ�����Ϣ
	return leResult;
}

// ��������
ERESULT CMenuEpubMobi::OnMouseOwnerTest(const D2D1_POINT_2F& rPoint)
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


//����ƶ�
ERESULT CMenuEpubMobi::OnMouseMoving(const STEMS_MOUSE_MOVING* npInfo)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		BREAK_ON_NULL(npInfo);

		if (MOUSE_LB(npInfo->KeyFlag) == false)  //��������������Ͳ�����
			break;

		//if (mbIsBeginMove != false)
		//{
		//	//��ʼ������¼����ʼ����
		//	mdDropBeginPos = npInfo->Position;
		//	mbIsBeginMove = false;
		//	break;
		//}

		//mpIterator->KillTimer(ZC_TIMER_ID_HIDE);
		//mpIterator->SetTimer(ZC_TIMER_ID_HIDE, 1, 1000 * 5, NULL);

		lResult = ERESULT_SUCCESS;

	} while (false);

	return lResult;
}

//��갴��
ERESULT CMenuEpubMobi::OnMousePressed(const STEMS_MOUSE_BUTTON* npInfo)
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
ERESULT CMenuEpubMobi::OnCtlButtonClick(IEinkuiIterator* npSender)
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


void CMenuEpubMobi::DoModal()
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

//���õ�˫ҳ
void CMenuEpubMobi::SetPageTwo(bool nbIsDouble, bool nbIsEnable)
{
	CExMessage::SendMessage(mpIterPageGroup, mpIterator, EACT_RBG_SET_SELECT, nbIsDouble==false?1:2);

	mpIterPageGroup->GetSubElementByID(2)->SetEnable(nbIsEnable);
}

//������Ļ����
void CMenuEpubMobi::SetScreenOritent(ULONG nulIndex)
{
	CExMessage::SendMessage(mpIterMenuBase->GetSubElementByID(6), mpIterator, EACT_RBG_SET_SELECT, nulIndex);
}

void CMenuEpubMobi::ExitModal()
{
	EinkuiGetSystem()->ExitModal(mpIterator, 0);
}


//��Ϣ������
ERESULT CMenuEpubMobi::ParseMessage(IEinkuiMessage* npMsg)
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
	case EEVT_MENU_ITEM_CLICKED:
	{
		ULONG lulID = 0;
		luResult = CExMessage::GetInputData(npMsg, lulID);
		if (luResult != ERESULT_SUCCESS)
			break;

		if (lulID == ME_BT_SHAPSHOT)
		{
			//����
			PostMessageToParent(EEVT_ER_ENTER_SNAPSHOT, CExMessage::DataInvalid);
		}

		ExitModal();

		break;
	}
	case EEVT_RBG_SELECTED_CHANGED:
	{
		//��˫ҳ��ѡ��仯
		ULONG lulSelectID = 0;
		luResult = CExMessage::GetInputData(npMsg, lulSelectID);
		if (luResult != ERESULT_SUCCESS)
			break;

		if (lulSelectID < 10)
		{
			bool lbIsDouble = false;
			if (lulSelectID == 1)
				lbIsDouble = false;
			else
				lbIsDouble = true;

			PostMessageToParent(EEVT_ER_TWO_SCREEN, lbIsDouble);
		}
		else
		{
			//ҳ�淽��

			CExMessage::SendMessage(mpIterator->GetParent(), mpIterator, EEVT_SET_SCREEN_STATUS, lulSelectID);
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
void CMenuEpubMobi::OnTimer(
	PSTEMS_TIMER npStatus
	)
{

}

//Ԫ�زο��ߴ緢���仯
ERESULT CMenuEpubMobi::OnElementResized(D2D1_SIZE_F nNewSize)
{

	if (mpIterMenuBase != NULL)
	{
		mpIterMenuBase->SetPosition(nNewSize.width - mpIterMenuBase->GetSizeX() - 120.0f, mpIterMenuBase->GetPositionY());
		
	}

	return ERESULT_SUCCESS;
}


//֪ͨԪ�ء���ʾ/���ء������ı�
ERESULT CMenuEpubMobi::OnElementShow(bool nbIsShow)
{
	//EiSetHomebarStatus(nbIsShow == false ? GI_HOMEBAR_SHOW : GI_HOMEBAR_HIDE);

	return ERESULT_SUCCESS;
}
