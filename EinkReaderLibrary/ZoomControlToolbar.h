/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#pragma once

/*
	���Ž���Ĺ�����
*/
#include "cmmstruct.h"

DECLARE_BUILTIN_NAME(ZoomControlToolbar)

#define ZC_MAX_ZOOM 64 //�����ԷŴ󼸼���0��100%

class CZoomControlToolbar:
	public CXuiElement<CZoomControlToolbar,GET_BUILTIN_NAME(ZoomControlToolbar)>
{
	friend CXuiElement<CZoomControlToolbar,GET_BUILTIN_NAME(ZoomControlToolbar)>;

public:
	ULONG InitOnCreate(
		IN IEinkuiIterator* npParent = NULL,	// ������ָ��
		IN ICfKey* npTemplete = NULL,		// npTemplete��Key ID����EID��ֵ��������EType
		IN ULONG nuEID = MAXULONG32	// �����Ϊ0��MAXULONG32����ָ����Ԫ�ص�EID; ����ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��XUIϵͳ�Զ�����
		);

	//��ʼ���Լ�
	void initData(void);

	// ���õ�ǰ��Fat�Ŵ���
	void SetFatRatio(float fatRatio);

protected:
	CZoomControlToolbar(void);
	~CZoomControlToolbar(void);

	//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
	virtual ERESULT OnElementCreate(IEinkuiIterator* npIterator);
	//��ʱ��
	virtual void OnTimer(
		PSTEMS_TIMER npStatus
		);
	//��Ϣ������
	virtual ERESULT ParseMessage(IEinkuiMessage* npMsg);
	//��ť�����¼�
	virtual ERESULT OnCtlButtonClick(IEinkuiIterator* npSender);
	//Ԫ�زο��ߴ緢���仯
	virtual ERESULT OnElementResized(D2D1_SIZE_F nNewSize);
	//֪ͨԪ�ء���ʾ/���ء������ı�
	virtual ERESULT OnElementShow(bool nbIsShow);

private:
	IEinkuiIterator* mpIterBili;	//��ʾ����
	IEinkuiIterator* mpIterBtAdd;
	IEinkuiIterator* mpIterBtSub;
	int mlCurrentZoomLevel; //��ǰ�Ŵ󼶱�Ĭ��0��
//	float mfZoom[ZC_MAX_ZOOM];
	cmmVector<float> mfZoom;
	int miFatRatioInx;
	//int miMaxRatioInx;
	IEinkuiIterator* mpIterBtDefault;

	//���÷Ŵ󼶱�
	void SetLevel(bool nbIsAdd);
	//������ʾ����
	void SetString(ULONG nulLevel);
};

#define ZC_BT_CLOSE 106
#define ZC_BT_DEFAULT 107
#define ZC_BT_ADD 109
#define ZC_BT_SUB 108
#define ZC_BT_SNAP 111
