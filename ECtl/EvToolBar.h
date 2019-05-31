/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#ifndef		__EVTOOLBARIMP_H__
#define		__EVTOOLBARIMP_H__

DECLARE_BUILTIN_NAME(ToolBar)
class CEvToolBar:
	public CXuiElement<CEvToolBar, GET_BUILTIN_NAME(ToolBar)>
{
	friend CXuiElement<CEvToolBar, GET_BUILTIN_NAME(ToolBar)>;

public:

	ULONG InitOnCreate(
		IN IXuiIterator* npParent = NULL,	// ������ָ��
		IN ICfKey* npTemplete = NULL,		// npTemplete��Key ID����EID��ֵ��������EType
		IN ULONG nuEID=MAXULONG32	// �����Ϊ0��MAXULONG32����ָ����Ԫ�ص�EID; ����ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��XUIϵͳ�Զ�����
		) ;

protected:
	// ֻ���ڱ������ó�ʼֵ����ָ����ΪNULL�����п���ʧ�ܵ������֮������㶼Ӧ����InitOnCreate�н���
	CEvToolBar();
	// �����ͷų�Ա����
	virtual ~CEvToolBar();
	//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
	virtual ERESULT OnElementCreate(IXuiIterator* npIterator);
	//����
	//virtual ERESULT OnPaint(IXuiPaintBoard* npPaintBoard);
	// �ֽ���Ϣ���ṩ��Ϣ�ֽ����Ϣ��Ӧ�Ĺ��ܣ������ʵ���ǽ���Ϣ�ֽ�Ϊ��ͬ������󣬵�����Ӧ�Ĵ����麯�������ڲ���ʶ����Ϣ��һ�ɷ���ERESULT_UNEXPECTED_MESSAGE
	// �������ķ���ֵ���Զ�ͬ�����õ�npMsgָ�����Ϣ������
	virtual ERESULT ParseMessage(IXuiMessage* npMsg);
	//������Ϣ
	virtual ERESULT OnPaint(IXuiPaintBoard* npPaintBoard);
	//virtual ERESULT OnCtlButtonClick(IXuiIterator* npSender);

	//ERESULT LoadResource();			//װ��������Դ

public:

	static UINT_PTR CALLBACK CCHookProc(          
		HWND hdlg,
		UINT uiMsg,
		WPARAM wParam,
		LPARAM lParam
		);

private:
	bool	ProcessMsg(TOOLBAR_MSG* npMsgInfo);

	//bool	SetFontColor();
	//bool	SetFontBkgColor();

	//��ɫ��
	ERESULT __stdcall ChooseFontColorWnd(ULONG nuFlag,LPVOID npContext);
	BOOL	OpenChooseFontColorWnd();

	ERESULT __stdcall ChooseFontBkgColorWnd(ULONG nuFlag,LPVOID npContext);
	BOOL	OpenChooseFontBkgColorWnd();

	BOOL SetCtrlDefaultValue();


private:
	ULONG	mulFontColor;
	ULONG	mluFontBkgColor;

	bool mbChangeFontColor;
	bool mbChangeFontBkgColor;

private:

};

#endif		// __EVTOOLBARIMP_H__