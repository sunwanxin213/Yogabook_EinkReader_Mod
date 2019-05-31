/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#include "stdafx.h"
#include "cmmBaseObj.h"
#include "Einkui.h"
#include "XCtl.h"
#include "ElementImp.h"
#include "EvButtonImp.h"

#include "EvPpButtonImp.h"

DEFINE_BUILTIN_NAME(PingpongButton)

CEvPingpongButton::CEvPingpongButton()
{
	miDirection = -1;
}

CEvPingpongButton::~CEvPingpongButton()
{

}



ULONG CEvPingpongButton::InitOnCreate( 
	IN IEinkuiIterator* npParent, 
	IN ICfKey* npTemplete, 
	IN ULONG nuEID/* =MAXULONG32 */ 
	)
{
	ERESULT leResult = ERESULT_UNSUCCESSFUL;

	do 
	{

		// �ȵ��û��ຯ��
		leResult = CEvButton::InitOnCreate(npParent, npTemplete, nuEID);
		if(leResult != ERESULT_SUCCESS)
			break;

		BREAK_ON_NULL(npTemplete);
		// ͨ���������ȡ��ť���򣬼���ֱ��ť����ˮƽ��ť
		miDirection = npTemplete->QuerySubKeyValueAsLONG(L"Direction", 0);

		CXuiElement::mhInnerCursor = LoadCursor(NULL,IDC_HAND);

	} while (false);

	return leResult;
}


//Ԫ����ק
ERESULT CEvPingpongButton::OnDragging(const STMS_DRAGGING_ELE* npInfo)
{
	ERESULT leResult = ERESULT_UNSUCCESSFUL;

	do 
	{
		if(MOUSE_LB(npInfo->ActKey) == false)  //��������������Ͳ�����
			break;

		// ˮƽ��ť
		if (miDirection == 1)
		{
			// ���������ƶ�,ƫ����Ϊ��ֵ����ƫ����>(��ť���/2)ʱ��״̬��λ
			if(npInfo->Offset.x > (CEvButton::GetIterator()->GetSize().width/2))
				CEvButton::SetChecked(true);

			// �������� ƫ����Ϊ��ֵ
			if(npInfo->Offset.x < -(CEvButton::GetIterator()->GetSize().width/2))
				CEvButton::SetChecked(false);
		}
		// ��ֱ��ť
		else if (miDirection == 2)
		{
			// ���������ƶ���ƫ����Ϊ��ֵ
			if (npInfo->Offset.y > (CEvButton::GetIterator()->GetSize().height/2))
				CEvButton::SetChecked(false);


			// ���������ƶ���ƫ����Ϊ��ֵ
			if (npInfo->Offset.y < -(CEvButton::GetIterator()->GetSize().height/2))
				CEvButton::SetChecked(true);

		}

		leResult = ERESULT_SUCCESS;

	} while (false);


			 
	return leResult;
}

//��ק��ʼ,nulActKey�ĸ���갴ť���½�����ק
ERESULT CEvPingpongButton::OnDragBegin(const STMS_DRAGGING_ELE* npInfo)
{
	ERESULT leResult = ERESULT_UNSUCCESSFUL;

	do 
	{
		if(MOUSE_LB(npInfo->ActKey) == false)  //��������������Ͳ�����
			break;

		if(mpIterator->IsEnable() == false)
			break;	//����ǽ���״̬�Ͳ���Ӧ

		leResult = ERESULT_SUCCESS;

	} while (false);

	return leResult;
}

//��ק����,nulActKey�ĸ���갴ť���½�����ק
ERESULT CEvPingpongButton::OnDragEnd(const STMS_DRAGGING_ELE* npInfo)
{
	ERESULT leResult = ERESULT_UNSUCCESSFUL;

	do 
	{
		if(MOUSE_LB(npInfo->ActKey) == false)  //��������������Ͳ�����
			break;

		if(npInfo->CurrentPos.x < 0 || npInfo->CurrentPos.y < 0 || npInfo->CurrentPos.x > mpIterator->GetSizeX() || npInfo->CurrentPos.y > mpIterator->GetSizeY())
			PostMessageToParent(EEVT_BUTTON_CLICK,CExMessage::DataInvalid); //����һ�ε���,������Button�ͻᷢ�����Ϣ�ˣ�����Ͳ���Ҫ����

		leResult = ERESULT_SUCCESS;

	} while (false);

	return leResult;
}
