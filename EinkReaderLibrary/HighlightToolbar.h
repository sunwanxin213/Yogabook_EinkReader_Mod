/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#pragma once

/*
	�����Ĺ�����
*/
#include "cmmstruct.h"

DECLARE_BUILTIN_NAME(HighlightToolbar)


class CHighlightToolbar:
	public CXuiElement<CHighlightToolbar,GET_BUILTIN_NAME(HighlightToolbar)>
{
	friend CXuiElement<CHighlightToolbar,GET_BUILTIN_NAME(HighlightToolbar)>;

public:
	ULONG InitOnCreate(
		IN IEinkuiIterator* npParent = NULL,	// ������ָ��
		IN ICfKey* npTemplete = NULL,		// npTemplete��Key ID����EID��ֵ��������EType
		IN ULONG nuEID = MAXULONG32	// �����Ϊ0��MAXULONG32����ָ����Ԫ�ص�EID; ����ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��XUIϵͳ�Զ�����
		);

	//��ʼ���Լ�
	void initData(void);
	//����highlight����
	void SetHighlightCount(int niCount);

protected:
	CHighlightToolbar(void);
	~CHighlightToolbar(void);

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

	IEinkuiIterator* mpIterBtCopy;		//����
	IEinkuiIterator* mpIterBtHighlight;	//����
	IEinkuiIterator* mpIterBtDeleteLine;//ɾ����
	IEinkuiIterator* mpIterBtLine;		//�»���
	IEinkuiIterator* mpIterBtDelete;		//ɾ����ť
};

//#define HT_BT_COPY 1  //����
//#define HT_BT_HIGHLIGHT 2  //����
//#define HT_BT_DELETE_LINE 3  //ɾ����
//#define HT_BT_UNDER_LINE 4  //�»���
//#define HT_BT_DELETE 5  //ɾ��