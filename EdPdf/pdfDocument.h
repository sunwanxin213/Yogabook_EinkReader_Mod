/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */

#pragma once
#ifndef _EDPDFDOC_H_
#define _EDPDFDOC_H_
#include "PdfmImp.h"
#include "cmmstring.h"
#include "PageCtl.h"

typedef cmmVector<CpdfPage*, 16, 16> CpdfPage_vector;

//class CEsThreadNode {
//public:
//	ULONG muThreadID;
//	IXsWidget* mpOwnerWidget;
//	void operator=(const class CEsThreadNode& src) {
//		muThreadID = src.muThreadID;
//		mpOwnerWidget = src.mpOwnerWidget;
//	}
//};
//
//class CEsThreadNodeCriterion	// Ĭ�ϵ��ж�׼��
//{
//public:
//	bool operator () (const CEsThreadNode& Obj1, const CEsThreadNode& Obj2)const // һ��Ҫ����������
//	{
//		// ������Ob1С�ڶ���Obj2ʱ������True�����򷵻�false
//		return (Obj1.muThreadID < Obj2.muThreadID);
//	}
//};
//
//// ����ID������̶߳���
//typedef cmmSequence<CEsThreadNode, CEsThreadNodeCriterion> TEsThreadSequence;



DECLARE_BUILTIN_NAME(CpdfDocument)
class CpdfDocument : public cmmBaseObject<CpdfDocument, IEdDocument, GET_BUILTIN_NAME(CpdfDocument)>
{
public:
	//enum ERENDER_STEP {
	//	eRenderBegin = 1,
	//	eRenderParepare = 2,
	//	eRenderRender = 3,
	//	eRenderEnd = 4,
	//	eRenderStop = 5
	//};

	DEFINE_CUMSTOMIZE_CREATE(CpdfDocument, (const char16_ptr pathName), (pathName))

	bool32 LoadAllPage(PEDDOC_CALLBACK callBackFun, void_ptr contextPtr, PPAGE_PDF_CONTEXT initPage);
	int32 GetMetaData(char* keyName, char* dataBuf, int32 bufSize);
	int32 GetDocType(void);
	int32 GetPageCount(void);
	IEdPage_ptr GetPage(int32Eink pageIndex);
	IEdPage_ptr GetPage(PPAGE_PDF_CONTEXT contextPtr);
	IEdPage_ptr GetPage(IEdPage_ptr currentPage, int32Eink pagesOff);
	bool32 LoadAllThumbnails(PEDDOC_THUMBNAILS_CALLBACK callBack, void_ptr contextPtr, const wchar_t* nphumbnailPathName);
	bool32 GetThumbnailPathName(int32Eink pageIndex, char16 pathName[256], bool* hasAnnot);
	bool32 GetThumbanilsPath(wchar_t* npszPathBuffer, int niLen);
	int32Eink GetAnnotPageCount(void);

	bool32 SaveAllChanges(const char16_ptr pathName = NULL);

	pdf_document* GetPdfObj(void);
	bool SetDirty(bool dirty);

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

	//void TestHighLight();
public:

protected:
	// �ڲ�����
	fz_document* fzDoc;
	int32 pageCount;
	int miObjCount; //�ϴα���ʱ�Ķ�������
	cmmStringW mPathName;
	bool mDirty;
	CEdPageControl mPageControl;
	CExclusiveAccess mDocAccLock;	// fz_doc����ķ��ʣ����뻥��
	volatile bool mLoadThumb;
	HANDLE mThumbThreadExit;

	// ��ǰҳ
	int32 incomplete;

	CpdfDocument();
	~CpdfDocument();
	ED_ERR Open_progressive(const char16_ptr fileName);


	// ��ʼ������������ʵ�ָ��ֲ�ͬ�����ĳ�ʼ��������ע�⣬����������InitOnCreate������һ��Ҫ���û����InitOnCreate����
	// ����0��ʾ�ɹ�������ֵ���λΪ1��ʾ�������ش���Ӧ����ֹ��ʼ�����̣����صľ��Ǵ����룻��������ֵ��ʾ�����Ǵ��󷵻���
	ULONG InitOnCreate(const char16_ptr pathName);

	ED_ERR LoadPage(CpdfPage* pageObj);
};



#endif//_EDPDFDOC_H_