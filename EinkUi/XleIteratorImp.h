/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#ifndef _ELEITERATORIMP_H_
#define _ELEITERATORIMP_H_
#include <d2d1.h>
/*
	���Ķ���Ԫ�ص�������ʵ����

*/

class CXuiIterator;

class CEoSubItrNode{
public:
	ULONG muID;
	CXuiIterator* mpIterator;
	void operator=(const class CEoSubItrNode& src){
		muID = src.muID;
		mpIterator = src.mpIterator;
	}
};

class CEoSubItrNodeCriterion	// Ĭ�ϵ��ж�׼��
{
public:
	bool operator () (const CEoSubItrNode& Obj1,const CEoSubItrNode& Obj2)const // һ��Ҫ����������
	{
		// ������Ob1С�ڶ���Obj2ʱ������True�����򷵻�false
		return (Obj1.muID < Obj2.muID);
	}
};

// ����ID����ĵ���������
typedef cmmSequence<CEoSubItrNode,CEoSubItrNodeCriterion> TSubItrSequence;


// ���ڰ���TabOrder��ZOrder������Ԫ�ص����������ݽṹ
typedef cmmVector<CXuiIterator*>  TElIteratorVector;


class CXuiHotkeyEntry{
public:
	enum ExKey{
		eControl=1,
		eShit=2,
		eAlt=4
	};
	ULONG muHotKeyID;		//ע��ʱ�ṩ��ID
	USHORT msuVkCode;	// ��ݼ�����ͨ����
	UCHAR mcuExKey;
	IEinkuiIterator* mpApplicant;

	void operator=(const class CXuiHotkeyEntry& src){
		muHotKeyID = src.muHotKeyID;
		msuVkCode = src.msuVkCode;
		mcuExKey = src.mcuExKey;
		mpApplicant = src.mpApplicant;
	}
};

class CXuiHotkeyEntryCriterion	// Ĭ�ϵ��ж�׼��
{
public:
	bool operator () (const CXuiHotkeyEntry& Obj1,const CXuiHotkeyEntry& Obj2)const // һ��Ҫ����������
	{
		// ������Ob1С�ڶ���Obj2ʱ������True�����򷵻�false
		return (Obj1.msuVkCode < Obj2.msuVkCode || (Obj1.msuVkCode == Obj2.msuVkCode && Obj1.mcuExKey < Obj2.mcuExKey));
	}
};


typedef cmmSequence<CXuiHotkeyEntry,CXuiHotkeyEntryCriterion,0,8> THotKeyTable;



// ��������չ��
class CXuiIteratorExtension{
public:
	TSubItrSequence moIDOrder;	// ����ID��������Ա�
	TElIteratorVector moZOrder;	// ����Ԫ�صĲ����ϵ���е�����
	TElIteratorVector moTabOder;	// ����Tab�����Ľ���ѡ����������������
	// Direct2D��Layer
	// Direct32����ز���

};

// ����ı�־�������ж�Iterator�����ݵı�־λ
#define EITR_FLAG_INIT		1
#define EITR_FLAG_DIRTY		2	//��Ԫ�ص�λ�ú�������ʾ���Է������ı䣬������δ����ˢ��
#define EITR_FLAG_VISIBLE	3	//�Ƿ����
#define EITR_FLAG_DISABLE	4	//�Ƿ�ʹ��
#define EITR_FLAG_ALPHA		5	//�Ƿ�������͸����
#define EITR_FLAG_ROTATION	6	//�Ƿ�������ƽ����ת	'rtal' 'rtcn'
#define EITR_FLAG_WIDGET_HOME	7	//�ǲ���һ��Widget��Home Page
#define EITR_FLAG_ENHANCER	8	//�Ƿ�����Ч��	'ehcr'
#define EITR_FLAG_HOOK		9	//�Ƿ�Hook		'hook'
#define EITR_FLAG_VREGION	10	//�Ƿ������˿����� 'vrgn'
#define EITR_FLAG_TIP		11	//�Ƿ�������Tip	'tip'
#define EITR_FLAG_HOTKEY	12  //�Ƿ�ע���˿�ݼ� 'htky'
#define EITR_FLAG_DELETED	13	//�Ѿ���ɾ��
#define EITR_FLAG_PAINTLEVEL_HOST 14 //�ڽ�����Ʋ㣬����ͨ��'plvc'���
#define EITR_FLAG_CRT_PAINTLEVEL  15 //�趨�˻��Ʋ�Σ���ǰ��ͨ��'plvn'���

#define ITR_CHECK() //CMMASSERT(gpElementManager->VerifyIterator(this)==ERESULT_SUCCESS)



DECLARE_BUILTIN_NAME(CXuiIterator)
class CXuiIterator: public cmmBaseObject<CXuiIterator,IEinkuiIterator,GET_BUILTIN_NAME(CXuiIterator)>
{
	friend class CXelManager;
	friend class CEinkuiSystem;
public:
	DEFINE_CUMSTOMIZE_CREATE(CXuiIterator,(),())

	// ����Iterator��׼��������Ϣ���������������Element���Ȼ��յ�EMSG_CREATE��Ϣ���������ͨ����Element��ʵ���������˳�ǰ����
	virtual void __stdcall Start(void);

	// �ر�һ��Ԫ��
	virtual void __stdcall Close(void);

	// �����������ýӿڣ�������
	virtual int __stdcall AddRefer(void){
		return 1;
	}
	// �����ͷŽӿڣ�������
	virtual int __stdcall Release(void){
		return 1;
	}

	// ��ñ�Element��EID
	virtual ULONG __stdcall  GetID(void);

	// ��ñ�Element������Etype
	virtual const wchar_t* __stdcall  GetType(void);

	// ��ø�����
	virtual IEinkuiIterator* __stdcall  GetParent(void);

	// ��ñ���������Ӧ��Element����
	virtual IXsElement* __stdcall GetElementObject(void);

	// �����һ����Ӷ��������
	virtual int __stdcall GetSubElementCount(void);

	// ѯ��ĳ��Iterator�ڲ�νṹ���ǲ��ǵ�ǰIterator������
	virtual bool __stdcall FindAncestor(const IEinkuiIterator* npIsAncestor);

	// ͨ��ZOder�����д������ӽڵ㣬���صĽӿ���Ҫ�ͷ�
	virtual IEinkuiIterator* __stdcall  GetSubElementByZOder(
		int niPos	// zero base index value to indicate the position in z-order array
		);

	// ͨ��ID����ӽڵ㣬���صĽӿ���Ҫ�ͷ�
	virtual IEinkuiIterator* __stdcall  GetSubElementByID(ULONG nuEid);

	// ����Ԫ�ط���һ����Ϣ������ģʽ��Send
	// ��Ϣ���ͺ󣬷�������Ȼ��Ҫ�ͷţ����ϣ���Ը��Ӽ򵥵ķ�ʽ������Ϣ���ο�IXelManager��SimplePostMessage����
	virtual ERESULT __stdcall SendMessage(IEinkuiMessage* npMsg);

	// ����Ԫ�ط���һ����Ϣ������ģʽ��Post
	// ��Ϣ���ͺ󣬷�������Ȼ��Ҫ�ͷţ����ϣ���Ը��Ӽ򵥵ķ�ʽ������Ϣ���ο�IXelManager��SimplePostMessage����
	virtual ERESULT __stdcall PostMessage(IEinkuiMessage* npMsg);

	// ����Ԫ�صĸ�Ԫ�ط���һ����Ϣ�����͵�ģʽ��FastPost
	// ��Ϣ���ͺ󣬷�������Ȼ��Ҫ�ͷ�
	// !!!ע��!!! �����ڷ������Ǳ���������Ӧ��Ԫ��֮���
	virtual ERESULT __stdcall PostMessageToParent(IEinkuiMessage* npMsg);

	// ������̽��㣬�����Ԫ�ؾ���popup���ԣ�Ҳ�������������ʵ��ϲ�
	virtual ERESULT __stdcall SetKeyBoardFocus(void);

	// �ͷż��̽��㣬�⽫����Tab Order����һ�����̽����߻�ý���
	virtual void __stdcall ReleaseKeyBoardFocus(bool nbShiftFocus);

	// ����Ԫ��Ϊ��Ծ�������Ԫ�ز�����EITR_STYLE_POPUP��EITR_STYLE_ACTIVE�����ô��͵�һ������EITR_STYLE_POPUP��EITR_STYLE_ACTIVE�����ϲ�Ԫ�ؽ�������
	virtual void __stdcall SetActive(void);

	// ����Style
	virtual void __stdcall SetStyles(ULONG nuStyles);

	// �޸�Style��ǰһ��������ϣ�����ӵ�Style����һ��������ϣ���Ƴ���Style����ǰ�����������а�����ͬStyleʱ����Style���ᱻ�Ƴ� 
	virtual void __stdcall ModifyStyles(ULONG nuSet,ULONG nuClear=0);

	// ��ȡStyle
	virtual ULONG __stdcall GetStyles(void);

	// ���붨ʱ�����������ô����Ķ�ʱ������Ҫע��
	virtual ERESULT __stdcall SetTimer(
		IN ULONG nuID,	  // ��ʱ��ID
		IN ULONG nuRepeat,// ��Ҫ�ظ������Ĵ�����MAXULONG32��ʾ��Զ�ظ�
		IN ULONG nuDuration,	// ��������
		IN void* npContext//�����ģ������Ŷ�ʱ����Ϣ���͸�������
		);

	// ���ٶ�ʱ��
	virtual ERESULT __stdcall KillTimer(
		IN ULONG nuID	  // ��ʱ��ID
		);

	// HookĿ�꣬��ǰ��֧�ֵ���ε�Hook������һ��Ԫ����ͬһʱ�̽���һ��Ԫ��Hook����ͼHookһ���Ѿ���Hook��Ԫ��ʱ�����᷵��ʧ��ERESULT_ACCESS_CONFLICT
	virtual ERESULT __stdcall SetHook(
		IN IEinkuiIterator* npHooker,	// Hook�����ߣ�һ��������Hook���������������Ϣ�������ȷ��͸�Hooker����Hooker�����޸��������Ϣ��Ҳ������ֹ��Ϣ���͸�������
		IN bool nbSet		// true to set ,false to remove
		);

	// ���Hooker����ȡ��Ԫ�ر�˭Hook
	virtual IEinkuiIterator* __stdcall GetHooker(void);

	// ������Ⱦ��Ч������Ч�����ڸ�ĳ��Ԫ�غ�������Ԫ���ṩ�ض�����Ⱦ����Ч������ѡ��Direct2D��Direct3D��������XUIϵͳ����Ⱦ
	// ͬһ��Ԫ����ͬһʱ��ֻ����һ����Ч���ڹ��������ң�ͨ����Ч�����Ƕ��丸Ԫ�ط�������
	// ����ERESULT_ACCESS_CONFLICT��ʾ�����Ч��������ͻ����Ч�����ã����ڽ��յ�EMSG_PREPARE_PAINTʱ���������ط������ã��п��ܵ������ش���
	virtual ERESULT __stdcall SetEnhancer(
		IN IEinkuiIterator* npEnhancer,
		IN bool nbEnable		// true ���ã�false ȡ��
		);

	// �����Ч��
	virtual IEinkuiIterator* __stdcall GetEnhancer(void);

	//////////////////////////////////////////////////////////////////////////
	// ��������������ʾ��λ����صķ���

	// ������ʾ/����
	virtual void __stdcall SetVisible(bool nbVisible);

	// ��ȡ��ʾ/����״̬
	virtual bool __stdcall IsVisible(void);

	// �趨����͸����
	virtual void __stdcall SetAlpha(FLOAT nfAlpha);

	// ��ȡ����͸����
	virtual FLOAT __stdcall GetAlpha(void);

	// ����ƽ������
	virtual void __stdcall SetPosition(FLOAT nfX,FLOAT nfY);
	virtual void __stdcall SetPosition(const D2D1_POINT_2F& rPosition);

	// ��ȡƽ������
	virtual FLOAT __stdcall GetPositionX(void);
	virtual FLOAT __stdcall GetPositionY(void);
	virtual D2D1_POINT_2F __stdcall GetPosition(void);
	virtual void __stdcall GetRect(D2D1_RECT_F& rRect);

	// ���ÿ�������
	virtual void __stdcall SetVisibleRegion(
		IN const D2D1_RECT_F& rRegion		// �����������Ŀ������򣬴�����֮�ⲻ����ʾ��Ԫ�ؼ���Ԫ�ص����ݣ����rRegion.left > region.right ��ʾȡ������������
		);

	// ��ȡ�������򣬷���false��ʾû�����ÿ�������
	virtual bool __stdcall GetVisibleRegion(
		OUT D2D1_RECT_F& rRegion	// ���ؿ����������û�����ÿ��������򲻻��޸��������
		);

	// ����ƽ��ת��
	virtual void __stdcall SetRotation(
		FLOAT nfAngle,			// �Ƕȵ�λ -359 -> +359��
		D2D1_POINT_2F ndCenter
		);
	// ����ƽ��ת�ǣ���Ԫ������Ϊ��ת����
	virtual void __stdcall SetRotation(
		FLOAT nfAngle			// �Ƕȵ�λ -359 -> +359��
		);

	// ��ȡƽ��ת��
	virtual FLOAT __stdcall GetRotationAngle(void);
	virtual D2D1_POINT_2F __stdcall GetRotationCenter(void);
	virtual FLOAT __stdcall GetRotation(D2D1_POINT_2F& rCenter);

	// ���òο��ߴ�
	virtual void __stdcall SetSize(FLOAT nfCx,FLOAT nfCy);
	virtual void __stdcall SetSize(const D2D1_SIZE_F& rSize);

	// ��ȡ�ο��ߴ�
	virtual FLOAT __stdcall GetSizeX(void);
	virtual FLOAT __stdcall GetSizeY(void);
	virtual D2D1_SIZE_F __stdcall GetSize(void);

	// �����Ƿ���Ч
	virtual void __stdcall SetEnable(bool nbSet);

	// ��ȡ�Ƿ���Ч
	virtual bool __stdcall IsEnable(void);

	// ����Ԫ�ص�����ͬ��������߲�
	virtual void __stdcall BringToTop(void);

	// ����Ԫ���ڸ�Ԫ�ص�Z Order�����У������ƶ�һλ
	virtual bool __stdcall MoveDown(void);

	// ����Ԫ���ڸ�Ԫ�ص�Z Order�����У������ƶ�һλ
	virtual bool __stdcall MoveUp(void);

	// ����������Ԫ�صĵ��Ŵ���Ϊԭʼ���ã��������ļ��ƶ��Ĵ���
	virtual bool __stdcall ResetChildrenByDefualtZOrder(void);

	// �������ñ�Ԫ�ص�Z Order��ֵ����������ֵ�Element������ͬ��Z Order���޷�ȷ�����ǵ��Ⱥ�ѭ�򣬵�ϵͳ�������в������
	virtual void __stdcall SetDefaultZOrder(const LONG nlDefaultZOrder);

	// ���Ĭ�ϵ�ZOrderֵ
	virtual LONG __stdcall GetDefaultZOrder(void);

	//����ToolTip������ڱ���������ͣ�������Զ���ʾ�ĵ��м��������ʾ�����һ���ƿ���ʾ��ToolTip�����Զ���ʧ
	virtual void __stdcall SetToolTip(const wchar_t* nswTip);

	//����IME���봰��λ�ã�ndPosition�Ǳ�Ԫ�ؾֲ������е�λ��; ֻ�е�ԭԪ�ؾ���EITR_STYLE_IME����ʱ������Ч
	virtual void __stdcall SetIMECompositionWindow(D2D1_POINT_2F ndPosition);

	// �Ӿֲ���ַ�������ַ
	virtual bool __stdcall LocalToWorld(const D2D1_POINT_2F& crLocalPoint,D2D1_POINT_2F& rWorldPoint);

	// �������ַת��Ϊ�ֲ���ַ
	virtual bool __stdcall WorldToLocal(const D2D1_POINT_2F& crWorldPoint,D2D1_POINT_2F& rLocalPoint);

	// ����Ԫ�ؽ������Ʋ㣬���Ʋ���һ���ı���Ԫ�ػ���ʱ���Ŵ���ķ�����ͨ������Ԫ�ذ��մ�����ϵ����Ϊ���νṹ������ʱҲ�ǰ������νṹ�ȸ�����ִ�У�
	//		������Ʋ㼼������Ԫ�ؽ��ڲ�ͬ������λ��ƣ�ͬһ�������Ԫ�أ���Ȼ���մ�����ϵ�����νṹ�ȸ�������
	//		�����趨����Ļ��Ʋ㣬�����ܴ���max-ulong����������ж�Ҳ�յ����Ʋ��Ӱ�죬��θߵ�Ԫ�����ȱ��ж�������
	//		�����һ���Ѿ��趨�˻��Ʋ�ε������е�ĳ��Ԫ���ٴν������Ʋ�Σ���ô���������е�������Ԫ�ؽ����ܵ���һ�����Ʋ�ε�Ӱ�죬�����µĲ�ι�����
	//		Ϊ�˱�������뾡���ܱ���ʹ��Ƕ�׵Ļ��Ʋ��;
	virtual ERESULT __stdcall CreatePaintLevel(LONG nlLevelCount);


	// �����Ԫ�ػ��Ʋ�����
	virtual LONG __stdcall GetPaintLevelCount(void);

	// ɾ�����Ʋ���趨
	virtual ERESULT __stdcall DeletePaintLevel(void
		//bool nbClearAll=true		// �����������Ԫ�صĻ��Ʋ���趨
		);

	// �趨����Ļ��Ʋ��
	virtual ERESULT __stdcall SetPaintLevel(LONG nlLevel);

	// �������Ļ��Ʋ��
	virtual LONG __stdcall GetPaintLevel(void);


	void SaveWorldMatrix(const D2D1::Matrix3x2F& rWorld){

		if(	mdWorldMatrix._11 == rWorld._11 &&
			mdWorldMatrix._12 == rWorld._12 &&
			mdWorldMatrix._21 == rWorld._21 &&
			mdWorldMatrix._22 == rWorld._22 &&
			mdWorldMatrix._31 == rWorld._31 &&
			mdWorldMatrix._32 == rWorld._32 )
			return;

		mdWorldMatrix = rWorld;
		mbInverted = false;
	}

	// ��ȡ���һ�λ���ʱ�õ�World Matrix
	const D2D1::Matrix3x2F& GetWorldMatrix(void){
		return mdWorldMatrix;
	}

	// ��LaunchWidget�������ã������趨���ҳ�ǲ���Widget����ҳ������Ŀ�Ĳ�Ҫʹ��
	void SetWidgetHomeFlag(bool nbSet){
		SetFlags(EITR_FLAG_WIDGET_HOME,nbSet);
	}
	bool IsWidgetHome(void){
		return TestFlag(EITR_FLAG_WIDGET_HOME);
	}

	// ����ʼ����־
	bool HasInitialized(void){
		return TestFlag(EITR_FLAG_INIT);
	}

	// ���Style��ִ�С��롯�������ȱȽϣ�����Ƿ������˶�Ӧ��Style���
	bool CheckStyle(ULONG nuStyle){
		return ((muStyle&nuStyle)==nuStyle);
	}

	// ����Ϊ�ѱ�ɾ��
	void SetAsDeleted(void){
		SetFlags(EITR_FLAG_DELETED,true);
	}

	// �Ƿ��Ѿ���ɾ��
	bool IsDeleted(void){
		return TestFlag(EITR_FLAG_DELETED);
	}

	// �����һ���Ʋ㣬����ֵС�ڵ���nuCrtLevel����ʾû����һ���Ʋ�
	LONG GetNextPaintLevel(LONG nlCrtLevel);

private:
	static CXelManager* gpElementManager;

	// �ڲ���Ա
	cmmAttributes<16> moAtts;
	static CExclusiveAccess goAttributesLock;	// ���ж������Attributeʹ��ͬһ��������
	CXuiIteratorExtension* mpExtension;
	ULONG muEID;
	LONG mlTabOrder;
	LONG mlZOrder;
	ULONG muStyle;
	D2D1_POINT_2F mdPosition;
//	D2D1_POINT_2F mdCenter;
	D2D1_SIZE_F mdSize;
//	FLOAT mfRotation;
	FLOAT mfAlpha;
	D2D1::Matrix3x2F mdWorldMatrix;	// ���һ�λ���ʱ�õ�World Matrix
	D2D1::Matrix3x2F mdWMInverted;	// ����ľ���mdWorldMatrix������
	bool mbInverted;	// �Ƿ��Ѿ��л���������
	IXsElement* mpElement;
	CXuiIterator* mpParent;
	IXsWidget* mpWidget;
	//IEinkuiIterator* mpEnhancer;	// �ҽӵ���Ч����ͬһ��Ԫ����ͬһʱ��ֻ����һ����Ч���ڹ��������ң�ͨ����Ч�����Ƕ��丸Ԫ�ط�������

	
	CXuiIterator();
	~CXuiIterator();

	// ʵ���������ýӿ�
	virtual int __stdcall KAddRefer(void){
		return cmmBaseObject::AddRefer();
	}
	// ʵ���ͷŽӿ�
	virtual int __stdcall KRelease(void){
		return cmmBaseObject::Release();
	}
	

	// ���ñ�־������31λ���ã���Ϊ˽�У������಻�ܵ���
	bool SetFlags(
		int niIndex,		// ��־����ţ���0��ʼ���������������������������Ҹ���������2����ϣ������������û��޸ĵı�־����ô���ĺ�������ʱ��niIndex=0��ʾ�������Ļ����2
		bool nbSet=true		// ���û��������־
		){
			return false;
	}

	// ��ȡ��־����Ϊ˽�У������಻�ܵ���
	bool TestFlag(int niIndex){
		return false;
	}

	// ���һ���ӽڵ�
	ERESULT AddSubElement(
		CXuiIterator* npSubElement
		);

	// ɾ��һ���ӽڵ�
	void RemoveSubElement(
		CXuiIterator* npSubElement
		);

	// ��ĳ����Ԫ�ص�����ZOder��߲�
	void BringSubElementToTop(
		CXuiIterator* npSubElement
		);

	// ��Ԫ�ز��뵽ZOrder��
	void InsertToZOder(
		CXuiIterator* npSubElement
		);

	// ���ӽڵ��в���Я��Ŀ��ĵ�����
	CXuiIterator* SeekIteratorInChild(IXsElement* npElement);

	// ��Ԫ�ع������ã��������ýӿ�
	int ProtectedAddRefer(void){
		return cmmBaseObject<CXuiIterator,IEinkuiIterator,GET_BUILTIN_NAME(CXuiIterator)>::AddRefer();
	}
	// ��Ԫ�ع������ã��ͷŽӿ�
	int ProtectedRelease(void){
		return cmmBaseObject<CXuiIterator,IEinkuiIterator,GET_BUILTIN_NAME(CXuiIterator)>::Release();
	}

	// ����TabOrder��ZOrder����ʼ��֮����Element����������
	void UpdateOrder(void);

	// ������Ԫ����ZOrder��˳��
	bool MoveOnZOrder(bool nbUp,CXuiIterator* npChild);

	// �����һ��EITR_STYLE_KEYBOARD��Ԫ�أ��������Iterator�ٽ���
	CXuiIterator* GetNextKeyBoardAccepter(CXuiIterator* npCurrent);

	void ClearTip();

	// ���Tip
	const wchar_t* GetToolTip(void);

	// ע���ݼ�������ݼ���������ע���ݼ���Element�����ܵ���
	// �����ͨ������ϣ�������Alt��)���µĵ�ʱ�����ڼ��̽��㣬������Ϣ�����ȷ��͸����̽��㣬������㷵��ERESULT_KEY_UNEXPECTED�Ż��ж��Ƿ���ڿ�ݼ���Ϊ
	bool RegisterHotKey(
		IN IEinkuiIterator* npApplicant,	// ע���Ԫ�أ��������յ�ע���ǿ�ݼ���Ϣ
		IN ULONG nuHotKeyID,	// ���ȶ���õĳ�������������Hotkey�����ܳ�����ͬ��ID����ͼע�����е�Hotkey����ʧ��
		IN ULONG nuVkNumber,	// �������
		IN bool nbControlKey,	// �Ƿ���ҪControl���
		IN bool nbShiftKey,		// �Ƿ���ҪShift���
		IN bool nbAltKey		// �Ƿ���ҪAlt���
		);

	// ע����ݼ�
	bool UnregisterHotKey(
		IN IEinkuiIterator* npApplicant,	// ע����
		IN ULONG UnuKeyNumber
		);

	// ����Ƿ���з��ϵ�HotKey
	bool DetectHotKey(
		CXuiHotkeyEntry& rToDetect
		);

	// ����Ԫ�أ�����֪ͨһ��Ԫ�����٣�ע�⣺��Ԫ�������յ�����Ϣ��Ӧ�ü�ʱ����Ԫ�ع�������UnregisterElement������
	// �Ӷ�����Ԫ�ع�������������һ��Ԫ�ط���������Ϣ�������ٽ��Լ���Ԫ�ع�����ע���������ͷ��������
	virtual ERESULT OnElementDestroy();
};















#endif//_ELEITERATORIMP_H_
