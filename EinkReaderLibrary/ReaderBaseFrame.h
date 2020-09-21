/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#pragma once
#include <tuple>
#include "ToolbarH.h"
#include "PdfPicture.h"
#include "FileOpenDlg.h"
#include "TipFrame.h"
#include "JumpPage.h"
#include "PreNextButton.h"
#include "SnapShot.h"
#include "ToolbarBottom.h"
#include "FileHistoryDlg.h"
#include "Highlight.h"
#include "ThumbnailDlg.h"
#include "FileOpenFailDlg.h"

#include "CYesNoPromptDlg.h"
#include "PDFOverwriteDlg.h"
#include "ConvertProgressDlg.h"
#include "AskConvertDlg.h"

/*
	������Ϊ�Ի���ͨ�û�����ܶ���ʹ�ã�����Ի����ͨ�û����ؼ�����Ϊ
*/

DECLARE_BUILTIN_NAME(Reader_BaseFrame)

#define ZCT_FONTSIZE_LEVEL 5 //5������

using std::tuple;

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

	ZoomStatus GetZoomStatus();

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
	//����windows������Ϣ
	ERESULT __stdcall OnInputChangeMsg(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& Result);
	//������̬�仯֪ͨ
	ERESULT __stdcall OnModeChanged(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& Result);
	//���ļ���ָ����Ϣ
	ERESULT __stdcall OnOpenOfficeFile(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& Result);
	// Open Office Docs with 'TimeStamp' method. [zhuhl5@20200121]
	ERESULT __stdcall OnOpenOfficeFileWithTS(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& Result);

	void InternalOpenOfficeFile(uint64_t timeStamp);

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
	bool GetAskConvertStatus();
	bool showAskDlg();

private:
	CLoadingView* mpLoadingView;
	CToolbarH* mpToolbarH;
	CPdfPicture* mpPdfPicture;
	IEinkuiIterator* mpIterBackground;
	COfficeConvertDlg *m_pDemoDlg = nullptr;
	CFileOpenFailDlg* m_fileOpenFailDlg = nullptr;

	CYesNoPromptDlg* promptDlg;
	CPDFOverwriteDlg* pdfOverwriteDlg;
	CConvertProgressDlg* convertProgressDlg;
	CAskConvertDlg* askConvertDlg;
	
	CFileHistoryDlg* mpFileHistoryDlg;
	CJumpPage* mpJumpPage;
	CTipFrame* mpTipFrame;	//��ʾ��ʾ/���ع�����
	CToolbarBottom* mpToolbarBottom; //�ײ�������
	CHighlight* mpHighlight; //����ѡ�����
	CThumbnailDlg* mpThumbnailDlg; //����ͼ����
	//ULONG mulCurrentPageNumber; //��ǰҳ��
	CPreNextButton* mpPreNextButton;
	CSnapShot* mpSnapShot;
	cmmVector<HISTORY_FILE_ATTRIB*> mdHistroyPath; //��ʷ�ļ�
	wstring m_cmdLineOpenFileName;
	IEinkuiIterator* mpIterToast;
	volatile LONG mlHoldInput;
	DWORD mdwFontSizeArray[ZCT_FONTSIZE_LEVEL];
	float mfArrayPenWidthArray[5]; //�ʿ�
	BYTE mfArrayFwPenWidthArray[5]; //FW���߱ʿ�
	BYTE mfArrayFwFingerWidthArray[5]; //FW������ָ���߿�

	DWORD mdwFontsizeIndex;
	DWORD mdwPenWidthIndex;
	DWORD mdwPenColorIndex;
	ULONG mulPenMode; //��ǰ״̬����/��Ƥ/ѡ��
	bool mbIsHand; //Ϊ���ʾ��ָ�����ڻ���

	ULONG muLastGcTick;
	ULONGLONG mxLastGcUiNumber;
	BOOL mbGcMode;
	bool mbIsTxt; //�򿪵��Ƿ���txt�ļ�
	wchar_t mszTempFile[MAX_PATH];  //Ϊ�˴�Ч�ʣ����ļ����嵽programdataĿ¼��
	wchar_t mszSrcFile[MAX_PATH]; //���ļ���ʵ��·��
	bool mbIsSetPartial; //���Ϊfalse,�򲻹ر�partial
	ULONG mulPageIndex; //ҳ��
	ULONG mulPageCount; //��ҳ��
	//HANDLE mhFile;
	bool mbIsLoadingSuccess; //�ļ������Ƿ����
	bool mbIsScreenAuto; //Ϊ���ʾ�Զ���ת
	bool lockvertical;
	bool enableCapacitivepen; //�Ƿ������ݱʶ�����ť������Ӧ

	ZoomStatus mZoomStatus = ZoomStatus::NONE;
	MoveForward mMoveForward;

	//����ҳ����ת���ڵ�λ��
	void SetJumpPagePos(void);
	//�û�ѡ����Ҫ�򿪵��ļ�,npFileAttrib!=NULL��ʾ����ʷ��¼�е��ļ�
	bool OpenFile(wchar_t* npszFilePath, HISTORY_FILE_ATTRIB* npFileAttrib);
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

	tuple<bool, wstring, bool> ConvertAndOpenOfficeFile(const wchar_t* filePath);

	//�������ض�ʱ��
	void ResetHideTime(bool lbIsOpen = true);
	//�����ʷ�ļ�״̬����������ڵ��ļ������
	void CheckHistoryList(void);
	//��ʾ�ļ���ҳ��
	void ShowFileOpenDlg(void);
	//���ñʿ�
	void SetHandWriteWidth();
	//�������ͼ�ļ�
	void ClearThumbnail(const wchar_t* npszPath);
	// ��ʾ��ʧ�ܶԻ���
	void ShowFileOpenFailDialog(const wchar_t* resourceName);
	// ����Ԫ�ؾ���
	void PlaceChildInCenter(IEinkuiIterator* childIterator);
	//�Զ�����
	void PageAutoZoom();
};

#define  RBF_HISTROY_MAX 12 //�����ʷ��¼


#define  RbF_REG_PAGE_NUMBER L"PageNumber" //�ϴιر�ʱ��ʾ��ҳ��
#define  RbF_REG_PAGE_CONTENT L"PageContent" //�ϴιر�ʱ��ʾ��ҳ��Ĺ�������
#define  RbF_REG_PAGE_CONTENT2 L"PageContent2" //�ϴιر�ʱ��ʾ��ҳ��Ĺ�������
#define  RbF_REG_DOUBLE_SCREEN L"DoubleScreen" //������˫��
#define  RbF_REG_RATIO L"Ratio" //�Ŵ����
#define  RbF_REG_TXT_FONT_SIZE_INDEX L"TxtFontSizeIndex" //Txt�Ŵ���������±�
#define  RbF_REG_PEN_WIDTH_INDEX L"PenWidthIndex" //�ʿ�������±�
#define  RbF_REG_PEN_COLOR_INDEX L"PenColorIndex" //����ɫ
#define  RbF_REG_LOG L"LogFile" //��־����
#define  RbF_REG_ONCE L"Once" //0��û�б�ʾ��һ�δ�
#define  RbF_REG_SCREEN_ORI L"Screen_ori" //��Ļ����10=��ϵͳ��11=���� 12=����
#define  RbF_REG_B_COVER L"BCoverState" //�������ǹر�B����

#define RBF_TIMER_TOAST 1
#define RBF_TIMER_TSHOW_TOOLBAR 2
#define RBF_TIMER_ENABL_PARTIAL 3 //����partial,��Ϊ�л�������һ֡Ҫ�ر�partial,������ܳ���ˢ�²���������
#define RBF_TIMER_INIT 4 //Ϊ�˽��̿���ʱ�������ʾ��ʹ�ö�ʱ��������ʼ��
#define RBF_TIMER_EXIT 5 //�յ��л�����̨��Ϣ��3��ر��Լ�����
#define RBF_TIMER_HOLDINPUT 6 //�Զ������������
#define RBF_TIMER_ACTIVITY 7 //��̨�л���ǰ̨


//�ļ�����ʷ��¼
#define RBF_REG_HISTORY_PATH L"historyPath" 
#define RBF_REG_HISTORY_THUMBNAIL_PATH L"historyThumbanilPath" 
#define RBF_REG_HISTORY_MODIFY L"historyModify" 
#define RBF_REG_HISTORY_PROGRESS L"historyProgress" 
#define RBF_REG_HISTORY_TIMESTAMP L"historyTimestamp" 

// Scaling data name[zhuhl5@20200116]
#define RBF_REG_HISTORY_AUTO_ZOOM L"historyAutoZoom" 
#define RBF_REG_HISTORY_SCALING L"historyScaling" 
#define RBF_REG_HISTORY_SCALINGLEVEL L"historyScalingLevel" 
#define RBF_REG_HISTORY_SCALINGPOSX L"historyScalingPosX"
#define RBF_REG_HISTORY_SCALINGPOSY L"historyScalingPosY"