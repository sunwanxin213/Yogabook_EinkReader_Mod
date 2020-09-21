#include "stdafx.h"
#include "cmmBaseObj.h"
#include "Einkui.h"
#include "XCtl.h"
#include "ElementImp.h"
#include "EvButtonImp.h"
#include "EvCheckButtonImp.h"


//using namespace D2D1;
DEFINE_BUILTIN_NAME(CheckButton)

// ֻ���ڱ������ó�ʼֵ����ָ����ΪNULL�����п���ʧ�ܵ������֮������㶼Ӧ����InitOnCreate�н���
CEvCheckButton::CEvCheckButton()
{

}

// �����ͷų�Ա����
CEvCheckButton::~CEvCheckButton()
{

}

ULONG CEvCheckButton::InitOnCreate(
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

		//�����Լ�������
		mpIterator->ModifyStyles(/*EITR_STYLE_CONTROL|*/EITR_STYLE_DRAG);

		LoadResource();

		leResult = ERESULT_SUCCESS;

	} while (false);

	return leResult;
}

//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
ERESULT CEvCheckButton::OnElementCreate(IEinkuiIterator* npIterator)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		if (CXuiElement::OnElementCreate(npIterator) != ERESULT_SUCCESS)
			break;

		//��������λ��
		if (mpBgBitmap != NULL && mpTextBitmap != NULL)
		{
			mdTextDestRect.left = mdFrameSize.width + 5.0f;
			int liTemp = mpTextBitmap->GetHeight();
			mdTextDestRect.top = (mdFrameSize.width - mpTextBitmap->GetHeight()) / 2.0f - 1.0f;
			mdTextDestRect.right = mdTextDestRect.left + mpTextBitmap->GetWidth();
			mdTextDestRect.bottom = mdTextDestRect.top + mpTextBitmap->GetHeight();

			//���ø�Ӧ����
			mdAcionSize.width = mdTextDestRect.right;
			mdAcionSize.height = mdTextDestRect.bottom;
		}

		if (mfTextTop - 1.0f >= 0.5f)
			mdTextDestRect.top = mfTextTop;

		ICfKey* lpCheckedKey = mpTemplete->OpenKey(TF_ID_CHECK_BT_CHECKED);
		if (lpCheckedKey != NULL)
		{
			//Ĭ��ѡ��
			SetChecked(true);
			CMM_SAFE_RELEASE(lpCheckedKey);
		}

		lResult = ERESULT_SUCCESS;
	} while (false);

	return lResult;
}


//����
ERESULT CEvCheckButton::OnPaint(IEinkuiPaintBoard* npPaintBoard)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		BREAK_ON_NULL(npPaintBoard);

		if (mpBgBitmap != NULL)
		{
			//���Ʊ���ͼ
			float lfX = 0;
			ULONG lulMethod = ESPB_DRAWBMP_LINEAR;
			LONG llIndex = GetCurrentStatesArrayIndex();		//��ȡ��ǰ״̬ͼƬ��Ϣ���ڵ������±�

			if (llIndex != -1)
			{
				lfX = (mdArrayFrame[llIndex].Index + mlCurrentPage) * mdFrameSize.width; //���ĸ�λ�ÿ�ʼ��ʾ

				npPaintBoard->DrawBitmap(D2D1::RectF(0.0f, 0.0f, mpIterator->GetSizeX(), mpIterator->GetSizeY()),
					D2D1::RectF(lfX, 0, lfX + mdFrameSize.width, mdFrameSize.height),
					mpBgBitmap,
					ESPB_DRAWBMP_EXTEND
				);
			}
		}

		if (mpTextBitmap != NULL)
		{
			//��������
			FLOAT lfValue = 0.0f;
			npPaintBoard->DrawBitmap(D2D1::RectF(mdTextDestRect.left + lfValue, mdTextDestRect.top + lfValue, mdTextDestRect.right + lfValue, mdTextDestRect.bottom + lfValue),
				mpTextBitmap,
				ESPB_DRAWBMP_NEAREST
			);
		}

		lResult = ERESULT_SUCCESS;

	} while (false);

	return lResult;
}


// ��������
ERESULT CEvCheckButton::OnMouseOwnerTest(const D2D1_POINT_2F& rPoint)
{
	ERESULT luResult = ERESULT_SUCCESS;

	do
	{
		if (mpIterator->IsVisible() == false)
			break;


		if (mdAcionSize.width > 1.0f && mdAcionSize.height > 1.0f)
		{
			//����и�Ӧ�������Ը�Ӧ��Ϊ��
			if (rPoint.x < 0.0f || rPoint.x >= mdAcionSize.width || rPoint.y < 0.0f || rPoint.y >= mdAcionSize.height)
				break;
		}
		else if (mpBgBitmap != NULL)
		{
			//�б���ͼ��ʱ��
			if (rPoint.x < 0.0f || (UINT)rPoint.x >= mpIterator->GetSizeX() || rPoint.y < 0.0f || (UINT)rPoint.y >= mpIterator->GetSizeY())
				break;

			float lfX = 0;
			LONG llIndex = GetCurrentStatesArrayIndex();		//��ȡ��ǰ״̬ͼƬ��Ϣ���ڵ������±�
			if (llIndex < 0)
				break;
			lfX = (mdArrayFrame[llIndex].Index + mlCurrentPage) * mdFrameSize.width;  	//���ĸ�λ�ÿ�ʼ��ʾ

			D2D1_POINT_2F ldPoint = CExPoint::BigToOldPoint(mdFrameSize, mpIterator->GetSize(), D2D1::Point2(rPoint.x, rPoint.y), ESPB_DRAWBMP_EXTEND, D2D1::Point2((FLOAT)mpBgBitmap->GetExtnedLineX(), (FLOAT)mpBgBitmap->GetExtnedLineY()));
			//ͨ������Alphaֵ���????
			DWORD luPixel;
			if (ERESULT_SUCCEEDED(mpBgBitmap->GetPixel(DWORD(lfX + ldPoint.x), (DWORD)ldPoint.y, luPixel)))
			{
				if (luPixel != 1)
					break;
			}
		}
		else if (mpTextBitmap != NULL)
		{
			//ֻ�����ֵ�ʱ��
			if (rPoint.x < 0.0f || (UINT)rPoint.x >= mpTextBitmap->GetWidth() || rPoint.y < 0.0f || (UINT)rPoint.y >= mpTextBitmap->GetHeight())
				break;
		}
		else
		{
			break;
		}

		luResult = ERESULT_MOUSE_OWNERSHIP;

	} while (false);

	return luResult;
}

//��λ����ͼƬ��ʾλ��
void CEvCheckButton::RelocateText(void)
{

}