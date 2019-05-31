/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#include "stdafx.h"
#include "cmmBaseObj.h"
#include "Einkui.h"
#include "XUIx.h"
#include "ElementImp.h"
#include "XCtl.h"
#include "EvSliderButtonImp.h"


//using namespace D2D1;

DEFINE_BUILTIN_NAME(SliderButton)

CEvSliderButton::CEvSliderButton()
{
	mnMoveStyle = ES_SLIDER_BUTTON_STYLE_HOR;

}
CEvSliderButton::~CEvSliderButton()
{
	//if(mpBarButton)
	//	mpBarButton->Release();
}


ULONG CEvSliderButton::InitOnCreate(
	IN IEinkuiIterator* npParent,	// ������ָ��
	IN ICfKey* npTemplete,		// npTemplete��Key ID����EID��ֵ��������EType
	IN ULONG nuEID	// �����Ϊ0��MAXULONG32����ָ����Ԫ�ص�EID; ����ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��XUIϵͳ�Զ�����
	)
{
	ERESULT leResult = ERESULT_UNSUCCESSFUL;

	do 
	{
		//���ȵ��û���
		leResult = 	CEvButton::InitOnCreate(npParent,npTemplete,nuEID);
		if(leResult != ERESULT_SUCCESS)
			break;

		mpIterator->ModifyStyles(EITR_STYLE_DRAG);
		leResult = ERESULT_SUCCESS;

	} while (false);
	return leResult;
}
// �ֽ���Ϣ���ṩ��Ϣ�ֽ����Ϣ��Ӧ�Ĺ��ܣ������ʵ���ǽ���Ϣ�ֽ�Ϊ��ͬ������󣬵�����Ӧ�Ĵ����麯�������ڲ���ʶ����Ϣ��һ�ɷ���ERESULT_UNEXPECTED_MESSAGE
// �������ķ���ֵ���Զ�ͬ�����õ�npMsgָ�����Ϣ������
ERESULT CEvSliderButton::ParseMessage(IEinkuiMessage* npMsg)
{
	// ʵ��ԭ�����ȵ�������ķֽ⹦�ܣ����󽫲��������Ϣ��������
	ERESULT luResult = ERESULT_UNEXPECTED_MESSAGE;
	switch (npMsg->GetMessageID())
	{
	case EACT_SLIDERBUTTON_SET_STYLE:
		{
			if(npMsg->GetInputDataSize() != sizeof(LONG))
			{
				break;
			}
			LONG * lpValue = (LONG*)npMsg->GetInputData();
			mnMoveStyle = * lpValue;
			luResult = ERESULT_SUCCESS;
			break;
		}
	case EACT_SLIDERBUTTON_SET_SLIDERRECT:
		{
			if(npMsg->GetInputDataSize() != sizeof(D2D1_RECT_F))
			{
				break;
			}
			D2D1_RECT_F * lpRect = (D2D1_RECT_F*)npMsg->GetInputData();
			if(lpRect)
			{
				RtlCopyMemory(&mRectSlider,lpRect,sizeof(D2D1_RECT_F));
				luResult = ERESULT_SUCCESS;
			}
			break;
		}
	default:
		luResult = ERESULT_NOT_SET;
		break;
	}
	if(luResult == ERESULT_NOT_SET)		
		luResult = CEvButton::ParseMessage(npMsg);

	return luResult;
}
//Ԫ����ק
ERESULT CEvSliderButton::OnDragging(const STMS_DRAGGING_ELE* npInfo)
{
	//
	if(mnMoveStyle == ES_SLIDER_BUTTON_STYLE_VER)
	{		
		FLOAT lfy = npInfo->Offset.y + mDragStartPoint.y;		
		//����λ�ã����ܳ�����Χ
		if(lfy < mRectSlider.top)
			lfy = mRectSlider.top;
		FLOAT lfMaxY = mRectSlider.top + ((mRectSlider.bottom - mRectSlider.top) - mpIterator->GetSizeY());
		if(lfy > lfMaxY)
			lfy = lfMaxY;

		mpIterator->SetPosition(mpIterator->GetPositionX(),lfy);
	}
	else if(mnMoveStyle == ES_SLIDER_BUTTON_STYLE_HOR)
	{
		FLOAT lfx = npInfo->Offset.x + mDragStartPoint.x;		
		//����λ�ã����ܳ�����Χ
		if(lfx < mRectSlider.left)
			lfx = mRectSlider.left;
		//int n = mpIterator->GetSizeX();
		FLOAT lfMaxX = mRectSlider.left + ((mRectSlider.right - mRectSlider.left) - mpIterator->GetSizeX());
		if(lfx > lfMaxX)
			lfx =  lfMaxX;
		mpIterator->SetPosition(lfx,mpIterator->GetPositionY());
	}
	else if(mnMoveStyle == ES_SLIDER_BUTTON_STYLE_ANYWAY)
	{
		FLOAT lfy = npInfo->Offset.y + mDragStartPoint.y;		
		//����λ�ã����ܳ���Y��Χ
		if(lfy < mRectSlider.top)
			lfy = mRectSlider.top;
		if(lfy > ((mRectSlider.bottom - mRectSlider.top) - mpIterator->GetSizeY()) )
			lfy = ((mRectSlider.bottom - mRectSlider.top) - mpIterator->GetSizeY());

		FLOAT lfx = npInfo->Offset.x + mDragStartPoint.x;		
		//����λ�ã����ܳ���X��Χ
		if(lfx < mRectSlider.left)
			lfx = mRectSlider.left;
		if(lfx > ((mRectSlider.right - mRectSlider.left) - mpIterator->GetSizeX()) )
			lfx = ((mRectSlider.right - mRectSlider.left) - mpIterator->GetSizeX());

		mpIterator->SetPosition(lfx,lfy);
	}
	
	//
// 	EinkuiGetSystem()->GetElementManager()->SimpleSendMessage(mpIterator->GetParent(),
// 		EACT_SLIDERBUTTON_DRAGING,
// 		(LPVOID)npInfo,
// 		sizeof(STMS_DRAGGING_ELE),NULL,0);

	CExMessage::PostMessage(mpIterator->GetParent(),mpIterator,EACT_SLIDERBUTTON_DRAGING,npInfo,EMSG_POSTTYPE_REDUCE);

	return ERESULT_SUCCESS;
}

//��ק��ʼ
ERESULT CEvSliderButton::OnDragBegin(const STMS_DRAGGING_ELE* npInfo)
{
	mDragStartPoint.x = mpIterator->GetPositionX();
	mDragStartPoint.y = mpIterator->GetPositionY();

	CExMessage::SendMessage(mpIterator->GetParent(),mpIterator,EACT_SLIDERBUTTON_DRAG_START,CExMessage::DataInvalid);
	return ERESULT_SUCCESS;
}

//��ק����
ERESULT CEvSliderButton::OnDragEnd(const STMS_DRAGGING_ELE* npInfo)
{
	CExMessage::SendMessage(mpIterator->GetParent(),mpIterator,EACT_SLIDERBUTTON_DRAG_END,CExMessage::DataInvalid);
	return ERESULT_SUCCESS;
}

//Ԫ��Enbale
ERESULT CEvSliderButton::OnElementEnable(bool nbIsEnable)
{
	if (nbIsEnable)
	{
		if (mpIterator) mpIterator->ModifyStyles(EITR_STYLE_MOUSE, NULL);
		mpIterator->SetAlpha(1.0f);
	}
	else
	{
		if (mpIterator) mpIterator->ModifyStyles(NULL, EITR_STYLE_MOUSE);
		mpIterator->SetAlpha(1.0f);
	}

	return ERESULT_SUCCESS;
}
