/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#pragma once

/*
	���Ž���Ĺ�����
*/
#include "cmmstruct.h"

DECLARE_BUILTIN_NAME(ZoomControlToolbarTxt)

#define ZCT_BT_SUB 106
#define ZCT_BT_ADD 107
#define ZCT_FONTSIZE_LEVEL 5 //5������

class CZoomControlToolbarTxt:
	public CXuiElement<CZoomControlToolbarTxt,GET_BUILTIN_NAME(ZoomControlToolbarTxt)>
{
	friend CXuiElement<CZoomControlToolbarTxt,GET_BUILTIN_NAME(ZoomControlToolbarTxt)>;

public:
	ULONG InitOnCreate(
		IN IEinkuiIterator* npParent = NULL,	// ������ָ��
		IN ICfKey* npTemplete = NULL,		// npTemplete��Key ID����EID��ֵ��������EType
		IN ULONG nuEID = MAXULONG32	// �����Ϊ0��MAXULONG32����ָ����Ԫ�ص�EID; ����ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��XUIϵͳ�Զ�����
		);

	//��ʼ���Լ�
	void initData(void);
	void SetFontsize(DWORD ldwFontSizeIndex);

protected:
	CZoomControlToolbarTxt(void);
	~CZoomControlToolbarTxt(void);

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
	//IEinkuiIterator* mpIterBili;	//��ʾ����
	IEinkuiIterator* mpIterBtAdd;
	IEinkuiIterator* mpIterBtSub;
	

	DWORD mdwFontSizeIndex;


	//���÷Ŵ󼶱�
	void SetLevel(bool nbIsAdd);

};



