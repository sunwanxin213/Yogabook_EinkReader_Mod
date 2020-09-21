#pragma once

DECLARE_BUILTIN_NAME(MenuButton)
class CEvMenuButton :
	public CXuiElement<CEvMenuButton, GET_BUILTIN_NAME(MenuButton)>
{
	friend CXuiElement<CEvMenuButton, GET_BUILTIN_NAME(MenuButton)>;
public:
	ULONG InitOnCreate(
		IN IEinkuiIterator* npParent = NULL,	// ������ָ��
		IN ICfKey* npTemplete = NULL,		// npTemplete��Key ID����EID��ֵ��������EType
		IN ULONG nuEID = MAXULONG32	// �����Ϊ0��MAXULONG32����ָ����Ԫ�ص�EID; ����ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��XUIϵͳ�Զ�����
	);

	// ������
	//		��ȡ��MenuButton��ָ��UniqueID��PopupMenu����������ģ�
	IEinkuiIterator* GetPopupMenuByUniqueID(
		IN UINT niUniqueID
	);

protected:
	CEvMenuButton(void);
	~CEvMenuButton(void);

	//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
	virtual ERESULT OnElementCreate(IEinkuiIterator* npIterator);
	//// ����Ԫ�أ�����֪ͨһ��Ԫ�����٣�ע�⣺��Ԫ�������յ�����Ϣ��Ӧ�ü�ʱ����Ԫ�ع�������UnregisterElement������
	//// �Ӷ�����Ԫ�ع�������������һ��Ԫ�ط���������Ϣ�������ٽ��Լ���Ԫ�ع�����ע���������ͷ��������
	//virtual ERESULT OnElementDestroy();

	//������Ϣ
	virtual ERESULT OnPaint(IEinkuiPaintBoard* npPaintBoard);

	virtual ERESULT ParseMessage(IEinkuiMessage* npMsg);

	//���û�����
	virtual ERESULT OnElementEnable(bool nbIsEnable);

	void OnMouseFocus(PSTEMS_STATE_CHANGE npState);

private:
	void LoadResource();

	void LoadSubPopupMenu();

	void OnBtnClick();



	// ������
	//		���úõ����˵���λ��
	void SetPopupMenuPosition();

	int miPopupMenuID;
	CEvPopupMenu* mpoPopupMenu;
	CEvButton*	mpoButton;
};

#define ID_OF_MENUBUTTON_BUTTON				1
#define ID_OF_MENUBUTTON_POPUPMENU			2

//#define ID_OF_MENUBUTTON_BASE				3


#define TF_ID_MENUBUTTON_POPUPMENU_ID		L"PopupMenuID"

#define TF_ID_MENUBUTTON_BUTTON				L"Button"

