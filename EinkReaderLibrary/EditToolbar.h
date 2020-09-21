/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#pragma once

/*
	�ʼ��༭������
*/

#include "MenuPen.h"

DECLARE_BUILTIN_NAME(EditToolbar)


class CEditToolbar:
	public CXuiElement<CEditToolbar,GET_BUILTIN_NAME(EditToolbar)>
{
	friend CXuiElement<CEditToolbar,GET_BUILTIN_NAME(EditToolbar)>;

public:
	ULONG InitOnCreate(
		IN IEinkuiIterator* npParent = NULL,	// ������ָ��
		IN ICfKey* npTemplete = NULL,		// npTemplete��Key ID����EID��ֵ��������EType
		IN ULONG nuEID = MAXULONG32	// �����Ϊ0��MAXULONG32����ָ����Ԫ�ص�EID; ����ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��XUIϵͳ�Զ�����
		);

	//���ð�ťѡ��״̬
	void SetSelect(ULONG nulID);
	// ���ó�ʼ������
	void SetData(DWORD ndwPenWidthIndex,DWORD ndwPenColorIndex);
	//����������
	void ToolbarShow(bool nbIsShow);
	//����redo,undo��ť״̬
	void SetUndoRedoStatus(bool nbUndoEnable, bool nbRedoEnable);
	//��ʼ����Ĭ�ϱ�״̬
	void init(void);
protected:
	CEditToolbar(void);
	~CEditToolbar(void);

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
	IEinkuiIterator* mpIterBtUndo;	//undo
	IEinkuiIterator* mpIterBtRedo;	//redo
	IEinkuiIterator* mpIterBtHand;	//hand
	IEinkuiIterator* mpIterLine1;	//
	IEinkuiIterator* mpIterLine2;	//
	IEinkuiIterator* mpIterGroup;	//


	CMenuPen* mpMenuPen;

	DWORD mdwPenWidthIndex;
	DWORD mdwPenColorIndex;
	bool mbIsHand; //�Ƿ�����д
	ULONG mulSelectID;
};

#define ET_BT_PEN 1
#define ET_BT_ERASER 2
#define ET_BT_A 3
#define ET_BT_HAND 4
#define ET_BT_UNDO 5
#define ET_BT_REDO 6