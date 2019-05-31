/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#ifndef _EVSLIDERBARIMP_H_
#define _EVSLIDERBARIMP_H_

#include "EvButtonImp.h"

// Element�Ļ���ʵ�֣���������Elementʱ�����Դ�Ϊ������Ҫ���ԴӴ����ֽ��������࣬��Ϊ��������������ʵ�������Ľӿڣ�ֱ�����������CEvPictureFrame���������ṩIXsElement�ӿ�
// ���ʵ�ֵ�����ͬ�ӿڵ���𣬾Ϳ���ֱ�Ӵ�ĳ��ʵ�������������ࡣ
DECLARE_BUILTIN_NAME(SliderBar)
class CEvSliderBar :
	public CXuiElement<CEvSliderBar ,GET_BUILTIN_NAME(SliderBar)>
{
friend CXuiElement<CEvSliderBar ,GET_BUILTIN_NAME(SliderBar)>;
public:

	// �������༰����������ʱ�����ر�ע��!!! һ��Ҫ���ȵ��û���ķ���
	// �����������ڽ�����Ԫ�ض��󡢳�ʼ���������ݣ�λ�á���С������ͼ�ȣ�
	ULONG InitOnCreate(
		IN IEinkuiIterator* npParent,	// ������ָ��
		IN ICfKey* npTemplete,		// npTemplete��Key ID����EID��ֵ��������EType
		IN ULONG nuEID=MAXULONG32		// �����Ϊ0��MAXULONG32����ָ����Ԫ�ص�EID; ����ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��XUIϵͳ�Զ�����
		);

protected:
	CEvSliderBar();
	virtual ~CEvSliderBar();

	//װ��������Դ
	virtual ERESULT LoadResource();
	//����
	//virtual ERESULT OnPaint(IEinkuiPaintBoard* npPaintBoard);
	// �ֽ���Ϣ���ṩ��Ϣ�ֽ����Ϣ��Ӧ�Ĺ��ܣ������ʵ���ǽ���Ϣ�ֽ�Ϊ��ͬ������󣬵�����Ӧ�Ĵ����麯�������ڲ���ʶ����Ϣ��һ�ɷ���ERESULT_UNEXPECTED_MESSAGE
	// �������ķ���ֵ���Զ�ͬ�����õ�npMsgָ�����Ϣ������
	virtual ERESULT ParseMessage(IEinkuiMessage* npMsg);
	// ����Ԫ�أ�����֪ͨһ��Ԫ�����٣�ע�⣺��Ԫ�������յ�����Ϣ��Ӧ�ü�ʱ����Ԫ�ع�������UnregisterElement������
	// �Ӷ�����Ԫ�ع�������������һ��Ԫ�ط���������Ϣ�������ٽ��Լ���Ԫ�ع�����ע���������ͷ��������
	virtual ERESULT OnElementDestroy();
	//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
	virtual ERESULT OnElementCreate(IEinkuiIterator* npIterator);

	// ��������
	virtual ERESULT OnMouseOwnerTest(const D2D1_POINT_2F& rPoint);
	
	//��갴��
	virtual ERESULT OnMousePressed(const STEMS_MOUSE_BUTTON* npInfo);
	//������Ϣ
	virtual ERESULT OnPaint(IEinkuiPaintBoard* npPaintBoard);

	//���û�����
	virtual ERESULT OnElementEnable(bool nbIsEnable);
	//Ԫ�زο��ߴ緢���仯
	virtual ERESULT OnElementResized(D2D1_SIZE_F nNewSize);
private:

	ULONG mulMethod;				 //����ʲô���ŷ�ʽ
	bool mbIsMouseFocus;			 //�Ƿ�ӵ����꽹��
	bool mbIsPressed;				 //�Ƿ񱻰���
private:
	int mnVertical;                  //0�Ǻ���1������
	FLOAT mfMin;
	FLOAT mfMax;
	FLOAT mnStep;
	FLOAT mfPos;
	D2D1_RECT_F mSliderRect;
	D2D1_RECT_F mBarRect;
	D2D1_RECT_F mdEffectiveRect;		// ֧�����������Ч����	add by colin
	FLOAT mfMaxScrollPixel;
	FLOAT mfDestPixelPerScrollPix;

	IEinkuiIterator* mpDragButton;
	IEinkuiIterator * mpBarPicture;
	IEinkuiIterator * mpLeftBarPicture;
public:
	bool SetRange(FLOAT nMin,FLOAT nMax);
	bool SetPos(FLOAT nPost);
	bool SetDeltaSize(FLOAT nfSize);
};

#define SLIDER_BAR_PICTURE 1
#define SLIDER_BAR_LEFT_PICTURE 2
#define SLIDER_BAR_SLIDERBUTTON 3

#define SLIDER_BAR_VER     L"Vertical"
#define SLIDER_BAR_MIN     L"MinValue"
#define SLIDER_BAR_MAX     L"MaxValue"
#define SLIDER_BAR_UP_BACK L"LeftOrUpBack"

// ����֧�����������Ч���� add by colin
#define TF_ID_SLIDERBAR_EFFECTIVERECT_LEFT		 L"EffectiveRect/Left"		
#define TF_ID_SLIDERBAR_EFFECTIVERECT_TOP		 L"EffectiveRect/Top"		
#define TF_ID_SLIDERBAR_EFFECTIVERECT_RIGHT		 L"EffectiveRect/Right"		
#define TF_ID_SLIDERBAR_EFFECTIVERECT_BOTTOM	 L"EffectiveRect/Bottom"		


#endif//_EVPICTUREFRAMEIMP_H_
