/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */

#ifndef _INKANNOT_H_
#define _INKANNOT_H_
#include "PdfmImp.h"
#include "cmmstruct.h"

class CpdfInkStroke {
public:
	float32 x, y;
	CpdfInkStroke() {
	}

	CpdfInkStroke(const CpdfInkStroke& src) {
		*this = src;
	}

	void operator = (const CpdfInkStroke& src) {
		x = src.x;
		y = src.y;
	}
	void operator =(const fz_point& pt) {
		x = pt.x;
		y = pt.y;
	}
};

typedef cmmVector<CpdfInkStroke, 0, 128> CpdfInkStrokeList;
typedef cmmVector<CpdfInkStrokeList*, 0, 16> CpdfInkMultiList;

DECLARE_BUILTIN_NAME(CpdfInkAnnot)
class CpdfInkAnnot : public cmmBaseObject<CpdfInkAnnot, IEdInkAnnot, GET_BUILTIN_NAME(CpdfInkAnnot)>
{
	friend 	class CpdfPage;


protected:
	// �ڲ�����
	pdf_annot* mPdfRawObj;
	ED_POINT mPosition;
	float mColor[4];
	int mColorN;
	float mBorder;

	cmmVector<CpdfInkStrokeList*, 0, 16> mPdfStrokeLists;	// ���pdf�ļ�װ��ıʼ�����

	CpdfInkAnnot();

	// ��ʼ������������ʵ�ָ��ֲ�ͬ�����ĳ�ʼ��������ע�⣬����������InitOnCreate������һ��Ҫ���û����InitOnCreate����
	// ����0��ʾ�ɹ�������ֵ���λΪ1��ʾ�������ش���Ӧ����ֹ��ʼ�����̣����صľ��Ǵ����룻��������ֵ��ʾ�����Ǵ��󷵻���
	ULONG InitOnCreate();

	DEFINE_CUMSTOMIZE_CREATE(CpdfInkAnnot, (), ())

	// ��ҳ�����INK Annotation����������Stroke List����װ�뱾����
	bool PdfCreateAnnot(pdf_page* pageObj, CpdfInkMultiList& strokeList, float* clr, int clrN, float32 border);
	// �Ӵ浵װ��INK Annotation���������Դ浵����װ�뱾����
	bool LoadFromAchive(pdf_page* pageObj, char* archive, int32 bufSize);
	// ��pdf_annotation����װ�뵽������
	bool Load(pdf_annot* annot);

	// ��ȡǩ�����ڲ�ֵ
	void GetSignature(ULONG& checksum, USHORT& altData);

	pdf_annot* GetPdfObj() {
		return mPdfRawObj;
	}

	int32 DetectIntersection(ED_POINTF* strokes, int32 pointCount); // ���غ͵ڼ����켣�ཻ

public:
	~CpdfInkAnnot();


	virtual uint32 GetType();	// EDPDF_ANNOT_INK ,EDPDF_ANNOT_UNDERL, EDPDF_ANNOT_DELETE,EDPDF_ANNOT_HIGHLIGHT, EDPDF_ANNOT_TEXT
	virtual char* GetTypeName();		// "ink" "text" "highlight" "underline" "deleteline" or "Identity"

	IEdTextAnnot_ptr ConvertToTextAnnot(void) { return NULL; }
	IEdInkAnnot_ptr ConvertToInkAnnot(void) { return this; }
	IEdStextAnnot_ptr ConvertToStextAnnot(void) { return NULL; }

	uint32 SaveToAchive(buf_ptr buf, uint32 bufSize);	// ���˶��󱣴浽�浵���������Ӵ浵�ָ��浽��Page(ͨ������IEdPage�ķ��� LoadAnnotFromArchive)

	void SetColor(ED_COLOR clr);		// ����������ɫ
	ED_COLOR GetColor(void);

	void SetBorder(int32 border);	// ����������� 1 ~ N
	int32 GetBorder(void);

};

class CpdfSmoothMethod {
public:
	CpdfSmoothMethod(CpdfInkStrokeList* list, float threshold);

	static bool SmoothStroke(CpdfInkStrokeList& mStrokePoints, float threshold = 5.0f, float nfPenWidth = 1.0f);	// 2 - 15�ȽϺ���

	//����B�������߽�������㷨����������ƽ��,nbisCloseΪtrue��ʾ�Ǳպ�����
	//static bool SmoothStrokeNew(CpdfInkStrokeList& rStrokePoints, bool nbisClose = false);

	static float DistancePointToLine(float x, float y, float x1, float y1, float x2, float y2);
	//��1����2���͵�2����3���߶��Ƿ��ص�  
	static int LineIntersectSide(float x, float y, float x1, float y1, float x2, float y2);
private:
	CpdfInkStrokeList* data;
	float mSmoothThreshold;

	void SmoothRecurs(int32 pb, int32 pe);
};



#endif//_INKANNOT_H_