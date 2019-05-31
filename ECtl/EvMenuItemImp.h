/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#pragma once

// ��Ÿ�PopupMenu������MenuItem����������Ϣ�Ľṹ��
struct _ST_MENUITEM_INFO
{
	LONG LeftWidth;
	LONG RightWidth;
	LONG MiddleWidth;
	LONG LeftIconWidth;
	LONG LeftIconHeight;
	LONG RightIconWidth;
	LONG RightIconHeight;

	wchar_t FontName[MAX_PATH];
	LONG FontSize;
	LONG FontColor;
	LONG FontDisableColor;
	LONG FontFocusColor;

	LONG ItemHeight;
};

typedef _ST_MENUITEM_INFO ST_MENUITEM_INFO, *PST_MENUITEM_INFO;

class CEvPopupMenu;


#define MAX_HOTKEY_LENGTH  20
#define MAX_TITTLE_LENGTH  260

DECLARE_BUILTIN_NAME(MenuItem)
class CEvMenuItem:
	public CXuiElement<CEvMenuItem, GET_BUILTIN_NAME(MenuItem)>
{
	friend CXuiElement<CEvMenuItem,GET_BUILTIN_NAME(MenuItem)>;
public:
	ULONG InitOnCreate(
		IN IEinkuiIterator* npParent = NULL,	// ������ָ��
		IN ICfKey* npTemplete = NULL,		// npTemplete��Key ID����EID��ֵ��������EType
		IN ULONG nuEID=MAXULONG32	// �����Ϊ0��MAXULONG32����ָ����Ԫ�ص�EID; ����ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��XUIϵͳ�Զ�����
		);

	enum ENUM_MENUITEM_TYPE
	{
		ENUM_MENUITEM_TYPE_SEPARATION = 0,			// �ָ���
		ENUM_MENUITEM_TYPE_NORMAL,					// ��ͨ�˵���
		ENUM_MENUITEM_TYPE_ICON,					// ��ͼ��˵���
		ENUM_MENUITEM_TYPE_CHECK,					// ��Checked̬�Ĳ˵���
		ENUM_MENUITEM_TYPE_EXTEND,					// �е����˵��Ĳ˵���
		ENUM_MENUITEM_TYPE_APPLE_EXTEND,			// ƻ��������չ�˵����Ҳ���չͼ����ȼ��Ҷ���
	};

	// ��ȡ����ͼ���
	int GetNameAreaWidth()
	{
		if(NULL != mpoTextImage)
			return mpoTextImage->GetWidth();
		else
			return -1;
	}

	// ��������������
	void SetNameAreaWidth(
		int niNameAreaWidth)
	{
		miNameAreaWidth = niNameAreaWidth;
	}

	// ��ȡ�ȼ�������
	int GetHotKeyAreaWidth()
	{
		if(NULL != mpoHotKeyImage)
			return mpoHotKeyImage->GetWidth();
		else
			return -1;
	}
	// �����ȼ�������
	void SetHotKeyAreaWidth(
		int niHotKeyAreaWidth
		)
	{
		miHotKeyAreaWidth = niHotKeyAreaWidth;
	}

	// ���³ߴ�
	void UpdateSize()
	{
		if(NULL == mpoMenuItemInfo)
			return;

		float lfWidthCount = (FLOAT)(mpoMenuItemInfo->LeftWidth + miNameAreaWidth + 
			mpoMenuItemInfo->MiddleWidth + miHotKeyAreaWidth + mpoMenuItemInfo->RightWidth);

		mpIterator->SetSize(lfWidthCount, (FLOAT)mpoMenuItemInfo->ItemHeight);
	}

	// ������
	//		���ó�ʼ���ṹ���ָ�루����֮����˵������ָ�룬��Ϊ�˱�ʾ����Ҫȥ�ͷţ�
	void SetMenuItemInfo(IN PST_MENUITEM_INFO npoMenuItemInfo);

	// ������
	//		��ȡ��ǰ�˵��������
	ENUM_MENUITEM_TYPE GetMenuItemType()
	{
		return meMemuItemType;
	}

	// ������
	//		���õ�ǰ�˵�������
	void SetMenuItemType(IN ENUM_MENUITEM_TYPE neMenuItemType)
	{
		meMemuItemType = neMenuItemType;
	}

	// ������
	//		��ȡ��ǰ�˵���ĵ����˵�
	CEvPopupMenu* GetPopupMenu()
	{
		if(NULL == mpoSubPopupMenu)
		{
			LoadSubPopupMenu();
		}

		if(NULL != mpoSubPopupMenu 
			&&(meMemuItemType == ENUM_MENUITEM_TYPE_APPLE_EXTEND 
			|| meMemuItemType == ENUM_MENUITEM_TYPE_EXTEND)
			)
			return mpoSubPopupMenu;
		else
			return NULL;
	}

	// ������
	//		��ȡ��ǰ�˵����CommandID
	int GetCommandID()
	{
		return miCommandID;
	}

	// ������
	//		���õ�ǰ�ŵ����CommandID
	void SetCommandID(
		IN UINT niCommandID
		)
	{
		miCommandID = niCommandID;
	}

	// ������
	//		���ñ���ͼ
	void SetBgBitmapPtr(
		IN IEinkuiBitmap* niBitmap,
		IN int niFrameCount
		)
	{
		CMM_SAFE_RELEASE(mpoItemBgBmp);
		mpoItemBgBmp = niBitmap;

		miItemFrameCount = niFrameCount;
	}

	// ������
	//		���ؼ����˵�
	void HideCascadeMenu();

protected:
	CEvMenuItem(void);
	~CEvMenuItem(void);

	//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
	virtual ERESULT OnElementCreate(IEinkuiIterator* npIterator);
	//// ����Ԫ�أ�����֪ͨһ��Ԫ�����٣�ע�⣺��Ԫ�������յ�����Ϣ��Ӧ�ü�ʱ����Ԫ�ع�������UnregisterElement������
	//// �Ӷ�����Ԫ�ع�������������һ��Ԫ�ط���������Ϣ�������ٽ��Լ���Ԫ�ع�����ע���������ͷ��������
	//virtual ERESULT OnElementDestroy();

	//������Ϣ
	virtual ERESULT OnPaint(IEinkuiPaintBoard* npPaintBoard);

	//��갴��
	virtual ERESULT OnMousePressed(const STEMS_MOUSE_BUTTON* npInfo);

	virtual ERESULT ParseMessage(IEinkuiMessage* npMsg);

	//��������뿪
	virtual void OnMouseFocus(PSTEMS_STATE_CHANGE npState);

	//��ʱ��
	virtual void OnTimer(
		PSTEMS_TIMER npStatus
		);

	// ��������
	ERESULT OnMouseOwnerTest(const D2D1_POINT_2F& rPoint);

	//���û�����
	virtual ERESULT OnElementEnable(bool nbIsEnable);

	//�����ͣ
	ERESULT OnMouseHover();

	//��ݼ���Ϣ
	virtual ERESULT OnHotKey(const STEMS_HOTKEY* npHotKey);

private:
	void LoadResource();

	// �����Ӳ˵��б�
	void LoadSubPopupMenu();

	// ������
	//		���ز˵�������ͼƬ����������������ʼ����������mpoTextImage poFocusTextImage
	// ����ֵ��
	//		-1 ��ʾʧ�ܣ����򷵻ش�����ͼƬ�����߶�
	//		ע�⣺�������ָ�����ָ����ַ���ָ�룬����������ͼƬ���ȼ�ͼƬ�����ظ߶�Ϊ0
	long LoadTextImage(
		IN wchar_t* nswMenuName					// �˵��ı�
		);

	// ������
	//		���ز˵����ȼ�ͼƬ����������������ʼ����������mpoHotKeyImage poFocusHotKeyImage
	// ����ֵ��
	//		-1 ��ʾʧ�ܣ����򷵻ش�����ͼƬ�����߶�
	//		ע�⣺�������ָ�����ָ����ַ���ָ�룬�������ȼ�ͼƬ�����ظ߶�Ϊ0
	long LoadHotkeyImage(IN PSTCTL_MENUITEM_HOTKEY npdHotKeyInfo);


protected:
	ENUM_MENUITEM_TYPE meMemuItemType;
	int miCommandID;					// �ò˵����Ψһ��ʶID
	int miShotcutKey;					// �ò˵�����ȼ�
	wchar_t mswMenuTittle[MAX_TITTLE_LENGTH];
	wchar_t mswHotKey[MAX_HOTKEY_LENGTH];
	PST_MENUITEM_INFO mpoMenuItemInfo;
	
	IEinkuiBitmap* mpoHotKeyImage;		// ��ݼ�ͼ
	IEinkuiBitmap* mpoFocusHotKeyImage;	// ����̬��ݼ�ͼ
	IEinkuiBitmap* mpoFocusTextImage;	// ����̬�˵�����ͼ
	IEinkuiBitmap* mpoTextImage;		// �˵�����ͼ
	IEinkuiBitmap* mpoLeftImage;		// ������ߵ�ͼ
	IEinkuiBitmap* mpoCheckedImage;		// Checked̬ͼ
	IEinkuiBitmap* mpoRightImage;		// �����ұߵ�ͼ
	IEinkuiBitmap* mpoSeparation;		// �ָ���ͼ
	IEinkuiBitmap* mpoItemBgBmp;		// ����ͼָ��

	int miItemFrameCount;			// ����ͼƬ��֡��

	int miNameAreaWidth;
	int miHotKeyAreaWidth;

	bool mbIsChecked;				// �Ƿ�Checked̬
	bool mbIsShowExtendMenu;		// �Ƿ���ʾ��չ�˵�
	
	int miExtendMenuID;				// �����˵�ID

	CEvPopupMenu* mpoSubPopupMenu;
	bool mbIsSubMenuShow;

	LONG miDuration;			

	bool mbIsFocused;



	LONG mlHotKeyID;				// �ȼ�ID�����ȼ���϶���      Ctrl | Shift | Alt | VirtualKey

};

#define MENUITEM_HOTKEY_ATL			0x00000100
#define MENUITEM_HOTKEY_CTRL		0x00000200
#define MENUITEM_HOTKEY_SHIFT		0x00000400






#define TIMER_ID_SHOW_POPUPMENU			1
#define DEFAULT_TIMER_DURATION			500

#define TF_ID_MENUITEM_COMMAND_ID		L"ID"						// �˵����Ψһ��ʶID
#define TF_ID_MENUITEM_TYPE				L"Type"						// �˵�������,����ֵ��ö������ENUM_MENUITEM_TYPE�Ķ���
#define TF_ID_MENUITEM_SEPARATION		L"Separation"				// �ָ���ͼ
#define TF_ID_MENUITEM_TEXT				L"Text"						// �˵������֣� ����п�ݼ���Ҳ�������ֺ����� �½�(&N)

// �ȼ�
#define TF_ID_MENUITEM_HOTKEY			L"HotKey"					// �ȼ���ʾ�ַ�  �� Shift + X
#define TF_ID_MENUITEM_HOTKEY_VIRTUAL_KEY L"HotKey/VirtualKey"		// �����ֵ�������У����������ȼ�����������ע�⣬������ַ��������Ǵ�д�ģ���ΪСд���ַ���������ֵռ���ˣ�
#define TF_ID_MENUITEM_HOTKEY_SHAFT_KEY	L"HotKey/ShiftKey"			// ָ���Ƿ���Ҫʹ��Shift�������������������ʾ��Ҫ�������ڣ����ʾ����Ҫ��
#define TF_ID_MENUITEM_HOTKEY_CTRL_KEY	L"HotKey/CtrlKey"			// ָ���Ƿ���Ҫʹ��Ctrl�������������������ʾ��Ҫ�������ڣ����ʾ����Ҫ��
#define TF_ID_MENUITEM_HOTKEY_ALT_KEY	L"HotKey/AltKey"			// ָ���Ƿ���Ҫʹ��Alt�������������������ʾ��Ҫ�������ڣ����ʾ����Ҫ��

// �����ȼ�
#define TF_ID_MENUITEM_RESERVE_HOTKEY	L"ReserveHotKey"			// �������õ��ȼ�������ȼ���������ʾ�ģ�ֻ����������ͨ�ȼ�֮��Ż�ȥע������ȼ�
#define TF_ID_MENUITEM__RESERVE_HOTKEY_VIRTUAL_KEY L"ReserveHotKey/VirtualKey"		// �����ֵ�������У����������ȼ�����������ע�⣬������ַ��������Ǵ�д�ģ���ΪСд���ַ���������ֵռ���ˣ�
#define TF_ID_MENUITEM__RESERVE_HOTKEY_SHAFT_KEY	L"ReserveHotKey/ShiftKey"			// ָ���Ƿ���Ҫʹ��Shift�������������������ʾ��Ҫ�������ڣ����ʾ����Ҫ��
#define TF_ID_MENUITEM__RESERVE_HOTKEY_CTRL_KEY	L"ReserveHotKey/CtrlKey"			// ָ���Ƿ���Ҫʹ��Ctrl�������������������ʾ��Ҫ�������ڣ����ʾ����Ҫ��
#define TF_ID_MENUITEM__RESERVE_HOTKEY_ALT_KEY	L"ReserveHotKey/AltKey"			// ָ���Ƿ���Ҫʹ��Alt�������������������ʾ��Ҫ�������ڣ����ʾ����Ҫ��

// (��ݼ���ʱ��֧��)

#define TF_ID_MENUITEM_CHECKED_TEXT		L"CheckedText"				// Checked״̬�²˵������֣��ȼ��Ϳ�ݼ����ú�MFC�����÷�����ͬ. �� �½�(&N)\tCtrl+N
#define TF_ID_MENUITEM_LEFT_ICON		L"LeftIcon"					// �˵�������ǰ���ͼ��
#define TF_ID_MENUITEM_LEFT_CHECKED_ICON L"LeftCheckedIcon"			// ������Checked״̬��ͼ��
#define TF_ID_MENUITEM_RIGHT_ICON		L"RightIcon"				// �˵������ƺ����ͼ��
#define TF_ID_MENUITEM_POPUPMENU_ID		L"PopupMenuID"				// Ҫ�����Ĳ˵���ID
#define TF_ID_MENUITEM_DURATION			L"Duration"					// �����Ӳ˵������ڣ��Ժ���Ϊ��λ��Ĭ��Ϊ500����

