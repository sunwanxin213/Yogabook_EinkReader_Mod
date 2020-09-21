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
	mpPdfPicture = NULL;
	mulPenMode = PEN_MODE_NONE;
	mbIsPressed = false;
	mbIsHand = false;

	miTopHeight = 0;
	miBottomHeight = 0;
	mdwAttrib = 0;
	mbIsFwRect = false;
	ZeroMemory(&mdFwLineRect, sizeof(ED_RECT));
	ZeroMemory(&mdPrePoint, sizeof(mdPrePoint));
	mulPointCount = 0;
	mdwLastPenTicket = 0;
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

		EinkuiGetSystem()->CaptureWindowsMessage(WM_EI_TOUCH, this, (PWINMSG_CALLBACK)&CPreNextButton::OnTouchMsg);// OnActivity);

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

//��д��Ϣ

ERESULT __stdcall CPreNextButton::OnTouchMsg(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& Result)
{
	ERESULT luResult = ERESULT_WINMSG_SENDTO_NEXT;
	PEI_TOUCHINPUT_POINT lpPoint = (PEI_TOUCHINPUT_POINT)lParam;

	do
	{
		if(mpIterator->IsVisible() == false)
			break; //�Լ�û����ʾ���Ͳ�������
		//���ƿ���Fw���߹��� 
		//if (lpPoint->Flags != EI_TOUCHEVENTF_HOVERING_LEAVE)
		//{
		//	//? ? ? ? ��ʱ���룬�ȴ�ite���ӽӿڲ�����ȡ������Ĵ���
		//	if (mbIsFwRect == false && mulPenMode == PEN_MODE_PEN && mbIsHand == false)
		//	{
		//		mbIsFwRect = true;
		//		ResetFwRect();
		//	}
		//}

		if (lpPoint->PenButton == EI_TOUCH_PEN_BUTTON_ABOVE && mulPenMode != PEN_MODE_SELECT && (lpPoint->Flags == EI_TOUCHEVENTF_DOWN || lpPoint->Flags == EI_TOUCHEVENTF_MOVE))
		{
			if (mulPenMode == PEN_MODE_PEN || mulPenMode == PEN_MODE_ERASER)
			{
				//�л�������ģʽ
				ResetFwRect(true);
				SendMessageToParent(EEVT_PEN_MODE, PEN_MODE_SELECT, NULL, 0);

				break;
			}
		}
		//? ? ? ? ��ʱ���룬�ȴ�ite���ӽӿڲ�����ȡ������Ĵ���
		if (lpPoint->Flags == EI_TOUCHEVENTF_HOVERING_LEAVE && mbIsHand == false)
		{
			//�ر�fw����
			//mbIsFwRect = false;
			//ResetFwRect(true);

		}
		else
		{
			if (mbIsFwRect == false)
			{
				if (mulPenMode == PEN_MODE_PEN && ((mbIsHand != false && lpPoint->FingerOrPen == EI_TOUCH_EVENT_FINGER) || (mbIsHand == false && lpPoint->FingerOrPen == EI_TOUCH_EVENT_PEN)))
				{
					//����ite ����
					//mbIsFwRect = true;
					//ResetFwRect();
				}
			}
			else
			{
				if (mulPenMode != PEN_MODE_PEN || (mbIsHand == false && lpPoint->FingerOrPen == EI_TOUCH_EVENT_FINGER) || (mbIsHand != false && lpPoint->FingerOrPen == EI_TOUCH_EVENT_PEN))
				{
					//�ر�fw����
					//mbIsFwRect = false;
					//ResetFwRect(true);
				}
			}
		}

		

		if (lpPoint->FingerOrPen == EI_TOUCH_EVENT_PEN && mulPenMode != PEN_MODE_NONE)
		{
			//����Ǳ����룬��ֻ���ڷ�д��״̬�²Ű���Ϣ�������ݸ�������
			if (!(lpPoint->x < 0.0f || lpPoint->x >= mpIterator->GetSizeX()
				|| lpPoint->y < (unsigned long)miTopHeight || lpPoint->y >= mpIterator->GetSizeY() - miBottomHeight))
			{
				luResult = ERESULT_SUCCESS;
			}

		}

		if (mulPenMode == PEN_MODE_SELECT)
			luResult = ERESULT_WINMSG_SENDTO_NEXT; //����ѡ��״̬�£���Ϣ��������

		if ((lpPoint->x < 0.0f || lpPoint->x >= mpIterator->GetSizeX()
			|| lpPoint->y < (unsigned long)miTopHeight || lpPoint->y >= mpIterator->GetSizeY() - miBottomHeight))
		{
			if(mulPenMode != PEN_MODE_PEN || (lpPoint->Flags != EI_TOUCHEVENTF_UP && lpPoint->Flags != EI_TOUCHEVENTF_HOVERING_LEAVE))
				break; //�����Լ���Χ��������
		}

		if (lpPoint->FingerOrPen == EI_TOUCH_EVENT_FINGER && mbIsHand == false)
			break; //û�п�����д���Ͳ�����������


		if (lpPoint->FingerOrPen == EI_TOUCH_EVENT_PEN && mbIsHand != false)
			break; //��д״̬�£������������

		
		//if(lpPoint->Flags == EI_TOUCHEVENTF_DOWN) //����ʹ��send
		//	CExMessage::SendMessage(mpIterator, mpIterator, EEVT_TOUCH_INPUT, (PEI_TOUCHINPUT_POINT)lParam);
		//else 
		if (lpPoint->Flags != EI_TOUCHEVENTF_HOVERING) //hovering����
			CExMessage::PostMessage(mpIterator, mpIterator, EEVT_TOUCH_INPUT, (PEI_TOUCHINPUT_POINT)lParam, EMSG_POSTTYPE_FAST);

		
		//if (!(lpPoint->x < 0.0f || lpPoint->x >= mpIterator->GetSizeX()
		//	|| lpPoint->y < (unsigned long)miTopHeight || lpPoint->y >= mpIterator->GetSizeY() - miTopHeight - miBottomHeight))
		//{
		//	luResult = ERESULT_SUCCESS; //���ٴ���
		//}

	} while (false);

	

	return luResult;
}


//�����ļ�����
void CPreNextButton::SetFileAttrib(DWORD ndwAttrib)
{
	mdwAttrib = ndwAttrib;
}

// ��������
ERESULT CPreNextButton::OnMouseOwnerTest(const D2D1_POINT_2F& rPoint)
{
	ERESULT luResult = ERESULT_SUCCESS;

	if (!(rPoint.x < 0.0f || rPoint.x >= mpIterator->GetSizeX()
		|| rPoint.y < miTopHeight || rPoint.y >= mpIterator->GetSizeY() - miBottomHeight))
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
			if (mbIsPressed != false)
			{
				//���������һ��Click
				mbIsPressed = false;
				
				if(GetTickCount() - mdwLastPenTicket <= 700)
					break; //˵����д�ֹ����У���������

				if (mZoomStatus == ZoomStatus::NONE)
					ProcessPageNext(npInfo->Position); //��ҳ����
				else
					ProcessPageMove(npInfo->Position); //ҳ���ƶ�����
			}
		}
		else
		{
			//��갴��
			mdPressPos = npInfo->Position;
			mdwClickTicount = GetTickCount();

			//��갴��
			if (mbIsPressed == false)
			{
				mbIsPressed = true;
			}
		}

		lResult = ERESULT_SUCCESS;

	} while (false);

	return lResult;
}

//����ҳ�¼�
void CPreNextButton::ProcessPageNext(D2D1_POINT_2F ndPos)
{
	do 
	{
		if (abs(mdPressPos.x - ndPos.x) >= 20.0f || abs(mdPressPos.y - ndPos.y) >= 20.0f)
		{
			if (mbIsHand != false)
				break; //����������ֻ��ߣ��Ͳ���Ӧ��ҳ���ƶ�

			//�ǻ�������
			if (abs(mdPressPos.x - ndPos.x) >= abs(mdPressPos.y - ndPos.y))	// ���˵���ֱ��
			{
				if ((mdPressPos.x - ndPos.x) >= 50.0f)
				{
					//��һҳ
					PostMessageToParent(EEVT_ER_PRE_NEXT_CLICKED, PNB_BT_NEXT);
				}
				else if ((ndPos.x - mdPressPos.x) >= 50.0f)
				{
					//ǰһҳ
					PostMessageToParent(EEVT_ER_PRE_NEXT_CLICKED, PNB_BT_PRE);
				}
			}

		}
		else
		{
			if (mbIsHand != false && (abs(mdPressPos.x - ndPos.x) >= 3.0f || abs(mdPressPos.y - ndPos.y) >= 3.0f))
				break;

			//��Ϊ�ǵ������
			if ((GetTickCount() - mdwClickTicount) > 500)
			{
				//����500ms�㳤����������
				break;
			}

			//�ж�һ�µ������
			//if (ndPos.x < mfLeftButton)
			//{
			//	//��һҳ
			//	if (mbIsHand != false)
			//		break; //����������ֻ��ߣ��Ͳ���Ӧ��ҳ���ƶ�

			//	PostMessageToParent(EEVT_ER_PRE_NEXT_CLICKED, PNB_BT_PRE);
			//}
			//else if (ndPos.x > mpIterBtNext->GetPositionX())
			//{
			//	//��һҳ
			//	if (mbIsHand != false)
			//		break; //����������ֻ��ߣ��Ͳ���Ӧ��ҳ���ƶ�

			//	PostMessageToParent(EEVT_ER_PRE_NEXT_CLICKED, PNB_BT_NEXT);
			//}
			//else
			{
				//��ʾ�����ع�����
				if ((abs(mdPressPos.x - ndPos.x) >= 12.0f || abs(mdPressPos.y - ndPos.y) >= 12.0f))
					break;

				PostMessageToParent(EEVT_ER_PRE_NEXT_CLICKED, PNB_BT_MIDDLE);
			}

		}
	} while (false);
}

//����ҳ���ƶ�
void CPreNextButton::ProcessPageMove(D2D1_POINT_2F ndpPos)
{
	do 
	{
		if (mbIsHand != false)
			break; //����������ֻ��ߣ��Ͳ���Ӧ��ҳ���ƶ�

		//���̧��
		POINT ldPos;
		ldPos.x = LONG(ndpPos.x - mdPressPos.x);
		ldPos.y = LONG(ndpPos.y - mdPressPos.y);

		if (abs(ldPos.x) >= 20.0f || abs(ldPos.y) >= 20.0f)
		{
			if (mMoveForward == MoveForward::HORIZONTAL)
				ldPos.y = 0;
			else if (mMoveForward == MoveForward::VERTICAL)
				ldPos.x = 0;

			RECT ldRect;
			CExMessage::SendMessage(mpIterator->GetParent(), mpIterator, EEVT_ER_SET_PAGE_MOVE, ldPos, &ldRect, sizeof(RECT));
		}
		else
		{
			//�����������ʾ�����ع�����
			if (abs(ldPos.x) >= 12.0f || abs(ldPos.y) >= 12.0f)
				break;
			PostMessageToParent(EEVT_ER_PRE_NEXT_CLICKED, PNB_BT_MIDDLE_ZOOM);
		}

	} while (false);
}

//�������������߶Ⱥ����������߶�
void CPreNextButton::SetToolbarHeight(int niTopHeight, int niBottomHeight)
{
	miTopHeight = niTopHeight;
	miBottomHeight = niBottomHeight;

	ResetFwRect(false);
}

//�����Ƿ�������״̬,nbIsZoomΪ���ʾ��ǰ������״̬
void CPreNextButton::SetZoomStatus(ZoomStatus status)
{
	mZoomStatus = status;
}

//���û�������
void CPreNextButton::SetMoveForward(MoveForward forward)
{
	mMoveForward = forward;
}

//�����ĵ�����ָ��
void CPreNextButton::SetPdfPicture(CPdfPicture* npPdfPicture, CHighlight* npHighlight)
{
	mpPdfPicture = npPdfPicture;
	mpHighlight = npHighlight;
}

//���ñ�״̬
void CPreNextButton::SetPenMode(ULONG nulPenMode)
{
	mulPenMode = nulPenMode;

	if (mulPenMode != PEN_MODE_PEN)
	{
		//�رջ���
		//mbIsFwRect = false;
		ResetFwRect(true);
	}
	else
	{
		//��������
		ResetFwRect(false);
	}
		
}


//���û�������
void CPreNextButton::SetFwLineRect(ED_RECT ndRect)
{
	mdFwLineRect = ndRect;
	ResetFwRect(false);
}

//�������Ƿ�������ڻ���
void CPreNextButton::SetHandWrite(bool nbIsHand)
{
	mbIsHand = nbIsHand;

	//mbIsFwRect = mbIsHand;
	ResetFwRect(false);
}

//��ȡ��д״̬
bool CPreNextButton::GetHandWrite(void)
{
	return mbIsHand;
}

//����FW��������
void CPreNextButton::ResetFwRect(bool nbIsClose)
{
	mbIsFwRect = !nbIsClose;

	EI_SIZE ldPaintSize;
	EinkuiGetSystem()->GetPaintboardSize(&ldPaintSize);

	EI_RECT ld;
	SET_TP_AREA ldTpArea;
	ldTpArea.Flag = SET_SP_AREA_NO_REPORT;
	ldTpArea.Index = 0;
	ldTpArea.Rect = { 0,0,0,0 };
	EiSetTpArea(ldTpArea); //�Ȼָ�

	if (mbIsHand != false)
	{
		//��������дģʽ���м�������ñ�
		if (miTopHeight > 0 && mulPenMode != PEN_MODE_NONE)
		{
			//��������������·����鴦��
			ldTpArea.Flag = SET_SP_AREA_TOUCH_PEN; //��������
			ldTpArea.Index = 0;
			ldTpArea.Rect.x = 0;
			ldTpArea.Rect.y = 0;
			ldTpArea.Rect.w = ldPaintSize.w;
			ldTpArea.Rect.h = miTopHeight;
			EiSetTpArea(ldTpArea);

			ldTpArea.Flag = SET_SP_AREA_TOUCH_ONLY; //�м�����
			ldTpArea.Index = 1;
			ldTpArea.Rect.x = 0;
			ldTpArea.Rect.y = miTopHeight;
			ldTpArea.Rect.w = ldPaintSize.w;
			ldTpArea.Rect.h = ldPaintSize.h - miTopHeight - miBottomHeight;
			EiSetTpArea(ldTpArea);

			ldTpArea.Flag = SET_SP_AREA_TOUCH_PEN; //��������
			ldTpArea.Index = 2;
			ldTpArea.Rect.x = 0;
			ldTpArea.Rect.y = ldPaintSize.h - miBottomHeight;
			ldTpArea.Rect.w = ldPaintSize.w;
			ldTpArea.Rect.h = miBottomHeight;
			EiSetTpArea(ldTpArea);
		}
		else
		{
			//û����������һ��͹���
			if(mulPenMode == PEN_MODE_NONE)
				ldTpArea.Flag = SET_SP_AREA_TOUCH_PEN; //������״̬
			else
				ldTpArea.Flag = SET_SP_AREA_TOUCH_ONLY;

			ldTpArea.Index = 0;
			ldTpArea.Rect = { 0,0,ldPaintSize.w,ldPaintSize.h };
			EiSetTpArea(ldTpArea);
		}
	}
	else
	{
		//û�п�����д
		ldTpArea.Flag = SET_SP_AREA_TOUCH_PEN; //һ��͹���
		ldTpArea.Index = 0;
		ldTpArea.Rect = { 0, 0, ldPaintSize.w, ldPaintSize.h};
		EiSetTpArea(ldTpArea);
	}

	if (nbIsClose != false || mulPenMode != PEN_MODE_PEN)
	{
		//�ر�fw����
		ld.x = 0;
		ld.y = 0;
		ld.w = 0;
		ld.h = 0;
	}
	else
	{
		

		ld.x = mdFwLineRect.left;
		ld.y = mdFwLineRect.top<miTopHeight? miTopHeight: mdFwLineRect.top;
		ld.w = mdFwLineRect.right - mdFwLineRect.left;
		ld.h = mdFwLineRect.bottom - ld.y;
		if (ld.h > (ldPaintSize.h - miBottomHeight - miTopHeight))
			ld.h = ldPaintSize.h - miBottomHeight - miTopHeight;


		APITconBoolValues ldValue;
		ldValue.EnableHandInHWDraw = mbIsHand == false ? FALSE : TRUE;
		ldValue.EnableAxisTransformInPenMouse = FALSE;
		ldValue.EnablePressureInHWDraw = FALSE;
		ldValue.EnableUpperButtonDrawInHWDraw = FALSE;
		EiTconBoolSetting(ldValue);

	}

	EiSetHandwritingRect(ld);
}

//�����¼�
void CPreNextButton::TouchMsgPro(PEI_TOUCHINPUT_POINT npTouch)
{
	do
	{
		BREAK_ON_NULL(npTouch);

		//if(npTouch->FingerOrPen != 2)
		//	break;
		if (mulPenMode != PEN_MODE_PEN && mulPenMode != PEN_MODE_ERASER && mulPenMode != PEN_MODE_SELECT)
			break; //���Ǳ�����״̬�Ͳ�����

		BREAK_ON_NULL(mpPdfPicture);
		BREAK_ON_NULL(mpHighlight);

		//touch�����¼�
		ULONG lulPenMode = mulPenMode;
		if (npTouch->PenButton == EI_TOUCH_PEN_BUTTON_BELOW || mdPrePoint.PenButton == EI_TOUCH_PEN_BUTTON_BELOW)
			lulPenMode = PEN_MODE_ERASER; //��������¼��˾͵���Ƥ����,FW��bug����ס�¼����������һ��̧�ʻ����������־��ֻ���Լ��ж�

		mdPrePoint.PenButton = npTouch->PenButton;

		if (lulPenMode == PEN_MODE_SELECT)
		{
			if (npTouch->Flags == EI_TOUCHEVENTF_DOWN)
			{
				//����
				mpHighlight->TouchDown(npTouch, lulPenMode, mbIsHand);
			}
			else if (npTouch->Flags == EI_TOUCHEVENTF_MOVE)
			{
				//�ƶ�
				mpHighlight->TouchMove(npTouch, lulPenMode, mbIsHand);
			}
			else if (npTouch->Flags == EI_TOUCHEVENTF_UP)
			{
				//̧��
				mpHighlight->TouchUp(npTouch, lulPenMode, mbIsHand);
			}
			else if (npTouch->Flags == EI_TOUCHEVENTF_HOVERING_LEAVE)
			{
				//�뿪
				mpHighlight->PenLeave(lulPenMode);
			}
		}
		else
		{
			if (npTouch->Flags == EI_TOUCHEVENTF_DOWN)
			{
				//����
				mpPdfPicture->TouchDown(npTouch, lulPenMode, mbIsHand);
			}
			else if (npTouch->Flags == EI_TOUCHEVENTF_MOVE)
			{
				//�ƶ�
				mpPdfPicture->TouchMove(npTouch, lulPenMode, mbIsHand);
			}
			else if (npTouch->Flags == EI_TOUCHEVENTF_UP)
			{
				//̧��
				mpPdfPicture->TouchUp(npTouch, lulPenMode, mbIsHand);
			}
			else if (npTouch->Flags == EI_TOUCHEVENTF_HOVERING_LEAVE)
			{
				//�뿪
				mpPdfPicture->PenLeave(lulPenMode);
			}
		}
		

	} while (false);
}

//��Ϣ������
ERESULT CPreNextButton::ParseMessage(IEinkuiMessage* npMsg)
{
	ERESULT luResult = ERESULT_UNEXPECTED_MESSAGE;

	switch (npMsg->GetMessageID())
	{
	case EEVT_TOUCH_INPUT:
	{
		if ((mdwAttrib&FILE_ATTRIBUTE_READONLY) != false)
			break; //ֻ���ĵ�

		PEI_TOUCHINPUT_POINT ldInput = *(PEI_TOUCHINPUT_POINT*)npMsg->GetInputData();
		if (ldInput->FingerOrPen == EI_TOUCH_EVENT_PEN && (ldInput->Flags == EI_TOUCHEVENTF_MOVE || ldInput->Flags == EI_TOUCHEVENTF_DOWN || ldInput->Flags == EI_TOUCHEVENTF_UP))
		{
			mdwLastPenTicket = GetTickCount();
		}

		TouchMsgPro(ldInput);

		//if (ldInput->Flags == EI_TOUCHEVENTF_DOWN || ldInput->Flags == EI_TOUCHEVENTF_MOVE)
		//{
		//	OutputDebugString(L"downdowndowndowndowndowndowndowndowndowndowndowndowndowndowndowndowndowndowndowndownvv");
		//}
		//else if (ldInput->Flags == EI_TOUCHEVENTF_UP)
		//{
		//	OutputDebugString(L"upupupupupupupupupupupupupupupupupupupupupupupupupupupupupupupupupupupupupupupupupupupupupvv");
		//}

		//if (ldInput->Flags == EI_TOUCHEVENTF_DOWN || ldInput->Flags == EI_TOUCHEVENTF_UP)
		//{
		//	mulPointCount = 0;
		//}
		//else if (ldInput->Flags == EI_TOUCHEVENTF_MOVE)
		//{
		//	mulPointCount++;
		//}

		//if (mulPointCount > 30 && mulPenMode == PEN_MODE_ERASER)
		//{
		//	//�������Ƥģʽ������һ�ʵĵ���������ָ�������˾Ͳ���һ��̧��
		//	EI_TOUCHINPUT_POINT ldNewPoint;
		//	ldNewPoint.x = ldInput->x;
		//	ldNewPoint.y = ldInput->y;
		//	ldNewPoint.z = ldInput->z;
		//	ldNewPoint.FingerOrPen = ldInput->FingerOrPen;
		//	ldNewPoint.PenButton = ldInput->PenButton;
		//	ldNewPoint.Flags = EI_TOUCHEVENTF_UP;
		//	TouchMsgPro(&ldNewPoint);
		//	ldNewPoint.Flags = EI_TOUCHEVENTF_DOWN;
		//	TouchMsgPro(&ldNewPoint);
		//	mulPointCount = 0;
		//}

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

//��ť�����¼�
//ERESULT CPreNextButton::OnCtlButtonClick(IEinkuiIterator* npSender)
//{
//	ERESULT lResult = ERESULT_UNSUCCESSFUL;
//
//	do
//	{
//		ULONG llBtnID = npSender->GetID();
//		switch (llBtnID)
//		{
//		case PNB_BT_PRE:
//		case PNB_BT_MIDDLE:
//		case PNB_BT_NEXT:
//		{
//			//��ҳ����
//			PostMessageToParent(EEVT_ER_PRE_NEXT_CLICKED, llBtnID);
//
//			break;
//		}
//		default:
//			break;
//		}
//
//
//		lResult = ERESULT_SUCCESS;
//	} while (false);
//
//	return lResult;
//}

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
		//mbIsFwRect = false;
		//ResetFwRect(false);
	}

	return ERESULT_SUCCESS;
}

//֪ͨԪ�ء���ʾ/���ء������ı�
ERESULT CPreNextButton::OnElementShow(bool nbIsShow)
{
	//EiSetHomebarStatus(nbIsShow == false ? GI_HOMEBAR_SHOW : GI_HOMEBAR_HIDE);

	return ERESULT_SUCCESS;
}