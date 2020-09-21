/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#pragma once

/*
	�ײ�������
*/

#include "ZoomControlToolbar.h"
#include "MoveControlToolbar.h"
#include "PageProgress.h"
#include "Define.h"

DECLARE_BUILTIN_NAME(ToolbarBottom)

class CToolbarBottom:
	public CXuiElement<CToolbarBottom,GET_BUILTIN_NAME(ToolbarBottom)>
{
	friend CXuiElement<CToolbarBottom,GET_BUILTIN_NAME(ToolbarBottom)>;

public:
	ULONG InitOnCreate(
		IN IEinkuiIterator* npParent = NULL,	// ������ָ��
		IN ICfKey* npTemplete = NULL,		// npTemplete��Key ID����EID��ֵ��������EType
		IN ULONG nuEID = MAXULONG32	// �����Ϊ0��MAXULONG32����ָ����Ԫ�ص�EID; ����ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��XUIϵͳ�Զ�����
		);

	//����ҳ���ַ���
	void SetPage(ULONG nulCurrentPage,ULONG nulPageCount);
	// ���õ�ǰ��Fat�Ŵ���
	void SetFatRatio(float fatRatio);
	//��ʾ������Ԫ��
	void ShowItem(bool nbIsShow);
	//���õ�ǰ�򿪵��ļ�����
	void SetDoctype(int niType);
	//����4���ƶ���ť��״̬
	void ShowMoveButton(RECT ldRect);
	//����/��ʾ��ҳ������
	void HidePageProcess(bool nbIsShow);
	//��������ģʽ
	void SetZoomStatus(ZoomStatus status, int scaleLevel = 0);

	void SetMoveToolbarVisible(bool visible);
	//���û�������
	void SetMoveForward(MoveForward forward);
	//��������Ӧ���ű�������Ӧ�����ʵ����ŵȼ�
	float AjustAutoZoomLevel(float ratio);
	//�����аױ߰�ť��״̬
	void EnableAutoZoomButton(bool enable);
	//��ʾ���ű���
	void SetRatioString(double ratio);

protected:
	CToolbarBottom(void);
	~CToolbarBottom(void);

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

	//���¶�λԪ��
	void RelocationItem(void);

	// Interface for ReaderBaseFrame to active Zoom [zhuhl5@20200116]
	void ActiveZoom(int scaleLevel = 0);

private:

	IEinkuiIterator* mpIterBtPageNumber;	//ҳ��
	IEinkuiIterator* mpIterBtPre;	//��һҳ
	IEinkuiIterator* mpIterBtNext;	//��һҳ
	IEinkuiIterator* mpIterBackground;	//����ͼ
	CMoveControlToolbar* mpMoveControlToolbar;
	CZoomControlToolbar* mpZoomControlToolbar;
	CPageProgress* mpPageProgress; //ҳ����ת

	ZoomStatus mZoomStatus = ZoomStatus::NONE;
	int miDocType;

	D2D1_POINT_2F mdDropBeginPos;
	D2D1_POINT_2F mdPressPos;	//������º�̧��ʱ��������50������Ϊ���ǵ��
	
	ULONG mulCurrentPage;
	ULONG mulPageCount;
	bool mbIsPressed;
};

#define TBB_BT_PRE 2
#define TBB_BT_NEXT 3
#define TBB_BT_ZOOM 4
#define TBB_BT_NUMBER 5
