#pragma once
#include "math.h"

#define D2DDRAWX_PI       3.14159265358979323846f

//////////////////////////////////////////////////////////////////////////
// ����ʵ����D2d Layer���Geometry����ͼʵ���ɰ����
class CEdxGeometryLayer
{
public:
	CEdxGeometryLayer(){
		mpGeomety = NULL;
		mpLayer = NULL;
		mpPushToTarget = NULL;
		mbEnalbed = true;
	}
	~CEdxGeometryLayer(){
		CMM_SAFE_RELEASE(mpGeomety);
		CMM_SAFE_RELEASE(mpLayer);
	}

	// ʹ�ܻ��߽��ã�Ĭ�϶���Enable״̬
	void Enable(bool nbEnble){
		mbEnalbed = nbEnble;
	}

	// ����ǰ���ã������Leave����ƥ��
	bool Enter(ID2D1RenderTarget* npD2dTarget){
		CMM_SAFE_RELEASE(mpLayer);

		//if(mbEnalbed == false)
		//	return true;

		if(mpGeomety == NULL)
			return false;

		HRESULT hr = npD2dTarget->CreateLayer(NULL, &mpLayer);
		if(FAILED(hr))
			return false;

		npD2dTarget->PushLayer(
			D2D1::LayerParameters(D2D1::InfiniteRect(), mpGeomety),
			mpLayer
			);
		mpPushToTarget = npD2dTarget;

		return true;
	}

	// ���ƺ���ã������Enter����ƥ��
	void Leave(ID2D1RenderTarget* npD2dTarget){
		if(npD2dTarget != mpPushToTarget || mpGeomety==NULL/* || mbEnalbed != false*/)
			return;
		mpPushToTarget = NULL;

		npD2dTarget->PopLayer();
		CMM_SAFE_RELEASE(mpLayer);
	}

protected:
	bool mbEnalbed;
	ID2D1Geometry* mpGeomety;
	ID2D1Layer* mpLayer;
	ID2D1RenderTarget* mpPushToTarget;

};


//////////////////////////////////////////////////////////////////////////
// �ӱ���״�ɰ�
class CEdxDialMask : public CEdxGeometryLayer
{
public:
	
	// ���û���ֵ��ÿ�ε���SetBase�󣬱����������һ��SetAngle�������ڻ���
	bool SetBase(
		bool nbClockwise,			// sweep direction on dial��true for clockwise,opposite for counter clockwise
		const D2D1_POINT_2F& ndCenter,
		float nfRadius,
		float nfStartAngle // ��ʼ�Ƕȣ�������ʾ˳ʱ�������������ʾ��ʱ�����������ɨ�跽���޹أ�ֻ���趨��ʼ�Ƕȵķ���
		)					// 0����12�㷽��180��ʾ6�㷽��
	{
		if(nfRadius < 1.0f )
			return false;

		mbClockwise = nbClockwise;
		mdCenter = ndCenter;
		mfRadius = nfRadius;
		mfStartAngle = nfStartAngle*D2DDRAWX_PI/180.0f;
		if(nfStartAngle < 0.0f)
			mfStartAngle += D2DDRAWX_PI*2.0f;
		mfSweeped = 0.0f;

		return true;
	}

	__inline bool SetAngle(float nfSweep);	// ����ɨ����ȣ�0.0 - 1.0��Χ����ʾһ����Բ��С��0.0û�����壬����1.0��Ȼ��һ��Բ

	// ��ȡָʾ���ϵĵ�����꣬Բ�ģ�ɨ�跽����ʵ�Ƕȶ�����SetBase���趨Ϊ׼
	D2D1_POINT_2F GetIndictorPosition(
			FLOAT nfRadius,		// �õ����Բ�ĵľ��룬���õ�����ָʾ��ɨ����ɵ�Բ�İ뾶
			FLOAT nfSweep=-1.0f		// ɨ����ȣ�0.0 - 1.0��Χ����ʾһ����Բ��С��0.0��ʾȡ��ǰ�ķ������ã�0.0��ʾ��ʼλ�ã�����1.0�൱�ڻ�����Բ�������
			){

		D2D1_POINT_2F ldEndPoint;

		if(nfSweep < 0.0f)
			nfSweep = mfSweeped;

		if(mbClockwise != false)
		{
			ldEndPoint.x = mdCenter.x + sin(mfStartAngle+nfSweep*D2DDRAWX_PI*2.0f)*nfRadius;
			ldEndPoint.y = mdCenter.y - cos(mfStartAngle+nfSweep*D2DDRAWX_PI*2.0f)*nfRadius;
		}
		else
		{
			ldEndPoint.x = mdCenter.x + sin(mfStartAngle-nfSweep*D2DDRAWX_PI*2.0f)*nfRadius;
			ldEndPoint.y = mdCenter.y - cos(mfStartAngle-nfSweep*D2DDRAWX_PI*2.0f)*nfRadius;
		}

		return ldEndPoint;
	}

protected:
	D2D1_POINT_2F mdCenter;
	bool mbClockwise;
	float mfRadius;
	float mfStartAngle;
	float mfSweeped;

	ID2D1GeometrySink* CreatePathGeomety() {
		HRESULT hr;
		ID2D1GeometrySink* lpSink = NULL;
		ID2D1PathGeometry* lpPathGeomety=NULL;

		CMM_SAFE_RELEASE(mpGeomety);
		do 
		{
			hr = EinkuiGetSystem()->GetD2dFactory()->CreatePathGeometry(&lpPathGeomety);
			if(FAILED(hr))
				break;

			hr = lpPathGeomety->Open(&lpSink);
			if(FAILED(hr))
				break;

			lpSink->SetFillMode(D2D1_FILL_MODE_ALTERNATE);
			mpGeomety = lpPathGeomety;
			lpPathGeomety = NULL;
		} while (false);

		CMM_SAFE_RELEASE(lpPathGeomety);

		return lpSink;
	}

};

bool CEdxDialMask::SetAngle(float nfSweep)	// ������ֹ�Ƕ�
{

	if(nfSweep < 0.0f)
		return false;
	
	if(abs(nfSweep - mfSweeped) < 0.001f)
	{
		if(mfSweeped!=0.0f)	// ��ʾ�Ѿ����ù�һ��SetAngle�����ˣ���ô�Ͳ���Ҫ�ı�
			return true;
		mfSweeped = 0.001f;
	}
	else	
	if(mfSweeped > 0.999f)	// ��ǰ�Ѿ���һ��Բ�ˣ��Ͳ������޸���
	{
		mfSweeped = nfSweep;
		return true;
	}
	else
		mfSweeped = nfSweep;


	if(mfSweeped > 0.999f)	// it's a circle
	{
		HRESULT hr;
		ID2D1EllipseGeometry* lpEllipseGeomety=NULL;

		CMM_SAFE_RELEASE(mpGeomety);

		hr = EinkuiGetSystem()->GetD2dFactory()->CreateEllipseGeometry(D2D1::Ellipse(mdCenter,mfRadius,mfRadius), &lpEllipseGeomety);

		if(FAILED(hr))
			return false;

		mpGeomety = lpEllipseGeomety;
	}
	else
	{
		ID2D1GeometrySink* lpSink;
		D2D1_POINT_2F ldEndPoint;


		lpSink = CreatePathGeomety();
		if(lpSink == NULL)
			return false;

		lpSink->BeginFigure(
			mdCenter,
			D2D1_FIGURE_BEGIN_FILLED
			);

		lpSink->AddLine(D2D1::Point2F(mdCenter.x + sin(mfStartAngle)*mfRadius, mdCenter.y - cos(-mfStartAngle)*mfRadius));

		if(mbClockwise != false)
		{
			ldEndPoint.x = mdCenter.x + sin(mfStartAngle+mfSweeped*D2DDRAWX_PI*2.0f)*mfRadius;
			ldEndPoint.y = mdCenter.y - cos(mfStartAngle+mfSweeped*D2DDRAWX_PI*2.0f)*mfRadius;
		}
		else
		{
			ldEndPoint.x = mdCenter.x + sin(mfStartAngle-mfSweeped*D2DDRAWX_PI*2.0f)*mfRadius;
			ldEndPoint.y = mdCenter.y - cos(mfStartAngle-mfSweeped*D2DDRAWX_PI*2.0f)*mfRadius;
		}

		lpSink->AddArc(
			D2D1::ArcSegment(
			ldEndPoint,
			D2D1::SizeF(mfRadius,mfRadius),
			0.0f,
			mbClockwise?D2D1_SWEEP_DIRECTION_CLOCKWISE:D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE,
			mfSweeped>0.5f?D2D1_ARC_SIZE_LARGE:D2D1_ARC_SIZE_SMALL
			) );

		lpSink->AddLine(mdCenter);

		lpSink->EndFigure(D2D1_FIGURE_END_CLOSED);

		lpSink->Close();

		CMM_SAFE_RELEASE(lpSink);
	}


	return true;
}





