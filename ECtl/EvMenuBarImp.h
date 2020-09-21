#pragma once

DECLARE_BUILTIN_NAME(MenuBar)

class CEvMenuBar :
	public CXuiElement<CEvMenuBar, GET_BUILTIN_NAME(MenuBar)>
{
	friend CXuiElement<CEvMenuBar, GET_BUILTIN_NAME(MenuBar)>;
public:
	ULONG InitOnCreate(
		IN IEinkuiIterator* npParent = NULL,	// ������ָ��
		IN ICfKey* npTemplete = NULL,		// npTemplete��Key ID����EID��ֵ��������EType
		IN ULONG nuEID = MAXULONG32	// �����Ϊ0��MAXULONG32����ָ����Ԫ�ص�EID; ����ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��XUIϵͳ�Զ�����
	);

protected:
	CEvMenuBar(void);
	~CEvMenuBar(void);

	//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
	virtual ERESULT OnElementCreate(IEinkuiIterator* npIterator);
	//// ����Ԫ�أ�����֪ͨһ��Ԫ�����٣�ע�⣺��Ԫ�������յ�����Ϣ��Ӧ�ü�ʱ����Ԫ�ع�������UnregisterElement������
	//// �Ӷ�����Ԫ�ع�������������һ��Ԫ�ط���������Ϣ�������ٽ��Լ���Ԫ�ع�����ע���������ͷ��������
	//virtual ERESULT OnElementDestroy();

	//������Ϣ
	virtual ERESULT OnPaint(IEinkuiPaintBoard* npPaintBoard);

	ERESULT ParseMessage(IEinkuiMessage* npMsg);

	//��ť�����¼�
	virtual ERESULT OnCtlButtonClick(IEinkuiIterator* npSender);			// ����MenuBarӦ��û�а�ť�ģ����������������

	//Ԫ�زο��ߴ緢���仯
	virtual ERESULT OnElementResized(D2D1_SIZE_F nNewSize);

	// ��ȡָ��CommandID��PopupMenu
	IEinkuiIterator* GetPopupMenuByCommandID(IN ULONG niCommandID);

	////��ݼ���Ϣ
	//ERESULT OnHotKey(const STEMS_HOTKEY* npHotKey);
private:
	void LoadResource();

	bool mbIsSubMenuVisible;

	IEinkuiIterator* mpoLastShowMenuButton;

	//ULONG mulHotKeyID;
};


#define TF_ID_MENUBAR_BTN_MIN			101		// ��С����ť
#define TF_ID_MENUBAR_BTN_CLOSE			102		// �رհ�ť

