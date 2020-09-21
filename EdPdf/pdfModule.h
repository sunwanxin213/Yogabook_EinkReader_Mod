/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */

#ifndef _EDPDFIMP_H_
#define _EDPDFIMP_H_
#include "PdfmImp.h"

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



DECLARE_BUILTIN_NAME(CEdpdfModule)
class CEdpdfModule : public cmmBaseObject<CEdpdfModule, IEdModule, GET_BUILTIN_NAME(CEdpdfModule)>
{
	friend 	IEdModule_ptr __stdcall EdGetModule(void);

public:
	//enum ERENDER_STEP {
	//	eRenderBegin = 1,
	//	eRenderParepare = 2,
	//	eRenderRender = 3,
	//	eRenderEnd = 4,
	//	eRenderStop = 5
	//};

	// ��ȡ֧�ֵ��ļ���ʽ����
	int32 GetTypeCount(void);
	// ��ȡ֧�ֵ��ļ���ʽ����չ��
	const char16_ptr GetTypeName(int32 index);

	// ���ĵ�
	ED_ERR OpenDocument(
		IN char16_ptr pathName,
		OUT IEdDocument_ptr* documentPtrPtr
	);


	// ���Ψһ����
	static CEdpdfModule* GetUniqueObject(void);

	auto GetFzContext(void) {
		return fzContext;
	}

public:
	// Ψһʵ��
	static CEdpdfModule* glModule;
	fz_context* fzContext;
	fz_transition transition;
	fz_colorspace *colorSpace;


protected:
	// �ڲ�����
	CExclusiveAccess mFzCtxLock[FZ_LOCK_MAX];

	CEdpdfModule();
	~CEdpdfModule();

	DEFINE_CUMSTOMIZE_CREATE(CEdpdfModule, (), ())

	// ��ʼ������������ʵ�ָ��ֲ�ͬ�����ĳ�ʼ��������ע�⣬����������InitOnCreate������һ��Ҫ���û����InitOnCreate����
	// ����0��ʾ�ɹ�������ֵ���λΪ1��ʾ�������ش���Ӧ����ֹ��ʼ�����̣����صľ��Ǵ����룻��������ֵ��ʾ�����Ǵ��󷵻���
	ULONG InitOnCreate();

};



#endif//_EDPDFIMP_H_