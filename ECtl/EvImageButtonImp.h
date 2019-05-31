/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#ifndef		__EVIMAGEBUTTONIMP_H__
#define		__EVIMAGEBUTTONIMP_H__

#include "EvPictureFrameImp.h"
#include "cmmstruct.h"
//#include "CommandIDDef.h"



#define BUTTON_FRAME_ARRAY_MAX 20		//ÿ��״̬��֡��Ϣ���������Ա����

DECLARE_BUILTIN_NAME(ImageButton)

class CEvImageButton:
	public CXuiElement<CEvImageButton, GET_BUILTIN_NAME(ImageButton)>
{
	friend CXuiElement<CEvImageButton, GET_BUILTIN_NAME(ImageButton)>;

public:

	//enum	//���֡������Ķ�Ӧ�±�
	//{
	//	ARRAY_INDEX_DISABLE = 0,			//����̬
	//	ARRAY_INDEX_NORMAL,					//��̬ͨ
	//	ARRAY_INDEX_FOCUS,					//����̬
	//	ARRAY_INDEX_PRESSED,				//����̬
	//	ARRAY_INDEX_CHECKED_DISABLE,		//Checked̬�Ľ���̬
	//	ARRAY_INDEX_CHECKED_NORMAL,			//Checked̬����̬ͨ
	//	ARRAY_INDEX_CHECKED_FOCUS,			//Checked̬��ӵ�н���̬
	//	ARRAY_INDEX_CHECKED_PRESSED,		//Checked̬�İ���̬
	//	ARRAY_INDEX_OTHER					//��������
	//};

	////ÿ��״̬��֡��Ϣ
	//struct ImageButtonFrame
	//{
	//	LONG Index;		//ͼƬ�ϵĵڼ�֡
	//	LONG Count;		//һ���м�֡
	//};


	ULONG InitOnCreate(
		IN IEinkuiIterator* npParent = NULL,	// ������ָ��
		IN ICfKey* npTemplete = NULL,		// npTemplete��Key ID����EID��ֵ��������EType
		IN ULONG nuEID=MAXULONG32	// �����Ϊ0��MAXULONG32����ָ����Ԫ�ص�EID; ����ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��XUIϵͳ�Զ�����
		) ;

protected:
	// ֻ���ڱ������ó�ʼֵ����ָ����ΪNULL�����п���ʧ�ܵ������֮������㶼Ӧ����InitOnCreate�н���
	CEvImageButton();
	// �����ͷų�Ա����
	virtual ~CEvImageButton();
	//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
	virtual ERESULT OnElementCreate(IEinkuiIterator* npIterator);
	//��������뿪
	virtual void OnMouseFocus(PSTEMS_STATE_CHANGE npState);
	//��갴��
	virtual ERESULT OnMousePressed(const STEMS_MOUSE_BUTTON* npInfo);
	// ��������
	virtual ERESULT OnMouseOwnerTest(const D2D1_POINT_2F& rPoint);
	//����
	virtual ERESULT OnPaint(IEinkuiPaintBoard* npPaintBoard);
	// �ֽ���Ϣ���ṩ��Ϣ�ֽ����Ϣ��Ӧ�Ĺ��ܣ������ʵ���ǽ���Ϣ�ֽ�Ϊ��ͬ������󣬵�����Ӧ�Ĵ����麯�������ڲ���ʶ����Ϣ��һ�ɷ���ERESULT_UNEXPECTED_MESSAGE
	// �������ķ���ֵ���Զ�ͬ�����õ�npMsgָ�����Ϣ������
	virtual ERESULT ParseMessage(IEinkuiMessage* npMsg);

	BOOL SetChildCtrlPara();			//װ��������Դ�������ӿؼ�

	void SetChecked(bool nbChecked);

	//Ԫ�زο��ߴ緢���仯
	virtual ERESULT OnElementResized(D2D1_SIZE_F nNewSize);

	//���û�����
	virtual ERESULT OnElementEnable(bool nbIsEnable);

	bool SetItemSelected(int nID);

public:

	struct ExpandMenuItem
	{
		int			id;
		wchar_t		pPicPath[MAX_PATH];
	};


private:

	//IUnificSetting* mpUnificSetting;

private:

	//	��ȡUnificSetting���ݣ�������Ϊ
	bool SetValue();

	bool SetImageButtonEnable(bool nbEnable);

private:

	CEvPictureFrame*	mpLeftPicture;
	CEvPictureFrame*	mpRightPicture;
		
	IEinkuiBitmap*			mpBitmapSelectOrOver;		//ѡ�л��߻����ı���
	bool				mbIsMouseFocus;				//�Ƿ�ӵ����꽹��
	bool				mbIsKeyboardFocus;			//�Ƿ�ӵ�м��̽���
	bool				mbIsPressed;				//�Ƿ񱻰���
	

	int					mnStyle;					//ImageButton���  0:uncheckable   1:checkable 

	bool				mbHasColorFlag;				//��ɫ��ť���
	ULONG				mluRGBColor;					//��ɫֵ

	//��չ�ӿؼ�����
	bool				mbExpandable;				//֧����չ�ӿؼ�
	IEinkuiIterator*		mpIterExpandCtrl;			//��չ�ӿؼ�
	bool				mbShowExpandCtrl;			//��չ�ӿؼ���ʾ������
	cmmVector<ExpandMenuItem>	mpVecExpandMenuItem;		//��չ�˵���Ϣ�б�


	//�ؼ���Ϣ
	TOOLBAR_MSG			mMsgInfo;
	//��ϢЯ��������
	bool				mbChecked;						//�Ƿ�ѡ��
	int					mnSubItemID;					//����ID
	double				mdValue;						//doubleֵ
	wchar_t*			pszValue;						//stringֵ


	//����
	float				mfRadio;

	bool				mbDrawShape;


	//ImageButtonFrame	mdArrayFrame[BUTTON_FRAME_ARRAY_MAX];	//���ÿ��״̬��֡��Ϣ
	//LONG				mlCurrentPage;							//��ǰ���ŵ��ڼ�֡��
	//LONG				mlPageCountMax;							//���֡��
};

#endif		// __EVIMAGEBUTTONIMP_H__