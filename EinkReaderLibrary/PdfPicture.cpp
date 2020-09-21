/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#include "StdAfx.h"
#include "PdfPicture.h"
#include "XCtl.h"
#include "stdio.h"
#include "cmmStrHandle.h"
#include "cmmPath.h"
#include "MsgDefine.h"
#include <Shellapi.h>
#include <Shlobj.h>
#include <Shlwapi.h>
#include <Wincrypt.h>

DEFINE_BUILTIN_NAME(PdfPicture)

// �����ɫ����
const unsigned char pixelGradeImprovement1[256] = \
	{	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x01, 0x03, 0x04, 0x06, 0x07, 0x08, 0x0a, 0x0b, 0x0d, 0x0e, 0x0f, 0x11, 0x12,
		0x14, 0x15, 0x16, 0x18, 0x19, 0x1a, 0x1c, 0x1d, 0x1f, 0x20, 0x21, 0x23, 0x24, 0x26, 0x27, 0x28,
		0x2a, 0x2b, 0x2d, 0x2e, 0x2f, 0x31, 0x32, 0x34, 0x35, 0x36, 0x38, 0x39, 0x3b, 0x3c, 0x3d, 0x3f,
		0x40, 0x41, 0x43, 0x44, 0x46, 0x47, 0x48, 0x4a, 0x4b, 0x4d, 0x4e, 0x4f, 0x51, 0x52, 0x54, 0x55,
		0x56, 0x58, 0x59, 0x5b, 0x5c, 0x5d, 0x5f, 0x60, 0x62, 0x63, 0x64, 0x66, 0x67, 0x69, 0x6a, 0x6b,
		0x6d, 0x6e, 0x6f, 0x71, 0x72, 0x74, 0x75, 0x76, 0x78, 0x79, 0x7b, 0x7c, 0x7d, 0x7f, 0x80, 0x82,
		0x83, 0x84, 0x86, 0x87, 0x89, 0x8a, 0x8b, 0x8d, 0x8e, 0x90, 0x91, 0x92, 0x94, 0x95, 0x96, 0x98,
		0x99, 0x9b, 0x9c, 0x9d, 0x9f, 0xa0, 0xa2, 0xa3, 0xa4, 0xa6, 0xa7, 0xa9, 0xaa, 0xab, 0xad, 0xae,
		0xb0, 0xb1, 0xb2, 0xb4, 0xb5, 0xb7, 0xb8, 0xb9, 0xbb, 0xbc, 0xbe, 0xbf, 0xc0, 0xc2, 0xc3, 0xc4,
		0xc6, 0xc7, 0xc9, 0xca, 0xcb, 0xcd, 0xce, 0xd0, 0xd1, 0xd2, 0xd4, 0xd5, 0xd7, 0xd8, 0xd9, 0xdb,
		0xdc, 0xde, 0xdf, 0xe0, 0xe2, 0xe3, 0xe5, 0xe6, 0xe7, 0xe9, 0xea, 0xeb, 0xed, 0xee, 0xf0, 0xf1,
		0xf2, 0xf4, 0xf5, 0xf7, 0xf8, 0xf9, 0xfb, 0xfc, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

// �ض���ɫ����
//const unsigned char pixelGradeImprovement2[256] = \
//	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
//	0x01, 0x01, 0x01, 0x02, 0x02, 0x02, 0x03, 0x03, 0x03, 0x04, 0x04, 0x05, 0x06, 0x06, 0x07, 0x08,
//	0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x14, 0x15, 0x16, 0x17,
//	0x18, 0x19, 0x1a, 0x1b, 0x1d, 0x1e, 0x20, 0x21, 0x22, 0x24, 0x25, 0x27, 0x29, 0x2a, 0x2c, 0x2d,
//	0x2f, 0x31, 0x33, 0x34, 0x36, 0x38, 0x3a, 0x3c, 0x3e, 0x40, 0x42, 0x44, 0x45, 0x47, 0x49, 0x4b,
//	0x4d, 0x4f, 0x52, 0x54, 0x56, 0x58, 0x5b, 0x5d, 0x5f, 0x62, 0x64, 0x67, 0x69, 0x6c, 0x6e, 0x71,
//	0x74, 0x76, 0x79, 0x7c, 0x7e, 0x81, 0x83, 0x85, 0x88, 0x8b, 0x8e, 0x91, 0x94, 0x97, 0x9a, 0x9d,
//	0xa0, 0xa3, 0xa6, 0xa9, 0xac, 0xb0, 0xb3, 0xb6, 0xb9, 0xbd, 0xc0, 0xc3, 0xc7, 0xca, 0xce, 0xd1,
//	0xd3, 0xd6, 0xda, 0xde, 0xe1, 0xe5, 0xe8, 0xec, 0xf0, 0xf4, 0xf7, 0xfb, 0xff, 0xff, 0xff, 0xff,
//	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
const unsigned char pixelGradeImprovement2[256] = \
{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f,
0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f,
0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f,
0x4f, 0x4f, 0x4f, 0x4f, 0x4f, 0x4f, 0x4f, 0x4f, 0x4f, 0x4f, 0x4f, 0x4f, 0x4f, 0x4f, 0x4f, 0x4f,
0x5f, 0x5f, 0x5f, 0x5f, 0x5f, 0x5f, 0x5f, 0x5f, 0x5f, 0x5f, 0x5f, 0x5f, 0x5f, 0x5f, 0x5f, 0x5f,
0x6f, 0x6f, 0x6f, 0x6f, 0x6f, 0x6f, 0x6f, 0x6f, 0x6f, 0x6f, 0x6f, 0x6f, 0x6f, 0x6f, 0x6f, 0x6f,
0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f,
0x8f, 0x8f, 0x8f, 0x8f, 0x8f, 0x8f, 0x8f, 0x8f, 0x8f, 0x8f, 0x8f, 0x8f, 0x8f, 0x8f, 0x8f, 0x8f,
0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f,
0xaf, 0xaf, 0xaf, 0xaf, 0xaf, 0xaf, 0xaf, 0xaf, 0xaf, 0xaf, 0xaf, 0xaf, 0xaf, 0xaf, 0xaf, 0xaf,
0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf,
0xcf, 0xcf, 0xcf, 0xcf, 0xcf, 0xcf, 0xcf, 0xcf, 0xcf, 0xcf, 0xcf, 0xcf, 0xcf, 0xcf, 0xcf, 0xcf,
0xdf, 0xdf, 0xdf, 0xdf, 0xdf, 0xdf, 0xdf, 0xdf, 0xdf, 0xdf, 0xdf, 0xdf, 0xdf, 0xdf, 0xdf, 0xdf,
0xef, 0xef, 0xef, 0xef, 0xef, 0xef, 0xef, 0xef, 0xef, 0xef, 0xef, 0xef, 0xef, 0xef, 0xef, 0xef,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

CPdfPicture::CPdfPicture(void)
{
	mpszSourcePath[0] = UNICODE_NULL;
	mhandleLogFile = INVALID_HANDLE_VALUE;
	mpSelectAnnotList = NULL;
	mpSelectStextList = NULL;
	mbIsLog = false;

	mulPageCount = 0;
	mfMiddleLineX = 0.0f;

	pdfDoc = NULL;
	pdfCrtPage = NULL;
	pdfCrtPage2 = NULL;
	pdfImage = NULL;
	pdfImage2 = NULL;
	mpElBitmap = NULL;
	pageCanvas = NULL;
	pageCanvasSize = 0;
	landScope = true;
	duopageMode = false;
	pageGap = 0;// 20;
	pageNo = 0;
	mIsTxtDoc = false;
	mFontSize = 12;
	mTitleBarHeight = 0.0f;
	mLoading = 0;
	mpLoadingView = NULL;
	miDocType = DOC_TYPE_PDF;
	mfPenWidth = 2.0f;
	mdPenColor.a = 255;
	mdPenColor.r = 0;
	mdPenColor.g = 0;
	mdPenColor.b = 0;
	ZeroMemory(&mdFwLineRect, sizeof(mdFwLineRect));

	mpLineBrush = NULL;
	mbIsModify = false;
	mpEarseRedoUndoStrokeList = NULL;
	mpHigilightBrush = NULL;
	mbIsPageChange = false;

	wchar_t* const moduleName[] = {
		L"EdPdf.dll",
		L"EdSmt.dll",
		L"EdTxt.dll"
	};	// ��Ҫ����pdfModuleArr���飬Ŀǰ��10

	for (auto &i : pdfModuleArr)
		i = NULL;

	for (auto j : moduleName)
	{
		CFilePathName loPath;
		loPath.SetByModulePathName();
		loPath.Transform(j);

		OutputDebugString(loPath.GetPathName());
		auto libModule = LoadLibraryEx(loPath.GetPathName(), NULL, LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
		if (libModule != NULL)
		{
			auto proc = (GetModule_Proc)GetProcAddress(libModule, "EdGetModule");
			for (auto &i : pdfModuleArr)
			{
				if (i == NULL)
				{
					i = proc();
					break;
				}
			}
		}
	}


}


CPdfPicture::~CPdfPicture(void)
{
	if (pageCanvas != NULL)
		HeapFree(GetProcessHeap(), 0, pageCanvas);

	CloseFile(false);

	if (mhandleLogFile != INVALID_HANDLE_VALUE && mhandleLogFile != NULL)
		CloseHandle(mhandleLogFile);
}

//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
ERESULT CPdfPicture::OnElementCreate(IEinkuiIterator* npIterator)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		if(CXuiElement::OnElementCreate(npIterator) != ERESULT_SUCCESS)
			break;

		mpIterator->ModifyStyles(EITR_STYLE_POPUP);
		
		lResult = ERESULT_SUCCESS;

	}while(false);

	return lResult;
}

// ����Ԫ�أ�����֪ͨһ��Ԫ�����٣�ע�⣺��Ԫ�������յ�����Ϣ��Ӧ�ü�ʱ����Ԫ�ع�������UnregisterElement������
// �Ӷ�����Ԫ�ع�������������һ��Ԫ�ط���������Ϣ�������ٽ��Լ���Ԫ�ع�����ע���������ͷ��������
ERESULT CPdfPicture::OnElementDestroy()
{
	CMM_SAFE_RELEASE(mpLineBrush);

	CXuiElement::OnElementDestroy();

	return ERESULT_SUCCESS;
}

ULONG CPdfPicture::InitOnCreate(
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
		/*mpIterBtShowTip = mpIterator->GetSubElementByID(PP_BT_SHOW_TIP);
		BREAK_ON_NULL(mpIterBtShowTip);*/
		

		// ������ˢ
		mpXuiBrush = EinkuiGetSystem()->CreateBrush(XuiSolidBrush, D2D1::ColorF(0.0f, 0.0f, 0.0f));
		BREAK_ON_NULL(mpXuiBrush);

		mpXuiBrush->SetStrokeWidth(2.0f);
		mpXuiBrush->SetStrokeType(
			D2D1::StrokeStyleProperties(
				D2D1_CAP_STYLE_FLAT,
				D2D1_CAP_STYLE_FLAT,
				D2D1_CAP_STYLE_ROUND,
				D2D1_LINE_JOIN_MITER,
				10.0f,
				D2D1_DASH_STYLE_DOT,
				0.0f),
			0,
			NULL);

		leResult = ERESULT_SUCCESS;

	} while (false);

	CMM_SAFE_RELEASE(lpSubKey);

	// ��ϵͳע����Ҫ�յ�����Ϣ
	return leResult;
}

void __stdcall PageLoadedCallBack(uint32Eink loadingStep, uint32Eink pageCount, CPdfPicture* viewObject)
{
	viewObject->OnPageLoaded(loadingStep, pageCount);
}

void __stdcall ThumbnailsLoadedCallBack(uint32Eink pageNum, CPdfPicture* viewObject)
{
	viewObject->OnThumbnailLoaded(pageNum);
}

//����ͼ����
void CPdfPicture::OnThumbnailLoaded(int32 loadingStep)
{
	if (loadingStep == MAXULONG32)
	{
		PostMessageToParent(EEVT_THUMBNAIL_COMPLETE, loadingStep);
	}
	
}

void CPdfPicture::OnPageLoaded(int32 loadingStep, int32 pagecount)
{
	static int count = 0;

	InterlockedExchange(&mLoading, loadingStep + 1);

	if (pdfCrtPage != NULL)
		pageNo = pdfCrtPage->GetPageIndex();

	mulPageCount = pagecount;

	switch (loadingStep)
	{
	case 0:
		// �����ʾ�Ű濪ʼ
		PostMessageToParent(EEVT_TXT_ARRANGED_START, CExMessage::DataInvalid);
		break;
	case MAXULONG32:
		{
			// �����ʾ�Ű����
			PostMessageToParent(EEVT_ARRANGED_COMPLETE, pagecount);

			if (mpLoadingView != NULL)
			{
				mpLoadingView->ExitModal();
				mpLoadingView = NULL;
			}

			// ����CPdfPicture::OpenFile(wchar_t* npszPath, PPAGE_PDF_CONTEXT npPageContext)��������������ģ̬�Ի�������Ϣ��������Ի����˳�
			// ???niu???
			break;
		}
	default:
		
		PostMessageToParent(EEVT_TXT_ARRANGED_DOING, pagecount);
		break;
	}
}

//������־����
void CPdfPicture::SetLogStatus(bool nbIsEnable)
{
	mbIsLog = nbIsEnable;
}

void CPdfPicture::SetLoadingPageIndex(LONG nlPageIndex)
{
	if (mpLoadingView != NULL)
	{
		mpLoadingView->SetPage(nlPageIndex);
	}
}

//��ť�����¼�
ERESULT CPdfPicture::OnCtlButtonClick(IEinkuiIterator* npSender)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		ULONG llBtnID = npSender->GetID();
		switch (llBtnID)
		{
		case PP_BT_SHOW_TIP:
		{
			PostMessageToParent(EEVT_ER_SHOW_TIP, CExMessage::DataInvalid);
			break;
		}
		default:
			break;
		}


		lResult = ERESULT_SUCCESS;
	} while (false);

	return lResult;
}

//�ʼ�����Ƥˢ��
void CPdfPicture::InkInputUpdate(void)
{
	try
	{
		mbIsPageChange = true;

		SvcDebugOutFmt("ink input update");
		EinkuiGetSystem()->EnablePaintboard(false);
		SvcDebugOutFmt("enable paintboard");

		bool lbRet = RenderPages();
		if (lbRet != false)
		{
			SvcDebugOutFmt("ClearEinkBuffer");
			EinkuiGetSystem()->ClearEinkBuffer();

			SvcDebugOutFmt("UpdateView");
			EinkuiGetSystem()->UpdateView(true);
			//OutputDebugString(L"EinkuiGetSystem UpdateView");
		}
	}
	catch (...)
	{

	}

	mpIterator->KillTimer(PP_TIMER_ID_SAVE_FILE);
	mpIterator->SetTimer(PP_TIMER_ID_SAVE_FILE, 1, 800, NULL);
}

//��ʱ��
void CPdfPicture::OnTimer(
	PSTEMS_TIMER npStatus
	)
{
	if (npStatus->TimerID == PP_TIMER_ID_UPDATE)
	{
		SvcDebugOutFmt("PP_TIMER_ID_UPDATE");
		InkInputUpdate();
	}
	else if (npStatus->TimerID == PP_TIMER_ID_ENABLE_FRAME)
	{
		//bool lbRet = RenderPages();
		SvcDebugOutFmt("enable paintboard timer");
		EinkuiGetSystem()->EnablePaintboard(false);
		mpIterator->KillTimer(PP_TIMER_ID_UPDATE);
		mpIterator->SetTimer(PP_TIMER_ID_UPDATE, 1, 800, NULL);
		//EinkuiGetSystem()->ClearEinkBuffer();
		//EinkuiGetSystem()->UpdateView(true);
	}
	else if (npStatus->TimerID == PP_TIMER_ID_SAVE_FILE)
	{
		try
		{
			if ((mpszSourcePath != NULL) && (pdfDoc != NULL))
			{
				SvcDebugOutFmt("SaveAllChanges");
				if (pdfDoc->SaveAllChanges(mpszSourcePath) == false)
				{
					SvcDebugOutFmt("SaveAllChanges fail");

				}

				SvcDebugOutFmt("SaveAllChanges end");
			}
		}
		catch (...)
		{

		}
	}
}

//Ԫ�زο��ߴ緢���仯
ERESULT CPdfPicture::OnElementResized(D2D1_SIZE_F nNewSize)
{
	//CExMessage::SendMessage(mpIterBtFull, mpIterator, EACT_BUTTON_SET_ACTION_RECT, nNewSize);
	////mpIterLineOne->SetSize(nNewSize.width, mpIterLineOne->GetSize().height);

	//mpIterBtOk->SetPosition(nNewSize.width - 85, mpIterBtOk->GetPositionY());
	if (nNewSize.width == 0 || nNewSize.height == 0)
		return ERESULT_SUCCESS;

	ED_SIZE viewPort;
	viewPort.x = CExFloat::ToLong(nNewSize.width);
	viewPort.y = CExFloat::ToLong(nNewSize.height);

	mViewCtl.SetViewPort(viewPort);
	if (pdfDoc != NULL && pdfCrtPage !=NULL)
	{
		pdfDoc -> SetViewPort(CExFloat::ToLong(viewPort.x-176.0f), CExFloat::ToLong(viewPort.y - 176.0f - mTitleBarHeight));	// ͼ��ߵ���Сһ������

		//if (IsTxtDocument() != false)
		//{
			auto newPage = pdfDoc->Rearrange(pdfCrtPage);

			mulPageCount = pdfDoc->GetPageCount();

			GoToPage(newPage, true);
		//}

	}

	if(mpLoadingView != NULL)
		mpLoadingView->GetIterator()->SetSize(mpIterator->GetSize());

	return ERESULT_SUCCESS;
}

//��ȡ�ĵ�����
IEdDocument_ptr CPdfPicture::GetDoc(void)
{
	return pdfDoc;
}

//����
bool CPdfPicture::HashBuffer(cmmStringW& input, cmmStringW& output)
{
	HCRYPTPROV hProv = 0;
	HCRYPTHASH hHash = 0;
	BYTE resultBuf[128];
	DWORD cbHash = 0;

	// Get handle to the crypto provider
	if (!CryptAcquireContext(&hProv,
		NULL,
		NULL,
		PROV_RSA_FULL,
		CRYPT_VERIFYCONTEXT))
	{
		return false;
	}

	if (!CryptCreateHash(hProv, CALG_MD5/*CALG_SSL3_SHAMD5*/, 0, 0, &hHash))
	{
		return false;
	}

	if (!CryptHashData(hHash, (BYTE*)input.ptr(), input.size() * sizeof(wchar_t), 0))
	{
		return false;
	}

	cbHash = 128;
	if (!CryptGetHashParam(hHash, HP_HASHVAL, resultBuf, &cbHash, 0))
		return false;

	if (output.DumpBinBuf(resultBuf, cbHash) == false)
		return false;

	CryptDestroyHash(hHash);
	CryptReleaseContext(hProv, 0);

	return true;
}

//��ȡ����ͼ·��
bool CPdfPicture::GetThumbnailPath(const wchar_t* npDocPathName, bool nbIsOpen)
{
	cmmStringW trail;
	cmmStringW hashName;
	FILETIME createTime, accessTime, modifyTime;
	HANDLE fileHandle;
	bool lbRet = false;

	do 
	{
		if(npDocPathName == NULL || npDocPathName[0] == UNICODE_NULL)
			break;

		// ��ȡԭʼ�ļ����޸�ʱ��ʹ�С
		fileHandle = CreateFile(npDocPathName, FILE_READ_ATTRIBUTES, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (fileHandle == INVALID_HANDLE_VALUE)
			break;

		if (GetFileTime(fileHandle, &createTime, &accessTime, &modifyTime) == FALSE)
		{
			modifyTime.dwLowDateTime = modifyTime.dwHighDateTime = 0;
		}

		DWORD hiSize;
		DWORD lowSize = GetFileSize(fileHandle, &hiSize);

		CloseHandle(fileHandle);
		trail.format(L"%s:%d%d", npDocPathName, modifyTime.dwLowDateTime, modifyTime.dwHighDateTime);

		// ��ȡ�ļ�������
		if (HashBuffer(trail, hashName) == false)
			break;

		
		wchar_t lszOldPath[MAX_PATH] = { 0 };
		wcscpy_s(lszOldPath, MAX_PATH, mCachePath.GetPathName());
		lszOldPath[wcslen(lszOldPath) + 1] = UNICODE_NULL; //˫0��β

		mCachePath.SetByUserAppData();
		mCachePath.AssurePath();
		mCachePath += L"EinkReader\\";
		mCachePath += hashName.ptr();
		mCachePath.AssurePath();

		//�ж�·�����Ƿ��б仯
		if (nbIsOpen == false && GetDocType() == DOC_TYPE_PDF && _wcsicmp(lszOldPath,mCachePath.GetPathName()) != 0)
		{
			//˵����PDF���Լ��ļ����޸ĵ�,��Ҫrenameһ���ļ���
			wchar_t lszNewPath[MAX_PATH] = { 0 };
			wcscpy_s(lszNewPath, MAX_PATH, mCachePath.GetPathName());
			lszNewPath[wcslen(lszNewPath) + 1] = UNICODE_NULL; //˫0��β

			SHFILEOPSTRUCT ldShfile;
			memset(&ldShfile, 0, sizeof(ldShfile));
			ldShfile.pFrom = lszOldPath;
			ldShfile.pTo = lszNewPath;
			ldShfile.fFlags = FOF_NO_UI | FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR;
			ldShfile.wFunc = FO_RENAME;
			SHFileOperation(&ldShfile);

			
			CExMessage::SendMessageWithText(mpIterator->GetParent(), mpIterator, EEVT_UPDATE_THUMBNAIL_PATH, lszNewPath);
		}
		else
		{
			//���û�����Ŀ¼���������Ŀ¼
			mCachePath.CreatePath();
		}

		lbRet = true;

	} while (false);
	

	return lbRet;
}


//����ҪԤ����ͼƬ,�ͽ�����Ϣ�Ķ���
ULONG CPdfPicture::OpenFile(wchar_t* npszPath, PPAGE_PDF_CONTEXT npPageContext, wchar_t* npszSource)
{
	wchar_t* lpszFilePath = npszPath;

	ULONG lulRet = EDERR_UNSUCCESSFUL;
	bool updateLayout = true;
	PAGE_PDF_CONTEXT defaultContext = { 0,0,0 };

	mpszSourcePath[0] = UNICODE_NULL;

	__try
	{
		if (npPageContext != NULL)
		{
			defaultContext.pageIndex = PAGEID_LIB(npPageContext->pageIndex);
			defaultContext.pageContext = npPageContext->pageContext;
			defaultContext.pageContext2 = npPageContext->pageContext2;
		}


		CloseFile(false);
		CMM_SAFE_RELEASE(mpSelectStextList);
		CMM_SAFE_RELEASE(mpSelectAnnotList);

		//���redo undo
		mcRedoUndoManager.ClearRedo();
		mcRedoUndoManager.ClearUndo();
		SetRedoUndoStatus();
		mbIsModify = false;

		//CMMASSERT(pdfDoc == NULL);

		do
		{
			BREAK_ON_NULL(lpszFilePath);
			IEdModule_ptr ldModule = NULL;

			int liLastChar = int(wcslen(lpszFilePath) - 1);
			if (lpszFilePath[liLastChar] == 't')
			{
				ldModule = pdfModuleArr[2];
				miDocType = DOC_TYPE_TXT;
			}
			else if (lpszFilePath[liLastChar] == 'b')
			{
				ldModule = pdfModuleArr[1];
				miDocType = DOC_TYPE_EPUB;
				defaultContext.pageIndex = 0;
			}
			else if (lpszFilePath[liLastChar] == 'i')
			{
				ldModule = pdfModuleArr[1];
				miDocType = DOC_TYPE_MOBI;
				defaultContext.pageIndex = 0;
			}
			else if (lpszFilePath[liLastChar] == 'f')
			{
				ldModule = pdfModuleArr[0];
				lpszFilePath = npszSource;
				miDocType = DOC_TYPE_PDF;
			}

			//for (auto i : pdfModuleArr)
			{
				if (ldModule == NULL)
					break;

				ULONG lulRet = ldModule->OpenDocument(lpszFilePath, &pdfDoc);
				if (ERR_SUCCESS(lulRet) && pdfDoc != NULL)
				{
					InterlockedExchange(&mLoading, 1);

					if (pdfDoc->LoadAllPage((PEDDOC_CALLBACK)PageLoadedCallBack, (void*)this, &defaultContext) != false)	//???niu: ���ӶԵ����������Ļ�ȡ�ͱ��棬������������ĵ�������ķ���ҳ��
					{

						if (_wcsicmp(ldModule->GetTypeName(pdfDoc->GetDocType()), L"TXT") == 0)
						{
							mIsTxtDoc = true;
						}
						else
						{
							mIsTxtDoc = false;

							if (mLoading != 0)
							{
								/*ICfKey* lpSubKey = mpTemplete->OpenKey(L"Loading");
								mpLoadingView = CLoadingView::CreateInstance(EinkuiGetSystem()->GetCurrentWidget()->GetHomePage(), lpSubKey);
								wchar_t* lpszFileName = wcsrchr(lpszFilePath, L'\\');
								if (lpszFileName != NULL)
									lpszFileName = lpszFileName + 1;
								mpLoadingView->SetData(lpszFileName);
								mpLoadingView->GetIterator()->SetSize(mpIterator->GetSize());
								mpLoadingView->DoModal(&mLoading, NULL);
								mpLoadingView = NULL;
								CMM_SAFE_RELEASE(lpSubKey);*/

							}

							// �������PageLoadedCallBack�����У�����ģ̬�Ի�������Ϣ��������Ի����˳�
						}
						//break;
					}
					else
						CMM_SAFE_RELEASE(pdfDoc);


					
				}
			}

			BREAK_ON_NULL(pdfDoc);
			LoadThumbnails(npszSource);

			if (mIsTxtDoc != false)
				mTitleBarHeight = 0.0f;
			else
				mTitleBarHeight = 0.0f;

			if (mViewCtl.GetViewPort().x > 0 && mViewCtl.GetViewPort().y > 0)
			{
				pdfDoc->SetViewPort(CExFloat::ToLong(mViewCtl.GetViewPort().x - 176.0f), CExFloat::ToLong(mViewCtl.GetViewPort().y - 176.0f - mTitleBarHeight));	// ͼ��ߵ���Сһ������
			}
			else
				updateLayout = false;

			if (mFontSize > 0)
				pdfDoc->SetFont(L"΢���ź�", mFontSize);
			else
				updateLayout = false;

			if (updateLayout != false)
			{
				pdfCrtPage = pdfDoc->Rearrange(&defaultContext);
				if (pdfCrtPage == NULL)
				{
					//���ļ�ʧ��
					CloseFile();
					lulRet = EDERR_UNSUCCESSFUL;
					break;
				}

				//�ļ���ҳ��
				mulPageCount = pdfDoc->GetPageCount();
			}
			else
				mulPageCount = 0;

			//duopageMode = false;

			wcscpy_s(mpszSourcePath, MAX_PATH, npszSource);

			lulRet = EDERR_SUCCESS;

		} while (false);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{

	}


	return lulRet;
}

//��������ͼ
void CPdfPicture::LoadThumbnails(const wchar_t* npszFilePath, bool nbIsOpen)
{
	if (pdfDoc != NULL)
	{
		GetThumbnailPath(npszFilePath,nbIsOpen);
		pdfDoc->LoadAllThumbnails((PEDDOC_THUMBNAILS_CALLBACK)ThumbnailsLoadedCallBack, (void*)this, mCachePath.GetPathName());
	}
		
}

void CPdfPicture::CloseFile(bool nbUpdateView)
{
	__try
	{
		if (GetDocType() == DOC_TYPE_PDF)
		{
			//���һ��Ҫ�رյ��ļ��Ƿ���Ҫ��������ͼĿ¼����
			RefreshThumbnail();
			GetThumbnailPath(mpszSourcePath, false);
		}

		CMM_SAFE_RELEASE(pdfImage);
		CMM_SAFE_RELEASE(pdfImage2);
		CMM_SAFE_RELEASE(pdfCrtPage);
		CMM_SAFE_RELEASE(pdfCrtPage2);
		CMM_SAFE_RELEASE(mpElBitmap);

		CMM_SAFE_RELEASE(pdfDoc);

		mCachePath.Clean();
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		pdfDoc = NULL;
	}
	mulPageCount = 0;

	if (nbUpdateView != false)
	{
		SvcDebugOutFmt("ClearEinkBuffer");
		EinkuiGetSystem()->ClearEinkBuffer();

		SvcDebugOutFmt("UpdateView");
		EinkuiGetSystem()->UpdateView();
	}
}

bool CPdfPicture::IsTxtDocument(void)
{
	return mIsTxtDoc;
}

//ҳ����ת
bool CPdfPicture::GoToPage(ULONG nulPageNumber, bool renderPage)
{
	if (nulPageNumber > mulPageCount)
		nulPageNumber = mulPageCount;

	IEdPage_ptr pageGoto;

	pageGoto = pdfDoc->GetPage(PAGEID_LIB(nulPageNumber));
	if (pageGoto == NULL)
		return false;

	return GoToPage(pageGoto, renderPage);
}

bool CPdfPicture::GoToPage(IEdPage_ptr pageGoto, bool renderPage)
{
	bool lbRet = false;
	IEdPage_ptr pageDuo = NULL;
	ED_SIZE sizePageGoto, sizePageDuo;
	int liOldPageNumber = pdfCrtPage!=NULL?pdfCrtPage->GetPageIndex():-1;

	RefreshThumbnail();

	if (pageGoto == NULL)
	{
		if (pdfCrtPage == NULL)
			return false;

		pageGoto = pdfCrtPage;
		pageGoto->AddRefer();	// �������ã���ֹ������ͷ�pdfCrtPage������ͬ����ͬһ����ɾ��
	}

	Sleep(50);
	// �����˫ҳ��ʾģʽ���������ҳ���䣬����ҳ
	if (landScope != false && duopageMode != false)
	{
		if ((pageGoto->GetPageIndex() % 2) != 0)		// ����ط����������ģ����ڵ��к��ǲ�����Ϊ׼ȷ�жϣ���Ϊװ������ڱ仯������Ҫ�����ң��޸ĵ�Ŀ�ģ��Ұ���ĳ���Ҫ��Ч���� ???niu???
		{
			//˫ҳ��ʾҪ������ҳ��ʼ
			pageDuo = pageGoto;
			pageGoto = pdfDoc->GetPage(pageGoto, -1);
		}
		else
		{
			pageDuo = pdfDoc->GetPage(pageGoto, 1);	// ȡ��һҳ
			//bug �����ַ�pageDuoΪ��
		}
	}

	GetPageSize(pageGoto,sizePageGoto);

	//pageGoto->AddRefer();
	CMM_SAFE_RELEASE(pdfCrtPage);
	pdfCrtPage = pageGoto;

	if (pageDuo != NULL)
	{
		GetPageSize(pageDuo, sizePageDuo);

		sizePageGoto.x += sizePageDuo.x;
		mViewCtl.SetCenterGap((float)pageGap);

		CMM_SAFE_RELEASE(pdfCrtPage2);
		pdfCrtPage2 = pageDuo;
	}
	else
	{
		CMM_SAFE_RELEASE(pdfCrtPage2);
		mViewCtl.SetCenterGap(0.0f);
	}

	mViewCtl.SetImageInit(sizePageGoto);

	if (renderPage)
	{
		lbRet = RenderPages();
		if (lbRet != false)
		{
			SvcDebugOutFmt("ClearEinkBuffer");
			EinkuiGetSystem()->ClearEinkBuffer();

			SvcDebugOutFmt("UpdateView");
			EinkuiGetSystem()->UpdateView();
		}
	}

	if (pdfCrtPage==NULL || liOldPageNumber != pdfCrtPage->GetPageIndex())
	{
		CMM_SAFE_RELEASE(mpSelectStextList);
		CMM_SAFE_RELEASE(mpSelectAnnotList);
	}
	
	mbIsPageChange = false;

	return lbRet;
}

//���ñ�״̬
void CPdfPicture::SetPenmode(ULONG nulPenMode)
{
	if (nulPenMode == PEN_MODE_PEN || nulPenMode == PEN_MODE_ERASER)
	{
		CMM_SAFE_RELEASE(mpSelectStextList);
		CMM_SAFE_RELEASE(mpSelectAnnotList);
	}
}

// ��ȡѡ��������Ϣ
void CPdfPicture::GetSelectHighlightInfo(SELECT_HIGHLIGHT& rdHighltght, ED_RECTF& rdRect)
{
	if (mpSelectStextList  != NULL)
	{
		rdHighltght.AnnotCount = mpSelectAnnotList->GetCount();
		rdHighltght.QuadCount = mpSelectStextList->GetQuadCount();
		ED_POINTF ldPos;
		ED_POINT ldViewPos;
		mpSelectStextList->GetAPoint(&ldPos);
		rdRect.left = ldPos.x;
		rdRect.top = ldPos.y;
		if(mpSelectPage == pdfCrtPage2)
			mViewCtl.IsInPage2(ldPos, ldPos, true);

		mViewCtl.ImageToViewInit((int)ldPos.x, (int)ldPos.y, ldViewPos);		

		rdHighltght.PointA.x = (FLOAT)ldViewPos.x;
		rdHighltght.PointA.y = (FLOAT)ldViewPos.y;

		mpSelectStextList->GetBPoint(&ldPos);
		rdRect.right = ldPos.x;
		rdRect.bottom = ldPos.y;

		if (mpSelectPage == pdfCrtPage2)
			mViewCtl.IsInPage2(ldPos, ldPos, true);
		mViewCtl.ImageToViewInit((int)ldPos.x, (int)ldPos.y, ldViewPos);

		rdHighltght.PointB.x = (FLOAT)ldViewPos.x;
		rdHighltght.PointB.y = (FLOAT)ldViewPos.y;
	}
}

//ѡ��ָ�����������,�������ֿ���
int CPdfPicture::SelectHighlight(const D2D1_RECT_F& rRegion,SELECT_HIGHLIGHT& rdHighltght)
{
	int liRet = 0;
	IEdStructuredTextPage_ptr ldTextPagePtr = NULL;

	do 
	{
		//�ͷ��ϴεĶ���
		CMM_SAFE_RELEASE(mpSelectStextList);
		CMM_SAFE_RELEASE(mpSelectAnnotList);

		wchar_t lszText[8192] = { 0 };

		//����ʾ����ת���ĵ�����
		ED_POINTF ldPos,ldPos2;
		if (mViewCtl.ViewToImageInit((int)rRegion.left, (int)rRegion.top, ldPos) == false)
			break;
		if (mViewCtl.ViewToImageInit((int)rRegion.right, (int)rRegion.bottom, ldPos2) == false)
			break;

		//Ŀǰֻ֧��ͬһҳ�ĸߣ����԰�������㣬����ڵ�һҳ���ʹ����һҳ�������ô��ҳ���ʹ���ڶ�ҳ
		mpSelectPage = pdfCrtPage;
		if (pdfCrtPage2 != NULL && mViewCtl.IsInPage2(ldPos,ldPos) != false)
		{
			//�������ڵڶ�ҳ
			mpSelectPage = pdfCrtPage2;

			if (mViewCtl.IsInPage2(ldPos2,ldPos2) == false)
			{
				//̧���ڵ�һҳ

			}
		}

		ED_RECTF ldRect;
		ldRect.left = (FLOAT)ldPos.x;
		ldRect.top = (FLOAT)ldPos.y;
		ldRect.right = (FLOAT)ldPos2.x;
		ldRect.bottom = (FLOAT)ldPos2.y;

		//��ȡ����ҳ����
		BREAK_ON_NULL(mpSelectPage);
		ldTextPagePtr = mpSelectPage->GetStructuredTextPage();
		BREAK_ON_NULL(ldTextPagePtr);



		ED_POINTF laPoint = { ldRect.left,ldRect.top };
		ED_POINTF lbPoint = { ldRect.right,ldRect.bottom };

		if (ldTextPagePtr->DetectSelectedText(&laPoint, &lbPoint, &mpSelectStextList, &mpSelectAnnotList,true) != false)
		{
			rdHighltght.AnnotCount = mpSelectAnnotList->GetCount();
			rdHighltght.QuadCount = mpSelectStextList->GetQuadCount();

			GetSelectHighlightInfo(rdHighltght,ldRect);

			ldTextPagePtr->CopyText(&ldRect, lszText, 8192);
			//pdfCrtPage->GetSelectedText(&ldRect, lszText, 1024);

			CopyToClipboard(lszText);
		}


		//ED_RECT ldstRect;
		//ED_RECT lsrcRect;
		//UCHAR ledgeImpact;
		//ED_SIZEF ldpageSize;

		//if (mViewCtl.GetViewMapArea(ldstRect, lsrcRect, ldpageSize, &ledgeImpact) == false)
		//{

		//}


	} while (false);

	if (ldTextPagePtr != NULL)
		ldTextPagePtr->Release();

	return liRet;
}

//ɾ������
void CPdfPicture::DeleteHighlight(IEdAnnotList_ptr npSelectAnnotList, CRedoUndoStrokeList* npRedoUndoStrokeList)
{
	if (npSelectAnnotList != NULL && mpSelectPage != NULL)
	{
		//���ѡ�еĶ������Ѿ���������ע�ˣ���ɾ��
		for (int i = 0; i < npSelectAnnotList->GetCount(); i++)
		{
			IEdAnnot_ptr lpAnnot = npSelectAnnotList->GetAnnot(i);
			
			CRedoUndoItem ldDeleteItem;
			ldDeleteItem.IsDelete = true;
			ldDeleteItem.PageNumber = mpSelectPage->GetPageIndex();
			ldDeleteItem.AnnotType = lpAnnot->GetType();

			//�жϲ����Ƿ��ڵ�ǰҳ������
			IEdPage_ptr lpdfModifyPage = GetPageByPageNumber(mpSelectPage->GetPageIndex());
			if (lpdfModifyPage == NULL)
				continue;

			//���л�����
			ldDeleteItem.PenByteDataSize = lpAnnot->SaveToAchive(NULL, 0);
			ldDeleteItem.PenByteData = new char[ldDeleteItem.PenByteDataSize];
			lpAnnot->SaveToAchive(ldDeleteItem.PenByteData, ldDeleteItem.PenByteDataSize);

			npRedoUndoStrokeList->Insert(-1, ldDeleteItem);

			//ɾ��������ʼ�
			lpdfModifyPage->GetAnnotManager()->RemoveAnnot(lpAnnot);
			lpAnnot->Release();

		}
	}
}

//������������Ϣ
void CPdfPicture::HighlightEvent(int niEvent)
{
	CRedoUndoStrokeList* lpRedoUndoStrokeList = new CRedoUndoStrokeList();

	do 
	{
		if (niEvent == HIGHLIGHT_BT_TRANSLATE)
		{
			//��������
			wchar_t lszFilePath[MAX_PATH] = { 0 };
			GetModuleFileName(GetModuleHandle(NULL), lszFilePath, MAX_PATH);
			*(wcsrchr(lszFilePath, L'\\') + 1) = UNICODE_NULL;
			wcscat_s(lszFilePath, MAX_PATH, L"YB2_Smart_Reading.exe");
			ShellExecute(NULL, L"open", lszFilePath, L"TranslationText", NULL, SW_SHOW);
		}
		else if (niEvent == HIGHLIGHT_BT_DELETE)
		{
			//ɾ����ť
			DeleteHighlight(mpSelectAnnotList, lpRedoUndoStrokeList);
		}
		else if (niEvent == HIGHLIGHT_BT_COPY)
		{
			//to do
		}
		else
		{
			//��ɾ��ԭ���ĸ���
			DeleteHighlight(mpSelectAnnotList, lpRedoUndoStrokeList);

			//������������
			if (mpSelectStextList != NULL && mpSelectPage != NULL)
			{
				IEdAnnot_ptr lpUndoPtr = NULL;

				if (niEvent == HIGHLIGHT_BT_HIGHLIGHT)
					lpUndoPtr = mpSelectPage->GetAnnotManager()->AddHighLightAnnot(mpSelectStextList);
				else if (niEvent == HIGHLIGHT_BT_DELETE_LINE)
					lpUndoPtr = mpSelectPage->GetAnnotManager()->AddDeleteLineAnnot(mpSelectStextList);
				else if (niEvent == HIGHLIGHT_BT_UNDER_LINE)
					lpUndoPtr = mpSelectPage->GetAnnotManager()->AddUnderLineAnnot(mpSelectStextList);

				//���undo����
				CRedoUndoItem ldRedoUndoItem;
				ldRedoUndoItem.AnnotType = lpUndoPtr->GetType();
				ldRedoUndoItem.IsDelete = false;
				ldRedoUndoItem.PageNumber = mpSelectPage->GetPageIndex();
				ldRedoUndoItem.PenItemSignature = mpSelectPage->GetAnnotManager()->GetSignature(lpUndoPtr);
				lpUndoPtr->AddRefer();
				lpRedoUndoStrokeList->Insert(0, ldRedoUndoItem);
			}
		}

	} while (false);
	
	if (lpRedoUndoStrokeList != NULL) //�������˫ҳģʽ�º���һ�ʻ�������ҳ�棬��һ�β���
	{
		if (lpRedoUndoStrokeList->Size() > 0)
		{
			mcRedoUndoManager.AddUndo(lpRedoUndoStrokeList);
			mcRedoUndoManager.ClearRedo(); //ֻҪ���������ˣ������redo����

			if (mbIsModify == false)
			{
				//�༭�����ļ���
				mbIsModify = true;

				PostMessageToParent(EEVT_FILE_MODIFY, CExMessage::DataInvalid);
			}

			SetRedoUndoStatus();

			InkInputUpdate();
		}
		else
		{
			delete lpRedoUndoStrokeList;
		}
		
	}

	CMM_SAFE_RELEASE(mpSelectAnnotList);
	CMM_SAFE_RELEASE(mpSelectStextList);
}

bool CPdfPicture::PageFoward(bool forward, bool renderPage)
{
#ifdef _AX_TEST_MEMLEAK
	if (forward == false)
	{
		EinkuiGetSystem()->ExitXui();
		return false;
	}
#endif//_AX_TEST_MEMLEAK
	CMMASSERT(pdfCrtPage != NULL);

	IEdPage_ptr pageJump = NULL;

	if (duopageMode != false && landScope != false)
	{
		//ֻ����˫ҳ���Һ�����
		if (forward != false)
			pageJump = pdfDoc->GetPage(pdfCrtPage, 2);
		else
			pageJump = pdfDoc->GetPage(pdfCrtPage, -2);
	}

	if (pageJump == NULL)
	{
		if (forward != false)
			pageJump = pdfDoc->GetPage(pdfCrtPage, 1);
		else
			pageJump = pdfDoc->GetPage(pdfCrtPage, -1);
	}


	if(pageJump != NULL)
		return GoToPage(pageJump, renderPage);

	return false;
}

ULONG CPdfPicture::GetCurrentPageNumber(ULONG& secondPage)
{
	if (pdfCrtPage == NULL)
		return 0;

	if (pdfCrtPage2 != NULL)
		secondPage = PAGEID_USER(pdfCrtPage2->GetPageIndex());
	else
		secondPage = 0;

	return PAGEID_USER(pdfCrtPage->GetPageIndex());
}

//��õ�ǰ��һҳ�������ģ���ǰҳ��ҳ���ڽṹ���pageIndex����
bool CPdfPicture::GetCrtPageContext(PPAGE_PDF_CONTEXT npPageContext)
{
	if (pdfCrtPage == NULL)
		return false;

	pdfCrtPage->GetPageContext(npPageContext);

	npPageContext->pageIndex = PAGEID_USER(npPageContext->pageIndex);

	return true;
}

//��������
//rMove4������0��ʾ�����ƶ��ˣ�1��ʾ�����ƶ�
bool CPdfPicture::SetScaleRatio(float nfScale, OUT RECT& rMove)
{
	bool lbRet = false;

	do
	{
		mViewCtl.SetUserRatio(nfScale);

		if (pdfImage == NULL)
			break;

		CalcMovalbe(rMove);

		lbRet = RenderPages();
		if (lbRet != false)
		{
			SvcDebugOutFmt("ClearEinkBuffer");
			EinkuiGetSystem()->ClearEinkBuffer();

			SvcDebugOutFmt("UpdateView");
			EinkuiGetSystem()->UpdateView();
		}
			

	} while (false);

	return lbRet;
}

//�ƶ�ҳ��
//niXOffset,niYOffset������ʾ�������ƶ���������ʾ�������ƶ�
//rMove4������0��ʾ�����ƶ��ˣ�1��ʾ�����ƶ�
bool CPdfPicture::MovePage(int niXOffset, int niYOffset, OUT RECT& rMove)
{
	bool lbRet = false;

	do
	{
		ED_POINT moveOffset;
		moveOffset.x = (int32Eink)niXOffset;
		moveOffset.y = (int32Eink)niYOffset;
		mViewCtl.Move(moveOffset);

		CalcMovalbe(rMove);

		SvcDebugOutFmt("ClearEinkBuffer");
		EinkuiGetSystem()->ClearEinkBuffer();

		SvcDebugOutFmt("UpdateView");
		EinkuiGetSystem()->UpdateView(true);
		lbRet = true;

	} while (false);

	return lbRet;

}

//�ƶ���ĳλ��
bool CPdfPicture::MovePageTo(int niX, int niY)
{
	bool lbRet = false;

	do
	{
		ED_POINT movePos;
		movePos.x = (int32Eink)niX;
		movePos.y = (int32Eink)niY;
		mViewCtl.MoveTo(movePos);

		SvcDebugOutFmt("ClearEinkBuffer");
		EinkuiGetSystem()->ClearEinkBuffer();

		SvcDebugOutFmt("UpdateView");
		EinkuiGetSystem()->UpdateView(true);
		lbRet = true;

	} while (false);

	return lbRet;
}

//��ȡ��ҳ��
ULONG CPdfPicture::GetPageCount(void)
{
	return mulPageCount;
}

//��ȡ�ĵ�����
int CPdfPicture::GetDocType(void)
{
	return miDocType;
}

//����ҳ���ȡҳ�����ָ��
IEdPage_ptr CPdfPicture::GetPageByPageNumber(int niPageNumber)
{
	IEdPage_ptr lpdfModifyPage = NULL;
	int liErrorCount = 0;

	do
	{
		if (pdfCrtPage != NULL && pdfCrtPage->GetPageIndex() == niPageNumber)
		{
			lpdfModifyPage = pdfCrtPage;
			break;
		}
		else if (pdfCrtPage2 != NULL && pdfCrtPage2->GetPageIndex() == niPageNumber)
		{
			lpdfModifyPage = pdfCrtPage2;
			break;
		}
		else
		{
			//��Ҫ����תҳ��
			GoToPage(niPageNumber + 1, true);
			PostMessageToParent(EEVT_PAGE_CHANGED, CExMessage::DataInvalid);
		}

	} while (liErrorCount++ < 5);

	return lpdfModifyPage;
}

//undo �� redo����
int CPdfPicture::RedoUndoProc(bool nbIsRedo)
{
	//ջ��С
	int liStackSize = 0;
	SvcDebugOutFmt("RedoUndoProc");
	try
	{
		
		CRedoUndoStrokeList* lpList = nbIsRedo == false ? mcRedoUndoManager.GetUndo(liStackSize) : mcRedoUndoManager.GetRedo(liStackSize);

		do
		{
			BREAK_ON_NULL(lpList);

			CRedoUndoStrokeList* lpRedoUndoStrokeList = new CRedoUndoStrokeList();

			for (int i = 0; i < lpList->Size(); i++)
			{
				CRedoUndoItem ldItem = lpList->GetEntry(i);
				if (ldItem.IsDelete == false)
				{
					//֮ǰ����Ӳ���
					SvcDebugOutFmt("RedoUndoProc remove annot");
					CRedoUndoItem ldDeleteItem;
					ldDeleteItem.IsDelete = true;
					ldDeleteItem.PageNumber = ldItem.PageNumber;
					ldDeleteItem.AnnotType = ldItem.AnnotType;

					//�жϲ����Ƿ��ڵ�ǰҳ������
					IEdPage_ptr lpdfModifyPage = GetPageByPageNumber(ldItem.PageNumber);
					if (lpdfModifyPage == NULL)
						continue;


					//���л�����
					SvcDebugOutFmt("RedoUndoProc remove annot GetAnnotBySignature");
					IEdAnnot_ptr lpAnnot = lpdfModifyPage->GetAnnotManager()->GetAnnotBySignature(ldItem.PenItemSignature);
					if(lpAnnot == NULL)
						continue;

					ldDeleteItem.PenByteDataSize = lpAnnot->SaveToAchive(NULL, 0);
					ldDeleteItem.PenByteData = new char[ldDeleteItem.PenByteDataSize];
					lpAnnot->SaveToAchive(ldDeleteItem.PenByteData, ldDeleteItem.PenByteDataSize);

					lpRedoUndoStrokeList->Insert(-1, ldDeleteItem);

					//ɾ��������ʼ�
					SvcDebugOutFmt("RedoUndoProc remove annot success");
					lpdfModifyPage->GetAnnotManager()->RemoveAnnot(lpAnnot);
					lpAnnot->Release();
				}
				else
				{
					//֮ǰ��ɾ������
					SvcDebugOutFmt("RedoUndoProc add annot");
					//�жϲ����Ƿ��ڵ�ǰҳ������
					IEdPage_ptr lpdfModifyPage = GetPageByPageNumber(ldItem.PageNumber);
					if (lpdfModifyPage == NULL)
						continue;

					//����ʼ�
					SvcDebugOutFmt("RedoUndoProc add annot LoadAnnotFromArchive");
					IEdAnnot_ptr lpNewAnnot = lpdfModifyPage->GetAnnotManager()->LoadAnnotFromArchive(ldItem.PenByteData, ldItem.PenByteDataSize);
					if (lpNewAnnot == NULL)
						continue;
					//����redo
					CRedoUndoItem ldAddItem;
					ldAddItem.IsDelete = false;
					ldAddItem.AnnotType = ldItem.AnnotType;
					ldAddItem.PageNumber = ldItem.PageNumber;
					ldAddItem.PenItemSignature = lpdfModifyPage->GetAnnotManager()->GetSignature(lpNewAnnot);
					lpNewAnnot->AddRefer();
					lpRedoUndoStrokeList->Insert(-1, ldAddItem);

					SvcDebugOutFmt("RedoUndoProc add annot success");
				}
			}

			if (lpRedoUndoStrokeList != NULL && lpRedoUndoStrokeList->Size() > 0)
			{
				//����redo
				if (nbIsRedo == false)
					mcRedoUndoManager.AddRedo(lpRedoUndoStrokeList);
				else
					mcRedoUndoManager.AddUndo(lpRedoUndoStrokeList);
			}
			else if (lpRedoUndoStrokeList != NULL)
			{
				SvcDebugOutFmt("RedoUndoProc delete");
				delete lpRedoUndoStrokeList;
			}
			

			//ɾ��������
			for (int i = 0; i < lpList->Size(); i++)
			{
				SvcDebugOutFmt("RedoUndoProc clear");
				if (lpList->GetEntry(i).PenByteData != NULL)
					delete lpList->GetEntry(i).PenByteData;
			}
			lpList->Clear();
			delete lpList;

			InkInputUpdate();

		} while (false);
	}
	catch (...)
	{
		SvcDebugOutFmt("RedoUndoProc try");
	}

	SetRedoUndoStatus();
	SvcDebugOutFmt("RedoUndoProc end");

	return liStackSize;
}


//��¼��
void CPdfPicture::SaveTouchPoint(PEI_TOUCHINPUT_POINT npPoint, ULONG nulPenMode, bool nbIsHand)
{
	ED_POINTF ldPos;

	do
	{

		//������
		if (mViewCtl.ViewToImageInit(npPoint->x, npPoint->y, ldPos) == false)
			break;

		if (pdfCrtPage2 == NULL)
		{
			//����ǵ�ҳ״̬
			mStrokePoints.Insert(-1, ldPos);
		}
		else
		{
			//�����˫ҳ
			ED_POINTF ldInPos;
			if (mViewCtl.IsInPage2(ldPos, ldInPos) == false)
			{
				//�ڵ�һҳ
				if (mStrokePoints2.Size() > 1 && mStrokePoints.Size() <=0)
				{
					//˵���ǿ�ҳ�����ĵ�һ���㣬�ǾͲ�һ�����ڵ�һҳ��ǰ��,���Ҳ�һ�����ڵڶ�ҳ�����
	/*				ED_POINTF ldPos = ldInPos;
					ldPos.x = mfMiddleLineX;*/

					EI_TOUCHINPUT_POINT ldTouchinputPoint;
					ldTouchinputPoint.FingerOrPen = npPoint->FingerOrPen;
					ldTouchinputPoint.Flags = EI_TOUCHEVENTF_UP;
					ldTouchinputPoint.PenButton = npPoint->PenButton;
					ldTouchinputPoint.x = (unsigned long)(mfMiddleLineX + 1.0f*GetRealRatio());
					
					ldTouchinputPoint.z = npPoint->z;

					//���ڶ�ҳ����up,����б�ʼ���y
					ED_POINTF ldLastPos = mStrokePoints2.GetEntry(mStrokePoints2.Size()-1);

					//ת����Ļ����
					mViewCtl.IsInPage2(ldLastPos, ldLastPos, true);
					mViewCtl.ImageToViewInit((int)ldLastPos.x, (int)ldLastPos.y, ldLastPos);
					float lfFlag = (ldLastPos.y - npPoint->y) / (ldLastPos.x - npPoint->x);

					ldTouchinputPoint.y = (unsigned long)((ldLastPos.x - ldTouchinputPoint.x) * lfFlag + ldLastPos.y);
					
					TouchUp(&ldTouchinputPoint, nulPenMode, nbIsHand);

					//����һҳ����down
					ldTouchinputPoint.x = (unsigned long)(mfMiddleLineX -1.0f*GetRealRatio());
					ldTouchinputPoint.Flags = EI_TOUCHEVENTF_DOWN;

					TouchDown(&ldTouchinputPoint, nulPenMode, nbIsHand);
				}

				mStrokePoints.Insert(-1, ldPos);
			}
			else
			{
				//�ڵڶ�ҳ
				if (mStrokePoints.Size() > 0 && mStrokePoints2.Size() <= 0)
				{
					//˵���ǿ�ҳ�����ĵ�һ���㣬�ǾͲ�һ�����ڵ�һҳ��ǰ��,���Ҳ�һ�����ڵڶ�ҳ�����
					/*ED_POINTF ldPos = ldInPos;
					ldPos.x = mfMiddleLineX;*/

					EI_TOUCHINPUT_POINT ldTouchinputPoint;
					ldTouchinputPoint.FingerOrPen = npPoint->FingerOrPen;
					ldTouchinputPoint.Flags = EI_TOUCHEVENTF_UP;
					ldTouchinputPoint.PenButton = npPoint->PenButton;
					ldTouchinputPoint.x = (unsigned long)(mfMiddleLineX - 1.0f*GetRealRatio());

					ldTouchinputPoint.z = npPoint->z;

					//����һҳ����up,����б�ʼ���y
					ED_POINTF ldLastPos = mStrokePoints.GetEntry(mStrokePoints.Size() - 1);

					//ת����Ļ����
					mViewCtl.ImageToViewInit((int)ldLastPos.x, (int)ldLastPos.y, ldLastPos);
					float lfFlag = (ldLastPos.y - npPoint->y) / (ldLastPos.x - npPoint->x);

					ldTouchinputPoint.y = (unsigned long)((ldTouchinputPoint.x - ldLastPos.x) * lfFlag + ldLastPos.y);
					
					TouchUp(&ldTouchinputPoint, nulPenMode, nbIsHand);

					//���ڶ�ҳ����down
					ldTouchinputPoint.x = (unsigned long)(mfMiddleLineX + 1.0f*GetRealRatio());
					ldTouchinputPoint.Flags = EI_TOUCHEVENTF_DOWN;

					TouchDown(&ldTouchinputPoint, nulPenMode, nbIsHand);
				}
				mStrokePoints2.Insert(-1, ldInPos);
				
			}
				

		}

	} while (false);
}

//�����¼�
void CPdfPicture::TouchDown(PEI_TOUCHINPUT_POINT npPoint, ULONG nulPenMode, bool nbIsHand)
{
	do 
	{
		SvcDebugOutFmt("touch down");
		mpIterator->KillTimer(PP_TIMER_ID_UPDATE);

		if (nulPenMode == PEN_MODE_PEN)
		{
			SvcDebugOutFmt("disable Paintboard");
			mpIterator->KillTimer(PP_TIMER_ID_ENABLE_FRAME);
			EinkuiGetSystem()->EnablePaintboard(true); //�ʼ�����ʱ����ˢ��
		}
			
		mStrokePoints.Clear();
		mStrokePoints2.Clear();

		SaveTouchPoint(npPoint,nulPenMode,nbIsHand);

		mpEarseRedoUndoStrokeList = NULL;

	} while (false);
}


void CPdfPicture::TouchMove(PEI_TOUCHINPUT_POINT npPoint, ULONG nulPenMode, bool nbIsHand)
{
	SaveTouchPoint(npPoint, nulPenMode, nbIsHand);

	if (nulPenMode == PEN_MODE_ERASER && 
		((mStrokePoints.Size() > 0 && (mStrokePoints.Size()%31)==0) || 
		 (mStrokePoints2.Size() > 0 && (mStrokePoints2.Size()%31)==0)))
	{
		//�������Ƥ��ÿ100���㴦��һ��
		//OutputDebugString(L"earsepro begin");
		EarsePro();
		//OutputDebugString(L"earsepro end");
	}
}

void  CPdfPicture::PenLeave(ULONG nulPenMode)
{
	SvcDebugOutFmt("pen leave");
	mpIterator->KillTimer(PP_TIMER_ID_UPDATE);
	mpIterator->SetTimer(PP_TIMER_ID_UPDATE, 1, 1200, NULL);
}

void CPdfPicture::TouchUp(PEI_TOUCHINPUT_POINT npPoint, ULONG nulPenMode, bool nbIsHand)
{
	CRedoUndoStrokeList* lpRedoUndoStrokeList = NULL;

	try
	{
		do
		{
			SvcDebugOutFmt("touch up");

			if (pdfCrtPage == NULL && pdfCrtPage2 == NULL)
				break;

			SaveTouchPoint(npPoint, nulPenMode, nbIsHand);

			if (nulPenMode == PEN_MODE_PEN)
			{
				//������
				
				if (mStrokePoints.Size() > 2 || mStrokePoints2.Size() > 2)
				{
					SvcDebugOutFmt("ClearRedo");
					mcRedoUndoManager.ClearRedo(); //ֻҪ���������ˣ������redo����

					lpRedoUndoStrokeList = new CRedoUndoStrokeList();
				}
				else
				{
					mStrokePoints.Clear();
					mStrokePoints2.Clear();

					if (nbIsHand != false)
					{
						//�������д��������Ҫ��ʾ/����������
						SvcDebugOutFmt("enable paintboard");
						EinkuiGetSystem()->EnablePaintboard(false);
					}
					break;
				}


				if (mStrokePoints.Size() > 0)
				{
					SvcDebugOutFmt("add ink annot page one");
					IEdAnnot_ptr lpUndoPtr = pdfCrtPage->GetAnnotManager()->AddInkAnnot(mStrokePoints.GetBuffer(), mStrokePoints.Size(), &mdPenColor, mfPenWidth);
					if (lpUndoPtr == NULL)
						break;

					//���undo����
					CRedoUndoItem ldRedoUndoItem;
					ldRedoUndoItem.AnnotType = RUM_TYPE_INK;
					ldRedoUndoItem.IsDelete = false;
					ldRedoUndoItem.PageNumber = pdfCrtPage->GetPageIndex();
					ldRedoUndoItem.PenItemSignature = pdfCrtPage->GetAnnotManager()->GetSignature(lpUndoPtr);
					lpUndoPtr->AddRefer();
					lpRedoUndoStrokeList->Insert(-1, ldRedoUndoItem);

					SvcDebugOutFmt("add ink annot page one success");
					mStrokePoints.Clear();
				}

				if (mStrokePoints2.Size() > 0 && pdfCrtPage2 != NULL)
				{
					SvcDebugOutFmt("add ink annot page two");
					IEdAnnot_ptr lpUndoPtr = pdfCrtPage2->GetAnnotManager()->AddInkAnnot(mStrokePoints2.GetBuffer(), mStrokePoints2.Size(), &mdPenColor, mfPenWidth);
					if (lpUndoPtr == NULL)
						break;

					//���undo����
					CRedoUndoItem ldRedoUndoItem;
					ldRedoUndoItem.AnnotType = RUM_TYPE_INK;
					ldRedoUndoItem.IsDelete = false;
					ldRedoUndoItem.PageNumber = pdfCrtPage2->GetPageIndex();
					ldRedoUndoItem.PenItemSignature = pdfCrtPage2->GetAnnotManager()->GetSignature(lpUndoPtr);
					lpUndoPtr->AddRefer();
					lpRedoUndoStrokeList->Insert(-1, ldRedoUndoItem);

					SvcDebugOutFmt("add ink annot page two success");
					mStrokePoints2.Clear();
				}

				if (lpRedoUndoStrokeList != NULL && lpRedoUndoStrokeList->Size() > 0) //�������˫ҳģʽ�º���һ�ʻ�������ҳ�棬��һ�β���
				{
					SvcDebugOutFmt("AddUndo");
					mcRedoUndoManager.AddUndo(lpRedoUndoStrokeList);

					mbIsPageChange = true;

					if (mbIsModify == false)
					{
						//�༭�����ļ���
						mbIsModify = true;
						SvcDebugOutFmt("EEVT_FILE_MODIFY");
						PostMessageToParent(EEVT_FILE_MODIFY, CExMessage::DataInvalid);
					}
				}

				if (nbIsHand != false)
				{
					//����Ǳ�д״̬����Ҫ�ȴ����뿪���Ż�
					SvcDebugOutFmt("hand touch up timeer");
					mpIterator->KillTimer(PP_TIMER_ID_UPDATE);
					mpIterator->SetTimer(PP_TIMER_ID_UPDATE, 1, 1200, NULL);
				}

			}
			else if (nulPenMode == PEN_MODE_ERASER)
			{
				//��Ƥ
				EarsePro();

				if (mpEarseRedoUndoStrokeList != NULL) //�������˫ҳģʽ�º���һ�ʻ�������ҳ�棬��һ�β���
				{
					if (mpEarseRedoUndoStrokeList->Size() > 0)
					{
						mcRedoUndoManager.AddUndo(mpEarseRedoUndoStrokeList);
						mpEarseRedoUndoStrokeList = NULL; //�Ѿ�����

						if (mbIsModify == false)
						{
							//�༭�����ļ���
							mbIsModify = true;
							SvcDebugOutFmt("EEVT_FILE_MODIFY");
							PostMessageToParent(EEVT_FILE_MODIFY, CExMessage::DataInvalid);
						}
					}
					else
					{
						//���û�в�������
						SvcDebugOutFmt("touch up not earse");
						delete mpEarseRedoUndoStrokeList;
						mpEarseRedoUndoStrokeList = NULL;
					}

				}

				mStrokePoints.Clear();
				mStrokePoints2.Clear();
			}

			SetRedoUndoStatus();

		} while (false);
	}
	catch (...)
	{
		SvcDebugOutFmt("touch up try");
	}

	if (lpRedoUndoStrokeList != NULL && lpRedoUndoStrokeList->Size() <= 0)
		delete lpRedoUndoStrokeList; //û������ink����

	SvcDebugOutFmt("touch up end");
	mpIterator->KillTimer(PP_TIMER_ID_ENABLE_FRAME);
	mpIterator->SetTimer(PP_TIMER_ID_ENABLE_FRAME, 1, 1200, NULL);
}

//��Ƥ����
void CPdfPicture::EarsePro(void)
{
	try
	{
		SvcDebugOutFmt("Earse begin");
		if (mStrokePoints.Size() > 2 || mStrokePoints2.Size() > 2)
		{
			if (mpEarseRedoUndoStrokeList == NULL)
				mpEarseRedoUndoStrokeList = new CRedoUndoStrokeList();
		}
		else
		{
			SvcDebugOutFmt("not Earse 1");
			return;
		}

		int liEarseCount = mpEarseRedoUndoStrokeList->Size();

		if (mStrokePoints.Size() > 0)
		{
			//������Χ��ǰһҳ
			SvcDebugOutFmt("Earse one page");
			IEdAnnot_ptr ldAnnotList[500];
			ZeroMemory(ldAnnotList, sizeof(IEdAnnot_ptr) * 500);
			int32Eink liCount = pdfCrtPage->GetAnnotManager()->DetectInkAnnot(mStrokePoints.GetBuffer(), mStrokePoints.Size(), ldAnnotList, 500);

			for (int i = 0; i < liCount; i++)
			{
				//���л�����
				if(ldAnnotList[i] == NULL)
					continue;

				SvcDebugOutFmt("Earse one page annot");
				CRedoUndoItem ldDeleteItem;
				ldDeleteItem.IsDelete = true;
				ldDeleteItem.AnnotType = RUM_TYPE_INK;
				ldDeleteItem.PageNumber = pdfCrtPage->GetPageIndex();
				ldDeleteItem.PenByteDataSize = ldAnnotList[i]->SaveToAchive(NULL, 0);
				ldDeleteItem.PenByteData = new char[ldDeleteItem.PenByteDataSize];
				ldAnnotList[i]->SaveToAchive(ldDeleteItem.PenByteData, ldDeleteItem.PenByteDataSize);
				mpEarseRedoUndoStrokeList->Insert(-1, ldDeleteItem);

				SvcDebugOutFmt("Earse one page annot RemoveAnnot");
				pdfCrtPage->GetAnnotManager()->RemoveAnnot(ldAnnotList[i]);
				ldAnnotList[i]->Release();
			}

			//�������һ����
			ED_POINTF ldLast = mStrokePoints.GetEntry(mStrokePoints.Size() - 1);
			mStrokePoints.Clear();
			mStrokePoints.Insert(0, ldLast);
		}

		if (mStrokePoints2.Size() > 0 && pdfCrtPage2 != NULL)
		{
			//������Χ�Ǻ�һҳ
			SvcDebugOutFmt("Earse two page annot");
			IEdAnnot_ptr ldAnnotList[500];
			ZeroMemory(ldAnnotList, sizeof(IEdAnnot_ptr) * 500);

			int32Eink liCount = pdfCrtPage2->GetAnnotManager()->DetectInkAnnot(mStrokePoints2.GetBuffer(), mStrokePoints2.Size(), ldAnnotList, 500);

			for (int i = 0; i < liCount; i++)
			{
				//���л�����
				if(ldAnnotList[i] == NULL)
					break;

				SvcDebugOutFmt("Earse two page annot");
				CRedoUndoItem ldDeleteItem;
				ldDeleteItem.AnnotType = RUM_TYPE_INK;
				ldDeleteItem.IsDelete = true;
				ldDeleteItem.PageNumber = pdfCrtPage2->GetPageIndex();
				ldDeleteItem.PenByteDataSize = ldAnnotList[i]->SaveToAchive(NULL, 0);
				ldDeleteItem.PenByteData = new char[ldDeleteItem.PenByteDataSize];
				ldAnnotList[i]->SaveToAchive(ldDeleteItem.PenByteData, ldDeleteItem.PenByteDataSize);
				mpEarseRedoUndoStrokeList->Insert(-1, ldDeleteItem);

				SvcDebugOutFmt("Earse two page annot removeannot");
				pdfCrtPage2->GetAnnotManager()->RemoveAnnot(ldAnnotList[i]);
				ldAnnotList[i]->Release();
			}

			//�������һ����
			ED_POINTF ldLast = mStrokePoints2.GetEntry(mStrokePoints2.Size() - 1);
			mStrokePoints2.Clear();
			mStrokePoints2.Insert(0, ldLast);
		}

		if (liEarseCount != mpEarseRedoUndoStrokeList->Size())
		{
			//OutputDebugString(L"xiang pi begin\r\n");
			InkInputUpdate();
			//OutputDebugString(L"xiang pi end\r\n");
			SvcDebugOutFmt("clear redo");

			mcRedoUndoManager.ClearRedo(); //����������ˣ������redo����
		}
	}
	catch (...)
	{
	}
}

bool32 CPdfPicture::GetThumbanilsPath(wchar_t* npszPathBuffer, int niLen)
{
	if (pdfDoc != NULL)
	{
		wcscpy_s(npszPathBuffer, niLen, mCachePath.GetPathName());
		return true;
	}
	else
		return false;
}

void CPdfPicture::RefreshThumbnail()
{
	RefreshThumbnail(pdfCrtPage, pdfCrtPage2);
}

//����PDF����ͼ,���ҳ���б仯�Ļ�
void CPdfPicture::RefreshThumbnail(IEdPage_ptr npPage1, IEdPage_ptr npPage2)
{
	if (GetDocType() == DOC_TYPE_PDF && mbIsPageChange != false)
	{
		wchar_t lszFilePath[MAX_PATH] = { 0 };

		if (npPage1 != NULL)
		{
			pdfDoc->GetThumbnailPathName(npPage1->GetPageIndex(), lszFilePath, NULL);
			if (lszFilePath[0] != UNICODE_NULL)
				DeleteFile(lszFilePath);

		}

		lszFilePath[0] = UNICODE_NULL;
		if (npPage2 != NULL)
		{
			pdfDoc->GetThumbnailPathName(npPage2->GetPageIndex(), lszFilePath, NULL);
			if (lszFilePath[0] != UNICODE_NULL)
				DeleteFile(lszFilePath);
		}

		mbIsPageChange = false;
	}
}

//��ȡ��ǰҳ���ע����
int CPdfPicture::GetCurrentPageInkCount(void)
{
	int liRet = 0;

	try
	{
		do
		{
			if (GetDocType() != DOC_TYPE_PDF)
				break; //ֻ��PDF�б�ע

			if (pdfCrtPage != NULL)
				liRet = pdfCrtPage->GetAnnotManager()->GetAllAnnot(NULL, 0);
			if (pdfCrtPage2 != NULL)
				liRet = pdfCrtPage2->GetAnnotManager()->GetAllAnnot(NULL, 0) + liRet;

		} while (false);
	}
	catch (...)
	{
	}
	

	return liRet;
}

//��յ�ǰҳ�����б�ע
bool CPdfPicture::ClearCurrentPageAllInk(void)
{
	bool lbRet = false;

	SvcDebugOutFmt("ClearCurrentPageAllInk");
	try
	{
		do
		{
			CRedoUndoStrokeList* lpRedoUndoStrokeList = NULL;

			if (pdfCrtPage != NULL)
			{
				int liCount = pdfCrtPage->GetAnnotManager()->GetAllAnnot(NULL, 0);
				if (liCount > 0)
				{
					if (lpRedoUndoStrokeList == NULL)
						lpRedoUndoStrokeList = new CRedoUndoStrokeList();

					IEdAnnot_ptr* lpList = new IEdAnnot_ptr[liCount];
					ZeroMemory(lpList, sizeof(IEdAnnot_ptr) * liCount);

					liCount = pdfCrtPage->GetAnnotManager()->GetAllAnnot(lpList, liCount);
					for (int i = 0; i < liCount; i++)
					{
						if(lpList[i] == NULL)
							continue;

						//���л�����
						CRedoUndoItem ldDeleteItem;
						ldDeleteItem.AnnotType = lpList[i]->GetType();
						ldDeleteItem.IsDelete = true;
						ldDeleteItem.PageNumber = pdfCrtPage->GetPageIndex();
						ldDeleteItem.PenByteDataSize = lpList[i]->SaveToAchive(NULL, 0);
						ldDeleteItem.PenByteData = new char[ldDeleteItem.PenByteDataSize];
						lpList[i]->SaveToAchive(ldDeleteItem.PenByteData, ldDeleteItem.PenByteDataSize);
						lpRedoUndoStrokeList->Insert(-1, ldDeleteItem);

						pdfCrtPage->GetAnnotManager()->RemoveAnnot(lpList[i]);
						lpList[i]->Release();
					}


					if (lpList != NULL)
						delete[] lpList;
				}
			}

			if (pdfCrtPage2 != NULL)
			{
				int liCount = pdfCrtPage2->GetAnnotManager()->GetAllAnnot(NULL, 0);
				if (liCount > 0)
				{
					if (lpRedoUndoStrokeList == NULL)
						lpRedoUndoStrokeList = new CRedoUndoStrokeList();

					IEdAnnot_ptr* lpList = new IEdAnnot_ptr[liCount];
					ZeroMemory(lpList, sizeof(IEdAnnot_ptr) * liCount);

					liCount = pdfCrtPage2->GetAnnotManager()->GetAllAnnot(lpList, liCount);
					for (int i = 0; i < liCount; i++)
					{
						//���л�����
						if(lpList[i] == NULL)
							continue;

						CRedoUndoItem ldDeleteItem;
						ldDeleteItem.IsDelete = true;
						ldDeleteItem.AnnotType = lpList[i]->GetType();
						ldDeleteItem.PageNumber = pdfCrtPage2->GetPageIndex();
						ldDeleteItem.PenByteDataSize = lpList[i]->SaveToAchive(NULL, 0);
						ldDeleteItem.PenByteData = new char[ldDeleteItem.PenByteDataSize];
						lpList[i]->SaveToAchive(ldDeleteItem.PenByteData, ldDeleteItem.PenByteDataSize);
						lpRedoUndoStrokeList->Insert(-1, ldDeleteItem);

						pdfCrtPage2->GetAnnotManager()->RemoveAnnot(lpList[i]);
						lpList[i]->Release();
					}


					if (lpList != NULL)
						delete[] lpList;
				}
			}

			if (lpRedoUndoStrokeList != NULL && lpRedoUndoStrokeList->Size() > 0)
			{
				mcRedoUndoManager.AddUndo(lpRedoUndoStrokeList);
				mcRedoUndoManager.ClearRedo(); //ֻҪ���������ˣ������redo����

				SetRedoUndoStatus();
			}
			else if (lpRedoUndoStrokeList != NULL)
				delete lpRedoUndoStrokeList;

			bool lbEnable = false;
			SendMessageToParent(EEVT_PARTIAL_ENABLE, lbEnable, NULL, 0);
			InkInputUpdate();


			lbRet = true;

		} while (false);
	}
	catch (...)
	{
		SvcDebugOutFmt("ClearCurrentPageAllInk try");
	}
	
	SvcDebugOutFmt("ClearCurrentPageAllInk end");

	return lbRet;
}

//����redo undo��ť״̬
void CPdfPicture::SetRedoUndoStatus(void)
{
	SvcDebugOutFmt("SetRedoUndoStatus");
	PAGE_STATUS ldStatus;
	ldStatus.UndoCount = mcRedoUndoManager.GetUndoCount();
	ldStatus.RedoCount = mcRedoUndoManager.GetRedoCount();
	ldStatus.InkCount = GetCurrentPageInkCount();
	//SendMessageToParent(EEVT_UPDATE_PAGE_STATUS, ldStatus);
	CExMessage::SendMessage(mpIterator->GetParent(), mpIterator, EEVT_UPDATE_PAGE_STATUS, ldStatus);
}

//�����߿�
void CPdfPicture::SetPenWidth(float nfPenWidth)
{
	mfPenWidth = nfPenWidth;
}

//������ɫ
void CPdfPicture::SetPenColor(ULONG nulPenColor)
{
	if (nulPenColor == PEN_COLOR_RED)
	{
		mdPenColor.r = 219;
		mdPenColor.g = 67;
		mdPenColor.b = 0;
	}
	else if (nulPenColor == PEN_COLOR_BLACK)
	{
		mdPenColor.r = 0;
		mdPenColor.g = 0;
		mdPenColor.b = 0;
	}
	else if (nulPenColor == PEN_COLOR_BLUE)
	{
		mdPenColor.r = 90;
		mdPenColor.g = 167;
		mdPenColor.b = 255;
	}
}


bool CPdfPicture::SetFontSize(int fontSize)
{
	mFontSize = fontSize;
	
	if (pdfDoc != NULL)
	{
		pdfDoc->SetFont(L"΢���ź�", mFontSize);

		PAGE_PDF_CONTEXT pageConText = { 0,0,0 };

		if (pdfCrtPage != NULL)
			pdfCrtPage->GetPageContext(&pageConText);

		auto newPage = pdfDoc->Rearrange(pdfCrtPage);

		mulPageCount = pdfDoc->GetPageCount();

		GoToPage(newPage, true);
	}

	return true;
}

//��Ļ������ת
void CPdfPicture::SetRotation(ULONG nulRotation)
{
	do 
	{
		if (nulRotation == GIR_NONE || nulRotation == GIR_180)
		{
			landScope = true;
		}
		else if (nulRotation == GIR_90 || nulRotation == GIR_270)
		{
			landScope = false;
		}

		CMM_SAFE_RELEASE(mpSelectStextList);
		CMM_SAFE_RELEASE(mpSelectAnnotList);

	} while (false);

}

void CPdfPicture::EnableDuopageView(bool nbEnable)
{
	bool32 changed = (duopageMode == false && nbEnable != false || duopageMode != false && nbEnable == false);

	duopageMode = nbEnable;

	if (changed == false)
		return;

	if (pdfCrtPage != NULL)
	{
		pdfCrtPage->AddRefer();
		GoToPage(pdfCrtPage, true);
	}

	CMM_SAFE_RELEASE(mpSelectStextList);
	CMM_SAFE_RELEASE(mpSelectAnnotList);
}

//�����ַ�����������
bool CPdfPicture::CopyToClipboard(const wchar_t* npszText)
{
	bool lbRet = false;

	do 
	{
		BREAK_ON_NULL(npszText);
		if(npszText[0] == UNICODE_NULL)
			break;

		if (OpenClipboard(NULL) == FALSE)
			break;
		EmptyClipboard();

		//SetClipboardData(CF_TEXT, npszText);
		HGLOBAL clipbuffer;
		wchar_t *buffer;
		clipbuffer = ::GlobalAlloc(GMEM_DDESHARE, (wcslen(npszText) + 1)*2);
		buffer = (wchar_t *)::GlobalLock(clipbuffer);
		wcscpy_s(buffer, wcslen(npszText) + 1, npszText);
		::GlobalUnlock(clipbuffer);
		::SetClipboardData(CF_UNICODETEXT, clipbuffer);

		CloseClipboard();

		lbRet = true;

	} while (false);

	return lbRet;
}

bool CPdfPicture::CopyToClipboard(const D2D1_RECT_F& rRegion)
{
	bin_ptr bufToCopy = NULL;
	D2D1_RECT_F intersectRect;
	bool resultOK = false;
	HBITMAP bitmapHandle = NULL;

	do 
	{
		if (mpElBitmap == NULL || CExRect::Intersect(rRegion,mRecentDst,intersectRect) == false)
			break;

		//���ܳ���ԭͼ��С
		if (mpElBitmap->GetWidth() < (intersectRect.right- intersectRect.left))
			intersectRect.right = mpElBitmap->GetWidth() + intersectRect.left;
		if (mpElBitmap->GetHeight() < (intersectRect.bottom- intersectRect.top))
			intersectRect.bottom = mpElBitmap->GetHeight() + intersectRect.top;

		int32 imageWidth = CExFloat::ToLong(ED_RECT_WIDTH(intersectRect));
		int32 imageHeight = CExFloat::ToLong(ED_RECT_HEIGHT(intersectRect));
		if (imageWidth == 0 || imageHeight == 0)
			break;

		bufToCopy = (bin_ptr)HeapAlloc(GetProcessHeap(),0,imageWidth*imageHeight*4);
		if (bufToCopy == NULL)
			break;

		intersectRect.left = CExFloat::Floor(intersectRect.left - mRecentDst.left + mRecentSrc.left);
		intersectRect.right = intersectRect.left + (float32)imageWidth;
		intersectRect.top = CExFloat::Floor(intersectRect.top - mRecentDst.top + mRecentSrc.top);
		intersectRect.bottom = intersectRect.top + (float32)imageHeight;

		if (mpElBitmap->GetBitmapBuffer(intersectRect,bufToCopy)==false)
			break;

		bitmapHandle = CreateCompatibleBitmap(GetDC(NULL), imageWidth,imageHeight);
		if(bitmapHandle == NULL)
			break;

		if(SetBitmapBits(bitmapHandle, imageWidth*imageHeight * 4,bufToCopy)<=0)
			break;

		if(OpenClipboard(NULL)==FALSE)
			break;
		EmptyClipboard();

		SetClipboardData(CF_BITMAP,bitmapHandle);

		CloseClipboard();

		resultOK = true;
	} while (false);

	if (bitmapHandle != NULL)
		DeleteObject(bitmapHandle);

	if(bufToCopy != NULL)
		HeapFree(GetProcessHeap(), 0, bufToCopy);


	return resultOK;
}

float CPdfPicture::GetFatRatio(void)
{
	return mViewCtl.GetFatRatio() / mViewCtl.GetBaseRatio();
}

//֪ͨԪ�ء���ʾ/���ء������ı�
ERESULT CPdfPicture::OnElementShow(bool nbIsShow)
{
	//EiSetHomebarStatus(nbIsShow == false ? GI_HOMEBAR_SHOW : GI_HOMEBAR_HIDE);

	return ERESULT_SUCCESS;
}

bool32 CPdfPicture::GetPageSize(IEdPage_ptr pagePtr, ED_SIZE& sizeInit)
{
	IEdPage_ptr pageGot = NULL;
	ED_RECTF box = { 0,0,1,1 };

	if (pagePtr->GetMediaBox(&box) == false)
		return false;

	sizeInit.x = CExFloat::ToLong(ED_RECT_WIDTH(box));
	sizeInit.y = CExFloat::ToLong(ED_RECT_HEIGHT(box));

	return true;
}

bin_ptr CPdfPicture::PrepareCanvas(int32 totalBytes)
{
	if (totalBytes > pageCanvasSize)
	{
		if (pageCanvas != NULL)
		{
			HeapFree(GetProcessHeap(), 0, pageCanvas);
			pageCanvas = NULL;
			pageCanvasSize = 0;
		}

		pageCanvas = (bin_ptr)HeapAlloc(GetProcessHeap(), 0, totalBytes);
		if (pageCanvas != NULL)
			pageCanvasSize = totalBytes;
	}

	return pageCanvas;
}

//��ȡҳ��Ŵ����
float32 CPdfPicture::GetRealRatio()
{
	return mViewCtl.GetRealRatio();
}

float32 CPdfPicture::GetBaseRatio()
{
	return mViewCtl.GetBaseRatio();
}

bool CPdfPicture::RenderPages(void)
{
	bool lbRet = false;
	bin_ptr imageBuffer = NULL;

	try
	{
		do
		{
			SvcDebugOutFmt("render pages");
			if (pdfCrtPage == NULL)
				break;

			CMM_SAFE_RELEASE(pdfImage);
			CMM_SAFE_RELEASE(pdfImage2);
			CMM_SAFE_RELEASE(mpElBitmap);

			pdfImage = pdfCrtPage->Render(mViewCtl.GetRealRatio(), true);
			if (pdfImage == NULL)
				break;

			if (pdfCrtPage2 != NULL)
			{
				pdfImage2 = pdfCrtPage2->Render(mViewCtl.GetRealRatio(), true);
				if (pdfImage2 == NULL)
					break;

				imageSize.x = (pdfImage->GetWidth() <= pdfImage2->GetWidth() ? pdfImage->GetWidth() : pdfImage2->GetWidth());
				imageSize.y = (pdfImage->GetHeight() <= pdfImage2->GetHeight() ? pdfImage->GetHeight() : pdfImage2->GetHeight());

				imageBuffer = PrepareCanvas((imageSize.x * 2 + pageGap) * 4 * imageSize.y);
				if (imageBuffer == NULL)
					break;

				bin_ptr src1, src2;
				register bin_ptr dst, dstEnd;
				int32 stridemin = imageSize.x * 4;
				int32 stride1 = pdfImage->GetWidth() * 4;
				int32 stride2 = pdfImage2->GetWidth() * 4;
				dst = imageBuffer;
				src1 = pdfImage->GetBuffer();
				src2 = pdfImage2->GetBuffer();

				if (mIsTxtDoc != false)
				{
					for (int32 i = 0; i < imageSize.y; i++)
					{
						memcpy_s(dst, stridemin, src1, stridemin);
						dst += stridemin;
						if (pageGap >= 1.0f)
						{
							//				RtlZeroMemory(dst, pageGap * 4);
							memset(dst, 0xFF, pageGap * 4);
							dst += pageGap * 4;
						}
						memcpy_s(dst, stridemin, src2, stridemin);
						dst += stridemin;
						src1 += stride1;
						src2 += stride2;
					}
				}
				else
				{
					for (int32 i = 0; i < imageSize.y; i++)
					{
						register unsigned char* src = src1;
						dstEnd = dst + stridemin;
						while (dst < dstEnd)
						{
							*dst++ = pixelGradeImprovement2[*src++];
						}

						if (pageGap >= 1.0f)
						{
							//				RtlZeroMemory(dst, pageGap * 4);
							memset(dst, 0xFF, pageGap * 4);
							dst += pageGap * 4;
						}

						src = src2;
						dstEnd = dst + stridemin;
						while (dst < dstEnd)
						{
							*dst++ = pixelGradeImprovement2[*src++];
						}

						src1 += stride1;
						src2 += stride2;
					}
				}

				imageSize.x = imageSize.x * 2 + pageGap;
			}
			else
			{
				auto src = pdfImage->GetBuffer();
				imageSize.x = pdfImage->GetWidth();
				imageSize.y = pdfImage->GetHeight();

				if (mIsTxtDoc != false)
				{
					imageBuffer = src;
				}
				else
				{
					imageBuffer = PrepareCanvas(imageSize.x*imageSize.y * 4);

					LONG totalBytes = imageSize.x * imageSize.y * 4;
					register unsigned char* lpDst = imageBuffer;
					register unsigned char* lpSrc = src;
					register unsigned char* lpSrcEnd = src + totalBytes;

					while (lpSrc < lpSrcEnd)
					{
						*lpDst++ = pixelGradeImprovement2[*lpSrc++];
					}
				}

				//////////////////////////////////////////////////////////////////////////
				// ͼ����ǿ�㷨��������ʾͼƬ����ʮ����Ч
				//LONG stride = imageSize.x * 4;

				//RtlCopyMemory(imageBuffer, src, stride);
				//RtlCopyMemory(imageBuffer + stride*(imageSize.y - 1), src + stride*(imageSize.y - 1), stride);

				//for (LONG i = 1; i < imageSize.y - 1; i++)
				//{
				//	register unsigned char* lpDst = imageBuffer + stride*i;
				//	register unsigned char* lpSrc = src + stride*i;
				//	register short EdgeVaule;
				//	LONG j;

				//	*(ULONG*)lpDst = *(ULONG*)lpSrc;
				//	lpDst += 4;
				//	lpSrc += 4;

				//	for (j = 0; j < imageSize.x - 2; j++)
				//	{
				//		EdgeVaule = (*lpSrc) * 5 - ((*(lpSrc - stride)) + (*(lpSrc - 4)) + (*(lpSrc + 4)) + (*(lpSrc + stride)));
				//		if (EdgeVaule > 255)
				//			*lpDst = 255;
				//		else if (EdgeVaule < 0)
				//			*lpDst = 0;
				//		else
				//			*lpDst = (unsigned char)EdgeVaule;

				//		lpSrc++;
				//		lpDst++;

				//		EdgeVaule = (*lpSrc) * 5 - ((*(lpSrc - stride)) + (*(lpSrc - 4)) + (*(lpSrc + 4)) + (*(lpSrc + stride)));
				//		if (EdgeVaule > 255)
				//			*lpDst = 255;
				//		else if (EdgeVaule < 0)
				//			*lpDst = 0;
				//		else
				//			*lpDst = (unsigned char)EdgeVaule;

				//		lpSrc++;
				//		lpDst++;

				//		EdgeVaule = (*lpSrc) * 5 - ((*(lpSrc - stride)) + (*(lpSrc - 4)) + (*(lpSrc + 4)) + (*(lpSrc + stride)));
				//		if (EdgeVaule > 255)
				//			*lpDst = 255;
				//		else if (EdgeVaule < 0)
				//			*lpDst = 0;
				//		else
				//			*lpDst = (unsigned char)EdgeVaule;

				//		lpSrc++;
				//		lpDst++;

				//		EdgeVaule = (*lpSrc) * 5 - ((*(lpSrc - stride)) + (*(lpSrc - 4)) + (*(lpSrc + 4)) + (*(lpSrc + stride)));
				//		if (EdgeVaule > 255)
				//			*lpDst = 255;
				//		else if (EdgeVaule < 0)
				//			*lpDst = 0;
				//		else
				//			*lpDst = (unsigned char)EdgeVaule;

				//		lpSrc++;
				//		lpDst++;
				//	}

				//	*(ULONG*)lpDst = *(ULONG*)lpSrc;
				//}
			}

			mViewCtl.AdjustImageRealSize(imageSize);
			RECT ldRect; //�ǳ���Ҫ������ɾ��
			CalcMovalbe(ldRect);

			mpElBitmap = EinkuiGetSystem()->GetAllocator()->CreateBitmapFromMemory(imageSize.x, imageSize.y, 4, imageSize.x * 4, imageBuffer);
			if (mpElBitmap == NULL)
				break;

			lbRet = true;

		} while (false);
	}
	catch (...)
	{
	}
	
	return lbRet;
}

void CPdfPicture::CalcMovalbe(OUT RECT& rMove)
{
	ED_RECT dstRect;
	ED_RECT srcRect;
	UCHAR edgeImpact;
	ED_SIZEF ldpageSize;

	if (mViewCtl.GetViewMapArea(dstRect, srcRect, ldpageSize,&edgeImpact) == false)
	{
		rMove.left = rMove.right = rMove.top = rMove.bottom = 0;
	}
	else
	{
		rMove.left = ((edgeImpact & 1) > 0);
		rMove.right = ((edgeImpact & 2) > 0);
		rMove.top = ((edgeImpact & 4) > 0);
		rMove.bottom = ((edgeImpact & 8) > 0);

		mfMiddleLineX = ldpageSize.x / 2.0f - srcRect.left;
		if (dstRect.left > 0.0f)
			mfMiddleLineX = (dstRect.left + dstRect.right) / 2.0f;
	}
}

ED_RECTF CPdfPicture::CalImageContentRect()
{
	auto image = pdfCrtPage->Render(1.0f, true);
	auto buff = image->GetBuffer();
	int32 height = image->GetHeight();
	int32 width = image->GetWidth();
	int32 stride = width * 4;

	int left = 0;
	int right = width - 1;
	int top = 0;
	int bottom = height - 1;

	size_t idx1 = 0;
	size_t idx2 = 0;
	bool bBlank = true;

	auto CheckPixel = [&buff](const int& p1, const int& p2) {
		if (buff[p1] == buff[p2] &&
			buff[p1 + 1] == buff[p2 + 1] &&
			buff[p1 + 2] == buff[p2 + 2] &&
			buff[p1 + 3] == buff[p2 + 3])
		{
			return true;
		}
		return false;
	};

	for (; left < width - 1; left += 2)
	{
		bBlank = true;
		for (int h = 0; h < height; ++h)
		{
			idx1 = stride * h + left * 4;
			idx2 = stride * h + (left + 1) * 4;
			if (!CheckPixel(idx1, idx2))
			{
				bBlank = false;
				break;
			}
		}
		if (!bBlank)
			break;
	}
	for (; right > left + 1; right -= 2)
	{
		bBlank = true;
		for (int h = 0; h < height; ++h)
		{
			idx1 = stride * h + right * 4;
			idx2 = stride * h + (right - 1) * 4;
			if (!CheckPixel(idx1, idx2))
			{
				bBlank = false;
				break;
			}
		}
		if (!bBlank)
			break;
	}
	for (; top < bottom - 1; top += 2)
	{
		bBlank = true;
		for (int h = left; h < right; ++h)
		{
			idx1 = stride * top + h * 4;
			idx2 = stride * (top + 1) + h * 4;
			if (!CheckPixel(idx1, idx2))
			{
				bBlank = false;
				break;
			}
		}
		if (!bBlank)
			break;
	}
	for (; bottom > top; bottom -= 2)
	{
		bBlank = true;
		for (int h = left; h < right; ++h)
		{
			idx1 = stride * bottom + h * 4;
			idx2 = stride * (bottom - 1) + h * 4;
			if (!CheckPixel(idx1, idx2))
			{
				bBlank = false;
				break;
			}
		}
		if (!bBlank)
			break;
	}
	CMM_SAFE_RELEASE(image);

	return ED_RECTF{ (float32)left , (float32)right , (float32)top, (float32)bottom };
}

//����
ERESULT CPdfPicture::OnPaint(IEinkuiPaintBoard* npPaintBoard)
{
	ERESULT leResult = ERESULT_UNSUCCESSFUL;
	ED_RECT dstRect;
	ED_RECT srcRect;
	UCHAR impact;
	ED_SIZEF ldpageSize;

	do
	{
		BREAK_ON_NULL(npPaintBoard);

		leResult = CXuiElement::OnPaint(npPaintBoard);
		if (ERESULT_FAILED(leResult))
			break;


		if (mpElBitmap != NULL && mViewCtl.GetViewMapArea(dstRect, srcRect,ldpageSize, &impact) != false)
		{
			if (dstRect.left != mdFwLineRect.left || dstRect.top != mdFwLineRect.top || dstRect.right != mdFwLineRect.right || dstRect.bottom != mdFwLineRect.bottom)
			{
				mdFwLineRect = dstRect;
				PostMessageToParent(EEVT_SET_FW_LINE_RECT, mdFwLineRect);
			}

			mRecentDst.left = (float32)dstRect.left;
			mRecentDst.right = (float32)dstRect.right;
			mRecentDst.top = (float32)dstRect.top + mTitleBarHeight / 2.0f;// ����һ��
			mRecentDst.bottom = (float32)dstRect.bottom + mTitleBarHeight / 2.0f;// ����һ��
			mRecentSrc.left = (float32)srcRect.left;
			mRecentSrc.right = (float32)srcRect.right;
			mRecentSrc.top = (float32)srcRect.top;
			mRecentSrc.bottom = (float32)srcRect.bottom;

			npPaintBoard->DrawBitmap(mRecentDst, mRecentSrc, mpElBitmap, ESPB_DRAWBMP_LINEAR);

			// ���潨����ˢ
			if (mpLineBrush == NULL)
			{
				mpLineBrush = EinkuiGetSystem()->CreateBrush(XuiSolidBrush, D2D1::ColorF(D2D1::ColorF::Black, 2.0f));
				if(mpLineBrush == NULL)
					break;
			}

			// ���潨����ˢ
			if (mpHigilightBrush == NULL)
			{
				mpHigilightBrush = EinkuiGetSystem()->CreateBrush(XuiSolidBrush, D2D1::ColorF(D2D1::ColorF::Gray, 0.5f));
				if (mpHigilightBrush == NULL)
					break;
			}

			//����ѡ������
			if (mpSelectStextList != NULL)
			{
				for (int i = 0; i < mpSelectStextList->GetQuadCount(); i++)
				{
					ED_RECTF ldRectPost;
					mpSelectStextList->GetQuadBound(i, &ldRectPost);
					ED_POINTF ldPointA = { ldRectPost.left ,ldRectPost.top }, ldPointB = { ldRectPost.right ,ldRectPost.bottom };

					if (mpSelectPage == pdfCrtPage2)
					{
						mViewCtl.IsInPage2(ldPointA, ldPointA, true);
						mViewCtl.IsInPage2(ldPointB, ldPointB, true);
					}

					mViewCtl.ImageToViewInit((int)ldPointA.x, (int)ldPointA.y, ldPointA);
					mViewCtl.ImageToViewInit((int)ldPointB.x, (int)ldPointB.y, ldPointB);
					/*mViewCtl.IsInPage2(ldPointA, ldPointA);
					mViewCtl.IsInPage2(ldPointB, ldPointB);*/

					D2D1_RECT_F ldBack;
					ldBack.left = (FLOAT)ldPointA.x;
					ldBack.top = (FLOAT)ldPointA.y;
					ldBack.right = (FLOAT)ldPointB.x;
					ldBack.bottom = (FLOAT)ldPointB.y;
					npPaintBoard->FillRect(ldBack, mpHigilightBrush);
				}
			}

			if (mIsTxtDoc == false)
			{
				D2D1_POINT_2F pt1, pt2;

				if ((impact & 0x1) == 0)
				{	// ���
					pt1.x = mRecentDst.left;
					pt1.y = mRecentDst.top;
					pt2.x = mRecentDst.left;
					pt2.y = mRecentDst.bottom;
					npPaintBoard->DrawLine(pt1, pt2, mpLineBrush);
				}
				if ((impact & 0x2) == 0)
				{	// �ұ�
					pt1.x = mRecentDst.right;
					pt1.y = mRecentDst.top;
					pt2.x = mRecentDst.right;
					pt2.y = mRecentDst.bottom;
					npPaintBoard->DrawLine(pt1, pt2, mpLineBrush);
				}
				if ((impact & 0x4) == 0)
				{	// �ϱ�
					pt1.x = mRecentDst.left;
					pt1.y = mRecentDst.top;
					pt2.x = mRecentDst.right;
					pt2.y = mRecentDst.top;
					npPaintBoard->DrawLine(pt1, pt2, mpLineBrush);
				}
				if ((impact & 0x8) == 0)
				{	// �±�
					pt1.x = mRecentDst.left;
					pt1.y = mRecentDst.bottom;
					pt2.x = mRecentDst.right;
					pt2.y = mRecentDst.bottom;
					npPaintBoard->DrawLine(pt1, pt2, mpLineBrush);
				}
				//if (duopageMode != false && landScope != false)
				if(pdfCrtPage2 != NULL)
				{	// ����
					//pt1.x = (mRecentDst.left + mRecentDst.right)/2.0f;
					pt1.x = ldpageSize.x / 2.0f - mRecentSrc.left;
					if(mRecentDst.left > 0.0f)
						pt1.x = (mRecentDst.left + mRecentDst.right) / 2.0f;

					mfMiddleLineX = pt1.x;
					pt1.y = mRecentDst.top;
					pt2.x = pt1.x;
					pt2.y = mRecentDst.bottom;
					npPaintBoard->DrawLine(pt1, pt2, mpLineBrush);
				}

				
			}
		}



		leResult = ERESULT_SUCCESS;
	} while (false);

	return leResult;
}

void CPdfPicture::GetRectOfViewportOnPage(D2D1_SIZE_F& nrImageSize, D2D1_RECT_F& nrViewPort)
{
	nrImageSize.width = (FLOAT)imageSize.x;
	nrImageSize.height = (FLOAT)imageSize.y;

	nrViewPort = mRecentSrc;
}

void CPdfPicture::SvcDebugOutFmt(char * nswString, ...)
{
	char buf[1000] = { 0 };
	va_list vl;
	va_start(vl, nswString);
	vsprintf_s(buf, 1000, nswString, vl);
	SvcDebugOut(buf);
	va_end(vl);
}

//�����־���ļ���debugview
void CPdfPicture::SvcDebugOut(char* nswString)
{
	moLogFileLock.Enter();

	__try {
		if (nswString == NULL || mbIsLog == false)
			__leave;

		SYSTEMTIME ldSystemTile;
		GetLocalTime(&ldSystemTile);
		char lszOutString[1000] = { 0 };
		sprintf_s(lszOutString, 1000, "%04d-%02d-%02d %02d:%02d:%02d.%03d %s\r\n", ldSystemTile.wYear, ldSystemTile.wMonth, ldSystemTile.wDay, ldSystemTile.wHour, ldSystemTile.wMinute, ldSystemTile.wSecond, ldSystemTile.wMilliseconds, nswString);

		if (true)
		{
			DWORD ldwFileSize = 0;

			if (mhandleLogFile == INVALID_HANDLE_VALUE)
			{
				//���û�д��ļ����ʹ��ļ�

				ULONG lulIndex = 0;

				wchar_t lszFilePath[MAX_PATH] = { 0 };

				do
				{
					wchar_t lpszPath[MAX_PATH];
					SHGetSpecialFolderPath(NULL, lpszPath, CSIDL_DESKTOP, FALSE);
					swprintf_s(lszFilePath, MAX_PATH, L"%s\\reader%d.log", lpszPath,lulIndex++);
					mhandleLogFile = CreateFile(lszFilePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, NULL, NULL);
					if (mhandleLogFile == INVALID_HANDLE_VALUE)
						continue;

					ldwFileSize = GetFileSize(mhandleLogFile, NULL);
					if (ldwFileSize >= 1024 * 1024 * 50) //�ļ����50M
					{
						//̫���˾ͻ����ļ�
						CloseHandle(mhandleLogFile);
						mhandleLogFile = INVALID_HANDLE_VALUE;
						continue;
					}

					SetFilePointer(mhandleLogFile, 0, NULL, FILE_END);

					break;

				} while (mhandleLogFile == INVALID_HANDLE_VALUE && lulIndex < 1000);


			}

			WriteFile(mhandleLogFile, lszOutString, (DWORD)((strlen(lszOutString) + 1)), &ldwFileSize, NULL);
			FlushFileBuffers(mhandleLogFile);
		}
		else
		{
			OutputDebugStringA(nswString);
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {

	}

	moLogFileLock.Leave();

	//OutputDebugStringA(nswString);
}