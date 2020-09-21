/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#include "StdAfx.h"
#include "ZoomControlToolbar.h"
#include "XCtl.h"
#include "stdio.h"
#include "cmmStrHandle.h"
#include "cmmPath.h"
#include "MsgDefine.h"
#include "ToolbarBottom.h"

DEFINE_BUILTIN_NAME(ZoomControlToolbar)

CZoomControlToolbar::CZoomControlToolbar(void)
{
	mpIterBili = NULL;

	mlCurrentZoomLevel = 0;
	mfZoom.Insert(-1, 1.0);
}

CZoomControlToolbar::~CZoomControlToolbar(void)
{
}

//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
ERESULT CZoomControlToolbar::OnElementCreate(IEinkuiIterator* npIterator)
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

ULONG CZoomControlToolbar::InitOnCreate(
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
		mpIterBili = mpIterator->GetSubElementByID(110);
		BREAK_ON_NULL(mpIterBili);
		
		mpIterBtAdd = mpIterator->GetSubElementByID(ZC_BT_ADD);
		BREAK_ON_NULL(mpIterBtAdd);

		mpIterBtSub = mpIterator->GetSubElementByID(ZC_BT_SUB);
		BREAK_ON_NULL(mpIterBtSub);

		mpIterBtAutoZoom = mpIterator->GetSubElementByID(ZC_BT_AUTO_ZOOM);
		BREAK_ON_NULL(mpIterBtAutoZoom);

		mpIterBtResetZoom = mpIterator->GetSubElementByID(ZC_BT_RESET_ZOOM);
		BREAK_ON_NULL(mpIterBtResetZoom);

		leResult = ERESULT_SUCCESS;

	} while (false);

	CMM_SAFE_RELEASE(lpSubKey);

	// ��ϵͳע����Ҫ�յ�����Ϣ
	return leResult;
}

//������ʾ����
void CZoomControlToolbar::SetString(ULONG nulLevel)
{
	wchar_t lszString[MAX_PATH] = { 0 };
	swprintf_s(lszString, MAX_PATH, L"%d%s", int(100 + nulLevel*10), L"%");

	CExMessage::SendMessageWithText(mpIterBili, mpIterator, EACT_LABEL_SET_TEXT, lszString, NULL, 0);
}

//������ʾ����
void CZoomControlToolbar::SetRatioString(double ratio)
{
	wchar_t lszString[MAX_PATH] = { 0 };
	swprintf_s(lszString, MAX_PATH, L"%d%s", int(ratio * 100.0), L"%");
	CExMessage::SendMessageWithText(mpIterBili, mpIterator, EACT_LABEL_SET_TEXT, lszString, NULL, 0);
}

//��ť�����¼�
ERESULT CZoomControlToolbar::OnCtlButtonClick(IEinkuiIterator* npSender)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		CExMessage::PostMessage(EinkuiGetSystem()->GetCurrentWidget()->GetHomePage(), mpIterator, EEVT_RESET_HIDE_TIME, true);

		ULONG llBtnID = npSender->GetID();
		switch (llBtnID)
		{
		case ZC_BT_DEFAULT:
		case ZC_BT_RESET_ZOOM:
		{
			//�ָ�Ϊ100%��ʾ
			PostMessageToParent(EEVT_ER_ENTER_ZOOM, false);
			initData();
			break;
		}
		case ZC_BT_ADD:
		{
			//�Ŵ�
			SetLevel(true);
			break;
		}
		case ZC_BT_SUB:
		{
			//��С
			SetLevel(false);
			break;
		}
		case ZC_BT_AUTO_ZOOM:
		{
			SendMessageToParent(EEVT_ER_AUTO_ZOOM, NULL, NULL, NULL);
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
void CZoomControlToolbar::initData(void)
{
	mlCurrentZoomLevel = 0;
	mpIterBtSub->SetEnable(true);
	mpIterBtAdd->SetEnable(true);
	SetString(mlCurrentZoomLevel);
}

void CZoomControlToolbar::SetFatRatio(double fatRatio)
{
	mfFatRatio = 1.0;
	bool fatSaved = false;
	mfZoom.Clear();
	//100%��120%��140%��160%
	mfZoom.Insert(-1, mfFatRatio);
	for(int i=11;i<=40;i+=1)
		mfZoom.Insert(-1, mfFatRatio*i*0.1);
}

//��������״̬,Ĭ��120%��ʾ
// [zhuhl5@20200116:ZoomRecovery]
// Modified function params for initiating zoom level.
void CZoomControlToolbar::EnterZoomStatus(int scaleLevel /*= 0*/)
{
	mlCurrentZoomLevel = 1;
	if (scaleLevel >= 1)
	{
		mlCurrentZoomLevel = scaleLevel; //Ĭ��120%
	}
	RECT ldRect;
	mpIterBtAdd->SetEnable(true);
	mpIterBtSub->SetEnable(true);
	CExMessage::SendMessage(mpIterator->GetParent()->GetParent(), mpIterator, EEVT_ER_SET_ZOOM, mfZoom[mlCurrentZoomLevel], &ldRect, sizeof(RECT));

	// Add zoom level notification, for recording
	CExMessage::SendMessage(mpIterator->GetParent()->GetParent(), mpIterator, EEVT_ER_SET_ZOOMLEVEL, mlCurrentZoomLevel);
}

//��������Ӧ���ű�������Ӧ�����ʵ����ŵȼ�
float CZoomControlToolbar::AjustAutoZoomLevel(float ratio)
{
	wchar_t* lszString = L"Auto";
	CExMessage::SendMessageWithText(mpIterBili, mpIterator, EACT_LABEL_SET_TEXT, lszString, NULL, 0);

	mlCurrentZoomLevel = -1;
	for (int i = 0; i < mfZoom.Size(); ++i)
	{
		if (ratio >= mfZoom[i])
			++mlCurrentZoomLevel;
		else
			break;
	}

	if (mlCurrentZoomLevel >= mfZoom.Size() - 1)
		mpIterBtAdd->SetEnable(false);
	else
		mpIterBtAdd->SetEnable(true);

	return mfZoom[mlCurrentZoomLevel];
}

void CZoomControlToolbar::EnableAutoZoomButton(bool enable)
{
	mpIterBtAutoZoom->SetEnable(enable);
}

//���÷Ŵ󼶱�
void CZoomControlToolbar::SetLevel(bool nbIsAdd)
{
	do
	{
		if (nbIsAdd == false)
		{
			//����
			if (mlCurrentZoomLevel <= 0)
			{
				PostMessageToParent(EEVT_ER_ENTER_ZOOM, false);
				initData();
				break; //�Ѿ��������
			}

			if (--mlCurrentZoomLevel <= 0)
				mpIterBtSub->SetEnable(false);

			mpIterBtAdd->SetEnable(true);
		}
		else
		{
			//����
			if (mlCurrentZoomLevel >= mfZoom.Size()-1)
				break; //�Ѿ��������

			if (++mlCurrentZoomLevel >= mfZoom.Size()-1)
				mpIterBtAdd->SetEnable(false);

			mpIterBtSub->SetEnable(true);
		}

		RECT ldRect;
		CExMessage::SendMessage(mpIterator->GetParent()->GetParent(), mpIterator, EEVT_ER_SET_ZOOM, mfZoom[mlCurrentZoomLevel], &ldRect, sizeof(RECT));
		// Add zoom level notification, for recording
		CExMessage::SendMessage(mpIterator->GetParent()->GetParent(), mpIterator, EEVT_ER_SET_ZOOMLEVEL, mlCurrentZoomLevel);
		
		if (mlCurrentZoomLevel <= 0)
		{
			PostMessageToParent(EEVT_ER_ENTER_ZOOM, false);
			initData();
		}

	} while (false);
}

//��Ϣ������
ERESULT CZoomControlToolbar::ParseMessage(IEinkuiMessage* npMsg)
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
void CZoomControlToolbar::OnTimer(
	PSTEMS_TIMER npStatus
	)
{
}

//Ԫ�زο��ߴ緢���仯
ERESULT CZoomControlToolbar::OnElementResized(D2D1_SIZE_F nNewSize)
{
	return ERESULT_SUCCESS;
}

//֪ͨԪ�ء���ʾ/���ء������ı�
ERESULT CZoomControlToolbar::OnElementShow(bool nbIsShow)
{
	return ERESULT_SUCCESS;
}