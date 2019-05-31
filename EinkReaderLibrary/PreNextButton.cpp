/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#include "StdAfx.h"
#include "PreNextButton.h"
#include "XCtl.h"
#include "stdio.h"
#include "cmmStrHandle.h"
#include "cmmPath.h"
#include "MsgDefine.h"
#include "..\ECtl\EvButtonImp.h"

DEFINE_BUILTIN_NAME(PreNextButton)

CPreNextButton::CPreNextButton(void)
{
	mpIterBtPre = NULL;
	mpIterBtMiddle = NULL;
	mpIterBtNext = NULL;
	mfLeftButton = 400.0f;
}


CPreNextButton::~CPreNextButton(void)
{
}

//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
ERESULT CPreNextButton::OnElementCreate(IEinkuiIterator* npIterator)
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

ULONG CPreNextButton::InitOnCreate(
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
		mpIterBtPre = mpIterator->GetSubElementByID(PNB_BT_PRE);
		BREAK_ON_NULL(mpIterBtPre);
		mpIterBtPre->SetEnable(false);

		mpIterBtMiddle = mpIterator->GetSubElementByID(PNB_BT_MIDDLE);
		BREAK_ON_NULL(mpIterBtMiddle);
		mpIterBtMiddle->SetEnable(false);

		mpIterBtNext = mpIterator->GetSubElementByID(PNB_BT_NEXT);
		BREAK_ON_NULL(mpIterBtNext);
		mpIterBtNext->SetEnable(false);

		leResult = ERESULT_SUCCESS;

	} while (false);

	CMM_SAFE_RELEASE(lpSubKey);

	// ��ϵͳע����Ҫ�յ�����Ϣ
	return leResult;
}



// ��������
ERESULT CPreNextButton::OnMouseOwnerTest(const D2D1_POINT_2F& rPoint)
{
	ERESULT luResult = ERESULT_SUCCESS;

	if (!(rPoint.x < 0.0f || rPoint.x >= mpIterator->GetSizeX()
		|| rPoint.y < 0.0f || rPoint.y >= mpIterator->GetSizeY()))
	{
		luResult = ERESULT_MOUSE_OWNERSHIP;
	}

	return luResult;
}

//��갴��
ERESULT CPreNextButton::OnMousePressed(const STEMS_MOUSE_BUTTON* npInfo)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		BREAK_ON_NULL(npInfo);
		if (mpIterator->IsEnable() == false)
			break;	//����ǽ���״̬���Ͳ���������

		if (mpIterator->IsVisible() == false)
			break;	//���������״̬���Ͳ���������

		if (MOUSE_LB(npInfo->ActKey) == false)  //��������������Ͳ�����
			break;

		if (npInfo->Presssed == false)
		{
			//���̧��
			if (abs(mdPressPos.x - npInfo->Position.x) >= 20.0f || abs(mdPressPos.y - npInfo->Position.y) >= 20.0f)
			{
				//�ǻ�������
				if (abs(mdPressPos.x - npInfo->Position.x) >= abs(mdPressPos.y - npInfo->Position.y))	// ���˵���ֱ��
				{
					if ((mdPressPos.x - npInfo->Position.x) >= 50.0f)
					{
						//��һҳ
						PostMessageToParent(EEVT_ER_PRE_NEXT_CLICKED, PNB_BT_NEXT);
					}
					else if ((npInfo->Position.x - mdPressPos.x) >= 50.0f)
					{
						//ǰһҳ
						PostMessageToParent(EEVT_ER_PRE_NEXT_CLICKED, PNB_BT_PRE);
					}
				}
				
			}
			else
			{
				//��Ϊ�ǵ������
				if ((GetTickCount() - mdwClickTicount) > 500)
				{
					//����500ms�㳤����������
					break;
				}

				//�ж�һ�µ������
				if (npInfo->Position.x < mfLeftButton)
				{
					//��һҳ
					PostMessageToParent(EEVT_ER_PRE_NEXT_CLICKED, PNB_BT_PRE);
				}
				else if (npInfo->Position.x > mpIterBtNext->GetPositionX())
				{
					//��һҳ
					PostMessageToParent(EEVT_ER_PRE_NEXT_CLICKED, PNB_BT_NEXT);
				}
				else
				{
					//��ʾ�����ع�����
					PostMessageToParent(EEVT_ER_PRE_NEXT_CLICKED, PNB_BT_MIDDLE);
				}
				
			}
		}
		else
		{
			//��갴��
			mdPressPos = npInfo->Position;
			mdwClickTicount = GetTickCount();
		}

		lResult = ERESULT_SUCCESS;

	} while (false);

	return lResult;
}


//��ť�����¼�
ERESULT CPreNextButton::OnCtlButtonClick(IEinkuiIterator* npSender)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		ULONG llBtnID = npSender->GetID();
		switch (llBtnID)
		{
		case PNB_BT_PRE:
		case PNB_BT_MIDDLE:
		case PNB_BT_NEXT:
		{
			//���ļ��Ի���
			PostMessageToParent(EEVT_ER_PRE_NEXT_CLICKED, llBtnID);

			break;
		}
		default:
			break;
		}


		lResult = ERESULT_SUCCESS;
	} while (false);

	return lResult;
}

//
////��Ϣ������
//ERESULT CPreNextButton::ParseMessage(IEinkuiMessage* npMsg)
//{
//	ERESULT luResult = ERESULT_UNEXPECTED_MESSAGE;
//
//	switch (npMsg->GetMessageID())
//	{
//	case EMSG_MODAL_ENTER:
//	{
//		//// ����Ҫ�����ĶԻ���
//		//mpIterator->SetVisible(true);
//		luResult = ERESULT_SUCCESS;
//		break;
//	}
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
void CPreNextButton::OnTimer(
	PSTEMS_TIMER npStatus
	)
{

}

//Ԫ�زο��ߴ緢���仯
ERESULT CPreNextButton::OnElementResized(D2D1_SIZE_F nNewSize)
{
	if (mpIterBtPre != NULL)
	{
		if (nNewSize.width > nNewSize.height)
		{
			//����
			//�����м䰴ť��Ч����
			float lfBtWidth = nNewSize.width * 0.09f;
			D2D1_SIZE_F ldActionSize;
			ldActionSize.width = nNewSize.width - lfBtWidth*2; //��һҳ��һҳ��ռ400
			ldActionSize.height = nNewSize.height - mpIterator->GetPositionY();
			CExMessage::SendMessage(mpIterBtMiddle, mpIterator, EACT_BUTTON_SET_ACTION_RECT, ldActionSize);
			mpIterBtMiddle->SetPosition(lfBtWidth, 0.0f);
			

			//��һҳ
			ldActionSize.width = lfBtWidth; //��һҳ��һҳ��ռ��Ļ��9%
			ldActionSize.height = nNewSize.height - mpIterator->GetPositionY();
			CExMessage::SendMessage(mpIterBtPre, mpIterator, EACT_BUTTON_SET_ACTION_RECT, ldActionSize);

			//��һҳ
			CExMessage::SendMessage(mpIterBtNext, mpIterator, EACT_BUTTON_SET_ACTION_RECT, ldActionSize);

			float lfX = mpIterator->GetSizeX() - lfBtWidth;
			mpIterBtNext->SetPosition(lfX, mpIterBtNext->GetPositionY());
			
			mfLeftButton = lfBtWidth;
		}
		else
		{
			//����
			//�����м䰴ť��Ч����
			float lfBtWidth = nNewSize.width * 0.16f;
			D2D1_SIZE_F ldActionSize;
			ldActionSize.width = nNewSize.width - lfBtWidth*2; //��һҳ��һҳ��ռ250
			ldActionSize.height = nNewSize.height - mpIterator->GetPositionY();
			CExMessage::SendMessage(mpIterBtMiddle, mpIterator, EACT_BUTTON_SET_ACTION_RECT, ldActionSize);
			mpIterBtMiddle->SetPosition(lfBtWidth, 0.0f);


			//��һҳ
			ldActionSize.width = lfBtWidth; //��һҳ��һҳ��ռ��Ļ��16%
			ldActionSize.height = nNewSize.height - mpIterator->GetPositionY();
			CExMessage::SendMessage(mpIterBtPre, mpIterator, EACT_BUTTON_SET_ACTION_RECT, ldActionSize);

			//��һҳ
			CExMessage::SendMessage(mpIterBtNext, mpIterator, EACT_BUTTON_SET_ACTION_RECT, ldActionSize);

			float lfX = mpIterator->GetSizeX() - lfBtWidth;
			mpIterBtNext->SetPosition(lfX, mpIterBtNext->GetPositionY());
			mfLeftButton = lfBtWidth;
		}
	}

	return ERESULT_SUCCESS;
}

//֪ͨԪ�ء���ʾ/���ء������ı�
ERESULT CPreNextButton::OnElementShow(bool nbIsShow)
{
	//EiSetHomebarStatus(nbIsShow == false ? GI_HOMEBAR_SHOW : GI_HOMEBAR_HIDE);

	return ERESULT_SUCCESS;
}