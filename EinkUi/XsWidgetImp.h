/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#pragma once



//////////////////////////////////////////////////////////////////////////
// Widget����̬
DECLARE_BUILTIN_NAME(CXsWidget)
class CXsWidget: public cmmBaseObject<CXsWidget,IXsWidget,GET_BUILTIN_NAME(CXsWidget)>
{
	friend class CEinkuiSystem;
	friend class CXelManager;
public:
	// ��ȡ΢�����ڰ�װĿ¼�������к�׺��'\'
	virtual const wchar_t* __stdcall GetWidgetDefaultPath(void);

	// ��Widget��ģ���ļ�������ʵ�ִ�Widget��DLL���ƣ���"IdeaMain.dll"
	virtual const wchar_t* __stdcall GetModuleName(void);

	// ��ñ�΢��ʵ������ϵͳ����ʱ�����ĵ�һ��΢��������system��
	virtual const wchar_t* __stdcall GetInstanceName(void);

	// ���΢������Module�Ĺ����ӿ�
	virtual IElementFactory* __stdcall GetDefaultFactory(void);

	// ���΢����ʵ��ר�����ã�һ��΢��Module������һ��������ͬʱ���ж��ʵ�������ͬʱ��һ̨�����ϣ�ÿһ��Windows�û��ʻ��£�����������һ��Idealife���̣�
	//		������ҪΪ΢����ÿһ������̬ʵ���ṩһ��ר�����ã������Խ���Ҫ���ڱ������������Ϣ��ŵ����ר��������
	virtual ICfKey* __stdcall GetInstanceConfig(void);

	//��ȡ΢�����������ʱ�ļ���Ŀ¼,���صĵ�ַ������\\��β,ͬһ����΢���Ĳ�ͬʵ���õ����ǲ�ͬ���ļ���·��
	//npswPathΪ�����������nlBufferLenΪ���������ȣ�BY TCHAR
	virtual bool __stdcall GetInstanceTempPath(OUT wchar_t* npswPath,IN LONG nlBufferLen);

	// ���΢��ʵ����Home Page
	virtual IEinkuiIterator* __stdcall GetHomePage(void);

	// �ر�Widget
	virtual void __stdcall Close(void);

private:
	IElementFactory* mpFactory;
	IEinkuiIterator* mpHomePage;
	ICfKey*	mpInstanceConfig;
	wchar_t* mswModulePath;	// ������������ַ���mswName��mswInstance���û�����
	wchar_t* mswModuleName;
	wchar_t* mswInstance;
	bool mbValid;

	DEFINE_CUMSTOMIZE_CREATE(CXsWidget,(const wchar_t* nswModulePathName,const wchar_t* nswInstanceName,ICfKey* npInstanceConfig),(nswModulePathName, nswInstanceName,npInstanceConfig))

	ERESULT InitOnCreate(
		IN const wchar_t* nswModulePathName,	// ��Widget��ģ���ļ���·��������ʵ�ִ�Widget��DLL����
		IN const wchar_t* nswInstanceName,	// �������е�ʵ������ʵ����������ͬ�����������ͬ��ʵ������ϵͳ���᷵��ʧ��
		IN ICfKey* npInstanceConfig	// ������ʵ����ר������
		);

	void SetHomePage(IEinkuiIterator* npHomePage);

	void SetFactory(IElementFactory* npFactory);

	bool IsValid(void){
		return mbValid;
	}

	void SetInvalid(void){
		mbValid = false;
	}


	CXsWidget();
	~CXsWidget();

};











