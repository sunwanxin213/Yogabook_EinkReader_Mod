/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#pragma once
#ifndef _TXTDOC_H_
#define _TXTDOC_H_
#include "TxtmImp.h"
#include "txtArrange.h"


DECLARE_BUILTIN_NAME(CtxtDocument)
class CtxtDocument : public cmmBaseObject<CtxtDocument, IEdDocument, GET_BUILTIN_NAME(CtxtDocument)>
{
public:
	enum BOM {
		un = 0,
		ansi = 1,
		u16 = 2,
		u16b = 3,
		u7 = 4,
		u8 = 5,
		u8NoBOM=6
	};

	DEFINE_CUMSTOMIZE_CREATE(CtxtDocument, (const char16_ptr pathName), (pathName))

	bool32 LoadAllPage(PEDDOC_CALLBACK callBackFun, void_ptr contextPtr);
	int32 GetMetaData(char* keyName, char* dataBuf, int32 bufSize);
	int32 GetDocType(void);
	int32 GetPageCount(void);
	IEdPage_ptr GetPage(int32 pageIndex);
	IEdPage_ptr GetPage(PPAGE_PDF_CONTEXT contextPtr);	// contextPtr==NULL for the first page of this doc
	IEdPage_ptr GetPage(IEdPage_ptr currentPage,int32 pagesOff);

	//////////////////////////////////////////////////////////////////////////
	// following functions just work on a txt document.

	// This method to update the layout of all pages.
	IEdPage_ptr Rearrange(PPAGE_PDF_CONTEXT contextPtr);

	// This method to update the layout of all pages.
	IEdPage_ptr Rearrange(IEdPage_ptr currentPage);

	// This method to change the font used to render this document. 
	void SetFont(const char16_ptr fontName, int32Eink fontSize);

	// this method to set the view port to render txt document. 
	void SetViewPort(int32Eink viewWidth, int32Eink viewHeight);

	wchar_t* GetDocBuffer(void) {
		return mpTextBuffer;
	}

	int32Eink GetPageIndex(CtxtPage* pageObj);

	//void RegisterPage(CtxtPage* pageObj);
	//void UnregisterPage(CtxtPage* pageObj);

public:
	char16_ptr GetFontName(void) {
		return mFontName;
	}
	float GetFontSize(void) {
		return mFontSize;
	}

	Gdiplus::PointF* GetViewPort(void) {
		return &mViewPort;
	}

protected:
	// �ڲ�����
	HANDLE mhTxtFile;
	DWORD muTxtFileLength;
	HANDLE mhTxtFileMap;
	bin_ptr mpTxtMappedBase;
	LARGE_INTEGER mFileSize;
	BOM mBom;
	wchar_t*  mpTextBuffer;
	uint32 mTextCharCount;
	PEDDOC_CALLBACK mPageLoadCallbackFun;
	void_ptr mPageLoadCallbackContext;

	// ��ǰҳ
	volatile int32 mUpdateForSize;
	volatile int32 mUpdateForFont;
	PageControl_Vector mAllPages;
	char16	mFontName[256];
	float mFontSize;
	Gdiplus::PointF mViewPort;
	CExclusiveAccess mThreadDataLock;

	volatile CTxdArrangeThread* mpArrangeThread;
	volatile LONG mThreadNumber;

	// �Ű���Ϣ
	ST_PAGE_CONTROL mRearrangePage;
	int32 mRearrangePageIndex;


	// ��������ҳ�󣬱��ֽ���ҳ
	//FocusControl_Stack mFocusCtl;

	CtxtDocument();
	~CtxtDocument();


	// ��ʼ������������ʵ�ָ��ֲ�ͬ�����ĳ�ʼ��������ע�⣬����������InitOnCreate������һ��Ҫ���û����InitOnCreate����
	// ����0��ʾ�ɹ�������ֵ���λΪ1��ʾ�������ش���Ӧ����ֹ��ʼ�����̣����صľ��Ǵ����룻��������ֵ��ʾ�����Ǵ��󷵻���
	ULONG InitOnCreate(const char16_ptr pathName);

	ED_ERR UnpackU8(void);
	ED_ERR IsU8File(void);
	//�ж��ļ��Ƿ���utf8�ļ�
	bool IsU8File(const void* npBuffer, long nlSize);

	ED_ERR UnpackAnsi(void);

	ED_ERR UnpackBigEnding16(void);

	ED_ERR LoadPage(CtxtPage* pageObj);

	BOM GetBomInfor();

	static void __stdcall PageArrangedCallBack(LONG threadNumber, uint32 loadingStep, PST_PAGE_CONTROL pageCtl, CtxtDocument* thisDoc);

};



#endif//_TXTDOC_H_