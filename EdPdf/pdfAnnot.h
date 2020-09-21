/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */

#ifndef _PDFANNOT_H_
#define _PDFANNOT_H_
#include "PdfmImp.h"
#include "vector"

using namespace std;

DECLARE_BUILTIN_NAME(CpdfdAnnot)
class CpdfdAnnot : public cmmBaseObject<CpdfdAnnot, IEdAnnot, GET_BUILTIN_NAME(CpdfdAnnot)>
{
	friend 	class CpdfPage;


protected:
	// �ڲ�����

	CpdfdAnnot();
	~CpdfdAnnot();

	// ��ʼ������������ʵ�ָ��ֲ�ͬ�����ĳ�ʼ��������ע�⣬����������InitOnCreate������һ��Ҫ���û����InitOnCreate����
	// ����0��ʾ�ɹ�������ֵ���λΪ1��ʾ�������ش���Ӧ����ֹ��ʼ�����̣����صľ��Ǵ����룻��������ֵ��ʾ�����Ǵ��󷵻���
	ULONG InitOnCreate(void);

	DEFINE_CUMSTOMIZE_CREATE(CpdfdAnnot, (void), ())

public:
	virtual uint32 GetType() {
		return EDPDF_ANNOT_NONE;
	}
	virtual char* GetTypeName() {
		return "identity";
	}

	IEdTextAnnot_ptr ConvertToTextAnnot(void) { return NULL; }
	IEdInkAnnot_ptr ConvertToInkAnnot(void) { return NULL; }
	IEdStextAnnot_ptr ConvertToStextAnnot(void) { return NULL; }

	virtual uint32 SaveToAchive(buf_ptr buf, uint32 bufSize);	// ���˶��󱣴浽�浵���������Ӵ浵�ָ��浽��Page(ͨ������IEdPage�ķ��� LoadAnnotFromArchive)
	virtual void Remove(void) {};		// ���ļ���ɾ���������ɾ���󣬱��ӿ�ҳ����Ҫ����Release�ͷ�

};

class CpdfStextPage;
class CpdfPage;

DECLARE_BUILTIN_NAME(CpdfdAnnotList)
class CpdfdAnnotList : public cmmBaseObject<CpdfdAnnotList, IEdAnnotList, GET_BUILTIN_NAME(CpdfdAnnotList)>
{
	friend CpdfPage;
	friend CpdfStextPage;

protected:
	// �ڲ�����
	vector<IEdAnnot_ptr> mAnnots;

	void AddAnnot(IEdAnnot_ptr annot);
	void AddAnnots(vector<IEdAnnot_ptr> annots);

	CpdfdAnnotList();

public:
	~CpdfdAnnotList();

	// ��ʼ������������ʵ�ָ��ֲ�ͬ�����ĳ�ʼ��������ע�⣬����������InitOnCreate������һ��Ҫ���û����InitOnCreate����
	// ����0��ʾ�ɹ�������ֵ���λΪ1��ʾ�������ش���Ӧ����ֹ��ʼ�����̣����صľ��Ǵ����룻��������ֵ��ʾ�����Ǵ��󷵻���
	ULONG InitOnCreate(void);

	DEFINE_CUMSTOMIZE_CREATE(CpdfdAnnotList, (void), ())

	virtual int32Eink GetCount(void);
	virtual IEdAnnot_ptr GetAnnot(int32Eink index);	// ���صĶ�����Ҫ�ͷ�

};



#endif//_PDFANNOT_H_