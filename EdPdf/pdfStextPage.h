/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */

#ifndef _EDPDFSTEXTPAGEIMP_H_
#define _EDPDFSTEXTPAGEIMP_H_
#include "PdfmImp.h"
#include "pdfImg.h"
#include "pdfAnnot.h"
#include "inkAnnot.h"
#include "vector"

using namespace std;

class CpdfPage;

DECLARE_BUILTIN_NAME(CpdfStextPage)
class CpdfStextPage : public cmmBaseObject<CpdfStextPage, IEdStructuredTextPage, GET_BUILTIN_NAME(CpdfStextPage)>
{
public:
	// ̽���ı�ѡ�����
	virtual bool DetectSelectedText(
		IN const ED_POINTF* aPoint,
		IN const ED_POINTF* bPoint,
		OUT IEdStextQuadList_ptr* stext,	// ���ؽṹ���ı�������Ҫ�ͷ�
		OUT IEdAnnotList_ptr* impactedAnnot,	// ������ײ������Annot�����б���Ҫ�ͷ�
		IN bool32 includeImpacted	// ����ײ��annot����Ҳ����ѡ��
	);	// ���صĶ�����Ҫ�ͷ�

	// ����Ŀ�������ı�������ֵ�Ǹ��Ƶ��ַ�������������β0
	virtual int32Eink CopyText(
		IN ED_RECTF_PTR selBox,
		OUT char16_ptr textBuf,	// ����NULL����������ʵ�ʵ��ַ�����������β0
		IN int32Eink bufSize
	);

	DEFINE_CUMSTOMIZE_CREATE(CpdfStextPage, (CpdfPage* page), (page))

	// ��ʼ������������ʵ�ָ��ֲ�ͬ�����ĳ�ʼ��������ע�⣬����������InitOnCreate������һ��Ҫ���û����InitOnCreate����
	// ����0��ʾ�ɹ�������ֵ���λΪ1��ʾ�������ش���Ӧ����ֹ��ʼ�����̣����صľ��Ǵ����룻��������ֵ��ʾ�����Ǵ��󷵻���
	ULONG InitOnCreate(CpdfPage* page);
protected:
	// �ڲ�����
	CpdfPage* mNotStextPage;
	//fz_page *mFzPage;
	fz_stext_page* mStextPage;
	
	CpdfStextPage();
	~CpdfStextPage();


};



#endif//_EDPDFSTEXTPAGEIMP_H_