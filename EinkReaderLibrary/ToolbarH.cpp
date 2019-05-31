/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#include "StdAfx.h"
#include "ToolbarH.h"
#include "XCtl.h"
#include "stdio.h"
#include "cmmStrHandle.h"
#include "cmmPath.h"
#include "MsgDefine.h"

DEFINE_BUILTIN_NAME(ToolbarH)

CToolbarH::CToolbarH(void)
{
	mpIterBackground = NULL;
	mbIsTwoScreen = false;
	mbIsTxt = false;
}


CToolbarH::~CToolbarH(void)
{
}

//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
ERESULT CToolbarH::OnElementCreate(IEinkuiIterator* npIterator)
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

// ��������
ERESULT CToolbarH::OnMouseOwnerTest(const D2D1_POINT_2F& rPoint)
{
	ERESULT luResult = ERESULT_SUCCESS;

	if (!(rPoint.x < 0.0f || rPoint.x >= mpIterator->GetSizeX()
		|| rPoint.y < 0.0f || rPoint.y >= mpIterator->GetSizeY()))
	{
		luResult = ERESULT_MOUSE_OWNERSHIP;
	}

	return luResult;
}

ULONG CToolbarH::InitOnCreate(
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
		mpIterPage = mpIterator->GetSubElementByID(7);
		BREAK_ON_NULL(mpIterPage);
		
		mpIterFileName = mpIterator->GetSubElementByID(6);
		BREAK_ON_NULL(mpIterFileName);

		mpIterBtFileOpen = mpIterator->GetSubElementByID(TBH_BT_OPEN_FILE);
		BREAK_ON_NULL(mpIterBtFileOpen);

		mpIterBackground = mpIterator->GetSubElementByID(1);
		BREAK_ON_NULL(mpIterBackground);
		
		mpIterBtTwo = mpIterator->GetSubElementByID(TBH_BT_TWO);
		BREAK_ON_NULL(mpIterBtTwo);
		mpIterBtTwo->SetVisible(false);

		mpIterBtJump = mpIterator->GetSubElementByID(TBH_BT_JUMP);
		BREAK_ON_NULL(mpIterBtJump);

		mpIterBtSnap = mpIterator->GetSubElementByID(TBH_BT_SNAPSHOT);
		BREAK_ON_NULL(mpIterBtSnap);

		mpIterBtSuofang = mpIterator->GetSubElementByID(TBH_BT_ZOOM);
		BREAK_ON_NULL(mpIterBtSuofang);

		mpIterBtOne = mpIterator->GetSubElementByID(TBH_BT_ONE_PIC);
		BREAK_ON_NULL(mpIterBtOne);
		mpIterBtOne->SetVisible(false);

		leResult = ERESULT_SUCCESS;

	} while (false);

	CMM_SAFE_RELEASE(lpSubKey);

	// ��ϵͳע����Ҫ�յ�����Ϣ
	return leResult;
}

//��ť�����¼�
ERESULT CToolbarH::OnCtlButtonClick(IEinkuiIterator* npSender)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		ULONG llBtnID = npSender->GetID();
		switch (llBtnID)
		{
		case TBH_BT_OPEN_FILE:
		{
			//���ļ��Ի���
			PostMessageToParent(EEVT_ER_OPEN_FILE, CExMessage::DataInvalid);

			mpIterator->SetVisible(false);
			break;
		}
		case TBH_BT_JUMP:
		{
			//��ҳ��Ի���
			PostMessageToParent(EEVT_ER_OPEN_PAGE_JUMP, CExMessage::DataInvalid);

			mpIterator->SetVisible(false);
			break;
		}
		case TBH_BT_TWO:
		{
			//˫����ʾ
			PostMessageToParent(EEVT_ER_TWO_SCREEN, true);
			

			SetDuopageButton(true);
// 			mpIterBtTwo->SetVisible(false);
// 			mpIterBtOne->SetVisible(true);

			break;
		}
		case TBH_BT_ONE_PIC:
		{
			//������ʾ
			PostMessageToParent(EEVT_ER_TWO_SCREEN, false);

			SetDuopageButton(false);
			//mpIterBtTwo->SetVisible(true);
			//mpIterBtOne->SetVisible(false);

			break;
		}
		case TBH_BT_ZOOM:
		{
			//����
			PostMessageToParent(EEVT_ER_ENTER_ZOOM, true);

			mpIterator->SetVisible(false);

			break;
		}
		case TBH_BT_SNAPSHOT:
		{
			//����
			PostMessageToParent(EEVT_ER_ENTER_SNAPSHOT, CExMessage::DataInvalid);

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
ERESULT CToolbarH::ParseMessage(IEinkuiMessage* npMsg)
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
void CToolbarH::OnTimer(
	PSTEMS_TIMER npStatus
	)
{

}

//Ԫ�زο��ߴ緢���仯
ERESULT CToolbarH::OnElementResized(D2D1_SIZE_F nNewSize)
{
	//CExMessage::SendMessage(mpIterBtFull, mpIterator, EACT_BUTTON_SET_ACTION_RECT, nNewSize);
	////mpIterLineOne->SetSize(nNewSize.width, mpIterLineOne->GetSize().height);

	//mpIterBtOk->SetPosition(nNewSize.width - 85, mpIterBtOk->GetPositionY());
	if (mpIterBackground != NULL)
	{
		EI_SIZE ldPaintSize;
		D2D1_SIZE_F ldActionSize;

		EinkuiGetSystem()->GetPaintboardSize(&ldPaintSize);
		if (ldPaintSize.w > ldPaintSize.h)
		{
			//����
			CExMessage::SendMessageWithText(mpIterBackground, mpIterator, EACT_PICTUREFRAME_CHANGE_PIC, L".\\Pic\\toolbar_h.png");
			mpIterBackground->SetSize(1764.0f, 52.0f);
			D2D1_POINT_2F ldPos;
			ldPos.y = 0.0f;

			//����
			ldPos.x = 1664.0f;
			mpIterBtSuofang->SetPosition(ldPos);

			ldPos.x = 1564.0f;
			mpIterBtSnap->SetPosition(ldPos);

			ldPos.x = 1464.0f;
			mpIterBtJump->SetPosition(ldPos);

			ldPos.x = 1364.0f;
			mpIterBtTwo->SetPosition(ldPos);
			if(mbIsTxt == false && mbIsTwoScreen == false)
				mpIterBtTwo->SetVisible(true);

			mpIterBtOne->SetPosition(ldPos);
			if (mbIsTxt == false && mbIsTwoScreen != false)
				mpIterBtOne->SetVisible(true);
			
			CExMessage::SendMessage(mpIterFileName, mpIterator, EACT_LABEL_SET_MAX_WIDTH, 900);

			ldActionSize.width = 1360.0f; 
			ldActionSize.height = 52.0f;
			CExMessage::SendMessage(mpIterBtFileOpen, mpIterator, EACT_BUTTON_SET_ACTION_RECT, ldActionSize);
			
		}
		else
		{
			//����
			CExMessage::SendMessageWithText(mpIterBackground, mpIterator, EACT_PICTUREFRAME_CHANGE_PIC, L".\\Pic\\toolbar_v.png");
			mpIterBackground->SetSize(924.0f, 52.0f);
			//����
			D2D1_POINT_2F ldPos;
			ldPos.y = 0.0f;
			ldPos.x = 824.0f;
			mpIterBtSuofang->SetPosition(ldPos);

			ldPos.x = 724.0f;
			mpIterBtSnap->SetPosition(ldPos);

			ldPos.x = 624.0f;
			mpIterBtJump->SetPosition(ldPos);

			mpIterBtTwo->SetVisible(false);
			mpIterBtOne->SetVisible(false);

			CExMessage::SendMessage(mpIterFileName, mpIterator, EACT_LABEL_SET_MAX_WIDTH, 300);

			ldActionSize.width = 620.0f;
			ldActionSize.height = 52.0f;
			CExMessage::SendMessage(mpIterBtFileOpen, mpIterator, EACT_BUTTON_SET_ACTION_RECT, ldActionSize);
		}

		mpIterPage->SetPosition(mpIterFileName->GetPositionX() + mpIterFileName->GetSizeX() + 20.0f, mpIterPage->GetPositionY());
	}

	return ERESULT_SUCCESS;
}


//�����ļ�����
void CToolbarH::SetFileName(wchar_t* npszString)
{
	do 
	{
		BREAK_ON_NULL(npszString);

		if (npszString[wcslen(npszString) - 1] == 't')
		{
			//�����txt�ļ����޸�Ϊ��Ӧ�����Ű�ťͼ��
			CExMessage::SendMessageWithText(mpIterBtSuofang, mpIterator, EACT_BUTTON_CHANGE_PIC, L".\\Pic\\suofang_txt.png");

			//txtֻ֧�ֵ���
			SetDuopageButton(false);
			PostMessageToParent(EEVT_ER_TWO_SCREEN, false);
			mpIterBtTwo->SetVisible(false);
			mpIterBtOne->SetVisible(false);

			mbIsTxt = true;
		}
		else
		{
			CExMessage::SendMessageWithText(mpIterBtSuofang, mpIterator, EACT_BUTTON_CHANGE_PIC, L".\\Pic\\suofang.png");

			SetDuopageButton(mbIsTwoScreen);
			/*mpIterBtTwo->SetVisible(true);
			mpIterBtOne->SetVisible(true);*/

			mbIsTxt = false;
		}

		CExMessage::SendMessageWithText(mpIterFileName, mpIterator, EACT_LABEL_SET_TEXT, npszString);

		mpIterPage->SetPosition(mpIterFileName->GetPositionX() + mpIterFileName->GetSizeX() + 20.0f, mpIterPage->GetPositionY());

	} while (false);
}

//����ҳ���ַ���
void CToolbarH::SetPage(wchar_t* npszString)
{
	do
	{
		BREAK_ON_NULL(npszString);
		CExMessage::PostMessageWithText(mpIterPage, mpIterator, EACT_LABEL_SET_TEXT, npszString);

	} while (false);
}

//��ȡ��ǰ˫ҳ��ʾ״̬
bool CToolbarH::GetDuopageStatus(void)
{
	return mbIsTwoScreen;
}

void CToolbarH::SetDuopageButton(bool nbSingle)
{
	do 
	{
		mbIsTwoScreen = nbSingle;

		EI_SIZE ldPaintSize;
		EinkuiGetSystem()->GetPaintboardSize(&ldPaintSize);
		if (ldPaintSize.w < ldPaintSize.h)
			break; //�Ǽ���ģʽ

		if (nbSingle != false)
		{
			mpIterBtTwo->SetVisible(false);
			mpIterBtOne->SetVisible(true);
		}
		else
		{
			mpIterBtTwo->SetVisible(true);
			mpIterBtOne->SetVisible(false);
		}

	} while (false);
	
}

//֪ͨԪ�ء���ʾ/���ء������ı�
ERESULT CToolbarH::OnElementShow(bool nbIsShow)
{
	//EiSetHomebarStatus(nbIsShow == false ? GI_HOMEBAR_SHOW : GI_HOMEBAR_HIDE);

	return ERESULT_SUCCESS;
}