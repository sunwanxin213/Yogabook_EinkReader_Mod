/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#include "StdAfx.h"
#include "ZoomControlToolbarTxt.h"
#include "XCtl.h"
#include "stdio.h"
#include "cmmStrHandle.h"
#include "cmmPath.h"
#include "MsgDefine.h"

DEFINE_BUILTIN_NAME(ZoomControlToolbarTxt)


CZoomControlToolbarTxt::CZoomControlToolbarTxt(void)
{
	mdwFontSizeIndex = 2;
}


CZoomControlToolbarTxt::~CZoomControlToolbarTxt(void)
{
}

//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
ERESULT CZoomControlToolbarTxt::OnElementCreate(IEinkuiIterator* npIterator)
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

ULONG CZoomControlToolbarTxt::InitOnCreate(
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
		
		mpIterBtAdd = mpIterator->GetSubElementByID(ZCT_BT_ADD);
		BREAK_ON_NULL(mpIterBtAdd);

		mpIterBtSub = mpIterator->GetSubElementByID(ZCT_BT_SUB);
		BREAK_ON_NULL(mpIterBtSub);

		initData();

		leResult = ERESULT_SUCCESS;

	} while (false);

	CMM_SAFE_RELEASE(lpSubKey);

	// ��ϵͳע����Ҫ�յ�����Ϣ
	return leResult;
}

void CZoomControlToolbarTxt::SetFontsize(DWORD ldwFontSizeIndex)
{
	mdwFontSizeIndex = ldwFontSizeIndex;
}

//��ť�����¼�
ERESULT CZoomControlToolbarTxt::OnCtlButtonClick(IEinkuiIterator* npSender)
{

	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		ULONG llBtnID = npSender->GetID();
		switch (llBtnID)
		{
		case ZCT_BT_ADD:
		{
			//�Ŵ�
			SetLevel(true);

			break;
		}
		case ZCT_BT_SUB:
		{
			//��С
			SetLevel(false);

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
void CZoomControlToolbarTxt::initData(void)
{
	//mpIterBtSub->SetEnable(mdwFontSizeIndex <= 0?false:true);

	//mpIterBtAdd->SetEnable(mdwFontSizeIndex >= ZCT_FONTSIZE_LEVEL-1 ? false : true);
}

//���÷Ŵ󼶱�
void CZoomControlToolbarTxt::SetLevel(bool nbIsAdd)
{
	do
	{
		if (nbIsAdd == false)
		{
			//����
			if (mdwFontSizeIndex <= 0)
				break; //�Ѿ��������

			--mdwFontSizeIndex;
			/*if (--mdwFontSizeIndex <= 0)
				mpIterBtSub->SetEnable(false);*/

			mpIterBtAdd->SetEnable(true);
		}
		else
		{
			//����
			if (mdwFontSizeIndex >= ZCT_FONTSIZE_LEVEL - 1)
				break; //�Ѿ��������

			++mdwFontSizeIndex;
			/*if (++mdwFontSizeIndex >= ZCT_FONTSIZE_LEVEL - 1)
				mpIterBtAdd->SetEnable(false);*/

			mpIterBtSub->SetEnable(true);
		}

		PostMessageToParent(EEVT_ER_SET_TXT_ZOOM, mdwFontSizeIndex);

	} while (false);
}

//��Ϣ������
ERESULT CZoomControlToolbarTxt::ParseMessage(IEinkuiMessage* npMsg)
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
void CZoomControlToolbarTxt::OnTimer(
	PSTEMS_TIMER npStatus
	)
{

}

//Ԫ�زο��ߴ緢���仯
ERESULT CZoomControlToolbarTxt::OnElementResized(D2D1_SIZE_F nNewSize)
{
	
	return ERESULT_SUCCESS;
}


//֪ͨԪ�ء���ʾ/���ء������ı�
ERESULT CZoomControlToolbarTxt::OnElementShow(bool nbIsShow)
{
	//EiSetHomebarStatus(nbIsShow == false ? GI_HOMEBAR_SHOW : GI_HOMEBAR_HIDE);

	return ERESULT_SUCCESS;
}