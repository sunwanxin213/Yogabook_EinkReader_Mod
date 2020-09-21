/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#include "windows.h"
#include "txtPage.h"
#include "txtDocument.h"
#include "gdiplus.h"


using namespace Gdiplus;

//////////////////////////////////////////////////////////////////////////
// class CTxdArrangeThread
//////////////////////////////////////////////////////////////////////////
volatile LONG CTxdArrangeThread::mGlobalThreadNumber = 0;


CTxdArrangeThread::CTxdArrangeThread(const wchar_t* charBuffer, uint32 charStart, uint32 charEnd, bool aheadPages, const wchar_t* fontName, float fontSize, Gdiplus::RectF* viewPort, PEDDOC_THREAD_CALLBACK callBackFun, void* callBackContext)
{
	mThreadNumber = InterlockedIncrement(&mGlobalThreadNumber);
	mResult = EDERR_UNSUCCESSFUL;

	mCharBuffer = charBuffer;
	mCharStart = charStart;
	mCharEnd = charEnd;

	mViewPortExt = *viewPort;
	mViewPortExt.Height *= 2.0f;
	mViewHeight = viewPort->Height;

	mCallBackContext = callBackContext;
	mCallBackFun = callBackFun;
	mAheadArrange = aheadPages;
	mExitFlag = 0;
	mPageLoadStep = 0;
	mThread2 = NULL;

	wcscpy_s(mFontName, 100, fontName);
	mFontSize = fontSize;

	if (mAheadArrange == false)
		mFocusPageLoaded = CreateEvent(NULL, true, false, NULL);
	else
		mFocusPageLoaded = NULL;

	mFontObj = NULL;
	mGdiObj = NULL;

	mThreadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)CTxdArrangeThread::ThreadBridge, this, CREATE_SUSPENDED, &mThreadID);
	if (mThreadHandle == NULL)
		return;

	mResult = EDERR_SUCCESS;
}


CTxdArrangeThread::~CTxdArrangeThread()
{

	TerminateThread();

	if (mThread2 != NULL)
		delete mThread2;

	if (mFocusPageLoaded != NULL)
		CloseHandle(mFocusPageLoaded);

	if (mThreadHandle != NULL)
		CloseHandle(mThreadHandle);
}


void CTxdArrangeThread::Start(void)
{
	ResumeThread(mThreadHandle);

	if (mFocusPageLoaded != NULL)
	{
		if (IsDebuggerPresent() != FALSE)
			WaitForSingleObject(mFocusPageLoaded, INFINITE);
		else
			WaitForSingleObject(mFocusPageLoaded, 10 * 1000);	// 10�볬ʱ
	}
}

void CTxdArrangeThread::TerminateThread()
{	// will wait the thread exit and set a timeout as 10 seconds
	if (mThread2 != NULL)
		mThread2->TerminateThread();

	InterlockedExchange(&mExitFlag, 1);
	if (WaitForSingleObject(mThreadHandle, 10 * 1000) == WAIT_TIMEOUT)
		::TerminateThread(mThreadHandle, -1);	// ֱ����ֹ
}

ULONG WINAPI CTxdArrangeThread::ThreadBridge(CTxdArrangeThread* thisPointer)
{

	if (thisPointer->InitGdip() == false)
		return -1;

	try {
		if (thisPointer->mAheadArrange == false)
			thisPointer->ArrangeThreadRoutine();
		else
			thisPointer->ArrangeThreadRoutine4AheadPages();
	}
	catch (...)
	{
	}

	thisPointer->UninitGdip();

	return 0;
}

bool CTxdArrangeThread::InitGdip(void)
{
	mGdiStart.Init();

	mGdiObj = new Gdiplus::Graphics(GetDC(NULL));
	if (mGdiObj == NULL)
		return false;

	mFontObj = new Gdiplus::Font(mFontName, mFontSize);

	return mFontObj != NULL;
}

void CTxdArrangeThread::UninitGdip(void)
{
	if (mFontObj != NULL)
		delete mFontObj;

	if (mGdiObj != NULL)
		delete mGdiObj;
	mGdiStart.UnInit();
}

void CTxdArrangeThread::ArrangeThreadRoutine()
{

	bool eventReleased = false;

	// ��ʼ�Ű�

	try
	{
		// ����һ�α�ʾ��ʼ
		mCallBackFun(mThreadNumber, 0, 0, mCallBackContext);

		//////////////////////////////////////////////////////////////////////////
		// �����Ų�ǰ��ҳ
		if (mCharStart > mCharEnd)
			mCharStart = 0;

		ArrangePages(2,mCharStart,mCharEnd);

		//////////////////////////////////////////////////////////////////////////
		// ֪ͨ�ⲿ�����첽ִ����
		SetEvent(mFocusPageLoaded);
		eventReleased = true;

		if (mExitFlag != 0)
			THROW_FALSE;

		//////////////////////////////////////////////////////////////////////////
		// �����Ե�ǰҳ֮ǰҳ�Ű�ĵڶ��߳�
		if (mCharStart > 0)
		{
			Gdiplus::RectF rectViewPort;
			rectViewPort = mViewPortExt;
			rectViewPort.Height = mViewHeight;
			mThread2 = new CTxdArrangeThread(mCharBuffer,0,mCharStart,true,mFontName,mFontSize,&rectViewPort,mCallBackFun,mCallBackContext);

			mThread2->Start();
		}

		//////////////////////////////////////////////////////////////////////////
		// �����Ų���ǰҳ�ĺ���ҳ
		ArrangePages(0,mPageCtl.charBegin, mCharEnd);

		if (mExitFlag != 0)
			THROW_FALSE;

		//////////////////////////////////////////////////////////////////////////
		// �ȴ��ڶ��̷߳��أ����ȴ�30����
		//if (mThread2 != NULL)
		//{
		//	//niuxiaojia ��ǰҳǰ���ҳ��Ҳ��̬��ʾ
		//	int liWaitCount = 0;
		//	while (WaitForSingleObject(mThread2->mThreadHandle, 2 * 1000))
		//	{
		//		mCallBackFun(mThreadNumber, 0xCFFFFFFF, (PST_PAGE_CONTROL)&mThread2->mPages, mCallBackContext);

		//		if (liWaitCount++ > 900)
		//			THROW_FALSE;	// ��ʱ�ģ�������ʧ�ܣ��������ݶ���ʹ��
		//	}
		//}
		


		if (mThread2 != NULL)
		{
			if (WaitForSingleObject(mThread2->mThreadHandle, 30 * 60 * 1000) == WAIT_TIMEOUT)
				THROW_FALSE;	// ��ʱ�ģ�������ʧ�ܣ��������ݶ���ʹ��

			//}
			// ��Ϊһ���Դ���ȥ��ע��˴���(PST_PAGE_CONTROL)&mThread2->mPages���ǽ��������ָ�봫���˲�ͬ���͵�����
			mCallBackFun(mThreadNumber, LOADING_STEP_AHEAD_COMPLETE,(PST_PAGE_CONTROL)&mThread2->mPages, mCallBackContext);

			//delete mThread2;
		}

	}
	catch (...)
	{
	}

	if (eventReleased == false)
	{
		SetEvent(mFocusPageLoaded);
		eventReleased = true;
	}

	// ����һ�α�ʾ����
	mCallBackFun(mThreadNumber,MAXULONG32, 0, mCallBackContext);

}

void CTxdArrangeThread::ArrangeThreadRoutine4AheadPages()
{	// �Ű����Ӿ�����ҳ֮ǰ��ҳ��

	// ���ȼ���Լ���Ҫ�Ű���������Ƿ�������̫�����ٴη���

	// ���ڿ�ʼ�Ų���ǰҳ֮ǰ��ҳ��
	ArrangePages(0,mCharStart,mCharEnd);

	// ����Ƿ����֮ǰ�ĵ�����һҳ���ݹ��ٵ����
	if (mPages.Size() >= 2 && mExitFlag==0)
	{
		Gdiplus::RectF boundRect;
		Gdiplus::PointF leftTop;

		leftTop.X = leftTop.Y = 0.0f;
		if (mGdiObj->MeasureString(mPages.Back().charBegin + mCharBuffer, mPages.Back().charCount, mFontObj,mViewPortExt, &boundRect) == Ok && boundRect.Height < mViewHeight / 2.0f)
		{
			// �������ڶ�ҳ��С1/4����������ݼ�����һҳ��������һҳ
			auto x1 = mPages[mPages.Size() - 2].charCount;
			auto x2 = mPages[mPages.Size() - 1].charCount;

			uint32 charCount2Page = mPages[mPages.Size() - 2].charCount + mPages[mPages.Size() - 1].charCount;

			mViewHeight = (mViewHeight * 3) / 4;

			uint32 charCountNewPage1 = FillCharsToPage(mPages[mPages.Size() - 2].charBegin, mCharEnd, 0, 32);
			if (charCountNewPage1 < MAXULONG32)
			{
				mPages[mPages.Size() - 2].charCount = charCountNewPage1;
				mPages[mPages.Size() - 1].charBegin = mPages[mPages.Size() - 2].charBegin + mPages[mPages.Size() - 2].charCount;
				mPages[mPages.Size() - 1].charCount = charCount2Page - charCountNewPage1;
			}
		}
	}
	// �˳��ڶ��߳�
}

ED_ERR CTxdArrangeThread::ArrangePages(int32 pageRequired, uint32 stringBase, uint32 stringEnd)
{
	try {

		mPageCtl.charBegin = stringBase;
		mPageCtl.charCount = 0;

		uint32 charCounted = 0;
		uint32 countTimes = 0;
		uint32 searchBase = 0;

		int32 pageLoaded = 0;
		//int32 pageIndex;
		//int32 pageCount;


		while (mPageCtl.charBegin < stringEnd && mExitFlag == 0)
		{
			mPageCtl.charCount = FillCharsToPage(mPageCtl.charBegin, stringEnd, searchBase, 32);
			if (mPageCtl.charCount == searchBase)	// ˵�����ԵĻ���ַ�Ѿ�����˱�ҳ�Ĵ�С���ٴ���ǰ��1024����λ���п���ֱ���˵�0��ʼ�����������
			{
				if (searchBase > 1024)	// ȷ����ǰ��Ծ1024�󣬻���������
					mPageCtl.charCount = FillCharsToPage(mPageCtl.charBegin, stringEnd, searchBase - 1024, 32);

				if (mPageCtl.charCount == searchBase)	// ����Խ���ˣ����ͷ��ʼ����
					mPageCtl.charCount = FillCharsToPage(mPageCtl.charBegin, stringEnd, 0, 32);

			}

			charCounted += mPageCtl.charCount;

			if (mAheadArrange != false)
			{
				mPages.Insert(-1, mPageCtl);

				mCallBackFun(mThreadNumber, LOADING_STEP_AHEAD_PREFIX(++mPageLoadStep), NULL, mCallBackContext);
			}
			else
				mCallBackFun(mThreadNumber, ++mPageLoadStep, &mPageCtl, mCallBackContext);

			mPageCtl.charBegin += mPageCtl.charCount;

			// ʹ��ƽ��ҳ������1024�¶��룬��Ϊ�����������Բ���
			searchBase = ((charCounted / (++countTimes)) / 1024) * 1024;

			if (pageRequired > 0 && ++pageLoaded >= pageRequired)
				break;
		}

	}
	catch (...)
	{
	}

	return EDERR_SUCCESS;
}

#define LOW_GRAIN(_X) (_X/32)//(_X>32?32:1)

uint32 CTxdArrangeThread::FillCharsToPage(uint32 stringBase, uint32 stringEnd, uint32 clusterBase, uint32 clusterSize)
{
	CharacterRange clusterCharRanges[32];
	Region clusterRegions[32];
	uint32 crtBoxBase = clusterBase;
	RectF boundOfCluster;
	int32 clusterCount = 0;

	if (mExitFlag != 0)
		return 0;	// �˳��̱߳�־������

	// ���ʣ������С��һ��̽�����ȣ��򽵵�����ȥ̽��
	if ((ULONG)stringBase + clusterBase + clusterSize >= stringEnd)
	{
		if (clusterSize > 1)
			return FillCharsToPage(stringBase, stringEnd, clusterBase, LOW_GRAIN(clusterSize));

		// û�н�һ���Ŀռ��ˣ�ֱ�Ӱѵ�ǰ���ַ������ذ�
		return stringEnd - stringBase;
	}

	//////////////////////////////////////////////////////////////////////////
	//ִ�����̽������
	//////////////////////////////////////////////////////////////////////////

	// ��clusterBase��ʼ����32��̽��cluster
	for (auto &i : clusterCharRanges)   
	{
		if (stringBase + crtBoxBase + clusterSize > stringEnd)
		{
			// �������½�β��
			if (stringBase + crtBoxBase >= stringEnd)
				break;
			crtBoxBase = stringEnd - stringBase - clusterSize;	// �����½�β������һ����Ԫ��ȷ�������ԪҲ�ǿ��Ա�10������
																// ��Ȼ��ᵼ�´˵�Ԫ��ǰһ����Ԫ���غϣ����㷨�ϲ������
		}

		i.First = crtBoxBase;
		i.Length = clusterSize;
		crtBoxBase += clusterSize;

		clusterCount++;	// ������Ч�ĵ�Ԫ��
	}

	StringFormat strFormat;
	strFormat.SetAlignment(StringAlignmentNear);
	if (strFormat.SetMeasurableCharacterRanges(clusterCount, clusterCharRanges) != Ok)
		THROW_FAILED;

	if (mGdiObj->MeasureCharacterRanges(mCharBuffer + stringBase, crtBoxBase,mFontObj, mViewPortExt, &strFormat, clusterCount, clusterRegions) != Ok)
		THROW_FAILED;

	for (int j = 0; j < clusterCount; j++)
	{
		//// ���˲��ɼ��ַ�
		//if (clusterSize == 1 && iswgraph(*(mCharBuffer + stringBase + clusterCharRanges[j].First)) == false)
		//	continue;

		if (clusterRegions[j].GetBounds(&boundOfCluster,mGdiObj) != Ok)
			THROW_FAILED;

		// �����ߴ�Ϊ�����������ж��ǲ��������˲��ɼ��ַ�����
		if (boundOfCluster.Height == 0)
		{
			// ��Ϊ�����趨���Ű�����ܸߣ��統ǰ�趨�Ĵش�СΪ1�����Բ�����ֿɼ��ַ������Ű��������������Ȼ�ǲ��ɼ��ַ����򽫲��ɼ��ַ��ŵ����㵽ǰҳ
			if(clusterSize == 1)
				continue;

			// ����ش���1��һ����32���ַ�����ô�������������ڲ��������Ҳ�������ض��ǲ��ɼ��ַ�; �������ȣ���������
			////return FillCharsToPage(stringBase, stringEnd, clusterCharRanges[j].First, LOW_GRAIN(clusterSize));	//��ʱ�����ȫ�������ַ������ɼ��Ĵ��������������Ҫ����һ�����ʵĽ����ַ�
			return FillCharsToPage(stringBase, /*stringEnd*/stringBase + clusterCharRanges[j].First + clusterCharRanges[j].Length*10, clusterCharRanges[j].First, LOW_GRAIN(clusterSize));	// 10����ǰ���Ժп�ȣ���������ж�Ϊ���ɼ�����ô�ͽ��������������Ϊһ����ҳ����
		}

		// ��������Ե�Ԫ�����ӿڴ�С
		if (/*boundOfCluster.Height == 0 || */boundOfCluster.Y + boundOfCluster.Height > mViewHeight)
		{
			if (clusterSize == 1)// �Ƿ����ȵ���1��
				return clusterCharRanges[j].First;// ��������

			// �������ȣ���һ�����������Ԫ
			//return FillCharsToPage(stringBase, stringEnd, clusterCharRanges[j].First, LOW_GRAIN(clusterSize));
			return FillCharsToPage(stringBase, /*stringEnd*/stringBase+clusterCharRanges[j].First + clusterCharRanges[j].Length, clusterCharRanges[j].First, LOW_GRAIN(clusterSize));
		}
	}

	// �Ƿ񲻴��ں����ַ���
	if (stringBase + crtBoxBase >= stringEnd)
		return crtBoxBase;

	// ���������涼û�г����ӿڣ���ô�����ң��������Ȳ���
	return FillCharsToPage(stringBase, stringEnd, crtBoxBase, clusterSize);
}

// ����Ĵ�����Ӹ�Ч�����Ǵ���һЩbug��������������������
//uint32 CTxdArrangeThread::FillCharsToPage(uint32 stringBase, uint32 stringEnd, uint32 clusterBase, uint32 clusterSize)
//{
//	CharacterRange clusterCharRanges[32];
//	Region clusterRegions[32];
//	uint32 crtBoxBase = clusterBase;
//	RectF boundOfCluster[32];
//	int32 clusterCount = 0;
//
//	if (mExitFlag != 0)
//		return 0;	// �˳��̱߳�־������
//
//					// ���ʣ������С��һ��̽�����ȣ��򽵵�����ȥ̽��
//	if ((ULONG)stringBase + clusterBase + clusterSize >= stringEnd)
//	{
//		if (clusterSize > 1)
//			return FillCharsToPage(stringBase, stringEnd, clusterBase, LOW_GRAIN(clusterSize));
//
//		// û�н�һ���Ŀռ��ˣ�ֱ�Ӱѵ�ǰ���ַ������ذ�
//		return stringEnd - stringBase;
//	}
//
//	//////////////////////////////////////////////////////////////////////////
//	//ִ�����̽������
//	//////////////////////////////////////////////////////////////////////////
//
//	// ��clusterBase��ʼ����32��̽��cluster
//	for (auto &i : clusterCharRanges)
//	{
//		if (stringBase + crtBoxBase + clusterSize > stringEnd)
//		{
//			// �������½�β��
//			if (stringBase + crtBoxBase >= stringEnd)
//				break;
//			crtBoxBase = stringEnd - stringBase - clusterSize;	// �����½�β������һ����Ԫ��ȷ�������ԪҲ�ǿ��Ա�10������
//																// ��Ȼ��ᵼ�´˵�Ԫ��ǰһ����Ԫ���غϣ����㷨�ϲ������
//		}
//
//		i.First = crtBoxBase;
//		i.Length = 1;// clusterSize;
//		crtBoxBase += clusterSize;
//
//		clusterCount++;	// ������Ч�ĵ�Ԫ��
//	}
//
//	StringFormat strFormat;
//	strFormat.SetAlignment(StringAlignmentNear);
//	if (strFormat.SetMeasurableCharacterRanges(clusterCount, clusterCharRanges) != Ok)
//		THROW_FAILED;
//
//	if (mGdiObj->MeasureCharacterRanges(mCharBuffer + stringBase, crtBoxBase, mFontObj, mViewPortExt, &strFormat, clusterCount, clusterRegions) != Ok)
//		THROW_FAILED;
//
//	for (int j = 0; j < clusterCount; j++)
//	{
//		if (clusterRegions[j].GetBounds(&boundOfCluster[j], mGdiObj) != Ok)
//			THROW_FAILED;
//	}
//	for (int j = 0; j < clusterCount; j++)
//	{
//		if (boundOfCluster[j].Height == 0) // �����ǲ��ɼ��ַ�
//			for (int k = j + 1; k < clusterCount; k++)
//				if (boundOfCluster[k].Height != 0)
//				{
//					boundOfCluster[j] = boundOfCluster[k];
//					break;
//				}
//	}
//
//	for (int j = 0; j < clusterCount; j++)
//	{
//		//if (clusterRegions[j].GetBounds(&boundOfCluster,mGdiObj) != Ok)
//		//	THROW_FAILED;
//
//		// ��������Ե�Ԫ�����ӿڴ�С
//		if (boundOfCluster[j].Height == 0 || boundOfCluster[j].Y /*+ boundOfCluster.Height*/ > mViewHeight)
//		{
//			if (clusterSize == 1 || j == 0)// �Ƿ����ȵ���1�ˣ�����û��ǰһ����Ԫ
//				return clusterCharRanges[j].First;// �������ˣ��ⲿ���ж����������һ���ҵ��˷ָ��ַ�������������Ե�Ԫ�������˿�������
//
//												  // �������ȣ���һ������ǰһ����Ԫ
//			return FillCharsToPage(stringBase, stringEnd, clusterCharRanges[j - 1].First, LOW_GRAIN(clusterSize));
//		}
//	}
//
//	// ���������涼û�г����ӿڣ���ô�����ң��������Ȳ���
//	return FillCharsToPage(stringBase, stringEnd, crtBoxBase, clusterSize);
//}
