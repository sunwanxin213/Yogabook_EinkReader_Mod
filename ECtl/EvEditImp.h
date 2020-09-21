#ifndef _EVEDITIMP_H_
#define _EVEDITIMP_H_

__interface IDWriteTextLayout;

class CEvEditTextSection {
public:
	D2D1_RECT_F mdRegion;
	LONG mlBgBrushIndex;
	LONG mlStart;
	LONG mlLength;

	void operator=(const CEvEditTextSection& src) {
		mdRegion = src.mdRegion;
		mlBgBrushIndex = src.mlBgBrushIndex;
		mlStart = src.mlStart;
		mlLength = src.mlLength;
	}
};




// Element�Ļ���ʵ�֣���������Elementʱ�����Դ�Ϊ������Ҫ���ԴӴ����ֽ��������࣬��Ϊ��������������ʵ�������Ľӿڣ�ֱ�����������CEvEditImp���������ṩIXsElement�ӿ�
// ���ʵ�ֵ�����ͬ�ӿڵ���𣬾Ϳ���ֱ�Ӵ�ĳ��ʵ�������������ࡣ
DECLARE_BUILTIN_NAME(Edit)
class CEvEditImp :
	public CXuiElement<CEvEditImp, GET_BUILTIN_NAME(Edit)>
{
	friend CXuiElement<CEvEditImp, GET_BUILTIN_NAME(Edit)>;
public:
	enum EBRUSHINDEX {
		eForeBrush = 0,
		eBackBrush = 1,
		eSelForeBrush = 2,
		eSelBackBrush = 3
	};

	// �������༰����������ʱ�����ر�ע��!!! һ��Ҫ���ȵ��û���ķ���
	// �����������ڽ�����Ԫ�ض��󡢳�ʼ���������ݣ�λ�á���С������ͼ�ȣ�
	ULONG InitOnCreate(
		IN IEinkuiIterator* npParent,	// ������ָ��
		IN ICfKey* npTemplete,		// npTemplete��Key ID����EID��ֵ��������EType
		IN ULONG nuEID = MAXULONG32		// �����Ϊ0��MAXULONG32����ָ����Ԫ�ص�EID; ����ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��XUIϵͳ�Զ�����
	);

protected:
	cmmVector<wchar_t> moText;	//�洢������ַ���
	LONG mlLimit;
	LONG mlCursorAt;		//�������λ��
	LONG mlViewBegin;			//��ͼ�ڵĵ�һ���ַ�
	LONG mlViewEnd;				//��ͼ�ڵ����һ���ַ�
	LONG mlSelBegin;			//ѡ���ַ���ʼ
	LONG mlSelEnd;				//ѡ�е����һ���ַ�
	bool mbDirty;
	bool mbInsertMode;
	bool mbCompletion;
	LONG mlBlinking;
	D2D1_RECT_F mdLayoutRect;		//���ֵ���ʾ����
	D2D1_RECT_F mdValidRect;
	D2D1_POINT_2F mdCursorPos;

	//��ʾ��Դ
	IDWriteTextFormat* mpTextFormat;	//�豸�޹�
	ID2D1SolidColorBrush* mpBrush[4];	// �豸���

	//��ʽ������
	cmmVector<CEvEditTextSection, 4> moTextSections;
	IDWriteTextLayout* mpTextLayout;	//�豸�޹�

	// ���ѡ��
	D2D1_POINT_2F mdDragFrom;
	LONG mlDragedText;


	CEvEditImp();
	virtual ~CEvEditImp();

	//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
	ERESULT OnElementCreate(IEinkuiIterator* npIterator);

	//virtual ERESULT OnPaint(IEinkuiPaintBoard* npPaintBoard);
	// �ֽ���Ϣ���ṩ��Ϣ�ֽ����Ϣ��Ӧ�Ĺ��ܣ������ʵ���ǽ���Ϣ�ֽ�Ϊ��ͬ������󣬵�����Ӧ�Ĵ����麯�������ڲ���ʶ����Ϣ��һ�ɷ���ERESULT_UNEXPECTED_MESSAGE
	// �������ķ���ֵ���Զ�ͬ�����õ�npMsgָ�����Ϣ������
	virtual ERESULT ParseMessage(IEinkuiMessage* npMsg);
	// ����Ԫ�أ�����֪ͨһ��Ԫ�����٣�ע�⣺��Ԫ�������յ�����Ϣ��Ӧ�ü�ʱ����Ԫ�ع�������UnregisterElement������
	// �Ӷ�����Ԫ�ع�������������һ��Ԫ�ط���������Ϣ�������ٽ��Լ���Ԫ�ع�����ע���������ͷ��������
	virtual ERESULT OnElementDestroy();

	//���û�����
	virtual ERESULT OnElementEnable(bool nbIsEnable);

	// ��������
	virtual ERESULT OnMouseOwnerTest(const D2D1_POINT_2F& rPoint);

	//������Ϣ
	virtual ERESULT OnKeyPressed(const STEMS_KEY_PRESSED* npInfo);

	//��������뿪
	virtual void OnMouseFocus(PSTEMS_STATE_CHANGE npState);

	//���˫��
	virtual ERESULT OnMouseDbClick(const STEMS_MOUSE_BUTTON* npInfo);

	//�ַ�������Ϣ
	virtual ERESULT OnChar(const PSTEMS_CHAR_INPUT npChar);

	//������Ϣ
	virtual ERESULT OnPaint(IEinkuiPaintBoard* npPaintBoard);

	//���̽����û���ʧȥ
	virtual void OnKeyBoardFocus(PSTEMS_STATE_CHANGE npState);

	//����
	virtual ERESULT OnCommand(const nes_command::ESCOMMAND neCmd);

	//Set Text
	virtual ERESULT OnSetText(const wchar_t* nswText);

	//Get Text
	virtual ERESULT OnGetText(wchar_t* nswTextBuf, LONG nlCharCount);

	// ��ˢ��
	void  OnLazyUpdate(
		PSTEMG_LAZY_UPDATE npLazyUpdate
	);

	//׼������
	ERESULT PrepareBrush(IEinkuiPaintBoard* npPaintBoard);

	// Left Arrow striked
	virtual void OnKeyLeft(const STEMS_KEY_PRESSED* npInfo);

	// Right Arrow striked
	virtual void OnKeyRight(const STEMS_KEY_PRESSED* npInfo);

	// Back Space striked
	virtual void OnKeyBack(const STEMS_KEY_PRESSED* npInfo);

	// Delete striked
	virtual void OnDeleteCommand(void);

	// Home striked
	virtual void OnKeyHome(const STEMS_KEY_PRESSED* npInfo);

	// End striked
	virtual void OnKeyEnd(const STEMS_KEY_PRESSED* npInfo);

	// Insert striked
	virtual void OnKeyInsert(const STEMS_KEY_PRESSED* npInfo);

	//Ԫ����ק
	virtual ERESULT OnDragging(const STMS_DRAGGING_ELE* npInfo);

	//��ק��ʼ
	virtual ERESULT OnDragBegin(const STMS_DRAGGING_ELE* npInfo);

	//��ק����
	virtual ERESULT OnDragEnd(const STMS_DRAGGING_ELE* npInfo);

	//��갴��
	virtual ERESULT OnMousePressed(const STEMS_MOUSE_BUTTON* npInfo);

	// copy
	virtual void OnCopyCommand(void);

	// Paste
	virtual void OnPasteCommand(void);

	// Cut
	virtual void OnCutCommand(void);

	// Undo
	virtual void OnUndoCommand(void);

	//���ָ��λ���ַ�,nlFromΪ���ĸ�������ʼ;nlCountΪɾ������
	virtual ERESULT RemoveChars(LONG nlFrom, LONG nlCount = 1);

	//�����µ��ַ�,nlInsertTo��ʾ���ĸ�λ�ý��в���,-1��ʾ�嵽���;nswChars��ʾҪ������ַ���; nlLength == -1 ��ʾnszChars��ȫ����Ч�ַ�������ȷ��nszChars����\0��β
	virtual LONG InsertChars(LONG nlInsertTo, wchar_t* nswChars, LONG nlLength = -1);

	// ����һ���ַ�
	virtual LONG InsertChar(LONG nlInsertTo, wchar_t nwcChar);

	// �������ɸ�ʽ������
	virtual ERESULT GenerateTextLayout(IEinkuiPaintBoard* npPaintBoard);

	// �������ֵ��Ų�����
	virtual void CalculateTextRect(IEinkuiPaintBoard* npPaintBoard);

	// �ͷŸ�ʽ������
	void ClearTextLayout();

	// ɾ���豸�����Դ
	void ReleaseDeviceResource();

	//�Ӽ��а���ַ�����return the character count
	int GetClipboardString(OUT wchar_t* npTextBuffer, LONG nlBufCharSize);

	//save string to clipboard
	void SetClipboardString(const wchar_t* nswString, LONG nlCharCount);

	// �������������
	void ClearClipboard(void);

	__inline void UpdateView(void);

	// ����λ��־������ 0 - 24
	bool SetFlags(
		int niIndex,		// ��־����ţ���0��ʼ���������������������������Ҹ���������2����ϣ������������û��޸ĵı�־����ô���ĺ�������ʱ��niIndex=0��ʾ�������Ļ����2
		bool nbSet		// ���û��������־
	) {
		return CXuiElement::SetFlags(niIndex + 4, nbSet);
	}

	// ��ȡ��־
	bool TestFlag(int niIndex) {
		return CXuiElement::TestFlag(niIndex + 4);
	}

	void EndModifying(void);

};


#define EEDT_FLAG_NUMBER 0
#define EEDT_FLAG_PASSWORD 1
#define EEDT_FLAG_READONLY 2





#endif//_EVEDITIMP_H_
