/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#ifndef _ELEMENTIMP_H_
#define _ELEMENTIMP_H_

#include "XCtl.h"
#include "Xuix.h"

#if 0
//////////////////////////////////////////////////////////////////////////
// Elementģ���ʹ�÷���

// ͷ�ļ���
DECLARE_BUILTIN_NAME(Button)	// �˴���Button����EType�ַ�����ע�⣬��Ҫʹ������
class CEvButton: public CXuiElement<CEvButton,GET_BUILTIN_NAME(Button)>
{
	// ��������캯���趨Ϊprotected������Ҫ����仰; ���򣬲���Ҫ�������
	friend CXuiElement<CEvButton,GET_BUILTIN_NAME(Button)>;
public:

	// ֻ��Ҫ�����Լ����ĵķ���
	virtual ERESULT ParseMessage(IEinkuiMessage* npMsg);

protected:
	CEvButton();		// ���Խ����캯���趨Ϊ�������ͣ�ȷ��ʵ��������ͨ����׼��CreateInstance������������ֱ����C++����ķ�������
	~CEvButton();

};


// Դ�ļ���
DEFINE_BUILTIN_NAME(Button)
ERESULT CEvButton::ParseMessage(IEinkuiMessage* npMsg)
{
	return CXuiElement<CEvButton,GET_BUILTIN_NAME(Button)>::ParseMessage(npMsg);
}

/////////////////////////////////////////////////////////////////////////*/
#endif



//////////////////////////////////////////////////////////////////////////
// ʵ��Element�ĸ���ģ�壬��������Elementʱ�����ԴӴ�ģ��ʵ�������������ࡣ
template<class CClassDeriveTo,const wchar_t* Name>
class CXuiElement: public cmmBaseObject<CClassDeriveTo,IXsElement,Name>
{
public:
	// ��ñ�Ԫ�ص�Ԫ������
	virtual const wchar_t* __stdcall  GetType(void);

	// ��ȫ�ַ�Χ����֤�˶����Ƿ���nswTypeָ��������
	virtual bool __stdcall  GlobleVerification(const wchar_t* nswType);

	// Ĭ����Ϣ��ں��������ڽ���������Ϣ
	virtual ERESULT __stdcall MessageReceiver(IEinkuiMessage* npMsg = NULL);

	// ��ñ�Ԫ�صĵ������ӿ�
	virtual IEinkuiIterator* __stdcall GetIterator(void);

	// ���Tab Order, -1 δ����
	virtual LONG __stdcall GetTabOrder(void);

	// ���Z Order��-1 δ����
	virtual LONG __stdcall GetZOrder(void);

	// �������༰����������ʱ�����ر�ע��!!! һ��Ҫ���ȵ��û���ķ���
	// �����������ڽ�����Ԫ�ض��󡢳�ʼ���������ݣ�λ�á���С������ͼ�ȣ�
	ULONG InitOnCreate(
		IN IEinkuiIterator* npParent,	// ������ָ��
		IN ICfKey* npTemplete,		// npTemplete��Key ID����EID��ֵ��������EType
		IN ULONG nuEID=MAXULONG32		// �����Ϊ0��MAXULONG32����ָ����Ԫ�ص�EID; ����ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��XUIϵͳ�Զ�����
		);

	// ͨ����������ʵ��������
	static CClassDeriveTo* CreateInstance(IEinkuiIterator* npParent,ICfKey* npTemplete,ULONG nuEID=0) 
	{		// ����C++Ĭ�ϵ�new operator������operator new�����������ȵ����������ص�void* __cdecl operator new(size_t nSize,bool nbDummy)�����ã������ȥ����Ĭ�ϵĹ��캯��
			CClassDeriveTo* lpObj = new (false)CClassDeriveTo;
			if(lpObj != NULL)
			{
				// ���ñ�־��ʾ���������ڴ���Ҫ���ͷ�
				lpObj->SetDeleteRequired();
				// ���ó�ʼ����������鷵��ֵ�����ʧ�ܣ��ͷŵ��ö���
				if((lpObj->InitOnCreate(npParent,npTemplete,nuEID)&0x80000000)!=0)
				{
					lpObj->Release();
					lpObj = NULL;
				}
				else
					lpObj->CompleteCreation();
			}
			// ���ع�����ϵĶ���
			return lpObj;
	}


	// ������ɺ������ṩһ���������ཨ�����֮�ʵ��û��෽���Ļ��ᣬ�����ĳ������������������࣬����ϣ�����������������!!!ע��!!!��һ��Ҫ���˳�֮ǰ�����ʱ�̵��û������ͬ����
	void CompleteCreation(void){
		mpIterator->Start();
	}

private:
	// ���λ����ṩ��Ĭ��ʵ��������
	static CClassDeriveTo* CreateInstance(){
		return NULL;
	} 

public:

	//////////////////////////////////////////////////////////////////////////
	// ����������������Ϣ����������׶Σ��������ɱ����CXuiElement::MessageReceiver���õģ�һ���޸���Ĭ�ϵ���Ϣ���ܺ����������д�����Ϣ�������
	//////////////////////////////////////////////////////////////////////////

	// �ֽ���Ϣǰ���裬�ṩһ������Ϣ�ֽ�ǰ�����Ϣ�Ļ��ᣬ�������޸���Ϣ���ݵ�Ŀ�ģ���������Ĺ����ǣ�һ��Ҫ���ȵ��û����BeforeParseMessage���Ӷ�ȷ�����ౣ������Ϣ������Ȩ��
	// ͨ������£�����Ҫ���ر����������������趨Ŀ����Ϊ��ȷ��ĳ����Ϣ�Ĵ�����Ϊ�ܹ��������������У��������������޸ģ�
	// ����ERESULT_UNEXPECTED_MESSAGE��������Ϣ�ֽ���̣�����ֵ����ʹ����Ϣ�ֽ������ֹ
	virtual ERESULT BeforeParseMessage(IEinkuiMessage* npMsg);

	// �ֽ���Ϣ���ṩ��Ϣ�ֽ����Ϣ��Ӧ�Ĺ��ܣ������ʵ���ǽ���Ϣ�ֽ�Ϊ��ͬ������󣬵�����Ӧ�Ĵ����麯�������ڲ���ʶ����Ϣ��һ�ɷ���ERESULT_UNEXPECTED_MESSAGE
	// �������ķ���ֵ���Զ�ͬ�����õ�npMsgָ�����Ϣ������
	virtual ERESULT ParseMessage(IEinkuiMessage* npMsg);

	// �ֽ���Ϣ���裬�ṩһ������Ϣ������Ϻ�ִ�еĻ��ᣬͨ�����ڼ����Ϣ�Ľ���������������Ϊ����������ĵ��ù����ǣ�һ��Ҫ�����û����ʵ�֣��Ӷ�ȷ���������ִ��
	// ͨ������£�����Ҫ���ر����������������趨Ŀ����Ϊ��ȷ��ĳ����Ϣ�Ĵ�����Ϊ�ܹ��������������У���������֮��ִ�У�
	// �����ض�Ŀ�ģ���Ӧ���޸ĺ�������Ϣ����ķ���ֵ���÷���ֵ�����npMsgָ�����Ϣ������;���������践��ֵ
	virtual void AfterParseMessage(IEinkuiMessage* npMsg);
	
	//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
	virtual ERESULT OnElementCreate(IEinkuiIterator* npIterator);

	// ����Ԫ�أ�����֪ͨһ��Ԫ�����٣�ע�⣺��Ԫ�������յ�����Ϣ��Ӧ�ü�ʱ����Ԫ�ع�������UnregisterElement������
	// �Ӷ�����Ԫ�ع�������������һ��Ԫ�ط���������Ϣ�������ٽ��Լ���Ԫ�ع�����ע���������ͷ��������
	virtual ERESULT OnElementDestroy();

	// ��������
	virtual ERESULT OnMouseOwnerTest(const D2D1_POINT_2F& rPoint);

	//��ť�����¼�
	virtual ERESULT OnCtlButtonClick(IEinkuiIterator* npSender);

	//Ԫ��λ�ñ��ı�,�Ѿ��ı����
	virtual ERESULT OnElementMoved(D2D1_POINT_2F nNewPoint);

	//Ԫ�زο��ߴ緢���仯
	virtual ERESULT OnElementResized(D2D1_SIZE_F nNewSize);

	//֪ͨԪ�ء���ʾ/���ء������ı�
	virtual ERESULT OnElementShow(bool nbIsShow);

	//��ʱ��
	virtual void OnTimer(
		PSTEMS_TIMER npStatus
		);

	// ��ˢ��
	virtual void OnLazyUpdate(
		PSTEMG_LAZY_UPDATE npLazyUpdate
		);

	//����ת����Hook����Ϣ
	virtual ERESULT OnHookMessage(IEinkuiIterator* npOriginalElement,	// ԭʼ����Ϣ����Ԫ��
								  IEinkuiMessage* npOriginalMessage		// ���������Ϣ
								 );

	//������Ϣ
	virtual ERESULT OnResponseMessage(IEinkuiIterator* npOriginalElement,	// ԭʼ��Ϣ�Ľ���Ԫ��
									  IEinkuiMessage* npOriginalMessage,		// ����������Ϣ
									  void* npContext					// ������Ϣ����ʱ��ָ����������
									 );

	//��갴��
	virtual ERESULT OnMousePressed(const STEMS_MOUSE_BUTTON* npInfo);

	//���˫��
	virtual ERESULT OnMouseDbClick(const STEMS_MOUSE_BUTTON* npInfo);

	//����ƶ�
	virtual ERESULT OnMouseMoving(const STEMS_MOUSE_MOVING* npInfo);

	//��������뿪
	virtual void OnMouseFocus(PSTEMS_STATE_CHANGE npState);

	//�����ͣ
	virtual ERESULT OnMouseHover();

	//��껬��
	virtual ERESULT OnMouseWheel(const STEMS_MOUSE_WHEEL* npInfo);

	//���̽����û���ʧȥ
	virtual void OnKeyBoardFocus(PSTEMS_STATE_CHANGE npState);

	//������Ϣ
	virtual ERESULT OnKeyPressed(const STEMS_KEY_PRESSED* npInfo);

	//��ݼ���Ϣ
	virtual ERESULT OnHotKey(const STEMS_HOTKEY* npHotKey);

	//������Ϣ
	virtual ERESULT OnPaint(IEinkuiPaintBoard* npPaintBoard);

	//���û�����
	virtual ERESULT OnElementEnable(bool nbIsEnable);

	//Ԫ����ק
	virtual ERESULT OnDragging(const STMS_DRAGGING_ELE* npInfo);

	//��ק��ʼ
	virtual ERESULT OnDragBegin(const STMS_DRAGGING_ELE* npInfo);

	//��ק����
	virtual ERESULT OnDragEnd(const STMS_DRAGGING_ELE* npInfo);

	//����
	virtual ERESULT OnCommand(const nes_command::ESCOMMAND neCmd);

	////���Լ������ڷ��ͼ���Ϣ
	//ERESULT __stdcall PostMessageToParent(
	//	IN ULONG nuMsgID,		// ��Ϣ����
	//	IN void* npInputBuffer,	// �������ݵĻ�����
	//	IN int niInputSize		// �������ݵĴ�С
	//	);

	// Postģʽ������Ϣ������ֱ�ӷ���Я���Ĳ�����ע�⣬���������һ��ָ�룬��ֻ�Ḵ�ƴ���ָ�루һ����ַ�����������Ḵ�ƴ���ָ��ָ������ݣ�Post������Ϣ��Ҫע��������⣬��ֹ��ַ����Ϣ�����߳��޷����ʡ�
	// ���û�����ݷ��ͣ�rDataToPost��дCExMessage::DataInvalid;
	template<typename DataType>
	ERESULT PostMessageToParent(ULONG nuMsgID,const DataType& rDataToPost);

	// Sendģʽ������Ϣ������ֱ�ӷ���Я���Ĳ�����ע�⣬���������һ��ָ�룬��ֻ�Ḵ�ƴ���ָ�루һ����ַ�����������Ḵ�ƴ���ָ��ָ������ݣ�Sendģʽ������Ϣʱ���ⲻ�ᵼ����������
	// ���û�����ݷ��ͣ�rDataToPost��дCExMessage::DataInvalid
	template<typename DataType>
	ERESULT SendMessageToParent(ULONG nuMsgID,const DataType& rDateForSend,void* npBufferForReceive,int niBytesOfBuffer);

	//�����Լ����е���Ԫ��
	virtual ERESULT LoadSubElement();

protected:
	ICfKey* mpTemplete;			     //�����ļ�ָ�룬������ȡ��������
	IEinkuiIterator* mpIterator;
	IEinkuiBitmap* mpBgBitmap;			 //Ĭ�ϱ���
	HCURSOR mhPreviousCursor;		//���������Լ�ǰ��״̬
	HCURSOR mhInnerCursor;			//��Ԫ�����õ�Cursor

	// ֻ���ڱ������ó�ʼֵ����ָ����ΪNULL�����п���ʧ�ܵ������֮������㶼Ӧ����InitOnCreate�н���
	CXuiElement()
	{
		mpTemplete = NULL;
		mpIterator = NULL;
		mpBgBitmap = NULL;
		mhPreviousCursor = NULL;
		mhInnerCursor = NULL;
	}

	// �����ͷų�Ա����
	~CXuiElement()
	{
		//CMMASSERT(mpTemplete!=NULL);
		//CMMASSERT(mpBgBitmap!=NULL);
	}

	
	// ����λ��־������ 0 - 28
	bool SetFlags(
		int niIndex,		// ��־����ţ���0��ʼ���������������������������Ҹ���������2����ϣ������������û��޸ĵı�־����ô���ĺ�������ʱ��niIndex=0��ʾ�������Ļ����2
		bool nbSet		// ���û��������־
		) {
		return cmmBaseObject::SetFlags(niIndex+2,nbSet);
	}

	// ��ȡ��־
	bool TestFlag(int niIndex){
		return cmmBaseObject::TestFlag(niIndex+2);
	}

};

#define TF_ID_ELEMENT_BACKGROUND L"BackGround"			//Ĭ�ϱ���ͼ
#define TF_ID_ELEMENT_X L"X"							//X����
#define TF_ID_ELEMENT_Y L"Y"							//Y����
#define TF_ID_ELEMENT_WIDTH L"Width"					//�ο���
#define TF_ID_ELEMENT_HEIGHT L"Height"					//�ο���
#define TF_ID_ELEMENT_EXTEND_X L"BackGround/ExtendX"				//������չ��
#define TF_ID_ELEMENT_EXTEND_Y L"BackGround/ExtendY"				//������չ��



//////////////////////////////////////////////////////////////////////////
// ��������ת��ΪԪ���࣬!!!ע��!!!ֻ��ת��Ϊ��ģ����ʵ�ֵ�Ԫ����(��ģ�鼴ͬһ��DLL)�����Բ���ȥ��ͼֱ�ӷ�������ģ��ʵ�ֵ�Ԫ����ķ�����������Ԫ����ķ���һ��ʹ����Ϣģ�͡�
template <typename Type>
__inline 
void EsIterator2Element(IEinkuiIterator* npIterator, const wchar_t* nswEType,Type& rElementPointer){

	IXsElement* lpElement;
	CMMASSERT(nswEType != NULL && nswEType[0]!=UNICODE_NULL);
	if(npIterator != NULL && (lpElement = npIterator->GetElementObject())!=NULL && lpElement->IsKindOf(nswEType)!=false)
		rElementPointer = dynamic_cast<Type>(lpElement);
	else
		rElementPointer = NULL;
}



//////////////////////////////////////////////////////////////////////////
// ������Eelementģ��ĺ���ʵ��

template<class CClassDeriveTo,const wchar_t* Name>
const wchar_t* CXuiElement<CClassDeriveTo,Name>::GetType(void)
{
	return GetObjectName();
}

// ��ȫ�ַ�Χ����֤�˶����Ƿ���nswTypeָ��������
template<class CClassDeriveTo,const wchar_t* Name>
bool CXuiElement<CClassDeriveTo,Name>::GlobleVerification(const wchar_t* nswType)
{
	return (_wcsicmp(nswType,GetObjectName())==0);
}

// Ĭ����Ϣ��ں��������ڽ���������Ϣ
template<class CClassDeriveTo,const wchar_t* Name>
ERESULT __stdcall CXuiElement<CClassDeriveTo,Name>::MessageReceiver(IEinkuiMessage* npMsg)
{
	ERESULT luResult=ERESULT_UNSUCCESSFUL;

	try
	{
		// ������ϢԤ����
		luResult = BeforeParseMessage(npMsg);
		if(luResult != ERESULT_UNEXPECTED_MESSAGE)
			THROW_FALSE;	// ��ֹ��Ϣ�ֽ����

		luResult = ParseMessage(npMsg);

		// ������Ϣ����ֵ
		npMsg->SetResult(luResult);

		// ���ú������
		AfterParseMessage(npMsg);
	}
	catch (...)
	{
		luResult = ERESULT_UNSUCCESSFUL;
		// ������Ϣ����ֵ
		npMsg->SetResult(luResult);
	}

	return luResult;
}

// �������༰����������ʱ�����ر�ע��!!! һ��Ҫ���ȵ��û���ķ���
// �����������ڽ�����Ԫ�ض��󡢳�ʼ���������ݣ�λ�á���С������ͼ�ȣ�
template<class CClassDeriveTo,const wchar_t* Name>
ULONG CXuiElement<CClassDeriveTo,Name>::InitOnCreate(
	IN IEinkuiIterator* npParent,	// ������ָ��
	IN ICfKey* npTemplete,		// npTemplete��Key ID����EID��ֵ��������EType
	IN ULONG nuEID	// �����Ϊ0��MAXULONG32����ָ����Ԫ�ص�EID; ����ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��XUIϵͳ�Զ�����
	)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		if(nuEID == MAXULONG32 && npTemplete != NULL)
			nuEID = npTemplete->GetID();

		mpTemplete = npTemplete;

		// !!!����!!! ������ϵͳע���Լ�
		mpIterator = EinkuiGetSystem()->GetElementManager()->RegisterElement(npParent,this,nuEID);	//IDΪ0��ʾ�����Զ�����
		if(mpIterator == NULL)
			break;

		//�����Լ��ķ��
		mpIterator->SetStyles(EITR_STYLE_MOUSE);

		ICfKey* lpBgBitmapKey = NULL;
		LONG llWidth = 0;
		LONG llHeight = 0;
		do 
		{
			BREAK_ON_NULL(npTemplete);

			mpTemplete->AddRefer();

			//����������ã����㲻�ɹ�Ҳ��Ӱ��Ԫ�صĴ���
			//����Ԫ������
			LONG llPosX = mpTemplete->QuerySubKeyValueAsLONG(TF_ID_ELEMENT_X,MAXUINT32);
			LONG llPosY = mpTemplete->QuerySubKeyValueAsLONG(TF_ID_ELEMENT_Y,MAXUINT32);
			if(llPosX != MAXUINT32 && llPosY != MAXUINT32)
				mpIterator->SetPosition((FLOAT)llPosX,(FLOAT)llPosY);

			//��ȡ�ο��ߴ�����
			llWidth = mpTemplete->QuerySubKeyValueAsLONG(TF_ID_ELEMENT_WIDTH,MAXUINT32);
			llHeight = mpTemplete->QuerySubKeyValueAsLONG(TF_ID_ELEMENT_HEIGHT,MAXUINT32);
			if (llWidth != MAXUINT32 && llHeight != MAXUINT32 )
				mpIterator->SetSize((FLOAT)llWidth,(FLOAT)llHeight);
			
			//װ��Ĭ�ϱ���
			wchar_t lszPath[MAX_PATH] = {0};
			lpBgBitmapKey = mpTemplete->GetSubKey(TF_ID_ELEMENT_BACKGROUND);
			BREAK_ON_NULL(lpBgBitmapKey);
			if(lpBgBitmapKey->GetValue(lszPath,MAX_PATH*sizeof(wchar_t)) <= 0)	//��ȡͼƬ����
				break;

			mpBgBitmap = EinkuiGetSystem()->GetAllocator()->LoadImageFile(lszPath,lszPath[0] == L'.'?false:true);
			BREAK_ON_NULL(mpBgBitmap);

			//������չ��,0��ʾδ���ã�-1��ʾʹ��������Ϊ��չ��
			mpBgBitmap->SetExtendLine(mpTemplete->QuerySubKeyValueAsLONG(TF_ID_ELEMENT_EXTEND_X,0),mpTemplete->QuerySubKeyValueAsLONG(TF_ID_ELEMENT_EXTEND_Y,0));

			// ���ģ����û��ָ���ο��ߴ磬��ô�ӱ���ͼ����ȡ�ο��ߴ�
			if (llWidth == MAXUINT32 || llHeight == MAXUINT32 )
			{
				//���û�����òο��ߴ磬����ͼƬ��СΪ�ο��ߴ�
				llWidth = (LONG)mpBgBitmap->GetWidth();
				llHeight = (LONG)mpBgBitmap->GetHeight();

				//���òο��ߴ�
				if(llWidth != 0 && llHeight != 0)
					mpIterator->SetSize((FLOAT)llWidth,(FLOAT)llHeight);
			}
		} while (false);

		CMM_SAFE_RELEASE(lpBgBitmapKey);

		lResult = LoadSubElement();	//����������Ԫ��

	}while(false);


	//����������Ԫ��
	return lResult;
}

// ��ñ�Ԫ�صĵ������ӿ�
template<class CClassDeriveTo,const wchar_t* Name>
IEinkuiIterator* __stdcall CXuiElement<CClassDeriveTo,Name>::GetIterator(void)
{
	return mpIterator;
}

// ���Tab Order, -1 δ����
template<class CClassDeriveTo,const wchar_t* Name>
LONG __stdcall CXuiElement<CClassDeriveTo,Name>::GetTabOrder(void)
{
	LONG llOrder;
	if(mpTemplete != NULL)
		llOrder = mpTemplete->QuerySubKeyValueAsLONG(L"TOrder",-1);
	else
		llOrder = -1;

	return llOrder;
}

// ���Z Order��-1 δ����
template<class CClassDeriveTo,const wchar_t* Name>
LONG __stdcall CXuiElement<CClassDeriveTo,Name>::GetZOrder(void)
{
	LONG llOrder;
	if(mpTemplete != NULL)
		llOrder = mpTemplete->QuerySubKeyValueAsLONG(L"ZOrder",-1);
	else
		llOrder = -1;
	return llOrder;
}



//////////////////////////////////////////////////////////////////////////
// ����������������Ϣ����������׶Σ��������ɱ����CXuiElement::MessageReceiver���õģ�һ���޸���Ĭ�ϵ���Ϣ���ܺ����������д�����Ϣ�������
//////////////////////////////////////////////////////////////////////////

// �ֽ���Ϣǰ���裬�ṩһ������Ϣ�ֽ�ǰ�����Ϣ�Ļ��ᣬ�������޸���Ϣ���ݵ�Ŀ�ģ���������Ĺ����ǣ�һ��Ҫ���ȵ��û����BeforeParseMessage���Ӷ�ȷ�����ౣ������Ϣ������Ȩ��
// ͨ������£�����Ҫ���ر����������������趨Ŀ����Ϊ��ȷ��ĳ����Ϣ�Ĵ�����Ϊ�ܹ��������������У��������������޸ģ�
// ����ERESULT_UNEXPECTED_MESSAGE��������Ϣ�ֽ���̣�����ֵ����ʹ����Ϣ�ֽ������ֹ
template<class CClassDeriveTo,const wchar_t* Name>
ERESULT CXuiElement<CClassDeriveTo,Name>::BeforeParseMessage(IEinkuiMessage* npMsg)
{
	// �������ʾ�������Ҫʹ���븴�ƣ���ȥ��ǰ���ʧЧ�����

#if 0

	ERESULT luResult = ERESULT_UNEXPECTED_MESSAGE;

	// ���ȵ��û����ʵ�֣�ע�⣺һ��Ҫ��������ֱ�ӻ���
	luResult = CXuiElement::BeforeParseMessage(npMsg);
	if(luResult != ERESULT_UNEXPECTED_MESSAGE)
		return luResult;

	switch (npMsg->GetMessageID())
	{
	case EMSG_TEST:	// ����Ȥ����Ϣ
		//�����޸���Ϣ����
		//���Բ���������Ϣ
		//luResult = ERESULT_SUCCESS;	������ֹ��Ϣ�ֽ�
	default:
		luResult = ERESULT_UNEXPECTED_MESSAGE;
	}

	// ���ϣ����ֹ��Ϣ�ֽ���̣��򷵻س�ERESULT_UNEXPECTED_MESSAGE�����ֵ
	return luResult;
#else
	return ERESULT_UNEXPECTED_MESSAGE;
#endif
}

// �ֽ���Ϣ���ṩ��Ϣ�ֽ����Ϣ��Ӧ�Ĺ��ܣ������ʵ���ǽ���Ϣ�ֽ�Ϊ��ͬ������󣬵�����Ӧ�Ĵ����麯�������ڲ���ʶ����Ϣ��һ�ɷ���ERESULT_UNEXPECTED_MESSAGE
// �������ķ���ֵ���Զ�ͬ�����õ�npMsgָ�����Ϣ������
//ERESULT CXuiElement::ParseMessage(IEinkuiMessage* npMsg)
//{
//	// ʵ��ԭ�����ȵ�������ķֽ⹦�ܣ����󽫲��������Ϣ��������
//
//	ERESULT luResult = ERESULT_UNEXPECTED_MESSAGE;
//
//	switch (npMsg->GetMessageID())
//	{
//	case EMSG_TEST:	// ����Ȥ����Ϣ
//		{
//			if(npMsg->GetInputDataSize() != sizeof(bool) || npMsg->GetOutputBufferSize() != sizeof(bool))
//			{
//				luResult = ERESULT_WRONG_PARAMETERS;
//				break;
//			}
//			// ��ȡ��������
//			bool* lpValue = (bool*)npMsg->GetInputData();
//
//			// �����������
//			bool* lpOut = (bool*)npMsg->GetOutputBuffer();
//
//			*lpOut = !(*lpValue);
//
//			npMsg->SetOutputDataSize(sizeof(bool));
//
//			luResult = ERESULT_SUCCESS;// ����ֵ��IEinkuiMessage����ķ���ֵ�ɱ������ĵ��������ݱ������Ľ������
//		}
//	default:
//		luResult = ERESULT_NOT_SET;
//	}
//
//	if(luResult == ERESULT_NOT_SET)
//	{
//		luResult = CXuiElement::ParseMessage(npMsg); // ���û����ͬ��������ע�⣺һ��Ҫ��������ֱ�ӻ���
//	}
//
//	return luResult;
//}


// �ֽ���Ϣ���ṩ��Ϣ�ֽ����Ϣ��Ӧ�Ĺ��ܣ������ʵ���ǽ���Ϣ�ֽ�Ϊ��ͬ������󣬵�����Ӧ�Ĵ����麯�������ڲ���ʶ����Ϣ��һ�ɷ���ERESULT_UNEXPECTED_MESSAGE
// �������ķ���ֵ���Զ�ͬ�����õ�npMsgָ�����Ϣ������
template<class CClassDeriveTo,const wchar_t* Name>
ERESULT CXuiElement<CClassDeriveTo,Name>::ParseMessage(IEinkuiMessage* npMsg)
{
	// ʵ��ԭ�����ȵ�������ķֽ⹦�ܣ����󽫲��������Ϣ��������

	ERESULT luResult = ERESULT_UNEXPECTED_MESSAGE;

	switch (npMsg->GetMessageID())
	{
	case EMSG_TEST:	// ����Ȥ����Ϣ
		{
			if(npMsg->GetInputDataSize() != sizeof(bool) || npMsg->GetOutputBufferSize() != sizeof(bool))
			{
				luResult = ERESULT_WRONG_PARAMETERS;
				break;
			}
			// ��ȡ��������
			bool* lpValue = (bool*)npMsg->GetInputData();

			// �����������
			bool* lpOut = (bool*)npMsg->GetOutputBuffer();

			*lpOut = !(*lpValue);

			npMsg->SetOutputDataSize(sizeof(bool));

			luResult = ERESULT_SUCCESS;// ����ֵ��IEinkuiMessage����ķ���ֵ�ɱ������ĵ��������ݱ������Ľ������

			break;
		}
	case EMSG_CREATE: 
		{
			//��ʼ����
			luResult = OnElementCreate(*(IEinkuiIterator**)npMsg->GetInputData());
			break;
		}
	case EMSG_DESTROY: 
		{
			//Ԫ������
			luResult = OnElementDestroy();
			break;
		}
	case EMSG_MOUSE_OWNER_TEST:
		{
			D2D1_POINT_2F ldPoint;
			if(npMsg->GetInputDataSize() != sizeof(ldPoint))
				return ERESULT_WRONG_PARAMETERS;

			ldPoint = *(D2D1_POINT_2F*)npMsg->GetInputData();
			luResult = OnMouseOwnerTest(ldPoint);
		}
		break;
	case EEVT_BUTTON_CLICK: 
		{
			//��ť����
			luResult = OnCtlButtonClick(npMsg->GetMessageSender());

			break;
		}
	case EMSG_ELEMENT_MOVED: 
		{
			//Ԫ��λ�ñ��ı�,�Ѿ��ı����
			if(npMsg->GetInputDataSize() != sizeof(D2D1_POINT_2F))
			{
				luResult = ERESULT_WRONG_PARAMETERS;
				break;
			}

			D2D1_POINT_2F* lpValue = (D2D1_POINT_2F*)npMsg->GetInputData();

			luResult = OnElementMoved(*lpValue);

			break;
		}
	case EMSG_ELEMENT_RESIZED:
		{
			//Ԫ�زο��ߴ緢���仯
			if(npMsg->GetInputDataSize() != sizeof(D2D1_SIZE_F))
			{
				luResult = ERESULT_WRONG_PARAMETERS;
				break;
			}

			D2D1_SIZE_F* lpValue = (D2D1_SIZE_F*)npMsg->GetInputData();

			luResult = OnElementResized(*lpValue);

			break;
		}
	case EMSG_SHOW_HIDE: 
		{
			//��ʾ������
			if(npMsg->GetInputDataSize() != sizeof(bool))
			{
				luResult = ERESULT_WRONG_PARAMETERS;
				break;
			}

			bool* lpValue = (bool*)npMsg->GetInputData();

			luResult = OnElementShow(*lpValue);

			break;
		}
	case EMSG_ENALBE_DISABLE: 
		{
			//���û����
			if(npMsg->GetInputDataSize() != sizeof(bool))
			{
				luResult = ERESULT_WRONG_PARAMETERS;
				break;
			}

			bool* lpValue = (bool*)npMsg->GetInputData();

			luResult = OnElementEnable(*lpValue);

			break;
		}
	case EMSG_TIMER: 
		{
			//��ʱ��
			if(npMsg->GetInputDataSize() != sizeof(STEMS_TIMER))
			{
				luResult = ERESULT_WRONG_PARAMETERS;
				break;
			}

			OnTimer((STEMS_TIMER*)npMsg->GetInputData());

			luResult = ERESULT_SUCCESS;
			break;
		}
	case EMSG_LAZY_UPATE:
		{
			if(npMsg->GetInputDataSize() != sizeof(STEMG_LAZY_UPDATE))
			{
				luResult = ERESULT_WRONG_PARAMETERS;
				break;
			}

			OnLazyUpdate((STEMG_LAZY_UPDATE*)npMsg->GetInputData());

			luResult = ERESULT_SUCCESS;
			break;
		}
	case EMSG_HOOKED_MESSAGE: 
		{
			//����ת����Hook����Ϣ
			if(npMsg->GetInputDataSize() != sizeof(STEMS_HOOKED_MSG))
			{
				luResult = ERESULT_WRONG_PARAMETERS;
				break;
			}

			STEMS_HOOKED_MSG* lpValue = (STEMS_HOOKED_MSG*)npMsg->GetInputData();

			luResult = OnHookMessage(lpValue->OriginalElement,lpValue->OriginalMessage);
			break;
		}
	case EMSG_RESPONSE_MESSAGE: 
		{
			//������Ϣ
			if(npMsg->GetInputDataSize() != sizeof(STEMS_RESPONSE_MSG))
			{
				luResult = ERESULT_WRONG_PARAMETERS;
				break;
			}

			STEMS_RESPONSE_MSG* lpValue = (STEMS_RESPONSE_MSG*)npMsg->GetInputData();

			luResult = OnResponseMessage(lpValue->OriginalElement,lpValue->OriginalMessage,lpValue->Context);
			break;
		}
	case EMSG_MOUSE_BUTTON: 
		{
			//��갴��
			if(npMsg->GetInputDataSize() != sizeof(STEMS_MOUSE_BUTTON))
			{
				luResult = ERESULT_WRONG_PARAMETERS;
				break;
			}

			const STEMS_MOUSE_BUTTON* lpValue = (const STEMS_MOUSE_BUTTON*)npMsg->GetInputData();

			luResult = OnMousePressed(lpValue);

			break;
		}
	case EMSG_MOUSE_DBCLICK: 
		{
			//��갴��
			if(npMsg->GetInputDataSize() != sizeof(STEMS_MOUSE_BUTTON))
			{
				luResult = ERESULT_WRONG_PARAMETERS;
				break;
			}

			const STEMS_MOUSE_BUTTON* lpValue = (const STEMS_MOUSE_BUTTON*)npMsg->GetInputData();

			luResult = OnMouseDbClick(lpValue);

			break;
		}
	case EMSG_MOUSE_MOVING: 
		{
			//����ƶ�
			if(npMsg->GetInputDataSize() != sizeof(STEMS_MOUSE_MOVING))
			{
				luResult = ERESULT_WRONG_PARAMETERS;
				break;
			}

			const STEMS_MOUSE_MOVING* lpValue = (const STEMS_MOUSE_MOVING*)npMsg->GetInputData();

			luResult = OnMouseMoving(lpValue);
			break;
		}
	case EMSG_MOUSE_FOCUS:
		{
			//��������뿪
			if(npMsg->GetInputDataSize()!=sizeof(STEMS_STATE_CHANGE))
				break;
			OnMouseFocus((STEMS_STATE_CHANGE*)npMsg->GetInputData());
			luResult = ERESULT_SUCCESS;
			break;
		}
	case EMSG_MOUSE_HOVER: 
		{
			//�����ͣ
			luResult = OnMouseHover();
			break;
		}
	case EMSG_MOUSE_WHEEL: 
		{
			//��껬��
			if(npMsg->GetInputDataSize() != sizeof(STEMS_MOUSE_WHEEL))
			{
				luResult = ERESULT_WRONG_PARAMETERS;
				break;
			}

			const STEMS_MOUSE_WHEEL* lpValue = (const STEMS_MOUSE_WHEEL*)npMsg->GetInputData();

			luResult = OnMouseWheel(lpValue);

			break;
		}
	case EMSG_KEYBOARD_FOCUS:
		{
			//���̽����û���ʧȥ
			if(npMsg->GetInputDataSize()!=sizeof(STEMS_STATE_CHANGE))
				break;
			OnKeyBoardFocus((STEMS_STATE_CHANGE*)npMsg->GetInputData());

			luResult = ERESULT_SUCCESS;
			break;
		}
	case EMSG_KEY_PRESSED: 
		{
			//������Ϣ
			if(npMsg->GetInputDataSize() != sizeof(STEMS_KEY_PRESSED))
			{
				luResult = ERESULT_WRONG_PARAMETERS;
				break;
			}

			const STEMS_KEY_PRESSED* lpValue = (const STEMS_KEY_PRESSED*)npMsg->GetInputData();

			luResult = OnKeyPressed(lpValue);
			break;
		}
	case EMSG_HOTKEY_PRESSED:
		{
			//��ݼ���Ϣ
			const STEMS_HOTKEY* lpHotKey;
			luResult = CExMessage::GetInputDataBuffer(npMsg,lpHotKey);
			if(luResult != ERESULT_SUCCESS)
				break;
			luResult = OnHotKey(lpHotKey);
			break;
		}
	case EMSG_PAINT:	
		{
			//������Ϣ
			if(npMsg->GetInputDataSize() != sizeof(IEinkuiPaintBoard*))
			{
				luResult = ERESULT_WRONG_PARAMETERS;
				break;
			}

			IEinkuiPaintBoard* lpValue = *(IEinkuiPaintBoard**)npMsg->GetInputData();

			luResult = OnPaint(lpValue);
			break;
		}
	case EMSG_DRAGGING_ELEMENT:
		if(npMsg->GetInputDataSize() != sizeof(STMS_DRAGGING_ELE))	{
			luResult = ERESULT_WRONG_PARAMETERS;
			break;
		}

		luResult = OnDragging((const STMS_DRAGGING_ELE*)npMsg->GetInputData());
		break;
	case EMSG_DRAG_BEGIN: 
		if(npMsg->GetInputDataSize() != sizeof(STMS_DRAGGING_ELE))	{
			luResult = ERESULT_WRONG_PARAMETERS;
			break;
		}

		luResult = OnDragBegin((const STMS_DRAGGING_ELE*)npMsg->GetInputData());
		break;
	case EMSG_DRAG_END: 
		if(npMsg->GetInputDataSize() != sizeof(STMS_DRAGGING_ELE))	{
			luResult = ERESULT_WRONG_PARAMETERS;
			break;
		}

		luResult = OnDragEnd((const STMS_DRAGGING_ELE*)npMsg->GetInputData());
		break;
	case EMSG_COMMAND:
		{
			nes_command::ESCOMMAND leCmd;
			luResult = CExMessage::GetInputData(npMsg,leCmd);
			if(luResult != ERESULT_SUCCESS)
				break;

			luResult = OnCommand(leCmd);
		}
		break;
	default:
		luResult = ERESULT_NOT_SET;
		break;
	}

	if(luResult == ERESULT_NOT_SET)
	{
		// luResult = CXuiElement::ParseMessage(npMsg); // ���û����ͬ��������ע�⣺һ��Ҫ��������ֱ�ӻ���
		luResult = ERESULT_UNEXPECTED_MESSAGE;	// ���û�л��࣬��������ʱ��ɾ�����䣻
	}

	return luResult;
}

// �ֽ���Ϣ���裬�ṩһ������Ϣ������Ϻ�ִ�еĻ��ᣬͨ�����ڼ����Ϣ�Ľ���������������Ϊ����������ĵ��ù����ǣ�һ��Ҫ�����û����ʵ�֣��Ӷ�ȷ���������ִ��
// ͨ������£�����Ҫ���ر����������������趨Ŀ����Ϊ��ȷ��ĳ����Ϣ�Ĵ�����Ϊ�ܹ��������������У���������֮��ִ�У�
// �����ض�Ŀ�ģ���Ӧ���޸ĺ�������Ϣ����ķ���ֵ���÷���ֵ�����npMsgָ�����Ϣ������;���������践��ֵ
template<class CClassDeriveTo,const wchar_t* Name>
void CXuiElement<CClassDeriveTo,Name>::AfterParseMessage(IEinkuiMessage* npMsg)
{
	// ִ��ĳЩ����

	// ��󣬵��û����ͬ������
	// CXuiElement::AfterParseMessage(npMsg);	// ע�⣺һ��Ҫ��������ֱ�ӻ���
}

//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
template<class CClassDeriveTo,const wchar_t* Name>
ERESULT CXuiElement<CClassDeriveTo,Name>::OnElementCreate(IEinkuiIterator* npIterator)
{
	ERESULT lResult = ERESULT_SUCCESS;
	wchar_t lswTip[128];
	ICfKey* lpTipKey;

	if(mpTemplete != NULL && (lpTipKey = mpTemplete->GetSubKey(L"ToolTip"))!=NULL)
	{
		if(lpTipKey->GetValueLength()>0)
		{

			lpTipKey->GetValue(lswTip,sizeof(wchar_t)*128);

			mpIterator->SetToolTip(lswTip);
		}

		lpTipKey->Release();
	}

	return lResult;
}

// ����Ԫ�أ�����֪ͨһ��Ԫ�����٣�ע�⣺��Ԫ�������յ�����Ϣ��Ӧ�ü�ʱ����Ԫ�ع�������UnregisterElement������
// �Ӷ�����Ԫ�ع�������������һ��Ԫ�ط���������Ϣ�������ٽ��Լ���Ԫ�ع�����ע���������ͷ��������
template<class CClassDeriveTo,const wchar_t* Name>
ERESULT CXuiElement<CClassDeriveTo,Name>::OnElementDestroy()
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		CMM_SAFE_RELEASE(mpTemplete);
		//CMM_SAFE_RELEASE(mpIterator);
		CMM_SAFE_RELEASE(mpBgBitmap);

		lResult = ERESULT_SUCCESS;
	}while(false);

	return lResult;
}

//��ť�����¼�
template<class CClassDeriveTo,const wchar_t* Name>
ERESULT CXuiElement<CClassDeriveTo,Name>::OnCtlButtonClick(IEinkuiIterator* npSender)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		lResult = ERESULT_SUCCESS;
	}while(false);

	return lResult;
}

//Ԫ��λ�ñ��ı�,�Ѿ��ı����
template<class CClassDeriveTo,const wchar_t* Name>
ERESULT CXuiElement<CClassDeriveTo,Name>::OnElementMoved(D2D1_POINT_2F nNewPoint)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{

		lResult = ERESULT_SUCCESS;

	}while(false);

	return lResult;
}

//Ԫ�زο��ߴ緢���仯
template<class CClassDeriveTo,const wchar_t* Name>
ERESULT CXuiElement<CClassDeriveTo,Name>::OnElementResized(D2D1_SIZE_F nNewSize)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{

		lResult = ERESULT_SUCCESS;

	}while(false);

	return lResult;
}

//Ԫ����ʾ������
template<class CClassDeriveTo,const wchar_t* Name>
ERESULT CXuiElement<CClassDeriveTo,Name>::OnElementShow(bool nbIsShow)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		lResult = ERESULT_SUCCESS;
	}while(false);

	return lResult;
}

//���û����
template<class CClassDeriveTo,const wchar_t* Name>
ERESULT CXuiElement<CClassDeriveTo,Name>::OnElementEnable(bool nbIsShow)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		lResult = ERESULT_SUCCESS;
	}while(false);

	return lResult;
}

//��ʱ��
template<class CClassDeriveTo,const wchar_t* Name>
void CXuiElement<CClassDeriveTo,Name>::OnTimer(
	PSTEMS_TIMER npStatus
	)
{
}

// ��ˢ��
template<class CClassDeriveTo,const wchar_t* Name>
void  CXuiElement<CClassDeriveTo,Name>::OnLazyUpdate(
	PSTEMG_LAZY_UPDATE npLazyUpdate
	)
{
}


//����ת����Hook����Ϣ
template<class CClassDeriveTo,const wchar_t* Name>
ERESULT CXuiElement<CClassDeriveTo,Name>::OnHookMessage(IEinkuiIterator* npOriginalElement,	// ԭʼ����Ϣ����Ԫ��
	IEinkuiMessage* npOriginalMessage		// ���������Ϣ
	)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{

		lResult = ERESULT_SUCCESS;
	}while(false);

	return lResult;
}

//������Ϣ
template<class CClassDeriveTo,const wchar_t* Name>
ERESULT CXuiElement<CClassDeriveTo,Name>::OnResponseMessage(IEinkuiIterator* npOriginalElement,	// ԭʼ��Ϣ�Ľ���Ԫ��
	IEinkuiMessage* npOriginalMessage,	// ����������Ϣ
	void* npContext					// ������Ϣ����ʱ��ָ����������
	)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		lResult = ERESULT_SUCCESS;
	}while(false);

	return lResult;
}

//��갴��
template<class CClassDeriveTo,const wchar_t* Name>
ERESULT CXuiElement<CClassDeriveTo,Name>::OnMousePressed(const STEMS_MOUSE_BUTTON* npInfo)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		lResult = ERESULT_SUCCESS;
	}while(false);

	return lResult;
}

//���˫��
template<class CClassDeriveTo,const wchar_t* Name>
ERESULT CXuiElement<CClassDeriveTo,Name>::OnMouseDbClick(const STEMS_MOUSE_BUTTON* npInfo)
{
	return ERESULT_SUCCESS;
}

//����ƶ�
template<class CClassDeriveTo,const wchar_t* Name>
ERESULT CXuiElement<CClassDeriveTo,Name>::OnMouseMoving(const STEMS_MOUSE_MOVING* npInfo)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		lResult = ERESULT_SUCCESS;
	}while(false);

	return lResult;
}

//������/�뿪
template<class CClassDeriveTo,const wchar_t* Name>
void CXuiElement<CClassDeriveTo,Name>::OnMouseFocus(PSTEMS_STATE_CHANGE npState)
{
	if(mhInnerCursor == NULL)
		return;

	if (npState->State != 0)
	{
		//������
		mhPreviousCursor = CExWinPromptBox::SetCursor(mhInnerCursor);
	}
	else
	if(mhPreviousCursor != NULL && mhPreviousCursor != LoadCursor(NULL,IDC_WAIT)) //���������æµ��־
	{
		//����Ƴ�
		CExWinPromptBox::SetCursor(mhPreviousCursor);
		mhPreviousCursor = NULL;
	}
}

//���̽����û���ʧȥ
template<class CClassDeriveTo,const wchar_t* Name>
void CXuiElement<CClassDeriveTo,Name>::OnKeyBoardFocus(PSTEMS_STATE_CHANGE npState)
{
}

//�����ͣ
template<class CClassDeriveTo,const wchar_t* Name>
ERESULT CXuiElement<CClassDeriveTo,Name>::OnMouseHover()
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		lResult = ERESULT_SUCCESS;
	}while(false);

	return lResult;
}

//��껬��
template<class CClassDeriveTo,const wchar_t* Name>
ERESULT CXuiElement<CClassDeriveTo,Name>::OnMouseWheel(const STEMS_MOUSE_WHEEL* npInfo)
{
	return ERESULT_UNEXPECTED_MESSAGE;
}

//������Ϣ
template<class CClassDeriveTo,const wchar_t* Name>
ERESULT CXuiElement<CClassDeriveTo,Name>::OnKeyPressed(const STEMS_KEY_PRESSED* npInfo)
{
	return ERESULT_KEY_UNEXPECTED;
}

//��ݼ���Ϣ
template<class CClassDeriveTo,const wchar_t* Name>
ERESULT CXuiElement<CClassDeriveTo,Name>::OnHotKey(const STEMS_HOTKEY* npHotKey)
{
	return ERESULT_KEY_UNEXPECTED;
}


//������Ϣ
template<class CClassDeriveTo,const wchar_t* Name>
ERESULT CXuiElement<CClassDeriveTo,Name>::OnPaint(IEinkuiPaintBoard* npPaintBoard)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		BREAK_ON_NULL(npPaintBoard);

		if(mpBgBitmap != NULL)
			npPaintBoard->DrawBitmap(D2D1::RectF(0,0,(FLOAT)mpBgBitmap->GetWidth(),(FLOAT)mpBgBitmap->GetHeight()),
			mpBgBitmap,
			ESPB_DRAWBMP_LINEAR);

		lResult = ERESULT_SUCCESS;

	}while(false);

	return lResult;
}

//Ԫ����ק
template<class CClassDeriveTo,const wchar_t* Name>
ERESULT CXuiElement<CClassDeriveTo,Name>::OnDragging(const STMS_DRAGGING_ELE* npInfo)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	return lResult;
}

//��ק��ʼ,nulActKey�ĸ���갴ť���½�����ק
template<class CClassDeriveTo,const wchar_t* Name>
ERESULT CXuiElement<CClassDeriveTo,Name>::OnDragBegin(const STMS_DRAGGING_ELE* npInfo)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	return lResult;
}

//��ק����,nulActKey�ĸ���갴ť���½�����ק
template<class CClassDeriveTo,const wchar_t* Name>
ERESULT CXuiElement<CClassDeriveTo,Name>::OnDragEnd(const STMS_DRAGGING_ELE* npInfo)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	return lResult;
}

//����
template<class CClassDeriveTo,const wchar_t* Name>
ERESULT CXuiElement<CClassDeriveTo,Name>::OnCommand(const nes_command::ESCOMMAND neCmd)
{
	return ERESULT_UNSUCCESSFUL;
}

//�����Լ����е���Ԫ��
template<class CClassDeriveTo,const wchar_t* Name>
ERESULT CXuiElement<CClassDeriveTo,Name>::LoadSubElement()
{
	ERESULT leResult = ERESULT_SUCCESS;	

	ICfKey* lpICfKey = NULL;

	do 
	{
		BREAK_ON_NULL(mpTemplete);

		lpICfKey = mpTemplete->OpenKey(L"Children"); //����Ԫ�ؼ�
		BREAK_ON_NULL(lpICfKey);

		lpICfKey = lpICfKey->MoveToSubKey();	
		while (lpICfKey != NULL)	//ѭ������������Ԫ��
		{
			EinkuiGetSystem()->GetAllocator()->CreateElement(mpIterator,lpICfKey);
			lpICfKey = lpICfKey->MoveToNextKey();	//����һ����Ԫ�ؼ�
		}

	} while (false);

	return leResult;
}

// ��������
template<class CClassDeriveTo,const wchar_t* Name>
ERESULT CXuiElement<CClassDeriveTo,Name>::OnMouseOwnerTest(const D2D1_POINT_2F& rPoint)
{
	ERESULT luResult = ERESULT_SUCCESS;

	if(mpBgBitmap != NULL && !(rPoint.x < 0.0f || (ULONG)rPoint.x >= mpBgBitmap->GetWidth() || rPoint.y < 0.0f || (ULONG)rPoint.y >= mpBgBitmap->GetHeight()))
	{
		DWORD luPixel;
		if(ERESULT_SUCCEEDED(mpBgBitmap->GetPixel((ULONG)CExFloat::Round(rPoint.x),(ULONG)CExFloat::Round(rPoint.y),luPixel)))
		{
			if(luPixel == 1)
				luResult = ERESULT_MOUSE_OWNERSHIP;
		}
	}

	return luResult;
}

////���Լ������ڷ��ͼ���Ϣ
//template<class CClassDeriveTo,const wchar_t* Name>
//ERESULT __stdcall CXuiElement<CClassDeriveTo,Name>::PostMessageToParent(
//	IN ULONG nuMsgID,		// ��Ϣ����
//	IN void* npInputBuffer,	// �������ݵĻ�����
//	IN int niInputSize		// �������ݵĴ�С
//	)
//{
//	ERESULT luResult = ERESULT_UNSUCCESSFUL;
//	IEinkuiMessage* lpMsgIntf = NULL;
//
//	do 
//	{
//		//������Ϣ�ṹ��
//		lpMsgIntf = EinkuiGetSystem()->GetElementManager()->AllocateMessage(nuMsgID,npInputBuffer,niInputSize,NULL,0);
//		BREAK_ON_NULL(lpMsgIntf);
//
//		mpIterator->PostMessageToParent(lpMsgIntf);
//
//	} while (false);
//
//	CMM_SAFE_RELEASE(lpMsgIntf);
//
//	return luResult;
//}

// Postģʽ������Ϣ������ֱ�ӷ���Я���Ĳ�����ע�⣬���������һ��ָ�룬��ֻ�Ḵ�ƴ���ָ�루һ����ַ�����������Ḵ�ƴ���ָ��ָ������ݣ�Post������Ϣ��Ҫע��������⣬��ֹ��ַ����Ϣ�����߳��޷����ʡ�
// ���û�����ݷ��ͣ�rDataToPost��дCExMessage::DataInvalid;
template<class CClassDeriveTo,const wchar_t* Name>
template<typename DataType>
ERESULT CXuiElement<CClassDeriveTo,Name>::PostMessageToParent(ULONG nuMsgID,const DataType& rDataToPost)
{
	return CExMessage::PostMessage(mpIterator->GetParent(),mpIterator,nuMsgID,rDataToPost,EMSG_POSTTYPE_FAST);
}

// Sendģʽ������Ϣ������ֱ�ӷ���Я���Ĳ�����ע�⣬���������һ��ָ�룬��ֻ�Ḵ�ƴ���ָ�루һ����ַ�����������Ḵ�ƴ���ָ��ָ������ݣ�Sendģʽ������Ϣʱ���ⲻ�ᵼ����������
// ���û�����ݷ��ͣ�rDataToPost��дCExMessage::DataInvalid
template<class CClassDeriveTo,const wchar_t* Name>
template<typename DataType>
ERESULT CXuiElement<CClassDeriveTo,Name>::SendMessageToParent(ULONG nuMsgID,const DataType& rDateForSend,void* npBufferForReceive,int niBytesOfBuffer)
{
	return CExMessage::SendMessage(mpIterator->GetParent(),mpIterator,nuMsgID,rDateForSend,npBufferForReceive,niBytesOfBuffer);
}































#endif//_ELEMENTIMP_H_
