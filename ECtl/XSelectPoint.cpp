/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#include "StdAfx.h"
#include "cmmBaseObj.h"
#include "Einkui.h"
#include "ElementImp.h"
#include "XCtl.h"

#include "xSelectPoint.h"
#include "xSelectFrameMacro.h"

DEFINE_BUILTIN_NAME(System_SelectPoint)

CSelectPoint::CSelectPoint(void)
{
	mfWidth = 80.0f;
	mfHeight = 80.0f;

	mpXuiBrush = NULL;
	mpFillBrush = NULL;
}


CSelectPoint::~CSelectPoint(void)
{
	CMM_SAFE_RELEASE(mpXuiBrush);
}


ULONG CSelectPoint::InitOnCreate(
	IN IEinkuiIterator* npParent,	// ������ָ��
	IN ICfKey* npTemplete,		// npTemplete��Key ID����EID��ֵ��������EType
	IN ULONG nuEID				// �����Ϊ0����ָ����Ԫ�ص�EID������ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��XUIϵͳ�Զ��Ǹ�����
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

		mpXuiBrush = EinkuiGetSystem()->CreateBrush(XuiSolidBrush, D2D1::ColorF(0.0f,0.0f,0.0f));
		BREAK_ON_NULL(mpXuiBrush);
		mpFillBrush = EinkuiGetSystem()->CreateBrush(XuiSolidBrush, D2D1::ColorF(0.0f, 0.0f, 0.0f));
		BREAK_ON_NULL(mpFillBrush);

		mpIterator->SetSize(mfWidth, mfHeight);

		leResult = ERESULT_SUCCESS;

	} while (false);

	CMM_SAFE_RELEASE(lpSubKey);

	// ��ϵͳע����Ҫ�յ�����Ϣ
	return leResult;
}


//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
ERESULT CSelectPoint::OnElementCreate(IEinkuiIterator* npIterator)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;
	ICfKey* lpToolTipKey = NULL;

	do
	{
		if(CXuiElement::OnElementCreate(npIterator) != ERESULT_SUCCESS)
			break;

		// ����Ԫ��ID����Ϊ0
		if(mpIterator->GetID() == 1 || mpIterator->GetID() == 5)
			CXuiElement::mhInnerCursor = LoadCursor(NULL,IDC_SIZENWSE);
		else if(mpIterator->GetID() == 3 || mpIterator->GetID() == 7)
			CXuiElement::mhInnerCursor = LoadCursor(NULL,IDC_SIZENESW);
		else if(mpIterator->GetID() == 2 || mpIterator->GetID() == 6)
			CXuiElement::mhInnerCursor = LoadCursor(NULL,IDC_SIZENS);
		else if(mpIterator->GetID() == 4 || mpIterator->GetID() == 8)
			CXuiElement::mhInnerCursor = LoadCursor(NULL,IDC_SIZEWE);

		mpIterator->ModifyStyles(EITR_STYLE_ALL_DRAG);

		lResult = ERESULT_SUCCESS;

	}while(false);

	return lResult;
}

//�޸������ʽ
void CSelectPoint::SetCursor(HCURSOR nhInnerCursor)
{
// 	if(CXuiElement::mhInnerCursor != NULL)
// 		CloseHandle(CXuiElement::mhInnerCursor);

	CXuiElement::mhInnerCursor = nhInnerCursor;
}

//�ֽ���Ϣ���ṩ��Ϣ�ֽ����Ϣ��Ӧ�Ĺ��ܣ������ʵ���ǽ���Ϣ�ֽ�Ϊ��ͬ������󣬵�����Ӧ�Ĵ����麯�������ڲ���ʶ����Ϣ��һ�ɷ���ERESULT_UNEXPECTED_MESSAGE
//�������ķ���ֵ���Զ�ͬ�����õ�npMsgָ�����Ϣ������
ERESULT CSelectPoint::ParseMessage(IEinkuiMessage* npMsg)
{
	// ʵ��ԭ�����ȵ�������ķֽ⹦�ܣ����󽫲��������Ϣ��������
	ERESULT luResult = ERESULT_UNEXPECTED_MESSAGE;

	switch (npMsg->GetMessageID())
	{
	case EMSG_SELECTPOINT_RESET_CURSOR:
		{
			if(npMsg->GetInputDataSize() != sizeof(HCURSOR))
			{
				luResult = ERESULT_WRONG_PARAMETERS;
				break;
			}

			HCURSOR* lpVoid = (HCURSOR *)npMsg->GetInputData();
			SetCursor(*lpVoid);

			break;
		}
	default:
		luResult = ERESULT_NOT_SET;
		break;
	}

	if(luResult == ERESULT_NOT_SET)
	{
		luResult = CXuiElement::ParseMessage(npMsg); // ���û����ͬ��������ע�⣺һ��Ҫ��������ֱ�ӻ���
	}

	return luResult;
}

// ��������
ERESULT CSelectPoint::OnMouseOwnerTest(const D2D1_POINT_2F& rPoint)
{
	if (mpIterator->IsVisible() == false || mpIterator->IsEnable() == false)
		return ERESULT_SUCCESS; //���ػ����ʱ�����

	if(rPoint.x < 0 || rPoint.x > mpIterator->GetSizeX() || rPoint.y < 0 || rPoint.y > mpIterator->GetSizeY())
		return ERESULT_SUCCESS;
	else
		return ERESULT_MOUSE_OWNERSHIP;
}

//������Ϣ
ERESULT CSelectPoint::OnPaint(IEinkuiPaintBoard* npPaintBoard)
{

	ERESULT luResult = ERESULT_UNSUCCESSFUL;
	
	do 
	{
		BREAK_ON_NULL(mpXuiBrush);
		mpXuiBrush->SetStrokeWidth(1.0f);
		
		//npPaintBoard->FillRect(D2D1::RectF(0.5f, 0.5f, CExFloat::UnderHalf(mfWidth),CExFloat::UnderHalf(mfHeight)), mpFillBrush);
		//npPaintBoard->DrawRect(D2D1::RectF(0.5f, 0.5f, CExFloat::UnderHalf(mfWidth),CExFloat::UnderHalf(mfHeight)), mpXuiBrush);
		npPaintBoard->FillEllipse(D2D1::RectF(25.0f, 25.0f, CExFloat::UnderHalf(55.0f), CExFloat::UnderHalf(55.0f)), mpFillBrush);

		luResult = ERESULT_SUCCESS;

	} while (false);

	return luResult;
}

ERESULT CSelectPoint::OnDragging(const STMS_DRAGGING_ELE* npInfo)
{
	ERESULT leResult = ERESULT_UNSUCCESSFUL;

	do 
	{
		BREAK_ON_NULL(npInfo);
		if ((npInfo->ActKey&MK_LBUTTON) == 0)	//ֻ����������϶�
			break;

		SendMessageToParent(EMSG_SELECTPOINT_MOVING,npInfo->Offset,NULL,0);

		leResult = ERESULT_SUCCESS;

	} while (false);

	return leResult;
}

//��ק��ʼ
ERESULT CSelectPoint::OnDragBegin(const STMS_DRAGGING_ELE* npInfo)
{
	ERESULT leResult = ERESULT_UNSUCCESSFUL;

	do 
	{
		BREAK_ON_NULL(npInfo);
		if ((npInfo->ActKey&MK_LBUTTON) == 0)	//ֻ����������϶�
			break;

		if(npInfo->DragOn != mpIterator) //ֻ�����Լ�����ſ���
			break;

		//֪ͨ�����ڣ���ʼ�϶�
		SendMessageToParent(EMSG_SELECTPOINT_BEGIN,*npInfo,NULL,0);

		leResult = ERESULT_SUCCESS;

	} while (false);

	return leResult;
}

//��ק����
ERESULT CSelectPoint::OnDragEnd(const STMS_DRAGGING_ELE* npInfo)
{
	ERESULT leResult = ERESULT_UNSUCCESSFUL;

	do 
	{
		BREAK_ON_NULL(npInfo);

		if ((npInfo->ActKey&MK_LBUTTON) == 0)	//ֻ����������϶�
			break;

		//֪ͨ�����ڣ��Լ��ƶ����
		SendMessageToParent(EMSG_SELECTFPOINT_MOVED,CExMessage::DataInvalid,NULL,0);

		leResult = ERESULT_SUCCESS;

	} while (false);

	return leResult;
}