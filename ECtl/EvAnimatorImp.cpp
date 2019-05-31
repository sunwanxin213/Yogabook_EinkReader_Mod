/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#include "stdafx.h"
#include "cmmBaseObj.h"
#include "cmmstruct.h"
#include "Einkui.h"

#include "ElementImp.h"
#include "XCtl.h"

#include "EvAnimatorImp.h"

DEFINE_BUILTIN_NAME(Animator)

CEvAnimatorImp::CEvAnimatorImp()
{
	mpFrontFrame=NULL;
	mpSweepBrush = NULL;
}

CEvAnimatorImp::~CEvAnimatorImp()
{

}

ULONG CEvAnimatorImp::InitOnCreate(
	IN IEinkuiIterator* npParent,	// ������ָ��
	IN ICfKey* npTemplete,		// npTemplete��Key ID����EID��ֵ��������EType
	IN ULONG nuEID	// �����Ϊ0��MAXULONG32����ָ����Ԫ�ص�EID; ����ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��XUIϵͳ�Զ�����
	)
{
	ERESULT leResult = ERESULT_UNSUCCESSFUL;
	//LONG llValue;

	do 
	{
		if(npTemplete == NULL)
			return ERESULT_UNSUCCESSFUL;

		//���ȵ��û���
		leResult = 	CXuiElement::InitOnCreate(npParent,npTemplete,nuEID);
		if(leResult != ERESULT_SUCCESS)
			break;

		//mpIterator->ModifyStyles(EITR_STYLE_LAZY_UPDATE);

		//llValue = mpTemplete->QuerySubKeyValueAsLONG(TF_ID_LABEL_EDGE_LEFT,0);

		leResult = ERESULT_SUCCESS;

	} while (false);

	return leResult;
}

//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
ERESULT CEvAnimatorImp::OnElementCreate(IEinkuiIterator* npIterator)
{
	ERESULT luResult = ERESULT_UNSUCCESSFUL;

	do
	{
		luResult = CXuiElement::OnElementCreate(npIterator);
		if(luResult != ERESULT_SUCCESS)
			break;

		//moMask.SetBase(false,D2D1::Point2F(160.0f,120.0f),120.0f,0.0f);
		//moMask.SetAngle(360.0f);

		luResult = ERESULT_SUCCESS;

	}while(false);

	return luResult;
}

// ����Ԫ�أ�����֪ͨһ��Ԫ�����٣�ע�⣺��Ԫ�������յ�����Ϣ��Ӧ�ü�ʱ����Ԫ�ع�������UnregisterElement������
// �Ӷ�����Ԫ�ع�������������һ��Ԫ�ط���������Ϣ�������ٽ��Լ���Ԫ�ع�����ע���������ͷ��������
ERESULT CEvAnimatorImp::OnElementDestroy()
{
	CMM_SAFE_RELEASE(mpFrontFrame);
	CMM_SAFE_RELEASE(mpSweepBrush);

	CXuiElement::OnElementDestroy();

	return ERESULT_SUCCESS;
}

// �ֽ���Ϣ���ṩ��Ϣ�ֽ����Ϣ��Ӧ�Ĺ��ܣ������ʵ���ǽ���Ϣ�ֽ�Ϊ��ͬ������󣬵�����Ӧ�Ĵ����麯�������ڲ���ʶ����Ϣ��һ�ɷ���ERESULT_UNEXPECTED_MESSAGE
// �������ķ���ֵ���Զ�ͬ�����õ�npMsgָ�����Ϣ������
ERESULT CEvAnimatorImp::ParseMessage(IEinkuiMessage* npMsg)
{
	// ʵ��ԭ�����ȵ�������ķֽ⹦�ܣ����󽫲��������Ϣ��������

	ERESULT luResult = ERESULT_UNEXPECTED_MESSAGE;

	switch (npMsg->GetMessageID())
	{
	case EACT_ANIMATOR_SET_FRAME:
		{
			if(npMsg->GetInputDataSize() < sizeof(STCTL_ANIMATOR_FRAME))
			{
				luResult = ERESULT_WRONG_PARAMETERS;
				break;
			}

			const PSTCTL_ANIMATOR_FRAME lpFrame = (const PSTCTL_ANIMATOR_FRAME)npMsg->GetInputData();

			luResult = UpdateFrame(lpFrame);
		}
		break;
	case EACT_ANIMATOR_SET_TRANSFORM:
		{
			if(npMsg->GetInputDataSize() < sizeof(STCTL_ANIMATOR_TRANSFORM))
			{
				luResult = ERESULT_WRONG_PARAMETERS;
				break;
			}

			const PSTCTL_ANIMATOR_TRANSFORM lpTrans = (const PSTCTL_ANIMATOR_TRANSFORM)npMsg->GetInputData();

			luResult = SetTransform(lpTrans);
		}
		break;
	default:
		luResult = CXuiElement::ParseMessage(npMsg); // ���û����ͬ��������ע�⣺һ��Ҫ��������ֱ�ӻ���
	}

	return luResult;
}

// ����֡
ERESULT CEvAnimatorImp::UpdateFrame(PSTCTL_ANIMATOR_FRAME npFrame)
{
	CMM_SAFE_RELEASE(mpFrontFrame);

	// ���ؿ�ȱ����ܱ�4������Ŀǰ�ٶ���Ȳ��ܴ���1�򣬸߶ȼٶ����ܴ���5000
	if((npFrame->PixelWidth/4)*4 != npFrame->PixelWidth || npFrame->PixelWidth > 10000 || npFrame->PixelHeight > 5000)
		return ERESULT_WRONG_PARAMETERS;

	mpFrontFrame = EinkuiGetSystem()->GetAllocator()->CreateBitmapFromMemory(npFrame->PixelWidth,npFrame->PixelHeight,3, npFrame->PixelWidth * 3, npFrame->PixelData);

	if(mpFrontFrame == NULL)
		return ERESULT_INSUFFICIENT_RESOURCES;

	EinkuiGetSystem()->UpdateView();

	return ERESULT_SUCCESS;
}

// �趨�任
ERESULT CEvAnimatorImp::SetTransform(PSTCTL_ANIMATOR_TRANSFORM npTransform)
{
	return ERESULT_SUCCESS;
}

//������Ϣ
ERESULT CEvAnimatorImp::OnPaint(IEinkuiPaintBoard* npPaintBoard)
{
	D2D1_RECT_F ldDestRect;
	D2D1::Matrix3x2F ldFlip(-1,0,0,1,0,0); // �Ծֲ�X=0.0f��Y�ᷭת
	D2D1::Matrix3x2F ldOld;

	// ��ȡĿ������
	ldDestRect.left = 0.0f;
	ldDestRect.top = 0.0f;
	ldDestRect.right = mpIterator->GetSizeX();
	ldDestRect.bottom = mpIterator->GetSizeY();

	{
		// ���潨����ˢ
 		if(mpSweepBrush ==NULL)
 		{
 			mpSweepBrush = EinkuiGetSystem()->CreateBrush(XuiSolidBrush,D2D1::ColorF(D2D1::ColorF::Black,1.0f));
 		}

//  		if(mpSweepBrush != NULL)
//  			npPaintBoard->DrawRect(ldDestRect,mpSweepBrush);

	}

	CExMessage::SendMessage(mpIterator->GetParent(),mpIterator,EEVT_ANIMATOR_BEFORE_PAINT,npPaintBoard,NULL,0);

//	moMask.Enter(npPaintBoard->GetD2dRenderTarget());

	// ���þ������
	npPaintBoard->GetD2dRenderTarget()->GetTransform(&ldOld);
	ldFlip = ldFlip*D2D1::Matrix3x2F::Translation(ldDestRect.left+ldDestRect.right,0.0f);	// �Ӿ���λ���ƶ���ԭ��λ��
	ldFlip = ldFlip*D2D1::Matrix3x2F(1,0,0,-1,0,0);											// �Ծֲ�Y=0.0f��X�ᷴת
	ldFlip = ldFlip*D2D1::Matrix3x2F::Translation(0.0f,ldDestRect.top+ldDestRect.bottom);	// �Ӿ���λ���ƶ���ԭ��λ��
	ldFlip = ldFlip*ldOld;			// ת����ȫ��λ��
	npPaintBoard->GetD2dRenderTarget()->SetTransform(ldFlip);

	if(mpFrontFrame != NULL)
	{
		//������ƾ��δ�С
		D2D1_RECT_F ldRect;
		ZeroMemory(&ldRect,sizeof(D2D1_RECT_F));
		if(mpIterator->GetSizeX()/mpIterator->GetSizeY() > mpFrontFrame->GetWidth() / mpFrontFrame->GetHeight())
		{
			ldRect.right = (FLOAT)mpFrontFrame->GetWidth();
			ldRect.bottom = mpIterator->GetSizeY()*mpFrontFrame->GetWidth()/mpIterator->GetSizeX();
		}
		else
		{
			ldRect.bottom = (FLOAT)mpFrontFrame->GetHeight();
			ldRect.right = mpIterator->GetSizeX()*mpFrontFrame->GetHeight()/mpIterator->GetSizeY();
		}

		npPaintBoard->DrawBitmap(ldDestRect,ldRect,mpFrontFrame,ESPB_DRAWBMP_NEAREST);
	}
	//else
 	//if(mpSweepBrush != NULL)
 	//	npPaintBoard->FillRect(ldDestRect,mpSweepBrush);

//	moMask.Leave(npPaintBoard->GetD2dRenderTarget());

	CExMessage::SendMessage(mpIterator->GetParent(),mpIterator,EEVT_ANIMATOR_AFTER_PAINT,npPaintBoard,NULL,0);

	// �ָ�����
	npPaintBoard->GetD2dRenderTarget()->SetTransform(ldOld);

	return ERESULT_SUCCESS;
}

