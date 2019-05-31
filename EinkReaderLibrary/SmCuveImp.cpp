/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#include "stdafx.h"
#include "cmmBaseObj.h"
#include "cmmstruct.h"
#include "Einkui.h"

#include "ElementImp.h"
#include "XCtl.h"

#include "SmCuveImp.h"

DEFINE_BUILTIN_NAME(Cuve)

CSmCuveImp::CSmCuveImp()
{
	mpLineBrush = NULL;
}

CSmCuveImp::~CSmCuveImp()
{

}

ULONG CSmCuveImp::InitOnCreate(
	IN IEinkuiIterator* npParent,	// ������ָ��
	IN ICfKey* npTemplete,		// npTemplete��Key ID����EID��ֵ��������EType
	IN ULONG nuEID	// �����Ϊ0��MAXULONG32����ָ����Ԫ�ص�EID; ����ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��XUIϵͳ�Զ�����
	)
{
	ERESULT leResult = ERESULT_UNSUCCESSFUL;

	do 
	{
		if(npTemplete == NULL)
			return ERESULT_UNSUCCESSFUL;

		//���ȵ��û���
		leResult = 	CXuiElement::InitOnCreate(npParent,npTemplete,nuEID);
		if(leResult != ERESULT_SUCCESS)
			break;

		//mpIterator->ModifyStyles(EITR_STYLE_LAZY_UPDATE);


		leResult = ERESULT_SUCCESS;

	} while (false);

	return leResult;
}

//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
ERESULT CSmCuveImp::OnElementCreate(IEinkuiIterator* npIterator)
{
	ERESULT luResult = ERESULT_UNSUCCESSFUL;

	do
	{
		luResult = CXuiElement::OnElementCreate(npIterator);
		if(luResult != ERESULT_SUCCESS)
			break;

		mlBaseLine = mpTemplete->QuerySubKeyValueAsLONG(L"BaseLine",(LONG)mpIterator->GetSizeY() - 1);
		mfInterval = (FLOAT)mpTemplete->QuerySubKeyValueAsLONG(L"Interval",6);
		if(mfInterval <= 0.0f)
			mfInterval = 6.0f;
		mlMaxPt = ((LONG)mpIterator->GetSizeX() + (LONG)mfInterval - 1)/(LONG)mfInterval;

		mpIterator->SetVisibleRegion(D2D1::RectF(0.0f,0.0f,mpIterator->GetSizeX(),mpIterator->GetSizeY()));

		luResult = ERESULT_SUCCESS;

	}while(false);

	return luResult;
}

// ����Ԫ�أ�����֪ͨһ��Ԫ�����٣�ע�⣺��Ԫ�������յ�����Ϣ��Ӧ�ü�ʱ����Ԫ�ع�������UnregisterElement������
// �Ӷ�����Ԫ�ع�������������һ��Ԫ�ط���������Ϣ�������ٽ��Լ���Ԫ�ع�����ע���������ͷ��������
ERESULT CSmCuveImp::OnElementDestroy()
{
	CMM_SAFE_RELEASE(mpLineBrush);

	CXuiElement::OnElementDestroy();

	return ERESULT_SUCCESS;
}

// �ֽ���Ϣ���ṩ��Ϣ�ֽ����Ϣ��Ӧ�Ĺ��ܣ������ʵ���ǽ���Ϣ�ֽ�Ϊ��ͬ������󣬵�����Ӧ�Ĵ����麯�������ڲ���ʶ����Ϣ��һ�ɷ���ERESULT_UNEXPECTED_MESSAGE
// �������ķ���ֵ���Զ�ͬ�����õ�npMsgָ�����Ϣ������
ERESULT CSmCuveImp::ParseMessage(IEinkuiMessage* npMsg)
{
	// ʵ��ԭ�����ȵ�������ķֽ⹦�ܣ����󽫲��������Ϣ��������

	ERESULT luResult = ERESULT_UNEXPECTED_MESSAGE;

	if(npMsg->GetMessageID() == EACT_CUVE_ADD_POINT)
	{
		if(npMsg->GetInputDataSize() < sizeof(LONG))
		{
			return ERESULT_WRONG_PARAMETERS;
		}

		LONG llPercent = *(LONG*)npMsg->GetInputData();
		if(llPercent < 0)
			llPercent = 0;

		llPercent = ((100-llPercent)*mlBaseLine)/100;
		moPointArr.Insert(-1,(FLOAT)llPercent);

		if(moPointArr.Size() > mlMaxPt)
			moPointArr.RemoveByIndex(0);
	}
	else
		luResult = CXuiElement::ParseMessage(npMsg); // ���û����ͬ��������ע�⣺һ��Ҫ��������ֱ�ӻ���

	return luResult;
}

//������Ϣ
ERESULT CSmCuveImp::OnPaint(IEinkuiPaintBoard* npPaintBoard)
{
	FLOAT lfX = mpIterator->GetSizeX();

	// ���潨����ˢ
	if(mpLineBrush ==NULL)
	{
		mpLineBrush = EinkuiGetSystem()->CreateBrush(XuiSolidBrush,D2D1::ColorF(D2D1::ColorF::Green,1.0f));
	}

	for (int i=moPointArr.Size()-1;i>0;i--)
	{
		npPaintBoard->DrawLine(D2D1::Point2F(lfX,moPointArr[i]),D2D1::Point2F(lfX-mfInterval,moPointArr[i-1]),mpLineBrush);
		lfX -= mfInterval;
	}

	return ERESULT_SUCCESS;
}

