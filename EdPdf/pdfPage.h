/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */

#ifndef _EDPDFPAGEIMP_H_
#define _EDPDFPAGEIMP_H_
#include "PdfmImp.h"
#include "pdfImg.h"
#include "pdfAnnot.h"
#include "inkAnnot.h"
#include "vector"
#include "pdfStextPage.h"

using namespace std;

DECLARE_BUILTIN_NAME(CpdfPage)
class CpdfPage : public cmmBaseObject<CpdfPage, IEdPage, GET_BUILTIN_NAME(CpdfPage)>, public IEdAnnotManager
{
	friend 	class CpdfDocument;
	friend CpdfStextPage;

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
	int32 GetPageIndex(void);
	
	bool32 GetPageContext(PPAGE_PDF_CONTEXT contextPtr);
		
	//bool32 AddTextAnnot(
	//	IN ED_POINT position,
	//	IN char16_ptr text
	//);

	//bool32 AddInkAnnot(
	//	IN int32 strokeCount,
	//	IN ED_POINT** strokeList,
	//	IN int32* pointCountList
	//);

	virtual IEdAnnotManager_ptr GetAnnotManager(void);

	// �Ӵ浵����װ��һ��Annotation
	virtual IEdAnnot_ptr LoadAnnotFromArchive(buf_ptr buf, uint32 bufSize);

	// �½��ı��ʼ�
	virtual IEdAnnot_ptr AddTextAnnot(
		IN ED_POINT position,
		IN char16_ptr text
	);

	// �½�īˮ�ʼ�
	virtual IEdAnnot_ptr AddInkAnnot(
		IN ED_POINTF* stroke,
		IN int32 pointCount,
		IN ED_COLOR* color = NULL,
		IN float32 border = 1.0f
	);

	// �½�Highlight�ʼ�
	virtual IEdAnnot_ptr AddHighLightAnnot(
		IN IEdStextQuadList_ptr stext,
		IN ED_COLOR* color = NULL,
		IN int32Eink clrN = 3
	);

	// �½�ɾ���ֱʼ�
	virtual IEdAnnot_ptr AddDeleteLineAnnot(
		IN IEdStextQuadList_ptr stext,
		IN ED_COLOR* color = NULL,
		IN int32Eink clrN = 3
	);

	// �½��»��߱ʼ�
	virtual IEdAnnot_ptr AddUnderLineAnnot(
		IN IEdStextQuadList_ptr stext,
		IN ED_COLOR* color = NULL,
		IN int32Eink clrN = 3
	);

	// ��ȡ���ж�����б���annotList==NULLʱ��������Ҫ��List��Ԫ��,������������ʱ����-1���ɹ����ػ�õ�IEdAnnot_ptr������
	virtual int32 GetAllAnnot(IEdAnnot_ptr* annotList, int bufSize);

	// ���һ�ʻ��Ӵ�����һϵ��Ink�ʼǶ����ཻ������annotList==NULLʱ��������Ҫ�Ļ���������,������������ʱ����-1
	virtual int32Eink DetectInkAnnot(
		IN ED_POINTF* stroke,
		IN int32Eink pointCount,
		OUT	IEdAnnot_ptr* annotList,	// ���ڷ��������ཻ����Ļ�����������һ���Է����Ļ����������� IEdAnnot_ptr buf[256];
		IN int32Eink bufSize					// �����������ĵ�Ԫ���������ֽ���
	);

	bool32 GetSelectedText(
		IN ED_RECTF_PTR selBox,
		OUT char16_ptr textBuf,
		IN int32Eink bufSize
	);

	virtual IEdStructuredTextPage_ptr GetStructuredTextPage(void);	// ���صĶ�����Ҫ����release�ͷ�

	// ͨ��ǩ������һ��Annot
	IEdAnnot_ptr GetAnnotBySignature(ULONGLONG signature);
	ULONGLONG GetSignature(IEdAnnot_ptr annot);


	IEdAnnot_ptr GetFirstAnnot(void);

	IEdAnnot_ptr GetNextAnnot(IEdAnnot_ptr crt);

	// ɾ��һ��Annotation�����ô˺�����IEdAnnot_ptr annot����Ȼ��Ҫ�ͷ�
	virtual void RemoveAnnot(
		IEdAnnot_ptr annot
	);

	pdf_page* GetPdfObj(void);
	fz_page* GetFzObj(void) {	return mPage; }

	bool32 Load(CpdfDocument* doc,fz_page *page);

	vector<pdf_annot*> mHighLights;

protected:
	// �ڲ�����
	int32 mPageNo;
	int32 incomplete;
	CpdfDocument* mDocObj;
	fz_page *mPage;
	fz_rect page_bbox;
	fz_display_list *page_list;
	vector<IEdAnnot_ptr> mAnnots;

	CpdfPage();
	~CpdfPage();
	fz_pixmap* DrawBitmap(float32 scalRatio, bool32 cleanUp);
	//bool32 GetBox(pdf_obj *boxName, ED_RECTF& boxRect);//PDF_NAME_MediaBox,PDF_NAME_CropBox,PDF_NAME_ArtBox,PDF_NAME_BleedBox,PDF_NAME_TrimBox
	//void MakeupMatrix(int32 w,int32 h,fz_matrix& matrix);


	DEFINE_CUMSTOMIZE_CREATE(CpdfPage, (int32 pageNo), (pageNo))

	// ��ʼ������������ʵ�ָ��ֲ�ͬ�����ĳ�ʼ��������ע�⣬����������InitOnCreate������һ��Ҫ���û����InitOnCreate����
	// ����0��ʾ�ɹ�������ֵ���λΪ1��ʾ�������ش���Ӧ����ֹ��ʼ�����̣����صľ��Ǵ����룻��������ֵ��ʾ�����Ǵ��󷵻���
	ULONG InitOnCreate(int32 pageNo);

	vector<IEdAnnot_ptr>& GetAnnotList(void) {
		return mAnnots;
	}

	// �½�Structured Text Annotation
	IEdAnnot_ptr AddStextAnnot(
		IN IEdStextQuadList_ptr stext,
		IN enum pdf_annot_type type,
		IN ED_COLOR* color,
		IN int32Eink clrN /*= 3 */
		);


};



#endif//_EDPDFPAGEIMP_H_