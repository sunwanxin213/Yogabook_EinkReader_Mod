


#include "stdafx.h"
#include "cmmstruct.h"
#include "Einkui.h"
#include "ElementImp.h"
#include "XCtl.h"
#include "EvToolPane.h"

#pragma warning (disable:4995) 


DEFINE_BUILTIN_NAME(ToolPane)

#define ID_TOOLBAR_ITEM_FONT		1		//	���� ����Ͽ�
#define ID_TOOLBAR_ITEM_FONT_SIZE	2		//	�ֺ�	����Ͽ�
#define ID_TOOLBAR_ITEM_SPACING		4		//	���	����Ͽ�

// ֻ���ڱ������ó�ʼֵ����ָ����ΪNULL�����п���ʧ�ܵ������֮������㶼Ӧ����InitOnCreate�н���
CEvToolPane::CEvToolPane() :
	mpUnificSetting(NULL)
{	
}

CEvToolPane::~CEvToolPane() 
{
}

ULONG CEvToolPane::InitOnCreate(
	IN IXuiIterator* npParent,	// ������ָ��
	IN ICfKey* npTemplete,		// npTemplete��Key ID����EID��ֵ��������EType
	IN ULONG nuEID				// �����Ϊ0��MAXULONG32����ָ����Ԫ�ص�EID; ����ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��XUIϵͳ�Զ�����
	)
{
	ERESULT leResult = ERESULT_UNSUCCESSFUL;

	do 
	{
		//���ȵ��û���
		leResult = 	CXuiElement::InitOnCreate(npParent,npTemplete,nuEID);
		if(leResult != ERESULT_SUCCESS)
			break;

		//	��ʼ��UnificSetting
		mpUnificSetting = GetUnificSetting();

		//mpIterator->ModifyStyles(EITR_STYLE_ALL_MWHEEL|EITR_STYLE_KEYBOARD);
		leResult = ERESULT_SUCCESS;

	} while (false);

	return leResult;
}

//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
ERESULT CEvToolPane::OnElementCreate(IXuiIterator* npIterator)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		if(CXuiElement::OnElementCreate(npIterator) != ERESULT_SUCCESS)
			break;

		//LoadResource();
		mpIterator->ModifyStyles(EITR_STYLE_POPUP);
		lResult = ERESULT_SUCCESS;
	} while(false);

	return lResult;
}

ERESULT CEvToolPane::OnCtlButtonClick(IXuiIterator* npSender)
{
	ULONG luCtrlId = npSender->GetID();

	ERESULT luResult = ERESULT_SUCCESS;

	/*switch (luCtrlId)
	{


	}*/

	return luResult;
}


//װ��������Դ
//ERESULT CEvToolPane::LoadResource()
//{
//
//	ERESULT leResult = ERESULT_UNSUCCESSFUL;
//
//	ICfKey* lpValue = NULL;
//
//	do 
//	{
//		//��ȡ֡��Ϣ
//		mlMaxFrame = mpTemplete->QuerySubKeyValueAsLONG(TF_ID_LIST_BACKIAMGE_FRAME_COUNT,1);
//
//		if (mlMaxFrame > 0 && mpBgBitmap != NULL)
//		{
//			//����ÿ֡��С
//			UINT luiWidth = mpBgBitmap->GetWidth();
//			UINT luiHeight = mpBgBitmap->GetHeight();
//			mlCurrentIndex = 0;
//
//			mpIterator->SetSize(float(luiWidth / mlMaxFrame),(float)luiHeight);
//		}
//		//��ȡ��ʾģʽ
//
//		//��ȡ�Ƿ���Ҫ��ʾ������������
//
//		leResult = ERESULT_SUCCESS;
//
//	} while (false);
//
//	return leResult;
//}

//����
//ERESULT CEvToolPane::OnPaint(IXuiPaintBoard* npPaintBoard)
//{
//
//	ERESULT lResult = ERESULT_UNSUCCESSFUL;
//
//	do
//	{
//		BREAK_ON_NULL(npPaintBoard);
//
//		if (mpBgBitmap != NULL)
//		{
//			float lfX = mpIterator->GetSizeX() * mlCurrentIndex;
//
//			npPaintBoard->DrawBitmap(D2D1::RectF(0,0,mpIterator->GetSizeX(),mpIterator->GetSizeY()),
//				D2D1::RectF(lfX,0,lfX + mpIterator->GetSizeX(),mpIterator->GetSizeY()),
//				mpBgBitmap,
//				0
//				);
//		}
//		 
//	}while(false);
//
//	return lResult;
//}

// �ֽ���Ϣ���ṩ��Ϣ�ֽ����Ϣ��Ӧ�Ĺ��ܣ������ʵ���ǽ���Ϣ�ֽ�Ϊ��ͬ������󣬵�����Ӧ�Ĵ����麯�������ڲ���ʶ����Ϣ��һ�ɷ���ERESULT_UNEXPECTED_MESSAGE
// �������ķ���ֵ���Զ�ͬ�����õ�npMsgָ�����Ϣ������
ERESULT CEvToolPane::ParseMessage(IXuiMessage* npMsg)
{
	// ʵ��ԭ�����ȵ�������ķֽ⹦�ܣ����󽫲��������Ϣ��������

	ERESULT luResult = ERESULT_UNEXPECTED_MESSAGE;

	do 
	{
		BREAK_ON_NULL(npMsg);

		switch (npMsg->GetMessageID())
		{
		//case EEVT_COMBOBOX_LIST_ITEM_CLICK_COMPLEX:
		//	{
		//		//TOOLBAR_MSG MsgInfo = *(TOOLBAR_MSG*)(npMsg->GetInputData());
		//
		//		//EinkuiGetSystem()->GetElementManager()->SimpleSendMessage(
		//		//	mpIterator->GetParent(), EEVT_TOOLBARITEM_CLICK, &MsgInfo, sizeof(TOOLBAR_MSG)
		//		//	, NULL, 0);
		//		TOOLBAR_MSG* lpMsgInfo;

		//		luResult = CExMessage::GetInputDataBuffer(npMsg,lpMsgInfo);
		//		if(luResult != ERESULT_SUCCESS)
		//			break;
		//		CExMessage::PostMessage(mpIterator->GetParent(),mpIterator,EEVT_TOOLBARITEM_CLICK,*lpMsgInfo,EMSG_POSTTYPE_FAST);
		//	}
		//	break;

		case EEVT_UPDATE_PANE:
			{
				int* value = (int*)npMsg->GetInputData();
				BREAK_ON_NULL(value);

				SetPaneItemValue(*value);
			}
			break;

		case EEVT_IMAGEBUTTON_CLICK:
			break;
		
		default:
			luResult = ERESULT_NOT_SET;
			break;
		}

		if(luResult == ERESULT_NOT_SET)
		{
			luResult = CXuiElement::ParseMessage(npMsg); // ���û����ͬ��������ע�⣺һ��Ҫ��������ֱ�ӻ���
		}

	} while (false);

	return luResult;
}

bool CEvToolPane::SetPaneItemValue(int lID)
{
	//	���������ļ�����Ԫ��ID

	if (NULL == mpUnificSetting) return false;

	IXuiIterator* lpItem = NULL;

	int i = 1;
	bool lbDirty = false;
	
	while(lpItem = mpIterator->GetSubElementByID(i++))
	{
		int lnID = 0;

		//ȡID
		EinkuiGetSystem()->GetElementManager()->SimpleSendMessage(
			lpItem, EEVT_GET_UNIFIC_SETTING_ID, NULL, 0, &lnID, sizeof(int));

		if (0 !=lnID)
		{
			//�ж�dirty
			if (lbDirty = mpUnificSetting->GetDirty(lnID))
			{
				EinkuiGetSystem()->GetElementManager()->SimpleSendMessage(
					lpItem, EEVT_PANE_ITEM_SET_VALUE, NULL, 0, NULL, 0);

				//����޸ı��
				mpUnificSetting->SetDirty(lnID, false);
			}
		}
	}

	/*switch (lID)
	{
	case 0x00200100:
	{
	if (lbDirty = mpUnificSetting->GetDirty(0x00200101))
	EinkuiGetSystem()->GetElementManager()->SimpleSendMessage(
	mpIterator->GetSubElementByID(1), EEVT_PANE_ITEM_SET_VALUE, NULL, 0, NULL, 0);
	if (lbDirty = mpUnificSetting->GetDirty(0x00200102))
	EinkuiGetSystem()->GetElementManager()->SimpleSendMessage(
	mpIterator->GetSubElementByID(2), EEVT_PANE_ITEM_SET_VALUE, NULL, 0, NULL, 0);
	if (lbDirty = mpUnificSetting->GetDirty(0x00200103))
	EinkuiGetSystem()->GetElementManager()->SimpleSendMessage(
	mpIterator->GetSubElementByID(3), EEVT_PANE_ITEM_SET_VALUE, NULL, 0, NULL, 0);
	if (lbDirty = mpUnificSetting->GetDirty(0x00200104))
	EinkuiGetSystem()->GetElementManager()->SimpleSendMessage(
	mpIterator->GetSubElementByID(4), EEVT_PANE_ITEM_SET_VALUE, NULL, 0, NULL, 0);


	}
	break;

	case 0x00200200:
	{
	if (lbDirty = mpUnificSetting->GetDirty(0x00200201))
	EinkuiGetSystem()->GetElementManager()->SimpleSendMessage(
	mpIterator->GetSubElementByID(1), EEVT_PANE_ITEM_SET_VALUE, NULL, 0, NULL, 0);
	if (lbDirty = mpUnificSetting->GetDirty(0x00200202))
	EinkuiGetSystem()->GetElementManager()->SimpleSendMessage(
	mpIterator->GetSubElementByID(2), EEVT_PANE_ITEM_SET_VALUE, NULL, 0, NULL, 0);
	if (lbDirty = mpUnificSetting->GetDirty(0x00200203))
	EinkuiGetSystem()->GetElementManager()->SimpleSendMessage(
	mpIterator->GetSubElementByID(3), EEVT_PANE_ITEM_SET_VALUE, NULL, 0, NULL, 0);
	if (lbDirty = mpUnificSetting->GetDirty(0x00200204))
	EinkuiGetSystem()->GetElementManager()->SimpleSendMessage(
	mpIterator->GetSubElementByID(4), EEVT_PANE_ITEM_SET_VALUE, NULL, 0, NULL, 0);
	if (lbDirty = mpUnificSetting->GetDirty(0x00200205))
	EinkuiGetSystem()->GetElementManager()->SimpleSendMessage(
	mpIterator->GetSubElementByID(5), EEVT_PANE_ITEM_SET_VALUE, NULL, 0, NULL, 0);
	if (lbDirty = mpUnificSetting->GetDirty(0x00200206))
	EinkuiGetSystem()->GetElementManager()->SimpleSendMessage(
	mpIterator->GetSubElementByID(6), EEVT_PANE_ITEM_SET_VALUE, NULL, 0, NULL, 0);
	if (lbDirty = mpUnificSetting->GetDirty(0x00200207))
	EinkuiGetSystem()->GetElementManager()->SimpleSendMessage(
	mpIterator->GetSubElementByID(7), EEVT_PANE_ITEM_SET_VALUE, NULL, 0, NULL, 0);
	if (lbDirty = mpUnificSetting->GetDirty(0x00200208))
	EinkuiGetSystem()->GetElementManager()->SimpleSendMessage(
	mpIterator->GetSubElementByID(8), EEVT_PANE_ITEM_SET_VALUE, NULL, 0, NULL, 0);
	}
	break;

	case 0x00200300:
	{
	if (lbDirty = mpUnificSetting->GetDirty(0x00200301))
	EinkuiGetSystem()->GetElementManager()->SimpleSendMessage(
	mpIterator->GetSubElementByID(1), EEVT_PANE_ITEM_SET_VALUE, NULL, 0, NULL, 0);
	if (lbDirty = mpUnificSetting->GetDirty(0x00200302))
	EinkuiGetSystem()->GetElementManager()->SimpleSendMessage(
	mpIterator->GetSubElementByID(2), EEVT_PANE_ITEM_SET_VALUE, NULL, 0, NULL, 0);
	if (lbDirty = mpUnificSetting->GetDirty(0x00200303))
	EinkuiGetSystem()->GetElementManager()->SimpleSendMessage(
	mpIterator->GetSubElementByID(3), EEVT_PANE_ITEM_SET_VALUE, NULL, 0, NULL, 0);
	if (lbDirty = mpUnificSetting->GetDirty(0x00200304))
	EinkuiGetSystem()->GetElementManager()->SimpleSendMessage(
	mpIterator->GetSubElementByID(4), EEVT_PANE_ITEM_SET_VALUE, NULL, 0, NULL, 0);
	if (lbDirty = mpUnificSetting->GetDirty(0x00200305))
	EinkuiGetSystem()->GetElementManager()->SimpleSendMessage(
	mpIterator->GetSubElementByID(5), EEVT_PANE_ITEM_SET_VALUE, NULL, 0, NULL, 0);
	if (lbDirty = mpUnificSetting->GetDirty(0x00200306))
	EinkuiGetSystem()->GetElementManager()->SimpleSendMessage(
	mpIterator->GetSubElementByID(6), EEVT_PANE_ITEM_SET_VALUE, NULL, 0, NULL, 0);
	if (lbDirty = mpUnificSetting->GetDirty(0x00200307))
	EinkuiGetSystem()->GetElementManager()->SimpleSendMessage(
	mpIterator->GetSubElementByID(7), EEVT_PANE_ITEM_SET_VALUE, NULL, 0, NULL, 0);
	if (lbDirty = mpUnificSetting->GetDirty(0x00200308))
	EinkuiGetSystem()->GetElementManager()->SimpleSendMessage(
	mpIterator->GetSubElementByID(8), EEVT_PANE_ITEM_SET_VALUE, NULL, 0, NULL, 0);
	if (lbDirty = mpUnificSetting->GetDirty(0x00200309))
	EinkuiGetSystem()->GetElementManager()->SimpleSendMessage(
	mpIterator->GetSubElementByID(9), EEVT_PANE_ITEM_SET_VALUE, NULL, 0, NULL, 0);
	if (lbDirty = mpUnificSetting->GetDirty(0x0020030a))
	EinkuiGetSystem()->GetElementManager()->SimpleSendMessage(
	mpIterator->GetSubElementByID(10), EEVT_PANE_ITEM_SET_VALUE, NULL, 0, NULL, 0);
	if (lbDirty = mpUnificSetting->GetDirty(0x0020030b))
	EinkuiGetSystem()->GetElementManager()->SimpleSendMessage(
	mpIterator->GetSubElementByID(11), EEVT_PANE_ITEM_SET_VALUE, NULL, 0, NULL, 0);
	if (lbDirty = mpUnificSetting->GetDirty(0x0020030c))
	EinkuiGetSystem()->GetElementManager()->SimpleSendMessage(
	mpIterator->GetSubElementByID(12), EEVT_PANE_ITEM_SET_VALUE, NULL, 0, NULL, 0);
	}
	break;

	case 0x00200400:
	{
	if (lbDirty = mpUnificSetting->GetDirty(0x00200401))
	EinkuiGetSystem()->GetElementManager()->SimpleSendMessage(
	mpIterator->GetSubElementByID(1), EEVT_PANE_ITEM_SET_VALUE, NULL, 0, NULL, 0);
	if (lbDirty = mpUnificSetting->GetDirty(0x00200402))
	EinkuiGetSystem()->GetElementManager()->SimpleSendMessage(
	mpIterator->GetSubElementByID(2), EEVT_PANE_ITEM_SET_VALUE, NULL, 0, NULL, 0);
	if (lbDirty = mpUnificSetting->GetDirty(0x00200403))
	EinkuiGetSystem()->GetElementManager()->SimpleSendMessage(
	mpIterator->GetSubElementByID(3), EEVT_PANE_ITEM_SET_VALUE, NULL, 0, NULL, 0);
	if (lbDirty = mpUnificSetting->GetDirty(0x00200404))
	EinkuiGetSystem()->GetElementManager()->SimpleSendMessage(
	mpIterator->GetSubElementByID(4), EEVT_PANE_ITEM_SET_VALUE, NULL, 0, NULL, 0);
	}
	break;

	case 0x00200500:
	{
	if (lbDirty = mpUnificSetting->GetDirty(0x00200501))
	EinkuiGetSystem()->GetElementManager()->SimpleSendMessage(
	mpIterator->GetSubElementByID(1), EEVT_PANE_ITEM_SET_VALUE, NULL, 0, NULL, 0);
	if (lbDirty = mpUnificSetting->GetDirty(0x00200502))
	EinkuiGetSystem()->GetElementManager()->SimpleSendMessage(
	mpIterator->GetSubElementByID(2), EEVT_PANE_ITEM_SET_VALUE, NULL, 0, NULL, 0);
	}
	break;

	case 0x00200600:
	{
	if (lbDirty = mpUnificSetting->GetDirty(0x00200601))
	EinkuiGetSystem()->GetElementManager()->SimpleSendMessage(
	mpIterator->GetSubElementByID(1), EEVT_PANE_ITEM_SET_VALUE, NULL, 0, NULL, 0);
	if (lbDirty = mpUnificSetting->GetDirty(0x00200602))
	EinkuiGetSystem()->GetElementManager()->SimpleSendMessage(
	mpIterator->GetSubElementByID(2), EEVT_PANE_ITEM_SET_VALUE, NULL, 0, NULL, 0);
	if (lbDirty = mpUnificSetting->GetDirty(0x00200603))
	EinkuiGetSystem()->GetElementManager()->SimpleSendMessage(
	mpIterator->GetSubElementByID(3), EEVT_PANE_ITEM_SET_VALUE, NULL, 0, NULL, 0);
	if (lbDirty = mpUnificSetting->GetDirty(0x00200604))
	EinkuiGetSystem()->GetElementManager()->SimpleSendMessage(
	mpIterator->GetSubElementByID(4), EEVT_PANE_ITEM_SET_VALUE, NULL, 0, NULL, 0);
	if (lbDirty = mpUnificSetting->GetDirty(0x00200605))
	EinkuiGetSystem()->GetElementManager()->SimpleSendMessage(
	mpIterator->GetSubElementByID(5), EEVT_PANE_ITEM_SET_VALUE, NULL, 0, NULL, 0);
	if (lbDirty = mpUnificSetting->GetDirty(0x00200606))
	EinkuiGetSystem()->GetElementManager()->SimpleSendMessage(
	mpIterator->GetSubElementByID(6), EEVT_PANE_ITEM_SET_VALUE, NULL, 0, NULL, 0);
	}
	break;
	}*/

	return true;
}