/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#ifndef _CONFIGBASE_H_
#define _CONFIGBASE_H_
/*
	modified by Ax Mar.15,2011	��������֮��ͨ�����ļ��࣬ͬǰ�ߵ������ǣ������˸���֧�֣��������£�
							 �����ļ�������ǰ�����δ򿪵Ĳ�ͬ�����������ǽ���ʹ�ã���Ϊ���ݣ��Ӷ���ֹĳ�ε�ʹ�ù������ƻ�������һ���ļ���
							 �磺��������ļ���Ϊabc.set����ô��ͬһ��Ŀ¼�½����ܴ���һ��abc.set.dup���ļ��������Ǹ�����

*/
#include "CfgIface.h"
#include "cmmstruct.h"

#define CF_LOOKASIDE_UNUSEDKEY_COUNT 5


#pragma pack(1)
//////////////////////////////////////////////////////////////////////////
// �洢���ļ��е����ݽṹ
typedef struct _CF_FILE_HEAD_SHORT{
		ULONG Signature;  // must be CF_SIGNATURE
		ULONG Version;	  // current version is 0x00010003
		ULONG SequenceA;
		ULONG SizeOfHead;		// �ļ�ͷ���ֽ���
		ULONG SizeOfReserved;	// ���������ֽ���
		ULONG SizeOfKeys;		// �洢��ȫ�������ֽ���
		ULONG CheckSum;		// ����Key�洢��У���
		ULONG Duplicate;		// ������ţ�������ֵΪ0��ʾû�и���
}CF_FILE_HEAD_SHORT,* PCF_FILE_HEAD_SHORT;
typedef union _CF_FILE_HEAD {
	struct _CF_FILE_HEAD_SHORT ShortHead;
	struct  
	{
		UCHAR Reserved[0x200-4];
		ULONG SequenceB;
	}Tail;
}CF_FILE_HEAD,* PCF_FILE_HEAD;

#define CF_SIGNATURE 'CFFL'
#define CF_VERSION 0x00010005

typedef struct _CF_KEY_ENTRY{
	UCHAR NameLength;
	UCHAR Flag;
	USHORT ValueSize;	// ֵ�ĳ���
	LONG SubEntryCount;	// ӵ�е�������
}CF_KEY_ENTRY,* PCF_KEY_ENTRY;

#define CFKEY_VALUE_TYPE(_X) ((IConfigFile::VALUETYPE)(_X&0x3))
#define CFKEY_SET_VALUETYPE(_X,_T) (_X=((_X&0xFC)|(_T&0x3)))
#define CFKEY_GET_EXTFLAG(_X) (_X&0xF0)
#define CFKEY_SET_EXTFLAG(_X,_T) (_X=((_X&0xF)|(_T&0xF0)))

#define CFKEY_INDEX_AVAILABLE 4		// ������������������
#define CFKEY_NODE_OPENED 8 // ���Key������
#define CFKEY_HAS_CHILD(P_NODE) ((P_NODE->Flag&CFKEY_INDEX_AVAILABLE)!=0)

#pragma pack()

struct _CFKEY_NODE;
class CCfKeyInterface;

class CCfKeyHash{
public:
	ULONG HashValue;
	struct _CFKEY_NODE* KeyObj;
	CCfKeyHash(){}
	void operator=(const class CCfKeyHash& src){
		HashValue = src.HashValue;
		KeyObj = src.KeyObj;
	}
	// ���ַ�������Hash��,���ص������ֵ���Ч����
	int GenerateHashCode(
		IN const wchar_t* nszName,
		IN int niNameLen=-1	// -1 indicate that the name was terminated by '\0' or '\\' or '/'
		);
};

//////////////////////////////////////////////////////////////////////////
// ��������������Ƚ���
class CKeyHashCriterion	// Ĭ�ϵ��ж�׼��
{
public:
	bool operator () (const CCfKeyHash& Obj1,const CCfKeyHash& Obj2)const // һ��Ҫ����������
	{
		// ������Ob1С�ڶ���Obj2ʱ������True�����򷵻�false
		return (Obj1.HashValue < Obj2.HashValue);
	}
};

class CCfKeyOpened{
public:
	struct _CFKEY_NODE* KeyNode;
	CCfKeyInterface* Interface;
	CCfKeyOpened(){}
	void operator=(const class CCfKeyOpened& src){
		KeyNode = src.KeyNode;
		Interface = src.Interface;
	}
};

//////////////////////////////////////////////////////////////////////////
// �򿪼��Ĺ������������Ƚ���
class CKeyInterfaceCriterion	// Ĭ�ϵ��ж�׼��
{
public:
	bool operator () (const CCfKeyOpened& Obj1,const CCfKeyOpened& Obj2)const // һ��Ҫ����������
	{
		// ������Ob1С�ڶ���Obj2ʱ������True�����򷵻�false
		return (Obj1.KeyNode < Obj2.KeyNode);
	}
};

typedef cmmMultiSequence<CCfKeyHash,CKeyHashCriterion,0,16> TCFKEYSEQUENCE; 
typedef cmmStack<CCfKeyInterface*,15> TCFKEYINTERFACESTACK;
typedef cmmMultiSequence<CCfKeyOpened,CKeyInterfaceCriterion,32,32> TCFKEYOPENED;


//////////////////////////////////////////////////////////////////////////
// �洢���ڴ��е�һ��
#pragma pack(1)

// ���ڵ�
typedef struct _CFKEY_NODE{
	UCHAR Flag;			// ָ������������	
	UCHAR NameLength;	// ���ֵ���������������β��0�����û�����֣���ֵΪ0�������Name�޶���
	USHORT ValueLength; // ĳֵ�Ĵ�С
}CFKEY_NODE,* PCFKEY_NODE;

// �����Ӽ�ֵ�Ľڵ�
typedef struct _CFKEY_BRANCH {
	UCHAR Flag;			// ָ������������
	UCHAR NameLength;	// ���ֵ���������������β��0�����û�����֣���ֵΪ0
	USHORT ValueLength; // ĳֵ�Ĵ�С
	TCFKEYSEQUENCE* mpSubKeys;	// �����Ӽ�ֵ�Ķ����ָ��
	wchar_t Name[1];	// �����ַ�����Я��һ����β��0
}CFKEY_BRANCH,* PCFKEY_BRANCH;

// �������Ӽ�ֵ�Ľڵ�
typedef struct _CFKEY_LEAF {
	UCHAR Flag;			// ָ������������
	UCHAR NameLength;	// ���ֵ���������������β��0�����û�����֣���ֵΪ0
	USHORT ValueLength; // ĳ��ֵ�Ĵ�С
	wchar_t Name[1];	// �����ַ�����Я��һ����β��0
}CFKEY_LEAF,* PCFKEY_LEAF;

#pragma pack()

//////////////////////////////////////////////////////////////////////////
// һ��Key������ļ��к��ڴ��е��������
//	�ļ��� |Head|Name|Value|Hash|
//  �ڴ��� |Head|Name|0|Value|
//			 �� SubKeys
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
// �����ļ���
//	��������ʵ��һ�������ļ�������������������ļ��������µ����ԣ�
//		1. ��ŵ�������Key���ɣ�ÿ��Key����һ�����ֺ�һ��Ĭ��ֵ��Ĭ��ֵ����������Ϊ�ַ������߶��������ݣ�2.�����νṹ����֯ȫ����Key�����и�Root��ϵͳĬ�ϵ�Key���������е�Key����������Key��������Key����Key��
//		3. ����Key��������ɢ��ֵ�����ṩ�������ܹ��ܿ����Ѱ�ʹ���Key���ʺϴ洢��������Key��4. ���Ը�ÿ��Keyָ����ţ�ͬһ��Key�µ�ֱ����Key��Ų����ظ����Ƿ�ʹ�������Ҫ�������Key����ʱָ��������ʹ�����
//		��Key���½�����key�����û���ṩ��ţ�ϵͳ���Զ�����Ŀǰ�����ŵ���һ����š�
DECLARE_BUILTIN_NAME(CConfigFile)
class CConfigFile: public cmmBaseObject<CConfigFile,IConfigFile,GET_BUILTIN_NAME(CConfigFile)>
{
	friend class CCfKeyInterface;
public:
	CConfigFile();
	virtual ~CConfigFile();

	ULONG InitOnCreate(
		IN const wchar_t* nszPathName,				// �ļ�������·����������ΪNULL����ʾ����һ�����ļ���ʱ��ָ���ļ���
		IN ULONG nuCreationDisposition			// ͬCreateFile API���ƣ���CfgIface.h�ļ��е���ض���
		);

	DEFINE_CUMSTOMIZE_CREATE(CConfigFile,(const wchar_t* nszPathName=NULL,ULONG nuCreationDisposition=CF_OPEN_EXISTING),(nszPathName,nuCreationDisposition))

	// �����޸ĵ��ļ�
	virtual bool __stdcall SaveFile(
		IN const wchar_t* nszOtherFile=NULL	// ����ָ������Ϊ�������ļ������ΪNULL���Ᵽ�浽�ղŴ򿪻����½����ļ�
		);


	// ��ø�������õĶ��󵱲��ٷ���ʱ��ҪRelease
	virtual ICfKey* __stdcall GetRootKey(void);

	// ��ָ���ļ�����õĶ��󵱲��ٷ���ʱ��ҪRelease
	virtual ICfKey* __stdcall OpenKey(
		IN const wchar_t* nszKeyName,		// ��'/'�ָ�Ӽ�������������ָ�����磺xx/yy/zz
		IN bool nbCreateIf=false			// �������������ڣ�������
		);

	// �ж��ַ���ֵ�ĳ���
	static int GetValueLengthOfTypeString(IConfigFile::VALUETYPE nuValueType,const void* npValueBuf);
protected:
	//int miReferenceCount;
	wchar_t mszFileName[256];
// 	ULONG muFileHeadSequence;
	CF_FILE_HEAD_SHORT mdFileHead;
	PCFKEY_BRANCH mpRoot;
	CCfKeyInterface* mpRootInterface;
	TCFKEYOPENED moKeyOpened;	// ���������Ŵ򿪵�Key
	TCFKEYINTERFACESTACK moReleasedKey;	// Ϊ�˼����ڴ������ͷŵĴ�������һЩ�ͷŵ�Key�򿪶��������������������Keyʱʹ��
	CExclusiveAccess moExclusive;
	TCFKEYINTERFACESTACK moUnusedKeyInterface;	// ��������ͷŵ�δʹ�õ�KeyInterface�����Լ���Ƶ���򿪹ر�Keyʱ�������ڴ������ͷ�

	// ����һ���µ�Node
	PCFKEY_NODE AllocateNode(
		IN const wchar_t* nszName,	// �Ӽ�������
		IN int niNameLen,		// count in wchar, if be -1 indicate that the nszName is terminated by '\0' or '\\' or '/'
		IN UCHAR nchFlag = 0,	// �Ӽ���Ĭ��ֵ���ͣ����ΪInvalid������Ժ���������
		IN const void* npValueBuf = NULL,
		IN int  niValuelen = 0,
		IN int  niSubKeyCount = 0
		);

	// �ݹ���ã�װ��ָ�����������Ӽ�������ʱnrKeyEntry���޸�ָ����һ��Entry
	PCFKEY_NODE LoadKey(PCF_KEY_ENTRY& nrKeyEntry,const PCF_KEY_ENTRY npEndEntry);

	// �ݹ���ã����汾�����Ӽ���ֵ������0��ʾʧ�ܡ�����ʽnrKeyEntry��Խ���Ѿ���ŵ����ݡ��������ʱnrKeyEntry==NULL����ֻ������Ҫ�Ĵ洢����С
	int SaveKey(PCFKEY_NODE npKey,PCF_KEY_ENTRY& nrKeyEntry);

	// ɾ����ֵ�������Ӽ�ֵ
	bool RemoveKey(PCFKEY_NODE npKey);

	// ѯ���Ƿ����ɾ��һ���Ӽ�ֵ�����������²�ĳ����ֵ
	bool QueryRemove(PCFKEY_NODE npKey);

	// ��һ��Key
	CCfKeyInterface* GetKeyInterface(CCfKeyInterface* npParents,int niPosition);

	// ����һ����Key�Ĵ򿪶���
	CCfKeyInterface* AllocateKeyInterface(PCFKEY_NODE npKeyNode);

	// ���Ӷ�һ��Key������
	int AddReferToKeyInterface(CCfKeyInterface* npKeyInterface);

	// ���ٶ�һ��Key�����ã��������Ϊ0���ͷ���
	int ReleaseKeyInterface(CCfKeyInterface* npKeyInterface);

	// ����һ���򿪲�����Ч��KEY����Ϊ���ܴ���ָ��ͬһ��npKeyNode�Ĳ�ͬnpKeyInterface������ֻ��һ������Ч�ģ�������ָ��Ķ��������Ѿ�ɾ����������Keyֵ
	CCfKeyInterface* FindValidKeyInterface(PCFKEY_NODE npKeyNode);

	//// ����һ����һ������������KeyInterface��ע�⣬�����Ƕ�Ӧ��Key�Ѿ���ɾ����Interface��û�б��ͷ�
	//int FindKeyInterface(PCFKEY_NODE npKeyNode);

	// ����һ��KeyInterface�Ƿ��ڱ��������д���
	int FindKeyInterface(CCfKeyInterface* npKeyInterface);

	// ����һ���򿪵ļ���Nodeָ��
	bool UpdateInterface(CCfKeyInterface* npInterface,PCFKEY_NODE npKeyNode);

	// ����һ���򿪵ļ���SubNodes���飬��Ϊ���鷢���˿��ܵ�������Interface������ָ��ĳ��SubNode)ʧЧ��������Ҫ����
	void UpdateSubNodes(CCfKeyInterface* npInterface);

};

DECLARE_BUILTIN_NAME(CCfKeyInterface)
class CCfKeyInterface: public cmmBaseObject<CCfKeyInterface,ICfKey,GET_BUILTIN_NAME(CCfKeyInterface)>
{
	friend class CConfigFile;
public:
	CCfKeyInterface(){
//		miReferenceCount = 1;	// �൱�ڼ�¼��һ������
		mbDeleted = false;
	}
	virtual ~CCfKeyInterface();

	ULONG InitOnCreate(CConfigFile* npConfigFile){
		mpConfigFile = npConfigFile;
		return 0;
	}

	void Reuse(CConfigFile* npConfigFile){
		mbDeleted = false;
		mpConfigFile = npConfigFile;
		cmmBaseObject<CCfKeyInterface,ICfKey,GET_BUILTIN_NAME(CCfKeyInterface)>::Reuse();	
	}


	DEFINE_CUMSTOMIZE_CREATE(CCfKeyInterface,(CConfigFile* npConfigFile),(npConfigFile))

	// ��������
	virtual int __stdcall AddRefer(void);

	//�ͷż����½����򿪣������κ�һ�ַ����õ��ı�����󶼱����ͷ�
	virtual int __stdcall Release(void);

	//ɾ���������ע�⣬����ɾ����Ľӿ���Ȼ��ҪRelease�����ù�Delete������ӿ�Ҳ��û�����ˣ���Release�⣬�������ܶ���֧����
	virtual bool __stdcall Delete(void);	

	// �½�һ����������NULL��ʾʧ�ܣ�ʧ�ܵ�ԭ��������Ѿ�������ͬ�ļ�ֵ�����߷����ڴ�ʧ��
	virtual ICfKey* __stdcall NewSubKey(
		IN const wchar_t* nszName,	// �Ӽ������֣�����Ϊ��
		IN IConfigFile::VALUETYPE nuValueType = IConfigFile::Invalid,	// �Ӽ���Ĭ��ֵ���ͣ����ΪInvalid������Ժ���������
		IN const void* npValueBuf = NULL,
		IN LONG  niValuelen = 0	// �ַ�������ʱ������ͨ����β��\0��������ֵ�ĳ��ȣ���ʱ����Ϊ-1
		);

	// �½�һ��������������NULL��ʾʧ�ܣ����ͬʱ����������ֵ�����п��ܴ���������ֵ�����������м�����
	virtual ICfKey* __stdcall NewSubKey(
		IN ULONG nuID,		// ָ���������ı�ʶID�������Բ�Ҫʹ����ͬ��ʶ
		IN bool nbAhead=false,	// ��������ͬID���Ӽ�ʱ������ͬID�Ӽ�֮ǰ�����¼�����������ͬID�Ӽ�֮�����¼�
		IN IConfigFile::VALUETYPE nuValueType = IConfigFile::Invalid,	// �Ӽ���Ĭ��ֵ���ͣ����ΪInvalid������Ժ���������
		IN const void* npValueBuf = NULL,
		IN LONG  niValuelen = 0		// �ַ�������ʱ������ͨ����β��\0��������ֵ�ĳ��ȣ���ʱ����Ϊ-1
		);

	// ��ñ����ĸ�������������Ǹ������򷵻�NULL
	virtual ICfKey* __stdcall GetParentsKey(void);

	// �������ø��ڵ㣬�����ڵ�ӵ�ǰ���ڵ��Ƴ��������µĸ��ڵ���
	virtual bool __stdcall SetParentKey(
		IN ICfKey* npNewParent,
		IN bool nbAhead
		);

	// �����һ��������õĶ��󵱲��ٷ���ʱ��ҪRelease
	virtual ICfKey* __stdcall GetNextKey(void);

	// ��õ�һ���Ӽ�����õĶ��󵱲��ٷ���ʱ��ҪRelease
	virtual ICfKey* __stdcall GetSubKey(void);

	// ����Ӽ�����õĶ��󵱲��ٷ���ʱ��ҪRelease��������ͬ�����GetSubKey�����ǣ�����İ����ִ򿪼�ֵ����������ֱ�Ӱ�ID�򿪣�ֻ��ͨ�����������ܴ�û�����ֵļ�ֵ
	virtual ICfKey* __stdcall GetSubKey(
		IN ULONG nuID,		// �Ӽ��ı�ʶID
		IN int niPos=0		// ����ͬ��ID�ļ�ֵ�е��Ⱥ�λ�ã�< 0��ʾȡ���һ��
		);

	// �򿪶��·��ָ�����Ӽ�����õĶ��󵱲��ٷ���ʱ��ҪRelease��������ͬ�����GetSubKey�����ǣ����������Դ�һ��·���磺xx/yy/zzָ�������²��zz�Ӽ�����GetSubKeyֻ��򿪵�һ���xx�Ӽ�
	virtual ICfKey* __stdcall OpenKey(
		IN const wchar_t* nszKeyName,		// ��'/'�ָ�Ӽ�������������ָ�����磺xx/yy/zz
		IN bool nbCreateIf=false			// �������������ڣ�������
		);

	// ����Ӽ�����õĶ��󵱲��ٷ���ʱ��ҪRelease��������ͬ�����OpenKey�����ǣ�OpenKey���Դ�һ��·���磺xx/yy/zzָ�������²��zz�Ӽ�����GetSubKeyֻ��򿪵�һ���xx�Ӽ�
	virtual ICfKey* __stdcall GetSubKey(
		IN const wchar_t* nszName,	// ��������ȥ��ȡ�Ӽ�
		IN int niNameLen=-1,	// -1 indicate nszName is terminated by '\0' or '\\' or '/', >=0 is the charactar count of nszName
		OUT int* npNameLen=NULL,	// �������ֵ���Ч����
		IN bool nbCreateIf=false	// ����ü������ڣ�������
		);

	// �����һ�����������ͷŵ�ǰ��!!!����õĶ��󵱲��ٷ���ʱ��ҪRelease
	virtual ICfKey* __stdcall MoveToParentsKey(void);

	// �����һ�����������ͷŵ�ǰ��!!!����õĶ��󵱲��ٷ���ʱ��ҪRelease
	virtual ICfKey* __stdcall MoveToNextKey(void);

	// �����һ������ɾ�����ͷŵ�ǰ��!!!����õĶ��󵱲��ٷ���ʱ��ҪRelease
	virtual ICfKey* __stdcall MoveToNextKey(bool nbDelete);

	// ��õ�һ���Ӽ��������ͷŵ�ǰ��!!!����õĶ��󵱲��ٷ���ʱ��ҪRelease
	virtual ICfKey* __stdcall MoveToSubKey(void);

	// ���ָ���Ӽ��������ͷŵ�ǰ��!!!����õĶ��󵱲��ٷ���ʱ��ҪRelease
	virtual ICfKey* __stdcall MoveToSubKey(
		IN ULONG nuID		// �Ӽ��ı�ʶID
		);

	// ���ָ���Ӽ��������ͷŵ�ǰ��!!!����õĶ��󵱲��ٷ���ʱ��ҪRelease
	virtual ICfKey* __stdcall MoveToSubKey(
		IN const wchar_t* nszName,	// ��������ȥ��ȡ�Ӽ�
		IN int niNameLen=-1	// -1 indicate nszName is terminated by '\0' or '\\' or '/', >=0 is the charactar count of nszName
		);

	// ��ñ�����Ĭ��ֵ����
	virtual IConfigFile::VALUETYPE __stdcall GetValueType(void);

	// ��ñ�������չ��־������������
	virtual UCHAR __stdcall GetExtFlag(void);

	// ��ñ�����Ĭ��ֵ���ȣ��ֽڵ�λ
	virtual LONG __stdcall GetValueLength(void);

	// ��ñ���������
	virtual int __stdcall GetName(wchar_t* npNameBuff,int niBufLenByWchar);

	// ������
	virtual bool __stdcall Rename(
		IN const wchar_t* nszName,
		IN bool FailIfExist=true	//	FailIfExist==true��������ᵼ��ͬһ�ڵ��³���ͬ���ڵ㣬��ʧ��;  ==false ���Զ����Ӹ����ַ�
		);

	// ���û�ı䵱ǰ��Ĭ��ֵ
	virtual bool __stdcall SetValue(
		IN IConfigFile::VALUETYPE nuValueType,	// �Ӽ���Ĭ��ֵ����
		IN const void* npValueBuf,
		IN LONG  niValuelen=-1
		);

	// ��õ�ǰĬ��ֵ�����ػ�õ�ֵ���ֽ���
	virtual int __stdcall GetValue(
		OUT PVOID npValueBuf,
		IN  LONG  niBufLen
		);

	// ���ID������������ֵ���Ի�õ���Hashֵ
	virtual ULONG __stdcall GetID(
		OUT int* npPos=NULL		// ����ͬ��ID�ļ�ֵ�е��Ⱥ�λ��
		);

	// ��ñ������ڵ����
	virtual int __stdcall GetPosition(void);

	// �޸���չ��־
	virtual bool __stdcall SetExtFlag(
		IN UCHAR nchFlag
		);

	// ֱ�ӽ�ֵ����ΪLONG��ȡ�����ֵ�����Ͳ���4�ֽڣ�����������Ĭ��ֵ
	virtual LONG __stdcall QueryValueAsLONG(
		IN LONG niDefault= -1
		);

	// ֱ�ӽ�ֵ����ΪFLOAT��ȡ�����ֵ�����Ͳ���sizeof(FLOAT)�ֽڣ�����������Ĭ��ֵ
	virtual FLOAT __stdcall QueryValueAsFLOAT(
		IN FLOAT nfDefault = 0.0f
		);

	// ֱ�ӽ�ֵ����ΪPVOID��ȡ�����ֵ�����Ͳ���sizeof(PVOID)�ֽڣ�����������Ĭ��ֵ
	virtual PVOID __stdcall QueryValueAsPVOID(
		IN PVOID npDefault = NULL
		);

	// �����ֲ����Ӽ���ֱ�ӽ��Ӽ���ֵ����ΪLONG��ȡ�����ֵ�����Ͳ���4�ֽڣ�����������Ĭ��ֵ
	virtual LONG __stdcall QuerySubKeyValueAsLONG(
		IN const wchar_t* nszSubKeyName,	// ָ��Ҫ��ȡ��ֵ�ļ���������ΪNULL����nszSubKeyName[0]==UNICODE_NULL
		IN LONG niDefault= -1
		);

	// ��ID�����Ӽ���ֱ�ӽ��Ӽ���ֵ����ΪLONG��ȡ�����ֵ�����Ͳ���4�ֽڣ�����������Ĭ��ֵ
	virtual LONG __stdcall QuerySubKeyValueAsLONG(
		IN ULONG nuID,		// �Ӽ��ı�ʶID
		IN LONG niDefault= -1
		);

	// �����ֲ����Ӽ���ֱ�ӽ��Ӽ���ֵ����ΪFLOAT��ȡ�����ֵ�����Ͳ���sizeof(FLOAT)�ֽڣ�����������Ĭ��ֵ
	virtual FLOAT __stdcall QuerySubKeyValueAsFLOAT(
		IN const wchar_t* nszSubKeyName,	// ָ��Ҫ��ȡ��ֵ�ļ���������ΪNULL����nszSubKeyName[0]==UNICODE_NULL
		IN FLOAT nfDefault = 0.0f
		);

	// ��ID�����Ӽ���ֱ�ӽ��Ӽ���ֵ����ΪFLOAT��ȡ�����ֵ�����Ͳ���sizeof(FLOAT)�ֽڣ�����������Ĭ��ֵ
	virtual FLOAT __stdcall QuerySubKeyValueAsFLOAT(
		IN ULONG nuID,		// �Ӽ��ı�ʶID
		IN FLOAT nfDefault = 0.0f
		);

	// �����ֲ����Ӽ���ֱ�ӽ��Ӽ���ֵ����ΪPVOID��ȡ�����ֵ�����Ͳ���sizeof(PVOID)�ֽڣ�����������Ĭ��ֵ
	virtual PVOID __stdcall QuerySubKeyValueAsPVOID(
		IN const wchar_t* nszSubKeyName,	// ָ��Ҫ��ȡ��ֵ�ļ���������ΪNULL����nszSubKeyName[0]==UNICODE_NULL
		IN PVOID npDefault = NULL
		);

	// ��ID�����Ӽ���ֱ�ӽ��Ӽ���ֵ����ΪPVOID��ȡ�����ֵ�����Ͳ���sizeof(PVOID)�ֽڣ�����������Ĭ��ֵ
	virtual PVOID __stdcall QuerySubKeyValueAsPVOID(
		IN ULONG nuID,		// �Ӽ��ı�ʶID
		IN PVOID npDefault = NULL
		);

	// �����ֲ����Ӽ���ֱ�ӻ�ȡ�Ӽ���ֵ������С�����ʾĿ��������ڣ�����0��ʾĿ���ֵΪ�գ�����0��ʾ���ص�Value����
	virtual int __stdcall QuerySubKeyValue(
		IN const wchar_t* nszSubKeyName,	// ָ��Ҫ��ȡ��ֵ�ļ���������ΪNULL����nszSubKeyName[0]==UNICODE_NULL
		OUT PVOID npValueBuf,
		IN  LONG  niBufLen
		);

	// ��ȡָ����ֵ�洢New�������ڴ��з�������ʧ�ܷ���NULL��ע��Ҫ�ͷŷ��ص�ָ��, ����Colin �ӡ�
	virtual PVOID __stdcall QueryValueAsBuffer();

	// ��ȡָ�����ӽ���ֵ�洢��New�������ڴ��з�������ʧ�ܷ���NULL��ע��Ҫ�ͷŷ��ص�ָ��, ����Colin �ӡ�
	virtual PVOID __stdcall QuerySubKeyValueAsBuffer(
		IN const wchar_t* nswSubKeyName
		); 

	// ��ȡָ�����ӽ���ֵ�洢��New�������ڴ��з�������ʧ�ܷ���NULL��ע��Ҫ�ͷŷ��ص�ָ��, ����Colin �ӡ�
	virtual PVOID __stdcall QuerySubKeyValueAsBuffer(
		IN ULONG nuID									// �Ӽ��ı�ʶID
		); 

	// �ͷ���Query***AsBufferϵ�к������ص�Buffer  ����Colin �ӡ�
	virtual void _stdcall ReleaseBuffer(OUT PVOID npvoBufferToRelease);

protected:
	//int miReferenceCount;
	bool mbDeleted;

	union {
		PCFKEY_NODE BaseNode;
		PCFKEY_BRANCH Branch;
		PCFKEY_LEAF Leaf;
	};
	CCfKeyInterface* mpParentsKey;
	int miPositionInBrothers;	// ���ڵ��ڸ��ڵ���ӽڵ��������е�λ��

	CConfigFile* mpConfigFile;

	// �����ӽڵ㣬�����ӽڵ���Hash�����е���ţ�����-1��ʾû���ҵ�
	int FindSubKey(
		IN const wchar_t* nszName,
		IN int niNameLen=-1,	// -1 indicate nszName is terminated by '\0' or '\\' or '/', >=0 is the charactar count of nszName
		OUT int* npNameLen=NULL	// �������ֵ���Ч����
		);

	// �����ӽڵ㣬�����ӽڵ���Hash�����е���ţ�����-1��ʾû���ҵ�
	int FindSubKey(
		ULONG nuHashCode,
		int niPos=0		// ����ͬ��ID�ļ�ֵ�е��Ⱥ�λ�ã�< 0��ʾȡ���һ��
		);

	// ����һ���µ��ӽڵ㣬���ز����λ��
	inline int InsertSubNode(
		PCFKEY_NODE npNode,
		ULONG nuHashCode=0,
		IN bool nbAhead=false	// ��������ͬID���Ӽ�ʱ������ͬID�Ӽ�֮ǰ�����¼�����������ͬID�Ӽ�֮�����¼�
		);

	// ����ӽڵ�
	inline PCFKEY_NODE GetSubNode(
		IN int niPosition
		);

	// �Ƴ��ӽڵ�
	inline bool RemoveSubNode(
		IN int niPosition
		);

	// �����ӽڵ㣬�ⲻ��ı��������
	inline bool UpdateSubNode(
		PCFKEY_NODE npNode,
		IN int niPosition
		);
};



//////////////////////////////////////////////////////////////////////////
// �����ļ���2
//	��������֮��ͨ�����ļ��࣬ͬǰ�ߵ������ǣ������˸���֧�֣��������£�
// �����ļ�������ǰ�����δ򿪵Ĳ�ͬ�����������ǽ���ʹ�ã���Ϊ���ݣ��Ӷ���ֹĳ�ε�ʹ�ù������ƻ�������һ���ļ���
// ��Ҫע�⣬���ݵ��Ǵ�ǰ�����ݣ���һ���򿪵����ñ���θ�д�������һ��д����ļ�����𻵣��������Զ��ظ��Ĳ���ǰһ��д������ݣ�����ǰ�ζ����ǰ��󱣴�����ݡ�
// �磺��������ļ���Ϊabc.set����ô��ͬһ��Ŀ¼�½����ܴ���һ��abc_dup.set���ļ��������Ǹ�����
DECLARE_BUILTIN_NAME(CStableConfigFile)
class CStableConfigFile:public CConfigFile
{
public:
	CStableConfigFile(){
		mbRestored = false;
	}

	ULONG InitOnCreate(
		IN const wchar_t* nszPathName,				// �ļ�������·����������ΪNULL����ʾ����һ�����ļ���ʱ��ָ���ļ���
		IN ULONG nuCreationDisposition			// ͬCreateFile API���ƣ���CfgIface.h�ļ��е���ض���
		);

	~CStableConfigFile(){
	}

	// ���ش������ߺ���
	DEFINE_CUMSTOMIZE_CREATE(CStableConfigFile,(const wchar_t* nszPathName=NULL,ULONG nuCreationDisposition=CF_OPEN_EXISTING),(nszPathName,nuCreationDisposition))

	// ��������ʶ��
	DEFINE_DERIVED_TYPECAST(CStableConfigFile,CConfigFile)

	// �жϴ�����ʱ���Ƿ��������������𻵣��ӱ����лָ�����Ϊ��???��δ֧��
	bool IsRestored(){
		return mbRestored;
	}

private:
	bool mbRestored;

};



#endif//_CONFIGBASE_H_