/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#ifndef _EDPDFIMP_H_
#define _EDPDFIMP_H_
#include "smtmImp.h"

#include "BaseUtil.h"
#include <wininet.h>
#include "WinDynCalls.h"
#include "CryptoUtil.h"
#include "DirIter.h"
#include "Dpi.h"
#include "FileUtil.h"
#include "FileWatcher.h"
//#include "FrameRateWnd.h"
#include "GdiPlusUtil.h"
//#include "LabelWithCloseWnd.h"
#include "HtmlParserLookup.h"
#include "HttpUtil.h"
//#include "Mui.h"
//#include "SplitterWnd.h"
#include "SquareTreeParser.h"
#include "ThreadUtil.h"
#include "UITask.h"
#include "WinUtil.h"
// rendering engines
#include "BaseEngine.h"
#include "PsEngine.h"
#include "Engineoverride.h"
#include "Doc.h"
#include "FileModifications.h"
#include "PdfCreator.h"

#include "GdipStart.h"

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



DECLARE_BUILTIN_NAME(CSmtModule)
class CSmtModule : public cmmBaseObject<CSmtModule, IEdModule, GET_BUILTIN_NAME(CSmtModule)>
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
	int32Eink GetTypeCount(void);
	// ��ȡ֧�ֵ��ļ���ʽ����չ��
	const char16_ptr GetTypeName(int32Eink index);

	// ���ĵ�
	ED_ERR OpenDocument(
		IN char16_ptr pathName,
		OUT IEdDocument_ptr* documentPtrPtr
	);

	// ���Ψһ����
	static CSmtModule* GetUniqueObject(void);

public:
	// Ψһʵ��
	static CSmtModule* glModule;

protected:
	// �ڲ�����
	CGdipStart mGidpStart;

	CSmtModule();
	~CSmtModule();

	DEFINE_CUMSTOMIZE_CREATE(CSmtModule, (), ())

	// ��ʼ������������ʵ�ָ��ֲ�ͬ�����ĳ�ʼ��������ע�⣬����������InitOnCreate������һ��Ҫ���û����InitOnCreate����
	// ����0��ʾ�ɹ�������ֵ���λΪ1��ʾ�������ش���Ӧ����ֹ��ʼ�����̣����صľ��Ǵ����룻��������ֵ��ʾ�����Ǵ��󷵻���
	ULONG InitOnCreate();

};



#endif//_EDPDFIMP_H_