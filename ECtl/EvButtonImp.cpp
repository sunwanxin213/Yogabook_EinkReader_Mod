#include "stdafx.h"
#include "cmmBaseObj.h"
#include "Einkui.h"
#include "XCtl.h"
#include "ElementImp.h"
#include "EvButtonImp.h"


//using namespace D2D1;
DEFINE_BUILTIN_NAME(Button)

// ֻ���ڱ������ó�ʼֵ����ָ����ΪNULL�����п���ʧ�ܵ������֮������㶼Ӧ����InitOnCreate�н���
CEvButton::CEvButton()
{
	mpszButtonText = mpszNormolTip = mpszCheckedTip = NULL;
	mpTextBitmap = NULL;
	mbIsMouseFocus = false;
	mbIsKeyboardFocus = false;
	mbIsPressed = false;
	mbIsChecked = false;
	mbIsCheckEnable = false;
	mbIsOther = false;
	mlCurrentPage = mlPageCountMax = 0;
	mpswFontName = NULL;
	mdFrameSize.width = mdFrameSize.height = 0.0f;
	mbIsPlayTimer = false;
	mfTextTop = mfTextLeft = mfTextRight = mfTextBottom = 0.0f;
	ZeroMemory(mdArrayFrame, sizeof(mdArrayFrame));
}

// �����ͷų�Ա����
CEvButton::~CEvButton()
{
	CMM_SAFE_DELETE(mpszButtonText);
	CMM_SAFE_DELETE(mpszNormolTip);
	CMM_SAFE_DELETE(mpszCheckedTip);
	CMM_SAFE_RELEASE(mpTextBitmap);
	CMM_SAFE_DELETE(mpswFontName);
}

ULONG CEvButton::InitOnCreate(
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

		//װ��һЩ��Ҫ��������Դ
		LoadResource();


		leResult = ERESULT_SUCCESS;

	} while (false);

	return leResult;
}

//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
ERESULT CEvButton::OnElementCreate(IEinkuiIterator* npIterator)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		if (CXuiElement::OnElementCreate(npIterator) != ERESULT_SUCCESS)
			break;

		StartPlayTimer();

		lResult = ERESULT_SUCCESS;
	} while (false);

	return lResult;
}

// ����Ԫ�أ�����֪ͨһ��Ԫ�����٣�ע�⣺��Ԫ�������յ�����Ϣ��Ӧ�ü�ʱ����Ԫ�ع�������UnregisterElement������
// �Ӷ�����Ԫ�ع�������������һ��Ԫ�ط���������Ϣ�������ٽ��Լ���Ԫ�ع�����ע���������ͷ��������
ERESULT CEvButton::OnElementDestroy()
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		CXuiElement::OnElementDestroy();	//���û���

		lResult = ERESULT_SUCCESS;

	} while (false);

	return lResult;
}

//װ��������Դ
ERESULT CEvButton::LoadResource()
{
	ERESULT leResult = ERESULT_UNSUCCESSFUL;

	ICfKey* lpValue = NULL;
	LONG llAttibute[16] = { 0 };	//��ť����
	LONG llLen = -1;

	do
	{
		//������ɫ
		mdwColor = (DWORD)mpTemplete->QuerySubKeyValueAsLONG(TF_ID_BT_COLOR, 0xFFFFFFFF);

		mdwDisabledColor = (DWORD)mpTemplete->QuerySubKeyValueAsLONG(TF_ID_BT_DISABLED_COLOR, 0xFF7d7d7d);

		//����
		lpValue = mpTemplete->GetSubKey(TF_ID_BT_FONT);
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
		mdwFontSize = (DWORD)mpTemplete->QuerySubKeyValueAsLONG(TF_ID_BT_FONT_SIZE, 15);

		lpValue = mpTemplete->GetSubKey(TF_ID_BT_TEXT); //��ť����
		if (lpValue != NULL)
		{
			llLen = lpValue->GetValueLength();
			mpszButtonText = new wchar_t[llLen];
			BREAK_ON_NULL(mpszButtonText);

			lpValue->GetValue(mpszButtonText, lpValue->GetValueLength());
			CMM_SAFE_RELEASE(lpValue);

			if (mpszButtonText[0] != UNICODE_NULL)
				ReCreateTextBmp();
		}

		lpValue = mpTemplete->GetSubKey(TF_ID_BT_NORMAL_TIP); //Tip1
		if (lpValue != NULL)
		{
			llLen = lpValue->GetValueLength();
			mpszNormolTip = new wchar_t[llLen];
			BREAK_ON_NULL(mpszNormolTip);

			lpValue->GetValue(mpszNormolTip, lpValue->GetValueLength());
			CMM_SAFE_RELEASE(lpValue);
			mpIterator->SetToolTip(mpszNormolTip);
		}

		lpValue = mpTemplete->GetSubKey(TF_ID_BT_CHECKED_TIP); //Tip2
		if (lpValue != NULL)
		{
			llLen = lpValue->GetValueLength();
			mpszCheckedTip = new wchar_t[llLen];
			BREAK_ON_NULL(mpszCheckedTip);

			lpValue->GetValue(mpszCheckedTip, lpValue->GetValueLength());
			CMM_SAFE_RELEASE(lpValue);
		}

		//���ֶ��뷽ʽ
		mlAlignType = mpTemplete->QuerySubKeyValueAsLONG(TF_ID_BT_ALIGN_TYPE, 2);

		//������߾�
		mfTextLeft = (FLOAT)mpTemplete->QuerySubKeyValueAsLONG(TF_ID_BT_TEXT_LEFT, 1);

		//�����ұ߾�
		mfTextRight = (FLOAT)mpTemplete->QuerySubKeyValueAsLONG(TF_ID_BT_TEXT_RIGHT, 1);

		//�����ϱ߾�
		mfTextTop = (FLOAT)mpTemplete->QuerySubKeyValueAsLONG(TF_ID_BT_TEXT_TOP, 1);

		//�����±߾�
		mfTextBottom = (FLOAT)mpTemplete->QuerySubKeyValueAsLONG(TF_ID_BT_TEXT_BOTTOM, 1);

		//��Ӧ����
		mdAcionSize.width = (FLOAT)mpTemplete->QuerySubKeyValueAsLONG(TF_ID_BT_ACTION_WIDTH, 0);

		//��Ӧ����
		mdAcionSize.height = (FLOAT)mpTemplete->QuerySubKeyValueAsLONG(TF_ID_BT_ACTION_HEIGHT, 0);


		//ÿ�������ӵڼ�֡��ʼ
		lpValue = mpTemplete->GetSubKey(TF_ID_BT_FRAME); //��̬��ť֡����
		if (lpValue != NULL)
		{
			lpValue->GetValue(llAttibute, lpValue->GetValueLength());
			lpValue->Release();

			// [CheckMarx Fix by zhuhl5, [TRY]]
			for (int i = ARRAY_INDEX_DISABLE; i < ARRAY_INDEX_PRESSED + 1; i++)
			{
				mdArrayFrame[i].Index = mlPageCountMax;			//�ӵڼ�֡��ʼ
				mdArrayFrame[i].Count = llAttibute[i];		//����֡

				mlPageCountMax += llAttibute[i];
			}
		}

		lpValue = mpTemplete->GetSubKey(TF_ID_BT_CHECKED_FRAME); //Checked̬֡����
		if (lpValue != NULL)
		{
			lpValue->GetValue(llAttibute, lpValue->GetValueLength());
			lpValue->Release();

			mbIsCheckEnable = true; //����Checked̬

			// [CheckMarx Fix by zhuhl5, [TRY]]
			for (int i = ARRAY_INDEX_CHECKED_DISABLE; i < ARRAY_INDEX_CHECKED_PRESSED + 1; i++)
			{
				mdArrayFrame[i].Index = mlPageCountMax;			//�ӵڼ�֡��ʼ
				mdArrayFrame[i].Count = llAttibute[i - ARRAY_INDEX_CHECKED_DISABLE];		//����֡

				mlPageCountMax += mdArrayFrame[i].Count;
			}
		}


		lpValue = mpTemplete->GetSubKey(TF_ID_BT_OTHER_FRAME); //��������֡����
		if (lpValue != NULL)
		{
			ICfKey* lpSubKey = lpValue->GetSubKey();

			int i = 0;
			for (i = ARRAY_INDEX_OTHER; i < BUTTON_FRAME_ARRAY_MAX, lpSubKey != NULL; i++)
			{
				lpSubKey->GetValue(&(mdArrayFrame[i].Count), lpSubKey->GetValueLength());
				mdArrayFrame[i].Index = mlPageCountMax;			//�ӵڼ�֡��ʼ

				mlPageCountMax += mdArrayFrame[i].Count;

				lpSubKey = lpSubKey->MoveToNextKey();	//�鿴�Ƿ�����һ����������
			}

			//if(i > ARRAY_INDEX_OTHER) //????����ΪʲôҪ����Check״̬�������� Edit by Jaryee
			//	mbIsCheckEnable = true;	//����Checked̬

			CMM_SAFE_RELEASE(lpValue);
			CMM_SAFE_RELEASE(lpSubKey);
		}

		if (mpBgBitmap != NULL)
		{
			UINT luiWidth = mpBgBitmap->GetWidth();
			UINT luiHeight = mpBgBitmap->GetHeight();	//��ȡ����ͼ���


			if (mlPageCountMax > 0)
			{
				mdFrameSize.width = (float)luiWidth / mlPageCountMax;
				mdFrameSize.height = (float)luiHeight;

				mpIterator->SetSize(mdFrameSize.width, mdFrameSize.height); //���ð�ť��ʾ��С
			}
		}

		RelocateText();

		leResult = ERESULT_SUCCESS;

	} while (false);


	return leResult;
}

//��λ����ͼƬ��ʾλ��
void CEvButton::RelocateText(void)
{
	do
	{
		if (mpTextBitmap == NULL)
			break;

		if (mpBgBitmap != NULL)
		{
			//if (wcsicmp(mpszButtonText,L"����")==0)
			//{
			//	//break;
			//}
			FLOAT lfWidth = mfTextLeft + mfTextRight + mpTextBitmap->GetWidth();
			FLOAT lfHeight = mfTextTop + mfTextBottom + mpTextBitmap->GetHeight();
			if (lfWidth > mpIterator->GetSizeX()) //�������ͼ������С,��Ҫ���󱳾�ͼ
				mpIterator->SetSize(lfWidth, mpIterator->GetSizeY());
			if (lfHeight > mpIterator->GetSizeY())
				mpIterator->SetSize(mpIterator->GetSizeX(), lfHeight);


			if (mlAlignType == 1)
			{
				//�����
			}
			else if (mlAlignType == 2)
			{
				//���ж���
			}
			else/* if(mlAlignType == 2)*/
			{
				//�Ҷ���

			}


			if (mlAlignType == 1)
				mdTextDestRect.left = mfTextLeft;
			else if (mlAlignType == 2)
				mdTextDestRect.left = CExFloat::Round((mpIterator->GetSizeX() - mpTextBitmap->GetWidth()) / 2.0f);
			else
				mdTextDestRect.left = mpIterator->GetSizeX() - mfTextRight - mpTextBitmap->GetWidth();

			mdTextDestRect.right = mdTextDestRect.left + mpTextBitmap->GetWidth();
			if (mfTextTop - 1.0f < 0.5f)
				mdTextDestRect.top = CExFloat::Round((mpIterator->GetSizeY() - mpTextBitmap->GetHeight()) / 2.0f);
			else
				mdTextDestRect.top = mfTextTop;

			mdTextDestRect.bottom = mdTextDestRect.top + mpTextBitmap->GetHeight();
		}
		else
		{
			mdTextDestRect.left = mdTextDestRect.top = 0.0f;
			mdTextDestRect.right = (FLOAT)mpTextBitmap->GetWidth();
			mdTextDestRect.bottom = (FLOAT)mpTextBitmap->GetHeight();
		}

	} while (false);
}

// �ֽ���Ϣ���ṩ��Ϣ�ֽ����Ϣ��Ӧ�Ĺ��ܣ������ʵ���ǽ���Ϣ�ֽ�Ϊ��ͬ������󣬵�����Ӧ�Ĵ����麯�������ڲ���ʶ����Ϣ��һ�ɷ���ERESULT_UNEXPECTED_MESSAGE
// �������ķ���ֵ���Զ�ͬ�����õ�npMsgָ�����Ϣ������
ERESULT CEvButton::ParseMessage(IEinkuiMessage* npMsg)
{
	// ʵ��ԭ�����ȵ�������ķֽ⹦�ܣ����󽫲��������Ϣ��������

	ERESULT luResult = ERESULT_UNEXPECTED_MESSAGE;

	switch (npMsg->GetMessageID())
	{
	case EACT_BUTTON_SET_CHECKED:	//����Check״̬
	{
		if (npMsg->GetInputDataSize() != sizeof(bool))
		{
			luResult = ERESULT_WRONG_PARAMETERS;
			break;
		}
		// ��ȡ��������
		bool* lpValue = (bool*)npMsg->GetInputData();
		luResult = SetChecked(*lpValue);// ����ֵ��IEinkuiMessage����ķ���ֵ�ɱ������ĵ��������ݱ������Ľ������
		break;
	}
	case EACT_BUTTON_GET_CHECKED:	//��ȡ�Ƿ���Check״̬
	{
		if (npMsg->GetOutputBufferSize() != sizeof(bool))
		{
			luResult = ERESULT_WRONG_PARAMETERS;
			break;
		}

		// �����������
		bool* lpOut = (bool*)npMsg->GetOutputBuffer();

		*lpOut = IsChecked();

		npMsg->SetOutputDataSize(sizeof(bool));

		luResult = ERESULT_SUCCESS;
		break;
	}
	case EACT_BUTTON_PLAY_OTHER_ANIMATION:
	{
		//�����Զ��嶯����Ϣ
		if (npMsg->GetInputDataSize() != sizeof(LONG))
		{
			luResult = ERESULT_WRONG_PARAMETERS;
			break;
		}
		// ��ȡ��������
		LONG* lpValue = (LONG*)npMsg->GetInputData();
		mlOtherIndex = *lpValue;	//���ŵ����ĸ�����
		mlOtherIndex -= 1;	//������˳���Ǵ�1��ʼ�����ǵ������Ǵ�0��ʼ
		if (mlOtherIndex < 0 || mlOtherIndex >(BUTTON_FRAME_ARRAY_MAX - ARRAY_INDEX_OTHER + 1))
			break;

		if (mdArrayFrame[mlOtherIndex + ARRAY_INDEX_OTHER].Count != 0)
		{
			mbIsOther = true;	//���ò��Ŷ���
			StartPlayTimer();
		}

		break;
	}
	case EACT_BUTTON_SETTEXT:
	{
		//������ʾ����
		// ��ȡ��������
		wchar_t* lpswText = (wchar_t*)npMsg->GetInputData();
		OnChangeText(lpswText);

		break;
	}
	case EACT_BUTTON_CHANGE_PIC:
	{
		//��������ͼƬ�����·��
		// ��ȡ��������
		wchar_t* lpValue = (wchar_t*)npMsg->GetInputData();

		luResult = OnChangeBackGround(lpValue, false);

		break;
	}
	case EACT_BUTTON_CHANGE_PIC_FULLPATH:
	{
		//��������ͼƬ��ȫ·��
		// ��ȡ��������
		wchar_t* lpValue = (wchar_t*)npMsg->GetInputData();

		luResult = OnChangeBackGround(lpValue, true);

		break;
	}
	case EACT_BUTTON_GETTEXT:
	{
		//��ȡ��ʾ����
		if (npMsg->GetOutputBufferSize() != sizeof(wchar_t*))
		{
			luResult = ERESULT_WRONG_PARAMETERS;
			break;
		}

		// �����������
		wchar_t** lpOut = (wchar_t**)npMsg->GetOutputBuffer();
		*lpOut = mpszButtonText;

		break;
	}
	case EACT_BUTTON_SET_ACTION_RECT:
	{
		//���ü�������
		if (npMsg->GetInputDataSize() != sizeof(D2D1_SIZE_F))
		{
			luResult = ERESULT_WRONG_PARAMETERS;
			break;
		}
		D2D1_SIZE_F* lpSize = (D2D1_SIZE_F*)npMsg->GetInputData();
		BREAK_ON_NULL(lpSize);

		mdAcionSize = *lpSize;

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

//����
ERESULT CEvButton::OnPaint(IEinkuiPaintBoard* npPaintBoard)
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
				//????Ҫ���ⲽ����ŵ����յ��Լ��ο��ߴ�ı�ĵط�
				//if((mpBgBitmap->GetExtnedLineX() != 0 && mpBgBitmap->GetExtnedLineY() != 0) && ((mpIterator->GetSizeX() - mdFrameSize.width > 1) || mpIterator->GetSizeY() - mdFrameSize.height > 1))
				//	lulMethod = ESPB_DRAWBMP_EXTEND;	//�����������չ�߲��Ҳο��ߴ����ʵ�ʳߴ磬�Ǿ�ʹ����չ��ʽ

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
			FLOAT lfValue = mbIsPressed ? 1.0f : 0.0f;
			lfValue = 0.0f;//eink��Ŀ����Ҫ�������ƶ�
			npPaintBoard->DrawBitmap(D2D1::RectF(mdTextDestRect.left + lfValue, mdTextDestRect.top + lfValue, mdTextDestRect.right + lfValue, mdTextDestRect.bottom + lfValue),
				mpTextBitmap,
				ESPB_DRAWBMP_NEAREST
			);
		}

		lResult = ERESULT_SUCCESS;

	} while (false);

	return lResult;
}

//��ʱ��
void CEvButton::OnTimer(PSTEMS_TIMER npStatus)
{
	do
	{
		if (npStatus->TimerID == BUTTON_TIMER_ID_PAGE)
		{
			//���Ŷ���
			if (mpIterator->IsVisible() == false)	//�����������״̬���͹ص���ʱ��
			{
				mpIterator->KillTimer(npStatus->TimerID);
				mbIsPlayTimer = false;
				break;
			}

			LONG llIndex = GetCurrentStatesArrayIndex();		//��ȡ��ǰ״̬ͼƬ��Ϣ���ڵ������±�

			if (llIndex != -1)
			{

				if (mlCurrentPage < mdArrayFrame[llIndex].Count - 1)
				{
					mlCurrentPage++;
				}
				else
				{
					mlCurrentPage = 0;	//�������ҳ
					if (mbIsOther != false)
					{
						mbIsOther = false;

						StartPlayTimer(); //�Զ��嶯��ֻ����һ��
						LONG llOtherIndex = mlOtherIndex + 1;
						PostMessageToParent(EEVT_BUTTON_PLAYED, llOtherIndex); //֪ͨ�����ڣ������������
					}
				}

				EinkuiGetSystem()->UpdateView();
			}
		}

	} while (false);

}

//��������뿪
void CEvButton::OnMouseFocus(PSTEMS_STATE_CHANGE npState)
{
	CXuiElement::OnMouseFocus(npState);

	if (npState->State != 0)
	{
		//������
		if (mbIsMouseFocus == false)
			mbIsMouseFocus = true;

		PostMessageToParent(EEVT_BUTTON_MOUSE_IN, mpIterator);
	}
	else
	{
		//����Ƴ�
		if (mbIsMouseFocus != false)
			mbIsMouseFocus = false;

		if (mbIsPressed != false)  //�����������ˣ���ȥ������״̬
			mbIsPressed = false;

		PostMessageToParent(EEVT_BUTTON_MOUSE_OUT, mpIterator);
	}

	if (mbIsOther == false)
		StartPlayTimer(); //������ڲ����Զ��嶯������ִ���������


	EinkuiGetSystem()->UpdateView();
}

//��갴��
ERESULT CEvButton::OnMousePressed(const STEMS_MOUSE_BUTTON* npInfo)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		BREAK_ON_NULL(npInfo);
		if (mpIterator->IsEnable() == false)
			break;	//����ǽ���״̬���Ͳ���������

		if (MOUSE_LB(npInfo->ActKey) == false)  //��������������Ͳ�����
			break;

		if (npInfo->Presssed == false)
		{
			//���̧��
			if (mbIsPressed != false)
			{
				//���������һ��Click
				PostMessageToParent(EEVT_BUTTON_CLICK, CExMessage::DataInvalid);

				mbIsPressed = false;

				if (mbIsCheckEnable != false)
				{
					//����Check
					if (mbIsChecked == false)
					{
						//Check״̬
						mpIterator->SetToolTip(mpszCheckedTip);
						mbIsChecked = true;
						PostMessageToParent(EEVT_BUTTON_CHECKED, CExMessage::DataInvalid);
					}
					else
					{
						//UnCheck״̬
						mpIterator->SetToolTip(mpszNormolTip);
						mbIsChecked = false;
						PostMessageToParent(EEVT_BUTTON_UNCHECK, CExMessage::DataInvalid);
					}
				}
			}
		}
		else
		{
			//��갴��
			if (mbIsPressed == false)
			{
				mbIsPressed = true;
			}
		}

		StartPlayTimer();
		EinkuiGetSystem()->UpdateView();

		lResult = ERESULT_SUCCESS;

	} while (false);

	return lResult;
}

//���û�����
ERESULT CEvButton::OnElementEnable(bool nbIsEnable)
{
	StartPlayTimer();

	// �ı�������ɫ
	ReCreateTextBmp();

	return ERROR_SUCCESS;
}

//����Check״̬
ERESULT CEvButton::SetChecked(bool nbIsChecked)
{
	mbIsChecked = nbIsChecked;
	StartPlayTimer();
	return ERESULT_SUCCESS;
}

//�ж��Ƿ���Check״̬
bool CEvButton::IsChecked()
{
	return mbIsChecked;
}

//��ȡ��ǰ״̬���ڵ������±�
LONG CEvButton::GetCurrentStatesArrayIndex()
{
	LONG llIndex = -1;

	//��ť��������Timer
	if (mbIsOther != false)
	{
		//�Զ��嶯��
		llIndex = mlOtherIndex + ARRAY_INDEX_OTHER;
	}
	else if (mpIterator->IsEnable() == false)		// ����̬
	{
		if (mbIsChecked == false)
		{
			//��ͨ����̬
			llIndex = ARRAY_INDEX_DISABLE;
		}
		else
		{
			//Checked����̬
			llIndex = ARRAY_INDEX_CHECKED_DISABLE;
		}
	}
	else if (mbIsPressed != false)
	{
		if (mbIsChecked == false)
		{
			//��ͨ����̬
			llIndex = ARRAY_INDEX_PRESSED;
		}
		else
		{
			//Checked����̬
			llIndex = ARRAY_INDEX_CHECKED_PRESSED;
		}
	}
	else if (mbIsMouseFocus != false)
	{
		if (mbIsChecked == false)
		{
			//��ͨ����̬
			llIndex = ARRAY_INDEX_FOCUS;
		}
		else
		{
			//Checked����̬
			llIndex = ARRAY_INDEX_CHECKED_FOCUS;
		}
	}
	else
	{
		if (mbIsChecked == false)
		{
			//��ͨ����̬
			llIndex = ARRAY_INDEX_NORMAL;
		}
		else
		{
			//Checked����̬
			llIndex = ARRAY_INDEX_CHECKED_NORMAL;
		}
	}

	return llIndex;
}

//������رն�����ʱ��
void CEvButton::StartPlayTimer()
{
	do
	{
		LONG llIndex = GetCurrentStatesArrayIndex();
		mlCurrentPage = 0;
		if (mdArrayFrame[llIndex].Count <= 1 && mbIsPlayTimer != false)
		{
			mpIterator->KillTimer(BUTTON_TIMER_ID_PAGE);
			mbIsPlayTimer = false;
		}
		else if (mdArrayFrame[llIndex].Count > 1 && mbIsPlayTimer == false && mpIterator->IsVisible() != false)
		{
			mpIterator->SetTimer(BUTTON_TIMER_ID_PAGE, MAXULONG32, 100, NULL);
			mbIsPlayTimer = true;
		}

	} while (false);

}

//�л���ʾ֡,��һ֡Ϊ1
ERESULT CEvButton::OnPlayAnimation(LONG nlIndex)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		if (nlIndex <= 0 || nlIndex > mlPageCountMax)
			break;

		mlCurrentPage = nlIndex - 1;

		lResult = ERESULT_SUCCESS;

	} while (false);

	return lResult;
}

// ��������
ERESULT CEvButton::OnMouseOwnerTest(const D2D1_POINT_2F& rPoint)
{
	ERESULT luResult = ERESULT_SUCCESS;

	do
	{
		if (mpIterator->IsVisible() == false || mpIterator->IsEnable() == false)
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

//�ı䰴ť����
ERESULT CEvButton::OnChangeText(wchar_t* npswText)
{
	ERESULT luResult = ERESULT_SUCCESS;

	do
	{
		BREAK_ON_NULL(npswText);

		CMM_SAFE_DELETE(mpszButtonText);	//���ԭ�����ַ�������
		int liLen = wcslen(npswText) + 1;
		mpszButtonText = new wchar_t[liLen];
		BREAK_ON_NULL(mpszButtonText);
		wcscpy_s(mpszButtonText, liLen, npswText);	//Copy������

		//�����µ�����ͼƬ
		ReCreateTextBmp();

		RelocateText(); //���¶�λ����λ��

		EinkuiGetSystem()->UpdateView();

		luResult = ERESULT_MOUSE_OWNERSHIP;

	} while (false);

	return luResult;
}


//������ʾͼƬ
ERESULT CEvButton::OnChangeBackGround(wchar_t* npswPicPath, bool nbIsFullPath)
{
	ERESULT leResult = ERESULT_UNSUCCESSFUL;

	do
	{
		if (npswPicPath == NULL || npswPicPath[0] == UNICODE_NULL)
			break;

		//????������ô�򵥴������ﻹ��Ҫ���¼���ͼƬ�������Ϣ
		CMM_SAFE_RELEASE(mpBgBitmap);	//ȥ��ԭ����ͼƬ

		mpBgBitmap = EinkuiGetSystem()->GetAllocator()->LoadImageFile(npswPicPath, nbIsFullPath);
		BREAK_ON_NULL(mpBgBitmap);

		EinkuiGetSystem()->UpdateView();

		leResult = ERESULT_SUCCESS;

	} while (false);

	return leResult;
}

//������������ͼƬ
bool CEvButton::ReCreateTextBmp()
{
	//�����ṹ��
	STETXT_BMP_INIT ldInit;
	ZeroMemory(&ldInit, sizeof(STETXT_BMP_INIT));
	ldInit.Text = mpszButtonText;
	ldInit.FontName = mpswFontName;
	ldInit.FontSize = mdwFontSize;
	ldInit.TextColor = false != mpIterator->IsEnable() ? mdwColor : mdwDisabledColor;

	LONG liTextMaxWidth = mpTemplete->QuerySubKeyValueAsLONG(TF_ID_BT_TEXT_MAXWIDTH, 0);
	LONG liTextMaxHeight = mpTemplete->QuerySubKeyValueAsLONG(TF_ID_BT_TEXT_MAXHEIGHT, 0);
	if (liTextMaxWidth > 0)
	{
		//Ϊ������...
		ldInit.Limit = STETXT_BMP_INIT::EL_FIXEDSIZE;
		ldInit.Width = liTextMaxWidth;
		ldInit.Height = liTextMaxHeight;
	}


	CMM_SAFE_RELEASE(mpTextBitmap);	//ȥ��ԭ����ͼƬ

	if (mdwFontSize == 0 || mpswFontName[0] == UNICODE_NULL)
		return false;

	mpTextBitmap = EinkuiGetSystem()->GetAllocator()->CreateImageByText(ldInit);

	if (NULL == mpBgBitmap && mpTextBitmap != NULL)			// ���û�б���ͼ����������ͼ�Ĵ�С��Ϊ���
	{
		mpIterator->SetSize((FLOAT)mpTextBitmap->GetWidth(), (FLOAT)mpTextBitmap->GetHeight());
	}

	return mpTextBitmap == NULL ? false : true;
}
