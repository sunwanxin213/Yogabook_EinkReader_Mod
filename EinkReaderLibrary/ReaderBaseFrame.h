/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#pragma once
#include "ToolbarH.h"
#include "PdfPicture.h"
#include "FileOpenDlg.h"
#include "TipFrame.h"
#include "JumpPage.h"
#include "PreNextButton.h"
#include "ZoomControl.h"
#include "ZoomControltxt.h"
#include "SnapShot.h"

/*
	������Ϊ�Ի���ͨ�û�����ܶ���ʹ�ã�����Ի����ͨ�û����ؼ�����Ϊ
*/

DECLARE_BUILTIN_NAME(Reader_BaseFrame)

class CReaderBaseFrame:
	public CXuiElement<CReaderBaseFrame,GET_BUILTIN_NAME(Reader_BaseFrame)>
{
	friend CXuiElement<CReaderBaseFrame,GET_BUILTIN_NAME(Reader_BaseFrame)>;

public:
	ULONG InitOnCreate(
		IN IEinkuiIterator* npParent = NULL,	// ������ָ��
		IN ICfKey* npTemplete = NULL,		// npTemplete��Key ID����EID��ֵ��������EType
		IN ULONG nuEID = MAXULONG32	// �����Ϊ0��MAXULONG32����ָ����Ԫ�ص�EID; ����ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��XUIϵͳ�Զ�����
		);

protected:
	CReaderBaseFrame(void);
	~CReaderBaseFrame(void);

	//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
	virtual ERESULT OnElementCreate(IEinkuiIterator* npIterator);
	//��Ϣ������
	virtual ERESULT ParseMessage(IEinkuiMessage* npMsg);
	//��ʱ��
	virtual void OnTimer(
		PSTEMS_TIMER npStatus
		);
	//����֪ͨӦ���л���ǰ̨���̨
	ERESULT __stdcall OnActivity(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& Result);
	//homebar״̬�仯֪ͨ
	ERESULT __stdcall OnHomebarChanged(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& Result);
	//Tp��reset�ˣ���Ҫ��������tp area
	ERESULT __stdcall OnNotifyResetTpArea(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& Result);
	//�յ���Դ�仯��Ϣ
	ERESULT __stdcall OnPowerChangeMsg(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& Result);

	//��ť�����¼�
	virtual ERESULT OnCtlButtonClick(IEinkuiIterator* npSender);

	//��������
	void HoldInput(bool nbSet=true);

	//����
	virtual ERESULT OnPaint(IEinkuiPaintBoard* npPaintBoard);
	//Ԫ�زο��ߴ緢���仯
	virtual ERESULT OnElementResized(D2D1_SIZE_F nNewSize);

	void OnRotated(ULONG nuOrient);
	static bool CopyFileThread(LPVOID npData);
private:
	CLoadingView* mpLoadingView;
	CToolbarH* mpToolbarH;
	CPdfPicture* mpPdfPicture;
	IEinkuiIterator* mpIterBackground;
	CFileOpenDlg* mpFileOpenDlg;
	CJumpPage* mpJumpPage;
	CTipFrame* mpTipFrame;	//��ʾ��ʾ/���ع�����
	//ULONG mulCurrentPageNumber; //��ǰҳ��
	CPreNextButton* mpPreNextButton;
	CZoomControl* mpZoomControl;
	CZoomControlTxt* mpZoomControlTxt;
	CSnapShot* mpSnapShot;
	cmmVector<wchar_t*> mdHistroyPath; //��ʷ�ļ�
	IEinkuiIterator* mpIterToast;
	volatile LONG mlHoldInput;
	DWORD mdwFontSizeArray[ZCT_FONTSIZE_LEVEL];
	DWORD mdwFontsizeIndex;

	ULONG muLastGcTick;
	ULONGLONG mxLastGcUiNumber;
	BOOL mbGcMode;
	bool mbIsTxt; //�򿪵��Ƿ���txt�ļ�
	wchar_t mszTempFile[MAX_PATH];  //Ϊ�˴�Ч�ʣ����ļ����嵽programdataĿ¼��
	wchar_t mszSrcFile[MAX_PATH]; //���ļ���ʵ��·��
	bool mbIsSetPartial; //���Ϊfalse,�򲻹ر�partial
	ULONG mulPageIndex; //ҳ��

	//���ô��ļ����ڵ�λ��
	void SetOpenFilePos(void);
	//����ҳ����ת���ڵ�λ��
	void SetJumpPagePos(void);
	//�û�ѡ����Ҫ�򿪵��ļ�
	bool OpenFile(wchar_t* npszFilePath);
	//��ʼ��
	void Init(void);
	////��ҳ
	//void GotoPage(ULONG nulPage);
	void ShowPageInfo(void);
	//��ʾtoast
	void ShowToast(wchar_t* npszKeyName);
	//��ʾ�����ع�����
	void ShowToolBar(bool nbIsShow);
	//��Ҫ�򿪵��ļ�copy����ʱĿ¼
	void CopyFileToTemp(IN wchar_t* npszSrc, OUT wchar_t* npszDest, IN LONG nlLen);
	//��ȡ�ļ���С
	ULONG GetFileSize(wchar_t* npszFilePath);

	static ERESULT __stdcall EinkUpdating(ULONGLONG nxNumber, CReaderBaseFrame* npThis);	// nuNumber�Ǹ�����ţ�ÿ�θ��¼�һ���ﵽ���ֵ��ص���


};

#define  RBF_HISTROY_MAX 4 //�����ʷ��¼


#define  RbF_REG_PAGE_NUMBER L"PageNumber" //�ϴιر�ʱ��ʾ��ҳ��
#define  RbF_REG_PAGE_CONTENT L"PageContent" //�ϴιر�ʱ��ʾ��ҳ��Ĺ�������
#define  RbF_REG_PAGE_CONTENT2 L"PageContent2" //�ϴιر�ʱ��ʾ��ҳ��Ĺ�������
#define  RbF_REG_DOUBLE_SCREEN L"DoubleScreen" //������˫��
#define  RbF_REG_RATIO L"Ratio" //�Ŵ����
#define  RbF_REG_TXT_FONT_SIZE_INDEX L"TxtFontSizeIndex" //Txt�Ŵ���������±�

#define RBF_TIMER_TOAST 1
#define RBF_TIMER_TSHOW_TOOLBAR 2
#define RBF_TIMER_ENABL_PARTIAL 3 //����partial,��Ϊ�л�������һ֡Ҫ�ر�partial,������ܳ���ˢ�²���������
#define RBF_TIMER_INIT 4 //Ϊ�˽��̿���ʱ�������ʾ��ʹ�ö�ʱ��������ʼ��
#define RBF_TIMER_EXIT 5 //�յ��л�����̨��Ϣ��3��ر��Լ�����
#define RBF_TIMER_HOLDINPUT 6 //�Զ������������