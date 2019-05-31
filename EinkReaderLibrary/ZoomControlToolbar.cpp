/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#include "StdAfx.h"
#include "ZoomControlToolbar.h"
#include "XCtl.h"
#include "stdio.h"
#include "cmmStrHandle.h"
#include "cmmPath.h"
#include "MsgDefine.h"

DEFINE_BUILTIN_NAME(ZoomControlToolbar)

float defaultRatios[] = {1.0f,1.26f,1.90f,2.54f,3.18f,4.78f,6.38f,9.58f,12.78f,19.18f,25.58f,35.18f,41.18f,68.18f,83.18f,115.18f,179.18f,0.0f};




CZoomControlToolbar::CZoomControlToolbar(void)
{
	mpIterBili = NULL;

	mlCurrentZoomLevel = 0;
	mfZoom.Insert(-1, 1.0f);
	//miMaxRatioInx = 0;
	miFatRatioInx = 0;
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

		mpIterBtDefault = mpIterator->GetSubElementByID(ZC_BT_DEFAULT);
		BREAK_ON_NULL(mpIterBtDefault);

		initData();

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
	swprintf_s(lszString, MAX_PATH, L"%d%s", int(mfZoom[nulLevel] * 100),L"%");
	CExMessage::SendMessageWithText(mpIterBili, mpIterator, EACT_LABEL_SET_TEXT, lszString, NULL, 0);
}

//��ť�����¼�
ERESULT CZoomControlToolbar::OnCtlButtonClick(IEinkuiIterator* npSender)
{

	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		ULONG llBtnID = npSender->GetID();
		switch (llBtnID)
		{
		case ZC_BT_DEFAULT:
		{
			//�ָ�Ϊ100%��ʾ
			SendMessageToParent(EEVT_ER_SET_ZOOM, mfZoom[miFatRatioInx],NULL,0);
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
		case ZC_BT_CLOSE:
		{
			//�˳�����ģʽ
			PostMessageToParent(EEVT_ER_ENTER_ZOOM, false);
			initData();

			break;
		}
		case ZC_BT_SNAP:
		{
			//����
			PostMessageToParent(EEVT_ER_ENTER_SNAPSHOT, false);

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
	mlCurrentZoomLevel = miFatRatioInx;
	mpIterBtSub->SetEnable(true);
	mpIterBtAdd->SetEnable(true);
	SetString(mlCurrentZoomLevel);

	//�û�
	mpIterBtDefault->SetEnable(false);
}

void CZoomControlToolbar::SetFatRatio(float fatRatio)
{
	//mfFatRatio = fatRatio;
	bool fatSaved = false;
	mfZoom.Clear();
	for (int i=0;defaultRatios[i]>0.0f;i++)
	{
		/*if (defaultRatios[i] > fatRatio*4.0f)
			break;*/

		if (i > miFatRatioInx + 4)
			break; //������Ļ���ٷŴ�4��

		if (fatSaved == false && CExFloat::Equal(defaultRatios[i], fatRatio, 0.1f))	// ����ֱ���滻
		{
			mfZoom.Insert(-1, fatRatio);
			miFatRatioInx = i;
			fatSaved = true;
		}
		else
			mfZoom.Insert(-1, defaultRatios[i]);

		if (fatSaved == false && defaultRatios[i] < fatRatio && defaultRatios[i + 1] > fatRatio) 
		{ 
			fatSaved = true;

			//�ж�һ�£�����ǰ��Ľ������������Ľ�
			if ((fatRatio - mfZoom.GetEntry(i)) > (defaultRatios[i + 1] - fatRatio))
			{
				miFatRatioInx = i + 1;
				mfZoom.Insert(-1, fatRatio);

				i++; //���������һ�����������
			}
			else
			{
				miFatRatioInx = i;
				mfZoom.GetEntry(i) = fatRatio;
			}
		}
		//miMaxRatioInx = i;
	}
	SendMessageToParent(EEVT_ER_SET_ZOOM, mfZoom[miFatRatioInx], NULL, 0);
	initData();
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
				break; //�Ѿ��������

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

		SendMessageToParent(EEVT_ER_SET_ZOOM, mfZoom[mlCurrentZoomLevel],NULL,0);
		SetString(mlCurrentZoomLevel);

		if (mlCurrentZoomLevel == miFatRatioInx)
			mpIterBtDefault->SetEnable(false);// CExMessage::SendMessageWithText(mpIterBtDefault, mpIterator, EACT_BUTTON_CHANGE_PIC, L".\\Pic\\zoom100%_ic_disable.png");
		else
			mpIterBtDefault->SetEnable(true);

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
	//EiSetHomebarStatus(nbIsShow == false ? GI_HOMEBAR_SHOW : GI_HOMEBAR_HIDE);

	return ERESULT_SUCCESS;
}