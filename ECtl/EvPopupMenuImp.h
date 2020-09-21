#pragma once

// ��Ÿ�PopupMenu������MenuItem����������Ϣ�Ľṹ��
struct _ST_MENUITEM_INFO;
typedef _ST_MENUITEM_INFO ST_MENUITEM_INFO, *PST_MENUITEM_INFO;

class CEvMenuItem;

typedef  cmmVector<CEvMenuItem*, 5, 5>  CMenuItemVec;

DECLARE_BUILTIN_NAME(PopupMenu)
class CEvPopupMenu :
	public CXuiElement<CEvPopupMenu, GET_BUILTIN_NAME(PopupMenu)>
{
	friend CXuiElement<CEvPopupMenu, GET_BUILTIN_NAME(PopupMenu)>;

public:
	ULONG InitOnCreate(
		IN IEinkuiIterator* npParent = NULL,	// ������ָ��
		IN ICfKey* npTemplete = NULL,		// npTemplete��Key ID����EID��ֵ��������EType
		IN ULONG nuEID = MAXULONG32	// �����Ϊ0��MAXULONG32����ָ����Ԫ�ص�EID; ����ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��XUIϵͳ�Զ�����
	);

	// ������
	//		��ȡ����MainID��PopupMenu(����MenuItem����������PopupMenu)
	// ����ֵ��
	//		�ɹ����ض�ӦID�ĵ�������ʧ�ܷ���NULL
	IEinkuiIterator* GetPopupMenuByMainID(
		IN UINT niUinqueID
	);

protected:
	CEvPopupMenu(void);
	~CEvPopupMenu(void);

	//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
	virtual ERESULT OnElementCreate(IEinkuiIterator* npIterator);
	//// ����Ԫ�أ�����֪ͨһ��Ԫ�����٣�ע�⣺��Ԫ�������յ�����Ϣ��Ӧ�ü�ʱ����Ԫ�ع�������UnregisterElement������
	//// �Ӷ�����Ԫ�ع�������������һ��Ԫ�ط���������Ϣ�������ٽ��Լ���Ԫ�ع�����ע���������ͷ��������
	//virtual ERESULT OnElementDestroy();

	//������Ϣ
	virtual ERESULT OnPaint(IEinkuiPaintBoard* npPaintBoard);

	virtual ERESULT ParseMessage(IEinkuiMessage* npMsg);

	//֪ͨԪ�ء���ʾ/���ء������ı�
	virtual ERESULT OnElementShow(bool nbIsShow);
private:
	void LoadResource();

	// ��ʼ��MenuItemInfo
	void InitMenuItemInfo();

	// ������
	//		�������Ӳ˵����ý���
	void OnMenuItemGetFocused(
		IN IEinkuiIterator* npIterMenuItem
	);

	// ������
	//		�����µĲ˵���
	bool OnInsertMenuItem(
		IN STCTL_POPUPMENU_MENUITEMINFO ndMenuInfo
	);

	// ������
	//		���¼���˵���Ĳ���
	void ReLayoutMenuItem();

	// ������
	//		����CommandIDɾ���˵���
	bool DeleteItemByCommandID(
		IN UINT niCommandID
	);

	// ������
	//		����CommandID��ȡ�˵���
	IEinkuiIterator* GetItemByCommandID(
		IN UINT niCommandID
	);

	// ������
	//		��������ɾ���˵���,����-1ɾ��ȫ����
	bool DeleteItemByIndex(
		IN int niIndex
	);

	// ������
	//		����������ȡ�˵���
	IEinkuiIterator* GetItemByIndex(
		IN UINT niIndex
	);

private:

	IEinkuiBitmap* mpoItemBgBitmap;

	int miMainID;
	PST_MENUITEM_INFO mpoMenuItemInfo;

	CMenuItemVec moMenuItemVec;

	int miItemBgFrameCount;

	bool mbIsManagerMenuItemEnable;				// ��ʾ��ʱ���Ƿ����˵��Ŀ���״̬
};

// MenuItem��ʾ��λ��
#define TF_ID_POPUPMENU_ALIGN_LEFT			L"Align/Left"		
#define TF_ID_POPUPMENU_ALIGN_RIGHT			L"Align/Right"
#define TF_ID_POPUPMENU_ALIGN_TOP			L"Align/Top"	
#define TF_ID_POPUPMENU_ALIGN_BOTTOM		L"Align/Bottom"	

#define TF_ID_POPUPMENU_MAIN_ID				L"ID"				// ��PopupMenu��ΨһID
#define TF_ID_POPUPMENU_MENU_ITEM			L"MenuItem"			// ���MenuItem�Ľ��

#define TF_ID_POPUPMENU_TEXT_WIDTH			L"TextWidth"		// ��������˵��������ֿ�ȣ��������������Ҫ���ṩ��ComboBoxʹ�ã���Ϊ�䵯���˵�ͨ���ǹ̶���ȵģ�

#define TF_ID_POPUPMENU_LEFT_WIDTH			L"LeftWidth"
#define TF_ID_POPUPMENU_RIGHT_WIDTH			L"RightWidth"
#define TF_ID_POPUPMENU_MIDDLE_WIDTH		L"MiddleWidth"
#define TF_ID_POPUPMENU_LEFT_ICON_WIDTH		L"LeftIconWidth"
#define TF_ID_POPUPMENU_LEFT_ICON_HEIGHT	L"LeftIconHeight"
#define TF_ID_POPUPMENU_RIGHT_ICON_WIDTH	L"RightIconWidth"
#define TF_ID_POPUPMENU_RIGHT_ICON_HEIGHT	L"RightIconHeight"
#define TF_ID_POPUPMENU_ITEM_HEIGHT			L"ItemHeight"		// ��ȥ�ָ�������ÿһ��ĸ߶ȣ�

#define TF_ID_POPUPMENU_ITEM_BACKGROUND		L"ItemBackGround"	// �˵���ı���
#define TF_ID_POPUPMENU_ITEM_FRAMECOUNT		L"ItemFrameCount"	// �˵������֡��

#define TF_ID_POPUPMENU_FONT_NAME			L"Font/Name"			
#define TF_ID_POPUPMENU_FONT_SIZE			L"Font/Size"			
#define TF_ID_POPUPMENU_FONT_COLOR			L"Font/Color"		
#define TF_ID_POPUPMENU_FONT_DISABLE_COLOR	L"Font/DisableColor"// ����̬�˵���������ɫ
#define TF_ID_POPUPMENU_FONT_FOCUS_COLOR	L"Font/FocusColor"// ����̬�˵���������ɫ





// һЩĬ�ϲ���ֵ
#define DEFAULT_LEFT_WIDTH					40
#define DEFAULT_RIGHT_WIDTH					40
#define DEFAULT_MIDDLE_WIDTH				20
//#define DEFAULT_LEFT_ICON_WIDTH			20
//#define DEFUALT_LEFT_ICON_HEIGHT			20
//#define DEFAULT_RIGHT_ICON_WIDTH			20
//#define DEFAULT_RIGHT_ICON_HEIGHT			20

#define DEFAULT_ITEM_FRAMECOUNT				2

#define DEFAULT_FONT_NAME					L"Tahoma"
#define DEFAULT_FONT_SIZE					15
#define DEFAULT_FONT_COLOR					0x000000
#define DEFAULT_FONT_DISABLE_COLOR			0x7d7d7d00
#define DEFAULT_FONT_FOCUS_COLOR			0xffffff00




