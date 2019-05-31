/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#include "StdAfx.h"
#include "ZoomControlTxt.h"
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

DEFINE_BUILTIN_NAME(ZoomControlTxt)

CZoomControlTxt::CZoomControlTxt(void)
{
	mbIsBeginMove = false;
}


CZoomControlTxt::~CZoomControlTxt(void)
{
}

//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
ERESULT CZoomControlTxt::OnElementCreate(IEinkuiIterator* npIterator)
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

ULONG CZoomControlTxt::InitOnCreate(
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
		//������
		lpSubKey = mpTemplete->OpenKey(L"ZoomControlToolbar");
		mpZoomControlTxtToolbar = CZoomControlToolbarTxt::CreateInstance(mpIterator, lpSubKey);
		CMM_SAFE_RELEASE(lpSubKey);
		BREAK_ON_NULL(mpZoomControlTxtToolbar);

		/*mpIterScrH = mpIterator->GetSubElementByID(105);
		BREAK_ON_NULL(mpIterScrH);
		mpIterScrH->GetSubElementByID(2)->SetEnable(false);

		mpIterScrV = mpIterator->GetSubElementByID(106);
		BREAK_ON_NULL(mpIterScrV);
		mpIterScrV->GetSubElementByID(2)->SetEnable(false);*/

		//CExMessage::SendMessage(mpIterScrH, mpIterator, EACT_SCROLLBAR_HVSCROLL_RELACATION, CExMessage::DataInvalid);
		//CExMessage::SendMessage(mpIterScrV, mpIterator, EACT_SCROLLBAR_HVSCROLL_RELACATION, CExMessage::DataInvalid);

		

		leResult = ERESULT_SUCCESS;

	} while (false);

	CMM_SAFE_RELEASE(lpSubKey);

	// ��ϵͳע����Ҫ�յ�����Ϣ
	return leResult;
}

//����ScrollBar��λ��
bool CZoomControlTxt::SetScrollBarPositionAndSize()
{
	//{
	//	//����ScrollBarλ��
	//	if (mpIterScrH)
	//	{
	//		mpIterScrH->SetPosition((mpIterator->GetSizeX() - mpIterScrH->GetSizeX()) / 2.0f, mpIterator->GetSizeY() - 20.0f);
	//		mpIterScrV->SetPosition(mpIterator->GetSizeX() - 20.0f, (mpIterator->GetSizeY() - mpIterScrH->GetSizeY()) / 2.0f);

	//		CExMessage::SendMessage(mpIterScrH, mpIterator, EACT_SCROLLBAR_HVSCROLL_RELACATION, CExMessage::DataInvalid);
	//		CExMessage::SendMessage(mpIterScrV, mpIterator, EACT_SCROLLBAR_HVSCROLL_RELACATION, CExMessage::DataInvalid);
	//	}

	//}
	return true;
}

void CZoomControlTxt::SetFontsize(DWORD ldwFontSizeIndex)
{
	mpZoomControlTxtToolbar->SetFontsize(ldwFontSizeIndex);
}

//��ť�����¼�
ERESULT CZoomControlTxt::OnCtlButtonClick(IEinkuiIterator* npSender)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		ULONG llBtnID = npSender->GetID();
		//switch (llBtnID)
		//{
		//case ZC_BT_UP:
		//case ZC_BT_DOWN:
		//case ZC_BT_LEFT:
		//case ZC_BT_RIGHT:
		//{
		//	//���������ƶ�
		//	MovePage(llBtnID);
		//	mpIterator->KillTimer(ZC_TIMER_ID_HIDE);
		//	mpIterator->SetTimer(ZC_TIMER_ID_HIDE, 1, 1000 * 5, NULL);

		//	break;
		//}
		//default:
		//	break;
		//}


		lResult = ERESULT_SUCCESS;
	} while (false);

	return lResult;
}

//��Ϣ������
ERESULT CZoomControlTxt::ParseMessage(IEinkuiMessage* npMsg)
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
	case EEVT_ER_SET_TXT_ZOOM:
	{
		//���÷Ŵ���
		DWORD ldwFontsizeIndex = 0;
		luResult = CExMessage::GetInputData(npMsg, ldwFontsizeIndex);
		if (luResult != ERESULT_SUCCESS)
			break;

		mpIterator->KillTimer(ZC_TIMER_ID_HIDE);

		RECT ldRect;
		CExMessage::SendMessage(mpIterator->GetParent(), mpIterator, EEVT_ER_SET_TXT_ZOOM, ldwFontsizeIndex,&ldRect, sizeof(RECT));
		mpIterator->SetTimer(ZC_TIMER_ID_HIDE, 1, 1000 * 5, NULL);

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
void CZoomControlTxt::OnTimer(
	PSTEMS_TIMER npStatus
	)
{
	if (npStatus->TimerID == ZC_TIMER_ID_HIDE)
	{
		mpIterator->KillTimer(ZC_TIMER_ID_HIDE);

		//PostMessageToParent(EEVT_ER_ENTER_ZOOM, false);
		//ShowItem(false);
	}
}

//Ԫ�زο��ߴ緢���仯
ERESULT CZoomControlTxt::OnElementResized(D2D1_SIZE_F nNewSize)
{
	if (mpZoomControlTxtToolbar != NULL)
	{
		D2D1_SIZE_F ldSize = mpIterator->GetSize();
		
		D2D1_POINT_2F ldPos;

		//��λ
		ldPos.x = (ldSize.width - 340) / 2.0f;
		ldPos.y = ldSize.height - mpZoomControlTxtToolbar->GetIterator()->GetSizeY() - 80.0f;
		mpZoomControlTxtToolbar->GetIterator()->SetPosition(ldPos);

	}

	return ERESULT_SUCCESS;
}

//֪ͨԪ�ء���ʾ/���ء������ı�
ERESULT CZoomControlTxt::OnElementShow(bool nbIsShow)
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
ERESULT CZoomControlTxt::OnMouseOwnerTest(const D2D1_POINT_2F& rPoint)
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
ERESULT CZoomControlTxt::OnMousePressed(const STEMS_MOUSE_BUTTON* npInfo)
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
						if ((mdPressPos.x - npInfo->Position.x) >= 50.0f)
						{
							//��һҳ
							PostMessageToParent(EEVT_ER_PRE_NEXT_CLICKED, PNB_BT_NEXT);
							PostMessageToParent(EEVT_ER_ENTER_ZOOM, false);
							break;
						}
						else if ((npInfo->Position.x - mdPressPos.x) >= 50.0f)
						{
							//ǰһҳ
							PostMessageToParent(EEVT_ER_PRE_NEXT_CLICKED, PNB_BT_PRE);
							PostMessageToParent(EEVT_ER_ENTER_ZOOM, false);
							break;
						}
					}

					//RECT ldRect;
					//CExMessage::SendMessage(mpIterator->GetParent(), mpIterator, EEVT_ER_SET_PAGE_MOVE, ldPos, &ldRect, sizeof(RECT));
					

				} while (false);

			}
			else
			{
				//��Ϊ�ǵ������
				//��ʾ�������Լ��İ�ť
				//ShowItem(!mpZoomControlTxtToolbar->GetIterator()->IsVisible());

				PostMessageToParent(EEVT_ER_ENTER_ZOOM, false);
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
void CZoomControlTxt::ShowItem(bool nbIsShow)
{
	mpZoomControlTxtToolbar->GetIterator()->SetVisible(nbIsShow);

	if (nbIsShow == false)
	{
		PostMessageToParent(EEVT_ER_ENTER_ZOOM, false);
	}
	else
	{
		mpIterator->KillTimer(ZC_TIMER_ID_HIDE);
		mpIterator->SetTimer(ZC_TIMER_ID_HIDE, 1, 1000 * 5, NULL);
	}
}

//����ƶ�
ERESULT CZoomControlTxt::OnMouseMoving(const STEMS_MOUSE_MOVING* npInfo)
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

//���÷Ŵ���ͼ��С����ʾ����
void CZoomControlTxt::SetRectOfViewportOnPage(D2D1_SIZE_F& nrImageSize, D2D1_RECT_F& nrViewPort)
{
	
}