/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */

#include "windows.h"
#include "pdfstextPage.h"
#include "pdfDocument.h"
#include "cmmPtr.h"
#include "pdfPage.h"
#include "pdfStextQuadList.h"
#include "pdfAnnot.h"
#include "stextAnnot.h"

//////////////////////////////////////////////////////////////////////////
// Structured Text Page��
DEFINE_BUILTIN_NAME(CpdfStextPage)

CpdfStextPage::CpdfStextPage()
{
	mStextPage = NULL;
	//mFzPage = NULL;
	mNotStextPage = NULL;
}

CpdfStextPage::~CpdfStextPage()
{
	fz_context *ctx = CEdpdfModule::GetUniqueObject()->fzContext;

	if (mStextPage != NULL)
		fz_drop_stext_page(ctx,mStextPage);

	if (mNotStextPage != NULL)
		mNotStextPage->Release();
}


// ��ʼ������������ʵ�ָ��ֲ�ͬ�����ĳ�ʼ��������ע�⣬����������InitOnCreate������һ��Ҫ���û����InitOnCreate����
// ����0��ʾ�ɹ�������ֵ���λΪ1��ʾ�������ش���Ӧ����ֹ��ʼ�����̣����صľ��Ǵ����룻��������ֵ��ʾ�����Ǵ��󷵻���
ULONG CpdfStextPage::InitOnCreate(CpdfPage* page)
{
	mNotStextPage = page;
	mNotStextPage->AddRefer();

	fz_cookie cookie = { 0 };
	fz_context *ctx = CEdpdfModule::GetUniqueObject()->fzContext;

	mStextPage = fz_new_stext_page(ctx, fz_bound_page(ctx, mNotStextPage->GetFzObj()));
	if (mStextPage == NULL)
		return EDERR_3RDPART_FAILED;

	fz_device* device = fz_new_stext_device(ctx, mStextPage, NULL);
	if (device == NULL)
		return EDERR_3RDPART_FAILED;

	fz_run_page(ctx, mNotStextPage->GetFzObj(), device, fz_identity, &cookie);

	fz_close_device(ctx, device);
	fz_drop_device(ctx, device);

	return EDERR_SUCCESS;
}

bool CpdfStextPage::DetectSelectedText(IN const ED_POINTF* aPoint, IN const ED_POINTF* bPoint, OUT IEdStextQuadList_ptr* stext, /* ���ؽṹ���ı�������Ҫ�ͷ� */ OUT IEdAnnotList_ptr* impactedAnnot, /* ������ײ������Annot�����б���Ҫ�ͷ� */IN bool32 includeImpacted /*����ײ��annot����Ҳ����ѡ��*/)
{
	fz_point a, b;

	a.x = aPoint->x;
	a.y = aPoint->y;
	b.x = bPoint->x;
	b.y = bPoint->y;

	//float zoom = 72.0 / 200;
	//mtx = fz_scale(a.x,a.y);
	//a = fz_transform_point(a, mtx);
	//b = fz_transform_point(b, mtx);
	//a.x = 140.0f;
	//a.y = 100.0f;
	//b.x = 120.0f;
	//b.y = 150.0f;

	cmmDeletePtr<fz_quad> quads;
	quads.allocate(1000);

	int n = fz_highlight_selection(CEdpdfModule::GetUniqueObject()->fzContext, mStextPage, a, b, quads.ptr(), 1000);
	if (n <= 0)
		return false;

	cmmReleasePtr<CpdfStextQuadsList> quadList = CpdfStextQuadsList::CreateInstance();
	if (quadList == NULL)
		return false;

	cmmReleasePtr<CpdfdAnnotList> impactList = CpdfdAnnotList::CreateInstance();
	if (quadList == NULL)
		return false;

	for (int i = 0; i < n; i++)
	{
		quadList->AddQuad(quads()[i]);
	}


	// ȡpage�����ȫ��annot��̽���Ƿ�����ײ
	auto annots = mNotStextPage->GetAnnotList();
	vector<IEdStextAnnot_ptr> impacked;
	if (annots.size() > 0)
	{
		for (auto i : annots)
		{
			CpdfStextQuadsList* quadlistFromPage = NULL;
			switch (i->GetType())
			{
			case EDPDF_ANNOT_UNDERL:
			case EDPDF_ANNOT_DELETE:
			case EDPDF_ANNOT_HIGHLIGHT:
			{
				IEdStextAnnot_ptr stextAnnot = i->ConvertToStextAnnot();
				if (stextAnnot == NULL)
					break;

				CpdfStextAnnot* obj = dynamic_cast<CpdfStextAnnot*>(stextAnnot);
				if (obj == NULL)
					break;

				quadlistFromPage = obj->GetQuadsList();

				if (quadlistFromPage != NULL && quadList->DetectIntersection(quadlistFromPage) >= 0)
				{
					// ������ײ�������������
					impactList->AddAnnot(i);
					i->AddRefer();
					impacked.push_back(obj);
				}
			}
			break;
			case EDPDF_ANNOT_INK:
			default:
				break;
			}

		}
	}

	// ����ײ����������ѡ����
	if (includeImpacted != false && impacked.size() > 0)
	{
		for (auto i : impacked)
		{
			if (quadList->Combination(i->GetQuadsList()) == false)
				return false;
		}

		// ��Ϊ����ѡȡ�ص����ܶ��quad������໥�ص���������Ҫ��һЩquad��ƴ�ӣ��˴���򵥵ķ�����ֱ�ӵ��ñ�����һ�Σ��õײ����¼���ѡȡ��δ�����ٽ���ײ����ѡȡ����Ϊ��ײ���Ѿ����ڶ��ε�ѡȡ����
		ED_POINTF ptA, ptB;
		quadList->GetAPoint(&ptA);
		quadList->GetBPoint(&ptB);

		return DetectSelectedText(&ptA, &ptB, stext, impactedAnnot, false);
	}

	*stext = quadList.notfree();
	
	if(impactedAnnot != NULL)
		*impactedAnnot = impactList.notfree();

	return true;
}

int32Eink CpdfStextPage::CopyText(IN ED_RECTF_PTR selBox, OUT char16_ptr textBuf, /* ����NULL����������ʵ�ʵ��ַ�����������β0 */ IN int32Eink bufSize)
{
	fz_point a, b;

	a.x = selBox->left;
	a.y = selBox->top;
	b.x = selBox->right;
	b.y = selBox->bottom;

	if (textBuf != NULL)
		*textBuf = UNICODE_NULL;

	char * stringSel = fz_copy_selection(CEdpdfModule::GetUniqueObject()->fzContext, mStextPage, a, b, 0);
	if (stringSel == NULL)
		return 0;

	cmmStringW ss;

	ss = stringSel;

	if (textBuf != NULL)
	{
		wcscpy_s(textBuf, bufSize, ss.ptr());
	}

	return ss.size();
}


