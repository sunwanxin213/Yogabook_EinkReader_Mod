/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#ifndef _CMMBASEOBJ_H_
#define _CMMBASEOBJ_H_
/*
	FileName: cmmBaseObject.h
	Author: Ax
	Purpose: �����ʵ��������Ƶ�C++����Լ���͹����ر��Ƕ���������ڿ��ƣ�
	More words; ���ļ����漰�϶��ģ���������Ӻ�C���Ժ�������÷�����������ʵע�ͣ��������߱��˽������ú͸�ϰ��Ҳ��������ѧ�ߺܺõĲο����ϡ�
	Revision History:
		Ax Dec.01,2010 Created
		Ax Mar.16,2011 Corrected the allocation and freeing of objects Derived from 'cmmBaseObject'
*/

// �����ӿ�
__interface IBaseObject{
public:
	// �������ã��������Ӻ��������
	virtual int __stdcall AddRefer(void)=NULL;
	// �ͷŶ��󣬷����ͷź��������
	virtual int __stdcall Release(void)=NULL;
	// ��ö�������
	virtual const wchar_t* __stdcall GetObjectName(void)=NULL;
	// ��̬����ȷ�ϣ�nszBuiltInName����ֱ�Ӵ����ַ���������ʹ��GET_BUILTIN_NAME(XXX)����д�����е�XXXΪ����ʵ������������
	virtual bool IsKindOf(const wchar_t* nszBuiltInName)=NULL;
};


// �����ӿ�ʵ�ָ���ģ�壻ע�����ģ���еĲ���CInterfaceDerivedFromӦ����һ������֮IBaseObject���������������Ĳ���cmmBaseObjectģ���������ࣻ
// �����Ҫ����һ��������cmmBaseObject�������࣬�����ظ�ʹ�ñ�ģ�壬���ǵ�������Ҫ֧������ʶ����ʹ������ĺ�DEFINE_DERIVED_TYPECAST�������������������ϸ�Ľ���
// ������Ϣ���ο�ʾ������
template<class CClassDeriveTo,class CInterfaceDerivedFrom,const wchar_t* Name>
class cmmBaseObject:public CInterfaceDerivedFrom{
protected:
	// ��ʼ������������ʵ�ָ��ֲ�ͬ�����ĳ�ʼ��������ע�⣬����������InitOnCreate������һ��Ҫ���û����InitOnCreate����
	// ����0��ʾ�ɹ�������ֵ���λΪ1��ʾ�������ش���Ӧ����ֹ��ʼ�����̣����صľ��Ǵ����룻��������ֵ��ʾ�����Ǵ��󷵻���
	ULONG InitOnCreate(void){
		return 0;	// ��������ײ�Ļ��࣬��ʵ�ʲ���
	}

public:
	// ���캯�������ڸ��ڲ����������ֵ�����ڸ���Ľ�һ���������磺��Դ���룬�������̵ȶ�Ӧ��ͨ������InitOnCreate����ʵ��
	// ��Ҫʹ�������Ĵ������Ĺ��캯��
	cmmBaseObject(){
		miReferenceCount=1;
		muFlags = 0;
	}
	virtual ~cmmBaseObject(){}

	void Reuse(){
		miReferenceCount = 1;
		muFlags = (muFlags&1);
	}


	/*/ ���ע�͵Ĵ��룬ʵ���˷ֽ���ͨnew ��������Ĺ��̣��������ڴ����ʾ���ù��캯�������漰��placement new�ĵ��ã�
	// ������������ȷ�ģ�ע�͵���ԭ�����ɸ��򵥵�д����������û��ע�͵ĵ���
	//// �������󣬾����÷��ο����ļ�ͷ����˵����Ҳ����ʵ����Ҫ�����Ĵ������ߺ�����ʹ������ĺ�Ϳ�������
	static CClassDeriveTo* CreateInstance(void){
		// ���������ǵ���C++��Ĭ�ϵ�operator new�����������ڴ棬ע�⣬C++Ĭ�ϵ�operator new��new operator�����𣬿��Բο�C++ primer �Լ�More effective C++����ؽ���
		// �򵥵�˵���� operator new ���������ڴ�������ù��캯������new operator��Ҳ��������ͨд�� new ClassA(xx);)�ڲ���ȥ����ǰ��ġ�operator new��������ö�Ӧ�Ĺ��캯����
		// �������������new�⣬C++����һ�ֽ���placement new������������Ƿ��亯������Ĭ������ɶ�һ�������ڴ�ĳ�ʼ����Ҳ���ǵ��ö�Ӧ��Ĺ��캯������Ȼ��Ҳ���Ը�д����ʵ�ַ���
		// �ڱ�ģ�����о����������plancement new������void* __cdecl operator new(size_t nSize,void* npObj)��������ֱ�ӷ����˳����npObj�������Ƿ���C++�����ʵ��
		CClassDeriveTo* lpObj = (CClassDeriveTo*)operator new(sizeof(CClassDeriveTo),false);
		if(lpObj != NULL)
		{
			// ����placement new�Ӷ��������캯������Release building������£�������ȥ��������ʵ�ֵ�������void* __cdecl operator new(size_t nSize,void* npObj)����ֱ�ӵ��ù��캯��
			// ����ִ�������Ĺ��캯������
			new (lpObj)CClassDeriveTo();
			// ���ñ�־��ʾ���������ڴ���Ҫ���ͷ�
			lpObj->SetDeleteRequired();
		}
		// ���ع�����ϵĶ���
		return lpObj;
	}
	// ��Ϊ�����placement new���ṩ��Ĭ�Ϲ��캯����Release Building�����κδ���
	void* __cdecl operator new(size_t nSize,void* npObj){
		return npObj;
	}
	// ��ֹ���������־��棬ʵ������һ��������Ե����������������������
	void __cdecl operator delete(void* nObj,void* npObj){
	}*/

	// �������󣬾����÷��ο����ļ�ͷ����˵����Ҳ����ʵ����Ҫ�����Ľ��칤�ߺ�����ʹ������ĺ�DEFINE_CUMSTOMIZE_CREATE�Ϳ�������
	// ���ʵ�ֵĴ�����ʹ��DEFINE_CUMSTOMIZE_CREATE(CClassDeriveTo,(),())��һ���ģ�����޸�������ط��Ĵ���!!!�����!!!��DEFINE_CUMSTOMIZE_CREATE���ȼ��޸�
	static CClassDeriveTo* CreateInstance(){
		// ����C++Ĭ�ϵ�new operator������operator new�����������ȵ����������ص�void* __cdecl operator new(size_t nSize,bool nbDummy)�����ã������ȥ����Ĭ�ϵĹ��캯��
		CClassDeriveTo* lpObj = new (false)CClassDeriveTo;
		if(lpObj != NULL)
		{
			// ���ñ�־��ʾ���������ڴ���Ҫ���ͷ�
			lpObj->SetDeleteRequired();
			// ���ó�ʼ����������鷵��ֵ�����ʧ�ܣ��ͷŵ��ö���
			if((lpObj->InitOnCreate()&0x80000000)!=0)
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
	// ��ģ��ĵ�һ������������Բ����ñ�ģ��ʵ������CompleteCreation��������Ϊ��û������
	void CompleteCreation(void){
	}

	// ��������new��delete��Ŀ���ǽ���Ĭ�ϵ�new��delete������ʹ�ñ�ģ����ʵ�ֵ������࣬�ڶ�̬����ʱ������ʹ�ó�Ա����CreateInstance;
	// ��ģ����Ŀͻ�����!!!���Բ�Ҫ!!!ʹ��C++�� new������������Ȼ����ͨ����new (false)ClassA���������󣬵������ᵼ��Release������©�ڴ��ͷţ�
	// ���ĳ����������Ҫ�Զ����ڴ����ķ������������������������
	void* __cdecl operator new(size_t nSize,bool nbDummy){
		return ::operator new(nSize);
	}
	// ʵ������һ��������Ե�ɾ������������������
	void __cdecl operator delete(void* npObj,bool nbDummy){
		// ֱ�ӵ���C++Ĭ����������
		::operator delete(npObj);
	}
	// �ṩĬ��ɾ������
	void __cdecl operator delete(void* npObj){
		// ֱ�ӵ���C++Ĭ����������
		::operator delete(npObj);
	}

	// �������ã�������Ƕ�̬�����Ķ��󣨼�Ƕ���Ķ����е��Ӷ��󣬻��߳���ջ�ϵľֲ��������󣩣����ܹ�ͨ��AddRef�ı������ڣ����Ƽ��������ݵ�������֮�������ȥ����Ϊ���������ڽ��������������ھ�����
	virtual int __stdcall AddRefer(void){
		return InterlockedIncrement(&miReferenceCount);
	}

	// �ͷŶ���
	virtual int __stdcall Release(void){
		int liReval = InterlockedDecrement(&miReferenceCount);

		if(liReval == 0 && (muFlags&1)!=0)
		{
			delete this;
			// ����Ĵ�����Է�����ִ�ж����ͷ�
			////// ������������
			//((CClassDeriveTo*)this)->~CClassDeriveTo();
			////// ϵͳĬ���ڴ��ͷź����������ٵ�������������
			//operator delete(this,false);
		}
		return liReval;
	}

	// ��ö�������
	virtual const wchar_t* __stdcall GetObjectName(void){
		return Name;
	}

	// ��̬����ȷ�ϣ�nszBuiltInName����ֱ�Ӵ����ַ���������ʹ��GET_BUILTIN_NAME(XXX)����д�����е�XXXΪ����ʵ������������
	virtual bool IsKindOf(const wchar_t* nszBuiltInName){
		return nszBuiltInName==Name;
	}

	// ����Ϊ��Ҫ�ͷŵ��ڴ�����
	void SetDeleteRequired(void){
		muFlags |= 1;
	}

	// ����λ��־������������ṩ������������ⲿ���õģ�������Ҳ������������������ṩ���Լ�����������û�ʹ��
 	bool SetFlags(
 		int niIndex,		// ��־����ţ���0��ʼ���������������������������Ҹ���������2����ϣ������������û��޸ĵı�־����ô���ĺ�������ʱ��niIndex=0��ʾ�������Ļ����2
 		bool nbSet=true		// ���û��������־
 		) {
		niIndex+=1;

		if(niIndex > 31)
			return false;

		if(nbSet != false)
			muFlags |= (1<<niIndex);
		else
			muFlags &= (~(1<<niIndex));

		return true;
		// ���湩���������زο����������������Ҫ2��λ��־
		// return ::SetFlags(niIndex+2,nbSet);
	}

	// ��ȡ��־
	bool TestFlag(int niIndex){
		niIndex+=1;
		if(niIndex > 31)
			return false;

		return (muFlags & (1<<niIndex))!=0;
		// ���湩���������زο����������������Ҫ2��λ��־
		// return ::TestFlag(niIndex+2,nbSet);
	}

protected:
	LONG miReferenceCount;
private:
	ULONG muFlags;	// ���λ��ʾ�������Ƿ�Ϊnew���䣬�ε�2λ��ʾ���ù��캯����ʱ�򣬹��캯��ִ�е�ִ�н��

};

#define DECLARE_BUILTIN_NAME(_X) extern const wchar_t glszBN##_X[];
#define DEFINE_BUILTIN_NAME(_X) extern const wchar_t glszBN##_X[]=L#_X;
#define GET_BUILTIN_NAME(_X) glszBN##_X

// ����ĺꡮDEFINE_CUMSTOMIZE_CREATE������ʵ���Զ���Ĵ������ߺ������÷��磺DEFINE_CUMSTOMIZE_CREATE(CClassX,(int niA,void* npB),(niA,npB))����ο��������
#define DEFINE_CUMSTOMIZE_CREATE(_X,_Y,_Z)	static _X* CreateInstance##_Y{_X* p = new  (false)##_X();if(p!=NULL)\
{p->SetDeleteRequired();if((p->InitOnCreate##_Z&0x80000000)!=0){p->Release();p=NULL;}else p->CompleteCreation();}return p;}
	// ���ϣ�����ĺ�����ʵ��һ��static������C++�Ĺ�����������ֻҪ�����������ͬ���ķ����⺯�������������ͬ�������⺯�������ᱩ¶���ⲿ����������������
	// �����������ĺ�ʵ����һ���Լ��ض���CreateInstance(�������)����ô�����������Ҳϣ�������û�ʹ��ԭʼ��CreateInstance(void)���ǣ����ڴ����м���DEFINE_CUMSTOMIZE_CREATE(��������,(bool nbTest),(nbTest))

	// ����д����ĺ��ǰһ�죬�ҷ��˸�С�����Ҵ������ͼʹ��##_XΪ�˽�����_Xչ��Ϊԭ�ģ���ʵֱ��д_X����չ��ԭ�ģ�ʹ��##��Ŀ����ͬ�����ַ��໥�ν�ʱ�������ֺ�չ���ģ�
	// ��Ϊʹ��##_X���������������⣬��##_X�����ܸ��ڿո���棬Ϊ���ڿո�����_X�ַ�����ֻ�����ú�ڶ���չ���İ취
	//#define __CMM_CC1(_X)
	//#define __CMM_CC2(_X) __CMM_CC1(_X) 
	//#define DEFINE_CUMSTOMIZE_CREATE(_X,_Y,_Z)	static __CMM_CC2(_X)##_X* CreateInstance##_Y{##_X* p = new  (false)##_X##_Z;if(p!=NULL)\
	//{p->SetDeleteRequired();if(p->GetConstructorResult()==1){p->Release();p=NULL;}}return p;}
	// ����һ�����ú�����չ����дԤ�������ĺ�˼·����Ȼ�Ǵ��󣬵�ֵ�òο�

// ��������ʶ��꣬_X��д����������_Y��д���������������ʵ�ֵ�ԭ���ǣ�����������Ʋ�����������ʱ��������Ҫѯ�ʻ����Ƿ�����������
// �������������
#define DEFINE_DERIVED_TYPECAST(_X,_Y) virtual bool IsKindOf(const wchar_t* s){if(s!=GET_BUILTIN_NAME(_X))return _Y::IsKindOf(s);return true;}virtual const wchar_t* __stdcall GetObjectName(void){return GET_BUILTIN_NAME(_X);}
// չ�������ӵȼ���
#if 0	// �����������ʾ�����������
virtual bool IsKindOf(const wchar_t* nszBuiltInName){
	if(nszBuiltInName!=Name)
		return CClassDerivedFrom::IsKindOf(nszBuiltInName);
	return true;
}

// �ӻ����ӿ�ʵ�������������÷���
DECLARE_BUILTIN_NAME(CNewDerivedClass)
class CNewDerivedClass:public CExistBaseClass		// CExistBaseClass������һ����cmmBaseObject�����Ļ�����
{
public:
	//////////////////////////////////////////////////////////////////////////
	// ��ʵ�ֹ��캯��
	//////////////////////////////////////////////////////////////////////////
	CNewDerivedClass(){}

	//////////////////////////////////////////////////////////////////////////
	// ��ʵ�����غ���
	//////////////////////////////////////////////////////////////////////////
	~CNewDerivedClass(){}

	//////////////////////////////////////////////////////////////////////////
	// �����س�ʼ������
	//////////////////////////////////////////////////////////////////////////
	// �����Լ��ĳ�ʼ������
	ULONG InitOnCreate(){
		// Ҫ���û���ĳ�ʼ������
		CExistBaseClass::InitOnCreate();
		// �ڵ��û����ʼ�����ٵ����Լ���������ʼ������
	}
	// Ҳ���Դ��в��� 
	ULONG InitOnCreate(int niParam){
		// Ҳ�����ڵ��û����ʼ��ǰ���ȵ����Լ���ĳЩ��ʼ������
		...
		// Ҫ���û���ĳ�ʼ������
		CExistBaseClass::InitOnCreate(niParam);
		// �����ʼ��֮���ٵ����Լ���ʣ�µĳ�ʼ������
		...
	}
	//////////////////////////////////////////////////////////////////////////
	// һ��Ҫ���ش������ߺ���
	//////////////////////////////////////////////////////////////////////////
	// �����ǲ����е��ò�����
	DEFINE_CUMSTOMIZE_CREATE(CNewDerivedClass,(),())
	// Ҳ�����Ǵ��е��ò�����
	DEFINE_CUMSTOMIZE_CREATE(CNewDerivedClass,(int niParam),(niParam))

	//////////////////////////////////////////////////////////////////////////
	// һ��Ҫ������һ�У�������������ʶ��
	DEFINE_DERIVED_TYPECAST(CNewDerivedClass,CExistBaseClass)

};

#endif//��ʾ����



// �򵥻�����ʶ����������ƶԶ��̹߳������ݵķ��ʣ��ں�̬ʹ��SpinLock���û�̬ʹ��CriticalSection
class CExclusiveAccess{
public:
	void Enter(){
		InterlockedIncrement(&count);
#if defined(KERNEL_CODE)
		KeAcquireSpinLock((PKSPIN_LOCK)&AccessLock,(PKIRQL)&SavedIrql);
#elif defined(NATIVE_CODE)
		ZwWaitForSingleObject(mhEvent,FALSE,NULL);
#else
		EnterCriticalSection((LPCRITICAL_SECTION)&CriticalSection);
		//if(mhMutex == NULL)
		//	RaiseException(0x888888,0,0,0); // ���Բ��ܼ���ִ�У��ó������

		//WaitForSingleObject(mhMutex,INFINITE);
#endif//KERNEL_CODE
	}
	void Leave(){
		if (InterlockedDecrement(&count) >= 0)
		{
#if defined(KERNEL_CODE)
		KeReleaseSpinLock((PKSPIN_LOCK)&AccessLock,SavedIrql);
#elif defined(NATIVE_CODE)
		ZwSetEvent(mhEvent,NULL);
#else
		LeaveCriticalSection((LPCRITICAL_SECTION)&CriticalSection);
		//if(mhMutex != NULL)
		//	ReleaseMutex(mhMutex);
#endif//KERNEL_CODE
		}
	}

	CExclusiveAccess(){
		count = 0;
#if defined(KERNEL_CODE)
		KeInitializeSpinLock(&AccessLock);
		SavedIrql = PASSIVE_LEVEL;
#elif defined(NATIVE_CODE)
		if(ZwCreateEvent(&mhEvent,SYNCHRONIZE|DELETE|EVENT_MODIFY_STATE,NULL,SynchronizationEvent,TRUE)!=STATUS_SUCCESS)
			mhEvent = NULL; // ???��δ����

		#ifdef _DEBUG
		if(mhEvent == NULL)
			DbgBreakPoint();			
		#endif//_DEBUG
#else
		InitializeCriticalSection(&CriticalSection);
		//mhMutex = CreateMutex(NULL,FALSE,NULL);
		//Test1 = Test2 = 0x8866;
#endif//KERNEL_CODE
	}
	~CExclusiveAccess(){
#if defined(KERNEL_CODE)
#elif defined(NATIVE_CODE)
		if(mhEvent != NULL)
			ZwClose(mhEvent);
#else
		DeleteCriticalSection(&CriticalSection);
		//if(mhMutex != NULL)
		//	CloseHandle(mhMutex);
#endif
	}
protected:
	volatile LONG count;
#if defined(KERNEL_CODE)
	KSPIN_LOCK AccessLock;
	KIRQL SavedIrql;
#elif defined(NATIVE_CODE)
	HANDLE mhEvent;
#else
	CRITICAL_SECTION CriticalSection;
	//HANDLE mhMutex;
#endif//KERNEL_CODE
};


#ifdef FLOW_CONTROL_ASSISTANT
#error Micro redifinition
#else//FLOW_CONTROL_ASSISTANT
#define FLOW_CONTROL_ASSISTANT

#ifndef ERROR_SUCCESS
#define ERROR_SUCCESS STATUS_SUCCESS
#endif

#define BREAK_ON_NULL(_X) {if(_X==NULL) break;}
#define LEAVE_ON_NULL(_X) {if(_X==NULL) __leave;}
#define THROW_ON_NULL(_X) {if(_X==NULL) throw 1;}
#define THROW_ON_FALSE(_X) {if((_X)==false) throw 2;}
#define THROW_ON_FAILED(_X)	  {if((_X&0x80000000)!=0) throw 3;}
#define THROW_ON_INVALID(_X) {if(_X==INVALID_HANDLE_VALUE) throw 4;}
#define BREAK_NOT_SUCCESS(_X) {if((_X)!=ERROR_SUCCESS) break;}
#define LEAVE_NOT_SUCCESS(_X) {if((_X)!=ERROR_SUCCESS) __leave;}
#define BREAK_ON_FAILED(_X)	  {if((_X&0x80000000)!=0) break;}
#define LEAVE_ON_FAILED(_X)   {if((_X&0x80000000)!=0) __leave;}
#define BREAK_ON_FALSE(_X) {if((_X)==false) break;}
#define CMM_SAFE_RELEASE(_X) {if((_X)!=NULL){(_X)->Release();(_X)=NULL;}}
#define CMM_SAFE_DELETE(_X) {if((_X)!=NULL){delete (_X);(_X)=NULL;}}
#define CMM_SAFE_CLOSE_HANDLE(_X) {if((_X)!=NULL){CloseHandle((_X));(_X)=NULL;}}

#define RETURN_ON_FAILED(_X)  {if((_X&0x80000000)!=0) return (_X);}
#define RETURN_NOT_SUCCESS(_X) {if((_X)!=ERROR_SUCCESS) return (_X);}
#define RETURN_ON_NULL(_X,_Y) {if((_X)==NULL) return (_Y);}

#define THROW_NULL	(throw 1)
#define THROW_FALSE (throw 2)
#define THROW_OVERFLOW (throw 3)
#define THROW_INVALID (throw 4)
#define THROW_EMPTY	(throw 5)
#define THROW_PARAMTER (throw 6)
#define THROW_USER	(throw 7)
#define THROW_FAILED (throw 8)
#define THROW_WRONG_FORMAT (throw 9)
#define THROW_UNKNOWN (throw 100)
#endif//FLOW_CONTROL_ASSISTANT

#ifdef CMM_RELEASE
#error Micro redifinition
#else
#define CMM_RELEASE( x )  \
	if (NULL != x)      \
{                   \
	x->Release();   \
	x = NULL;       \
}
#endif//CMM_RELEASE

#ifndef SAFE_CLOSE_HANDLE
#undef SAFE_CLOSE_HANDLE
#define SAFE_CLOSE_HANDLE( x )  \
	if (NULL != x)      \
{                   \
	CloseHandle(x);   \
	x = NULL;       \
}
#endif

#ifndef CMMASSERT
#if defined(DBG)||defined(_DEBUG)	// >>>>>>>>>>>>>>>>>>>

#if defined(KERNEL_CODE)||defined(NATIVE_CODE)
#define CMMASSERT(_X) {if(!(_X))DbgBreakPoint();}
#else
#define CMMASSERT(_X) {if(!(_X))RaiseException(0x888888,0,0,0);}
#endif
#else
#define CMMASSERT

#endif		//  define(DBG) || define(_DEBUG) <<<<<<<<<<<<<<
#endif//CMMASSERT

class CmmTrace
{
public:
	static void Trace(TCHAR *format, ...) {
	#ifdef _DEBUG

		wchar_t buffer[1000];

		va_list argptr;

		va_start(argptr, format);

		wvsprintf(buffer, format, argptr);

		va_end(argptr);

		OutputDebugString(buffer);
	}
	#else
	}
	#endif
};



#endif//_CMMBASEOBJ_H_