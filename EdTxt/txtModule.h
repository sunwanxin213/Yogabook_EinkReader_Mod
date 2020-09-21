/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#ifndef _EDPDFIMP_H_
#define _EDPDFIMP_H_
#include "txtmImp.h"
#include "GdipStart.h"


DECLARE_BUILTIN_NAME(CEdtxtModule)
class CEdtxtModule : public cmmBaseObject<CEdtxtModule, IEdModule, GET_BUILTIN_NAME(CEdtxtModule)>
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
	static CEdtxtModule* GetUniqueObject(void);

public:
	// Ψһʵ��
	static CEdtxtModule* glModule;

protected:
	// �ڲ�����
	//CGdipStart mGdipStart;

	CEdtxtModule();
	~CEdtxtModule();

	DEFINE_CUMSTOMIZE_CREATE(CEdtxtModule, (), ())

	// ��ʼ������������ʵ�ָ��ֲ�ͬ�����ĳ�ʼ��������ע�⣬����������InitOnCreate������һ��Ҫ���û����InitOnCreate����
	// ����0��ʾ�ɹ�������ֵ���λΪ1��ʾ�������ش���Ӧ����ֹ��ʼ�����̣����صľ��Ǵ����룻��������ֵ��ʾ�����Ǵ��󷵻���
	ULONG InitOnCreate();

};



#endif//_EDPDFIMP_H_