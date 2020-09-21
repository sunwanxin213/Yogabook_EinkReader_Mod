/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */

#ifndef _PDFSTEXTLIST_H_
#define _PDFSTEXTLIST_H_
#include "PdfmImp.h"
#include "cmmstruct.h"



DECLARE_BUILTIN_NAME(CpdfStextQuadsList)
class CpdfStextQuadsList : public cmmBaseObject<CpdfStextQuadsList, IEdStextQuadList, GET_BUILTIN_NAME(CpdfStextQuadsList)>
{
protected:
	// �ڲ�����
	cmmVector<ED_RECTF,16,16> mQuads;
	//ED_POINTF mAPoint, mBPoint;


	CpdfStextQuadsList();

	bool HasIntersection(const ED_RECTF& a, const ED_RECTF& b);
	bool IsSameLine(const ED_RECTF& a, const ED_RECTF& b);

public:
	~CpdfStextQuadsList();
	// ��ʼ������������ʵ�ָ��ֲ�ͬ�����ĳ�ʼ��������ע�⣬����������InitOnCreate������һ��Ҫ���û����InitOnCreate����
	// ����0��ʾ�ɹ�������ֵ���λΪ1��ʾ�������ش���Ӧ����ֹ��ʼ�����̣����صľ��Ǵ����룻��������ֵ��ʾ�����Ǵ��󷵻���
	ULONG InitOnCreate(void);

	DEFINE_CUMSTOMIZE_CREATE(CpdfStextQuadsList, (void), ())

	void AddQuad(const ED_RECTF& quad);
	void AddQuad(const fz_quad& quad);
	bool Combination(IEdStextQuadList_ptr addition);

	// ���A�㣨��ֹ�㣩
	virtual void GetAPoint(
		OUT ED_POINTF* pt
	);

	// ���B�㣨��ֹ�㣩
	virtual void GetBPoint(
		OUT ED_POINTF* pt
	);

	// ��÷ֿ�����
	virtual int32Eink GetQuadCount(void);

	// ���һ���ֿ��λ��
	virtual void GetQuadBound(
		IN int32Eink index,
		OUT ED_RECTF_PTR quad
	);

	virtual const ED_RECTF* GetQuad(IN int32Eink index);

	int32 DetectIntersection(IEdStextQuadList_ptr checkWith); // ���غͱ�ʵ���ĵڼ���Quad�ཻ������-1��ʾ���ཻ
	int32 DetectIntersection(IEdStextQuadList_ptr quadList, const ED_RECTF& quad);

};



#endif//_PDFSTEXTLIST_H_