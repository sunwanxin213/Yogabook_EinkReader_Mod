/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#pragma once

/*
	����ʾ�ķ�ҳ���м书������ʾ���ع��ܵİ�ť
*/

#include "PdfPicture.h"
#include "Highlight.h"
#include "Define.h"

DECLARE_BUILTIN_NAME(PreNextButton)

class CPreNextButton:
	public CXuiElement<CPreNextButton,GET_BUILTIN_NAME(PreNextButton)>
{
	friend CXuiElement<CPreNextButton,GET_BUILTIN_NAME(PreNextButton)>;

public:
	ULONG InitOnCreate(
		IN IEinkuiIterator* npParent = NULL,	// ������ָ��
		IN ICfKey* npTemplete = NULL,		// npTemplete��Key ID����EID��ֵ��������EType
		IN ULONG nuEID = MAXULONG32	// �����Ϊ0��MAXULONG32����ָ����Ԫ�ص�EID; ����ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��XUIϵͳ�Զ�����
		);

	//���ñ�״̬
	void SetPenMode(ULONG nulPenMode);
	//�����ĵ�����ָ��
	void SetPdfPicture(CPdfPicture* npPdfPicture, CHighlight* npHighlight);
	//�����Ƿ�������״̬,nbIsZoomΪ���ʾ��ǰ������״̬
	void SetZoomStatus(ZoomStatus status);
	//���û�������
	void SetMoveForward(MoveForward forward);
	//�������Ƿ�������ڻ���
	void SetHandWrite(bool nbIsHand);
	//��ȡ��д״̬
	bool GetHandWrite(void);
	//�������������߶Ⱥ����������߶�
	void SetToolbarHeight(int niTopHeight, int niBottomHeight);
	//�����ļ�����
	void SetFileAttrib(DWORD ndwAttrib);
	//���û�������
	void SetFwLineRect(ED_RECT ndRect);
protected:
	CPreNextButton(void);
	~CPreNextButton(void);

	//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
	virtual ERESULT OnElementCreate(IEinkuiIterator* npIterator);
	//��ʱ��
	virtual void OnTimer(
		PSTEMS_TIMER npStatus
		);
	//��Ϣ������
	virtual ERESULT ParseMessage(IEinkuiMessage* npMsg);
	//��ť�����¼�
	//virtual ERESULT OnCtlButtonClick(IEinkuiIterator* npSender);
	//Ԫ�زο��ߴ緢���仯
	virtual ERESULT OnElementResized(D2D1_SIZE_F nNewSize);
	//֪ͨԪ�ء���ʾ/���ء������ı�
	virtual ERESULT OnElementShow(bool nbIsShow);
	// ��������
	virtual ERESULT OnMouseOwnerTest(const D2D1_POINT_2F& rPoint);
	//��갴��
	virtual ERESULT OnMousePressed(const STEMS_MOUSE_BUTTON* npInfo);
	//�����¼�
	ERESULT __stdcall OnTouchMsg(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& Result);
	//�����¼�
	void TouchMsgPro(PEI_TOUCHINPUT_POINT npTouch);
	//����FW��������
	void ResetFwRect(bool nbIsClose = false);
	//����ҳ�¼�
	void ProcessPageNext(D2D1_POINT_2F ndPos);
	//����ҳ���ƶ�
	void ProcessPageMove(D2D1_POINT_2F ndpPos);

private:
	IEinkuiIterator* mpIterBtPre;		//��һҳ
	IEinkuiIterator* mpIterBtMiddle;	//�м�
	IEinkuiIterator* mpIterBtNext;		//��һҳ

	MoveForward mMoveForward = MoveForward::HORIZONTAL_VERTICAL;

	D2D1_POINT_2F mdPressPos;
	float mfLeftButton; //�����жϵ���Ƿ��Ƿ�ҳ����
	DWORD mdwClickTicount; //�����жϳ��� ����500ms�㳤��
	CPdfPicture* mpPdfPicture;
	CHighlight* mpHighlight;
	ULONG mulPenMode;

	bool mbIsPressed;
	ZoomStatus mZoomStatus = ZoomStatus::NONE;
	bool mbIsHand; //���Ƿ�����ڻ���
	bool mbIsFwRect; //true��ʾ����fw����

	int miTopHeight;
	int miBottomHeight;
	DWORD mdwAttrib;
	ED_RECT mdFwLineRect; //FW��������
	ULONG mulPointCount; //��¼�ѷ��͵�����
	DWORD mdwLastPenTicket; //���һ�α��¼�ʱ���
	EI_TOUCHINPUT_POINT mdPrePoint; //��һ�������Ϣ
};

#define PNB_BT_MIDDLE_ZOOM 99  //�Ŵ�״̬���м�
#define PNB_BT_MIDDLE 100  //�м�
#define PNB_BT_PRE 101	//��һҳ
#define PNB_BT_NEXT 102 //��һҳ