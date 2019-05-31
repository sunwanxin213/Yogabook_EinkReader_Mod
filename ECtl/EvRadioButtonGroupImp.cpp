/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#include "stdafx.h"
#include "cmmBaseObj.h"
#include "cmmstruct.h"
#include "Einkui.h"
#include "XCtl.h"
#include "ElementImp.h"
#include "EvButtonImp.h"
#include "EvCheckButtonImp.h"
#include "EvRadioButtonGroupImp.h"


//using namespace D2D1;
DEFINE_BUILTIN_NAME(RadioButtonGroup)

// ֻ���ڱ������ó�ʼֵ����ָ����ΪNULL�����п���ʧ�ܵ������֮������㶼Ӧ����InitOnCreate�н���
CEvRadioButtonGroup::CEvRadioButtonGroup()
{
	mpCheckedItem = NULL;
}

// �����ͷų�Ա����
CEvRadioButtonGroup::~CEvRadioButtonGroup()
{

}

ULONG CEvRadioButtonGroup::InitOnCreate(
	IN IEinkuiIterator* npParent,	// ������ָ��
	IN ICfKey* npTemplete,		// npTemplete��Key ID����EID��ֵ��������EType
	IN ULONG nuEID	// �����Ϊ0��MAXULONG32����ָ����Ԫ�ص�EID; ����ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��XUIϵͳ�Զ�����
	)
{
	ERESULT leResult = ERESULT_UNSUCCESSFUL;

	do 
	{
		//���ȵ��û���
		leResult = 	CXuiElement::InitOnCreate(npParent,npTemplete,nuEID);
		if(leResult != ERESULT_SUCCESS)
			break;

		//�����Լ�������
		mpIterator->ModifyStyles(/*EITR_STYLE_CONTROL|*/EITR_STYLE_DRAG);



		leResult = ERESULT_SUCCESS;

	} while (false);

	return leResult;
}

//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
ERESULT CEvRadioButtonGroup::OnElementCreate(IEinkuiIterator* npIterator)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		if(CXuiElement::OnElementCreate(npIterator) != ERESULT_SUCCESS)
			break;

		//��һ����û��Ĭ��ѡ�еģ����û�У���ѡ�е�һ��
		bool lbChecked = false;
		for (int i=0;i<mpIterator->GetSubElementCount();i++)
		{
			if(CExMessage::SendMessage(mpIterator->GetSubElementByZOder(i),mpIterator,EACT_BUTTON_GET_CHECKED,CExMessage::DataInvalid,&lbChecked,sizeof(bool)) != ERESULT_SUCCESS)
				continue;	//����CheckButton

			if(lbChecked != false)
			{
				if(mpCheckedItem == NULL)
				{
					mpCheckedItem = mpIterator->GetSubElementByZOder(i);
				}
				else
				{
					lbChecked = false;	//�Ѿ���ѡ�е��ˣ���ȡ�������ѡ��
					CExMessage::SendMessage(mpIterator->GetSubElementByZOder(i),mpIterator,EACT_BUTTON_SET_CHECKED,lbChecked);
				}
			}
		}

		if(mpCheckedItem == NULL)
		{
			lbChecked = true;	//û��ѡ�еģ���ѡ�е�һ������
			CExMessage::SendMessage(mpIterator->GetSubElementByZOder(0),mpIterator,EACT_BUTTON_SET_CHECKED,lbChecked);
			mpCheckedItem = mpIterator->GetSubElementByZOder(0);
		}

		lResult = ERESULT_SUCCESS;
	}while(false);

	return lResult;
}


// �ֽ���Ϣ���ṩ��Ϣ�ֽ����Ϣ��Ӧ�Ĺ��ܣ������ʵ���ǽ���Ϣ�ֽ�Ϊ��ͬ������󣬵�����Ӧ�Ĵ����麯�������ڲ���ʶ����Ϣ��һ�ɷ���ERESULT_UNEXPECTED_MESSAGE
// �������ķ���ֵ���Զ�ͬ�����õ�npMsgָ�����Ϣ������
ERESULT CEvRadioButtonGroup::ParseMessage(IEinkuiMessage* npMsg)
{
	// ʵ��ԭ�����ȵ�������ķֽ⹦�ܣ����󽫲��������Ϣ��������

	ERESULT luResult = ERESULT_UNEXPECTED_MESSAGE;
	ULONG lulId = 0;
	IEinkuiIterator* lpItem = NULL;

	switch (npMsg->GetMessageID())
	{
	case EEVT_BUTTON_CLICK:
		{
			//������һ�����������һ�ε���
			OnItemClick(npMsg->GetMessageSender());
			luResult = ERESULT_MSG_SENDTO_NEXT;	//����Ϣ�������Ϸ�
			break;
		}
	case EACT_RBG_SET_SELECT:
		{
			//Ҫ�����ѡ�ж���
			bool lbChecked = false;
			luResult = CExMessage::GetInputData(npMsg,lulId);
			if(luResult != ERESULT_SUCCESS)
				break;

			//ȡ��ѡ��
			if(mpCheckedItem != NULL)
				CExMessage::SendMessage(mpCheckedItem,mpIterator,EACT_BUTTON_SET_CHECKED,lbChecked);

			mpCheckedItem = mpIterator->GetSubElementByID(lulId);
			lbChecked = true;
			if(mpCheckedItem != NULL)	//ѡ��
				CExMessage::SendMessage(mpCheckedItem,mpIterator,EACT_BUTTON_SET_CHECKED,lbChecked);

			EinkuiGetSystem()->UpdateView(true);

			luResult = ERESULT_SUCCESS;

			break;
		}
	case EACT_RBG_GET_SELECT:
		{
			//��ȡѡ�ж���ID
			if(npMsg->GetOutputBufferSize() != sizeof(ULONG))
				break;

			ULONG* lpWarp = (ULONG*)npMsg->GetOutputBuffer();
			
			if (mpCheckedItem != NULL)
			{
				*lpWarp = mpCheckedItem->GetID();
				npMsg->SetOutputDataSize(sizeof(ULONG));
				luResult = ERESULT_SUCCESS;
			}
			else
			{
				luResult = ERESULT_UNSUCCESSFUL;
			}
			

			break;
		}
	case EACT_RBG_DISABLE:
		{
			//����ĳ�����
			luResult = CExMessage::GetInputData(npMsg,lulId);
			if(luResult != ERESULT_SUCCESS)
				break;

			lpItem = mpIterator->GetSubElementByID(lulId);
			BREAK_ON_NULL(lpItem);
			lpItem->SetEnable(false);
			EinkuiGetSystem()->UpdateView(true);
			luResult = ERESULT_SUCCESS;
			
			break;
		}
	case EACT_RBG_ENABLE:
		{
			//����ĳ������
			luResult = CExMessage::GetInputData(npMsg,lulId);
			if(luResult != ERESULT_SUCCESS)
				break;

			lpItem = mpIterator->GetSubElementByID(lulId);
			BREAK_ON_NULL(lpItem);
			lpItem->SetEnable(true);
			EinkuiGetSystem()->UpdateView(true);
			luResult = ERESULT_SUCCESS;

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

//��һ�������һ�ε���
ERESULT CEvRadioButtonGroup::OnItemClick(IEinkuiIterator* npItem)
{
	ERESULT luResult = ERESULT_UNSUCCESSFUL;
	bool lbChecked = false;

	do 
	{
// 		if(mArrayGroup.Size() <= 0)
// 			break;	//û�ж���

		

		if(npItem == mpCheckedItem)
		{
			lbChecked = true;
			CExMessage::SendMessage(mpCheckedItem,mpIterator,EACT_BUTTON_SET_CHECKED,lbChecked);

			PostMessageToParent(EEVT_RBG_SELECTED_ITEM_CLICK, npItem->GetID());
			break;	//������ѡ����
		}
		//ȡ��ѡ��
		if(mpCheckedItem != NULL)
			CExMessage::SendMessage(mpCheckedItem,mpIterator,EACT_BUTTON_SET_CHECKED,lbChecked);

		//�ı�ѡ�м�¼
		mpCheckedItem = npItem;

		PostMessageToParent(EEVT_RBG_SELECTED_CHANGED,mpCheckedItem->GetID());

		luResult = ERESULT_SUCCESS;

	} while (false);

	return luResult;
}

//���û�����
ERESULT CEvRadioButtonGroup::OnElementEnable(bool nbIsEnable)
{
	for (int i=0;i<mpIterator->GetSubElementCount();i++)
	{
		mpIterator->GetSubElementByZOder(i)->SetEnable(nbIsEnable);
	}

	return ERESULT_SUCCESS;
}