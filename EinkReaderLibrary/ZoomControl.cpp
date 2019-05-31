/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#include "StdAfx.h"
#include "ZoomControl.h"
#include "XCtl.h"
#include "stdio.h"
#include "cmmStrHandle.h"
#include "cmmPath.h"
#include "MsgDefine.h"
#include "..\ECtl\EvButtonImp.h"

// �������У���PreNextButton.h�ļ��н���
#define PNB_BT_MIDDLE 100  //�м�
#define PNB_BT_PRE 101	//��һҳ
#define PNB_BT_NEXT 102 //��һҳ

DEFINE_BUILTIN_NAME(ZoomControl)

CZoomControl::CZoomControl(void)
{
	mpIterBtUP = NULL;
	mbIsBeginMove = false;
}


CZoomControl::~CZoomControl(void)
{
}

//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
ERESULT CZoomControl::OnElementCreate(IEinkuiIterator* npIterator)
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

ULONG CZoomControl::InitOnCreate(
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
		
		mpIterScrH = mpIterator->GetSubElementByID(105);
		BREAK_ON_NULL(mpIterScrH);
		mpIterScrH->GetSubElementByID(2)->SetEnable(false);

		mpIterScrV = mpIterator->GetSubElementByID(106);
		BREAK_ON_NULL(mpIterScrV);
		mpIterScrV->GetSubElementByID(2)->SetEnable(false);

		CExMessage::SendMessage(mpIterScrH, mpIterator, EACT_SCROLLBAR_HVSCROLL_RELACATION, CExMessage::DataInvalid);
		CExMessage::SendMessage(mpIterScrV, mpIterator, EACT_SCROLLBAR_HVSCROLL_RELACATION, CExMessage::DataInvalid);

		//������
		lpSubKey = mpTemplete->OpenKey(L"ZoomControlToolbar");
		mpZoomControlToolbar = CZoomControlToolbar::CreateInstance(mpIterator, lpSubKey);
		CMM_SAFE_RELEASE(lpSubKey);
		BREAK_ON_NULL(mpZoomControlToolbar);

		leResult = ERESULT_SUCCESS;

	} while (false);

	CMM_SAFE_RELEASE(lpSubKey);

	// ��ϵͳע����Ҫ�յ�����Ϣ
	return leResult;
}

//����ScrollBar��λ��
bool CZoomControl::SetScrollBarPositionAndSize()
{
	{
		//����ScrollBarλ��
		if (mpIterScrH)
		{
			mpIterScrH->SetPosition((mpIterator->GetSizeX()- mpIterScrH->GetSizeX())/2.0f, mpIterator->GetSizeY() - 20.0f);
			mpIterScrV->SetPosition(mpIterator->GetSizeX() - 20.0f, (mpIterator->GetSizeY() - mpIterScrH->GetSizeY()) / 2.0f);

			CExMessage::SendMessage(mpIterScrH, mpIterator, EACT_SCROLLBAR_HVSCROLL_RELACATION, CExMessage::DataInvalid);
			CExMessage::SendMessage(mpIterScrV, mpIterator, EACT_SCROLLBAR_HVSCROLL_RELACATION, CExMessage::DataInvalid);
		}

	}
	return true;
}

void CZoomControl::SetFatRatio(float fatRatio)
{
	mpZoomControlToolbar->SetFatRatio(fatRatio);
}

//��ť�����¼�
ERESULT CZoomControl::OnCtlButtonClick(IEinkuiIterator* npSender)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
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
			mpIterator->KillTimer(ZC_TIMER_ID_HIDE);
			mpIterator->SetTimer(ZC_TIMER_ID_HIDE, 1, 1000 * 5, NULL);

			break;
		}
		default:
			break;
		}


		lResult = ERESULT_SUCCESS;
	} while (false);

	return lResult;
}

//���������ƶ�
void CZoomControl::MovePage(ULONG nulID)
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

	CExMessage::SendMessage(mpIterator->GetParent(), mpIterator, EEVT_ER_SET_PAGE_MOVE, ldPos, &ldRect, sizeof(RECT));
	ShowMoveButton(ldRect);
}

//��Ϣ������
ERESULT CZoomControl::ParseMessage(IEinkuiMessage* npMsg)
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
	case EEVT_ER_ENTER_ZOOM:
	{
		//�˳����ţ��ָ���100%
		RECT ldRect;
		float lfRatio = 1.0f;
		CExMessage::SendMessage(mpIterator->GetParent(), mpIterator, EEVT_ER_SET_ZOOM, lfRatio, &ldRect, sizeof(RECT));
		ShowMoveButton(ldRect);

		PostMessageToParent(EEVT_ER_ENTER_ZOOM, false);

		break;
	}
	case EEVT_ER_SET_ZOOM:
	{
		//���÷Ŵ���
		float lfRatio = 1.0f;
		luResult = CExMessage::GetInputData(npMsg, lfRatio);
		if (luResult != ERESULT_SUCCESS)
			break;

		RECT ldRect;
		CExMessage::SendMessage(mpIterator->GetParent(), mpIterator, EEVT_ER_SET_ZOOM, lfRatio,&ldRect, sizeof(RECT));
		ShowMoveButton(ldRect);

		mpIterator->KillTimer(ZC_TIMER_ID_HIDE);
		mpIterator->SetTimer(ZC_TIMER_ID_HIDE, 1, 1000 * 5, NULL);

		break;
	}
	case EEVT_ER_ENTER_SNAPSHOT:
	{
		//��Ҫ����
		//�����Լ��Ŀؼ�
		ShowItem(false);
		
		SendMessageToParent(EEVT_ER_ENTER_SNAPSHOT, CExMessage::DataInvalid, NULL, 0);


		//��ʾ�Լ��Ŀؼ�
		//ShowItem(true);

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
void CZoomControl::OnTimer(
	PSTEMS_TIMER npStatus
	)
{
	if (npStatus->TimerID == ZC_TIMER_ID_HIDE)
	{
		mpIterator->KillTimer(ZC_TIMER_ID_HIDE);
		ShowItem(false);
	}
}

//Ԫ�زο��ߴ緢���仯
ERESULT CZoomControl::OnElementResized(D2D1_SIZE_F nNewSize)
{
	if (mpIterBtUP != NULL)
	{
		D2D1_SIZE_F ldSize = mpIterator->GetSize();
		
		D2D1_POINT_2F ldPos;

		//��λ
		ldPos.x = 00.0f;
		ldPos.y = (ldSize.height - mpIterBtLeft->GetSizeY()) / 2.0f;
		mpIterBtLeft->SetPosition(ldPos);

		ldPos.x = ldSize.width - mpIterBtRight->GetSizeX();
		mpIterBtRight->SetPosition(ldPos);

		ldPos.y = 00.0f;
		ldPos.x = (ldSize.width - mpIterBtLeft->GetSizeX()) / 2.0f;
		mpIterBtUP->SetPosition(ldPos);

		ldPos.y = ldSize.height - mpIterBtRight->GetSizeY() - 170.0f;
		mpIterBtDown->SetPosition(ldPos);

		ldPos.x = (ldSize.width - mpZoomControlToolbar->GetIterator()->GetSizeX()) / 2.0f;
		ldPos.y = ldSize.height - mpZoomControlToolbar->GetIterator()->GetSizeY() - 80.0f;
		mpZoomControlToolbar->GetIterator()->SetPosition(ldPos);

		if (mpIterScrH)
		{
			mpIterScrH->SetPosition((mpIterator->GetSizeX() - mpIterScrH->GetSizeX()) / 2.0f, mpIterator->GetSizeY() - 20.0f);
			mpIterScrV->SetPosition(mpIterator->GetSizeX() - 20.0f, (mpIterator->GetSizeY() - mpIterScrV->GetSizeY()) / 2.0f);
		}
	}

	return ERESULT_SUCCESS;
}

//֪ͨԪ�ء���ʾ/���ء������ı�
ERESULT CZoomControl::OnElementShow(bool nbIsShow)
{
	//EiSetHomebarStatus(nbIsShow == false ? GI_HOMEBAR_SHOW : GI_HOMEBAR_HIDE);
	if (nbIsShow != false)
	{
		mpIterator->KillTimer(ZC_TIMER_ID_HIDE);
		mpIterator->SetTimer(ZC_TIMER_ID_HIDE, 1, 1000 * 5, NULL);
	}
	return ERESULT_SUCCESS;
}


// ��������
ERESULT CZoomControl::OnMouseOwnerTest(const D2D1_POINT_2F& rPoint)
{
	ERESULT luResult = ERESULT_SUCCESS;

	if (!(rPoint.x < 0.0f || rPoint.x >= mpIterator->GetSizeX()
		|| rPoint.y < 0.0f || rPoint.y >= mpIterator->GetSizeY()))
	{
		luResult = ERESULT_MOUSE_OWNERSHIP;
	}

	return luResult;
}

//��갴��
ERESULT CZoomControl::OnMousePressed(const STEMS_MOUSE_BUTTON* npInfo)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		BREAK_ON_NULL(npInfo);
		if (mpIterator->IsEnable() == false)
			break;	//����ǽ���״̬���Ͳ���������

		if (MOUSE_LB(npInfo->ActKey) == false)  //��������������Ͳ�����
			break;

		if (npInfo->Presssed == false)
		{
			//���̧��
			POINT ldPos;
			ldPos.x = LONG(npInfo->Position.x - mdPressPos.x);
			ldPos.y = LONG(npInfo->Position.y - mdPressPos.y);

			mbIsBeginMove = false;
			if (abs(mdPressPos.x - npInfo->Position.x) >= 20.0f || abs(mdPressPos.y - npInfo->Position.y) >= 20.0f)
			{
				//�ǻ�������
	
				do 
				{
					// �ж��Ƿ������ҳ
					if (abs(mdPressPos.x - npInfo->Position.x) >= abs(mdPressPos.y - npInfo->Position.y))	// Ҫ����򻬶�
					{
						if ((mdPressPos.x - npInfo->Position.x) >= 50.0f && mpIterBtRight->IsVisible()==false)
						{
							//��һҳ
							//PostMessageToParent(EEVT_ER_PRE_NEXT_CLICKED, PNB_BT_NEXT);
							CExMessage::SendMessage(mpIterator->GetParent(), mpIterator, EEVT_ER_PRE_NEXT_CLICKED, PNB_BT_NEXT);
							ldPos.x = 100;
							//break;
						}
						else if ((npInfo->Position.x - mdPressPos.x) >= 50.0f && mpIterBtLeft->IsVisible()==false)
						{
							//ǰһҳ
							//PostMessageToParent(EEVT_ER_PRE_NEXT_CLICKED, PNB_BT_PRE);
							CExMessage::SendMessage(mpIterator->GetParent(), mpIterator, EEVT_ER_PRE_NEXT_CLICKED, PNB_BT_PRE);
							ldPos.x = 100;
							//break;
						}
					}

					RECT ldRect;
					CExMessage::SendMessage(mpIterator->GetParent(), mpIterator, EEVT_ER_SET_PAGE_MOVE, ldPos, &ldRect, sizeof(RECT));
					ShowMoveButton(ldRect);

				} while (false);

			}
			else
			{
				//��Ϊ�ǵ������
				//��ʾ�������Լ��İ�ť
				ShowItem(!mpZoomControlToolbar->GetIterator()->IsVisible());
			}
		}
		else
		{
			//��갴��
			mbIsBeginMove = true;
			mdPressPos = npInfo->Position;
		}

		lResult = ERESULT_SUCCESS;

	} while (false);

	return lResult;
}


//��ʾ���������пؼ�
void CZoomControl::ShowItem(bool nbIsShow)
{
	

	if (nbIsShow == false)
	{
		mpIterBtUP->SetVisible(nbIsShow);
		mpIterBtDown->SetVisible(nbIsShow);
		mpIterBtLeft->SetVisible(nbIsShow);
		mpIterBtRight->SetVisible(nbIsShow);
		mpZoomControlToolbar->GetIterator()->SetVisible(nbIsShow);

		mpIterScrH->SetVisible(nbIsShow);
		mpIterScrV->SetVisible(nbIsShow);
	}
	else
	{
		ShowMoveButton(mdMoveBtShow);

		mpIterator->KillTimer(ZC_TIMER_ID_HIDE);
		mpIterator->SetTimer(ZC_TIMER_ID_HIDE, 1, 1000 * 5, NULL);
	}
}

//����ƶ�
ERESULT CZoomControl::OnMouseMoving(const STEMS_MOUSE_MOVING* npInfo)
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

		mpIterator->KillTimer(ZC_TIMER_ID_HIDE);
		mpIterator->SetTimer(ZC_TIMER_ID_HIDE, 1, 1000 * 5, NULL);

		lResult = ERESULT_SUCCESS;

	} while (false);

	return lResult;
}

//����4���ƶ���ť��״̬
void CZoomControl::ShowMoveButton(RECT ldRect)
{
	mdMoveBtShow = ldRect;

	mpIterBtUP->SetVisible(ldRect.top == 0?false:true);
	mpIterBtDown->SetVisible(ldRect.bottom == 0 ? false : true);
	mpIterBtLeft->SetVisible(ldRect.left == 0 ? false : true);
	mpIterBtRight->SetVisible(ldRect.right == 0 ? false : true);

	mpZoomControlToolbar->GetIterator()->SetVisible(true);
	mpIterScrH->SetVisible(true);
	mpIterScrV->SetVisible(true);
}

//���÷Ŵ���ͼ��С����ʾ����
void CZoomControl::SetRectOfViewportOnPage(D2D1_SIZE_F& nrImageSize, D2D1_RECT_F& nrViewPort)
{
	if (mpIterScrV != NULL)
	{
		float lfLen = nrImageSize.height - mpIterator->GetSizeY();
		if (lfLen <= 0.0f)
			lfLen = 0.0f;
		CExMessage::SendMessage(mpIterScrV, mpIterator, EACT_SCROLLBAR_HVSCROLL_SET_DELTA_SIZE, lfLen);
		CExMessage::SendMessage(mpIterScrV, mpIterator, EACT_SCROLLBAR_VSCROLL_SET_SLIDER_POSTION, nrViewPort.top);

		lfLen = nrImageSize.width - mpIterator->GetSizeX();
		if (lfLen <= 0.0f)
			lfLen = 0.0f;
		CExMessage::SendMessage(mpIterScrH, mpIterator, EACT_SCROLLBAR_HVSCROLL_SET_DELTA_SIZE, lfLen);
		CExMessage::SendMessage(mpIterScrH, mpIterator, EACT_SCROLLBAR_HSCROLL_SET_SLIDER_POSTION, nrViewPort.left);
	}
}