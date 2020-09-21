/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */

#include "windows.h"
#include "Float.h"
#include "pdfStextQuadList.h"



//////////////////////////////////////////////////////////////////////////
// Annot List ��
DEFINE_BUILTIN_NAME(CpdfStextQuadsList)


CpdfStextQuadsList::CpdfStextQuadsList()
{
	//mAPoint.x = mAPoint.y = mBPoint.x = mBPoint.y = 0.0f;
}

CpdfStextQuadsList::~CpdfStextQuadsList()
{
	//mAPoint.x = 0;
}

// ��ʼ������������ʵ�ָ��ֲ�ͬ�����ĳ�ʼ��������ע�⣬����������InitOnCreate������һ��Ҫ���û����InitOnCreate����
// ����0��ʾ�ɹ�������ֵ���λΪ1��ʾ�������ش���Ӧ����ֹ��ʼ�����̣����صľ��Ǵ����룻��������ֵ��ʾ�����Ǵ��󷵻���
ULONG CpdfStextQuadsList::InitOnCreate(void)
{
	return EDERR_SUCCESS;
}

void CpdfStextQuadsList::GetAPoint(OUT ED_POINTF* pt)
{
	if (mQuads.Size() <= 0)
	{
		pt->x = pt->y = 0.0f;
		return;
	}
	pt->x = mQuads.Front().left;
	pt->y = (mQuads.Front().top + mQuads.Front().bottom) / 2.0f;	// A��ȡ��ߴ�Quad�ĸ߶ȵ�һ��
}

void CpdfStextQuadsList::GetBPoint(OUT ED_POINTF* pt)
{
	if (mQuads.Size() <= 0)
	{
		pt->x = pt->y = 0.0f;
		return;
	}
	pt->x = mQuads.Back().right;
	pt->y = (mQuads.Back().top + mQuads.Back().bottom) / 2.0f;	// B��ȡ��ʹ�Quad�ĸ߶ȵ�һ��λ��
}

int32Eink CpdfStextQuadsList::GetQuadCount(void)
{
	return mQuads.Size();
}

void CpdfStextQuadsList::GetQuadBound(IN int32Eink index, OUT ED_RECTF_PTR quad)
{
	quad->left = mQuads.GetEntry(index).left;
	quad->top = mQuads.GetEntry(index).top;
	quad->right = mQuads.GetEntry(index).right;
	quad->bottom = mQuads.GetEntry(index).bottom;
}

const ED_RECTF* CpdfStextQuadsList::GetQuad(IN int32Eink index)
{
	if (index < 0 || index >= mQuads.Size())
	{
		return NULL;
	}
	return &mQuads[index];
}

//#include "math.h"
//#include "cmmString.h"
void CpdfStextQuadsList::AddQuad(const ED_RECTF& quad)
{
	//if (fabs(quad.left - 48.0f) <= 1.0f && fabs(quad.right - 260.0f) <= 1.0f && fabs(quad.top - 723.0f) <= 1.0f && fabs(quad.bottom - 736.0f) <= 1.0f)
	//	DebugBreak();
	//cmmStringW msg;
	//msg.format(L"[%.0f:%.0f] - [%.0f:%.0f]\n", quad.left,  quad.right,quad.top, quad.bottom);
	//OutputDebugString(msg.ptr());

	mQuads.Insert(-1, quad);
	//mBPoint.x = quad.right;
	//mBPoint.y = (quad.top + quad.bottom) / 2.0f;	// B��ȡ��β��Quad�ĸ߶ȵ�һ��λ��
	//if (mQuads.Size() <= 0)
	//{	// A��ȡ��ͷ��Quad�ĸ߶ȵ�һ��λ��
	//	mAPoint = mBPoint;
	//}

	/*
	modified by Ax. 2020.04.16
	//mAPoint.x = mAPoint.y = FLT_MAX;
	//mBPoint.x = mBPoint.y = 0;

	int i;
	// ����top���ȣ������б�
	for (i = 0; i < mQuads.Size(); i++)
	{
		// �����ж��ǲ���ͬ�У���Ҫ�ж������Y��ͶӰ�Ƿ��ཻ
		if(IsSameLine(mQuads[i],quad))
			continue;// ��ͬ�У����������

		// ����ͬ�У��������i������֮ǰ
		if (mQuads[i].top > quad.top)
			break;
	}

	// �������
	mQuads.Insert(i, quad);

	// ���¼���A/B��
	int a, b;

	//a = b = 0;
	//for (i = 1; i < mQuads.Size(); i++)
	//{
	//	// �����ͬһ��
	//	if (IsSameLine(mQuads[a], mQuads[i]))
	//	{
	//		// �ж��Ƿ������
	//		if (mQuads[a].left > mQuads[i].left)
	//			a = i;
	//	}
	//	else
	//	if (mQuads[i].top < mQuads[a].top)
	//	{	// ��a�����ڵ���ҪС������a��
	//		a = i;
	//	}

	//	// �����ͬһ��
	//	if (IsSameLine(mQuads[b], mQuads[i]))
	//	{
	//		// �ж��Ƿ����ұ�
	//		if (mQuads[b].right < mQuads[i].right)
	//			b = i;
	//	}
	//	else
	//	if (mQuads[i].top > mQuads[b].top)
	//	{	// ��b�����ڵ���Ҫ�󣬸���b��
	//		b = i;
	//	}
	//}
	// ����A/B���ֵ
	//mAPoint.x = mQuads[a].left;
	//mAPoint.y = (mQuads[a].top + mQuads[a].bottom) / 2.0f;	// A��ȡ��ߴ�Quad�ĸ߶ȵ�һ��λ��
	//mBPoint.x = mQuads[b].right;
	//mBPoint.y = (mQuads[b].top + mQuads[b].bottom) / 2.0f;	// B��ȡ��ʹ�Quad�ĸ߶ȵ�һ��λ��
	*/
}

void CpdfStextQuadsList::AddQuad(const fz_quad& quad)
{
	ED_RECTF quadRect;

	quadRect.left =		quad.ul.x;
	quadRect.top =		quad.ul.y;
	quadRect.right =	quad.lr.x;
	quadRect.bottom =	quad.lr.y;

	AddQuad(quadRect);
}

int32 CpdfStextQuadsList::DetectIntersection(IEdStextQuadList_ptr checkWith)
{
	int32 rev = -1;
	for (int i = 0; i < mQuads.Size(); i++)
	{
		rev = DetectIntersection(checkWith, mQuads[i]);
		if (rev > -1)
			break;
	}

	return rev;
}

int32 CpdfStextQuadsList::DetectIntersection(IEdStextQuadList_ptr quadList, const ED_RECTF& quad)
{
	int rev = -1;
	for (int i = 0; i < quadList->GetQuadCount(); i++)
	{
		if (HasIntersection(*quadList->GetQuad(i), quad) != false)
		{
			rev = i;
			break;
		}
	}

	return rev;
}

bool CpdfStextQuadsList::HasIntersection(const ED_RECTF& a, const ED_RECTF& b)
{
	float lfRight = min(a.right, b.right);
	float lfBottom = min(a.bottom, b.bottom);
	float lfLeft = max(a.left, b.left);
	float lfTop = max(a.top, b.top);

	return (lfRight > lfLeft && lfBottom > lfTop);
}

bool CpdfStextQuadsList::IsSameLine(const ED_RECTF& a, const ED_RECTF& b)
{
	int insect = min(a.bottom, b.bottom) - max(a.top, b.top);
	if (insect > 0 && (insect*2 > (a.bottom-a.top) || insect*2 > (b.bottom-b.top)))
		return true;

	return false;
}

bool CpdfStextQuadsList::Combination(IEdStextQuadList_ptr addition)
{
	//ED_RECTF quad;

	//for (int i = 0; i < src->GetQuadCount(); i++)
	//{
	//	src->GetQuadBound(i, &quad);
	//	AddQuad(quad);
	//}
	if (addition->GetQuadCount() <= 0)
		return false;

	// ���Ȳ鿴���Ӷ������λ�Ƿ������ײ
	int impact = DetectIntersection(this, *addition->GetQuad(0));
	if (impact >= 0)
	{	// ����ײ��˵��������ѡ��˳���ϣ����Ӷ������е��ڻ��߳��ڱ���������
		if (impact == 0)	// ��������ĵ�һ�ֿ���ײ������Ҫ�ж�˭����
		{	// ��Ȼ��ײ����Ȼ��ͬһ�У������������
			if (mQuads.Front().left <= addition->GetQuad(0)->left)
				impact = 1;	// ��һ�������
		}

	}
	else
	{// û����ײ��˵��������ѡ��˳���ϣ����������г��ڸ��Ӷ�������
		//CMMASSERT(DetectIntersection(addition, mQuads.Front()) >= 0);
		if (DetectIntersection(addition, mQuads.Front()) < 0)
			return false;	// �����ԭ�������pdf annot���ö����quad��¼�����϶��������Ű���Ⱥ���򣬱�������֮ǰ��bug��Ҳ�ᵼ�²����ļ��д����������

		impact = 0;
	}

	// ���븽�Ӷ�������һ�飬��Ȼ�����������ܻᵼ���ڲ����ݲ������룬������Ҫ��������Ҫ���Ǽ�¼һ�����ǣ�����֤AB����Ч�ͺ�
	for (int i = 0; i < addition->GetQuadCount() - 1; i++)
	{
		mQuads.Insert(impact + i, *addition->GetQuad(i));
	}

	// ��鸽�Ӷ���β���Ƿ������ײ
	impact = DetectIntersection(this, *addition->GetQuad(addition->GetQuadCount() - 1));
	if (impact >= 0)
	{	// ����ײ��˵��������ѡ��˳���ϣ����Ӷ���β�е��ڻ������ڱ�����β��
		if (impact == mQuads.Size() - 1)	// ��ײ���ҵ����һ�У�����Ҫ�ж�˭�ں�
		{
			if (mQuads.Back().right <= addition->GetQuad(addition->GetQuadCount() - 1)->right)
				impact++;
		}
	}
	else
	{ // û����ײ��˵��������ѡ��˳���ϣ�������β�����ڸ��Ӷ���β��
		//CMMASSERT(DetectIntersection(addition, mQuads.Back())>=0);
		if (DetectIntersection(addition, mQuads.Back()) < 0)
			return false;	// �����ԭ�������pdf annot���ö����quad��¼�����϶��������Ű���Ⱥ���򣬱�������֮ǰ��bug��Ҳ�ᵼ�²����ļ��д����������
		impact = mQuads.Size();
	}
	mQuads.Insert(impact, *addition->GetQuad(addition->GetQuadCount() - 1));

	return true;
}