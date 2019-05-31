/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#include "StdAfx.h"
#include "TipFrame.h"
#include "XCtl.h"
#include "stdio.h"
#include "cmmStrHandle.h"
#include "cmmPath.h"
#include "MsgDefine.h"

DEFINE_BUILTIN_NAME(TipFrame)

CTipFrame::CTipFrame(void)
{

}


CTipFrame::~CTipFrame(void)
{
}

//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
ERESULT CTipFrame::OnElementCreate(IEinkuiIterator* npIterator)
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

ULONG CTipFrame::InitOnCreate(
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
		mpIterBtClose = mpIterator->GetSubElementByID(TIP_F_BT_CLOSE);
		BREAK_ON_NULL(mpIterBtClose);

		mpIterPicBack = mpIterator->GetSubElementByID(1);
		BREAK_ON_NULL(mpIterPicBack);

		mpIterPicLeft = mpIterator->GetSubElementByID(2);
		BREAK_ON_NULL(mpIterPicLeft);

		mpIterPicRight = mpIterator->GetSubElementByID(3);
		BREAK_ON_NULL(mpIterPicRight);

		mpIterPicPic = mpIterator->GetSubElementByID(4);
		BREAK_ON_NULL(mpIterPicPic);

		mpIterText = mpIterator->GetSubElementByID(5);
		BREAK_ON_NULL(mpIterText);
		
		leResult = ERESULT_SUCCESS;

	} while (false);

	CMM_SAFE_RELEASE(lpSubKey);

	// ��ϵͳע����Ҫ�յ�����Ϣ
	return leResult;
}

//��ť�����¼�
ERESULT CTipFrame::OnCtlButtonClick(IEinkuiIterator* npSender)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		ULONG llBtnID = npSender->GetID();
		switch (llBtnID)
		{
		case TIP_F_BT_CLOSE:
		{
			ExitModal();
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
ERESULT CTipFrame::ParseMessage(IEinkuiMessage* npMsg)
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
void CTipFrame::OnTimer(
	PSTEMS_TIMER npStatus
	)
{

}

void CTipFrame::DoModal()
{
	do
	{
		mpIterator->SetActive();
		mpIterator->BringToTop();
		EinkuiGetSystem()->UpdateView(true);
		EinkuiGetSystem()->DoModal(mpIterator);


		mpIterator->Close();

	} while (false);
}

void CTipFrame::ExitModal()
{
	EinkuiGetSystem()->ExitModal(mpIterator, 0);

}

//Ԫ�زο��ߴ緢���仯
ERESULT CTipFrame::OnElementResized(D2D1_SIZE_F nNewSize)
{
	CExMessage::SendMessage(mpIterBtClose, mpIterator, EACT_BUTTON_SET_ACTION_RECT, nNewSize);
	mpIterPicBack->SetSize(nNewSize);

	//��������ͼƬ
	FLOAT lfY = (nNewSize.height - mpIterPicLeft->GetSizeY()) / 2;
	mpIterPicLeft->SetPosition(mpIterPicLeft->GetPositionX(), lfY);
	mpIterPicRight->SetPosition(nNewSize.width-mpIterPicRight->GetSizeX() - 40.0f, lfY);

	//�м�ͼƬ
	FLOAT lfX = (nNewSize.width - mpIterPicPic->GetSizeY()) / 2;
	lfY = (nNewSize.height - mpIterPicPic->GetSizeY()) / 2 + 40;
	mpIterPicPic->SetPosition(lfX, lfY);

	//�м�����
	lfX = (nNewSize.width - mpIterText->GetSizeY()) / 2;
	lfY = (nNewSize.height - mpIterText->GetSizeY()) / 2 - 40;
	mpIterText->SetPosition(lfX, lfY);

	return ERESULT_SUCCESS;
}

//֪ͨԪ�ء���ʾ/���ء������ı�
ERESULT CTipFrame::OnElementShow(bool nbIsShow)
{
	//EiSetHomebarStatus(nbIsShow == false ? GI_HOMEBAR_SHOW : GI_HOMEBAR_HIDE);

	return ERESULT_SUCCESS;
}