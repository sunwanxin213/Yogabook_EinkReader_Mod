/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#ifndef MAP_LIST_H_
#define MAP_LIST_H_

#include "MapDefine.h"
#include "cmmstruct.h"

// ���ַ�����ǰ�ĸ��ַ���ȡHASHֵ
class CHashFront
{
public:
	ULONG operator()(const wchar_t* nswString)
	{

		ULONG luHash = 0;
		int liLen = wcslen(nswString);
		wchar_t lcCrt;

		liLen = liLen>4?4:liLen;	// ���liLen>4,��ȡ4���ֽ�
		
		for (int liIndex = 0; liIndex < liLen; liIndex++)
		{
			if (nswString[liIndex] == 0)
				break;

			// ����HASHֵ
			lcCrt = nswString[liIndex];
			if((lcCrt|0x20) >= L'a' && (lcCrt|0x20) <= L'z')
					lcCrt |= 0x20;
			luHash += lcCrt;
			luHash = (luHash<<1) | ((luHash&0x010000)>>16);

		}

		return luHash;
	}

};

// ���ַ���������ĸ��ַ���ȡHASHֵ
class CHashEnd
{
public:
	ULONG operator()(const wchar_t* nswString)
	{
		ULONG luHash = 0;
		int liLen = wcslen(nswString);
		wchar_t lcCrt;

		liLen = (liLen-4)>0?(liLen-4):0;	// ���liLen>4,��ȡ4���ֽ�

		for (int liIndex = wcslen(nswString)-1; liIndex >= liLen; liIndex--)
		{
			if (nswString[liIndex] == 0)
				break;

			// ����HASHֵ
			lcCrt = nswString[liIndex];
			if((lcCrt|0x20) >= L'a' && (lcCrt|0x20) <= L'z')
				lcCrt |= 0x20;
			luHash += lcCrt;
			luHash = (luHash<<1) | ((luHash&0x010000)>>16);

		}

		return luHash;
	}

};

// ���ַ�����ȫ���ֽ������ϼ���HASHֵ
class CHashFull
{
public:
	ULONG operator()(const wchar_t* nswString)
	{
		ULONG luHash = 0;
		int liLen = wcslen(nswString);
		wchar_t lcCrt;
		
		if (liLen > 255 || liLen < 0)
			liLen = 255;

		for (int liIndex = 0; liIndex < liLen; liIndex++)
		{
			if (nswString[liIndex] == 0)
				break;

			// ����HASHֵ
			lcCrt = nswString[liIndex];
			if((lcCrt|0x20) >= L'a' && (lcCrt|0x20) <= L'z')
				lcCrt |= 0x20;
			luHash += lcCrt;
			luHash = (luHash<<1) | ((luHash&0x010000)>>16);

		}

		return luHash;
	}

};


// �ַ����뺯����ַ���ڴ��е�pair�ṹ��
template<typename DataType>
class CPairNode{
public:
	ULONG		muHash;
	//ULONG		muStringLen;
	const wchar_t*	mswString;
//	AFX_MAPCALL		pfnFuncAddr;
	DataType mdUserData;

	__inline void operator=(const class CPairNode& src) {
		muHash = src.muHash;
		mswString = src.mswString;
//		pfnFuncAddr = src.pfnFuncAddr;
		mdUserData = src.mdUserData;
		//muStringLen = src.muStringLen;
	}

	CPairNode()
	{
		muHash = 0;
		//muStringLen = 0;
		mswString = NULL;
//		pfnFuncAddr = NULL;
	}

	bool SavePath(const wchar_t* nswString){
		if(nswString == NULL || nswString[0]==UNICODE_NULL)
			return true;
		int liSize = (int)wcslen(nswString)*sizeof(wchar_t);
		wchar_t* lpStr = new wchar_t[liSize];
		if(lpStr == NULL)
			return false;

		RtlCopyMemory(lpStr,nswString,liSize*sizeof(wchar_t));
		mswString = lpStr;

		return true;
	}
	void FreePath(void)	// ��һ�����������������ɾ���󣬲��ܵ��ñ������ͷŵ�������ڴ�
	{
		if((mswString)!=NULL)
		{
			delete mswString;
			mswString =NULL;
		}
	}

};

template<typename DataType>
class CMapNodeCriterion	// Ĭ�ϵ��ж�׼��
{
public:
	bool operator () (const CPairNode<DataType>& Obj1,const CPairNode<DataType>& Obj2)const // һ��Ҫ����������
	{
		// ������Ob1С�ڶ���Obj2ʱ������True�����򷵻�false
		return (Obj1.muHash < Obj2.muHash || (Obj1.muHash == Obj2.muHash && _wcsicmp(Obj1.mswString,Obj2.mswString)<0));
	}
};



// ����ά��pair���ϣ�pair{string, funcAddr}
// ��Ҫ��;������һ��string,�ܿ��ٵĲ��ҳ���Ӧ�ĺ�����ַ
// DataType���ܷŹ���������ݻ������������16���ֽڣ��ܵ�������Ҳ���˹��������1ǧ����Ԫ����


template<typename DataType,class CHashValue>
class CMapList
{
public:
	CMapList(){};
	~CMapList(){};

public:
	// ���һ��pair����
	// ��ʧ�ܣ��򷵻�false,ʧ�ܵ�ԭ���������Ϊ�Ѿ����ڣ������ڴ�������
	bool AddList(const wchar_t* nswString,DataType UserData);

	// ɾ��һ��pair����
	bool DelList(const wchar_t* nswString);

	// ����һ��pair����
	bool UpdataList(const wchar_t* nswString,DataType UserData);

	// ����ָ���Ľڵ��¼�Ƿ����
	// ����-1����ʾ�����ڣ�����ֵ����ʾ�ҵ���Ԫ�ؽڵ����
	int LookupRecord(const wchar_t* nswString);

	//// ͨ��������nswString,���ض�Ӧ�ĺ�����ַ
	//AFX_MAPCALL GetFuncAddr(const wchar_t* nswString);

	// ����û����ݣ���������ڽ��������õ�Ĭ��ֵ
	DataType GetUserData(const wchar_t* nswString,DataType Default);

	// ���ӳ����Ŀ����
	int GetCount(void){
		return moData.Size();
	}

	// ���niIndexָ������Ŀ��ӳ���ַ�����0 <= niIndex < GetCount()
	const wchar_t* GetMappedString(int niIndex){
		return moData[niIndex].mswString;
	}

	// ���niIndexָ������Ŀ��ӳ�����ݣ�0 <= niIndex < GetCount()
	DataType GetDataByIndex(int niIndex){
		return moData[niIndex].mdUserData;
	}

protected:

private:

	cmmSequence<CPairNode<DataType>,CMapNodeCriterion<DataType>> moData;	

	
};


template<typename DataType,class CHashValue>
int CMapList<DataType,CHashValue>::LookupRecord(
	const wchar_t* nswString
	)
{
	CPairNode<DataType> loPair;
	CHashValue	loHashValue;

	loPair.muHash = loHashValue.operator()(nswString);
	loPair.mswString = nswString;

	return moData.Find(loPair);
	// 	CHashValue loHashValue;
	// 	ULONG	luHash;
	// 	int		liIndex = -1;
	// 
	// 
	// 	luHash = loHashValue.operator()(nswString);
	// 
	// 	for (int liLoop = 0; liLoop< moData.Size(); liLoop++)
	// 	{
	// 		if (luHash == moData[liLoop].muHash && _wcsicmp(nswString, moData[liLoop].mswString)==0)
	// 		{
	// 			liIndex = liLoop;
	// 			break;
	// 		}
	// 	}
	// 
	// 	return liIndex;
}

template<typename DataType,class CHashValue>
bool CMapList<DataType,CHashValue>::AddList(
	const wchar_t* nswString,
	DataType UserData
	//AFX_MAPCALL npFuncAddr
	)
{

	CPairNode<DataType>	loPairNode;
	CHashValue	loHashValue;


	// �����жϣ��ýڵ��¼�Ƿ����,>=0��ʾ���ڣ���ֱ�ӷ���
	if(LookupRecord(nswString)>= 0)
		return false;

	loPairNode.muHash = loHashValue.operator()(nswString);
	//loPairNode.muStringLen = wcslen(nswString);
	//loPairNode.pfnFuncAddr = npFuncAddr;
	loPairNode.mdUserData = UserData;
	loPairNode.SavePath(nswString);

	return (moData.Insert(loPairNode)>=0);

}


template<typename DataType,class CHashValue>
bool CMapList<DataType,CHashValue>::DelList(
	const wchar_t* nswString
	)
{

	int liIndex = LookupRecord(nswString);
	if (liIndex < 0)
		return false;	// ����Ҫɾ��

	moData[liIndex].FreePath();
	moData.RemoveByIndex(liIndex);

	return true;
}


template<typename DataType,class CHashValue>
bool CMapList<DataType,CHashValue>::UpdataList(
	const wchar_t* nswString, 
	DataType UserData
	//	AFX_MAPCALL npFuncAddr
	)
{
	int liIndex = LookupRecord(nswString);
	if (liIndex < 0)	// ��ʾ�����ڸ�Ԫ�أ���ֱ�����
		return AddList(nswString, UserData);	

	CPairNode<DataType>	loPairNode;
	CHashValue	loHashValue;

	// ������֪Ԫ�أ�����ļ�¼
	moData[liIndex].muHash = loHashValue.operator()(nswString);// ???�Ƿ���Ҫ�޸�
	//moData[liIndex].muStringLen = wcslen(nswString);
	//moData[liIndex].pfnFuncAddr = npFuncAddr;
	moData[liIndex].mdUserData = UserData;
	moData[liIndex].FreePath();
	moData[liIndex].SavePath(nswString);

	return true;
}


// ����û����ݣ���������ڽ��������õ�Ĭ��ֵ
template<typename DataType,class CHashValue>
DataType CMapList<DataType,CHashValue>::GetUserData(const wchar_t* nswString,DataType Default)
{
	int liIndex = LookupRecord(nswString);
	if (liIndex < 0)	// ��ʾ�����ڸ�Ԫ��
		return Default;

	return moData[liIndex].mdUserData;
}



//// template���ڲ����ӣ������Ͷ���ʵ�ֱ�����һ����Ԫ�ļ���
//#include "mapList.cpp"
//

#endif

