/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */

#ifndef _HIGHLIGHT_H_
#define _HIGHLIGHT_H_
#include "PdfmImp.h"
#include "cmmstruct.h"
#include "cmmGeometry.h"
#include "cmmptr.h"
#include "pdfStextQuadList.h"


DECLARE_BUILTIN_NAME(CpdfStextAnnot)
class CpdfStextAnnot : public cmmBaseObject<CpdfStextAnnot, IEdStextAnnot, GET_BUILTIN_NAME(CpdfStextAnnot)>
{
	friend 	class CpdfPage;

protected:
	// �ڲ�����
	uint32 mAnnotType;
	pdf_annot* mPdfRawObj;
	float mColor[4];
	int mColorN;
	float mBorder;

	cmmReleasePtr<CpdfStextQuadsList> mQuadListObj;

	CpdfStextAnnot();

	// ��ʼ������������ʵ�ָ��ֲ�ͬ�����ĳ�ʼ��������ע�⣬����������InitOnCreate������һ��Ҫ���û����InitOnCreate����
	// ����0��ʾ�ɹ�������ֵ���λΪ1��ʾ�������ش���Ӧ����ֹ��ʼ�����̣����صľ��Ǵ����룻��������ֵ��ʾ�����Ǵ��󷵻���
	ULONG InitOnCreate(void);

	DEFINE_CUMSTOMIZE_CREATE(CpdfStextAnnot, (void), ())


	// ��ҳ�����INK Annotation����������Stroke List����װ�뱾����
	bool PdfCreateAnnot(
		IN pdf_page* pageObj, 
		IN IEdStextQuadList_ptr stext,
		IN enum pdf_annot_type type,
		IN float* clr, 
		IN int clrN = 3, 
		IN float32 border = 1
		);

	// �Ӵ浵����StextAnnot�浵����
	static bool UnpackFromAchive(char* archive, int32 bufSize,OUT ED_POINTF& a,OUT ED_POINTF& b,OUT uint32& type,OUT ED_COLOR& clr,OUT int32Eink& clrN);
	// ��pdf_annotation����װ�뵽������
	bool Load(pdf_annot* annot);

	// ��ȡǩ�����ڲ�ֵ
	void GetSignature(ULONG& checksum, USHORT& altData);

	pdf_annot* GetPdfObj() {
		return mPdfRawObj;
	}

	//int32 DetectIntersection(CpdfStextQuadsList& checkWith); // ���غͱ�ʵ���ĵڼ���Quad�ཻ������-1��ʾ���ཻ

public:
	~CpdfStextAnnot();


	virtual uint32 GetType();	// EDPDF_ANNOT_INK ,EDPDF_ANNOT_UNDERL, EDPDF_ANNOT_DELETE,EDPDF_ANNOT_HIGHLIGHT, EDPDF_ANNOT_TEXT
	virtual char* GetTypeName();		// "ink" "text" "highlight" "underline" "deleteline" or "Identity"

	IEdTextAnnot_ptr ConvertToTextAnnot(void) { return NULL; }
	IEdInkAnnot_ptr ConvertToInkAnnot(void) { return NULL; }
	IEdStextAnnot_ptr ConvertToStextAnnot(void) { return this; }

	uint32 SaveToAchive(buf_ptr buf, uint32 bufSize);	// ���˶��󱣴浽�浵���������Ӵ浵�ָ��浽��Page(ͨ������IEdPage�ķ��� LoadAnnotFromArchive)

	void SetColor(ED_COLOR clr);		// ����������ɫ
	ED_COLOR GetColor(void);

	void SetBorder(int32 border);	// ����������� 1 ~ N
	int32 GetBorder(void);

	virtual CpdfStextQuadsList* GetQuadsList(void); // ��÷ֿ��б�
};



#endif//_HIGHLIGHT_H_