/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#pragma once

/*
	����ѡ�������
*/

#include "PdfPicture.h"
#include "HighlightToolbar.h"

DECLARE_BUILTIN_NAME(Highlight)

class CHighlight:
	public CXuiElement<CHighlight,GET_BUILTIN_NAME(Highlight)>
{
	friend CXuiElement<CHighlight,GET_BUILTIN_NAME(Highlight)>;

public:
	ULONG InitOnCreate(
		IN IEinkuiIterator* npParent = NULL,	// ������ָ��
		IN ICfKey* npTemplete = NULL,		// npTemplete��Key ID����EID��ֵ��������EType
		IN ULONG nuEID = MAXULONG32	// �����Ϊ0��MAXULONG32����ָ����Ԫ�ص�EID; ����ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��XUIϵͳ�Զ�����
		);

	//�����ĵ�����ָ��
	void SetPdfPicture(CPdfPicture* npPdfPicture);
	//�������������߶Ⱥ����������߶�
	void SetToolbarHeight(int niTopHeight, int niBottomHeight);
	//����ѡ��
	void HideSelect(void);
	//���¶�λ
	void Relocation(void);
	//�����¼�
	void TouchDown(PEI_TOUCHINPUT_POINT npPoint, ULONG nulPenMode, bool nbIsHand);
	void TouchMove(PEI_TOUCHINPUT_POINT npPoint, ULONG nulPenMode, bool nbIsHand);
	void TouchUp(PEI_TOUCHINPUT_POINT npPoint, ULONG nulPenMode, bool nbIsHand);
	void PenLeave(ULONG nulPenMode);
protected:
	CHighlight(void);
	~CHighlight(void);

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
	//��갴��
	virtual ERESULT OnMousePressed(const STEMS_MOUSE_BUTTON* npInfo);
	////����ƶ�
	//virtual ERESULT OnMouseMoving(const STEMS_MOUSE_MOVING* npInfo);
	//�����ƶ�����¼�
	void DragEndPro(D2D1_POINT_2F ndTop, D2D1_POINT_2F ndBottom);
private:


	D2D1_POINT_2F mdPressPos;
	DWORD mdwClickTicount; //�����жϳ��� ����500ms�㳤��
	CPdfPicture* mpPdfPicture;
	CHighlightToolbar* mpHighlightToolbar;
	IEinkuiIterator* mpIterBtTop;
	IEinkuiIterator* mpIterBtBottom;

	bool mbIsPressed;
	bool mbIsTouchDown;
	SELECT_HIGHLIGHT mdHighlightStruct;
	bool mbModifyStatus; //Ϊ���ʾ��ǰ���޸�״̬��

	int miTopHeight;
	int miBottomHeight;

	EI_TOUCHINPUT_POINT mdPrePoint; //��һ�������Ϣ


};


