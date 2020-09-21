#include "stdafx.h"
#include "cmmBaseObj.h"
#include "Einkui.h"
#include "XCtl.h"
#include "ElementImp.h"
#include "EvStaticTextImp.h"
#include "XCtl.h"
//

DEFINE_BUILTIN_NAME(StaticText)

CEvStaticText::CEvStaticText()
{
	mpswText = NULL;
	mdwColor = 0;
	mdwFontSize = 0;
	mpswFontName = NULL;
	mdDrawRect.left = mdDrawRect.top = mdDrawRect.right = mdDrawRect.bottom = 0.0f;
}

CEvStaticText::~CEvStaticText()
{
	CMM_SAFE_DELETE(mpswFontName);
	CMM_SAFE_DELETE(mpswText);
}

// ����Ԫ�أ�����֪ͨһ��Ԫ�����٣�ע�⣺��Ԫ�������յ�����Ϣ��Ӧ�ü�ʱ����Ԫ�ع�������UnregisterElement������
// �Ӷ�����Ԫ�ع�������������һ��Ԫ�ط���������Ϣ�������ٽ��Լ���Ԫ�ع�����ע���������ͷ��������
ERESULT CEvStaticText::OnElementDestroy()
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		CXuiElement::OnElementDestroy();	//���û���


		lResult = ERESULT_SUCCESS;

	} while (false);

	return lResult;
}

ULONG CEvStaticText::InitOnCreate(
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
		mpIterator->ModifyStyles(/*EITR_STYLE_CONTROL*/NULL, EITR_STYLE_MOUSE);

		leResult = ERESULT_SUCCESS;

	} while (false);

	return leResult;
}


//װ��������Դ
ERESULT CEvStaticText::LoadResource()
{
	ERESULT leResult = ERESULT_UNSUCCESSFUL;

	ICfKey* lpValue = NULL;
	LONG llLen = 0;

	do
	{
		lpValue = mpTemplete->GetSubKey(TF_ID_ST_TEXT); //��ʾ����
		if (lpValue != NULL)
		{
			llLen = lpValue->GetValueLength();
			mpswText = new wchar_t[llLen];
			BREAK_ON_NULL(mpswText);

			lpValue->GetValue(mpswText, lpValue->GetValueLength());
			CMM_SAFE_RELEASE(lpValue);
		}

		//������ɫ
		mdwColor = (DWORD)mpTemplete->QuerySubKeyValueAsLONG(TF_ID_ST_COLOR, 0xFFFFFFFF);
		mLimit = (STETXT_BMP_INIT::eSIZELIMIT)mpTemplete->QuerySubKeyValueAsLONG(TF_ID_ST_SIZE_LIMIT, 0);
		mTalign = (STETXT_BMP_INIT::eTEXTALIGN)mpTemplete->QuerySubKeyValueAsLONG(TF_ID_ST_TALIGN, 0);
		mPalign = (STETXT_BMP_INIT::ePARAALIGN)mpTemplete->QuerySubKeyValueAsLONG(TF_ID_ST_PALIGN, 0);
		mFontWidget = (STETXT_BMP_INIT::eFONTWEIGHT)mpTemplete->QuerySubKeyValueAsLONG(TF_ID_ST_FONTWIDGET, 0);

		//����
		lpValue = mpTemplete->GetSubKey(TF_ID_ST_FONT);
		if (lpValue != NULL)
		{
			llLen = lpValue->GetValueLength();
			if (llLen <= 0)
				break;

			mpswFontName = new wchar_t[llLen];
			BREAK_ON_NULL(mpswFontName);

			lpValue->GetValue(mpswFontName, lpValue->GetValueLength());
		}
		CMM_SAFE_RELEASE(lpValue);

		if (mpswFontName != NULL && mpswFontName[0] == UNICODE_NULL)
			CMM_SAFE_DELETE(mpswFontName);	//���û�ж����������

		if (mpswFontName == NULL)
		{
			llLen = wcslen(L"Tahoma") + 1;
			mpswFontName = new wchar_t[llLen];
			BREAK_ON_NULL(mpswFontName);
			wcscpy_s(mpswFontName, llLen, L"Tahoma");	//Ĭ������
		}

		//�ֺ�
		mdwFontSize = mpTemplete->QuerySubKeyValueAsLONG(TF_ID_ST_FONT_SIZE, 15);

		//��������ͼƬ
		ReCreateBmp();

		leResult = ERESULT_SUCCESS;

	} while (false);

	return leResult;
}

// �ֽ���Ϣ���ṩ��Ϣ�ֽ����Ϣ��Ӧ�Ĺ��ܣ������ʵ���ǽ���Ϣ�ֽ�Ϊ��ͬ������󣬵�����Ӧ�Ĵ����麯�������ڲ���ʶ����Ϣ��һ�ɷ���ERESULT_UNEXPECTED_MESSAGE
// �������ķ���ֵ���Զ�ͬ�����õ�npMsgָ�����Ϣ������
ERESULT CEvStaticText::ParseMessage(IEinkuiMessage* npMsg)
{
	// ʵ��ԭ�����ȵ�������ķֽ⹦�ܣ����󽫲��������Ϣ��������

	ERESULT luResult = ERESULT_UNEXPECTED_MESSAGE;

	switch (npMsg->GetMessageID())
	{
	case EACT_STATICTEXT_SET_TEXT:
	{
		//������ʾ����
		// ��ȡ��������
		wchar_t* lpswText = (wchar_t*)npMsg->GetInputData();
		SetText(lpswText);

		break;
	}
	case EACT_STATICTEXT_GET_TEXT:
	{
		//��ȡ��ʾ����
		if (npMsg->GetOutputBufferSize() != sizeof(wchar_t*))
		{
			luResult = ERESULT_WRONG_PARAMETERS;
			break;
		}

		// �����������
		wchar_t** lpOut = (wchar_t**)npMsg->GetOutputBuffer();
		*lpOut = mpswText;

		break;
	}
	case EACT_STATICTEXT_SET_TEXT_COLOR:
	{
		//����������ɫ
		if (npMsg->GetOutputBufferSize() != sizeof(LONG))
		{
			luResult = ERESULT_WRONG_PARAMETERS;
			break;
		}
		LONG * lpValue = (LONG*)npMsg->GetInputData();
		mdwColor = *lpValue;

		ReCreateBmp();	//��������ͼƬ

		luResult = ERESULT_SUCCESS;

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
ERESULT CEvStaticText::OnElementCreate(IEinkuiIterator* npIterator)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		if (CXuiElement::OnElementCreate(npIterator) != ERESULT_SUCCESS)
			break;

		//װ��һЩ��Ҫ��������Դ
		LoadResource();

		lResult = ERESULT_SUCCESS;
	} while (false);

	return lResult;
}

//������ʾ����
bool CEvStaticText::SetText(wchar_t* npswText)
{
	bool lbRet = false;

	do
	{
		BREAK_ON_NULL(npswText);

		//�̶���С������ı����ַ�������Ԫ�صĲο���СҲ����仯

		CMM_SAFE_DELETE(mpswText);	//���ԭ�����ַ�������
		int liLen = wcslen(npswText) + 1;
		mpswText = new wchar_t[liLen];
		BREAK_ON_NULL(mpswText);
		wcscpy_s(mpswText, liLen, npswText);	//Copy������

		//��������ͼƬ
		ReCreateBmp();

		lbRet = true;

	} while (false);

	return lbRet;
}

//����
ERESULT CEvStaticText::OnPaint(IEinkuiPaintBoard* npPaintBoard)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		BREAK_ON_NULL(npPaintBoard);

		if (mpBgBitmap != NULL)
			npPaintBoard->DrawBitmap(mdDrawRect, mdDrawRect, mpBgBitmap, ESPB_DRAWBMP_NEAREST);

		//if(mpBgBitmap != NULL)
		//	npPaintBoard->DrawBitmap(D2D1::Point2F(0.0f,0.0f),mpBgBitmap/*,ESPB_DRAWBMP_NEAREST*/);

		lResult = ERESULT_SUCCESS;

	} while (false);

	return lResult;
}

//��������ͼƬ
bool CEvStaticText::ReCreateBmp()
{
	bool lbRet = false;
	mdDrawRect.left = mdDrawRect.top = mdDrawRect.right = mdDrawRect.bottom = 0.0f;

	do
	{
		//�����ṹ��
		STETXT_BMP_INIT ldInit;
		ZeroMemory(&ldInit, sizeof(STETXT_BMP_INIT));
		ldInit.Text = mpswText;
		ldInit.FontName = mpswFontName;
		ldInit.FontSize = mdwFontSize;
		ldInit.TextColor = mdwColor;
		ldInit.Width = (DWORD)mpTemplete->QuerySubKeyValueAsLONG(TF_ID_ELEMENT_WIDTH, 0);
		ldInit.Height = (DWORD)mpTemplete->QuerySubKeyValueAsLONG(TF_ID_ELEMENT_HEIGHT, 0);
		ldInit.Limit = mLimit;
		ldInit.Talign = mTalign;
		ldInit.Palign = mPalign;
		ldInit.FontWeight = mFontWidget;

		CMM_SAFE_RELEASE(mpBgBitmap);	//ȥ��ԭ����ͼƬ
		mpBgBitmap = EinkuiGetSystem()->GetAllocator()->CreateImageByText(ldInit);

		BREAK_ON_NULL(mpBgBitmap);

		if (mpTemplete->QuerySubKeyValueAsLONG(TF_ID_ELEMENT_WIDTH, 0) == 0)
			mpIterator->SetSize((FLOAT)mpBgBitmap->GetWidth(), mpIterator->GetSizeY());	//û�����ù̶���С���Ǿ������ɵ�����ͼƬ��С��Ϊ�Լ��Ĵ�С

		if (mpTemplete->QuerySubKeyValueAsLONG(TF_ID_ELEMENT_HEIGHT, 0) == 0)
			mpIterator->SetSize(mpIterator->GetSizeX(), (FLOAT)mpBgBitmap->GetHeight());	//û�����ù̶���С���Ǿ������ɵ�����ͼƬ��С��Ϊ�Լ��Ĵ�С

		//Ϊ�˱�֤���ֲ������죬�����ĸ�ֵС�����ĸ�
		mdDrawRect.right = mpIterator->GetSizeX() < (FLOAT)mpBgBitmap->GetWidth() ? mpIterator->GetSizeX() : (FLOAT)mpBgBitmap->GetWidth();
		mdDrawRect.bottom = mpIterator->GetSizeY() < (FLOAT)mpBgBitmap->GetHeight() ? mpIterator->GetSizeY() : (FLOAT)mpBgBitmap->GetHeight();

		lbRet = true;

	} while (false);

	return lbRet;
}