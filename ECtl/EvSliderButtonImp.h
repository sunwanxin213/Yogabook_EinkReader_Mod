/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#ifndef _EVSliderButtonIMP_H_
#define _EVSliderButtonIMP_H_

#include "EvButtonImp.h"

// Element�Ļ���ʵ�֣���������Elementʱ�����Դ�Ϊ������Ҫ���ԴӴ����ֽ��������࣬��Ϊ��������������ʵ�������Ľӿڣ�ֱ�����������CEvPictureFrame���������ṩIXsElement�ӿ�
// ���ʵ�ֵ�����ͬ�ӿڵ���𣬾Ϳ���ֱ�Ӵ�ĳ��ʵ�������������ࡣ
DECLARE_BUILTIN_NAME(SliderButton)
class CEvSliderButton :	public CEvButton
{
friend CEvButton;
public:

	//////////////////////////////////////////////////////////////////////////
	// ����ʵ�������������ڴ�CXuiElement���������ٴ���������
	DEFINE_CUMSTOMIZE_CREATE(CEvSliderButton,(IEinkuiIterator* npParent,ICfKey* npTemplete,ULONG nuEID),(npParent,npTemplete,nuEID))


	//////////////////////////////////////////////////////////////////////////
	// ��������ʶ��ǰһ�������Ǳ����ETYPE��ͬDECLARE_BUILTIN_NAME()���е�����һ��
	// ���ڴ�CXuiElement���������ٴ���������
	DEFINE_DERIVED_TYPECAST(SliderButton,CEvButton)


	// �������༰����������ʱ�����ر�ע��!!! һ��Ҫ���ȵ��û���ķ���
	// �����������ڽ�����Ԫ�ض��󡢳�ʼ���������ݣ�λ�á���С������ͼ�ȣ�
	ULONG InitOnCreate(
		IN IEinkuiIterator* npParent,	// ������ָ��
		IN ICfKey* npTemplete,		// npTemplete��Key ID����EID��ֵ��������EType
		IN ULONG nuEID=MAXULONG32		// �����Ϊ0��MAXULONG32����ָ����Ԫ�ص�EID; ����ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��XUIϵͳ�Զ�����
		);
	// �ֽ���Ϣ���ṩ��Ϣ�ֽ����Ϣ��Ӧ�Ĺ��ܣ������ʵ���ǽ���Ϣ�ֽ�Ϊ��ͬ������󣬵�����Ӧ�Ĵ����麯�������ڲ���ʶ����Ϣ��һ�ɷ���ERESULT_UNEXPECTED_MESSAGE
	// �������ķ���ֵ���Զ�ͬ�����õ�npMsgָ�����Ϣ������
	virtual ERESULT ParseMessage(IEinkuiMessage* npMsg);
protected:
	CEvSliderButton();
	virtual ~CEvSliderButton();
public:
	LONG mnMoveStyle; //���������Ectl.h
	D2D1_RECT_F mRectSlider;//��������
	D2D1_POINT_2F mDragStartPoint;

	//Ԫ����ק
	virtual ERESULT OnDragging(const STMS_DRAGGING_ELE* npInfo);

	//��ק��ʼ
	virtual ERESULT OnDragBegin(const STMS_DRAGGING_ELE* npInfo);

	//��ק����
	virtual ERESULT OnDragEnd(const STMS_DRAGGING_ELE* npInfo);

	//���û�����
	virtual ERESULT OnElementEnable(bool nbIsEnable);
};

#endif//_EVPICTUREFRAMEIMP_H_
