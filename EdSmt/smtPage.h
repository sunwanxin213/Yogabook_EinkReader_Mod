/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#ifndef _EDPDFPAGEIMP_H_
#define _EDPDFPAGEIMP_H_
#include "smtmImp.h"


//typedef cmmVector<CXsWidget*, 64, 64> TEsWidgetVector;

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

DECLARE_BUILTIN_NAME(CSmtBitmap)
class CSmtBitmap : public cmmBaseObject<CSmtBitmap, IEdBitmap, GET_BUILTIN_NAME(CSmtBitmap)>
{
	friend 	class CSmtPage;

public:
	bin_ptr GetBuffer();
	int32Eink GetWidth();
	int32Eink GetHeight();

protected:
	// �ڲ�����
	RenderedBitmap* mEngineBmp;
	HBITMAP mDIBSection;

	CSmtBitmap();
	~CSmtBitmap();

	//DEFINE_CUMSTOMIZE_CREATE(CSmtBitmap, (fz_pixmap* fzImage), (fzImage))
	DEFINE_CUMSTOMIZE_CREATE(CSmtBitmap, (RenderedBitmap* engineBmp), (engineBmp))

	// ��ʼ������������ʵ�ָ��ֲ�ͬ�����ĳ�ʼ��������ע�⣬����������InitOnCreate������һ��Ҫ���û����InitOnCreate����
	// ����0��ʾ�ɹ�������ֵ���λΪ1��ʾ�������ش���Ӧ����ֹ��ʼ�����̣����صľ��Ǵ����룻��������ֵ��ʾ�����Ǵ��󷵻���
	ULONG InitOnCreate(RenderedBitmap* engineBmp);
};




DECLARE_BUILTIN_NAME(CSmtPage)
class CSmtPage : public cmmBaseObject<CSmtPage, IEdPage, GET_BUILTIN_NAME(CSmtPage)>
{
	friend 	class CSmtDocument;

public:
	//enum ERENDER_STEP {
	//	eRenderBegin = 1,
	//	eRenderParepare = 2,
	//	eRenderRender = 3,
	//	eRenderEnd = 4,
	//	eRenderStop = 5
	//};

	bool32 GetMediaBox(
		OUT ED_RECTF_PTR mediaBox
	);
	bool32 GetCropBox(
		OUT ED_RECTF_PTR cropBox
	);
	bool32 GetBleedBox(
		OUT ED_RECTF_PTR bleedBox
	);
	IEdBitmap_ptr Render(
		IN float32 scalRatio,
		IN bool32 cleanUp
	);
	int32Eink GetPageIndex(void);

	bool32 GetPageContext(
		PPAGE_PDF_CONTEXT contextPtr
	);

protected:
	// �ڲ�����
	int32Eink mPageNo;	// begin from zero to count-1
	int32Eink incomplete;
	CSmtDocument* documentObject;

	CSmtPage();
	~CSmtPage();
//	fz_pixmap* DrawBitmap(float32 scalRatio, bool32 cleanUp);
	//bool32 GetBox(pdf_obj *boxName, ED_RECTF& boxRect);//PDF_NAME_MediaBox,PDF_NAME_CropBox,PDF_NAME_ArtBox,PDF_NAME_BleedBox,PDF_NAME_TrimBox
	//void MakeupMatrix(int32Eink w,int32Eink h,fz_matrix& matrix);


	DEFINE_CUMSTOMIZE_CREATE(CSmtPage, (int32Eink pageNo,CSmtDocument* docObj), (pageNo, docObj))

	// ��ʼ������������ʵ�ָ��ֲ�ͬ�����ĳ�ʼ��������ע�⣬����������InitOnCreate������һ��Ҫ���û����InitOnCreate����
	// ����0��ʾ�ɹ�������ֵ���λΪ1��ʾ�������ش���Ӧ����ֹ��ʼ�����̣����صľ��Ǵ����룻��������ֵ��ʾ�����Ǵ��󷵻���
	ULONG InitOnCreate(int32Eink pageNo, CSmtDocument* docObj);

};

#define ENGINE_INDEX(_X) (_X+1)

#endif//_EDPDFPAGEIMP_H_