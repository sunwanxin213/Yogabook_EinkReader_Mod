/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#include "StdAfx.h"
#include "ToolbarBottom.h"
#include "XCtl.h"
#include "stdio.h"
#include "cmmStrHandle.h"
#include "cmmPath.h"
#include "MsgDefine.h"
#include "PreNextButton.h"
#include "..\ECtl\EvButtonImp.h"

DEFINE_BUILTIN_NAME(ToolbarBottom)

CToolbarBottom::CToolbarBottom(void)
{
	mpIterBackground = NULL;
	miDocType = DOC_TYPE_PDF;
	mpPageProgress = NULL;
}

CToolbarBottom::~CToolbarBottom(void)
{
}

//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
ERESULT CToolbarBottom::OnElementCreate(IEinkuiIterator* npIterator)
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


ULONG CToolbarBottom::InitOnCreate(
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

		//�ƶ�
		lpSubKey = mpTemplete->OpenKey(L"MoveControlToolbar");
		mpMoveControlToolbar = CMoveControlToolbar::CreateInstance(mpIterator, lpSubKey);
		CMM_SAFE_RELEASE(lpSubKey);
		BREAK_ON_NULL(mpMoveControlToolbar);
		mpMoveControlToolbar->GetIterator()->SetVisible(false);

		//��ȡ������
		mpIterBtPageNumber = mpIterator->GetSubElementByID(TBB_BT_NUMBER);
		BREAK_ON_NULL(mpIterBtPageNumber);

		mpIterBtPre = mpIterator->GetSubElementByID(TBB_BT_PRE);
		BREAK_ON_NULL(mpIterBtPre);

		mpIterBtNext = mpIterator->GetSubElementByID(TBB_BT_NEXT);
		BREAK_ON_NULL(mpIterBtNext);

		mpIterBackground = mpIterator->GetSubElementByID(1);
		BREAK_ON_NULL(mpIterBackground);

		//������
		lpSubKey = mpTemplete->OpenKey(L"ZoomControlToolbar");
		mpZoomControlToolbar = CZoomControlToolbar::CreateInstance(mpIterator, lpSubKey);
		CMM_SAFE_RELEASE(lpSubKey);
		BREAK_ON_NULL(mpZoomControlToolbar);
		mpZoomControlToolbar->GetIterator()->SetVisible(false);

		//ҳ����ת
		lpSubKey = mpTemplete->OpenKey(L"PageProgress");
		mpPageProgress = CPageProgress::CreateInstance(mpIterator, lpSubKey);
		CMM_SAFE_RELEASE(lpSubKey);
		BREAK_ON_NULL(mpPageProgress);
		mpPageProgress->GetIterator()->SetVisible(false);

		leResult = ERESULT_SUCCESS;

	} while (false);

	CMM_SAFE_RELEASE(lpSubKey);

	// ��ϵͳע����Ҫ�յ�����Ϣ
	return leResult;
}

//����/��ʾ��ҳ������
void CToolbarBottom::HidePageProcess(bool nbIsShow)
{
	if (mpPageProgress != NULL)
		mpPageProgress->GetIterator()->SetVisible(nbIsShow);
}

//��ʾ������Ԫ��
void CToolbarBottom::ShowItem(bool nbIsShow)
{
	mpIterator->SetVisible(nbIsShow);
}

//��ť�����¼�
ERESULT CToolbarBottom::OnCtlButtonClick(IEinkuiIterator* npSender)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{

		PostMessageToParent(EEVT_RESET_HIDE_TIME, true);

		ULONG llBtnID = npSender->GetID();
		switch (llBtnID)
		{
		case TBB_BT_PRE:
		{
			//��һҳ
			PostMessageToParent(EEVT_ER_PRE_NEXT_CLICKED, PNB_BT_PRE);

			//mpIterator->SetVisible(false);
			break;
		}
		case TBB_BT_NEXT:
		{
			//��һҳ
			PostMessageToParent(EEVT_ER_PRE_NEXT_CLICKED, PNB_BT_NEXT);

			//mpIterator->SetVisible(false);
			break;
		}
		case TBB_BT_NUMBER:
		{
			//����ҳ��
			//PostMessageToParent(EEVT_ER_TWO_SCREEN, true);
			if (mpPageProgress != NULL)
			{
				mpPageProgress->SetData((FLOAT)mulCurrentPage, (FLOAT)mulPageCount);
				mpPageProgress->GetIterator()->SetVisible(!mpPageProgress->GetIterator()->IsVisible());
			}
				

			break;
		}
		case TBB_BT_ZOOM:
		{
			//��������
			ActiveZoom();

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
ERESULT CToolbarBottom::ParseMessage(IEinkuiMessage* npMsg)
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
		SetZoomStatus(ZoomStatus::NONE);
		break;
	}
	case EEVT_ER_SET_ZOOM:
	{
		//���÷Ŵ���
		double lfRatio = 1.0;
		luResult = CExMessage::GetInputData(npMsg, lfRatio);
		if (luResult != ERESULT_SUCCESS)
			break;

		RECT ldRect;
		CExMessage::SendMessage(mpIterator->GetParent(), mpIterator, EEVT_ER_SET_ZOOM, lfRatio, &ldRect, sizeof(RECT));
		mpMoveControlToolbar->ShowMoveButton(ldRect);

		break;
	}
	case EEVT_ER_AUTO_ZOOM:
	{
		CExMessage::SendMessage(mpIterator->GetParent(), mpIterator, EEVT_ER_AUTO_ZOOM, NULL, NULL, NULL);
		break;
	}
	case EEVT_ER_SET_ZOOMLEVEL:
	{
		int scalingLevel = 0;
		luResult = CExMessage::GetInputData(npMsg, scalingLevel);
		if (luResult != ERESULT_SUCCESS)
			break;
		CExMessage::SendMessage(mpIterator->GetParent(), mpIterator, EEVT_ER_SET_ZOOM, scalingLevel);
		break;
	}
	case EEVT_ER_PAGE_JUMP:
	{
		//ҳ����ת
		int liPage = 1;
		luResult = CExMessage::GetInputData(npMsg, liPage);
		if (luResult != ERESULT_SUCCESS)
			break;

		CExMessage::PostMessage(mpIterator->GetParent(), mpIterator, EEVT_ER_PAGE_JUMP, liPage);

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
void CToolbarBottom::OnTimer(
	PSTEMS_TIMER npStatus
	)
{

}

//��������ģʽ
void CToolbarBottom::SetZoomStatus(ZoomStatus status, int scaleLevel /*= 0*/)
{
	if (status == ZoomStatus::NONE)
	{
		mZoomStatus = status;
		mpZoomControlToolbar->initData();

		RECT ldRect;
		double lfRatio = 1.0;
		CExMessage::SendMessage(mpIterator->GetParent(), mpIterator, EEVT_ER_SET_ZOOM, lfRatio, &ldRect, sizeof(RECT));
		
		RelocationItem();
		PostMessageToParent(EEVT_ER_ENTER_ZOOM, false);
	}
	else if (miDocType != DOC_TYPE_TXT)
	{
		if (status == ZoomStatus::ZOOM)
			ActiveZoom(scaleLevel);
		else if (status == ZoomStatus::AUTO_ZOOM)
			CExMessage::SendMessage(mpIterator->GetParent(), mpIterator, EEVT_ER_AUTO_ZOOM, NULL, NULL, NULL);
	}
}

//���õ�ǰ�򿪵��ļ�����
void CToolbarBottom::SetDoctype(int niType)
{
	miDocType = niType;
	RelocationItem();

	//�ر���ҳ������
	mpPageProgress->GetIterator()->SetVisible(false);
}

//���¶�λԪ��
void CToolbarBottom::RelocationItem(void)
{
	if (mpIterBackground != NULL)
	{
		EI_SIZE ldPaintSize;
		EinkuiGetSystem()->GetPaintboardSize(&ldPaintSize);

		float lfWidth = 0.0f;
		D2D1_POINT_2F ldPos;
		ldPos.y = 10.0f;

		float lfBackHeight = 80.0f;
		mpIterator->SetPosition(0.0f, ldPaintSize.h - lfBackHeight);

		//����
		CExMessage::SendMessageWithText(mpIterBackground, mpIterator, EACT_PICTUREFRAME_CHANGE_PIC, L".\\Pic\\toolbar_h.png");
		mpIterBackground->SetSize(float(ldPaintSize.w), lfBackHeight);

		if (miDocType == DOC_TYPE_TXT)
		{
			//�����txt�ļ�����ֻ�з�ҳ��ť
			lfWidth = mpIterBtPre->GetSizeX() + mpIterBtNext->GetSizeX() + mpIterBtPageNumber->GetSizeX();
			ldPos.x = (ldPaintSize.w - lfWidth) / 2.0f;

			mpMoveControlToolbar->GetIterator()->SetVisible(false);
			mpZoomControlToolbar->GetIterator()->SetVisible(false);
		}
		else if (miDocType == DOC_TYPE_PDF)
		{
			mpMoveControlToolbar->GetIterator()->SetVisible(true);
			mpZoomControlToolbar->GetIterator()->SetVisible(true);
		}
		else
		{
			mpMoveControlToolbar->GetIterator()->SetVisible(true);
			mpZoomControlToolbar->GetIterator()->SetVisible(true);
		}

		lfWidth =  mpIterBtPre->GetSizeX() + mpIterBtNext->GetSizeX() + mpIterBtPageNumber->GetSizeX();
		ldPos.x = (ldPaintSize.w - lfWidth) / 2.0f;
		ldPos.y = 0.0f;

		if (mpZoomControlToolbar->GetIterator()->IsVisible())
		{
			lfWidth += mpZoomControlToolbar->GetIterator()->GetSizeX();
			ldPos.x = (ldPaintSize.w - lfWidth) / 2.0f;

			if (mpMoveControlToolbar->GetIterator()->IsVisible())
			{
				FLOAT fPosX = mpMoveControlToolbar->GetIterator()->GetPositionX();
				FLOAT fSizeX = mpMoveControlToolbar->GetIterator()->GetSizeX();
				if (ldPos.x < fPosX + fSizeX + 5.0f)
					ldPos.x = fPosX + fSizeX + 5.0f;
			}

			mpZoomControlToolbar->GetIterator()->SetPosition(ldPos);
			ldPos.x += mpZoomControlToolbar->GetIterator()->GetSizeX() + 30.0f;
			ldPos.y = 10.0f;
		}

		mpIterBtPre->SetPosition(ldPos);
		ldPos.x += mpIterBtPre->GetSizeX();
		mpIterBtPageNumber->SetPosition(ldPos);
		ldPos.x += mpIterBtPageNumber->GetSizeX();
		mpIterBtNext->SetPosition(ldPos);

		ldPos.x = mpIterBtPageNumber->GetPositionX() - (mpPageProgress->GetIterator()->GetSizeX() - mpIterBtPageNumber->GetSizeX()) / 2.0f;
		ldPos.y = -80.0f;
		if (ldPos.x + mpPageProgress->GetIterator()->GetSizeX() > ldPaintSize.w)
			ldPos.x = ldPaintSize.w - mpPageProgress->GetIterator()->GetSizeX()-75.0f; //���������Ļ����������

		mpPageProgress->GetIterator()->SetPosition(ldPos);
		ldPos.x = mpIterBtPageNumber->GetPositionX() - ldPos.x + mpIterBtPageNumber->GetSizeX()/2.0f;
		ldPos.y = 58.0f;
		mpPageProgress->SetArrowPos(ldPos);
	}
}

//���û�������
void CToolbarBottom::SetMoveForward(MoveForward forward)
{
	mpMoveControlToolbar->SetMoveForward(forward);
}

//��������Ӧ���ű�������Ӧ�����ʵ����ŵȼ�
float CToolbarBottom::AjustAutoZoomLevel(float ratio)
{
	return mpZoomControlToolbar->AjustAutoZoomLevel(ratio);
}

void CToolbarBottom::EnableAutoZoomButton(bool enable)
{
	return mpZoomControlToolbar->EnableAutoZoomButton(enable);
}

void CToolbarBottom::SetRatioString(double ratio)
{
	mpZoomControlToolbar->SetRatioString(ratio);
}

//Ԫ�زο��ߴ緢���仯
ERESULT CToolbarBottom::OnElementResized(D2D1_SIZE_F nNewSize)
{
	RelocationItem();

	return ERESULT_SUCCESS;
}

//����ҳ���ַ���
void CToolbarBottom::SetPage(ULONG nulCurrentPage, ULONG nulPageCount)
{
	do
	{
		wchar_t lszString[MAX_PATH] = { 0 };

		swprintf_s(lszString, MAX_PATH, L"%d/%d", nulCurrentPage, nulPageCount);

		CExMessage::PostMessageWithText(mpIterBtPageNumber, mpIterator, EACT_BUTTON_SETTEXT, lszString);

		mpIterBtPre->SetEnable(nulCurrentPage > 1?true:false);
		mpIterBtNext->SetEnable(nulCurrentPage < nulPageCount ? true : false);

		mulCurrentPage = nulCurrentPage;
		mulPageCount = nulPageCount;

		if (mpPageProgress != NULL)
			mpPageProgress->SetData((FLOAT)mulCurrentPage, (FLOAT)mulPageCount);

	} while (false);
}


//֪ͨԪ�ء���ʾ/���ء������ı�
ERESULT CToolbarBottom::OnElementShow(bool nbIsShow)
{
	//EiSetHomebarStatus(nbIsShow == false ? GI_HOMEBAR_SHOW : GI_HOMEBAR_HIDE);
	if (nbIsShow != false)
	{
		mpIterator->BringToTop();
	}
	else
	{
		//�Լ�����ʱ�ѷ�ҳ������Ҳ�����ˣ��´���ʾʱ����ʾ����Ҫ�û��ٴε��ҳ������Ż���ʾ
		mpPageProgress->GetIterator()->SetVisible(false);
	}

	return ERESULT_SUCCESS;
}

void CToolbarBottom::SetFatRatio(float fatRatio)
{
	mpZoomControlToolbar->SetFatRatio(fatRatio);
}

//����4���ƶ���ť��״̬
void CToolbarBottom::ShowMoveButton(RECT ldRect)
{
	mpMoveControlToolbar->ShowMoveButton(ldRect);
}

// To active Zoom mode. [zhuhl5@20200116:ZoomRecovery]
void CToolbarBottom::ActiveZoom(int scaleLevel /*= 0*/)
{
	mZoomStatus = ZoomStatus::ZOOM;
	SendMessageToParent(EEVT_ER_ENTER_ZOOM, true, NULL, 0);

	mpZoomControlToolbar->EnterZoomStatus(scaleLevel);
	RelocationItem();
}

void CToolbarBottom::SetMoveToolbarVisible(bool visible)
{
	mpMoveControlToolbar->GetIterator()->SetVisible(visible);
}
