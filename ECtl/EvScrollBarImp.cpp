#include "stdafx.h"
#include "cmmBaseObj.h"
#include "Einkui.h"
#include "ElementImp.h"
#include "XCtl.h"
#include "EvScrollBarImp.h"
#include "EvButtonImp.h"


DEFINE_BUILTIN_NAME(ScrollBar)

CEvScrollBar::CEvScrollBar()
{
	mpBtUp = mpBtDrag = mpBtDown = NULL;
	mfSVScrollBarWidth = 0.0;
	mfSHScrollBarHeigth = 0.0;
	mbVertical = true;
	mfDestPixelPerScrollPix = 1.0f;
	mfMinBarSize = 0.0f;
}

CEvScrollBar::~CEvScrollBar()
{
	//CMM_SAFE_RELEASE(mpBtUp);
	//CMM_SAFE_RELEASE(mpBtDown);
	//CMM_SAFE_RELEASE(mpBtDrag);

}

// ����Ԫ�أ�����֪ͨһ��Ԫ�����٣�ע�⣺��Ԫ�������յ�����Ϣ��Ӧ�ü�ʱ����Ԫ�ع�������UnregisterElement������
// �Ӷ�����Ԫ�ع�������������һ��Ԫ�ط���������Ϣ�������ٽ��Լ���Ԫ�ع�����ע���������ͷ��������
ERESULT CEvScrollBar::OnElementDestroy()
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		CXuiElement::OnElementDestroy();	//���û���

		lResult = ERESULT_SUCCESS;

	} while (false);

	return lResult;
}

ULONG CEvScrollBar::InitOnCreate(
	IN IEinkuiIterator* npParent,	// ������ָ��
	IN ICfKey* npTemplete,		// npTemplete��Key ID����EID��ֵ��������EType
	IN ULONG nuEID	// �����Ϊ0��MAXULONG32����ָ����Ԫ�ص�EID; ����ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��XUIϵͳ�Զ�����
)
{
	ERESULT leResult = ERESULT_UNSUCCESSFUL;

	do
	{
		//���ȵ��û���
		leResult = CXuiElement::InitOnCreate(npParent, npTemplete, nuEID);
		if (leResult != ERESULT_SUCCESS)
			break;
		//����Button
		ICfKey * lpUp = npTemplete->GetSubKey(SCB_BT_UP);
		ICfKey * lpDown = npTemplete->GetSubKey(SCB_BT_DOWN);
		ICfKey * lpDrag = npTemplete->GetSubKey(SCB_BT_DRAG);

		//����List�������ļ��������������������
		if (lpUp)
			mpBtUp = EinkuiGetSystem()->GetAllocator()->CreateElement(mpIterator, lpUp);
		if (lpDown)
			mpBtDown = EinkuiGetSystem()->GetAllocator()->CreateElement(mpIterator, lpDown);
		if (lpDrag)
			mpBtDrag = EinkuiGetSystem()->GetAllocator()->CreateElement(mpIterator, lpDrag);


		leResult = ERESULT_SUCCESS;

	} while (false);

	return leResult;
}
ERESULT CEvScrollBar::OnCtlButtonClick(IEinkuiIterator* npSender)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		BREAK_ON_NULL(npSender);
		if (npSender->GetID() == SCB_BT_UP)
		{
			SetPosition(true);
		}
		if (npSender->GetID() == SCB_BT_DOWN)
		{
			SetPosition(false);
		}

		lResult = ERESULT_SUCCESS;
	} while (false);
	return lResult;
}

FLOAT CEvScrollBar::GetVScrollBarWidth()
{
	return mfSVScrollBarWidth;
}
//��ȡ����������ĸ߶�
FLOAT CEvScrollBar::GetHScrollBarHeigth()
{
	return mfSHScrollBarHeigth;
}
//���ù����ķ�Χ
bool CEvScrollBar::SetDeltaSize(FLOAT nfSize)
{
	mfDeltaSize = nfSize;

	//�ȵõ����Ŀɹ�������
	FLOAT mfMaxScrollPixel = 0.0;
	if (mbVertical)
		mfMaxScrollPixel = mRectSlider.bottom - mRectSlider.top - mfMinBarSize;
	else
		mfMaxScrollPixel = mRectSlider.right - mRectSlider.left - mfMinBarSize;

	//����Ŀ�����غ͹������صĶ�Ӧ��ϵ
	mfDestPixelPerScrollPix = mfDeltaSize / mfMaxScrollPixel;

	//���mfDestPixelPerScrollPix < 1.0  ����Ҫ�Ŵ󻬿�
	//��Ȼ�򽫻�����С����Сֵ
	if (mfDestPixelPerScrollPix <= 1.0)
	{
		mfDestPixelPerScrollPix = 1.0;
		if (mbVertical)
			mpBtDrag->SetSize(mpBtDrag->GetSizeX(), mfMinBarSize + mfMaxScrollPixel - mfDeltaSize);
		else
			mpBtDrag->SetSize(mfMinBarSize + mfMaxScrollPixel - mfDeltaSize, mpBtDrag->GetSizeY());
	}
	else
	{
		if (mbVertical)
			mpBtDrag->SetSize(mpBtDrag->GetSizeX(), mfMinBarSize);
		else
			mpBtDrag->SetSize(mfMinBarSize, mpBtDrag->GetSizeY());
	}


	return true;
}
bool CEvScrollBar::SetPosition(bool bUp)
{
	FLOAT lfDeta = -20.0f;
	if (bUp == false)
		lfDeta = -lfDeta;
	if (mpBtDrag)
	{
		if (mbVertical)
		{
			FLOAT lfTempPosition = mpBtDrag->GetPositionY() + lfDeta;
			if (lfTempPosition < mRectSlider.top)
				lfTempPosition = mRectSlider.top;
			else if ((lfTempPosition + mpBtDrag->GetSizeY()) > mRectSlider.bottom)
				lfTempPosition = mRectSlider.bottom - mpBtDrag->GetSizeY();

			mpBtDrag->SetPosition(mpBtDrag->GetPositionX(), lfTempPosition);
		}
		else
		{
			FLOAT lfTempPosition = mpBtDrag->GetPositionX() + lfDeta;
			if (lfTempPosition < mRectSlider.left)
				lfTempPosition = mRectSlider.left;
			else if ((lfTempPosition + mpBtDrag->GetSizeX()) > mRectSlider.right)
				lfTempPosition = mRectSlider.right - mpBtDrag->GetSizeX();

			mpBtDrag->SetPosition(lfTempPosition, mpBtDrag->GetPositionY());

		}
	}
	EinkuiGetSystem()->GetElementManager()->SimplePostMessage(mpIterator,
		EACT_SLIDERBUTTON_DRAGING, NULL, 0);

	return true;
}
//װ��������Դ
ERESULT CEvScrollBar::LoadResource()
{
	ERESULT leResult = ERESULT_UNSUCCESSFUL;

	do
	{
		LONG lHor = mpTemplete->QuerySubKeyValueAsLONG(SCROLLBAR_HOR, 0);
		if (lHor != 0)
			mbVertical = false;

		leResult = ERESULT_SUCCESS;

	} while (false);


	return leResult;
}

// �ֽ���Ϣ���ṩ��Ϣ�ֽ����Ϣ��Ӧ�Ĺ��ܣ������ʵ���ǽ���Ϣ�ֽ�Ϊ��ͬ������󣬵�����Ӧ�Ĵ����麯�������ڲ���ʶ����Ϣ��һ�ɷ���ERESULT_UNEXPECTED_MESSAGE
// �������ķ���ֵ���Զ�ͬ�����õ�npMsgָ�����Ϣ������
ERESULT CEvScrollBar::ParseMessage(IEinkuiMessage* npMsg)
{
	// ʵ��ԭ�����ȵ�������ķֽ⹦�ܣ����󽫲��������Ϣ��������

	ERESULT luResult = ERESULT_UNEXPECTED_MESSAGE;

	switch (npMsg->GetMessageID())
	{

	case EACT_SLIDERBUTTON_DRAGING:
	{
		//����Position,�������ڷ���λ��Ϣ
		if (mpBtDrag)
		{
			if (mbVertical)
			{
				FLOAT lfy = mpBtDrag->GetPositionY();
				if (mpBtUp)
					lfy -= mpBtUp->GetSizeY();
				lfy *= mfDestPixelPerScrollPix;
				EinkuiGetSystem()->GetElementManager()->SimplePostMessage(mpIterator->GetParent(),
					EACT_SCROLLBAR_VSCROLL_THUMB_POSITION, &lfy, sizeof(FLOAT));
			}
			else
			{
				FLOAT lfX = mpBtDrag->GetPositionX();
				if (mpBtUp)
					lfX -= mpBtUp->GetSizeX();
				lfX *= mfDestPixelPerScrollPix;
				EinkuiGetSystem()->GetElementManager()->SimplePostMessage(mpIterator->GetParent(),
					EACT_SCROLLBAR_HSCROLL_THUMB_POSITION, &lfX, sizeof(FLOAT));
			}
		}
		break;
	}
	case EACT_SCROLLBAR_VSCROLL_SET_SLIDER_POSTION:
	{
		if (npMsg->GetInputDataSize() != sizeof(FLOAT))
		{
			luResult = ERESULT_WRONG_PARAMETERS;
			break;
		}
		FLOAT* lpValue = (FLOAT*)npMsg->GetInputData();
		FLOAT lfy = (*lpValue) / mfDestPixelPerScrollPix;
		if (mpBtDrag)
		{
			if (lfy < mRectSlider.top)
				lfy = mRectSlider.top;
			else if (lfy > (mRectSlider.bottom - mpBtDrag->GetSizeY()))
				lfy = mRectSlider.bottom - mpBtDrag->GetSizeY();
			else
				lfy += mRectSlider.top;

			mpBtDrag->SetPosition(mpBtDrag->GetPositionX(), lfy);
		}
		break;
	}
	case EACT_SCROLLBAR_HSCROLL_SET_SLIDER_POSTION:
	{
		if (npMsg->GetInputDataSize() != sizeof(FLOAT))
		{
			luResult = ERESULT_WRONG_PARAMETERS;
			break;
		}
		FLOAT* lpValue = (FLOAT*)npMsg->GetInputData();
		FLOAT lfX = (*lpValue) / mfDestPixelPerScrollPix;
		if (mpBtDrag)
		{
			if (lfX < mRectSlider.left)
				lfX = mRectSlider.left;
			else if (lfX > (mRectSlider.right - mpBtDrag->GetSizeX()))
				lfX = mRectSlider.right - mpBtDrag->GetSizeX();
			else
				lfX += mRectSlider.left;


			mpBtDrag->SetPosition(lfX, mpBtDrag->GetPositionY());
		}
		break;
	}
	case EACT_SCROLLBAR_HVSCROLL_SET_DELTA_SIZE:
	{
		//���ù�����Χ
		if (npMsg->GetInputDataSize() != sizeof(FLOAT))
		{
			luResult = ERESULT_WRONG_PARAMETERS;
			break;
		}
		SetDeltaSize(*(FLOAT*)npMsg->GetInputData());

		break;
	}

	case EACT_SCROLLBAR_HSCROLL_GET_HEIGTH:
	{
		if (npMsg->GetOutputBufferSize() != sizeof(float))
		{
			luResult = ERESULT_WRONG_PARAMETERS;
			break;
		}

		// �����������
		float* lpOut = (float*)npMsg->GetOutputBuffer();

		*lpOut = GetHScrollBarHeigth();

		npMsg->SetOutputDataSize(sizeof(float));
		break;

	}
	case EACT_SCROLLBAR_VSCROLL_GET_WIDTH:
	{
		if (npMsg->GetOutputBufferSize() != sizeof(float))
		{
			luResult = ERESULT_WRONG_PARAMETERS;
			break;
		}

		// �����������
		float* lpOut = (float*)npMsg->GetOutputBuffer();

		*lpOut = GetVScrollBarWidth();

		npMsg->SetOutputDataSize(sizeof(float));
		break;

	}
	case EACT_SCROLLBAR_HVSCROLL_RELACATION:
	{
		Relocation();
		break;
	}
	case EACT_SCROLLBAR_BT_CLICK:
	{
		//ģ�����°�ť���
		bool lbUp = true;
		CExMessage::GetInputData(npMsg, lbUp, lbUp);

		SetPosition(lbUp);

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

//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
ERESULT CEvScrollBar::OnElementCreate(IEinkuiIterator* npIterator)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		if (CXuiElement::OnElementCreate(npIterator) != ERESULT_SUCCESS)
			break;

		//װ��һЩ��Ҫ��������Դ
		//LoadResource();
		mpIterator->ModifyStyles(EITR_STYLE_TOP);
		LoadResource();

		//Relocation();
		if (mpBtDrag)
		{
			if (mbVertical)
			{
				//��������
				mfSVScrollBarWidth = mpIterator->GetSizeX();
				LONG lnStyle = ES_SLIDER_BUTTON_STYLE_VER;
				//EinkuiGetSystem()->GetElementManager()->SimplePostMessage(mpBtDrag,EACT_SLIDERBUTTON_SET_STYLE,&lnStyle,sizeof(LONG));
				CExMessage::PostMessage(mpBtDrag, mpIterator, EACT_SLIDERBUTTON_SET_STYLE, lnStyle);
			}
			else
			{
				//���ú���
				mfSHScrollBarHeigth = mpIterator->GetSizeY();
				LONG lnStyle = ES_SLIDER_BUTTON_STYLE_HOR;
				//EinkuiGetSystem()->GetElementManager()->SimplePostMessage(mpBtDrag,EACT_SLIDERBUTTON_SET_STYLE,&lnStyle,sizeof(LONG));
				CExMessage::PostMessage(mpBtDrag, mpIterator, EACT_SLIDERBUTTON_SET_STYLE, lnStyle);
			}
		}

		lResult = ERESULT_SUCCESS;

	} while (false);

	return lResult;
}
//Ԫ��Enbale
ERESULT CEvScrollBar::OnElementEnable(bool nbIsEnable)
{
	if (mpBtUp) mpBtUp->SetEnable(nbIsEnable);
	if (mpBtDown) mpBtDown->SetEnable(nbIsEnable);
	if (mpBtDrag) mpBtDrag->SetEnable(nbIsEnable);
	return ERESULT_SUCCESS;
}
//Ԫ�زο��ߴ緢���仯
ERESULT CEvScrollBar::OnElementResized(D2D1_SIZE_F nNewSize)
{
	Relocation();
	return ERESULT_SUCCESS;
}
//�޸�Ԫ�ز���
void CEvScrollBar::Relocation(void)
{
	D2D1_POINT_2F ldPoint;	//�޸�Ԫ��λ��

	do
	{
		BREAK_ON_NULL(mpBtDrag);


		ldPoint.x = ldPoint.y = 0.0f;
		//��������ϰ�ť
		if (mpBtUp)
		{
			if (mbVertical)
				ldPoint.y = mpBtUp->GetPositionY();
			else
				ldPoint.x = mpBtUp->GetPositionX();
			mpBtUp->SetPosition(ldPoint);
		}
		//��������°�ť
		if (mpBtDown)
		{
			if (mbVertical)
				ldPoint.y = mpIterator->GetSizeY() - mpBtDown->GetSizeY();
			else
				ldPoint.x = mpIterator->GetSizeX() - mpBtDown->GetSizeX();
			mpBtDown->SetPosition(ldPoint);
		}

		//���û��۵�λ�úʹ�С
		if (mbVertical)
		{
			ldPoint.x = mpBtDrag->GetPositionX();
			ldPoint.y = 0;
			if (mpBtUp)
				ldPoint.y += mpBtUp->GetSizeY();	//������

			FLOAT lfHeigth = mpIterator->GetSizeY();
			if (mpBtUp)
				lfHeigth -= mpBtUp->GetSizeY();
			if (mpBtDown)
				lfHeigth -= mpBtDown->GetSizeY();

			mRectSlider.left = ldPoint.x;
			mRectSlider.top = ldPoint.y;
			mRectSlider.right = mRectSlider.left + mpIterator->GetSizeX();
			mRectSlider.bottom = mRectSlider.top + lfHeigth;

		}
		else
		{
			ldPoint.x = 0;
			ldPoint.y = mpBtDrag->GetPositionY();
			if (mpBtUp)
				ldPoint.x += mpBtUp->GetSizeX();	//������
			FLOAT lfWidth = mpIterator->GetSizeX();
			if (mpBtUp)
				lfWidth -= mpBtUp->GetSizeX();
			if (mpBtDown)
				lfWidth -= mpBtDown->GetSizeX();
			mRectSlider.left = ldPoint.x;
			mRectSlider.top = ldPoint.y;
			mRectSlider.right = mRectSlider.left + lfWidth;
			mRectSlider.bottom = mRectSlider.top + mpIterator->GetSizeY();
		}

		//���û����λ�úͻ�����Χ����
		if (mpBtDrag)
		{
			if (mbVertical)
			{
				mpBtDrag->SetPosition(mRectSlider.left, mRectSlider.top);
				if (mfMinBarSize == 0.0f)
					mfMinBarSize = mpBtDrag->GetSizeY();
				//EinkuiGetSystem()->GetElementManager()->SimplePostMessage(mpBtDrag,EACT_SLIDERBUTTON_SET_SLIDERRECT,&mRectSlider,sizeof(D2D1_RECT_F));
				CExMessage::PostMessage(mpBtDrag, mpIterator, EACT_SLIDERBUTTON_SET_SLIDERRECT, mRectSlider);
			}
			else
			{
				mpBtDrag->SetPosition(mRectSlider.left, mpBtDrag->GetPositionY());
				if (mfMinBarSize == 0.0f)
					mfMinBarSize = mpBtDrag->GetSizeX();
				//EinkuiGetSystem()->GetElementManager()->SimplePostMessage(mpBtDrag,EACT_SLIDERBUTTON_SET_SLIDERRECT,&mRectSlider,sizeof(D2D1_RECT_F));
				CExMessage::PostMessage(mpBtDrag, mpIterator, EACT_SLIDERBUTTON_SET_SLIDERRECT, mRectSlider);
			}
		}



	} while (false);
}

//��갴��
ERESULT CEvScrollBar::OnMousePressed(const STEMS_MOUSE_BUTTON* npInfo)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		BREAK_ON_NULL(npInfo);
		if (mpIterator->IsEnable() == false)
			break;	//����ǽ���״̬���Ͳ���������

		if (MOUSE_LB(npInfo->ActKey) == false)  //��������������Ͳ�����
			break;

		if (npInfo->Presssed != false)
		{
			//��갴�£����������mBarRect ����
			if (npInfo->Position.x < mRectSlider.left ||
				npInfo->Position.x > mRectSlider.right ||
				npInfo->Position.y < mRectSlider.top ||
				npInfo->Position.y > mRectSlider.bottom)
				break;
			FLOAT lf = 0.0;
			if (mbVertical)
			{
				FLOAT lfPos = npInfo->Position.y;

				if (lfPos > mRectSlider.bottom - mpBtDrag->GetSizeY())
					lfPos = mRectSlider.bottom - mpBtDrag->GetSizeY();


				lf = (lfPos - mRectSlider.top) * mfDestPixelPerScrollPix;
				//EinkuiGetSystem()->GetElementManager()->SimplePostMessage(mpIterator,EACT_SLIDERBAR_SET_POSITION,&lf,sizeof(FLOAT));
				CExMessage::PostMessage(mpIterator, mpIterator, EACT_SCROLLBAR_VSCROLL_SET_SLIDER_POSTION, lf);
				CExMessage::PostMessage(mpIterator->GetParent(), mpIterator, EACT_SCROLLBAR_VSCROLL_THUMB_POSITION, lf);
			}
			else
			{
				FLOAT lfPos = npInfo->Position.x;

				if (lfPos > mRectSlider.right - mpBtDrag->GetSizeX())
					lfPos = mRectSlider.right - mpBtDrag->GetSizeX();
				lf = (lfPos - mRectSlider.left) * mfDestPixelPerScrollPix;
				//EinkuiGetSystem()->GetElementManager()->SimplePostMessage(mpIterator,EACT_SLIDERBAR_SET_POSITION,&lf,sizeof(FLOAT));
				CExMessage::PostMessage(mpIterator, mpIterator, EACT_SCROLLBAR_HSCROLL_SET_SLIDER_POSTION, lf);
				CExMessage::PostMessage(mpIterator->GetParent(), mpIterator, EACT_SCROLLBAR_HSCROLL_THUMB_POSITION, lf);
			}

			//EinkuiGetSystem()->GetElementManager()->SimplePostMessage(mpIterator->GetParent(),EACT_SLIDERBAR_THUMB_POSITION,&lf,sizeof(FLOAT));

		}



		lResult = ERESULT_SUCCESS;

	} while (false);

	return lResult;
}

// ��������
ERESULT CEvScrollBar::OnMouseOwnerTest(const D2D1_POINT_2F& rPoint)
{
	ERESULT luResult = ERESULT_SUCCESS;

	do
	{
		if (mpIterator->IsVisible() == false)
			break;

		//����ǻ�������
		if (rPoint.x < 0.0f ||
			(UINT)rPoint.x >= mpIterator->GetSizeX() ||
			rPoint.y < 0.0f ||
			(UINT)rPoint.y >= mpIterator->GetSizeY())
			break;

		luResult = ERESULT_MOUSE_OWNERSHIP;

	} while (false);

	return luResult;
}
//������Ϣ
ERESULT CEvScrollBar::OnPaint(IEinkuiPaintBoard* npPaintBoard)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		BREAK_ON_NULL(npPaintBoard);

		// ֻ������ʱʱ���Ż滭��������add by colin
		if (false != mpIterator->IsEnable())
		{
			if (mpBgBitmap != NULL)
				npPaintBoard->DrawBitmap(D2D1::RectF(0, 0, mpIterator->GetSizeX(), mpIterator->GetSizeY()),
					mpBgBitmap,
					ESPB_DRAWBMP_EXTEND);
		}



		lResult = ERESULT_SUCCESS;

	} while (false);

	return lResult;
}