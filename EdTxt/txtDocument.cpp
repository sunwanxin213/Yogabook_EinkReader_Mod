/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#include "windows.h"
#include "txtPage.h"
#include "txtDocument.h"
#include "gdiplus.h"

//////////////////////////////////////////////////////////////////////////
DEFINE_BUILTIN_NAME(CtxtDocument)

using namespace Gdiplus;

CtxtDocument::CtxtDocument()
{
	mpTxtMappedBase = NULL;
	mhTxtFileMap = INVALID_HANDLE_VALUE;
	mhTxtFile = INVALID_HANDLE_VALUE;
	mViewPort.X = 0.0f;// 250.0f;
	mViewPort.Y = 0.0f;// 400.0f;
	mpTextBuffer = NULL;
	//mIncomplete = 0;
	mUpdateForSize = false;
	mUpdateForFont = false;
	mPageLoadCallbackFun = NULL;
	//mArrageAgain = false;
	//mThreadNumber = 0;
	mpArrangeThread = NULL;
	/*mPreFocusPage.charBegin = */mRearrangePage.charBegin = MAXULONG32;
	SetFont(L"Arial", 12);
	mAheadPageLoaded = 0;
	mArranging = 0;
	mCrtPageBegin = 0;
	mCrtPageNumber = 0;
}

CtxtDocument::~CtxtDocument()
{
	// ������Ҫͣ�������߳�
	if (mpArrangeThread != NULL)
		delete mpArrangeThread;

	if(mpTextBuffer != NULL && ((bin_ptr)mpTextBuffer < mpTxtMappedBase)|| (bin_ptr)mpTextBuffer > mpTxtMappedBase+mFileSize.LowPart)
		HeapFree(GetProcessHeap(), 0, mpTextBuffer);

	if(mpTxtMappedBase != NULL)
		UnmapViewOfFile(mpTxtMappedBase);

	if(mhTxtFile != INVALID_HANDLE_VALUE)
		CloseHandle(mhTxtFile);
	if (mhTxtFileMap != INVALID_HANDLE_VALUE)
		CloseHandle(mhTxtFileMap);
}

ULONG CtxtDocument::InitOnCreate(const char16_ptr pathName)
{
	ULONG errorCode = EDERR_UNSUCCESSFUL;

	try
	{
		//mpFont = new Gdiplus::Font(L"Arial", 12);
		//THROW_ON_NULL(mpFont);

		// ӳ���ļ����ڴ�
		mhTxtFile = CreateFile(pathName, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL);
		if (mhTxtFile == INVALID_HANDLE_VALUE)
			THROW_INVALID;

		if (GetFileSizeEx(mhTxtFile, &mFileSize) == FALSE)
			THROW_FAILED;

		if (mFileSize.HighPart != 0)
			THROW_OVERFLOW;

		if (mFileSize.LowPart == 0)
			THROW_EMPTY;

		//ָ���ļ���С
		mhTxtFileMap = CreateFileMapping(mhTxtFile, NULL, PAGE_READONLY, 0,mFileSize.LowPart, NULL);
		THROW_ON_INVALID(mhTxtFileMap);

		mpTxtMappedBase = (bin_ptr)MapViewOfFile(mhTxtFileMap, FILE_MAP_READ, 0, 0, 0);
		THROW_ON_NULL(mpTxtMappedBase);

		if (GetBomInfor() == CtxtDocument::un)
			THROW_WRONG_FORMAT;

		// �����Utf8��ʽ
		if ((mBom == u8 || mBom == u8NoBOM)&& UnpackU8()!= EDERR_SUCCESS)
			THROW_WRONG_FORMAT;

		// �����Ansi��ʽ
		if (mBom == ansi && UnpackAnsi() != EDERR_SUCCESS)
			THROW_WRONG_FORMAT;

		// �����UNIDOE big ending��ʽ
		if (mBom == u16b && UnpackBigEnding16() != EDERR_SUCCESS)
			THROW_WRONG_FORMAT;

		if (mBom == u16)
		{
			mpTextBuffer = (wchar_t*)(mpTxtMappedBase+2);	// skip BOM
			mTextCharCount = (mFileSize.LowPart / 2) - 1;
		}
		
		//mFirstPageLoaded = CreateEvent(NULL, true, false, NULL);

		//mThreadStopped = CreateEvent(NULL, true, true, NULL);

		errorCode = EDERR_SUCCESS;// UpdateLayoutForAll(true);
	}
	catch (...)
	{
	}

	return errorCode;
}

ED_ERR CtxtDocument::UnpackU8(void)
{
	//if (mpTextBuffer != NULL && (bin_ptr)mpTextBuffer != mpTxtMappedBase)
	//	HeapFree(GetProcessHeap(), 0, mpTextBuffer);

	// ����һ�³ߴ�
	auto nextChar = mpTxtMappedBase;
	auto endChar = nextChar + mFileSize.LowPart;
	uint32 wideCharCount = 0;

	// skip BOM
	if(mBom != u8NoBOM)
		nextChar += 3;

	while (nextChar < endChar)
	{
		try
		{
			while (nextChar < endChar)
			{
				if ((nextChar[0] & 0x80) == 0)
				{
					nextChar++;
				}
				else
				if ((nextChar[0] & 0xE0) == 0xC0)
				{
					// two bytes
					if (nextChar + 1 >= endChar || (nextChar[1] & 0xC0) != 0x80)
						THROW_UNKNOWN;	// ��������
					nextChar += 2;
				}
				else
				if ((nextChar[0] & 0xF0) == 0xE0)
				{
					// three bytes
					if (nextChar + 2 >= endChar || (nextChar[1] & 0xC0) != 0x80 || (nextChar[2] & 0xC0) != 0x80)
						THROW_UNKNOWN;	// ��������

					nextChar += 3;
				}
				else
				if ((nextChar[0] & 0xF8) == 0xF0)
				{
					// four bytes
					if (nextChar + 3 >= endChar || (nextChar[1] & 0xC0) != 0x80 || (nextChar[2] & 0xC0) != 0x80 || (nextChar[3] & 0xC0) != 0x80)
						THROW_UNKNOWN;	// ��������
					nextChar += 4;
				}
				else
				THROW_UNKNOWN;	// ��������

				wideCharCount++;
			}
		}
		catch (...)
		{
			while (++nextChar < endChar)
			{
				// ������һ����Ч�ַ�
				if ((nextChar[0] & 0x80) == 0 || (nextChar[0] & 0xC0) == 0xC0)
					break;
			}
		}
	}
	wideCharCount += 10;	// ����һ�㻺����


	// �����ڴ����ڽ��
	mpTextBuffer = (wchar_t*)HeapAlloc(GetProcessHeap(),0,wideCharCount * 2);
	if (mpTextBuffer == NULL)
		return EDERR_NOTENOUGH_MEMORY;

	auto crtWideChar = mpTextBuffer;
	auto endWideChar = crtWideChar + wideCharCount;


	nextChar = mpTxtMappedBase;
	// skip BOM
	if (mBom != u8NoBOM)
		nextChar += 3;

	while (nextChar < endChar)
	{
		try
		{
			while (nextChar < endChar && crtWideChar < endWideChar)
			{
				if ((nextChar[0] & 0x80) == 0)
				{
					*crtWideChar = *nextChar++;
				}
				else
				if ((nextChar[0] & 0xE0) == 0xC0)
				{
					// two bytes
					if(nextChar+1 >= endChar || (nextChar[1] & 0xC0) != 0x80)
						THROW_UNKNOWN;	// ��������

					*crtWideChar = (((wchar_t)(nextChar[0] & 0X1F)) << 6) | ((wchar_t)(nextChar[1] & 0X3F));
					nextChar += 2;
				}
				else
				if ((nextChar[0] & 0xF0) == 0xE0)
				{
					// three bytes
					if (nextChar + 2 >= endChar || (nextChar[1] & 0xC0) != 0x80 || (nextChar[2] & 0xC0) != 0x80)
						THROW_UNKNOWN;	// ��������

					*crtWideChar = (((wchar_t)(nextChar[0] & 0X1F)) << 12) | (((wchar_t)(nextChar[1] & 0X3F)) << 6) | ((wchar_t)(nextChar[2] & 0X3F));
					nextChar += 3;
				}
				else
				if ((nextChar[0] & 0xF8) == 0xF0)
				{
					// four bytes
					if (nextChar + 3 >= endChar || (nextChar[1] & 0xC0) != 0x80 || (nextChar[2] & 0xC0) != 0x80 || (nextChar[3] & 0xC0) != 0x80)
						THROW_UNKNOWN;	// ��������
					// ����ط���������ģ�wide charֻ��16λ���޷���������21��bits��������ʱ��֧�֣���������ַ���$ax$
					//*crtWideChar = (((uint16)(nextChar[0] & 0X1F)) << 18) | (((uint16)(nextChar[1] & 0X3F)) << 12) | (((uint16)(nextChar[2] & 0X3F)) << 6) | ((uint16)(nextChar[3] & 0X3F));
					nextChar += 4;
				}
				else
					THROW_UNKNOWN;	// ��������

				crtWideChar++;
			}

		}
		catch (...)
		{
			while (++nextChar < endChar)
			{
				// ������һ����Ч�ַ�
				if ((nextChar[0]&0x80)==0 || (nextChar[0] & 0xC0) == 0xC0)
					break;
			}
		}
	}

	mTextCharCount = (uint32)(crtWideChar - mpTextBuffer);
	*crtWideChar = UNICODE_NULL;

	return EDERR_SUCCESS;
}

//�ж��ļ��Ƿ���utf8�ļ�
bool CtxtDocument::IsU8File(const void* npBuffer, long nlSize)
{
	bool lbIsUTF8 = false;
	int liUtf8Count = 0;
	unsigned char* lpStart = (unsigned char*)npBuffer;
	unsigned char* lpEnd = (unsigned char*)npBuffer + nlSize;
	while (lpStart < lpEnd)
	{
		if (*lpStart < 0x80) // (10000000): ֵС��0x80��ΪASCII�ַ�    
		{
			lpStart++;
		}
		else if (*lpStart < (0xC0)) // (11000000): ֵ����0x80��0xC0֮���Ϊ��ЧUTF-8�ַ�    
		{
			lbIsUTF8 = false;
			liUtf8Count = 0;
			break;
		}
		else if (*lpStart < (0xE0)) // (11100000): �˷�Χ��Ϊ2�ֽ�UTF-8�ַ�    
		{
			if (lpStart >= lpEnd - 1)
			{
				break;
			}

			if ((lpStart[1] & (0xC0)) != 0x80)
			{
				lbIsUTF8 = false;
				liUtf8Count = 0;
				break;
			}

			liUtf8Count++;
			lpStart += 2;
		}
		else if (*lpStart < (0xF0)) // (11110000): �˷�Χ��Ϊ3�ֽ�UTF-8�ַ�    
		{
			if (lpStart >= lpEnd - 2)
			{
				break;
			}

			if ((lpStart[1] & (0xC0)) != 0x80 || (lpStart[2] & (0xC0)) != 0x80)
			{
				lbIsUTF8 = false;
				liUtf8Count = 0;
				break;
			}

			liUtf8Count++;
			lpStart += 3;
		}
		else
		{
			lbIsUTF8 = false;
			liUtf8Count = 0;
			break;
		}
	}

	return liUtf8Count>0;
}

//���������ʱ�����У����ansi��ʽ�ļ���Ϊ��utf8
ED_ERR CtxtDocument::IsU8File(void)
{
	auto nextChar = mpTxtMappedBase;
	auto endChar = nextChar + mFileSize.LowPart;
	uint32 wideCharCount = 0;
	uint32 u8FlagCount = 0;

	try
	{
		while (nextChar < endChar && u8FlagCount < 2)
		{
			if ((nextChar[0] & 0x80) == 0)
			{
				nextChar++;
			}
			else
			if ((nextChar[0] & 0xE0) == 0xC0)
			{
				// two bytes
				if (nextChar + 1 >= endChar || (nextChar[1] & 0xC0) != 0x80)
					THROW_UNKNOWN;	// ��������
				nextChar += 2;
				u8FlagCount++;
			}
			else
			if ((nextChar[0] & 0xF0) == 0xE0)
			{
				// three bytes
				if (nextChar + 2 >= endChar || (nextChar[1] & 0xC0) != 0x80 || (nextChar[2] & 0xC0) != 0x80)
					THROW_UNKNOWN;	// ��������

				nextChar += 3;
				u8FlagCount++;
			}
			else
			if ((nextChar[0] & 0xF8) == 0xF0)
			{
				// four bytes
				if (nextChar + 3 >= endChar || (nextChar[1] & 0xC0) != 0x80 || (nextChar[2] & 0xC0) != 0x80 || (nextChar[3] & 0xC0) != 0x80)
					THROW_UNKNOWN;	// ��������
				nextChar += 4;
				u8FlagCount++;
			}
			else
			THROW_UNKNOWN;	// ��������

			wideCharCount++;
		}
	}
	catch (...)
	{
		// �����ˣ���Ϊû��BOMͷ���ǾͲ��ܱ�����utf8���������ʽ�Ľ�ѹ�����ܹ��ݴ���utf8�еĴ���
		nextChar = endChar;
	}

	return u8FlagCount>=2;	// �ҵ���u8�����ݿ��β����϶���u8�ļ����������ansi�ļ�
}

ED_ERR CtxtDocument::UnpackAnsi(void)
{
	uint32 wideCharCount;

	wideCharCount = MultiByteToWideChar(CP_ACP, 0, (char*)mpTxtMappedBase, mFileSize.LowPart, NULL, 0);
	if (wideCharCount == 0)
		return EDERR_UNSUCCESSFUL;

	// �����ڴ����ڽ��
	mpTextBuffer = (wchar_t*)HeapAlloc(GetProcessHeap(), 0, (wideCharCount+10) * 2);
	if (mpTextBuffer == NULL)
		return EDERR_NOTENOUGH_MEMORY;

	if (MultiByteToWideChar(CP_ACP,0, (char*)mpTxtMappedBase,mFileSize.LowPart,mpTextBuffer,wideCharCount) == 0)
		return EDERR_UNSUCCESSFUL;

	mpTextBuffer[wideCharCount] = UNICODE_NULL;

	mTextCharCount = wideCharCount;

	return EDERR_SUCCESS;
}

ED_ERR CtxtDocument::UnpackBigEnding16(void)
{
	uint32 wideCharCount = mFileSize.LowPart / 2;
	// �����ڴ����ڽ��
	mpTextBuffer = (wchar_t*)HeapAlloc(GetProcessHeap(), 0, (wideCharCount +10) * 2);
	if (mpTextBuffer == NULL)
		return EDERR_NOTENOUGH_MEMORY;

	auto crtWideChar = mpTextBuffer;
	auto endWideChar = crtWideChar + mTextCharCount;

	// skip BOM
	auto nextChar = mpTxtMappedBase + 2;
	auto endChar = mpTxtMappedBase + mFileSize.LowPart;

	while (nextChar < endChar)
	{
		*crtWideChar = ((((wchar_t)(nextChar[0])) << 8) | nextChar[1]);

		nextChar += 2;
		crtWideChar++;
	}

	mTextCharCount = (uint32)(crtWideChar - mpTextBuffer);
	*crtWideChar = UNICODE_NULL;

	return EDERR_SUCCESS;
}

bool32 CtxtDocument::LoadAllPage(PEDDOC_CALLBACK callBackFun, void_ptr contextPtr, PPAGE_PDF_CONTEXT initPage)
{
	mPageLoadCallbackFun = callBackFun;
	mPageLoadCallbackContext = contextPtr;
 
	return true;
}

int32 CtxtDocument::GetMetaData(char* keyName, char* dataBuf, int32 bufSize)
{
	if (_stricmp(keyName, "format") == 0)
	{
		strcpy_s(dataBuf, bufSize, "Text files");
		return (int32)strlen("Text files");
	}

	return 0;
}

int32 CtxtDocument::GetDocType(void)
{
	return 0;
}

int32 CtxtDocument::GetPageCount(void)
{
	int32 revDate;

	mThreadDataLock.Enter();

	revDate = mAllPages.Size();

	mThreadDataLock.Leave();

	return revDate;
}

ED_ERR CtxtDocument::LoadPage(CtxtPage* pageObj)
{
	ED_ERR errBack = EDERR_SUCCESS;


	return errBack;
}

IEdPage_ptr CtxtDocument::GetPage(PPAGE_PDF_CONTEXT contextPtr)
{
	CtxtPage* pageObj = NULL;
	int32 pageIndex;
	PAGE_PDF_CONTEXT defaultContext = { 0,0,0 };

	if (contextPtr != NULL)
	{
		if(defaultContext.pageContext < mTextCharCount)
			defaultContext = *contextPtr;	// ��Ч��ʹ����

		contextPtr = &defaultContext;
	}


	mThreadDataLock.Enter();

	pageIndex = contextPtr->pageIndex;
	if (contextPtr->pageIndex >= (uint32)mAllPages.Size() || mAllPages[contextPtr->pageIndex].charBegin != contextPtr->pageContext)
	{
		for (int i = 0; i < mAllPages.Size(); i++)
		{
			if (mAllPages[i].charBegin == contextPtr->pageContext)
			{
				pageIndex = i;
				break;
			}
		}
	}

	if (pageIndex < mAllPages.Size())
	{
		try
		{
			pageObj = CtxtPage::CreateInstance(pageIndex, mpTextBuffer + mAllPages[pageIndex].charBegin, mAllPages[pageIndex].charCount, this);
			if (pageObj != NULL && ERR_FAILED(LoadPage(pageObj)))
			{
				pageObj->Release();
				pageObj = NULL;
			}
		}
		catch (...)
		{
		}
	}

	mThreadDataLock.Leave();


	return pageObj;
}

IEdPage_ptr CtxtDocument::GetPage(int32 pageIndex)
{
	CtxtPage* pageObj = NULL;

	mThreadDataLock.Enter();

	if (pageIndex >= 0 && pageIndex < mAllPages.Size())
	{
		try
		{
			pageObj = CtxtPage::CreateInstance(pageIndex, mpTextBuffer + mAllPages[pageIndex].charBegin, mAllPages[pageIndex].charCount, this);
			if (pageObj != NULL && ERR_FAILED(LoadPage(pageObj)))
			{
				pageObj->Release();
				pageObj = NULL;
			}
		}
		catch (...)
		{
		}
	}

	mThreadDataLock.Leave();


	return pageObj;
}

IEdPage_ptr CtxtDocument::GetPage(IEdPage_ptr currentPage, int32 pagesOff)
{
	PAGE_PDF_CONTEXT pageContext;
	int32 pageIndex;

	currentPage->GetPageContext(&pageContext);

	if (pageContext.pageIndex < 0)
		return NULL;

	mThreadDataLock.Enter();

	pageIndex = (int32)pageContext.pageIndex;
	if (pageIndex >= mAllPages.Size() || mAllPages[pageIndex].charBegin != pageContext.pageContext)
	{	// ���index���ԣ���ȥ�����в�ѯ�����µ�index
		for (int i = 0; i < mAllPages.Size(); i++)
		{
			if (mAllPages[i].charBegin == pageContext.pageContext)
			{
				pageIndex = i;
				break;
			}
		}
	}

	if (pageIndex < mAllPages.Size() && pageIndex + pagesOff >= 0 && pageIndex + pagesOff < mAllPages.Size())
	{
		pageContext.pageIndex = (uint32)(pageIndex+pagesOff);
		pageContext.pageContext = mAllPages[pageContext.pageIndex].charBegin;
		pageContext.pageContext2 = 0;
	}
	else
		pageContext.pageIndex = MAXULONG32;

	mThreadDataLock.Leave();

	if (pageContext.pageIndex == MAXULONG32)
		return NULL;

	return GetPage(&pageContext);
}


bool32 CtxtDocument::LoadAllThumbnails(PEDDOC_THUMBNAILS_CALLBACK /*callBack*/, void_ptr /*contextPtr*/, const wchar_t* /*orgFilePathName*/)
{
	return false;
}

bool32 CtxtDocument::GetThumbanilsPath(wchar_t* npszPathBuffer, int niLen)
{
	return false;
}

bool32 CtxtDocument::GetThumbnailPathName(int32Eink pageIndex, char16 pathName[256], bool* /*hasAnnot*/)
{
	return false;
}

IEdPage_ptr CtxtDocument::Rearrange(IEdPage_ptr currentPage)
{
	PAGE_PDF_CONTEXT context = { 0,0,0 };

	if (currentPage != NULL)
		currentPage->GetPageContext(&context);

	return Rearrange(&context);
}

cmmVector<ULONG> threadArrived;
// This method to update the layout of all pages.
IEdPage_ptr CtxtDocument::Rearrange(PPAGE_PDF_CONTEXT contextPtr)
{
	IEdPage_ptr newPage = NULL;

	threadArrived.Insert(-1, GetCurrentThreadId());

	if (mUpdateForSize == false && mUpdateForFont == false)
		return NULL;

	Gdiplus::RectF viewPort;

	viewPort.X = 0;
	viewPort.Y = 0;
	viewPort.Width = mViewPort.X;
	viewPort.Height = mViewPort.Y;


	InterlockedExchange(&mArranging, 1);

	// �ͷ�ǰһ���̶߳���
	CTxdArrangeThread* lpThreadObj = (CTxdArrangeThread*)InterlockedExchangePointer((void**)&mpArrangeThread, NULL);
	if (lpThreadObj != NULL)
		delete lpThreadObj;

	lpThreadObj = new CTxdArrangeThread(mpTextBuffer, contextPtr->pageContext, mTextCharCount, false, mFontName, (float)mFontSize, &viewPort, (PEDDOC_THREAD_CALLBACK)CtxtDocument::PageArrangedCallBack, (void*)this);
	if (lpThreadObj == NULL)
		return NULL;

	InterlockedExchangePointer((void**)&mpArrangeThread, lpThreadObj);

	//mThreadNumber = lpThreadObj->mThreadNumber;

	//��յ�ǰҳ��
	mThreadDataLock.Enter();
	mAllPages.Clear();
	mThreadDataLock.Leave();

	// ��ʼ�Ű�
	lpThreadObj->Start();

	if (lpThreadObj->mResult != EDERR_SUCCESS)
		return NULL;

	return GetPage(contextPtr);
}

void CtxtDocument::SetFont(const char16_ptr fontName, int32Eink fontSize)
{
	wcscpy_s(mFontName, 256, fontName);
	mFontSize = (float)fontSize;

	mUpdateForFont = true;
}

void CtxtDocument::SetViewPort(int32Eink viewWidth, int32Eink viewHeight)
{
	if (mViewPort.X != (float)viewWidth || mViewPort.Y != (float)viewHeight)
	{
		mViewPort.X = (float)viewWidth;
		mViewPort.Y = (float)viewHeight;
		mUpdateForSize = true;
	}
}

int32Eink CtxtDocument::GetPageIndex(CtxtPage* pageObj)
{
	uint32 pageBegin = (uint32)(pageObj->mCharPtr - mpTextBuffer);
	int32Eink pageIndex = -1;

	if (mCrtPageBegin == pageBegin)
	{
		pageIndex = mCrtPageNumber;
	}
	else
	{
		CSectionAutoLock lock(mThreadDataLock);

		for (int i = 0; i < mAllPages.Size(); i++)
		{
			if (mAllPages[i].charBegin == pageBegin)
			{
				if (mArranging != 0)
				{
					mCrtPageNumber = i;
					mCrtPageBegin = pageBegin;
				}

				pageIndex = i;

				break;
			}
		}

	}


	return pageIndex;
}

CtxtDocument::BOM CtxtDocument::GetBomInfor()
{
	const unsigned char* const stringHead = (const unsigned char*)mpTxtMappedBase;
	mBom = CtxtDocument::ansi;
//	WCHAR string[] = L"˽��4��ǰ�˖|�����ФäƤ��ޤ����� ˽�����Ť����ΤϺ�����Ǥ��� ���Κݳ֤��ϤȤƤ␙�����ˤϛQ����ס��Ǥ��ʤ��Τǡ�˽�Κݳ֤��ϤȤƤ��y�����Ǥ��������1���¤ϖ|����ס��Ǥ��ޤ�����";
	//WCHAR string[] = L"���������";
	//char ss[] = "���AAA";

	do 
	{
		if (mFileSize.LowPart < 4)
			break;

		if (stringHead[0] == 0xFF && stringHead[1] == 0xFE)
		{
			mBom = u16;
			break;
		}

		if (stringHead[0] == 0xFE && stringHead[1] == 0xFF)
		{
			mBom = u16b;
			break;
		}

		if (stringHead[0] == 0xEF && stringHead[1] == 0xBB && stringHead[2] == 0xBF)
		{
			mBom = u8;
			break;
		}

		// Ŀǰ��֧��Utf7��ʽ
		//if (stringHead[0] == 0x2B && stringHead[1] == 0x2F && stringHead[2] == 0x76 )
		//{
		//	mBom = u7;
		//	break;
		//}

		// ���Է���һ�£����Ŀ���ǲ���utf8
		if (IsU8File(mpTxtMappedBase,mFileSize.LowPart) != false)
		{
			mBom = u8NoBOM;
		}

	} while (false);
		
	return mBom;
}

void __stdcall CtxtDocument::PageArrangedCallBack(LONG threadNumber, uint32 loadingStep, PST_PAGE_CONTROL pageCtl, CtxtDocument* thisDoc)
{
// 	if (thisDoc->mThreadNumber != threadNumber)
// 		return;

	uint32 pageCount = 0;

	thisDoc->mThreadDataLock.Enter();

	if (loadingStep != LOADING_STEP_COMPLETE)
	{
		if (LOADING_STEP_AHEAD(loadingStep) != 0)
		{
			if (loadingStep == LOADING_STEP_AHEAD_COMPLETE)
			{
				// ���������ָ��pageCtl�����˲�ͬ���͵����ݣ��˴��������������ڵ�����
				PageControl_Vector* pageVector = (PageControl_Vector*)pageCtl;

				thisDoc->mAllPages.Insert(0, *pageVector);

				thisDoc->mAheadPageLoaded = 0;	// ǰҳ�Ѿ�ȫ��װ�룬������Ҫ��ʱ�������ҳ��
			}
			else
			{
				thisDoc->mAheadPageLoaded = LOADING_STEP_AHEAD_NUMBER(loadingStep);
			}

			loadingStep = 1;
		}
		else
		{
			if(pageCtl != NULL)
				thisDoc->mAllPages.Insert(-1, *pageCtl);
		}
	}
	else
	{
		thisDoc->mCrtPageBegin = 0;
		thisDoc->mCrtPageNumber = 0;
		InterlockedExchange(&thisDoc->mArranging, 0);
	}

	pageCount = thisDoc->mAllPages.Size() + thisDoc->mAheadPageLoaded;

	thisDoc->mThreadDataLock.Leave();

	// ����
	thisDoc->mPageLoadCallbackFun(loadingStep, pageCount, thisDoc->mPageLoadCallbackContext);
}


