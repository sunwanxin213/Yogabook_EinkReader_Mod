/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#pragma once
#include "EdDoc.h"


class CEdViewCtl
{
public:
	CEdViewCtl();
	~CEdViewCtl();

	void SetViewPort(const ED_SIZE& viewPort);
	void SetUserRatio(float32 userRatio);
	void SetImageInit(const ED_SIZE& imageInit);
	void SetCenterGap(float32 gap)	{
		mGap = gap;
	}
	void Move(const ED_POINT& offset);
	void MoveTo(const ED_POINT& postion);
	void AdjustImageRealSize(const ED_SIZE& imageReal) {
		mImageReal.x = imageReal.x;
		mImageReal.y = imageReal.y;
	}

	const ED_SIZE& GetViewPort(void) {
		return mViewPort;
	}
	float32 GetUserRatio(void) {
		return mUserRatio;
	}
	const ED_SIZE& GetImageInit(void) {
		return mImageInit;
	}

	float32 GetRealRatio();
	float32 GetBaseRatio();
	float32 GetFatRatio();
	bool GetViewMapArea(ED_RECT& destArea, ED_RECT& srcArea, ED_SIZEF& imageExtSize,UCHAR* edgeImpact=NULL); // ���ͼ����ʾ����������true�����򷵻�false
	

	void ImageInitToView(IN ED_RECTF& rectInit, OUT ED_RECTF& rectView);
	bool ViewToImageInit(int viewX, int viewY, ED_POINTF& ptInInit);
	//��ʾ����ת��Ϊ�ĵ�����
	bool ViewToImageInit(int viewX, int viewY, ED_POINT& ptInInit);
	//�ĵ�����ת��Ϊ��ʾ����
	bool ImageToViewInit(int imageX, int imageY, ED_POINT& ptInInit);
	bool ImageToViewInit(int imageX, int imageY, ED_POINTF& ptInInit);

	bool IsInPage2(ED_POINT& ptInInit, ED_POINT& ptInPage,bool nbView = false);	// ��ҳ�洦��˫ҳ��ʾʱ���ã��жϵ��Ƿ��ڵڶ�ҳ�У�����ture���ڵڶ�ҳ��������Ȼ�ڵ�һҳ��ptInPage����ҳ�ڵ�����ֵ
	bool IsInPage2(ED_POINTF& ptInInit, ED_POINTF& ptInPage, bool nbView = false);	// ��ҳ�洦��˫ҳ��ʾʱ���ã��жϵ��Ƿ��ڵڶ�ҳ�У�����ture���ڵڶ�ҳ��������Ȼ�ڵ�һҳ��ptInPage����ҳ�ڵ�����ֵ

protected:
	ED_SIZE mViewPort;	// �ӿڴ�С
	ED_POINTF mFocusOn;	// ����
	ED_SIZE mImageInit; // ͼ���ʼ��С
	float32 mGap;		// ˫ҳ��ʾģʽ�µ��м�ķ�϶�Ŀ��
	ED_SIZE mImageReal;	// ͼ���ʵ�ʴ�С
	float32 mBaseRatio;	// �����Ŵ��������������������û��Ŵ������1.0
	float32 mUserRatio;	// �û��趨�ķŴ���
	float32 mRealRatio;	// �������û��Ŵ����ϲ���
	float32 mFatRatio;	// ռ����Ļ��Ҫ����С�Ŵ����
	bool32 mViewPortModified;
	bool32 mUserRatioModified;
	bool32 mImageInitModified;

	ED_RECT mVisibleRectOnView;
	ED_RECT mVisibleRectOnSource;

	bool32 Calculate(void);
};

