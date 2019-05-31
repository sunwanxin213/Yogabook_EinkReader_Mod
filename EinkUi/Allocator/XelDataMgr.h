/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#ifndef _EOB_DATA_MGR_
#define _EOB_DATA_MGR_



// ������DLL����������ڴ��е�pair�ṹ��
class CLelFactoryNode{
public:
	HANDLE	mpDllHandle;
	wchar_t*	mswDllPath;
	IElementFactory*	pfnElementFact;
	ULONG muHash;
	__inline void operator=(class CLelFactoryNode& src) {
		mpDllHandle = src.mpDllHandle;
		mswDllPath = src.mswDllPath;
		src.mswDllPath = NULL;
		pfnElementFact = src.pfnElementFact;
		muHash = src.muHash;
	}
	CLelFactoryNode(){
		mpDllHandle = NULL;
		mswDllPath = NULL;
		pfnElementFact = NULL;
	}
	~CLelFactoryNode() {
		CMM_SAFE_DELETE(mswDllPath);
	}
	// ���ַ�������Hash��,���ص������ֵ���Ч����
	int GenerateHashCode(
		IN const wchar_t* nszName,
		IN int niNameLen	// -1 indicate that the name was terminated by '\0' or '\\' or '/'
		)
	{
		LONG liIndex = 0;
		muHash = 0;

		if(niNameLen < 0)
		{
			niNameLen = 255;
		}

		wchar_t lcCrt;
		while (nszName[liIndex] != 0 && liIndex < niNameLen)
		{
			lcCrt = nszName[liIndex++];
			if((lcCrt|0x20) >= L'a' && (lcCrt|0x20) <= L'z')
				lcCrt |= 0x20;
			muHash += lcCrt;
			muHash = (muHash<<1) | ((muHash&0x010000)>>16);
		}
		
		return liIndex;
	}
	bool SavePath(const wchar_t* nswDllPath){
		if(nswDllPath == NULL || nswDllPath[0]==UNICODE_NULL)
			return true;
		int liSize = GenerateHashCode(nswDllPath,-1)+1;
		mswDllPath = new wchar_t[liSize];
		if(mswDllPath == NULL)
			return false;

		RtlCopyMemory(mswDllPath,nswDllPath,liSize*sizeof(wchar_t));

		return true;
	}
	void FreePath(void)	// ��һ�����������������ɾ���󣬲��ܵ��ñ������ͷŵ�������ڴ�
	{
		CMM_SAFE_DELETE(mswDllPath);
	}
};

enum REGSTATUS{
	Invalid = 0,	// û�б�ע��
	LOAD = 1,		// �Ѿ�������
	UNLOAD = 2		// �������У�����DLL·��
};



// ��������̬�����ݣ�����ȫ��Ψһʵ����
// �����б�������ע����Ϣ

DECLARE_BUILTIN_NAME(CXelDataMgr)
class CXelDataMgr:public cmmBaseObject<CXelDataMgr, IBaseObject, GET_BUILTIN_NAME(CXelDataMgr)> 
{
public:
	CXelDataMgr();
	virtual ~CXelDataMgr();

	
public:
	// ��ȡΨһʵ�����Ķ���ӿ�
	static CXelDataMgr* SingleInstance(const wchar_t* nswRegPath=NULL);
	static CXelDataMgr* sm_Inst;

	// ��ʼ��
	ULONG InitOnCreate(const wchar_t* nswRegPath);
	DEFINE_CUMSTOMIZE_CREATE(CXelDataMgr,(const wchar_t* nswRegPath),(nswRegPath))
	
public:

	// ͨ��ָ�����������Ҷ�Ӧ��DLL
	// nswClsName�������������������
	// nswDllPath����������������ҵ�����DLLȫ·���������ַ�ָ��ָ��Ļ�����
	BOOL __stdcall FindFactoryDll(
		const wchar_t* nswClsName, 
		wchar_t* nswDllPath
		);
	
	// �޸ļ��ص�DLL�����
	// niMode���޸�ģʽ��niMode=1����ʾ��ӣ�niMode=0,��ʾɾ��
	BOOL __stdcall ModLoadedList(
		const wchar_t* nswDllPath,
		HANDLE nhDllHandle,
		IElementFactory* pfnElementFact,
		BOOL niMode
		);

	//// ע��һ��������
	//void RemoveFactory(
	//	IElementFactory* npElementFact
	//);

	// �鿴ָ����DLL�Ƿ��Ѿ�������
	IElementFactory* __stdcall HasFactoryBeenLoaded(
		const wchar_t* nswDllPath
		);


	// �鿴�Ƿ��Ѿ���ע��,���ݸ��������ͣ��鿴�Ƿ����һ����Ӧ�Ĺ����ࡣ�����߸���ENUM����ֵ�ֱ�ȡ����������ֵ
	// �������ȼ���Ƿ��Ѿ���ע�ᣬȻ���ѯ��������Ƿ��Ѿ����ء�����������ڣ���֤��û�б�ע��
	// nswClsName������
	// nswDllPath������������DLL��·��
	// npElementFact������������������ӿ�
	REGSTATUS __stdcall IsRegisted(
		const wchar_t* nswClsName, 
		wchar_t* nswDllPath,
		IElementFactory** npElementFact
		);

	// ��ȡ������ע����Ϣ������KEY
	HKEY GetMainKey();	


	// ����ά���������������б�
protected:
	// {�����࣬������}ӳ���
//	cmmVector<FACTCLASSPAIR> mFactClassPairVec;
	cmmVector<CLelFactoryNode> moFactories;
	HKEY	mpMainKey;

private:
	BOOL mb64Bit;

};



#endif