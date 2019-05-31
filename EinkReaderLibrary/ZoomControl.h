/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#pragma once

/*
	���Ž���
*/
#include "ZoomControlToolbar.h"

DECLARE_BUILTIN_NAME(ZoomControl)


class CZoomControl:
	public CXuiElement<CZoomControl,GET_BUILTIN_NAME(ZoomControl)>
{
	friend CXuiElement<CZoomControl,GET_BUILTIN_NAME(ZoomControl)>;

public:
	ULONG InitOnCreate(
		IN IEinkuiIterator* npParent = NULL,	// ������ָ��
		IN ICfKey* npTemplete = NULL,		// npTemplete��Key ID����EID��ֵ��������EType
		IN ULONG nuEID = MAXULONG32	// �����Ϊ0��MAXULONG32����ָ����Ԫ�ص�EID; ����ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��XUIϵͳ�Զ�����
		);

	// ���õ�ǰ��Fat�Ŵ���
	void SetFatRatio(float fatRatio);
	//���÷Ŵ���ͼ��С����ʾ����
	void SetRectOfViewportOnPage(D2D1_SIZE_F& nrImageSize, D2D1_RECT_F& nrViewPort);

protected:
	CZoomControl(void);
	~CZoomControl(void);

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
	// ��������
	virtual ERESULT OnMouseOwnerTest(const D2D1_POINT_2F& rPoint);
	//����ƶ�
	virtual ERESULT OnMouseMoving(const STEMS_MOUSE_MOVING* npInfo);
	//��������뿪
	//virtual void OnMouseFocus(PSTEMS_STATE_CHANGE npState);
	//��갴��
	virtual ERESULT OnMousePressed(const STEMS_MOUSE_BUTTON* npInfo);
private:
	IEinkuiIterator* mpIterBtUP;	
	IEinkuiIterator* mpIterBtDown;		
	IEinkuiIterator* mpIterBtLeft;	
	IEinkuiIterator* mpIterBtRight;		
	IEinkuiIterator* mpIterScrH;//���������
	IEinkuiIterator* mpIterScrV;//���������

	CZoomControlToolbar* mpZoomControlToolbar;

	bool mbIsBeginMove;
	D2D1_POINT_2F mdDropBeginPos;
	D2D1_POINT_2F mdPressPos;	//������º�̧��ʱ��������50������Ϊ���ǵ��
	RECT mdMoveBtShow;

	//���������ƶ�
	void MovePage(ULONG nulID);
	//����4���ƶ���ť��״̬
	void ShowMoveButton(RECT ldRect);
	//��ʾ���������пؼ�
	void ShowItem(bool nbIsShow);
	//����ScrollBar��λ��
	bool SetScrollBarPositionAndSize();
};

#define ZC_TIMER_ID_HIDE 1

#define ZC_BT_UP 101
#define ZC_BT_DOWN 102
#define ZC_BT_LEFT 103
#define ZC_BT_RIGHT 104

