/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */

#ifndef _HIGHLIGHT_H_
#define _HIGHLIGHT_H_
#include "PdfmImp.h"
#include "cmmstruct.h"

class CpdfHighlightQuad {
public:
	float32 x1,y1,x2,y2;
	CpdfHighlightQuad() {
	}

	CpdfHighlightQuad(const CpdfHighlightQuad& src) {
		*this = src;
	}

	void operator = (const CpdfHighlightQuad& src) {
		x1 = src.x1;
		x2 = src.x2;
		y1 = src.y1;
		y2 = src.y2;
	}
	void operator =(const fz_quad& quad) {
		x1 = quad.ul.x;
		y1 = quad.ul.y;
		x2 = quad.lr.x;
		y2 = quad.lr.y;
	}

	// �ж��߶��Ƿ������ཻ
	bool IsLineCrossed(float p1x, float p1y, float p2x, float p2y)
	{
		// �����ж��߶ε����������Ƿ����ھ����е�
		if ((p1x >= x1 && p1x < x2 && p1y >= y1 && p1y < y2) != false)
			return true;
		if ((p2x >= x1 && p2x < x2 && p2y >= y1 && p2y < y2) != false)
			return true;

		// ���ж������߶��Ƿ�;��ε��������е�ĳ���ཻ
		if (CExLine::IsSegmentCross(p1x, p1y, p2x, p2y, x1, y1, x2, y1) != false)
			return true;
		if (CExLine::IsSegmentCross(p1x, p1y, p2x, p2y, x2, y1, x2, y2) != false)
			return true;
		if (CExLine::IsSegmentCross(p1x, p1y, p2x, p2y, x1, y2, x2, y2) != false)
			return true;
		if (CExLine::IsSegmentCross(p1x, p1y, p2x, p2y, x1, y1, x1, y2) != false)
			return true;

		return false;
	}
};

typedef cmmVector<CpdfHighlightQuad, 0, 128> CpdfHighlightQuadList;

DECLARE_BUILTIN_NAME(CpdfHlAnnot)
class CpdfHlAnnot : public cmmBaseObject<CpdfHlAnnot, IEdHighLightAnnot, GET_BUILTIN_NAME(CpdfHlAnnot)>
{
	friend 	class CpdfPage;


protected:
	// �ڲ�����
	pdf_annot* mPdfRawObj;
	//ED_POINT mPosition;
	float mColor[4];
	int mColorN;
	float mBorder;

	CpdfHighlightQuadList mPdfHlQuads;	// ���pdf�ļ�װ���Highlight quad����

	CpdfHlAnnot();

	// ��ʼ������������ʵ�ָ��ֲ�ͬ�����ĳ�ʼ��������ע�⣬����������InitOnCreate������һ��Ҫ���û����InitOnCreate����
	// ����0��ʾ�ɹ�������ֵ���λΪ1��ʾ�������ش���Ӧ����ֹ��ʼ�����̣����صľ��Ǵ����룻��������ֵ��ʾ�����Ǵ��󷵻���
	ULONG InitOnCreate();

	DEFINE_CUMSTOMIZE_CREATE(CpdfHlAnnot, (), ())

	// ��ҳ�����INK Annotation����������Stroke List����װ�뱾����
	bool AddInk(pdf_page* pageObj, CpdfInkMultiList& strokeList, float* clr, int clrN, float32 border);
	// ��ҳ�����INK Annotation���������Դ浵����װ�뱾����
	bool AddInk(pdf_page* pageObj, char* archive, int32 bufSize);
	// ��pdf_annotation����װ�뵽������
	bool LoadInk(pdf_annot* annot);

	// ��ȡǩ�����ڲ�ֵ
	void GetSignature(ULONG& checksum, USHORT& altData);

	pdf_annot* GetPdfObj() {
		return mPdfRawObj;
	}

	int32 DetectIntersection(ED_POINT* strokes, int32 pointCount); // ���غ͵ڼ����켣�ཻ

public:
	~CpdfHlAnnot();


	virtual uint32 GetType();	// EDPDF_ANNOT_INK ,EDPDF_ANNOT_UNDERL, EDPDF_ANNOT_DELETE,EDPDF_ANNOT_HIGHLIGHT, EDPDF_ANNOT_TEXT
	virtual char* GetTypeName();		// "ink" "text" "highlight" "underline" "deleteline" or "Identity"

	IEdTextAnnot_ptr ConvertToTextAnnot(void) { return NULL; }
	IEdInkAnnot_ptr ConvertToInkAnnot(void) { return NULL; }
	IEdHighLightAnnot_ptr ConvertToHighLightAnnot(void) { return this; }
	IEdDeleteLineAnnot_ptr ConvertToDeleteLineAnnot(void) { return NULL; }
	IEdUnderLineAnnot_ptr ConvertToUnderLineAnnot(void) { return NULL; }

	uint32 SaveToAchive(buf_ptr buf, uint32 bufSize);	// ���˶��󱣴浽�浵���������Ӵ浵�ָ��浽��Page(ͨ������IEdPage�ķ��� LoadAnnotFromArchive)

	void SetColor(ED_COLOR clr);		// ����������ɫ
	ED_COLOR GetColor(void);

	void SetBorder(int32 border);	// ����������� 1 ~ N
	int32 GetBorder(void);

};



#endif//_HIGHLIGHT_H_