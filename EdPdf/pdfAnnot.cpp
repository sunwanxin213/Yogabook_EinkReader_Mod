/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */

#include "windows.h"
#include "pdfannot.h"

//////////////////////////////////////////////////////////////////////////
// Annot��
DEFINE_BUILTIN_NAME(CpdfdAnnot)


CpdfdAnnot::CpdfdAnnot()
{
}

CpdfdAnnot::~CpdfdAnnot()
{

}

// ��ʼ������������ʵ�ָ��ֲ�ͬ�����ĳ�ʼ��������ע�⣬����������InitOnCreate������һ��Ҫ���û����InitOnCreate����
// ����0��ʾ�ɹ�������ֵ���λΪ1��ʾ�������ش���Ӧ����ֹ��ʼ�����̣����صľ��Ǵ����룻��������ֵ��ʾ�����Ǵ��󷵻���
ULONG CpdfdAnnot::InitOnCreate(void)
{

	return EDERR_SUCCESS;
}

uint32 CpdfdAnnot::SaveToAchive(buf_ptr buf, uint32 bufSize)
{
	return NULL;
}








//////////////////////////////////////////////////////////////////////////
// Annot List ��
DEFINE_BUILTIN_NAME(CpdfdAnnotList)


CpdfdAnnotList::CpdfdAnnotList()
{
}

CpdfdAnnotList::~CpdfdAnnotList()
{
	for (auto i : mAnnots)
	{
		i->Release();
	}
}

// ��ʼ������������ʵ�ָ��ֲ�ͬ�����ĳ�ʼ��������ע�⣬����������InitOnCreate������һ��Ҫ���û����InitOnCreate����
// ����0��ʾ�ɹ�������ֵ���λΪ1��ʾ�������ش���Ӧ����ֹ��ʼ�����̣����صľ��Ǵ����룻��������ֵ��ʾ�����Ǵ��󷵻���
ULONG CpdfdAnnotList::InitOnCreate(void)
{
	return EDERR_SUCCESS;
}

int32Eink CpdfdAnnotList::GetCount(void)
{
	return (int32Eink)mAnnots.size();
}

IEdAnnot_ptr CpdfdAnnotList::GetAnnot(int32Eink index)
{
	mAnnots[index]->AddRefer();
	return mAnnots[index];
}

void CpdfdAnnotList::AddAnnot(IEdAnnot_ptr annot)
{
	annot->AddRefer();
	mAnnots.push_back(annot);
}

void CpdfdAnnotList::AddAnnots(vector<IEdAnnot_ptr> annots)
{
	for (auto i : annots)
	{
		i->AddRefer();
		mAnnots.push_back(i);
	}
}

