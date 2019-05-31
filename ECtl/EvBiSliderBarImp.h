/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#ifndef _EVBiSliderBarIMP_H_
#define _EVBiSliderBarIMP_H_

#include "EvButtonImp.h"

// Element�Ļ���ʵ�֣���������Elementʱ�����Դ�Ϊ������Ҫ���ԴӴ����ֽ��������࣬��Ϊ��������������ʵ�������Ľӿڣ�ֱ�����������CEvPictureFrame���������ṩIXsElement�ӿ�
// ���ʵ�ֵ�����ͬ�ӿڵ���𣬾Ϳ���ֱ�Ӵ�ĳ��ʵ�������������ࡣ
DECLARE_BUILTIN_NAME(BiSliderBar)
class CEvBiSliderBar :
	public CXuiElement<CEvBiSliderBar ,GET_BUILTIN_NAME(BiSliderBar)>
{
friend CXuiElement<CEvBiSliderBar ,GET_BUILTIN_NAME(BiSliderBar)>;
public:

	// �������༰����������ʱ�����ر�ע��!!! һ��Ҫ���ȵ��û���ķ���
	// �����������ڽ�����Ԫ�ض��󡢳�ʼ���������ݣ�λ�á���С������ͼ�ȣ�
	ULONG InitOnCreate(
		IN IEinkuiIterator* npParent,	// ������ָ��
		IN ICfKey* npTemplete,		// npTemplete��Key ID����EID��ֵ��������EType
		IN ULONG nuEID=MAXULONG32		// �����Ϊ0��MAXULONG32����ָ����Ԫ�ص�EID; ����ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��XUIϵͳ�Զ�����
		);

protected:
	CEvBiSliderBar();
	virtual ~CEvBiSliderBar();

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
private:

	ULONG mulMethod;				 //����ʲô���ŷ�ʽ
	bool mbIsMouseFocus;			 //�Ƿ�ӵ����꽹��
	bool mbIsPressed;				 //�Ƿ񱻰���
private:
	int mnVertical;                  //0�Ǻ���1������
	FLOAT mfMinLeft;
	FLOAT mfMinRigth;
	FLOAT mfMaxLeft;
	FLOAT mfMaxRight;
	FLOAT mfMaxMid;
	FLOAT mfMinMid;
	FLOAT mnStep;

	FLOAT mfPosLeft;
	FLOAT mfPosRight;
	FLOAT mfPosMid;
	D2D1_RECT_F mSliderRect;
	D2D1_RECT_F mBarRect;
	FLOAT mfMaxScrollPixelLeft;
	FLOAT mfDestPixelPerScrollPixLeft;

	FLOAT mfMaxScrollPixelRight;
	FLOAT mfDestPixelPerScrollPixRight;

	FLOAT mfMaxScrollPixelMid;
	FLOAT mfDestPixelPerScrollPixMid;

	IEinkuiIterator* mpDragButtonLeft;
	IEinkuiIterator* mpDragButtonRigth;
	IEinkuiIterator* mpDragButtonMid; 
	IEinkuiIterator * mpDragButtonMidLable;
	FLOAT    mfLabelLength;
	IEinkuiIterator * mpBarPicture;
	IEinkuiIterator * mpLeftBarPicture;
	IEinkuiIterator * mpRightBarPicture;
public:
	bool SetRangMid(FLOAT nMin,FLOAT nMax);
	bool SetPosMid(FLOAT nPost);
	bool SetDeltaSizeMid(FLOAT nfSize);

	bool SetRangeLeft(FLOAT nMin,FLOAT nMax);
	bool SetRangeRight(FLOAT nMin,FLOAT nMax);
	bool SetPosLeft(FLOAT nPost);
	bool SetPosRight(FLOAT nPost);
	bool SetDeltaSizeLeft(FLOAT nfSize);
	bool SetDeltaSizeRigth(FLOAT nfSize);
};

#define SLIDER_BAR_PICTURE 1
#define SLIDER_BAR_LEFT_SLIDERBUTTON 2
#define SLIDER_BAR_RIGHT_SLIDERBUTTON 3
#define BI_SLIDER_BAR_LEFT_PICTURE 4
#define SLIDER_BAR_RIGHT_PICTURE 5
#define SLIDER_BAR_MID_SLIDERBUTTON 6
#define SLIDER_BAR_MID_LABEL 7

#define SLIDER_BAR_VER     L"Vertical"
#define SLIDER_BAR_MIN     L"MinValue"
#define SLIDER_BAR_MAX     L"MaxValue"
#define SLIDER_BAR_UP_BACK L"LeftOrUpBack"


#endif//_EVPICTUREFRAMEIMP_H_
