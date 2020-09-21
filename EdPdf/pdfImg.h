/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */

#ifndef _EDPDFIM_H_
#define _EDPDFIM_H_
#include "PdfmImp.h"



DECLARE_BUILTIN_NAME(CpdfBitmap)
class CpdfBitmap : public cmmBaseObject<CpdfBitmap, IEdBitmap, GET_BUILTIN_NAME(CpdfBitmap)>
{
	friend 	class CpdfPage;

public:
	bin_ptr GetBuffer();
	int32 GetWidth();
	int32 GetHeight();

protected:
	// �ڲ�����
	int32 mWidth;
	int32 mHeight;
	fz_pixmap* mfzImage;

	CpdfBitmap();
	~CpdfBitmap();

	DEFINE_CUMSTOMIZE_CREATE(CpdfBitmap, (fz_pixmap* fzImage), (fzImage))

	// ��ʼ������������ʵ�ָ��ֲ�ͬ�����ĳ�ʼ��������ע�⣬����������InitOnCreate������һ��Ҫ���û����InitOnCreate����
	// ����0��ʾ�ɹ�������ֵ���λΪ1��ʾ�������ش���Ӧ����ֹ��ʼ�����̣����صľ��Ǵ����룻��������ֵ��ʾ�����Ǵ��󷵻���
	ULONG InitOnCreate(fz_pixmap* fzImage);

};



#endif//_EDPDFIM_H_