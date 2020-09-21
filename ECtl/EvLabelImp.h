#ifndef _EVLABEL_H_
#define _EVLABEL_H_

__interface IDWriteTextLayout;



// Element�Ļ���ʵ�֣���������Elementʱ�����Դ�Ϊ������Ҫ���ԴӴ����ֽ��������࣬��Ϊ��������������ʵ�������Ľӿڣ�ֱ�����������CEvLabelImp���������ṩIXsElement�ӿ�
// ���ʵ�ֵ�����ͬ�ӿڵ���𣬾Ϳ���ֱ�Ӵ�ĳ��ʵ�������������ࡣ
DECLARE_BUILTIN_NAME(Label)
class CEvLabelImp :
	public CXuiElement<CEvLabelImp, GET_BUILTIN_NAME(Label)>
{
	friend CXuiElement<CEvLabelImp, GET_BUILTIN_NAME(Label)>;
public:

	// �������༰����������ʱ�����ر�ע��!!! һ��Ҫ���ȵ��û���ķ���
	// �����������ڽ�����Ԫ�ض��󡢳�ʼ���������ݣ�λ�á���С������ͼ�ȣ�
	ULONG InitOnCreate(
		IN IEinkuiIterator* npParent,	// ������ָ��
		IN ICfKey* npTemplete,		// npTemplete��Key ID����EID��ֵ��������EType
		IN ULONG nuEID = MAXULONG32		// �����Ϊ0��MAXULONG32����ָ����Ԫ�ص�EID; ����ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��XUIϵͳ�Զ�����
	);

protected:
	HCURSOR mdPreviousCursor;		//���������Լ�ǰ��״̬
	cmmVector<wchar_t> moText;	//�洢������ַ���
	bool mbDirty;
	bool mbLbPressed;
	LONG mlBlinking;
	LONG mlVisibleCount;
	UINT32 muLines;
	ULONG muForeColor;
	ULONG muBackColor;
	D2D1_RECT_F mdTextEdge;		//��������ͬ��ͼ�ı߽�
	D2D1_SIZE_F mdTextSize;		//��ǰ���ֵ�ʵ����ʾ���εĿ��
	D2D1_RECT_F mdBgRect;		//������λ�úʹ�С
	D2D1_POINT_2F mdTextPos;	//��ǰ��ʾ���ֵ�λ�ã���mdTextEdge�ı߽翪ʼ���㣬���ϽǶ����Ǿ���x=0��y=0

	//��ʾ��Դ
	IDWriteTextFormat* mpTextFormat;	//�豸�޹�
	IDWriteTextLayout* mpTextLayout;

	ID2D1SolidColorBrush* mpForeBrush;	// �豸���
	ID2D1SolidColorBrush* mpBackBrush;
	FLOAT mfTextMaxHeight;
	FLOAT mfTextMaxWidth;
	CEvLabelImp();
	virtual ~CEvLabelImp();

	//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
	ERESULT OnElementCreate(IEinkuiIterator* npIterator);

	// �ֽ���Ϣ���ṩ��Ϣ�ֽ����Ϣ��Ӧ�Ĺ��ܣ������ʵ���ǽ���Ϣ�ֽ�Ϊ��ͬ������󣬵�����Ӧ�Ĵ����麯�������ڲ���ʶ����Ϣ��һ�ɷ���ERESULT_UNEXPECTED_MESSAGE
	// �������ķ���ֵ���Զ�ͬ�����õ�npMsgָ�����Ϣ������
	virtual ERESULT ParseMessage(IEinkuiMessage* npMsg);

	// ����Ԫ�أ�����֪ͨһ��Ԫ�����٣�ע�⣺��Ԫ�������յ�����Ϣ��Ӧ�ü�ʱ����Ԫ�ع�������UnregisterElement������
	// �Ӷ�����Ԫ�ع�������������һ��Ԫ�ط���������Ϣ�������ٽ��Լ���Ԫ�ع�����ע���������ͷ��������
	virtual ERESULT OnElementDestroy();

	// ��������
	virtual ERESULT OnMouseOwnerTest(const D2D1_POINT_2F& rPoint);

	//������Ϣ
	virtual ERESULT OnKeyPressed(const STEMS_KEY_PRESSED* npInfo);

	//��������뿪
	virtual void OnMouseFocus(PSTEMS_STATE_CHANGE npState);

	//������Ϣ
	virtual ERESULT OnPaint(IEinkuiPaintBoard* npPaintBoard);

	//���̽����û���ʧȥ
	virtual void OnKeyBoardFocus(PSTEMS_STATE_CHANGE npState);

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

	//��갴��
	virtual ERESULT OnMousePressed(const STEMS_MOUSE_BUTTON* npInfo);

	// �������ɸ�ʽ������
	virtual ERESULT GenerateTextLayout(void);

	// �������ֵ��Ų�����
	virtual void CalculateTextRect(void);

	// ɾ���豸�����Դ
	void ReleaseDeviceResource();

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

	// ��������
	void EnableInput(LONG nlSet);

	ERESULT SetBrushColor(IEinkuiMessage* npMsg, bool nbFore);

	ERESULT GetBrushColor(IEinkuiMessage* npMsg, bool nbFore);

	ERESULT GetLayout(IEinkuiMessage* npMsg);

	ERESULT CreateFont(void);

};


#define ELAB_FLAG_UNDERLINE 0	// ���»���
#define ELAB_FLAG_HYPERLINK 1	// �����ӣ����ַ�ʽ�£���֧�����ͼ����¼����Ӷ����ϲ㷴��һ��Open�¼�
#define ELAB_FLAG_FORE_COLOR 2
#define ELAB_FLAG_BACK_COLOR 3

#define TF_ID_LABEL_HYPER_LINK L"HyperLink"				// �Ƿ�֧�ֳ�����
#define TF_ID_LABEL_UNDER_LINE	L"UnderLine"			// �Ƿ���Ҫ�»���
#define TF_ID_LABEL_EDGE_LEFT L"Edge/Left"				// ��߾�
#define TF_ID_LABEL_EDGE_RIGHT L"Edge/Right"			// �ұ߾�
#define TF_ID_LABEL_EDGE_TOP L"Edge/Top"				// �ϱ߾�
#define TF_ID_LABEL_EDGE_BOTTOM L"Edge/Bottom"			// �±߾�
#define TF_ID_LABEL_VALUE		L"Value"				// ��ʾ���ı�
#define TF_ID_LABEL_MAX_WIDTH		L"MaxWidth"			// �����
#define TF_ID_LABEL_MAX_HEIGHT		L"MaxHeight"		// ���߶�
#define TF_ID_LABEL_COLOR_FORE	L"Color/Fore"			// �ı�ǰ��ɫ
#define TF_ID_LABEL_COLOR_BACK	L"Color/Back"			// �ı�����ɫ
#define TF_ID_LABEL_FONT		L"Font"					// ����
#define TF_ID_LABEL_FONT_SIZE	L"Font/Size"		// �ֺ�
#define TF_ID_LABEL_ALIGN_VERTICAL L"Align/Vertical"	// ��ֱ���뷽ʽ
#define TF_ID_LABEL_ALIGN_HORIZONTAL L"Align/Horizontal"	// ˮƽ���뷽ʽ



#endif//_EVLABEL_H_
