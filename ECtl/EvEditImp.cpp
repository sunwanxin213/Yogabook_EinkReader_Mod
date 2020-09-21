#include "stdafx.h"
#include "cmmBaseObj.h"
#include "cmmstruct.h"
#include "Einkui.h"

#include "math.h"
#include "ElementImp.h"
#include "XCtl.h"

#include "Dwrite.h"
#include "EvEditImp.h"

DEFINE_BUILTIN_NAME(Edit)

CEvEditImp::CEvEditImp()
{
	mlCursorAt = 0;
	mlViewBegin = 0;
	mlViewEnd = 0;
	mlSelBegin = -1;
	mlSelEnd = -1;
	mlLimit = -1;

	mpTextFormat = NULL;
	mpBrush[0] = NULL;
	mpBrush[1] = NULL;
	mpBrush[2] = NULL;
	mpBrush[3] = NULL;
	mpTextLayout = NULL;

	mbDirty = true;
	mbInsertMode = false;
	mlBlinking = -1;
	mdValidRect.left = mdValidRect.right = 0;

	mbCompletion = false;
}

CEvEditImp::~CEvEditImp()
{
	ClearTextLayout();
	moText.Clear();

	CMM_SAFE_RELEASE(mpTextFormat);
}

ULONG CEvEditImp::InitOnCreate(
	IN IEinkuiIterator* npParent,	// ������ָ��
	IN ICfKey* npTemplete,		// npTemplete��Key ID����EID��ֵ��������EType
	IN ULONG nuEID	// �����Ϊ0��MAXULONG32����ָ����Ԫ�ص�EID; ����ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��XUIϵͳ�Զ�����
)
{
	ERESULT leResult = ERESULT_UNSUCCESSFUL;

	do
	{
		if (npTemplete == NULL)
			return ERESULT_UNSUCCESSFUL;

		//���ȵ��û���
		leResult = CXuiElement::InitOnCreate(npParent, npTemplete, nuEID);
		if (leResult != ERESULT_SUCCESS)
			break;

		mpIterator->ModifyStyles(EITR_STYLE_DRAG | EITR_STYLE_KEYBOARD | EITR_STYLE_LAZY_UPDATE | EITR_STYLE_COMMAND);
		leResult = ERESULT_SUCCESS;

	} while (false);


	return leResult;
}

//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
ERESULT CEvEditImp::OnElementCreate(IEinkuiIterator* npIterator)
{
	ERESULT luResult = ERESULT_UNSUCCESSFUL;
	ICfKey* lpValueKey = NULL;

	do
	{
		luResult = CXuiElement::OnElementCreate(npIterator);
		if (luResult != ERESULT_SUCCESS)
			break;

		lpValueKey = mpTemplete->GetSubKey(L"Value");
		if (lpValueKey != NULL && lpValueKey->GetValueLength() > 0)
		{
			wchar_t lswValue[MAX_PATH];

			int liSize = lpValueKey->GetValue(lswValue, sizeof(wchar_t)*MAX_PATH) / sizeof(wchar_t);
			for (int i = 0; i < liSize; i++)
				moText.Insert(-1, lswValue[i]);

			for (int i = liSize - 1; i >= 0; i--)
			{
				if (moText[i] == UNICODE_NULL)
					moText.RemoveByIndex(i);
			}

		}

		CXuiElement::mhInnerCursor = LoadCursor(NULL, IDC_IBEAM);

		luResult = ERESULT_SUCCESS;
	} while (false);

	CMM_SAFE_RELEASE(lpValueKey);
	return luResult;
}

// ����Ԫ�أ�����֪ͨһ��Ԫ�����٣�ע�⣺��Ԫ�������յ�����Ϣ��Ӧ�ü�ʱ����Ԫ�ع�������UnregisterElement������
// �Ӷ�����Ԫ�ع�������������һ��Ԫ�ط���������Ϣ�������ٽ��Լ���Ԫ�ع�����ע���������ͷ��������
ERESULT CEvEditImp::OnElementDestroy()
{
	ERESULT luResult = ERESULT_UNSUCCESSFUL;

	do
	{
		CXuiElement::OnElementDestroy();

		luResult = ERESULT_SUCCESS;

	} while (false);

	return luResult;
}

//���û�����
ERESULT CEvEditImp::OnElementEnable(bool nbIsEnable)
{
	if (nbIsEnable == false)
	{
		mpIterator->ReleaseKeyBoardFocus();

		mpIterator->ModifyStyles(0, EITR_STYLE_KEYBOARD);
	}
	else
		mpIterator->ModifyStyles(EITR_STYLE_KEYBOARD);


	return ERESULT_SUCCESS;
}

// ��������
ERESULT CEvEditImp::OnMouseOwnerTest(const D2D1_POINT_2F& rPoint)
{
	ERESULT luResult = CXuiElement::OnMouseOwnerTest(rPoint);

	if (ERESULT_FAILED(luResult) || luResult == ERESULT_MOUSE_OWNERSHIP)
		return luResult;

	if (!(rPoint.x < mdValidRect.left || rPoint.x >= mdValidRect.right || rPoint.y < mdValidRect.top || rPoint.y >= mdValidRect.bottom))
		return ERESULT_MOUSE_OWNERSHIP;

	return ERESULT_SUCCESS;
}


// �ֽ���Ϣ���ṩ��Ϣ�ֽ����Ϣ��Ӧ�Ĺ��ܣ������ʵ���ǽ���Ϣ�ֽ�Ϊ��ͬ������󣬵�����Ӧ�Ĵ����麯�������ڲ���ʶ����Ϣ��һ�ɷ���ERESULT_UNEXPECTED_MESSAGE
// �������ķ���ֵ���Զ�ͬ�����õ�npMsgָ�����Ϣ������
ERESULT CEvEditImp::ParseMessage(IEinkuiMessage* npMsg)
{
	// ʵ��ԭ�����ȵ�������ķֽ⹦�ܣ����󽫲��������Ϣ��������

	ERESULT luResult = ERESULT_UNEXPECTED_MESSAGE;

	switch (npMsg->GetMessageID())
	{
	case EMSG_CHAR_INPUT:
	{
		if (npMsg->GetInputData() != NULL && npMsg->GetInputDataSize() == sizeof(STEMS_CHAR_INPUT))
			luResult = OnChar((PSTEMS_CHAR_INPUT)npMsg->GetInputData());
		else
			luResult = ERESULT_WRONG_PARAMETERS;
	}
	break;
	case EACT_EDIT_SET_TEXT:
	{
		const wchar_t* lswInputString = (const wchar_t*)npMsg->GetInputData();

		luResult = OnSetText(lswInputString);
	}
	break;
	case EACT_EDIT_GET_TEXT:
	{
		wchar_t* lpTextBuf = (wchar_t*)npMsg->GetOutputBuffer();
		LONG llCharCount = npMsg->GetOutputBufferSize() / sizeof(wchar_t);
		luResult = OnGetText(lpTextBuf, llCharCount);
	}
	break;
	case EACT_EDIT_GET_TEXT_LENGTH:
	{
		if (npMsg->GetOutputBuffer() == NULL || npMsg->GetOutputBufferSize() < sizeof(ULONG))
			luResult = ERESULT_WRONG_PARAMETERS;
		else
		{
			*(ULONG*)npMsg->GetOutputBuffer() = moText.Size();
			npMsg->SetOutputDataSize(sizeof(ULONG));
			luResult = ERESULT_SUCCESS;
		}
	}
	break;
	case EACT_EDIT_NUMBER_ONLY:
	{
		if (npMsg->GetInputDataSize() != sizeof(LONG))
			luResult = ERESULT_WRONG_PARAMETERS;
		else
		{
			if (*(LONG*)npMsg->GetInputData() != 0)
				CXuiElement::SetFlags(EEDT_FLAG_NUMBER, true);
			else
				CXuiElement::SetFlags(EEDT_FLAG_NUMBER, false);

			luResult = ERESULT_SUCCESS;
		}
	}
	break;
	case EACT_EDIT_PASSWORD_MODE:
	{
		if (npMsg->GetInputDataSize() != sizeof(LONG))
			luResult = ERESULT_WRONG_PARAMETERS;
		else
		{
			if (*(LONG*)npMsg->GetInputData() != 0)
			{
				mpIterator->ModifyStyles(EITR_STYLE_DISABLE_IME);
				CXuiElement::SetFlags(EEDT_FLAG_PASSWORD, true);
			}
			else
			{
				mpIterator->ModifyStyles(0, EITR_STYLE_DISABLE_IME);
				CXuiElement::SetFlags(EEDT_FLAG_PASSWORD, false);
			}

			//mpIterator->SetKeyBoardFocus(); �ƺ���Ӧ�����ý���
			UpdateView();
			luResult = ERESULT_SUCCESS;
		}
	}
	break;
	case EACT_EDIT_SET_LENGTH_LIMIT:
	{
		if (npMsg->GetInputDataSize() != sizeof(LONG))
			luResult = ERESULT_WRONG_PARAMETERS;
		else
		{
			mlLimit = *(LONG*)npMsg->GetInputData();

			if (mlLimit >= 0)
			{
				while (moText.Size() > mlLimit)
				{
					moText.RemoveByIndex(mlLimit);
				}
			}

			luResult = ERESULT_SUCCESS;
		}
	}
	break;
	case EACT_EDIT_GET_SELECTION:
	{
		if (npMsg->GetOutputBuffer() == NULL || npMsg->GetOutputBufferSize() < sizeof(STCTL_EDIT_SELECTION))
			luResult = ERESULT_WRONG_PARAMETERS;
		else
		{
			PSTCTL_EDIT_SELECTION lpSel = (PSTCTL_EDIT_SELECTION)npMsg->GetOutputBuffer();

			lpSel->Chars = moText.Size();
			if (mlSelEnd > 0)
				lpSel->SelCount = mlSelEnd - mlSelBegin;
			else
				lpSel->SelCount = 0;
			lpSel->SelBegin = mlSelBegin;
			npMsg->SetOutputDataSize(sizeof(STCTL_EDIT_SELECTION));
			luResult = ERESULT_SUCCESS;
		}
	}
	break;
	case EACT_EDIT_SET_SELECTION:
	{
		PSTCTL_EDIT_SELECTION lpSel;

		lpSel = (PSTCTL_EDIT_SELECTION)npMsg->GetInputData();
		if (npMsg->GetInputDataSize() != sizeof(STCTL_EDIT_SELECTION) || lpSel->SelBegin < 0 || lpSel->SelCount < -1 || (lpSel->SelBegin + lpSel->SelCount)>moText.Size())
		{
			luResult = ERESULT_WRONG_PARAMETERS;
			break;
		}
		if (-1 == lpSel->SelCount)			// ȫѡ�� add by Colin
		{
			mlSelBegin = 0;
			mlSelEnd = moText.Size();
			mlCursorAt = mlSelEnd;
		}
		else
		{
			mlSelBegin = lpSel->SelBegin;
			mlSelEnd = mlSelBegin + lpSel->SelCount;
			mlCursorAt = mlSelEnd;
		}


		UpdateView();
		luResult = ERESULT_SUCCESS;
	}
	break;
	case EMSG_DISCARD_DEVICE_RESOURCE:
		ReleaseDeviceResource();
		luResult = ERESULT_SUCCESS;
		break;
	default:
		luResult = CXuiElement::ParseMessage(npMsg); // ���û����ͬ��������ע�⣺һ��Ҫ��������ֱ�ӻ���
	}

	return luResult;
}

//Set Text
ERESULT CEvEditImp::OnSetText(const wchar_t* nswText)
{
	moText.Clear();
	mlSelBegin = mlSelEnd = -1;
	mlCursorAt = 0;

	if (nswText != NULL)
	{
		for (int i = 0; nswText[i] != UNICODE_NULL; i++)
		{
			moText.Insert(-1, nswText[i]);
		}
	}

	PostMessageToParent(EEVT_EDIT_CONTENT_MODIFIED, CExMessage::DataInvalid);

	UpdateView();

	return ERESULT_SUCCESS;
}

//Get Text
ERESULT CEvEditImp::OnGetText(wchar_t* nswTextBuf, LONG nlCharCount)
{
	if (nlCharCount < moText.Size() + 1 || nswTextBuf == NULL)
		return ERESULT_INSUFFICIENT_RESOURCES;

	for (int i = 0; i < moText.Size(); i++)
	{
		nswTextBuf[i] = moText[i];
	}

	nswTextBuf[moText.Size()] = UNICODE_NULL;

	return ERESULT_SUCCESS;
}

// ��ˢ��
void  CEvEditImp::OnLazyUpdate(
	PSTEMG_LAZY_UPDATE npLazyUpdate
)
{
	if (mlBlinking >= 0 && npLazyUpdate->Updated % 3 == 0)
	{
		if (InterlockedIncrement(&mlBlinking) >= 2)
			InterlockedExchange(&mlBlinking, 0);

		EinkuiGetSystem()->UpdateView();
	}
}

//���̽����û���ʧȥ
void CEvEditImp::OnKeyBoardFocus(PSTEMS_STATE_CHANGE npState)
{
	LONG llValue;
	if (npState->State != 0)
	{
		mlBlinking = 0;
		llValue = 1;

		// ѡ��ʱȫѡ�У���������ݣ���ȫ��ѡ��  add by colin
		if (0 != mpTemplete->QuerySubKeyValueAsLONG(L"SelAllOnFocus", 0) && 0 != moText.Size())
		{
			mlSelBegin = 0;
			mlSelEnd = moText.Size();
			mlCursorAt = mlSelEnd;
		}

		// ����Ime������ڱ������֮��
		mpIterator->SetIMECompositionWindow(D2D1::Point2F(mdLayoutRect.left + 10.0f, mdLayoutRect.bottom + 10.0f));

	}
	else
	{
		mlBlinking = -1000;
		llValue = 0;
	}

	PostMessageToParent(EEVT_EDIT_KEYBOARD_FOCUS, llValue);

	UpdateView();

	// ����ټ��һ���Ƿ���Ҫ���������Ϣ
// 	if(npState->State == 0)	//Edit by Jaryee EndModifying����������ڰ��»س���ʱ���õģ����ﲻ�����ã������߼�������
// 		EndModifying();
}

//��������뿪
void CEvEditImp::OnMouseFocus(PSTEMS_STATE_CHANGE npState)
{
	PostMessageToParent(EEVT_EDIT_MOUSE_FOCUS, npState->State);

	CXuiElement::OnMouseFocus(npState);
}

//���˫��
ERESULT CEvEditImp::OnMouseDbClick(const STEMS_MOUSE_BUTTON* npInfo)
{
	return PostMessageToParent(EEVT_EDIT_MOUSE_DCLICK, mlCursorAt);
}

//Ԫ����ק
ERESULT CEvEditImp::OnDragging(const STMS_DRAGGING_ELE* npInfo)
{
	DWRITE_HIT_TEST_METRICS ldHitAt;
	BOOL lbTrail = FALSE;
	BOOL lbInside = FALSE;

	PostMessageToParent(EEVT_EDIT_DRAGING, CExMessage::DataInvalid);

	if (mpTextLayout == NULL || mpTextLayout->HitTestPoint(mdDragFrom.x + npInfo->Offset.x - mdLayoutRect.left, mdDragFrom.y, &lbTrail, &lbInside, &ldHitAt) != S_OK || lbInside == FALSE && lbTrail == FALSE ||
		ldHitAt.textPosition >= (UINT32)moText.Size())
		return ERESULT_SUCCESS;

	if (fabs(npInfo->Offset.x) < ldHitAt.width / 2.0f)
		return ERESULT_SUCCESS;

	if ((LONG)ldHitAt.textPosition + mlViewBegin < mlDragedText)
	{
		mlSelBegin = (LONG)ldHitAt.textPosition + mlViewBegin;
		mlSelEnd = mlDragedText;

		mlCursorAt = mlSelBegin;
	}
	else
	{
		if (mdDragFrom.x + npInfo->Offset.x > mdLayoutRect.right)
			return ERESULT_SUCCESS;
		mlSelBegin = mlDragedText;
		mlSelEnd = (LONG)ldHitAt.textPosition + mlViewBegin + 1;
		mlCursorAt = mlSelEnd;
	}

	UpdateView();

	return ERESULT_SUCCESS;
}

//��ק��ʼ,nulActKey�ĸ���갴ť���½�����ק
ERESULT CEvEditImp::OnDragBegin(const STMS_DRAGGING_ELE* npInfo)
{
	DWRITE_HIT_TEST_METRICS ldHitAt;
	BOOL lbTrail = FALSE;
	BOOL lbInside = FALSE;

	if (mpTextLayout == NULL || mpTextLayout->HitTestPoint(npInfo->CurrentPos.x - mdLayoutRect.left, npInfo->CurrentPos.y - mdLayoutRect.top, &lbTrail, &lbInside, &ldHitAt) != S_OK || lbInside == FALSE && lbTrail == FALSE ||
		ldHitAt.textPosition >= (UINT32)moText.Size())
	{
		if (mpTextLayout == NULL || fabs(npInfo->CurrentPos.x - mdLayoutRect.left) < fabs(npInfo->CurrentPos.x - mdLayoutRect.right))
		{
			// empty or near to left side
			mdDragFrom.x = npInfo->CurrentPos.x;
			mdDragFrom.y = (mdLayoutRect.top + mdLayoutRect.bottom) / 2.0f;
			mlDragedText = mlViewBegin;
			mlCursorAt = 0;
		}
		else
		{
			// near to right side
			mdDragFrom.x = npInfo->CurrentPos.x;
			mdDragFrom.y = (mdLayoutRect.top + mdLayoutRect.bottom) / 2.0f;
			mlDragedText = mlViewEnd - 1;
			mlCursorAt = mlViewEnd - 1;
		}
	}
	else
	{
		mdDragFrom = npInfo->CurrentPos;
		if (ldHitAt.left + ldHitAt.width / 2.0f <= npInfo->CurrentPos.x - mdLayoutRect.left && ldHitAt.textPosition > 0)
			ldHitAt.textPosition++;

		mlDragedText = (LONG)ldHitAt.textPosition + mlViewBegin;

		mlCursorAt = (LONG)ldHitAt.textPosition + mlViewBegin;
	}

	UpdateView();

	return ERESULT_SUCCESS;
}

//��ק����,nulActKey�ĸ���갴ť���½�����ק
ERESULT CEvEditImp::OnDragEnd(const STMS_DRAGGING_ELE* npInfo)
{
	return ERESULT_SUCCESS;
}

//��갴��
ERESULT CEvEditImp::OnMousePressed(const STEMS_MOUSE_BUTTON* npInfo)
{
	DWRITE_HIT_TEST_METRICS ldHitAt;
	BOOL lbTrail = FALSE;
	BOOL lbInside = FALSE;

	if (npInfo->Presssed == false)
		return ERESULT_SUCCESS;

	if (mlSelBegin != -1)
	{
		mlSelBegin = mlSelEnd = -1;
	}

	if (npInfo->ActKey != MK_LBUTTON || mpTextLayout == NULL || mpTextLayout->HitTestPoint(npInfo->Position.x - mdLayoutRect.left, npInfo->Position.y - mdLayoutRect.top, &lbTrail, &lbInside, &ldHitAt) != S_OK || lbInside == FALSE && lbTrail == FALSE ||
		ldHitAt.textPosition >= (UINT32)moText.Size())
		return ERESULT_SUCCESS;

	if (npInfo->Position.x >= mdLayoutRect.right)
		mlCursorAt = mlViewEnd;
	else
		if (npInfo->Position.x <= mdLayoutRect.left)
			mlCursorAt = mlViewBegin;
		else
		{
			if (ldHitAt.left + ldHitAt.width / 2.0f <= npInfo->Position.x - mdLayoutRect.left)
				ldHitAt.textPosition++;

			mlCursorAt = (LONG)ldHitAt.textPosition + mlViewBegin;
		}


	UpdateView();

	return ERESULT_SUCCESS;
}


//������Ϣ
ERESULT CEvEditImp::OnPaint(IEinkuiPaintBoard* npPaintBoard)
{

	ERESULT luResult;

	// ���Ʊ���
	if (mpBgBitmap != NULL)
		npPaintBoard->DrawBitmap(D2D1::RectF(0, 0, mpIterator->GetSizeX(), mpIterator->GetSizeY()),
			mpBgBitmap,
			ESPB_DRAWBMP_EXTEND);

	luResult = PrepareBrush(npPaintBoard);
	if (luResult != ERESULT_SUCCESS)
		return luResult;

	luResult = GenerateTextLayout(npPaintBoard);
	if (luResult != ERESULT_SUCCESS)
		return luResult;

	// ���Ʊ���
	for (int i = 0; i < moTextSections.Size(); i++)
	{
		if (moTextSections[i].mlBgBrushIndex >= 0)
			npPaintBoard->GetD2dRenderTarget()->FillRectangle(moTextSections[i].mdRegion, mpBrush[moTextSections[i].mlBgBrushIndex]);
	}

	// ��������
	if (mpTextLayout != NULL && mpBrush[0] != NULL)
		npPaintBoard->GetD2dRenderTarget()->DrawTextLayout(D2D1::Point2F(mdLayoutRect.left, mdLayoutRect.top), mpTextLayout, mpBrush[0]);

	// ��ʾ�����꣬???��ʱ���ṩinsert mode
	if (mlBlinking >= 0 && mlBlinking != 1)
	{
		npPaintBoard->GetD2dRenderTarget()->DrawLine(D2D1::Point2F(CExFloat::HalfPixel(mdCursorPos.x), CExFloat::Round(mdCursorPos.y + 2.0f))
			, D2D1::Point2F(CExFloat::HalfPixel(mdCursorPos.x), CExFloat::Round(mdLayoutRect.bottom)), mpBrush[eForeBrush], 1.0f);
	}

	return ERESULT_SUCCESS;
}

// ɾ����������Դ
void CEvEditImp::ReleaseDeviceResource()
{
	for (int i = 0; i < 4; i++)
	{
		CMM_SAFE_RELEASE(mpBrush[i]);
	}
	ClearTextLayout();
	mbDirty = true;
}

//׼������
ERESULT CEvEditImp::PrepareBrush(IEinkuiPaintBoard* npPaintBoard)
{
	ICfKey* lpKey = NULL;;
	HRESULT hr;
	ULONG luForeColor;
	ULONG luColor;
	FLOAT lfAlpha;
	ERESULT luResult = ERESULT_UNSUCCESSFUL;

	if (mpBrush[0] != NULL)	// �л�ˢ˵���Ѿ���ʼ��
		return ERESULT_SUCCESS;

	ReleaseDeviceResource();

	do
	{
		// ���Ƚ�������
		if (mpTextFormat == NULL)
		{
			wchar_t lswFontName[100];
			LONG llFontSize;

			lpKey = mpTemplete->GetSubKey(L"Font");
			BREAK_ON_NULL(lpKey);

			if (lpKey->GetValue(lswFontName, sizeof(wchar_t) * 100) <= 0)
				break;

			llFontSize = lpKey->QuerySubKeyValueAsLONG(L"size", 15);

			hr = npPaintBoard->GetDWriteFactory()->CreateTextFormat(
				lswFontName,
				NULL,
				DWRITE_FONT_WEIGHT_NORMAL,
				DWRITE_FONT_STYLE_NORMAL,
				DWRITE_FONT_STRETCH_NORMAL,
				(FLOAT)llFontSize,
				L"",
				&mpTextFormat
			);
			if (FAILED(hr))
				break;

			CMM_SAFE_RELEASE(lpKey);
		}


		// ���潨����ˢ
		lpKey = mpTemplete->GetSubKey(L"color");
		BREAK_ON_NULL(lpKey);

		// ����Fore color brush
		luForeColor = (ULONG)lpKey->QuerySubKeyValueAsLONG(L"fore", 0xFFFFFF);
		if (luForeColor == 0xFFFFFF)
		{
			luForeColor = D2D1::ColorF::Black;
			lfAlpha = 1.0f;
		}
		else
		{
			lfAlpha = ((FLOAT)(luForeColor >> 24) / 255.0f);
			if (lfAlpha > 0.99)
				lfAlpha = 1.0f;
			luForeColor &= 0xFFFFFF;
		}

		hr = npPaintBoard->GetD2dRenderTarget()->CreateSolidColorBrush(
			D2D1::ColorF(luForeColor, lfAlpha),
			&mpBrush[eForeBrush]
		);
		if (FAILED(hr))
			break;

		// ����back color brush
		luColor = (ULONG)lpKey->QuerySubKeyValueAsLONG(L"back", 0xFFFFFF);
		if (luColor != 0xFFFFFF)
		{
			lfAlpha = ((FLOAT)(luColor >> 24) / 255.0f);
			if (lfAlpha > 0.99)
				lfAlpha = 1.0f;
			luColor &= 0xFFFFFF;
			hr = npPaintBoard->GetD2dRenderTarget()->CreateSolidColorBrush(
				D2D1::ColorF(luColor, lfAlpha),
				&mpBrush[eBackBrush]
			);
			if (FAILED(hr))
				break;
		}

		// ����Selection Fore color brush
		luColor = (ULONG)lpKey->QuerySubKeyValueAsLONG(L"Selectedfore", 0xFFFFFF);
		if (luColor != 0xFFFFFF)
		{
			lfAlpha = ((FLOAT)(luColor >> 24) / 255.0f);
			if (lfAlpha > 0.99)
				lfAlpha = 1.0f;
			luColor &= 0xFFFFFF;

			hr = npPaintBoard->GetD2dRenderTarget()->CreateSolidColorBrush(
				D2D1::ColorF(luColor, lfAlpha),
				&mpBrush[eSelForeBrush]
			);
			if (FAILED(hr))
				break;
		}
		else
		{
			mpBrush[eSelForeBrush] = mpBrush[eForeBrush];
			mpBrush[eSelForeBrush]->AddRef();
		}

		// ����Fore color brush
		luColor = (ULONG)lpKey->QuerySubKeyValueAsLONG(L"SelectedBack", 0xFFFFFF);
		if (luColor == 0xFFFFFF)
		{
			luColor = ((~luForeColor) & 0xFFFFFF);	// �򵥵ذ�λȡ��
			lfAlpha = 1.0f;
		}
		else
		{
			lfAlpha = ((FLOAT)(luColor >> 24) / 255.0f);
			if (lfAlpha > 0.99)
				lfAlpha = 1.0f;
			luColor &= 0xFFFFFF;
		}

		hr = npPaintBoard->GetD2dRenderTarget()->CreateSolidColorBrush(
			D2D1::ColorF(luColor, lfAlpha),
			&mpBrush[eSelBackBrush]
		);
		if (FAILED(hr))
			break;


		luResult = ERESULT_SUCCESS;
	} while (false);

	CMM_SAFE_RELEASE(lpKey);

	return luResult;
}


// �ͷŸ�ʽ������
void CEvEditImp::ClearTextLayout() {
	moTextSections.Clear();
	CMM_SAFE_RELEASE(mpTextLayout);
}

//������Ϣ
ERESULT CEvEditImp::OnKeyPressed(const STEMS_KEY_PRESSED* npInfo)
{
	ERESULT luResult = ERESULT_SUCCESS;

	luResult = SendMessageToParent(EEVT_EDIT_KEY_STIKE, *npInfo, NULL, 0);
	if (luResult == ERESULT_DISCARD)
		return ERESULT_DISCARD;

	switch (npInfo->VirtualKeyCode)
	{
	case VK_LEFT:
		OnKeyLeft(npInfo);
		break;
	case VK_RIGHT:
		OnKeyRight(npInfo);
		break;
	case VK_BACK:
		OnKeyBack(npInfo);
		break;
		//case VK_DELETE:
		//	OnKeyDelete(npInfo);
		//	break;
	case VK_INSERT:
		OnKeyInsert(npInfo);
		break;
	case VK_HOME:
		OnKeyHome(npInfo);
		break;
	case VK_END:
		OnKeyEnd(npInfo);
		break;
	case VK_RETURN:
		EndModifying();
		break;
	default:
		luResult = ERESULT_KEY_UNEXPECTED;
	}

	return luResult;
}

void CEvEditImp::EndModifying(void)
{

	if (mbCompletion != false)
		return;

	moText.Insert(-1, UNICODE_NULL);

	CExMessage::PostMessageWithText(mpIterator->GetParent(), mpIterator, EEVT_EDIT_CONTENT_COMPLETION, moText.GetBuffer(), EMSG_POSTTYPE_FAST);

	moText.RemoveByIndex(moText.Size() - 1);

	mbCompletion = true;
}


//�ַ�������Ϣ
ERESULT CEvEditImp::OnChar(const PSTEMS_CHAR_INPUT npChar)
{
	ERESULT luResult;

	luResult = SendMessageToParent(EEVT_EDIT_CHAR_INPUT, *npChar, NULL, 0);
	if (luResult == ERESULT_DISCARD)
		return ERESULT_SUCCESS;

	// ���˷��ַ�
	if (npChar->CharIn < 0x20 || npChar->CharIn >= 0x7F && npChar->CharIn < 0x0100)
	{
		//  �����Ctrl+c Ctrl+v Ctrl+x Ctrl+z ���⴦��
		switch (npChar->CharIn)
		{
			//case 3:	// Ctrl+C
			//	OnCopyCommand();
			//	break;
			//case 22: // Ctrl+V
			//	OnPasteCommand();
			//	break;
			//case 24: // Ctrl+X
			//	OnCutCommand();
			//	break;
		case 26: // Ctrl+Z
			OnUndoCommand();
			break;
		default:;
		}
		return ERESULT_SUCCESS;
	}
	// �Ƿ���������
	if (CXuiElement::TestFlag(EEDT_FLAG_NUMBER) != false)
	{
		if ((npChar->CharIn < L'0' || npChar->CharIn > L'9'))
		{
			if (npChar->CharIn != L'-' || mlCursorAt != 0 && mlSelBegin != 0 || moText.Size() > 0 && moText[0] == L'-')
				return ERESULT_SUCCESS;
		}
	}

	if (mlSelBegin >= 0 && mlSelBegin < mlSelEnd)
	{
		// ȥ��ѡ��
		RemoveChars(mlSelBegin, mlSelEnd - mlSelBegin);

		// ��궨λ��ѡ������ʼ
		mlCursorAt = mlSelBegin;

		mlSelBegin = mlSelEnd = -1;
	}
	else	// �ǲ��ǲ���״̬
		if (mbInsertMode != false)
		{
			// ɾ����ǰ�ַ�
			RemoveChars(mlCursorAt, 1);
		}

	// �����ַ�
	if (InsertChar(mlCursorAt, npChar->CharIn) == 1)
	{
		mlCursorAt++;

		if (mlCursorAt >= mlViewEnd)
		{
			mlViewBegin = -1;
		}
	}

	mbDirty = true;

	PostMessageToParent(EEVT_EDIT_CONTENT_MODIFIED, CExMessage::DataInvalid);

	EinkuiGetSystem()->UpdateView();

	mbCompletion = false;

	return ERESULT_SUCCESS;
}

// Left Arrow striked
void CEvEditImp::OnKeyLeft(const STEMS_KEY_PRESSED* npInfo)
{
	if (npInfo->IsPressDown == false)
		return;

	if (mlCursorAt > 0)
	{
		mlCursorAt--;

		if (mlCursorAt <= mlViewBegin && mlViewBegin > 0)
			mlViewBegin--;

		if ((GetKeyState(VK_SHIFT) & 0x8000) != 0 && mlSelBegin != mlCursorAt)
		{
			if (mlSelBegin >= 0)
			{
				if (mlSelBegin > mlCursorAt)
					mlSelBegin = mlCursorAt;
				else
					mlSelEnd = mlCursorAt;
			}
			else
			{
				mlSelBegin = mlCursorAt;
				mlSelEnd = mlCursorAt + 1;
			}
		}
		else
		{
			mlSelBegin = -1;
			mlSelEnd = -1;
		}
	}
	else
		if ((GetKeyState(VK_SHIFT) & 0x8000) == 0 && mlSelBegin >= 0)
		{
			mlSelBegin = -1;
			mlSelEnd = -1;
		}
		else
			return;


	UpdateView();
}

// Right Arrow striked
void CEvEditImp::OnKeyRight(const STEMS_KEY_PRESSED* npInfo)
{
	if (npInfo->IsPressDown == false)
		return;

	if (mlCursorAt < moText.Size())
	{
		mlCursorAt++;
		if (mlCursorAt > mlViewEnd)
			mlViewBegin++;

		if ((GetKeyState(VK_SHIFT) & 0x8000) != 0 && mlSelEnd != mlCursorAt)
		{
			if (mlSelEnd >= 0)
			{
				if (mlSelEnd < mlCursorAt)
					mlSelEnd = mlCursorAt;
				else
					mlSelBegin = mlCursorAt;
			}
			else
			{
				mlSelBegin = mlCursorAt - 1;
				mlSelEnd = mlCursorAt;
			}
		}
		else
		{
			mlSelBegin = -1;
			mlSelEnd = -1;
		}
	}
	else
		if ((GetKeyState(VK_SHIFT) & 0x8000) == 0 && mlSelBegin >= 0)
		{
			mlSelBegin = -1;
			mlSelEnd = -1;
		}
		else
			return;


	UpdateView();
}

// Back Space striked
void CEvEditImp::OnKeyBack(const STEMS_KEY_PRESSED* npInfo)
{
	if (npInfo->IsPressDown == false)
		return;


	if (mlSelBegin >= 0)
	{
		mlCursorAt = mlSelBegin;
		RemoveChars(mlSelBegin, mlSelEnd - mlSelBegin);
		mlSelBegin = mlSelEnd = -1;
	}
	else
		if (mlCursorAt > 0)
			RemoveChars(--mlCursorAt, 1);
		else
			return;

	if (mlCursorAt <= mlViewBegin && mlViewBegin > 0)
		mlViewBegin--;

	PostMessageToParent(EEVT_EDIT_CONTENT_MODIFIED, CExMessage::DataInvalid);
	UpdateView();

	if (mbCompletion != false)
		mbCompletion = false;
}

void CEvEditImp::UpdateView(void)
{
	if (mlBlinking >= 0)
		InterlockedExchange(&mlBlinking, 2);

	mbDirty = true;
	EinkuiGetSystem()->UpdateView();
}


// Delete striked
void CEvEditImp::OnDeleteCommand(void)
{
	if (mlCursorAt < 0)
		return;

	if (mlSelBegin >= 0)
	{
		mlCursorAt = mlSelBegin;
		RemoveChars(mlSelBegin, mlSelEnd - mlSelBegin);
		mlSelBegin = mlSelEnd = -1;
	}
	else
		RemoveChars(mlCursorAt, 1);

	PostMessageToParent(EEVT_EDIT_CONTENT_MODIFIED, CExMessage::DataInvalid);
	UpdateView();

	if (mbCompletion != false)
		mbCompletion = false;
}

// Home striked
void CEvEditImp::OnKeyHome(const STEMS_KEY_PRESSED* npInfo)
{
	if (npInfo->IsPressDown == false)
		return;

	if ((GetKeyState(VK_SHIFT) & 0x8000) != 0)
	{
		if (mlSelEnd == mlCursorAt)
		{
			mlSelEnd = mlSelBegin;
			mlSelBegin = 0;
		}
		else
			if (mlSelBegin < 0)
			{
				mlSelBegin = 0;
				mlSelEnd = mlCursorAt;
			}
			else
				if (mlCursorAt > 0)
				{
					mlSelBegin = 0;
				}
	}
	else
	{
		mlSelBegin = -1;
		mlSelEnd = -1;
	}

	mlCursorAt = 0;
	mlViewBegin = 0;

	UpdateView();
}

// End striked
void CEvEditImp::OnKeyEnd(const STEMS_KEY_PRESSED* npInfo)
{
	if (npInfo->IsPressDown == false)
		return;

	if ((GetKeyState(VK_SHIFT) & 0x8000) != 0)
	{
		if (mlSelBegin == mlCursorAt)
		{
			mlSelBegin = mlSelEnd;
			mlSelEnd = moText.Size();
		}
		else
			if (mlSelBegin < 0)
			{
				mlSelBegin = mlCursorAt;
				mlSelEnd = moText.Size();
			}
			else
				if (mlSelBegin < mlCursorAt)
				{
					mlSelEnd = moText.Size();
				}
	}
	else
	{
		mlSelBegin = -1;
		mlSelEnd = -1;
	}

	mlCursorAt = moText.Size();
	mlViewBegin = -1;

	UpdateView();
}

// Insert striked
void CEvEditImp::OnKeyInsert(const STEMS_KEY_PRESSED* npInfo)
{

}

//���ָ��λ���ַ�,nlFromΪ���ĸ�������ʼ;nlCountΪɾ������
ERESULT CEvEditImp::RemoveChars(LONG nlFrom, LONG nlCount)
{
	LONG llTail = min(moText.Size() - 1, nlFrom + nlCount - 1);

	for (; llTail >= nlFrom; llTail--)
	{
		moText.RemoveByIndex(llTail);
	}

	//�����ַ���ɾ����Ҫ���¼�����ʾ�ַ���
	mlViewBegin = -1;
	return ERESULT_SUCCESS;
}

//�����µ��ַ�,nlInsertTo��ʾ���ĸ�λ�ý��в���,-1��ʾ�嵽���;nswChars��ʾҪ������ַ���; nlLength == -1 ��ʾnszChars��ȫ����Ч�ַ�������ȷ��nszChars����\0��β
LONG CEvEditImp::InsertChars(LONG nlInsertTo, wchar_t* nswChars, LONG nlLength)
{
	LONG i;

	for (i = 0; i < nlLength; i++)
	{
		// �Ƿ�ﵽ�������
		if (mlLimit >= 0 && moText.Size() >= mlLimit)
			return i;

		if (moText.Insert(mlCursorAt + i, nswChars[i]) < 0)
			return i;
	}

	return i;
}

// ����һ���ַ�
LONG CEvEditImp::InsertChar(LONG nlInsertTo, wchar_t nwcChar)
{
	// �Ƿ�ﵽ�������
	if (mlLimit >= 0 && moText.Size() >= mlLimit)
	{
		if (mlLimit == 0 || CXuiElement::TestFlag(EEDT_FLAG_NUMBER) == false || moText.Size() > mlLimit || moText[0] != L'-')
			return 0;
	}

	return (moText.Insert(nlInsertTo, nwcChar) >= 0) ? 1 : 0;
}

// �������ɸ�ʽ������
ERESULT CEvEditImp::GenerateTextLayout(IEinkuiPaintBoard* npPaintBoard)
{
	HRESULT hr;
	if (mbDirty == false)
		return ERESULT_SUCCESS;

	mbDirty = false;

	ClearTextLayout();

	CalculateTextRect(npPaintBoard);

	if (moText.Size() == 0)
	{
		// �գ�ȡ��Ч�ַ�����������Ϊ���λ��
		mdCursorPos.x = mdLayoutRect.left;
		mdCursorPos.y = mdLayoutRect.top;

		// ����
		return ERESULT_SUCCESS;
	}

	// ��ȡ�ַ����
	DWRITE_CLUSTER_METRICS* lpGlyphMetrics = NULL;
	UINT32 luCount;
	LONG llCrt;
	LONG llShowSelAlways;
	DWRITE_TEXT_RANGE ldRange;
	CEvEditTextSection loSection;
	ERESULT luResult = ERESULT_UNSUCCESSFUL;

	do
	{
		hr = mpTextLayout->GetClusterMetrics(NULL, 0, &luCount);
		if (hr != E_NOT_SUFFICIENT_BUFFER || luCount == 0)
			break;

		if (luCount != (UINT32)(mlViewEnd - mlViewBegin))	// Ϊ�˼򵥣����ǲ�֧��û�а��ַ��ָ��ķ�ʽ
			break;

		lpGlyphMetrics = new DWRITE_CLUSTER_METRICS[luCount];
		if (lpGlyphMetrics == NULL)
			break;

		hr = mpTextLayout->GetClusterMetrics(lpGlyphMetrics, luCount, &luCount);
		if (FAILED(hr))
			break;


		loSection.mlStart = mlViewBegin;
		llCrt = mlViewBegin;
		loSection.mdRegion.left = mdLayoutRect.left;
		loSection.mdRegion.right = mdLayoutRect.left;
		loSection.mdRegion.top = mdLayoutRect.top;
		loSection.mdRegion.bottom = mdLayoutRect.bottom;

		for (; llCrt <= mlViewEnd; llCrt++)
		{
			if (llCrt == mlSelEnd || llCrt == mlSelBegin || llCrt == mlCursorAt || llCrt == mlViewEnd)
			{
				// ������
				if (llCrt > loSection.mlStart)
				{
					bool lbSelected = (llCrt == mlSelEnd || loSection.mlStart == mlSelBegin);
					if (lbSelected != false && mlBlinking < 0)
					{
						llShowSelAlways = mpTemplete->QuerySubKeyValueAsLONG(L"ShowSelAlways", -1);
						if (llShowSelAlways <= 0)
							lbSelected = false;
					}

					loSection.mdRegion.right = loSection.mdRegion.left;
					for (LONG i = loSection.mlStart; i < llCrt; i++)
						loSection.mdRegion.right += lpGlyphMetrics[i - mlViewBegin].width;

					loSection.mlBgBrushIndex = (lbSelected) ? eSelBackBrush : eBackBrush;
					loSection.mlLength = llCrt - loSection.mlStart;
					moTextSections.Insert(-1, loSection);

					// ����������ɫ
					ldRange.startPosition = (UINT32)(loSection.mlStart - mlViewBegin);
					ldRange.length = (UINT32)loSection.mlLength;
					mpTextLayout->SetDrawingEffect(mpBrush[(lbSelected) ? eSelForeBrush : eForeBrush], ldRange);

				}

				if (mlCursorAt == llCrt)
				{
					mdCursorPos.x = loSection.mdRegion.right;
					mdCursorPos.y = loSection.mdRegion.top;
				}

				// Ϊ��һ��section׼��
				loSection.mlStart = llCrt;
				loSection.mdRegion.left = loSection.mdRegion.right;
			}
		}

		if (llCrt <= mlViewEnd)
			break;

		luResult = ERESULT_SUCCESS;
	} while (false);

	CMM_SAFE_DELETE_ARRAY(lpGlyphMetrics);//CMM_SAFE_DELETE(lpGlyphMetrics);	//hy20190723

	return luResult;
}


// �������ֵ��Ų�����
void CEvEditImp::CalculateTextRect(IEinkuiPaintBoard* npPaintBoard)
{
	IDWriteTextLayout* lpTextLayout = NULL;
	LONG llCount;
	UINT32 luLines;
	HRESULT hr;
	DWRITE_TEXT_METRICS ldMetrics;
	ZeroMemory(&ldMetrics, sizeof(DWRITE_TEXT_METRICS));

	FLOAT lfTextMaxWidth;
	FLOAT lfTextMaxHeight;
	ULONG llValue;
	LONG llVertical;
	LONG llHorizontal;	// left align

	// ���Ȼ��������ʾ����
	do
	{
		if (mdValidRect.left == 0 || mdValidRect.right == 0)
		{
			llValue = mpTemplete->QuerySubKeyValueAsLONG(L"edge/Left", 0);
			mdValidRect.left = (FLOAT)llValue;

			llValue = mpTemplete->QuerySubKeyValueAsLONG(L"edge/Top", 0);
			mdValidRect.top = (FLOAT)llValue;

			llValue = mpTemplete->QuerySubKeyValueAsLONG(L"edge/Right", 0);
			mdValidRect.right = mpIterator->GetSizeX() - (FLOAT)llValue;

			llValue = mpTemplete->QuerySubKeyValueAsLONG(L"edge/Bottom", 0);
			mdValidRect.bottom = mpIterator->GetSizeY() - (FLOAT)llValue;

		}

		lfTextMaxWidth = mdValidRect.right - mdValidRect.left;
		lfTextMaxHeight = mdValidRect.bottom - mdValidRect.top;


		llVertical = mpTemplete->QuerySubKeyValueAsLONG(L"Align/Vertical", 1); 	// center 
		llHorizontal = mpTemplete->QuerySubKeyValueAsLONG(L"Align/Horizontal", 0);// left align


	} while (false);


	bool lbKeepViewBegin = true;
	cmmVector<wchar_t> loAlter;
	wchar_t* lpText;


	if (mlViewBegin == -1)
	{
		mlViewBegin = 0;
		lbKeepViewBegin = false;
	}

	llCount = moText.Size() - mlViewBegin;
	if (llCount <= 0)
	{
		if (moText.Size() > 1)
		{
			mlViewBegin = moText.Size() - 1;
			llCount = 1;
		}
	}

	// ����������ַ������Ƿ���Է��Ͽ��Ҫ��
	while (llCount > 0)
	{
		loAlter.Clear();
		if (CXuiElement::TestFlag(EEDT_FLAG_PASSWORD) != false)
		{
			for (int i = 0; i < llCount; i++)
				loAlter.Insert(-1, L'*');

			lpText = loAlter.GetBuffer();
		}
		else
			lpText = moText.GetBuffer() + mlViewBegin;

		CMM_SAFE_RELEASE(lpTextLayout);

		hr = npPaintBoard->GetDWriteFactory()->CreateTextLayout(lpText, llCount, mpTextFormat, lfTextMaxWidth, lfTextMaxHeight, &lpTextLayout);
		if (SUCCEEDED(hr))
		{
			hr = lpTextLayout->GetLineMetrics(NULL, 0, &luLines);
			if (luLines == 1)
			{
				hr = lpTextLayout->GetMetrics(&ldMetrics);
				if (SUCCEEDED(hr))
				{
					if (ldMetrics.width < lfTextMaxWidth)	// �Ƿ���Ҫ�ô�widthIncludingTrailingWhitespace�ж�???
					{
						break;
					}
				}
			}
		}
		if (lbKeepViewBegin == false)
			mlViewBegin++;

		llCount--;
	}

	if (lpTextLayout == NULL)
	{
		// ȡһ���ַ������'Q'
		hr = npPaintBoard->GetDWriteFactory()->CreateTextLayout(L"Q", 1, mpTextFormat, lfTextMaxWidth, lfTextMaxHeight, &lpTextLayout);
		if (SUCCEEDED(hr))
		{
			hr = lpTextLayout->GetMetrics(&ldMetrics);
			lpTextLayout->Release();
		}

		if (FAILED(hr))
		{
			ldMetrics.width = lfTextMaxWidth;
			ldMetrics.height = lfTextMaxHeight;
		}
	}
	else
		mpTextLayout = lpTextLayout;

	mlViewEnd = mlViewBegin + llCount;

	// ��������Ҫ����������
	switch (llVertical)
	{
	case 1:	// center
		mdLayoutRect.top = CExFloat::Round(mdValidRect.top + (lfTextMaxHeight - ldMetrics.height) / 2.0f);
		mdLayoutRect.bottom = mdLayoutRect.top + ldMetrics.height;//mdValidRect.bottom - (lfTextMaxHeight - ldMetrics.height)/2.0f;
		break;
	case 2:	// bottom
		mdLayoutRect.bottom = mdValidRect.bottom;
		mdLayoutRect.top = mdValidRect.bottom - ldMetrics.height;//mdValidRect.top + (lfTextMaxHeight - ldMetrics.height);
		break;
	case 0:	// top
	default:
		mdLayoutRect.top = mdValidRect.top;
		mdLayoutRect.bottom = mdValidRect.top + ldMetrics.height;//mdValidRect.bottom - (lfTextMaxHeight - ldMetrics.height);
	}
	switch (llHorizontal)
	{
	case 1:	// center
		mdLayoutRect.left = CExFloat::Round(mdValidRect.left + (lfTextMaxWidth - ldMetrics.widthIncludingTrailingWhitespace) / 2.0f);
		mdLayoutRect.right = mdLayoutRect.left + ldMetrics.width;//mdValidRect.right - (lfTextMaxWidth - ldMetrics.widthIncludingTrailingWhitespace)/2.0f;
		break;
	case 2:	// right
		mdLayoutRect.right = mdValidRect.right;
		mdLayoutRect.left = mdValidRect.right - ldMetrics.width;//mdValidRect.left + (lfTextMaxWidth - ldMetrics.widthIncludingTrailingWhitespace);
		break;
	case 0:	// left
	default:
		mdLayoutRect.left = mdValidRect.left;
		mdLayoutRect.right = mdValidRect.left + ldMetrics.width;//mdValidRect.right - (lfTextMaxWidth - ldMetrics.widthIncludingTrailingWhitespace);
	}
}

//�Ӽ��а���ַ�����return the character count
int CEvEditImp::GetClipboardString(OUT wchar_t* npTextBuffer, LONG nlBufCharSize)
{
	int liLength = 0;
	HANDLE lhClipMem = NULL;

	if (OpenClipboard(EinkuiGetSystem()->GetMainWindow()) == FALSE)  //�򿪼��а�
		return 0;

	do
	{
		lhClipMem = GetClipboardData(CF_UNICODETEXT);
		if (lhClipMem == NULL)
			break;

		const wchar_t* lswInput = (const wchar_t*)GlobalLock(lhClipMem); //��ȡ�ַ���
		if (lswInput == NULL)
			break;

		liLength = wcslen(lswInput);
		if (liLength == 0 || npTextBuffer == NULL)
			break;

		if (liLength + 1 > nlBufCharSize)
		{
			liLength = -1;
			break;
		}

		// copy the string
		wcscpy_s(npTextBuffer, nlBufCharSize, lswInput);

		GlobalUnlock(lhClipMem);

	} while (false);

	CloseClipboard();  //�رռ��а�

	return liLength;
}

//save string to clipboard
void CEvEditImp::SetClipboardString(const wchar_t* nswString, LONG nlCharCount)
{
	wchar_t* lpGlobalBuffer = NULL;
	HANDLE lhClipMem = NULL;

	if (OpenClipboard(EinkuiGetSystem()->GetMainWindow()) == FALSE)  //�򿪼��а�
		return;

	do
	{
		if (nlCharCount <= 0)
			break;

		lhClipMem = GlobalAlloc(GMEM_MOVEABLE, (nlCharCount + 1) * sizeof(wchar_t));

		wchar_t* lswClipboardBuffer = (wchar_t*)GlobalLock(lhClipMem); //��ȡ�ַ���
		if (lswClipboardBuffer == NULL)
			break;

		RtlCopyMemory(lswClipboardBuffer, nswString, nlCharCount * sizeof(wchar_t));
		lswClipboardBuffer[nlCharCount] = UNICODE_NULL;

		GlobalUnlock(lhClipMem);

		SetClipboardData(CF_UNICODETEXT, lhClipMem);

	} while (false);

	CloseClipboard();  //�رռ��а�
}

// �������������
void CEvEditImp::ClearClipboard(void)
{
	if (OpenClipboard(EinkuiGetSystem()->GetMainWindow()) == FALSE)  //�򿪼��а�
		return;

	EmptyClipboard();

	CloseClipboard();  //�رռ��а�
}



// copy
void CEvEditImp::OnCopyCommand(void)
{
	int liChars;

	// û��ѡ������������ģʽ�£����ܸ�������
	if (mlSelBegin < 0 || mlSelBegin >= mlSelEnd || CXuiElement::TestFlag(EEDT_FLAG_PASSWORD) != false)
	{
		// ����������оɵ�����
		ClearClipboard();
		return;
	}

	liChars = mlSelEnd - mlSelBegin;

	SetClipboardString(moText.GetBuffer() + mlSelBegin, liChars);
}

// Paste
void CEvEditImp::OnPasteCommand(void)
{
	wchar_t* lswText;
	int liChars = GetClipboardString(NULL, 0);

	if (liChars <= 0)
		return;

	lswText = new wchar_t[liChars + 1];
	if (lswText == NULL)
		return;

	liChars = GetClipboardString(lswText, liChars + 1);
	if (liChars > 0)
	{
		if (mlSelBegin >= 0 && mlSelBegin < mlSelEnd)
		{
			// ȥ��ѡ��
			RemoveChars(mlSelBegin, mlSelEnd - mlSelBegin);

			// ��궨λ��ѡ������ʼ
			mlCursorAt = mlSelBegin;

			mlSelBegin = mlSelEnd = -1;
		}
		else	// �ǲ��ǲ���״̬
			if (mbInsertMode != false)
			{
				// ɾ��ͬ��
				int liToClear = min(liChars, moText.Size() - mlCursorAt);
				RemoveChars(mlCursorAt, liToClear);
			}

		liChars = InsertChars(mlCursorAt, lswText, liChars);

		mlCursorAt += liChars;

		PostMessageToParent(EEVT_EDIT_CONTENT_MODIFIED, CExMessage::DataInvalid);
		UpdateView();

	}

	delete lswText;
	mbCompletion = false;
}
// Cut
void CEvEditImp::OnCutCommand(void)
{
	int liChars;

	if (mlSelBegin < 0 || mlSelBegin >= mlSelEnd)
		return;

	liChars = mlSelEnd - mlSelBegin;

	// ������ģʽ�������ݵ�������
	if (CXuiElement::TestFlag(EEDT_FLAG_PASSWORD) == false)
		SetClipboardString(moText.GetBuffer() + mlSelBegin, liChars);
	else
		ClearClipboard();// ����������оɵ�����

	// ȥ��ѡ��
	RemoveChars(mlSelBegin, mlSelEnd - mlSelBegin);

	// ��궨λ��ѡ������ʼ
	mlCursorAt = mlSelBegin;

	mlSelBegin = mlSelEnd = -1;

	PostMessageToParent(EEVT_EDIT_CONTENT_MODIFIED, CExMessage::DataInvalid);
	UpdateView();

	mbCompletion = false;
}

// Undo
void CEvEditImp::OnUndoCommand(void)
{
}

//����
ERESULT CEvEditImp::OnCommand(const nes_command::ESCOMMAND neCmd)
{
	ERESULT luResult;
	luResult = ERESULT_SUCCESS;

	switch (neCmd)
	{
	case nes_command::eCopy:
		OnCopyCommand();
		break;
	case nes_command::eCut:
		OnCutCommand();
		break;
	case nes_command::ePaste:
		OnPasteCommand();
		break;
	case nes_command::eDelete:
		OnDeleteCommand();
		break;
	case nes_command::eSelAll:
		break;
	default:
		luResult = ERESULT_UNSUCCESSFUL;
	}
	return luResult;
}
