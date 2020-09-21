/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */

#include "windows.h"
#include "pdfImg.h"
#include "pdfDocument.h"

//////////////////////////////////////////////////////////////////////////
// Bitmap��
DEFINE_BUILTIN_NAME(CpdfBitmap)


CpdfBitmap::CpdfBitmap()
{
	mfzImage = NULL;
}

CpdfBitmap::~CpdfBitmap()
{
	if(mfzImage != NULL)
		fz_drop_pixmap(CEdpdfModule::GetUniqueObject()->fzContext, mfzImage);
}

// ��ʼ������������ʵ�ָ��ֲ�ͬ�����ĳ�ʼ��������ע�⣬����������InitOnCreate������һ��Ҫ���û����InitOnCreate����
// ����0��ʾ�ɹ�������ֵ���λΪ1��ʾ�������ش���Ӧ����ֹ��ʼ�����̣����صľ��Ǵ����룻��������ֵ��ʾ�����Ǵ��󷵻���
ULONG CpdfBitmap::InitOnCreate(fz_pixmap* fzImage)
{
	mfzImage = fzImage;
	mWidth = fz_pixmap_width(CEdpdfModule::GetUniqueObject()->fzContext,mfzImage);
	mHeight = fz_pixmap_height(CEdpdfModule::GetUniqueObject()->fzContext,mfzImage);
	fz_pixmap_components(CEdpdfModule::GetUniqueObject()->fzContext, mfzImage);

	return EDERR_SUCCESS;
}

bin_ptr CpdfBitmap::GetBuffer()
{
	return fz_pixmap_samples(CEdpdfModule::GetUniqueObject()->fzContext, mfzImage);
}

int32 CpdfBitmap::GetWidth()
{
	return mWidth;
}

int32 CpdfBitmap::GetHeight()
{
	return mHeight;
}

