/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#pragma once
#ifndef _EDPDFDOC_H_
#define _EDPDFDOC_H_
#include "smtmImp.h"



typedef struct _ST_DOC_LOADING_THREAD {
	char16_ptr pathName;
	PVOID docObject;
}ST_DOC_LOADING_THREAD,* PST_DOC_LOADING_THREAD;


DECLARE_BUILTIN_NAME(CSmtDocument)
class CSmtDocument : public cmmBaseObject<CSmtDocument, IEdDocument, GET_BUILTIN_NAME(CSmtDocument)>
{
public:
	//enum ERENDER_STEP {
	//	eRenderBegin = 1,
	//	eRenderParepare = 2,
	//	eRenderRender = 3,
	//	eRenderEnd = 4,
	//	eRenderStop = 5
	//};

	DEFINE_CUMSTOMIZE_CREATE(CSmtDocument, (const char16_ptr pathName), (pathName))

	BaseEngine* GetEngine(void) { return documentEngine; }

	bool32 LoadAllPage(PEDDOC_CALLBACK callBackFun, void_ptr contextPtr);
	int32Eink GetMetaData(char* keyName, char* dataBuf, int32Eink bufSize);
	int32Eink GetDocType(void);
	int32Eink GetPageCount(void);
	IEdPage_ptr GetPage(int32Eink pageIndex);
	IEdPage_ptr GetPage(PPAGE_PDF_CONTEXT contextPtr);
	IEdPage_ptr GetPage(IEdPage_ptr currentPage, int32Eink pagesOff);

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

public:

protected:
	// �ڲ�����
	volatile int32Eink documentType;
	volatile int32Eink pageCount;
	BaseEngine* volatile documentEngine;
	EngineType engineType;

	PEDDOC_CALLBACK mPageLoadCallbackFun;
	void_ptr mPageLoadCallbackContext;

	ST_DOC_LOADING_THREAD mThreadData;
	HANDLE mThreadHandle;
	HANDLE mPageLoadEvent;
	volatile LONG mExitThread;


	//// ��ǰҳ
	//int32Eink incomplete;

	CSmtDocument();
	~CSmtDocument();
	//ED_ERR Open_progressive(const char16_ptr fileName);


	// ��ʼ������������ʵ�ָ��ֲ�ͬ�����ĳ�ʼ��������ע�⣬����������InitOnCreate������һ��Ҫ���û����InitOnCreate����
	// ����0��ʾ�ɹ�������ֵ���λΪ1��ʾ�������ش���Ӧ����ֹ��ʼ�����̣����صľ��Ǵ����룻��������ֵ��ʾ�����Ǵ��󷵻���
	ULONG InitOnCreate(const char16_ptr pathName);

	ED_ERR LoadPage(CSmtPage* pageObj);

	static ULONG WINAPI LoadingThread(PST_DOC_LOADING_THREAD npContext);

	static bool __stdcall PageLoadCallBack(uint32 loadingStep, CSmtDocument* thisDoc);

};



#endif//_EDPDFDOC_H_