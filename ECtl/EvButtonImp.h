#ifndef _EVBUTTONIMP_H_
#define _EVBUTTONIMP_H_



// Element�Ļ���ʵ�֣���������Elementʱ�����Դ�Ϊ������Ҫ���ԴӴ����ֽ��������࣬��Ϊ��������������ʵ�������Ľӿڣ�ֱ�����������CEvButton���������ṩIXsElement�ӿ�
// ���ʵ�ֵ�����ͬ�ӿڵ���𣬾Ϳ���ֱ�Ӵ�ĳ��ʵ�������������ࡣ

#define BUTTON_TIMER_ID_PAGE 1		//����֡�����õĶ�ʱ��ID


#define  MOUSE_LB(_X) ((_X&MK_LBUTTON)!=0)
#define  MOUSE_RB(_X) ((_X&MK_RBUTTON)!=0)
#define  MOUSE_MB(_X) ((_X&MK_MBUTTON)!=0)
#define  MOUSE_SHIFT(_X) ((_X&MK_SHIFT)!=0)
#define  MOUSE_CONTROL(_X) ((_X&MK_CONTROL)!=0)
#define  MOUSE_X1(_X) ((_X&MK_XBUTTON1)!=0)
#define  MOUSE_X2(_X) ((_X&MK_XBUTTON2)!=0)

#define BUTTON_FRAME_ARRAY_MAX 20  //ÿ��״̬��֡��Ϣ���������Ա����

DECLARE_BUILTIN_NAME(Button)
class CEvButton :
	public CXuiElement<CEvButton, GET_BUILTIN_NAME(Button)>
{
	friend CXuiElement<CEvButton, GET_BUILTIN_NAME(Button)>;
public:
	enum	//���֡������Ķ�Ӧ�±�
	{
		ARRAY_INDEX_DISABLE = 0,			//����̬
		ARRAY_INDEX_NORMAL,					//��̬ͨ
		ARRAY_INDEX_FOCUS,					//����̬
		ARRAY_INDEX_PRESSED,				//����̬
		ARRAY_INDEX_CHECKED_DISABLE,		//Checked̬�Ľ���̬
		ARRAY_INDEX_CHECKED_NORMAL,			//Checked̬����̬ͨ
		ARRAY_INDEX_CHECKED_FOCUS,			//Checked̬��ӵ�н���̬
		ARRAY_INDEX_CHECKED_PRESSED,		//Checked̬�İ���̬
		ARRAY_INDEX_OTHER					//��������
	}BUTTON_ARRAY_INDEX;

	//ÿ��״̬��֡��Ϣ
	typedef struct _XuiButtonFrame
	{
		LONG Index;		//ͼƬ�ϵĵڼ�֡
		LONG Count;		//һ���м�֡
	}XuiButtonFrame;

	// �������༰����������ʱ�����ر�ע��!!! һ��Ҫ���ȵ��û���ķ���
	// �����������ڽ�����Ԫ�ض��󡢳�ʼ���������ݣ�λ�á���С������ͼ�ȣ�
	ULONG InitOnCreate(
		IN IEinkuiIterator* npParent,	// ������ָ��
		IN ICfKey* npTemplete,		// npTemplete��Key ID����EID��ֵ��������EType
		IN ULONG nuEID = MAXULONG32		// �����Ϊ0��MAXULONG32����ָ����Ԫ�ص�EID; ����ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��XUIϵͳ�Զ�����
	);

protected:
	// ֻ���ڱ������ó�ʼֵ����ָ����ΪNULL�����п���ʧ�ܵ������֮������㶼Ӧ����InitOnCreate�н���
	CEvButton();
	// �����ͷų�Ա����
	virtual ~CEvButton();
	//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
	virtual ERESULT OnElementCreate(IEinkuiIterator* npIterator);
	//����
	virtual ERESULT OnPaint(IEinkuiPaintBoard* npPaintBoard);
	//��������뿪
	virtual void OnMouseFocus(PSTEMS_STATE_CHANGE npState);
	//��갴��
	virtual ERESULT OnMousePressed(const STEMS_MOUSE_BUTTON* npInfo);
	//����Check״̬
	virtual ERESULT SetChecked(bool nbIsChecked = false);
	//�ж��Ƿ���Check״̬
	virtual bool IsChecked();
	// �ֽ���Ϣ���ṩ��Ϣ�ֽ����Ϣ��Ӧ�Ĺ��ܣ������ʵ���ǽ���Ϣ�ֽ�Ϊ��ͬ������󣬵�����Ӧ�Ĵ����麯�������ڲ���ʶ����Ϣ��һ�ɷ���ERESULT_UNEXPECTED_MESSAGE
	// �������ķ���ֵ���Զ�ͬ�����õ�npMsgָ�����Ϣ������
	virtual ERESULT ParseMessage(IEinkuiMessage* npMsg);
	// ����Ԫ�أ�����֪ͨһ��Ԫ�����٣�ע�⣺��Ԫ�������յ�����Ϣ��Ӧ�ü�ʱ����Ԫ�ع�������UnregisterElement������
	// �Ӷ�����Ԫ�ع�������������һ��Ԫ�ط���������Ϣ�������ٽ��Լ���Ԫ�ع�����ע���������ͷ��������
	virtual ERESULT OnElementDestroy();

	//��ʱ��
	virtual void OnTimer(
		PSTEMS_TIMER npStatus
	);
	//ָ�����ŵڼ�֡����
	virtual ERESULT OnPlayAnimation(LONG nlIndex = 0);

	// ��������
	virtual ERESULT OnMouseOwnerTest(const D2D1_POINT_2F& rPoint);

	//�ı䰴ť����
	virtual ERESULT OnChangeText(wchar_t* npswText);

	//������ʾͼƬ
	virtual ERESULT OnChangeBackGround(wchar_t* npswPicPath = NULL, bool nbIsFullPath = false);
	//���û�����
	virtual ERESULT OnElementEnable(bool nbIsEnable);

	//��ȡ��ǰ״̬���ڵ������±�
	virtual LONG GetCurrentStatesArrayIndex();
	//װ��������Դ
	virtual ERESULT LoadResource();
	//��λ����ͼƬ��ʾλ��
	virtual void RelocateText(void);
protected:
	IEinkuiBitmap* mpTextBitmap;		 //��ʾ����ת���ɵ�ͼƬ
	XuiButtonFrame mdArrayFrame[BUTTON_FRAME_ARRAY_MAX];	 //���ÿ��״̬��֡��Ϣ
	LONG mlCurrentPage;				 //��ǰ���ŵ��ڼ�֡��
	D2D1_SIZE_F mdFrameSize;		 //ÿ֡����ʵ�ߴ�
	D2D1_SIZE_F mdAcionSize;		 //��Ӧ����С,0��ʾΪ����ͼ��С
	bool mbIsPressed;				 //�Ƿ񱻰���
	D2D1_RECT_F mdTextDestRect;		 //��������ʱ��Ŀ��Rect
	LONG mlAlignType;				 //���ֶ��뷽ʽ��1������� 2�����ж��� 3���Ҷ��룩
	FLOAT mfTextLeft;				 //������߾�
	FLOAT mfTextRight;				 //�����ұ߾�
	FLOAT mfTextTop;				 //������߾�
	FLOAT mfTextBottom;				 //�����ұ߾�
private:
	wchar_t* mpszButtonText;		 //��ť��ʾ����
	wchar_t* mpszNormolTip;
	wchar_t* mpszCheckedTip;
	bool mbIsMouseFocus;			 //�Ƿ�ӵ����꽹��
	bool mbIsKeyboardFocus;			 //�Ƿ�ӵ�м��̽���
	bool mbIsChecked;				 //�Ƿ���Check״̬
	bool mbIsCheckEnable;			 //�Ƿ�����Check״̬
	bool mbShrinkOnPressed;			 //����״̬���Ƿ��������ƶ�
	LONG mlFontSize;				 //�����С
	LONG mlPageCountMax;			 //���֡��
	bool mbIsOther;					 //�Ƿ��ǲ�����������
	LONG mlOtherIndex;				 //���ŵ�����һ����������
	DWORD mdwColor;				     //������ɫ
	DWORD mdwDisabledColor;			 // ����ʱ���ֵ���ɫ
	DWORD mdwFontSize;				 //�����С
	wchar_t* mpswFontName;			 //��������

	bool mbIsPlayTimer;				 //���Ŷ�����ʱ���Ƿ���



	//������������ͼƬ
	bool ReCreateTextBmp();
	//������رն�����ʱ��
	void StartPlayTimer();
};




#define TF_ID_BT_TEXT L"Text"							//��ť����
#define TF_ID_BT_FRAME L"FrameCount"					//��̬��ťÿ̬��֡��
#define TF_ID_BT_CHECKED_FRAME L"CheckFrameCount"		//Checked��ťÿ̬��֡��
#define TF_ID_BT_OTHER_FRAME L"Other"		//�����Զ��嶯����֡��
#define TF_ID_BT_NORMAL_TIP L"Tip1"			//��ͨ״̬��ʾ
#define TF_ID_BT_CHECKED_TIP L"Tip2"		//Checked״̬��ʾ
#define TF_ID_BT_COLOR L"TextColor"			//������ɫ
#define TF_ID_BT_DISABLED_COLOR L"TextDisabledColor"			//���ֽ���ʱ����ɫ
#define TF_ID_BT_FONT L"FontName"			//����
#define TF_ID_BT_FONT_SIZE L"FontSize"		//�ֺ�
#define TF_ID_BT_ALIGN_TYPE L"TextAlign"		//���ֶ��뷽ʽ
#define TF_ID_BT_TEXT_LEFT L"TextLeft"		//������߾�
#define TF_ID_BT_TEXT_RIGHT L"TextRight"		//�����ұ߾�
#define TF_ID_BT_TEXT_TOP L"TextTop"		//�����ϱ߾�
#define TF_ID_BT_TEXT_BOTTOM L"TextBottom"		//�����±߾�
#define TF_ID_BT_ACTION_WIDTH L"ActionWidth"		//��Ӧ����
#define TF_ID_BT_ACTION_HEIGHT L"ActionHeight"		//��Ӧ����
#define TF_ID_BT_TEXT_MAXWIDTH L"TextMaxWidth"		//���������
#define TF_ID_BT_TEXT_MAXHEIGHT L"TextMaxHeight"	//�������߶�

#endif//_EVBUTTONIMP_H_
