/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#include "StdAfx.h"
#include "ThumbnailListItem.h"
#include "XCtl.h"
#include "stdio.h"
#include "cmmStrHandle.h"
#include "cmmPath.h"
#include "MsgDefine.h"
#include <time.h>

DEFINE_BUILTIN_NAME(ThumbnailListItem)

CThumbnailListItem::CThumbnailListItem(void)
{
	mdwClickTicount = 0;
	mpIterPicture = NULL;
	mpIterPageNumber = NULL;
	mpIterBtClick = NULL;
	mulPageNumber = 0;
	mpLineBrush = NULL;
	mdPicSize.width = mdPicSize.height = 80.0f;
	mbIsCurrent = false;
}


CThumbnailListItem::~CThumbnailListItem(void)
{

}

//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
ERESULT CThumbnailListItem::OnElementCreate(IEinkuiIterator* npIterator)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		if(CXuiElement::OnElementCreate(npIterator) != ERESULT_SUCCESS)
			break;

		//mpIterator->ModifyStyles(EITR_STYLE_POPUP);
		
		//mpIterPicture->SetRotation(90.0f);

		lResult = ERESULT_SUCCESS;

	}while(false);

	return lResult;
}

// ����Ԫ�أ�����֪ͨһ��Ԫ�����٣�ע�⣺��Ԫ�������յ�����Ϣ��Ӧ�ü�ʱ����Ԫ�ع�������UnregisterElement������
// �Ӷ�����Ԫ�ع�������������һ��Ԫ�ط���������Ϣ�������ٽ��Լ���Ԫ�ع�����ע���������ͷ��������
ERESULT CThumbnailListItem::OnElementDestroy()
{
	CMM_SAFE_RELEASE(mpLineBrush);

	CXuiElement::OnElementDestroy();

	return ERESULT_SUCCESS;
}

ULONG CThumbnailListItem::InitOnCreate(
	IN IEinkuiIterator* npParent,	// ������ָ��
	IN ICfKey* npTemplete,		// npTemplete��Key ID����EID��ֵ��������EType
	IN ULONG nuEID	// �����Ϊ0��MAXULONG32����ָ����Ԫ�ص�EID; ����ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��XUIϵͳ�Զ�����
	)
{
	ERESULT leResult = ERESULT_UNSUCCESSFUL;
	ICfKey* lpSubKey = NULL;
	IConfigFile* lpProfile = NULL;

	do 
	{
		//���ȵ��û���
		leResult = 	CXuiElement::InitOnCreate(npParent,npTemplete,nuEID);
		if(leResult != ERESULT_SUCCESS)
			break;


		//��ȡ������
		mpIterPageNumber = mpIterator->GetSubElementByID(2);
		BREAK_ON_NULL(mpIterPageNumber);

		mpIterPicture = mpIterator->GetSubElementByID(1);
		BREAK_ON_NULL(mpIterPicture);

		mpIterBtClick = mpIterator->GetSubElementByID(TL_BT_CLICK);
		BREAK_ON_NULL(mpIterBtClick);

		CExMessage::SendMessageWithText(mpIterPicture, mpIterator, EACT_PICTUREFRAME_CHANGE_PIC, L".\\Pic\\loading.png");

		leResult = ERESULT_SUCCESS;

	} while (false);

	CMM_SAFE_RELEASE(lpSubKey);
	CMM_SAFE_RELEASE(lpProfile);

	// ��ϵͳע����Ҫ�յ�����Ϣ
	return leResult;
}

//��������
void CThumbnailListItem::SetData(wchar_t* npszFilePath, ULONG nulPageNumber, bool nbIsCurrent)
{
	do 
	{
		BREAK_ON_NULL(npszFilePath);
		mulPageNumber = nulPageNumber;
		mbIsCurrent = nbIsCurrent;

		wchar_t lszNumber[MAX_PATH] = { 0 };
		swprintf_s(lszNumber, MAX_PATH, L"%d", nulPageNumber);
		CExMessage::SendMessageWithText(mpIterPageNumber, mpIterator, EACT_LABEL_SET_TEXT, lszNumber);

		ZeroMemory(&mdPicSize, sizeof(mdPicSize));
		CExMessage::SendMessageWithText(mpIterPicture, mpIterator, EACT_PICTUREFRAME_CHANGE_PIC_FULLPATH, npszFilePath, &mdPicSize, sizeof(D2D1_SIZE_F));
		mpIterPicture->SetSize(mdPicSize);
		if (mdPicSize.width < 10.0f)
		{
			//˵���ļ����ˣ�ɾ����
			DeleteFile(npszFilePath);
		}

		RelocationItem();

	} while (false);
}

//��ť�����¼�
ERESULT CThumbnailListItem::OnCtlButtonClick(IEinkuiIterator* npSender)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		ULONG llBtnID = npSender->GetID();
		switch (llBtnID)
		{
		case TL_BT_CLICK:
		{
			//�Լ������
			if (GetTickCount() - mdwClickTicount > 400)
			{
				//Ϊ�˷�ֹ˫��
				mdwClickTicount = GetTickCount();
				int liPagenumber = mulPageNumber;
				CExMessage::PostMessage(mpIterator->GetParent(), mpIterator, EEVT_THUMBNAIL_CLICK, liPagenumber);
				//CExMessage::PostMessageWithText(mpIterator->GetParent(), mpIterator, EEVT_ER_LIST_CLICK, mpFileAttrib->FilePath);
			}
			
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
//ERESULT CThumbnailListItem::ParseMessage(IEinkuiMessage* npMsg)
//{
//	ERESULT luResult = ERESULT_UNEXPECTED_MESSAGE;
//
//	switch (npMsg->GetMessageID())
//	{
//	default:
//		luResult = ERESULT_NOT_SET;
//		break;
//	}
//
//	if (luResult == ERESULT_NOT_SET)
//	{
//		luResult = CXuiElement::ParseMessage(npMsg); // ���û����ͬ��������ע�⣺һ��Ҫ��������ֱ�ӻ���
//	}
//
//	return luResult;
//}

//��ʱ��
void CThumbnailListItem::OnTimer(
	PSTEMS_TIMER npStatus
	)
{

}


//����
ERESULT CThumbnailListItem::OnPaint(IEinkuiPaintBoard* npPaintBoard)
{
	ERESULT leResult = ERESULT_UNSUCCESSFUL;

	do
	{
		BREAK_ON_NULL(npPaintBoard);

		leResult = CXuiElement::OnPaint(npPaintBoard);
		if (ERESULT_FAILED(leResult))
			break;

		// ���潨����ˢ
		if (mpLineBrush == NULL)
		{
			mpLineBrush = EinkuiGetSystem()->CreateBrush(XuiSolidBrush, D2D1::ColorF(D2D1::ColorF::Black, 2.0f));
			if (mpLineBrush == NULL)
				break;
		}

		if (mbIsCurrent != false)
			mpLineBrush->SetStrokeWidth(4.0f);
		else
			mpLineBrush->SetStrokeWidth(2.0f);

		D2D1_POINT_2F pt1, pt2;
		
		if (mbIsCurrent != false)
		{	// ���
			pt1.x = 0.0f;
			pt1.y = 0.0f;
			pt2.x = 1.0f;
			pt2.y = mpIterator->GetSizeY();
			npPaintBoard->DrawLine(pt1, pt2, mpLineBrush);
		}
		//if ((impact & 0x2) == 0)
		{	// �ұ�
			pt1.x = mpIterator->GetSizeX();
			pt1.y = 0.0f;
			pt2.x = pt1.x - 1.0f;
			pt2.y = mpIterator->GetSizeY();
			npPaintBoard->DrawLine(pt1, pt2, mpLineBrush);
		}
		if (mbIsCurrent != false)
		{	// �ϱ�
			pt1.x = 0.0f;
			pt1.y = 0.0f;
			pt2.x = mpIterator->GetSizeX();
			pt2.y = 1.0f;
			npPaintBoard->DrawLine(pt1, pt2, mpLineBrush);
		}
		//if ((impact & 0x8) == 0)
		{	// �±�
			pt1.x = 0.0f;
			pt1.y = mpIterator->GetSizeY();
			pt2.x = mpIterator->GetSizeX();
			pt2.y = pt1.y - 1.0f;
			npPaintBoard->DrawLine(pt1, pt2, mpLineBrush);
		}

		leResult = ERESULT_SUCCESS;
	} while (false);

	return leResult;
}


//�ȱ����ţ�ndSizeDestԭʼͼƬ��С  ndSizeSrcҪ���Ƶ�����
//���صȱ����ŵĴ�С
D2D1_RECT_F CThumbnailListItem::GetProportionalScaling(D2D1_SIZE_F ndSizeSrc, D2D1_SIZE_F ndSizeDest)
{
	D2D1_RECT_F ldRetRect;

	// ��¼��С�ı仯��
	float lfSizeX = ndSizeSrc.width;
	float lfSizeY = ndSizeSrc.height;

	// �ȱȱ���ǰ�����ۿ�Ⱥ͸߶�
	float lfBeforeScaledWidth = 0.0f;
	float lfBeforeScaledHeight = 0.0f;

	// ��׼б��
	float lfBaseLineSlope;
	// ��ǰб��
	float lfCurLineSlope;

	// ��ȡ�ı��Ĵ�С
	lfBeforeScaledWidth = ndSizeDest.width + lfSizeX;
	lfBeforeScaledHeight = ndSizeDest.height + lfSizeY;

	{
		// �����׼б��,��ԭʼ���ε�����ȷ����������ʼ��ʼ����(0.0f, 0.0f)������б�ʾ��ǿ�Ⱥ͸߶ȵı�ֵ
		lfBaseLineSlope = ndSizeDest.width / ndSizeDest.height;

		// ���㵱ǰб�ʣ�Ϊ��ǰ�����߶ȵı�ֵ
		lfCurLineSlope = lfBeforeScaledWidth / lfBeforeScaledHeight;
		// ȡ����ֵ�����ǵķ�ת�����
		lfCurLineSlope = lfCurLineSlope > 0 ? lfCurLineSlope : -lfCurLineSlope;
	}

	D2D1_SIZE_F ldNewSize;
	//////////////////////////////////////////////////////////////////////////
	// ����5���Ǿ������룬ͳһ����ȱ����ź�ı仯������˸������
	//////////////////////////////////////////////////////////////////////////
	{

		if (lfCurLineSlope >= lfBaseLineSlope)
		{
			// ȡY������ű���
			lfSizeY = (fabs(lfSizeY + ndSizeSrc.height) - ndSizeSrc.height);
			ldNewSize = D2D1::SizeF(lfSizeY*lfBaseLineSlope, lfSizeY);
		}
		else
		{
			// ȡX������ű�����������X�����
			lfSizeX = (fabs(lfSizeX + ndSizeSrc.width) - ndSizeSrc.width);
			ldNewSize = D2D1::SizeF(lfSizeX, lfSizeX / lfBaseLineSlope);
		}
	}

	ldRetRect.left = 0;
	ldRetRect.top = 0;
	ldRetRect.right = ldNewSize.width;
	ldRetRect.bottom = ldNewSize.height;

	//���Ҿ�����ʾ
	if (ldRetRect.right < (ndSizeSrc.width - 1.0f))
	{
		float lfOffset = (ndSizeSrc.width - ldRetRect.right) / 2.0f;
		ldRetRect.left += lfOffset;
		ldRetRect.right += lfOffset;
	}

	//���¾�����ʾ
	if (ldRetRect.bottom < (ndSizeSrc.height - 1.0f))
	{
		float lfOffset = (ndSizeSrc.height - ldRetRect.bottom) / 2.0f;
		ldRetRect.top += lfOffset;
		ldRetRect.bottom += lfOffset;
	}

	return ldRetRect;
}

//����ͼƬ��С��λ��
void CThumbnailListItem::RelocationItem(void)
{
	if (mpIterBtClick != NULL)
	{
		mpIterPageNumber->SetPosition(mpIterator->GetSizeX() - mpIterPageNumber->GetSizeX() - 20.0f, mpIterator->GetSizeY() - mpIterPageNumber->GetSizeY() - 20.0f);

		D2D1_RECT_F ldRect = GetProportionalScaling(D2D1::SizeF(mpIterator->GetSizeX() - 4.0f, mpIterator->GetSizeY() - 4.0f),
			D2D1::SizeF(mpIterPicture->GetSizeX(), mpIterPicture->GetSizeY()));;

		mpIterPicture->SetPosition(ldRect.left + 2.0f, ldRect.top + 2.0f);
		mpIterPicture->SetSize(ldRect.right - ldRect.left, ldRect.bottom - ldRect.top);
	}
}

//Ԫ�زο��ߴ緢���仯
ERESULT CThumbnailListItem::OnElementResized(D2D1_SIZE_F nNewSize)
{
	if (mpIterBtClick != NULL)
	{
		CExMessage::SendMessage(mpIterBtClick, mpIterator, EACT_BUTTON_SET_ACTION_RECT, nNewSize);

		
		RelocationItem();
	}
	
	////mpIterLineOne->SetSize(nNewSize.width, mpIterLineOne->GetSize().height);

	//

	return ERESULT_SUCCESS;
}


//֪ͨԪ�ء���ʾ/���ء������ı�
ERESULT CThumbnailListItem::OnElementShow(bool nbIsShow)
{
	//EiSetHomebarStatus(nbIsShow == false ? GI_HOMEBAR_SHOW : GI_HOMEBAR_HIDE);

	return ERESULT_SUCCESS;
}