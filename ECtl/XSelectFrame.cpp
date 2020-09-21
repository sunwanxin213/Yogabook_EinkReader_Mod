#include "StdAfx.h"
#include "cmmBaseObj.h"
#include "Einkui.h"
#include "ElementImp.h"
#include "XCtl.h"

#include "xSelectPoint.h"
#include "xSelectFrame.h"
#include "xSelectFrameMacro.h"

DEFINE_BUILTIN_NAME(System_SelectFrame)

CSelectFrame::CSelectFrame(void)
{
	for (int i = 0; i < SF_POINT_MAX; i++)
		mpArrayPoint[i] = NULL;

	mhCursorNesw = NULL;
	mhCursorNwse = NULL;
	mpXuiBrush = NULL;
	mbModifying = false;

	mcProportionalScaling = false;
	mcProportionalScalingByKey = false;

	mcLastHTurn = false;
	mcLastVTurn = false;

	// Ĭ������༭
	mcIsEdit = true;

}


CSelectFrame::~CSelectFrame(void)
{
	CMM_SAFE_RELEASE(mpXuiBrush);
}


ULONG CSelectFrame::InitOnCreate(
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
		leResult = CXuiElement::InitOnCreate(npParent, npTemplete, nuEID);
		if (leResult != ERESULT_SUCCESS)
			break;

		for (int liID = 0; liID < SF_POINT_MAX; liID++)
		{

			CSelectPoint* lpPoint = CSelectPoint::CreateInstance(mpIterator, npTemplete, liID + 1);
			BREAK_ON_NULL(lpPoint);

			mpArrayPoint[liID] = lpPoint->GetIterator();
			if (liID == 1 || liID == 3 || liID == 5 || liID == 7)
				lpPoint->GetIterator()->SetVisible(false);
		}

		// ������ˢ
		mpXuiBrush = EinkuiGetSystem()->CreateBrush(XuiSolidBrush, D2D1::ColorF(0.0f, 0.0f, 0.0f));
		BREAK_ON_NULL(mpXuiBrush);

		mpXuiBrush->SetStrokeWidth(1.0f);
		mpXuiBrush->SetStrokeType(
			D2D1::StrokeStyleProperties(
				D2D1_CAP_STYLE_FLAT,
				D2D1_CAP_STYLE_FLAT,
				D2D1_CAP_STYLE_ROUND,
				D2D1_LINE_JOIN_MITER,
				10.0f,
				D2D1_DASH_STYLE_DASH,//D2D1_DASH_STYLE_DASH_DOT_DOT,
				0.0f),
			0,
			NULL);

		mhCursorNwse = LoadCursor(NULL, IDC_SIZENWSE);
		mhCursorNesw = LoadCursor(NULL, IDC_SIZENESW);

		leResult = ERESULT_SUCCESS;

	} while (false);

	CMM_SAFE_RELEASE(lpSubKey);

	// ��ϵͳע����Ҫ�յ�����Ϣ
	return leResult;
}


//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
ERESULT CSelectFrame::OnElementCreate(IEinkuiIterator* npIterator)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;
	ICfKey* lpToolTipKey = NULL;

	do
	{
		if (CXuiElement::OnElementCreate(npIterator) != ERESULT_SUCCESS)
			break;

		//�޸�����ƶ�Ҫ�Լ�����ʱ�������ʽ
		CXuiElement::mhInnerCursor = LoadCursor(NULL, IDC_SIZEALL);
		mpIterator->ModifyStyles(EITR_STYLE_ALL_DRAG | EITR_STYLE_ALL_KEY);

		lResult = ERESULT_SUCCESS;

	} while (false);

	return lResult;
}


//�ֽ���Ϣ���ṩ��Ϣ�ֽ����Ϣ��Ӧ�Ĺ��ܣ������ʵ���ǽ���Ϣ�ֽ�Ϊ��ͬ������󣬵�����Ӧ�Ĵ����麯�������ڲ���ʶ����Ϣ��һ�ɷ���ERESULT_UNEXPECTED_MESSAGE
//�������ķ���ֵ���Զ�ͬ�����õ�npMsgָ�����Ϣ������
ERESULT CSelectFrame::ParseMessage(IEinkuiMessage* npMsg)
{
	// ʵ��ԭ�����ȵ�������ķֽ⹦�ܣ����󽫲��������Ϣ��������
	ERESULT luResult = ERESULT_UNEXPECTED_MESSAGE;

	switch (npMsg->GetMessageID())
	{
	case EMSG_SELECTPOINT_BEGIN:
	{
		if (mcIsEdit != false)
		{
			SetPointVisible(false);

			mdOriginPosition = mpIterator->GetPosition();
			mdOriginSize = mpIterator->GetSize();

			//�жϰ�����Ϣ
			if (npMsg->GetInputDataSize() != sizeof(STMS_DRAGGING_ELE))
			{
				luResult = ERESULT_WRONG_PARAMETERS;
				break;
			}
			STMS_DRAGGING_ELE* lpInfo = (STMS_DRAGGING_ELE*)npMsg->GetInputData();
			if ((lpInfo->KeyFlag & MK_SHIFT) != 0)
				mcProportionalScalingByKey = true;

			// Ϊ�����ڷ��Ϳ�ʼ�϶�����Ϣ����Я���κ�����
			SendMessageToParent(EMSG_SELECTPOINT_BEGIN, CExMessage::DataInvalid, NULL, 0);
		}
		else
		{
			SendMessageToParent(EMSG_SELECTFRAME_BEGIN, CExMessage::DataInvalid, NULL, 0);
		}

		break;
	}
	case EMSG_SELECTPOINT_MOVING:
	{
		// �ƶ�ĳ���㣬����ѡ���������ŷ�ת
		if (npMsg->GetInputDataSize() != sizeof(D2D1_SIZE_F))
		{
			luResult = ERESULT_WRONG_PARAMETERS;
			break;
		}

		D2D1_SIZE_F* lpInfo = ((D2D1_SIZE_F*)npMsg->GetInputData());
		if (mcIsEdit != false)
		{
			luResult = OnPointDrag(npMsg->GetMessageSender(), lpInfo);
		}
		else
		{
			// ������༭��������ê�㶼�����ƶ�״̬	
			SendMessageToParent(EMSG_SELECTFRAME_MOVING, *lpInfo, NULL, 0);
		}
		break;
	}
	case EMSG_SELECTFPOINT_MOVED:
	{
		// ÿ���һ���϶������øñ�־
		mcProportionalScaling = false;
		mcProportionalScalingByKey = false;

		// ���÷�ת״̬
		mcLastHTurn = false;
		mcLastVTurn = false;

		SetPointVisible(true);
		mpIterator->SetVisible(true);

		Relocation();

		if (mcIsEdit != false)
		{
			// Ϊ�����ڷ����϶���������Ϣ����Я���κ�����
			SendMessageToParent(EMSG_SELECTFPOINT_MOVED, CExMessage::DataInvalid, NULL, 0);
		}
		else
		{
			SendMessageToParent(EMSG_SELECTFRAME_DRAGED, CExMessage::DataInvalid, NULL, 0);
		}
		break;
	}
	// �����һ������
	case EMSG_SELECTFRAME_REGULAR:
	{
		// �ȴ������ڵ�����������������ţ�������ê��
		//if (mcProportionalScaling == false)
		//	EinkuiGetSystem()->GetElementManager()->ResetDragBegin(mpArrayPoint[miActivePoint-1]);
		break;
	}
	case EMSG_SET_PROPORTIONAL_SCALING:
	{
		// �Ƿ�ִ�еȱ�������
		if (npMsg->GetInputDataSize() != sizeof(bool))
		{
			luResult = ERESULT_WRONG_PARAMETERS;
			break;
		}

		bool* lpInfo = ((bool*)npMsg->GetInputData());
		mcProportionalScaling ^= *lpInfo;
		break;
	}
	case EMSG_SET_EDIT_STATUS:
	{
		if (npMsg->GetInputDataSize() != sizeof(bool))
		{
			luResult = ERESULT_WRONG_PARAMETERS;
			break;
		}

		bool* lpInfo = ((bool*)npMsg->GetInputData());
		mcIsEdit = *lpInfo;
	}
	break;
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

//������Ϣ
ERESULT CSelectFrame::OnKeyPressed(const STEMS_KEY_PRESSED* npInfo)
{
	ERESULT luResult;

	luResult = SendMessageToParent(EMSG_SELECTFRAME_KEYBOARD, *npInfo, NULL, 0);
	if (luResult == ERESULT_KEY_ACCEPTED)
		return luResult;

	if (false != npInfo->IsPressDown)		// ֻ������
	{
		D2D1_POINT_2F ldOffset = { 0.0f,0.0f };

		luResult = ERESULT_SUCCESS;

		switch (npInfo->VirtualKeyCode)
		{
		case VK_LEFT:
			ldOffset.x = -1.0f;
			break;
		case VK_RIGHT:
			ldOffset.x = 1.0f;
			break;
		case VK_UP:
			ldOffset.y = -1.0f;
			break;
		case VK_DOWN:
			ldOffset.y = 1.0f;
			break;
		default:
			luResult = ERESULT_KEY_UNEXPECTED;
		}

		if (luResult == ERESULT_SUCCESS)
		{
			SendMessageToParent(EMSG_SELECTFRAME_BEGIN, CExMessage::DataInvalid, NULL, 0);
			SendMessageToParent(EMSG_SELECTFRAME_MOVING, ldOffset, NULL, 0);
			SendMessageToParent(EMSG_SELECTFRAME_DRAGED, CExMessage::DataInvalid, NULL, 0);
		}
	}
	else
		luResult = ERESULT_KEY_UNEXPECTED;

	return luResult;
}


//Ԫ�زο��ߴ緢���仯
ERESULT CSelectFrame::OnElementResized(D2D1_SIZE_F nNewSize)
{
	Relocation();

	return ERESULT_SUCCESS;
}


// ���ö�Ӧ������״̬
void CSelectFrame::SetCursor(D2D1_POINT_2F ndPoint, D2D1_RECT_F ndNormalRect, int niIndex)
{
	float lfHalfWidth = mpArrayPoint[0]->GetSizeX() / 2;
	float lfHalfHeight = mpArrayPoint[0]->GetSizeY() / 2;
	float lfWidth = mpArrayPoint[0]->GetSizeX();
	float lfHeight = mpArrayPoint[0]->GetSizeY();

	HCURSOR lhCursorNwse = LoadCursor(NULL, IDC_SIZENWSE);
	HCURSOR lhCursorNesw = LoadCursor(NULL, IDC_SIZENESW);

	// �Ƿ���1��5��
	if (ndPoint.x == (ndNormalRect.left - lfHalfWidth) && ndPoint.y == (ndNormalRect.top - lfHalfHeight) ||
		ndPoint.x == (ndNormalRect.right - lfHalfWidth) && ndPoint.y == (ndNormalRect.bottom - lfHalfHeight)
		)
	{
		CExMessage::SendMessage(mpArrayPoint[niIndex], mpIterator, EMSG_SELECTPOINT_RESET_CURSOR, lhCursorNwse);
	}

	// �Ƿ���3��7��
	if (ndPoint.x == ndNormalRect.right - lfHalfWidth && ndPoint.y == (ndNormalRect.top - lfHalfHeight) ||
		ndPoint.x == ndNormalRect.left - lfHalfWidth && ndPoint.y == (ndNormalRect.bottom - lfHalfHeight)
		)
	{
		CExMessage::SendMessage(mpArrayPoint[niIndex], mpIterator, EMSG_SELECTPOINT_RESET_CURSOR, lhCursorNesw);
	}

	if (lhCursorNwse != NULL)
		DestroyCursor(lhCursorNwse);

	if (lhCursorNesw != NULL)
		DestroyCursor(lhCursorNesw);

}

//�����˸����λ��
void CSelectFrame::Relocation(void)
{
	if (mpArrayPoint[0] != NULL)
	{
		float lfHalfWidth = mpArrayPoint[0]->GetSizeX() / 2;
		float lfHalfHeight = mpArrayPoint[0]->GetSizeY() / 2;
		float lfWidth = mpArrayPoint[0]->GetSizeX();
		float lfHeight = mpArrayPoint[0]->GetSizeY();

		D2D1_POINT_2F ldPoint = mpIterator->GetPosition();
		D2D1_RECT_F	ldNormalRect = CExRect::GetNormalizedRectangle(mpIterator->GetPositionX(), mpIterator->GetPositionY(), mpIterator->GetSizeX(), mpIterator->GetSizeY());

		HCURSOR lhCursorSizeAll = LoadCursor(NULL, IDC_SIZEALL);

		do
		{

			mpArrayPoint[0]->SetPosition(-lfHalfWidth, -lfHalfHeight);
			if (mcIsEdit == false)
				CExMessage::SendMessage(mpArrayPoint[0], mpIterator, EMSG_SELECTPOINT_RESET_CURSOR, lhCursorSizeAll);
			else
				SetCursor(D2D1::Point2F(-lfHalfWidth, -lfHalfHeight), ldNormalRect, 0);

			mpArrayPoint[1]->SetPosition(FLOAT((mpIterator->GetSizeX() - lfHalfWidth) / 2.0f), -lfHalfHeight);
			if (mcIsEdit == false)
				CExMessage::SendMessage(mpArrayPoint[1], mpIterator, EMSG_SELECTPOINT_RESET_CURSOR, lhCursorSizeAll);

			mpArrayPoint[2]->SetPosition(mpIterator->GetSizeX() - lfHalfWidth, -lfHalfHeight);
			if (mcIsEdit == false)
				CExMessage::SendMessage(mpArrayPoint[2], mpIterator, EMSG_SELECTPOINT_RESET_CURSOR, lhCursorSizeAll);
			else
				SetCursor(D2D1::Point2F(mpIterator->GetSizeX() - lfHalfWidth, -lfHalfHeight), ldNormalRect, 2);

			mpArrayPoint[3]->SetPosition(mpIterator->GetSizeX() - lfHalfWidth, FLOAT((mpIterator->GetSizeY() - lfWidth) / 2.0f));
			if (mcIsEdit == false)
				CExMessage::SendMessage(mpArrayPoint[3], mpIterator, EMSG_SELECTPOINT_RESET_CURSOR, lhCursorSizeAll);

			mpArrayPoint[4]->SetPosition(mpIterator->GetSizeX() - lfHalfWidth, mpIterator->GetSizeY() - lfHalfHeight);
			if (mcIsEdit == false)
				CExMessage::SendMessage(mpArrayPoint[4], mpIterator, EMSG_SELECTPOINT_RESET_CURSOR, lhCursorSizeAll);
			else
				SetCursor(D2D1::Point2F(mpIterator->GetSizeX() - lfHalfWidth, mpIterator->GetSizeY() - lfHalfHeight), ldNormalRect, 4);

			mpArrayPoint[5]->SetPosition(FLOAT((mpIterator->GetSizeX() - lfWidth) / 2.0f), mpIterator->GetSizeY() - lfHalfHeight);
			if (mcIsEdit == false)
				CExMessage::SendMessage(mpArrayPoint[5], mpIterator, EMSG_SELECTPOINT_RESET_CURSOR, lhCursorSizeAll);

			mpArrayPoint[6]->SetPosition(-lfHalfWidth, mpIterator->GetSizeY() - lfHalfHeight);
			if (mcIsEdit == false)
				CExMessage::SendMessage(mpArrayPoint[6], mpIterator, EMSG_SELECTPOINT_RESET_CURSOR, lhCursorSizeAll);
			else
				SetCursor(D2D1::Point2F(-lfHalfWidth, mpIterator->GetSizeY() - lfHalfHeight), ldNormalRect, 6);

			mpArrayPoint[7]->SetPosition(-lfHalfWidth, FLOAT((mpIterator->GetSizeY() - lfHalfHeight) / 2.0f));
			if (mcIsEdit == false)
				CExMessage::SendMessage(mpArrayPoint[7], mpIterator, EMSG_SELECTPOINT_RESET_CURSOR, lhCursorSizeAll);

		} while (false);

		if (lhCursorSizeAll != NULL)
			DestroyCursor(lhCursorSizeAll);
	}

}


//����
ERESULT CSelectFrame::OnPaint(IEinkuiPaintBoard* npPaintBoard)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		BREAK_ON_NULL(npPaintBoard);

		if (mpBgBitmap != NULL)
		{
			npPaintBoard->DrawBitmap(D2D1::RectF(0, 0, mpIterator->GetSizeX(), mpIterator->GetSizeY()),
				mpBgBitmap,
				ESPB_DRAWBMP_LINEAR
			);
		}

		//��ѡ��������
		float lfHalfWidth = mpArrayPoint[0]->GetSizeX() / 2;
		float lfHalfHeight = mpArrayPoint[0]->GetSizeY() / 2;

		for (ULONG liLoop = 0; liLoop < SF_POINT_MAX - 1; liLoop++)
		{
			npPaintBoard->DrawLine(
				D2D1::Point2F(
					CExFloat::HalfPixel(mpArrayPoint[liLoop]->GetPosition().x + lfHalfWidth),
					CExFloat::HalfPixel(mpArrayPoint[liLoop]->GetPosition().y + lfHalfHeight)),
				D2D1::Point2F(
					CExFloat::HalfPixel(mpArrayPoint[liLoop + 1]->GetPosition().x + lfHalfWidth),
					CExFloat::HalfPixel(mpArrayPoint[liLoop + 1]->GetPosition().y + lfHalfHeight)),
				mpXuiBrush);
		}

		npPaintBoard->DrawLine(
			D2D1::Point2F
			(CExFloat::HalfPixel(mpArrayPoint[7]->GetPosition().x + lfHalfWidth),
				CExFloat::HalfPixel(mpArrayPoint[7]->GetPosition().y + lfHalfHeight)),
			D2D1::Point2F
			(CExFloat::HalfPixel(mpArrayPoint[0]->GetPosition().x + lfHalfWidth),
				CExFloat::HalfPixel(mpArrayPoint[0]->GetPosition().y + lfHalfHeight)),
			mpXuiBrush);

		lResult = ERESULT_SUCCESS;

	} while (false);

	return lResult;
}

// ��������
ERESULT CSelectFrame::OnMouseOwnerTest(const D2D1_POINT_2F& rPoint)
{
	ERESULT luResult = ERESULT_SUCCESS;

	do
	{
		if (rPoint.x < 0.0f || (UINT)rPoint.x >= mpIterator->GetSizeX() || rPoint.y < 0.0f || (UINT)rPoint.y >= mpIterator->GetSizeY())
			break;

		luResult = ERESULT_MOUSE_OWNERSHIP;

	} while (false);

	return luResult;

	// ���˸�����Χ�ɵľ�������

	// �������ο�
	D2D1_RECT_F ldTopRect = D2D1::RectF(
		mpArrayPoint[0]->GetPositionX(),
		mpArrayPoint[0]->GetPositionY(),
		mpArrayPoint[0]->GetPositionX() + mpIterator->GetSizeX(),
		mpArrayPoint[0]->GetSizeY()
	);
	if (CExRect::PtInRect(rPoint, ldTopRect))
		return ERESULT_MOUSE_OWNERSHIP;

	// �ұ߾��ο�
	D2D1_RECT_F ldRightRect = D2D1::RectF(
		mpArrayPoint[2]->GetPositionX(),
		mpArrayPoint[2]->GetPositionY(),
		mpArrayPoint[2]->GetPositionX() + mpArrayPoint[2]->GetSizeX(),
		mpIterator->GetSizeY() + mpArrayPoint[2]->GetSizeY()
	);
	if (CExRect::PtInRect(rPoint, ldRightRect))
		return ERESULT_MOUSE_OWNERSHIP;

	// �ײ����ο�
	D2D1_RECT_F ldBottomRect = D2D1::RectF(
		mpArrayPoint[6]->GetPositionX(),
		mpArrayPoint[6]->GetPositionY(),
		mpArrayPoint[6]->GetPositionX() + mpIterator->GetSizeX(),
		mpArrayPoint[6]->GetPositionY() + mpArrayPoint[6]->GetSizeY());
	if (CExRect::PtInRect(rPoint, ldBottomRect))
		return ERESULT_MOUSE_OWNERSHIP;

	// ��߾��ο�
	D2D1_RECT_F ldLeftRect = D2D1::RectF(
		mpArrayPoint[0]->GetPositionX(),
		mpArrayPoint[0]->GetPositionY(),
		mpArrayPoint[0]->GetPositionX() + mpArrayPoint[0]->GetSizeX(),
		mpIterator->GetSizeY() + mpArrayPoint[0]->GetSizeY()
	);
	if (CExRect::PtInRect(rPoint, ldLeftRect))
		return ERESULT_MOUSE_OWNERSHIP;

	return ERESULT_SUCCESS;

}

//���û�����
ERESULT CSelectFrame::OnElementEnable(bool nbIsEnable)
{
	for (int i = 0; i < SF_POINT_MAX; i++)
	{
		mpArrayPoint[i]->SetVisible(nbIsEnable);	//����ǽ��þͲ���ʾ�˸���
	}

	return ERESULT_SUCCESS;
}

ERESULT CSelectFrame::OnDragging(const STMS_DRAGGING_ELE* npInfo)
{
	ERESULT leResult = ERESULT_UNSUCCESSFUL;

	do
	{
		BREAK_ON_NULL(npInfo);
		if ((npInfo->ActKey&MK_LBUTTON) == 0)	//ֻ����������϶�
			break;

		SendMessageToParent(EMSG_SELECTFRAME_MOVING, npInfo->Offset, NULL, 0);

		leResult = ERESULT_SUCCESS;

	} while (false);

	return leResult;
}

//��ק��ʼ
ERESULT CSelectFrame::OnDragBegin(const STMS_DRAGGING_ELE* npInfo)
{
	ERESULT leResult = ERESULT_UNSUCCESSFUL;

	do
	{
		if (mpIterator->IsEnable() == false)
			break;	//����ǽ���״̬�Ͳ����϶�

		BREAK_ON_NULL(npInfo);
		if ((npInfo->ActKey&MK_LBUTTON) == 0)	//ֻ����������϶�
			break;

		if (npInfo->DragOn != mpIterator) //ֻ�����Լ�����ſ���
			break;

		SendMessageToParent(EMSG_SELECTFRAME_BEGIN, CExMessage::DataInvalid, NULL, 0);

		leResult = ERESULT_SUCCESS;

	} while (false);

	return leResult;
}

//��ק����
ERESULT CSelectFrame::OnDragEnd(const STMS_DRAGGING_ELE* npInfo)
{
	ERESULT leResult = ERESULT_UNSUCCESSFUL;

	do
	{
		BREAK_ON_NULL(npInfo);

		if ((npInfo->ActKey&MK_LBUTTON) == 0)	//ֻ����������϶�
			break;

		SendMessageToParent(EMSG_SELECTFRAME_DRAGED, CExMessage::DataInvalid, NULL, 0);

		Relocation();

		leResult = ERESULT_SUCCESS;

	} while (false);

	return leResult;
}

//����˸����϶�
ERESULT CSelectFrame::OnPointDrag(IEinkuiIterator* npDragItem, D2D1_SIZE_F* npOffset)
{
	ERESULT leResult = ERESULT_UNSUCCESSFUL;

	STCTL_CHANGE_POSITION_SIZE	ldPositionSize;	// �϶��㣬�ı�λ�úʹ�С
	D2D1_POINT_2F	ldPositionOffset;		// ��¼λ��ƫ����
	D2D1_SIZE_F		ldSizeVariation;		// ��С�ı���


	float lfSizeX = 0.0f;
	float lfSizeY = 0.0f;

	do
	{

		// �ȼٶ�����Ҫ��ת
		ldPositionSize.mcHTurn = false;
		ldPositionSize.mcVTurn = false;
		mcHTurn = false;
		mcVTurn = false;

		BREAK_ON_NULL(npDragItem);
		BREAK_ON_NULL(npOffset);

		// ���㲻ͬê������Ӧ��λ��ƫ�ƺʹ�С�仯��
		switch (npDragItem->GetID())
			//switch(miActivePoint)
		{
			// ���ϵ�
		case XuiLeftTop:
		{
			// ����λ��ƫ����
			ldPositionOffset = D2D1::Point2F(npOffset->width, npOffset->height);
			ldSizeVariation = D2D1::SizeF(-npOffset->width, -npOffset->height);
			break;
		}
		// ���ϵ�
		case XuiMidTop:
		{
			ldPositionOffset = D2D1::Point2F(0, npOffset->height);
			ldSizeVariation = D2D1::SizeF(0, -npOffset->height);		// ��������������
			break;
		}
		// ���ϵ�
		case XuiRightTop:
		{
			ldPositionOffset = D2D1::Point2F(0, npOffset->height);
			ldSizeVariation = D2D1::SizeF(npOffset->width, -npOffset->height);
			break;
		}
		// ���е�
		case XuiRightMid:
		{
			ldPositionOffset = D2D1::Point2F(0, 0);
			ldSizeVariation = D2D1::SizeF(npOffset->width, 0);	// ��������������
			break;
		}
		// ���µ�
		case XuiRightBottom:
		{
			ldPositionOffset = D2D1::Point2F(0, 0);
			ldSizeVariation = D2D1::SizeF(npOffset->width, npOffset->height);
			break;
		}
		// ���µ�
		case XuiMidBottom:
		{
			ldPositionOffset = D2D1::Point2F(0, 0);
			ldSizeVariation = D2D1::SizeF(0, npOffset->height);	// ��������������
			break;
		}
		// ���µ�
		case XuiLeftBottom:
		{
			ldPositionOffset = D2D1::Point2F(npOffset->width, 0);
			ldSizeVariation = D2D1::SizeF(-npOffset->width, npOffset->height);
			break;
		}
		// ���е�
		case XuiLeftMid:
		{
			ldPositionOffset = D2D1::Point2F(npOffset->width, 0);
			ldSizeVariation = D2D1::SizeF(-npOffset->width, 0);	// ��������������
			break;
		}

		}

		// ��ȡ�ı��Ĵ�С
		lfSizeX = mdOriginSize.width + ldSizeVariation.width;
		lfSizeY = mdOriginSize.height + ldSizeVariation.height;

		// ���size=0,�򱾴��϶�������
		if (lfSizeX == 0 || lfSizeY == 0)
			return ERESULT_SUCCESS;

		// �����и�Ч������
		// 1������ϣ�����ת����Ҫ��һ�����Σ���ʱ�����������϶���ê�㣻�����������������ת����϶�ÿ�ζ�������жϷ�ת�����̣��Ӷ���������������
		// 2���ȱ����ŵ�����£������һ���󣬼���б��ʱ���㽫���临�ӣ����Բ���һ����
		// 3���������ܸĳ�֧�ָ����Σ�ʹ��Ϊͳһ�����������������϶�ʱ���������ת����߿��ܻ�为ֵ���϶���ɺ��ڹ�һ�����꼴�ɡ�
		// 4�����ڵ�ģ���ǣ�����������Ų��ù�һ��ģ�ͣ��ȱ����Ų��ø��������㣬���ϲ�����Ľ��ת��Ϊ�����Ρ�


		// ��һ���������positionƫ�ƺ�size�仯��
		ldPositionSize.mdPositionOffset = ldPositionOffset;
		ldPositionSize.mdSizeVariation = ldSizeVariation;

		// ����ȱ������߼����������ź�Ľ���������positionƫ�ƺ�size�仯��
		if (mcProportionalScaling != false || mcProportionalScalingByKey != false)
		{
			OnProportionalScaling(npDragItem, ldPositionSize);
		}
		else
		{
			OnNormalScaling(npDragItem, ldPositionSize);
		}

		// �򸸴��ڱ����ƶ�״̬{λ��ƫ��������С�ñ�����ˮƽ��ת����ֱ��ת}
		SendMessageToParent(EMSG_SELECTPOINT_CHANGE_POSITION_SIZE, ldPositionSize, NULL, 0);

		leResult = ERESULT_SUCCESS;

	} while (false);

	return leResult;
}


ERESULT CSelectFrame::OnNormalScaling(IEinkuiIterator* npDragItem, STCTL_CHANGE_POSITION_SIZE& noChangePositionSize)
{

	float lfSizeX = 0;
	float lfSizeY = 0;

	// ��ȡ�ı��Ĵ�С
	lfSizeX = mdOriginSize.width + noChangePositionSize.mdSizeVariation.width;
	lfSizeY = mdOriginSize.height + noChangePositionSize.mdSizeVariation.height;


	// �ж����޷�ת
	do
	{
		// �ж��Ƿ�ֱ��ת
		if (lfSizeY < 0)
		{
			noChangePositionSize.mcVTurn = true;
			mcVTurn = true;
		}
		// �ж��Ƿ�ˮƽ��ת
		if (lfSizeX < 0)
		{
			noChangePositionSize.mcHTurn = true;
			mcHTurn = true;
		}

		// ���û�з�ת����ֱ������
		if (mcHTurn == false && mcVTurn == false)
			break;

		// ����з�ת�������ÿ���㣬��������
		switch (npDragItem->GetID())
		{
			// ���ϵ�
		case XuiLeftTop:
		{
			if (mcHTurn != false)
				noChangePositionSize.mdPositionOffset.x = mdOriginSize.width;
			if (mcVTurn != false)
				noChangePositionSize.mdPositionOffset.y = mdOriginSize.height;

			break;
		}
		// ���ϵ�
		case XuiMidTop:
		{
			if (mcVTurn != false)
				noChangePositionSize.mdPositionOffset.y = mdOriginSize.height;

			break;
		}
		// ���ϵ�
		case XuiRightTop:
		{
			if (mcHTurn != false)
				noChangePositionSize.mdPositionOffset.x = lfSizeX;

			if (mcVTurn != false)
				noChangePositionSize.mdPositionOffset.y = mdOriginSize.height;

			break;
		}
		// ���е�
		case XuiRightMid:
		{
			if (mcHTurn != false)
				noChangePositionSize.mdPositionOffset.x = lfSizeX;

			break;
		}
		// ���µ�
		case XuiRightBottom:
		{
			if (mcHTurn != false)
				noChangePositionSize.mdPositionOffset.x = lfSizeX;

			if (mcVTurn != false)
				noChangePositionSize.mdPositionOffset.y = lfSizeY;

			break;
		}
		// ���µ�
		case XuiMidBottom:
		{
			if (mcVTurn != false)
				noChangePositionSize.mdPositionOffset.y = lfSizeY;

			break;
		}
		// ���µ�
		case XuiLeftBottom:
		{
			if (mcHTurn != false)
				noChangePositionSize.mdPositionOffset.x = mdOriginSize.width;

			if (mcVTurn != false)
				noChangePositionSize.mdPositionOffset.y = lfSizeY;

			break;
		}
		// ���е�
		case XuiLeftMid:
		{
			if (mcHTurn != false)
				noChangePositionSize.mdPositionOffset.x = mdOriginSize.width;

			break;
		}

		}// end switch

	} while (false);


	if (mcHTurn != false || mcVTurn != false)
	{

		// ��ת֮��Ĵ�С
		D2D1_SIZE_F ldAfterTurnedSize;

		ldAfterTurnedSize.width = lfSizeX > 0 ? lfSizeX : -lfSizeX;
		ldAfterTurnedSize.height = lfSizeY > 0 ? lfSizeY : -lfSizeY;

		// ��תǰ��ı仯�� = ��ת��Ĵ�С - ԭʼ��С
		noChangePositionSize.mdSizeVariation.width = ldAfterTurnedSize.width - mdOriginSize.width;
		noChangePositionSize.mdSizeVariation.height = ldAfterTurnedSize.height - mdOriginSize.height;

	}

	// ���ݵ�ǰ״̬����һ�εķ�ת״̬���ж�ʵ���Ƿ�����ת
	if (mcLastHTurn != mcHTurn)
	{
		noChangePositionSize.mcHTurn = true;
		mcLastHTurn = mcHTurn;
	}
	else
	{
		noChangePositionSize.mcHTurn = false;
	}

	if (mcLastVTurn != mcVTurn)
	{
		noChangePositionSize.mcVTurn = true;
		mcLastVTurn = mcVTurn;
	}
	else
	{
		noChangePositionSize.mcVTurn = false;
	}

	return ERESULT_SUCCESS;

}



ERESULT CSelectFrame::OnProportionalScaling(IEinkuiIterator* npDragItem, STCTL_CHANGE_POSITION_SIZE& noChangePositionSize)
{
	ERESULT luResult = ERESULT_UNSUCCESSFUL;

	// ��¼��С�ı仯��
	float lfSizeX = noChangePositionSize.mdSizeVariation.width;
	float lfSizeY = noChangePositionSize.mdSizeVariation.height;

	// �ȱȱ���ǰ�����ۿ�Ⱥ͸߶�
	float lfBeforeScaledWidth = 0.0f;
	float lfBeforeScaledHeight = 0.0f;

	// ��׼б��
	float lfBaseLineSlope;
	// ��ǰб��
	float lfCurLineSlope;

	// ��ȡ�ı��Ĵ�С
	lfBeforeScaledWidth = mdOriginSize.width + lfSizeX;
	lfBeforeScaledHeight = mdOriginSize.height + lfSizeY;

	{
		// �����׼б��,��ԭʼ���ε�����ȷ����������ʼ��ʼ����(0.0f, 0.0f)������б�ʾ��ǿ�Ⱥ͸߶ȵı�ֵ
		lfBaseLineSlope = mdOriginSize.width / mdOriginSize.height;

		// ���㵱ǰб�ʣ�Ϊ��ǰ�����߶ȵı�ֵ
		lfCurLineSlope = lfBeforeScaledWidth / lfBeforeScaledHeight;
		// ȡ����ֵ�����ǵķ�ת�����
		lfCurLineSlope = lfCurLineSlope > 0 ? lfCurLineSlope : -lfCurLineSlope;
	}


	{
		// �ж��Ƿ�ˮƽ��ת
		if (lfBeforeScaledWidth < 0)
		{
			noChangePositionSize.mcHTurn = true;
			mcHTurn = true;
		}

		// �ж��Ƿ���ֱ��ת
		if (lfBeforeScaledHeight < 0)
		{
			noChangePositionSize.mcVTurn = true;
			mcVTurn = true;
		}
	}


	//////////////////////////////////////////////////////////////////////////
	// ����5���Ǿ������룬ͳһ����ȱ����ź�ı仯������˸������
	//////////////////////////////////////////////////////////////////////////
	{
		if (lfCurLineSlope >= lfBaseLineSlope)
		{
			// ȡX������ű�����������X�����
			lfSizeX = (fabs(lfSizeX + mdOriginSize.width) - mdOriginSize.width);
			noChangePositionSize.mdSizeVariation = D2D1::SizeF(lfSizeX, lfSizeX / lfBaseLineSlope);
		}
		else
		{
			// ȡY������ű���
			lfSizeY = (fabs(lfSizeY + mdOriginSize.height) - mdOriginSize.height);
			noChangePositionSize.mdSizeVariation = D2D1::SizeF(lfSizeY*lfBaseLineSlope, lfSizeY);
		}
	}

	// ����ȱȱ��κ��X���Y��ı仯����Ҳ���ǿ�Ⱥ͸߶ȵı仯�����������仯����Ҫ���ݲ�ͬ�ķ�ת�����������
	float lfSizeVarX = 0.0f;
	float lfSizeVarY = 0.0f;

	lfSizeVarX = noChangePositionSize.mdSizeVariation.width;
	lfSizeVarY = noChangePositionSize.mdSizeVariation.height;

	do
	{


		switch (npDragItem->GetID())
		{
			// ���ϵ�
		case XuiLeftTop:
		{
			if (mcHTurn != false)
			{
				noChangePositionSize.mdPositionOffset.x = mdOriginSize.width;
				noChangePositionSize.mdPositionOffset.y = -lfSizeVarY;
			}
			if (mcVTurn != false)
			{
				noChangePositionSize.mdPositionOffset.x = -lfSizeVarX;
				noChangePositionSize.mdPositionOffset.y = mdOriginSize.height;
			}

			if (mcHTurn != false && mcVTurn != false)
			{
				noChangePositionSize.mdPositionOffset.x = mdOriginSize.width;
				noChangePositionSize.mdPositionOffset.y = mdOriginSize.height;
			}

			if (mcVTurn == false && mcHTurn == false)
			{
				noChangePositionSize.mdPositionOffset = D2D1::Point2F(-lfSizeVarX, -lfSizeVarY);
			}

			break;
		}
		// ���ϵ�
		case XuiMidTop:
		{
			float lfWidthOffset = (lfSizeY / mdOriginSize.height)*mdOriginSize.width;

			if (mcVTurn != false)
			{
				// ����ļ����߼��ܱ�̬����ͼ�Ը������20120701
				float xTmp = 0.0f;
				if (mdOriginSize.width / 2 + lfWidthOffset / 2 < 0)
				{
					xTmp = mdOriginSize.width + lfWidthOffset / 2;
				}
				else
				{
					xTmp = -lfWidthOffset / 2;
				}

				noChangePositionSize.mdPositionOffset = D2D1::Point2F(xTmp, mdOriginSize.height);
			}
			else
			{
				noChangePositionSize.mdPositionOffset = D2D1::Point2F(-lfWidthOffset / 2, noChangePositionSize.mdPositionOffset.y);
			}

			noChangePositionSize.mdSizeVariation = D2D1::SizeF(lfWidthOffset, lfSizeY);
			break;
		}
		// ���ϵ�
		case XuiRightTop:
		{
			if (mcHTurn != false)
			{
				noChangePositionSize.mdPositionOffset.x = -(mdOriginSize.width + lfSizeVarX);
				noChangePositionSize.mdPositionOffset.y = -lfSizeVarY;
			}

			if (mcVTurn != false)
			{
				noChangePositionSize.mdPositionOffset.x = 0.0f;
				noChangePositionSize.mdPositionOffset.y = mdOriginSize.height;
			}

			if (mcHTurn != false && mcVTurn != false)
			{
				noChangePositionSize.mdPositionOffset.x = -(mdOriginSize.width + lfSizeVarX);
				noChangePositionSize.mdPositionOffset.y = mdOriginSize.height;
			}

			if (mcHTurn == false && mcVTurn == false)
			{
				noChangePositionSize.mdPositionOffset = D2D1::Point2F(0, -lfSizeVarY);
			}

			break;
		}
		// ���е�
		case XuiRightMid:
		{
			float lfHeightOffset = (lfSizeX / mdOriginSize.width)*mdOriginSize.height;
			if (mcHTurn != false)
			{
				float xTmp = 0.0f;
				if (mdOriginSize.height / 2 + lfHeightOffset / 2 < 0)
				{
					xTmp = mdOriginSize.height + lfHeightOffset / 2;
				}
				else
				{
					xTmp = -lfHeightOffset / 2;
				}
				noChangePositionSize.mdPositionOffset = D2D1::Point2F(lfBeforeScaledWidth, xTmp);
			}
			else
			{
				noChangePositionSize.mdPositionOffset = D2D1::Point2F(0.0f, -lfHeightOffset / 2);
			}

			noChangePositionSize.mdSizeVariation = D2D1::SizeF(lfSizeX, lfHeightOffset);
			break;
		}
		// ���µ�
		case XuiRightBottom:
		{

			if (mcHTurn != false && mcVTurn != false)
			{
				noChangePositionSize.mdPositionOffset.x = -(mdOriginSize.width + lfSizeVarX);
				noChangePositionSize.mdPositionOffset.y = -(mdOriginSize.height + lfSizeVarY);
				break;
			}

			if (mcHTurn != false)
			{
				noChangePositionSize.mdPositionOffset.x = -(mdOriginSize.width + lfSizeVarX);
				noChangePositionSize.mdPositionOffset.y = 0;
			}

			if (mcVTurn != false)
			{
				noChangePositionSize.mdPositionOffset.x = 0;
				noChangePositionSize.mdPositionOffset.y = -(mdOriginSize.height + lfSizeVarY);
			}

			if (mcHTurn == false && mcVTurn == false)
			{
				noChangePositionSize.mdPositionOffset = D2D1::Point2F(0, 0);
			}

			break;
		}
		// ���µ�
		case XuiMidBottom:
		{
			float lfWidthOffset = (lfSizeY / mdOriginSize.height)*mdOriginSize.width;
			if (mcVTurn != false)
			{

				float xTmp = 0.0f;
				if (mdOriginSize.width / 2 + lfWidthOffset / 2 < 0)
				{
					xTmp = mdOriginSize.width + lfWidthOffset / 2;
				}
				else
				{
					xTmp = -lfWidthOffset / 2;
				}

				noChangePositionSize.mdPositionOffset = D2D1::Point2F(xTmp, lfBeforeScaledHeight);
			}
			else
			{
				noChangePositionSize.mdPositionOffset = D2D1::Point2F(-lfWidthOffset / 2, noChangePositionSize.mdPositionOffset.y);
			}

			//noChangePositionSize.mdSizeVariation = D2D1::SizeF(lfWidthOffset, fabs(lfHeight) - mdOriginSize.height);	
			noChangePositionSize.mdSizeVariation = D2D1::SizeF(lfWidthOffset, lfSizeY);

			break;
		}
		// ���µ�
		case XuiLeftBottom:
		{

			if (mcHTurn != false)
			{
				noChangePositionSize.mdPositionOffset.x = mdOriginSize.width;
				noChangePositionSize.mdPositionOffset.y = 0.0f;
			}

			if (mcVTurn != false)
			{
				noChangePositionSize.mdPositionOffset.x = -lfSizeVarX;
				noChangePositionSize.mdPositionOffset.y = -(mdOriginSize.height + lfSizeVarY);

			}

			if (mcHTurn != false && mcVTurn != false)
			{
				noChangePositionSize.mdPositionOffset.x = mdOriginSize.width;
				noChangePositionSize.mdPositionOffset.y = -(mdOriginSize.height + lfSizeVarY);
			}

			if (mcHTurn == false && mcVTurn == false)
			{
				noChangePositionSize.mdPositionOffset = D2D1::Point2F(-lfSizeVarX, 0);
			}

			break;
		}
		// ���е�
		case XuiLeftMid:
		{
			float lfHeightOffset = (lfSizeX / mdOriginSize.width)*mdOriginSize.height;

			if (mcHTurn != false)
			{
				float xTmp = 0.0f;
				if (mdOriginSize.height / 2 + lfHeightOffset / 2 < 0)
				{
					xTmp = mdOriginSize.height + lfHeightOffset / 2;
				}
				else
				{
					xTmp = -lfHeightOffset / 2;
				}

				noChangePositionSize.mdPositionOffset = D2D1::Point2F(mdOriginSize.width, xTmp);
			}
			else
			{
				noChangePositionSize.mdPositionOffset = D2D1::Point2F(noChangePositionSize.mdPositionOffset.x, -lfHeightOffset / 2);
			}

			noChangePositionSize.mdSizeVariation = D2D1::SizeF(lfSizeX, lfHeightOffset);

			break;
		}
		}

	} while (false);


	// ����ȱȱ��κ�Ŀ�Ⱥ͸߶�
	float lfAfterScaledWidth = 0.0f;
	float lfAfterScaledHeight = 0.0f;

	lfAfterScaledWidth = mdOriginSize.width + noChangePositionSize.mdSizeVariation.width;
	lfAfterScaledHeight = mdOriginSize.height + noChangePositionSize.mdSizeVariation.height;

	// ����ˮƽ��ת�ʹ�ֱ��ת��ı仯��
	if (mcHTurn != false || mcVTurn != false)
	{
		// ��ת֮��Ĵ�С
		D2D1_SIZE_F ldAfterTurnedSize;

		ldAfterTurnedSize.width = lfAfterScaledWidth > 0 ? lfAfterScaledWidth : -lfAfterScaledWidth;
		ldAfterTurnedSize.height = lfAfterScaledHeight > 0 ? lfAfterScaledHeight : -lfAfterScaledHeight;

		// ��תǰ��ı仯�� = ��ת��Ĵ�С - ԭʼ��С
		noChangePositionSize.mdSizeVariation.width = ldAfterTurnedSize.width - mdOriginSize.width;
		noChangePositionSize.mdSizeVariation.height = ldAfterTurnedSize.height - mdOriginSize.height;

	}

	// ���ݵ�ǰ״̬����һ�εķ�ת״̬���ж�ʵ���Ƿ�����ת
	if (mcLastHTurn != mcHTurn)
	{
		noChangePositionSize.mcHTurn = true;
		mcLastHTurn = mcHTurn;
	}
	else
	{
		noChangePositionSize.mcHTurn = false;
	}

	if (mcLastVTurn != mcVTurn)
	{
		noChangePositionSize.mcVTurn = true;
		mcLastVTurn = mcVTurn;
	}
	else
	{
		noChangePositionSize.mcVTurn = false;
	}

	return ERESULT_SUCCESS;

}


// ���ð˸������ʾ״̬
void CSelectFrame::SetPointVisible(bool nbFlag)
{
	/*if (nbFlag == false)
	{
		mbModifying = true;
		for (int liLoop = 0; liLoop < SF_POINT_MAX; liLoop++)
		{
			mpArrayPoint[liLoop]->SetVisible(false);
		}
	}
	else
	{
		mbModifying = false;
		for (int liLoop = 0; liLoop < SF_POINT_MAX; liLoop++)
		{
			mpArrayPoint[liLoop]->SetVisible(true);
		}
	}*/

}