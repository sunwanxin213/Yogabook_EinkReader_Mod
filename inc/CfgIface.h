/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */
#ifndef _CONFIGINTF_H_
#define _CONFIGINTF_H_

#include "cmmBaseObj.h"

#ifndef IN
#define IN
#endif
#ifndef OUT
#define OUT
#endif



__interface ICfKey;
//////////////////////////////////////////////////////////////////////////
// �����ļ��ӿ�
//	��������ʵ��һ�������ļ�������������������ļ��������µ����ԣ�
//		1. ��ŵ�������Key���ɣ�ÿ��Key����һ�����ֺ�һ��Ĭ��ֵ��Ĭ��ֵ����������Ϊ�ַ������߶��������ݣ�2.�����νṹ����֯ȫ����Key�����и�Root��ϵͳĬ�ϵ�Key���������е�Key����������Key��������Key����Key��
//		3. ����Key��������ɢ��ֵ�����ṩ�������ܹ��ܿ����Ѱ�ʹ���Key���ʺϴ洢��������Key��
__interface IConfigFile:public IBaseObject{
public:
	// ��ø�������õĶ��󵱲��ٷ���ʱ��ҪRelease
	virtual ICfKey* __stdcall GetRootKey(void)=NULL;

	// ��ָ���ļ�����õĶ��󵱲��ٷ���ʱ��ҪRelease
	virtual ICfKey* __stdcall OpenKey(
		IN const wchar_t* nszKeyName,		// ��'/'�ָ�Ӽ�������������ָ�����磺xx/yy/zz
		IN bool nbCreateIf=false			// �������������ڣ�������
		)=NULL;

	// �����޸ĵ��ļ�
	virtual bool __stdcall SaveFile(
		IN const wchar_t* nszOtherFile=NULL	// ����ָ������Ϊ�������ļ������ΪNULL���Ᵽ�浽�ղŴ򿪻����½����ļ�
		)=NULL;


	enum VALUETYPE{
		Invalid=0,	// ����Ч����
		Binary=1,	// ����������
		AnsiString=2,	// Ansi�����ַ���
		UnicodeString=3	// Unicode�����ַ���
	};
};



//////////////////////////////////////////////////////////////////////////
// ��ֵ��
__interface ICfKey:public IBaseObject{
public:
	//ɾ���������ע�⣬����ɾ����Ľӿ���Ȼ��ҪRelease�����ù�Delete������ӿ�Ҳ��û�����ˣ���Release�⣬�������ܶ���֧����
	virtual bool __stdcall Delete(void)=NULL;	

	// �½�һ����������NULL��ʾʧ�ܣ�ʧ�ܵ�ԭ��������Ѿ�������ͬ�ļ�ֵ�����߷����ڴ�ʧ��
	virtual ICfKey* __stdcall NewSubKey(
		IN const wchar_t* nszName,	// �Ӽ������֣�����Ϊ�գ������в���ʹ������4���ַ�" < > / \ " ����������
		IN IConfigFile::VALUETYPE nuValueType = IConfigFile::Invalid,	// �Ӽ���Ĭ��ֵ���ͣ����ΪInvalid������Ժ���������
		IN const void* npValueBuf = NULL,
		IN LONG  niValuelen = 0		// �ַ�������ʱ������ͨ����β��\0��������ֵ�ĳ��ȣ���ʱ����Ϊ-1
		)=NULL;

	// �½�һ��������������NULL��ʾʧ�ܣ����ͬʱ����������ֵ�����п��ܴ���������ֵ�����������м�����
	virtual ICfKey* __stdcall NewSubKey(
		IN ULONG nuID,		// ָ���������ı�ʶID�������ܲ�Ҫʹ����ͬ��ʶ������Ϊ0
		IN bool nbAhead=false,	// ��������ͬID���Ӽ�ʱ������ͬID�Ӽ�֮ǰ�����¼�����������ͬID�Ӽ�֮�����¼�
		IN IConfigFile::VALUETYPE nuValueType = IConfigFile::Invalid,	// �Ӽ���Ĭ��ֵ���ͣ����ΪInvalid������Ժ���������
		IN const void* npValueBuf = NULL,
		IN LONG  niValuelen = 0		// �ַ�������ʱ������ͨ����β��\0��������ֵ�ĳ��ȣ���ʱ����Ϊ-1
		)=NULL;

	// �򿪶��·��ָ�����Ӽ�����õĶ��󵱲��ٷ���ʱ��ҪRelease��������ͬ�����GetSubKey�����ǣ����������Դ�һ��·���磺xx/yy/zzָ�������²��zz�Ӽ�����GetSubKeyֻ��򿪵�һ���xx�Ӽ�
	virtual ICfKey* __stdcall OpenKey(
		IN const wchar_t* nszKeyName,		// ��'/'�ָ�Ӽ�������������ָ�����磺xx/yy/zz
		IN bool nbCreateIf=false			// �������������ڣ�������
		)=NULL;

	// ��ñ����ĸ�������������Ǹ������򷵻�NULL,��õĶ��󵱲��ٷ���ʱ��ҪRelease
	virtual ICfKey* __stdcall GetParentsKey(void)=NULL;

	// �������ø��ڵ㣬�����ڵ�ӵ�ǰ���ڵ��Ƴ��������µĸ��ڵ���
	virtual bool __stdcall SetParentKey(
		IN ICfKey* npNewParent,
		IN bool nbAhead
		)=NULL;

	// �����һ��������õĶ��󵱲��ٷ���ʱ��ҪRelease
	virtual ICfKey* __stdcall GetNextKey(void)=NULL;

	// ��õ�һ���Ӽ�����õĶ��󵱲��ٷ���ʱ��ҪRelease
	virtual ICfKey* __stdcall GetSubKey(void)=NULL;

	// ����Ӽ�����õĶ��󵱲��ٷ���ʱ��ҪRelease��������ͬ�����OpenKey�����ǣ�OpenKey���Դ�һ��·���磺xx/yy/zzָ�������²��zz�Ӽ�����GetSubKeyֻ��򿪵�һ���xx�Ӽ�
	virtual ICfKey* __stdcall GetSubKey(
		IN const wchar_t* nszName,	// ��������ȥ��ȡ�Ӽ�
		IN int niNameLen=-1,	// -1 indicate nszName is terminated by '\0' or '\\' or '/', >=0 is the charactar count of nszName
		OUT int* npNameLen=NULL,	// �������ֵ���Ч����
		IN bool nbCreateIf=false	// ����ü������ڣ�������
		)=NULL;

	// ����Ӽ�����õĶ��󵱲��ٷ���ʱ��ҪRelease��������ͬ�����GetSubKey�����ǣ�����İ����ִ򿪼�ֵ����������ֱ�Ӱ�ID�򿪣�ֻ��ͨ�����������ܴ�û�����ֵļ�ֵ
	virtual ICfKey* __stdcall GetSubKey(
		IN ULONG nuID,		// �Ӽ��ı�ʶID
		IN int niPos=0		// ����ͬ��ID�ļ�ֵ�е��Ⱥ�λ�ã�< 0��ʾȡ���һ��
		)=NULL;

	// ��ø����������ͷŵ�ǰ��!!!����õĶ��󵱲��ٷ���ʱ��ҪRelease
	virtual ICfKey* __stdcall MoveToParentsKey(void)=NULL;

	// �����һ�����������ͷŵ�ǰ��!!!����õĶ��󵱲��ٷ���ʱ��ҪRelease
	virtual ICfKey* __stdcall MoveToNextKey(void)=NULL;

	// �����һ������ɾ�����ͷŵ�ǰ��!!!����õĶ��󵱲��ٷ���ʱ��ҪRelease
	virtual ICfKey* __stdcall MoveToNextKey(bool nbDelete)=NULL;

	// ��õ�һ���Ӽ��������ͷŵ�ǰ��!!!����õĶ��󵱲��ٷ���ʱ��ҪRelease
	virtual ICfKey* __stdcall MoveToSubKey(void)=NULL;

	// ���ָ���Ӽ��������ͷŵ�ǰ��!!!����õĶ��󵱲��ٷ���ʱ��ҪRelease
	virtual ICfKey* __stdcall MoveToSubKey(
		IN ULONG nuID		// �Ӽ��ı�ʶID
		)=NULL;

	// ���ָ���Ӽ��������ͷŵ�ǰ��!!!����õĶ��󵱲��ٷ���ʱ��ҪRelease
	virtual ICfKey* __stdcall MoveToSubKey(
		IN const wchar_t* nszName,	// ��������ȥ��ȡ�Ӽ�
		IN int niNameLen=-1	// -1 indicate nszName is terminated by '\0' or '\\' or '/', >=0 is the charactar count of nszName
		)=NULL;

	// ��ñ�����Ĭ��ֵ����
	virtual IConfigFile::VALUETYPE __stdcall GetValueType(void)=NULL;

	// ��ñ�������չ��־������������
	virtual UCHAR __stdcall GetExtFlag(void)=NULL;

	// ��ñ�����Ĭ��ֵ���ȣ��ֽڵ�λ
	virtual LONG __stdcall GetValueLength(void)=NULL;

	// ��ñ��������֣����ص������ֳ��ȣ����npNameBuffΪNULL���������ֵĳ���(wchar_tΪ��λ�����������ַ�����β��0������-1��ʾ���������
	virtual int __stdcall GetName(wchar_t* npNameBuff,int niBufLenByWchar)=NULL;	//ע�⣬niBufLenByWchar������ŵ���wchar_t�����������ֽ���

	// ����
	virtual bool __stdcall Rename(
		IN const wchar_t* nszName,
		IN bool FailIfExist=true	//	FailIfExist==true��������ᵼ��ͬһ�ڵ��³���ͬ���ڵ㣬��ʧ��;  ==false ���Զ����Ӹ����ַ�
		)=NULL;

	// ���ID������������ֵ���Ի�õ���Hashֵ
	virtual ULONG __stdcall GetID(
		OUT int* npPos=NULL		// ����ͬ��ID�ļ�ֵ�е��Ⱥ�λ��
		)=NULL;

	// ��ñ������ڵĸ����µĴ洢λ��
	virtual int __stdcall GetPosition(void)=NULL;

	// ���û�ı䵱ǰ��Ĭ��ֵ
	virtual bool __stdcall SetValue(
		IN IConfigFile::VALUETYPE nuValueType,	// �Ӽ���Ĭ��ֵ����
		IN const void* npValueBuf,
		IN LONG  niValuelen=-1		// �ַ�������ʱ������ͨ����β��\0��������ֵ�ĳ���
		)=NULL;

	// ��õ�ǰĬ��ֵ�����ػ�õ�ֵ���ֽ���
	virtual int __stdcall GetValue(
		OUT PVOID npValueBuf,
		IN  LONG  niBufLen
		)=NULL;

	// �޸���չ��־
	virtual bool __stdcall SetExtFlag(
		IN UCHAR nchFlag
		)=NULL;

	// ֱ�ӽ�ֵ����ΪLONG��ȡ�����ֵ�����Ͳ���4�ֽڣ�����������Ĭ��ֵ
	virtual LONG __stdcall QueryValueAsLONG(
		IN LONG niDefault= -1
		)=NULL;

	// ֱ�ӽ�ֵ����ΪFLOAT��ȡ�����ֵ�����Ͳ���sizeof(FLOAT)�ֽڣ�����������Ĭ��ֵ
	virtual FLOAT __stdcall QueryValueAsFLOAT(
		IN FLOAT nfDefault = 0.0f
		)=NULL;

	// ֱ�ӽ�ֵ����ΪPVOID��ȡ�����ֵ�����Ͳ���sizeof(PVOID)�ֽڣ�����������Ĭ��ֵ
	virtual PVOID __stdcall QueryValueAsPVOID(
		IN PVOID npDefault = NULL
		)=NULL;

	// �����ֲ����Ӽ���ֱ�ӽ��Ӽ���ֵ����ΪLONG��ȡ�����ֵ�����Ͳ���4�ֽڣ�����������Ĭ��ֵ
	virtual LONG __stdcall QuerySubKeyValueAsLONG(
		IN const wchar_t* nszSubKeyName,	// ָ��Ҫ��ȡ��ֵ�ļ���������ΪNULL����nszSubKeyName[0]==UNICODE_NULL
		IN LONG niDefault= -1
		)=NULL;

	// ��ID�����Ӽ���ֱ�ӽ��Ӽ���ֵ����ΪLONG��ȡ�����ֵ�����Ͳ���4�ֽڣ�����������Ĭ��ֵ
	virtual LONG __stdcall QuerySubKeyValueAsLONG(
		IN ULONG nuID,		// �Ӽ��ı�ʶID
		IN LONG niDefault= -1
		)=NULL;

	// �����ֲ����Ӽ���ֱ�ӽ��Ӽ���ֵ����ΪFLOAT��ȡ�����ֵ�����Ͳ���sizeof(FLOAT)�ֽڣ�����������Ĭ��ֵ
	virtual FLOAT __stdcall QuerySubKeyValueAsFLOAT(
		IN const wchar_t* nszSubKeyName,	// ָ��Ҫ��ȡ��ֵ�ļ���������ΪNULL����nszSubKeyName[0]==UNICODE_NULL
		IN FLOAT nfDefault = 0.0f
		)=NULL;

	// ��ID�����Ӽ���ֱ�ӽ��Ӽ���ֵ����ΪFLOAT��ȡ�����ֵ�����Ͳ���sizeof(FLOAT)�ֽڣ�����������Ĭ��ֵ
	virtual FLOAT __stdcall QuerySubKeyValueAsFLOAT(
		IN ULONG nuID,		// �Ӽ��ı�ʶID
		IN FLOAT nfDefault = 0.0f
		)=NULL;

	// �����ֲ����Ӽ���ֱ�ӽ��Ӽ���ֵ����ΪPVOID��ȡ�����ֵ�����Ͳ���sizeof(PVOID)�ֽڣ�����������Ĭ��ֵ
	virtual PVOID __stdcall QuerySubKeyValueAsPVOID(
		IN const wchar_t* nszSubKeyName,	// ָ��Ҫ��ȡ��ֵ�ļ���������ΪNULL����nszSubKeyName[0]==UNICODE_NULL
		IN PVOID npDefault = NULL
		)=NULL;

	// ��ID�����Ӽ���ֱ�ӽ��Ӽ���ֵ����ΪPVOID��ȡ�����ֵ�����Ͳ���sizeof(PVOID)�ֽڣ�����������Ĭ��ֵ
	virtual PVOID __stdcall QuerySubKeyValueAsPVOID(
		IN ULONG nuID,		// �Ӽ��ı�ʶID
		IN PVOID npDefault = NULL
		)=NULL;

	// �����ֲ����Ӽ���ֱ�ӻ�ȡ�Ӽ���ֵ������С�����ʾĿ��������ڣ�����0��ʾĿ���ֵΪ�գ�����0��ʾ���ص�Value����
	virtual int __stdcall QuerySubKeyValue(
		IN const wchar_t* nszSubKeyName,	// ָ��Ҫ��ȡ��ֵ�ļ���������ΪNULL����nszSubKeyName[0]==UNICODE_NULL
		OUT PVOID npValueBuf,
		IN  LONG  niBufLen
		)=NULL;

	// ��ȡָ����ֵ�洢New�������ڴ��з�������ע��Ҫ�ͷŷ��ص�ָ��, ����Colin �ӡ�
	virtual PVOID __stdcall QueryValueAsBuffer()=NULL;

	// ��ȡָ�����ӽ���ֵ�洢��New�������ڴ��з�������ע��Ҫ�ͷŷ��ص�ָ��, ����Colin �ӡ�
	virtual PVOID __stdcall QuerySubKeyValueAsBuffer(
		IN const wchar_t* nswSubKeyName
		)=NULL;

	// ��ȡָ�����ӽ���ֵ�洢��New�������ڴ��з�������ע��Ҫ�ͷŷ��ص�ָ��, ����Colin �ӡ�
	virtual PVOID __stdcall QuerySubKeyValueAsBuffer(
		IN ULONG nuID									// �Ӽ��ı�ʶID
		)=NULL;

	// �ͷ���Query***AsBufferϵ�к������ص�Buffer  ����Colin �ӡ�
	virtual void _stdcall ReleaseBuffer(OUT PVOID npvoBufferToRelease)=NULL;

};

#define CDKEY_FLAG_LOCKED	16		// ���������Ա��޸Ļ���ɾ��
#define CFKEY_FLAG_DISABLE	32		// ���ñ���Ϊ���ã�ʧЧ���򲻿ɼ����˱�־ֻ�ǹ�Ӧ�ó���ʶ��ʹ�ã�����ϵͳֻ���𱣴�Ͷ�ȡ
#define CDKEY_FLAG_CUSTOM1	64		// �Զ�������1���������ݵ�ʹ�ó�������������;
#define CDKEY_FLAG_CUSTOM2	128		// �Զ�������2���������ݵ�ʹ�ó�������������;

#ifdef __cplusplus
extern "C" {
#endif

// nuCreationDisposition refer to the function CfCreateConfig
#if defined(KERNEL_CODE)||defined(NATIVE_CODE)
// ����̬
#define CF_CREATE_ALWAYS	FILE_OVERWRITE_IF
#define CF_CREATE_NEW		FILE_CREATE
#define CF_OPEN_ALWAYS	FILE_OPEN_IF
#define CF_OPEN_EXISTING	FILE_OPEN
#else
// �û�̬
#define CF_CREATE_ALWAYS	CREATE_ALWAYS
#define CF_CREATE_NEW		CREATE_NEW
#define CF_OPEN_ALWAYS	OPEN_ALWAYS
#define CF_OPEN_EXISTING	OPEN_EXISTING
#endif//KERNEL_CODE

// �������ߴ�һ�������ļ������������ļ�������߷���NULL��ʾʧ�ܣ�ʧ�ܵ�ԭ��������ļ������ڻ����ļ���ʽ����
IConfigFile* __stdcall CfCreateConfig(
		IN const wchar_t* nszPathName,				// �ļ�������·����������ΪNULL����ʾ����һ�����ļ���ʱ��ָ���ļ���
		IN ULONG nuCreationDisposition=CF_OPEN_EXISTING	// ͬCreateFile API���ƣ���������CREATE_ALWAYS��CREATE_NEW��OPEN_ALWAYS��OPEN_EXISTING�����������
		);

// ͬ����ĺ����������ǣ�StableConfig���Զ�Ϊ�ļ�����һ�����ݸ�������ֹ�����ļ���������ԭ���µ�һ�ļ��ƻ�
// �����ļ�������ǰ�����δ򿪵Ĳ�ͬ�����������ǽ���ʹ�ã���Ϊ���ݣ��Ӷ���ֹĳ�ε�ʹ�ù������ƻ�������һ���ļ���
// ��Ҫע�⣬���ݵ��Ǵ�ǰ�����ݣ���һ���򿪵����ñ���θ�д�������һ��д����ļ�����𻵣��������Զ��ظ��Ĳ���ǰһ��д������ݣ�����ǰ�ζ����ǰ��󱣴�����ݡ�
// �磺��������ļ���Ϊabc.set����ô��ͬһ��Ŀ¼�½����ܴ���һ��abc_dup.set���ļ��������Ǹ�����
IConfigFile* __stdcall CfCreateStableConfig(
	IN const wchar_t* nszPathName,				// �ļ�������·����������ΪNULL����ʾ����һ�����ļ���ʱ��ָ���ļ���
	IN ULONG nuCreationDisposition=CF_OPEN_EXISTING	// ͬCreateFile API���ƣ���������CREATE_ALWAYS��CREATE_NEW��OPEN_ALWAYS��OPEN_EXISTING�����������
	);


#ifdef __cplusplus
}
#endif




#endif//_CONFIGINTF_H_