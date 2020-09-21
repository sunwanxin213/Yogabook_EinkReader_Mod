/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


/*++

Do not use this code until allowed by Ensurebit rights. This code is only allowed for Ensurebit Internal Developers.

Module Name:

    cmmstruct.h

Abstract:

    This module implements vector,sequence,multi-sequence,stack and queue.

--*/

#ifndef _CMMSTRUCT_H_
#define _CMMSTRUCT_H_

#if defined(KERNEL_CODE)||defined(NATIVE_CODE)
#include "ntddk.h"
#else
#include "Windows.h"
#endif

#if defined(DBG)||defined(_DEBUG)	// >>>>>>>>>>>>>>>>>>>

#if defined(KERNEL_CODE)||defined(NATIVE_CODE)
#define CMMASSERT(_X) {if(!(_X))DbgBreakPoint();}
#else
#define CMMASSERT(_X) {if(!(_X))RaiseException(0x888888,0,0,0);}
#endif
#else
#define CMMASSERT

#endif		//  define(DBG) || define(_DEBUG) <<<<<<<<<<<<<<





/////////////////////////////////////////////////////////////////////////*/
// ʸ������ģ�壬ע�⣬InitSize���ȵĳ�ʼ���齫ֱ�ӷ����ڱ�ģ�����ɵĶ�����
// ���е����ݶ��󣬱ض��Ǵ���һ�������洢���У����ԣ����ǵ�ÿ�β���ɾ�����µĴ����ڴ渴�ƣ����ʺ�����Ԫ�ع����Ŀ�ĺʹ洢���ڴ��Ԫ�أ�����256�ֽڣ������ֳ���Ԫ������1000�����ֳ�������100

template<class CCmmContent,int InitSize=16,int Increment=16>
class cmmVector
{
public:
	cmmVector(){
		miCapacity = InitSize;
		mpArray = moInitArray;
		miSize = 0;
	}
	~cmmVector(){
		if(mpArray != moInitArray && mpArray != NULL)
		{
			delete []mpArray;
			mpArray = NULL;
		}
	}

	// ���س�Ա����
	CCmmContent& operator [](int niIndex){
		return GetEntry(niIndex);
	}

	// ���س�Ա����
	CCmmContent&  GetEntry(int niIndex){
		CMMASSERT(niIndex >= 0 && niIndex<miSize);

		return mpArray[niIndex];
	}

	// ���ص�ǰ��ȫ����Ա�Ĵ洢��
	CCmmContent* GetBuffer(void){
		return mpArray;
	}

	// ���ȫ����Ա
	void Clear(void){
		if(mpArray != moInitArray)
			delete []mpArray;
		miCapacity = InitSize;
		mpArray = moInitArray;
		miSize = 0;
		//for (int i=0;i<InitSize;i++)
		//{	������ʽ��ʼ�����ǣ���Щ�洢��Ŀǰ��δ��״̬��һ�㳯���Ǹ�ֵ���������캯���ͻᱲ����
		//	new(&moInitArray[i])CCmmContent();
		//}
	}

	// ������Ч��Ա��Ŀ
	int Size(void){
		return miSize;
	}

	// ���ش洢����
	int Capacity(void){
		return miCapacity;
	}

	// ���ص�һ����Ա����
	CCmmContent& Front(void){
		CMMASSERT(miSize>0);
		return mpArray[0];
	}

	// �������һ����Ա����
	CCmmContent& Back(void){
		CMMASSERT(miSize>0);
		return mpArray[miSize-1];
	}

	// ����һ����Ա���󣬷���ʵ�ʲ����λ��
	int Insert(
		IN int niIndex,	// -1 or one above Size() indicate to append element, others is the location
		IN CCmmContent& nrObject
		);

	// ����һ����Ա���󣬷���ʵ�ʲ����λ��
	int Insert(
		IN int niIndex,	// -1 or one above Size() indicate to append element, others is the location
		IN const CCmmContent& nrObject
		){
		CCmmContent loNew = nrObject;
		return Insert(niIndex,loNew);
	}

	// ɾ��һ����Ա����ͨ���ƶ���Ա�����λ��
	bool RemoveByIndex(
		IN int niIndex	// -1 or one above Size() indicate the last element, others is the location
		);

	// ����һ��ͬ���ʸ��
	int Insert(
		IN int niIndex,	// -1 or one above Size() indicate to append element, others is the location
		IN const cmmVector<CCmmContent, InitSize, Increment>& srcVector
	);

	int operator = (const cmmVector<CCmmContent, InitSize, Increment>& srcVector) {
		Clear();

		miSize = srcVector.Size();

		ExpandCapacity(true, miSize);

		for (int i = 0; i < miSize; i++)
		{
			mpArray[i] = srcVector[i];
		}

		return miSize;
	}


protected:
	int miCapacity;
	int miSize;
	CCmmContent* mpArray;
	CCmmContent  moInitArray[InitSize+1];

	bool PrepareBuffer(bool nbIncrease){
		if(nbIncrease != false)
		{
			if(miSize+1 > miCapacity)
				return ExpandCapacity(true);
		}
		else
		{
			// �����ǰ��Ч���ݳ�Ա������С����������3/4������С����������ȥ3/2��������ʱ������
			if(miSize-1 < miCapacity - Increment - Increment/2 && miSize < (miCapacity*3)/4 && mpArray != moInitArray)
				return ExpandCapacity(false);
		}
		return true;
	}
	bool ExpandCapacity(bool nbIncrease,int ExpandTo=0);

};

template<class CCmmContent,int InitSize,int Increment>
bool cmmVector<CCmmContent,InitSize,Increment>::ExpandCapacity(bool nbIncrease, int ExpandTo)
{
	int liNewCap = (nbIncrease!=false)?miCapacity+Increment:miCapacity-Increment;

	if(ExpandTo != 0)
		liNewCap = ((ExpandTo + Increment - 1) / Increment)*Increment;

	CCmmContent* lpNewArray = NULL;
	if(liNewCap > InitSize)
	{
		lpNewArray = new CCmmContent[liNewCap];
	}
	else
	{
		CMMASSERT(mpArray != moInitArray);

		lpNewArray = moInitArray;
		liNewCap = InitSize;
		// ����˵��Ҫ����Щ��Ԫ����Ĭ�Ϲ��캯���������п���Ŀ�������û���ṩ��������غ��������ԣ��ǲ���֧�����ֵ��ù��캯���Ĳ�����
		// *** ��Ϊ�����placement new���ṩ��Ĭ�Ϲ��캯����Release Building�����κδ���
		// *** void* __cdecl operator new(size_t nSize,void* npObj){return npObj;}
		// *** ��ֹ���������־��棬ʵ������һ��������Ե����������������������
		// *** void __cdecl operator delete(void* nObj,void* npObj){}
		//for (int i=0;i<InitSize;i++)
		//{
		//	new(&moInitArray[i])CCmmContent();
		//}
	}

	if(lpNewArray == NULL)
		return false;

	for(int liIndex=0;liIndex < miSize;liIndex++)
	{
		lpNewArray[liIndex] = mpArray[liIndex];
	}

	if(mpArray != moInitArray)
		delete []mpArray;

	mpArray = lpNewArray;
	miCapacity = liNewCap;

	return true;
}

template<class CCmmContent,int InitSize,int Increment>
int cmmVector<CCmmContent,InitSize,Increment>::Insert(
	IN int niIndex,	// -1 or one above Size() indicate to append element, others is the location
	IN CCmmContent& nrObject
	)
{
	if(niIndex < 0 || niIndex >= miSize)
		niIndex = miSize;
	if(PrepareBuffer(true) == false)
		return -1;

	for(int liIndex=miSize;liIndex > niIndex; liIndex--)
	{
		mpArray[liIndex] = mpArray[liIndex-1];
	}
	mpArray[niIndex] = nrObject;
	miSize++;

	return niIndex;
}
template<class CCmmContent,int InitSize,int Increment>
bool cmmVector<CCmmContent,InitSize,Increment>::RemoveByIndex(
	IN int niIndex	// -1 or one above Size() indicate the last element, others is the location
	)
{
	if(miSize == 0)
		return false;

	if(niIndex < 0 || niIndex >= miSize)
		niIndex = miSize-1;

	if(PrepareBuffer(false) == false)
		return false;

	// �����Ķ���ȫ��ǰ��һ��
	for(int liBehind = niIndex;liBehind < miSize-1;liBehind++)
	{
		mpArray[liBehind] = mpArray[liBehind+1];
	}
	miSize--;

	return true;
}

// ����һ��ͬ���ʸ��
template<class CCmmContent, int InitSize, int Increment>
int cmmVector<CCmmContent, InitSize, Increment>::Insert(
	IN int niIndex,	// -1 or one above Size() indicate to append element, others is the location
	IN const cmmVector<CCmmContent, InitSize, Increment>& srcVector
)
{
	int newSize = miSize + srcVector.miSize;
	if (ExpandCapacity(1,newSize) == false)
		return 0;

	if (niIndex < 0 || niIndex >= miSize)
		niIndex = miSize;

	for (int liIndex = miSize-1,liDst = newSize-1; liIndex >= niIndex; liIndex--,liDst--)
	{
		mpArray[liDst] = mpArray[liIndex];
	}

	for (int liIndex = 0; liIndex < srcVector.miSize; liIndex++)
	{
		mpArray[liIndex+niIndex] = srcVector.mpArray[liIndex];
	}

	miSize = newSize;

	return miSize;
}

//////////////////////////////////////////////////////////////////////////
// ���������ڲ����ݽṹ������������������
template<class CCmmContent>
class cmmNodeOfFastList
{
public:
	cmmNodeOfFastList(){}
	~cmmNodeOfFastList(){}

	cmmNodeOfFastList<CCmmContent>& operator =(cmmNodeOfFastList<CCmmContent>& src){
		moData = src.moData;
		LastNode = src.LastNode;
		NextNode = src.NextNode;
		return *this;
	}

#pragma pack(4)
	LONG LastNode;	// ָ��ǰһ���ڵ㣬������׽ڵ㽫��ָ���Լ���Free node����Ϊ-1
	LONG NextNode;
#pragma pack()
	CCmmContent moData;
};

//////////////////////////////////////////////////////////////////////////
// �����������ʺϴ�Ź�������ݣ�32�ֽڣ������ʺϴ�Ź���Ľڵ㣨����32K������Ҫ���ڴ��С����ע��ָ��Ҳ��С���󣩣�Ƶ������ɾ�������ٱ�����ѯ������������ǲ��롢ɾ�����ǲ�ѯ��Ҫ������ͨ����ṹ��
// ԭ��ÿ������ڵ㲢���Ƕ���������ڴ棬���Ƕ����������һ��cmmVector�����У���ĳ���ڵ㱻ɾ���󣬽����ǰ�����������ժ�����ڴ������Ȼ��cmmVector��û���ͷţ������Ϳ��Ա���
// ������ɾ�ڵ㵼�µĴ����ڴ���������������һ������ʱ�����������������б������ң��ٶȻ�������ͨ����
// ע�⣺��ģ����Ĵ��������ʹ�õ�Index���������ڻ�ȡ��ָ��ĳ���ڵ�ģ������Ǵ���ĳ���ڵ��������е�ǰ�����С��0��ʾʧ�ܻ��ߴ��������
template<class CCmmContent,int InitSize=16,int Increment=16>
class cmmFastList:protected cmmVector<cmmNodeOfFastList<CCmmContent>,InitSize,Increment>
{
public:
	cmmFastList(){
		miValidNodes = 0;
		miFirstFree = -1;
		CMMASSERT(Increment != 0);

		cmmNodeOfFastList<CCmmContent> loNode;
		loNode.NextNode = loNode.LastNode = 0;
		cmmVector<cmmNodeOfFastList<CCmmContent>,InitSize,Increment>::Insert(0,loNode);
	}
	~cmmFastList(){}

	// ����һ���ڵ㣬����ֵ�ǲ���ڵ����������������Ǵ洢���ڲ������е�λ�ã������������е�ǰ����򣻷���ֵС�ڵ���0��ʾʧ��
	int Insert(
		int niNodeRef,	// ���뵽����ڵ�ǰ�����ֵ������ĳ�λ�õ���Ч�ڵ������
		CCmmContent& nrObject,
		bool nbBefore=false	// ���뵽ǰ���ߺ�
		);

	// ����һ���ڵ㣬����ֵ�ǲ���ڵ����������������Ǵ洢���ڲ������е�λ�ã������������е�ǰ����򣻷���ֵС�ڵ���0��ʾʧ��
	int Insert(
		int niNodeRef,	// ���뵽����ڵ�ǰ�����ֵ������ĳ�λ�õ���Ч�ڵ������
		const CCmmContent& nrObject,
		bool nbBefore=false	// ���뵽ǰ���ߺ�
		){
		CCmmContent loNew = nrObject;
		return Insert(niNodeRef,loNew,nbBefore);
	}

	// ɾ��һ���ڵ�
	bool Remove(
		int niNodeRef
		);

	// ���ȫ���ڵ�
	bool Clear(){return false;}
	 
	// ����������Ч��Ա��
	int Size(){
		return miValidNodes;
	}

	// �������ĵ�һ���ڵ��������Ȼ��ͨ��[]���������GetEntry����Ŀ��ڵ������
	int Front(void){
		return cmmVector<cmmNodeOfFastList<CCmmContent>,InitSize,Increment>::GetEntry(0).NextNode;
	}

	// �����������һ���ڵ��������Ȼ��ʹ��[]���������GetEntry����Ŀ��ڵ������
	int Back(void){
		return cmmVector<cmmNodeOfFastList<CCmmContent>,InitSize,Increment>::GetEntry(0).LastNode;
	}

	// �����һ���ڵ�
	int Next(int niNodeRef);

	// ���ǰһ���ڵ�
	int Previous(int niNodeRef);

	// ����ĳ���ڵ㣬���Ҫʹ�����������CCmmContent�����ṩoperator ==�����֧�֣����ڻ�������ϵͳ�ṩ��Ĭ�ϵ�֧�֣����Զ����������Ҫ������������
	// ע�⣺ĳ��CCmmContent�����������п��ܲ�Ψһ�������һ���ȼ۶��������Find
	int Find(
		const CCmmContent& nrObject,
		int niOver= 0	// �������������֮��(������niOver������ָ)���ң�Ĭ�ϴӵ�һλ��ʼ
		);

	// ���س�Ա����
	CCmmContent& operator [](
		int niIndex	// ��Ա����ֵ��ע�⣺���ֵ���ǳ�Ա�������е����򣬶��Ǵ洢�������е���ţ���������ͨ��Front Back Next Previous�Ⱥ�����õ�ֵ��������ֵ�û��Լ������趨��
		){
		return cmmVector<cmmNodeOfFastList<CCmmContent>,InitSize,Increment>::GetEntry(niIndex).moData;
	}

	// ���س�Ա����
	CCmmContent&  GetEntry(
		int niIndex	// ��Ա����ֵ��ע�⣺���ֵ���ǳ�Ա�������е����򣬶��Ǵ洢�������е���ţ���������ͨ��Front Back Next Previous�Ⱥ�����õ�ֵ��������ֵ�û��Լ������趨��
		){
		CMMASSERT(niIndex >= 0 && niIndex<miSize);
		return cmmVector<cmmNodeOfFastList<CCmmContent>,InitSize,Increment>::GetEntry(niIndex).moData;
	}
	
protected:
	int miValidNodes;
	int miFirstFree;

	// ����һ�����õĴ洢��Ԫ
	bool SaveFreeNode(int niNodeRef){
		cmmVector<cmmNodeOfFastList<CCmmContent>,InitSize,Increment>::GetEntry(niNodeRef).NextNode = miFirstFree;
		miFirstFree = niNodeRef;
		cmmVector<cmmNodeOfFastList<CCmmContent>,InitSize,Increment>::GetEntry(niNodeRef).LastNode = -1;	// ��ʾ���Ѿ���ɾ����

		if(miValidNodes < miCapacity/2 && mpArray != moInitArray)
			return RecoverDataBuffer();

		return true;
	}

	// �ָ�BufferΪС�ߴ�
	bool RecoverDataBuffer(){
		cmmNodeOfFastList<CCmmContent>* lpNewArray;
		int liNewCap = ((miValidNodes+Increment-1)/Increment)*Increment;	// ȡ��ӽ��ı���

		if(liNewCap > InitSize)
		{
			lpNewArray = new cmmNodeOfFastList<CCmmContent>[liNewCap];
		}
		else
		{
			lpNewArray = moInitArray;
			liNewCap = InitSize;
		}

		if(lpNewArray == NULL)
			return false;

		int liSrc = 0;	// ���ǵ�����ͷҲҪ���ƹ�ȥ
		int liDst = 0;

		do
		{
			lpNewArray[liDst].moData = cmmVector<cmmNodeOfFastList<CCmmContent>,InitSize,Increment>::GetEntry(liSrc).moData;
			lpNewArray[liDst].LastNode = liDst-1;
			lpNewArray[liDst].NextNode = liDst+1;

			liSrc = Next(liSrc);
			liDst++;
		}while(liSrc > 0);
		CMMASSERT(miValidNodes == liDst-1); // ����ͷ���ܼ�������

		miFirstFree = -1;

		lpNewArray[0].LastNode = liDst-1;
		lpNewArray[liDst-1].NextNode = 0;

		miSize = miValidNodes+1;

		if(mpArray != moInitArray)
			delete []mpArray;

		mpArray = lpNewArray;
		miCapacity = liNewCap;

		return true;
	}

};

template<class CCmmContent,int InitSize,int Increment>
int cmmFastList<CCmmContent,InitSize,Increment>::Insert(
	int niNodeRef,	// ���뵽����ڵ�ǰ�����ֵ������ĳ�λ�õ���Ч�ڵ������
	CCmmContent& nrObject,
	bool nbBefore	// ���뵽ǰ���ߺ�
	)
{
	int liSet;

	// ����ȷ�ϲ���Ĳο�λ���Ƿ���ȷ
	if(niNodeRef < 0 || niNodeRef >= miSize)
		return 0;	// ����ʧ��

	if(niNodeRef >= 0 && cmmVector<cmmNodeOfFastList<CCmmContent>,InitSize,Increment>::GetEntry(niNodeRef).LastNode < 0)
		return 0; // �ⲻ����Ч����ڵ������������ʧ��

	cmmNodeOfFastList<CCmmContent> loNode;
	loNode.moData = nrObject;

	// ȷ���Ƿ����δʹ�õĽڵ�
	if(miFirstFree < 0)
	{
		// Ŀǰû��δʹ�õĽڵ㣬ֱ���ڻ����������һ����¼
		liSet = cmmVector<cmmNodeOfFastList<CCmmContent>,InitSize,Increment>::Insert(-1,loNode);
		if(liSet < 0)
			return 0;	//ʧ��
	}
	else
	{
		liSet = miFirstFree;
		miFirstFree = cmmVector<cmmNodeOfFastList<CCmmContent>,InitSize,Increment>::GetEntry(liSet).NextNode;
	}

	if(nbBefore != false)
	{	// ���뵽�ο�λ��ǰ
		loNode.LastNode = cmmVector<cmmNodeOfFastList<CCmmContent>,InitSize,Increment>::GetEntry(niNodeRef).LastNode;
		loNode.NextNode = niNodeRef;

		cmmVector<cmmNodeOfFastList<CCmmContent>,InitSize,Increment>::GetEntry(liSet) = loNode;

		cmmVector<cmmNodeOfFastList<CCmmContent>,InitSize,Increment>::GetEntry(niNodeRef).LastNode = liSet;
		cmmVector<cmmNodeOfFastList<CCmmContent>,InitSize,Increment>::GetEntry(loNode.LastNode).NextNode = liSet;

	}
	else
	{
		// ���뵽�ο�λ�ú�
		loNode.LastNode = niNodeRef;
		loNode.NextNode = cmmVector<cmmNodeOfFastList<CCmmContent>,InitSize,Increment>::GetEntry(niNodeRef).NextNode;

		cmmVector<cmmNodeOfFastList<CCmmContent>,InitSize,Increment>::GetEntry(liSet) = loNode;

		cmmVector<cmmNodeOfFastList<CCmmContent>,InitSize,Increment>::GetEntry(loNode.NextNode).LastNode = liSet;
		cmmVector<cmmNodeOfFastList<CCmmContent>,InitSize,Increment>::GetEntry(niNodeRef).NextNode = liSet;
	}
	
	miValidNodes++;
	return liSet;
}

template<class CCmmContent,int InitSize,int Increment>
bool cmmFastList<CCmmContent,InitSize,Increment>::Remove(int niNodeRef)
{
	if(niNodeRef <= 0 || niNodeRef >= miSize)
		return false;

	if(cmmVector<cmmNodeOfFastList<CCmmContent>,InitSize,Increment>::GetEntry(niNodeRef).LastNode < 0)	// �Ѿ�ɾ����
		return true;

	cmmNodeOfFastList<CCmmContent> loCrt = cmmVector<cmmNodeOfFastList<CCmmContent>,InitSize,Increment>::GetEntry(niNodeRef);
	cmmVector<cmmNodeOfFastList<CCmmContent>,InitSize,Increment>::GetEntry(loCrt.LastNode).NextNode = loCrt.NextNode;
	cmmVector<cmmNodeOfFastList<CCmmContent>,InitSize,Increment>::GetEntry(loCrt.NextNode).LastNode = loCrt.LastNode;

	// �����ʧЧ�ĵ�Ԫ���浽�����б��У����ҳ�����С�ܴ洢�ռ�
	miValidNodes--;
	return SaveFreeNode(niNodeRef);
}

template<class CCmmContent,int InitSize,int Increment>
int cmmFastList<CCmmContent,InitSize,Increment>::Next(int niNodeRef)
{
	// niNodeRef < 0 ����һ��������ͬ�����������0����������׽ڵ�
	if(niNodeRef < 0 || niNodeRef >= miSize || cmmVector<cmmNodeOfFastList<CCmmContent>,InitSize,Increment>::GetEntry(niNodeRef).LastNode < 0)
		return 0;

	return cmmVector<cmmNodeOfFastList<CCmmContent>,InitSize,Increment>::GetEntry(niNodeRef).NextNode;
}

template<class CCmmContent,int InitSize,int Increment>
int cmmFastList<CCmmContent,InitSize,Increment>::Previous(int niNodeRef)
{
	if(niNodeRef <= 0 || niNodeRef >= miSize || cmmVector<cmmNodeOfFastList<CCmmContent>,InitSize,Increment>::GetEntry(niNodeRef).LastNode < 0) 
		return 0;

	return cmmVector<cmmNodeOfFastList<CCmmContent>,InitSize,Increment>::GetEntry(niNodeRef).LastNode;
}

template<class CCmmContent,int InitSize,int Increment>
int cmmFastList<CCmmContent,InitSize,Increment>::Find(
	const CCmmContent& nrObject,
	int niOver	// �������������֮����ң�Ĭ�ϴӵ�һλ��ʼ
	)
{
	if (niOver <= 0)
		niOver = Front();

	while(niOver > 0)
	{
		if(cmmVector<cmmNodeOfFastList<CCmmContent>,InitSize,Increment>::GetEntry(niOver).moData == nrObject && cmmVector<cmmNodeOfFastList<CCmmContent>,InitSize,Increment>::GetEntry(niOver).LastNode >= 0)
			break;

		niOver = Next(niOver);
	}

	if(niOver >= miSize)
		niOver = 0;

	return niOver;
}


//////////////////////////////////////////////////////////////////////////
// ��������������Ƚ���
class CSequenceCriterion	// Ĭ�ϵ��ж�׼��
{
public:
	bool operator () (const int& Obj1,const int& Obj2)const // һ��Ҫ����������
	{
		// ������Ob1С�ڶ���Obj2ʱ������True�����򷵻�false
		return (Obj1 < Obj2);
	}
	bool operator () (const void* Obj1,const void* Obj2)const // һ��Ҫ����������
	{
		// ������Ob1С�ڶ���Obj2ʱ������True�����򷵻�false
		return (Obj1 < Obj2);
	}
};
//////////////////////////////////////////////////////////////////////////
// �������飬���ʺϷ��ù�������ݣ�128���ֽڣ������ʺϷ��ù���Ķ������齫���ִ洢��Ϊ�������ʱ������
template<class CCmmContent,class Criterion=CSequenceCriterion,int InitSize=16,int Increment=16>
class cmmSequence : protected cmmVector<CCmmContent,InitSize,Increment>
{
public:
	cmmSequence(){}
	~cmmSequence(){}

	// ����һ����ֵ���ѱ���Ԫ�أ��������Ч�Ľṹ�ڶ�����������������Ϊ-1��û���ҵ�
	int Find(const CCmmContent& nrFind)
	{
		return InternalFind(nrFind,NULL);
	}

	// ���룬����-1��ʾʧ�ܣ������ʾ�����λ��
	int Insert(const CCmmContent& nrKeyObj)
	{
		CCmmContent loContent = nrKeyObj;
		return Insert(loContent);
	}
	// ���룬����-1��ʾʧ�ܣ������ʾ�����λ��
	int Insert(CCmmContent& nrKeyObj)
	{
		int liLower;
		int liIndex = InternalFind(nrKeyObj,&liLower);
		if(liIndex >= 0)
			return -1;

		liLower++;
		
		return cmmVector<CCmmContent,InitSize,Increment>::Insert(liLower,nrKeyObj);
	}
	// ���һ��Ԫ�ص���β����ֻ��ȷ�����Ԫ�ص���������β��������ô���ã������Ū������������У�������ͨ��ֻ���ڴӱ��浽�ļ��Ľṹ��װ�������ṹ������ֵ�Ǳ�Ԫ�ز����λ��
	int Push_Back(const CCmmContent& nrKeyObj)
	{
		CCmmContent loContent = nrKeyObj;
		return Push_Back(loContent);
	}
	// ���һ��Ԫ�ص���β����ֻ��ȷ�����Ԫ�ص���������β��������ô���ã������Ū������������У�������ͨ��ֻ���ڴӱ��浽�ļ��Ľṹ��װ�������ṹ������ֵ�Ǳ�Ԫ�ز����λ��
	int Push_Back(CCmmContent& nrKeyObj)
	{
		return cmmVector<CCmmContent,InitSize,Increment>::Insert(-1,nrKeyObj);
	}

	// ɾ�������سɹ����niIndex��ɾ���Ľڵ������
	bool RemoveByIndex(int niIndex)
	{
		return cmmVector<CCmmContent,InitSize,Increment>::RemoveByIndex(niIndex);
	}

	// ɾ�������سɹ����
	bool Remove(const CCmmContent& nrKeyObj)
	{
		return RemoveByIndex(Find(nrKeyObj));
	}

	CCmmContent& operator [](int niIndex)
	{
		return GetEntry(niIndex);
	}

	void Clear(void){
		cmmVector<CCmmContent,InitSize,Increment>::Clear();
	}

	// ��ȡ������
	int Size(void)
	{
		return cmmVector<CCmmContent,InitSize,Increment>::Size();
	}

	// �������������Ƿ�������ģ�ִ���ٶȺ����������ڵ��Ժͽ׶��Լ��
	bool SequenceTest(void){
		int liLower = 0;
		bool lbReval = true;

		while(liLower+1 < Size())
		{
			if(mCriterion(GetEntry(liLower),GetEntry(liLower+1)) == false)
			{
				lbReval = false;
				break;
			}
			liLower++;
		}
		return lbReval;
	}
protected:
	int InternalFind(const CCmmContent& nrFind,int* npLower)
	{
		int liTop,liBottom,liCrt;
		liTop = miSize-1;
		liBottom = 0;

		while(liBottom <= liTop)
		{
			// �۰����
			liCrt = (liTop + liBottom)/2;

			// ��С�ڵ�ǰֵ
			if(mCriterion(nrFind,GetEntry(liCrt)) == false)
			{
				// �Ƿ����
				if(mCriterion(GetEntry(liCrt),nrFind) == false)
					return liCrt;
				liBottom = liCrt+1;
			}
			else
				liTop = liCrt-1;
		}
		if(npLower != NULL)
			*npLower = liTop;
		return -1;
	}
	Criterion mCriterion;
};

//////////////////////////////////////////////////////////////////////////
// ��Ԫ�������飬�ܹ����ö��������������ȵ�Ԫ�ء����ʺϷ��ù�������ݣ�128���ֽڣ������ʺϷ��ù���Ķ������齫���ִ洢��Ϊ�������ʱ������
template<class CCmmContent,class Criterion=CSequenceCriterion,int InitSize=16,int Increment=16>
class cmmMultiSequence : public cmmSequence<CCmmContent,Criterion,InitSize,Increment>
{
public:
	// ���ҵ�һ���ȼ۵��ѱ���Ԫ�أ��������Ч�Ľṹ�ڶ�����������������Ϊ-1��û���ҵ�
	// ��ȡ�����ĵȼ�Ԫ�أ�ֱ��ʹ����������һö�ٲ������жϣ����ߵ��������NextEquivalent
	int Find(const CCmmContent& nrFind)
	{
		int liReval = InternalFind(nrFind,NULL);
		while(liReval > 0)
		{
			if(mCriterion(nrFind,GetEntry(liReval-1)) != false || mCriterion(GetEntry(liReval-1),nrFind)!=false)
				break;	// ���ǰһ���Ѿ����ȼۣ����˳�ѭ��
			liReval--;
		}
		return liReval;
	}

	// ���룬����-1��ʾʧ�ܣ������ʾ�����λ�ã�nbAhead!=falseʱ�����ö�����뵽�ȼ۵Ķ���֮ǰ����������ڵȼ۶���֮��
	int Insert(const CCmmContent& nrKeyObj,bool nbAhead=false)
	{
		CCmmContent loContent = nrKeyObj;
		return Insert(loContent,nbAhead);
	}
	// ���룬����-1��ʾʧ�ܣ������ʾ�����λ�ã�nbAhead!=falseʱ�����ö�����뵽�ȼ۵Ķ���֮ǰ����������ڵȼ۶���֮��
	int Insert(CCmmContent& nrKeyObj,bool nbAhead=false)
	{
		int liIndex;

		liIndex = Find(nrKeyObj);
		if(liIndex < 0)	// ���û����ȵ�ֵ����ȥ�ұ���С��ֵ
		{
			InternalFind(nrKeyObj,&liIndex);
			liIndex++;
		}
		else
		{
			if(nbAhead == false)
			{
				// �ҵ���ֵ֮ͬ��Ĳ���λ��
				while(NextEquivalent(liIndex++)>=0);
			}
		}

		return cmmVector<CCmmContent,InitSize,Increment>::Insert(liIndex,nrKeyObj);
	}

	// ȡ��һ���ȼ۵�Ԫ�أ�����-1��ʾû����
	int NextEquivalent(int niCurrent)
	{
		if(niCurrent < 0 || niCurrent+1 >= miSize)
			return -1;

		if(mCriterion(GetEntry(niCurrent),GetEntry(niCurrent+1)) == false && mCriterion(GetEntry(niCurrent+1),GetEntry(niCurrent))==false)
			return niCurrent+1;
		return -1;
	}

	// ɾ��ȫ���ȼ۵�Ԫ�أ����سɹ����
	bool Remove(const CCmmContent& nrKeyObj)
	{
		int liReval = Find(nrKeyObj);
		while(liReval >= 0 )
		{
			if(Remove(liReval)==false)
				return false;

			liReval = Find(nrKeyObj);
		}

		return true;
	}
	// ɾ�������سɹ����niIndex��ɾ���Ľڵ������
	bool RemoveByIndex(int niIndex)
	{
		return cmmSequence<CCmmContent,Criterion,InitSize,Increment>::RemoveByIndex(niIndex);
	}
};


//////////////////////////////////////////////////////////////////////////
// ��ջ�ṹ
template<class CCmmContent,int InitSize=16,int Increment=16>
class cmmStack :protected cmmVector<CCmmContent,InitSize,Increment>
{
public:
	cmmStack(){}
	~cmmStack(){}

	bool Push(const CCmmContent& nrObject){
		return cmmVector<CCmmContent,InitSize,Increment>::Insert(-1,nrObject)>=0;
	}
	CCmmContent& Top(void){
		return cmmVector<CCmmContent,InitSize,Increment>::Back();
	}
	void Pop(void){
		cmmVector<CCmmContent,InitSize,Increment>::RemoveByIndex(-1);
	}
	int Size(void) {
		return cmmVector<CCmmContent,InitSize,Increment>::Size();
	}
	// ���ȫ����Ա
	void Clear(void){
		cmmVector<CCmmContent,InitSize,Increment>::Clear();
	}

	// ���س�Ա����
	CCmmContent& operator [](int niIndex){
		return cmmVector<CCmmContent,InitSize,Increment>::GetEntry(niIndex);
	}

	// ���س�Ա����
	CCmmContent&  GetEntry(int niIndex){
		return cmmVector<CCmmContent,InitSize,Increment>::GetEntry(niIndex);
	}

};


//////////////////////////////////////////////////////////////////////////
// ���У����ʺϴ�Ź���(����32K)�ĳ�Ա��Ҳ���ʺϴ�Ź���(����16���ֽڣ��κ�ָ�����Ͷ�ֻ��4�ֽ�)�Ķ���
template<class CCmmContent,int InitSize=16,int Increment=16>
class cmmQueue:protected cmmFastList<CCmmContent,InitSize,Increment> {
public:
	cmmQueue(){};
	~cmmQueue(){};
	bool Push_Front(const CCmmContent& nrObject){
		return Insert(cmmFastList<CCmmContent,InitSize,Increment>::Front(),nrObject,true)>=0;
	}
	bool Push_Back(const CCmmContent& nrObject){
		return Insert(cmmFastList<CCmmContent,InitSize,Increment>::Back(),nrObject)>=0;
	}
	void Pop_Front(void){
		Remove(cmmFastList<CCmmContent,InitSize,Increment>::Front());
	}
	void Pop_Back(void) {
		Remove(cmmFastList<CCmmContent,InitSize,Increment>::Back());
	}
	CCmmContent& Front(void){
		return cmmFastList<CCmmContent,InitSize,Increment>::GetEntry(cmmFastList<CCmmContent,InitSize,Increment>::Front());
	}
	CCmmContent& Back(void){
		return cmmFastList<CCmmContent,InitSize,Increment>::GetEntry(cmmFastList<CCmmContent,InitSize,Increment>::Back());
	}
	// ��ȡ������
	int Size(void){
		return cmmFastList<CCmmContent,InitSize,Increment>::Size();
	}
	// ���ȫ����Ա
	void Clear(){
		cmmFastList<CCmmContent,InitSize,Increment>::Clear();
	}
};


//////////////////////////////////////////////////////////////////////////
// ���Ա�ͨ��4���ֽڵ���������ȡ���Ա��е�����ֵ
// 4�ֽ���������Ӣ����ĸ�����ֵķ�ʽ���磺'Att1'��ÿ�����Կ��Ա��������ֵ�������Ϊ1K
// InitSize�ǳ�ʼ���õĴ洢����С��������һ�����ԣ���Ҫ6���ֽڵ�ͷ��������ֵ���ȣ��κ�ʱ����Ҫ6���ֽڵĽ�����
template<int InitSize=16,int Increment=30>
class cmmAttributes:protected cmmVector<unsigned char,InitSize,Increment>
{
public:
	cmmAttributes()
	{
		memset(cmmVector<unsigned char,InitSize,Increment>::GetBuffer(), 0x0, cmmVector<unsigned char,InitSize,Increment>::Size());
	}
	~cmmAttributes(){}
public:

	template<typename Type>
	// ����һ�����ԣ����ʧ�ܿ����������Ѿ����ڻ����ڴ����ʧ��
	bool SetAttribute(
		ULONG nuIndex,	// ���������������ַ�����ʽ�ṩ���磺'Att1'
		const Type& rToSave,	// ����Ϊһ��ULONG��ֵ
		bool nbOverwrite=false	// ��������Ƿ��д������Ϊtrue��������ͬ���������Ծ͸�д����Ϊfalse��������ͬ�����ͷ���ʧ��
		){
			return SetAttributeItem(nuIndex,(unsigned char*)&rToSave,sizeof(Type));
	}

	// ɾ��һ�����ԣ����ʧ�ܿ�����δ���ڻ����ڴ����ʧ��
	bool DeleteAttribute(
		ULONG nuIndex	// ���������������ַ�����ʽ�ṩ���磺'Att1'
		);	


	// ����һ�����ԣ����ʧ�ܿ����������Ѿ����ڻ����ڴ����ʧ��
	bool SetAttributeItem(
		ULONG nuIndex,	// ���������������ַ�����ʽ�ṩ���磺'Att1'
		unsigned char* npBuffer,	// ����Ϊһ��Buffer��ֵ
		int niValueSize,
		bool nbOverwrite=false	// ��������Ƿ��д������Ϊtrue��������ͬ���������Ծ͸�д����Ϊfalse��������ͬ�����ͷ���ʧ��
		);


	template<typename Type>
	// ���һ�����ԣ����سɹ����
	bool GetAttribute(
		ULONG nuIndex,	// ���������������ַ�����ʽ�ṩ���磺'Att1'
		Type& nrValue
		){
			if(GetAttributeItem(nuIndex,(unsigned char*)&nrValue,sizeof(Type))!=sizeof(Type))
				return false;

			return true;
	}

	// ���һ�����ԣ����ػ�õ��ֽ�����С�ڵ���0��ʾʧ�ܻ��߲����ڸ�����
	int GetAttributeItem(
		ULONG nuIndex,	// ���������������ַ�����ʽ�ṩ���磺'Att1'
		unsigned char* npBuffer,
		int niBufSize	// �������Ĵ�С
		);


};


template<int InitSize,int Increment>
bool cmmAttributes<InitSize, Increment>::DeleteAttribute(
	ULONG nuIndex		// ���������������ַ�����ʽ�ṩ���磺'Att1'
	)	
{
#define ATTRIBUTE_ZERO_LEN	sizeof(ULONG)

	unsigned char* lpDataBuffer = NULL;		// ��ô洢���Ա�Ļ�����
	ULONG	luAttrNum = 0;					// ���Ը���
	USHORT	lsValueLen = 0;					// �Ѵ��ڵ�����ֵ���ܳ���
	USHORT	lsAttrValueLen = 0;
	int		liHitIndex = 0;					// ��������
	int		luDataSize = 0;					// �����б��ܵĻ���������
	bool	lbIsHit = false;
	int		liBase;
	char	lcData;

	lpDataBuffer = cmmVector<unsigned char,InitSize,Increment>::GetBuffer();
	luDataSize = cmmVector<unsigned char,InitSize,Increment>::Size();

	// ������������б�
	for (int liLoop = 0; liLoop < luDataSize; )
	{
		if ('0000' == *(ULONG*)&lpDataBuffer[liLoop])
			break;

		if (nuIndex == *(ULONG*)&lpDataBuffer[liLoop])
		{	
			liHitIndex = luAttrNum;
			lbIsHit = true;
		}
		luAttrNum++;
		liLoop += 4;
	}

	// ������ָ�����ԣ�ֱ�ӷ���
	if (!lbIsHit)
		return false;

	// ��ȡָ�����Ե�ֵ����,���Ȼ�ȡ�����Ǹ�λ�������Բ��õ���ȡֵ�ķ���
	liBase = luAttrNum*sizeof(ULONG) + (liHitIndex+1)*sizeof(USHORT) + ATTRIBUTE_ZERO_LEN;
	for (int liLoop = 0; liLoop < sizeof(USHORT); liLoop++)
	{
		liBase--; lcData = '0';

		lcData = cmmVector<unsigned char,InitSize,Increment>::GetEntry(liBase);
		lsAttrValueLen = lsAttrValueLen<<(liLoop*8);
		lsAttrValueLen |= lcData;

	}
	if (lsAttrValueLen <= 0)
		return false;


	// ����liHitIndex����֮ǰ���������ݵĳ���		
	liBase = luAttrNum*sizeof(ULONG) + ATTRIBUTE_ZERO_LEN;
	for (int liLoop = 0; liLoop< liHitIndex; liLoop++)
	{
		USHORT lsTmpLen = 0;
		// ��2�ֽ�ƴ��һ��USHORT,������Ҫע��little-endian����
		lsTmpLen |= cmmVector<unsigned char,InitSize,Increment>::GetEntry(liBase + liLoop*sizeof(USHORT)+1);
		lsTmpLen = lsTmpLen<<8;
		lsTmpLen |= cmmVector<unsigned char,InitSize,Increment>::GetEntry(liBase + liLoop*sizeof(USHORT)+0);

		lsValueLen += lsTmpLen;
	}

	// ɾ��ָ�����Ե�ֵ,����������Զ�ǰ�ƣ�������ͬһ����������ɾ������
	liBase = luAttrNum*sizeof(ULONG) + luAttrNum*sizeof(USHORT) + lsValueLen + ATTRIBUTE_ZERO_LEN;
	for (int liLoop = 0; liLoop < lsAttrValueLen; liLoop++)
	{
		cmmVector<unsigned char,InitSize,Increment>::RemoveByIndex(liBase);
	}

	// ɾ���洢���ȵ��ڴ浥Ԫ
	liBase = luAttrNum*sizeof(ULONG) + liHitIndex*sizeof(USHORT)+ ATTRIBUTE_ZERO_LEN;
	for (int liLoop = 0; liLoop < sizeof(USHORT); liLoop++)
	{
		cmmVector<unsigned char,InitSize,Increment>::RemoveByIndex(liBase);
	}

	// ɾ����������
	liBase = liHitIndex*sizeof(ULONG);
	for (int liLoop = 0; liLoop < sizeof(ULONG); liLoop++)
	{
		cmmVector<unsigned char,InitSize,Increment>::RemoveByIndex(liBase);
	}

	return true;

}


template<int InitSize,int Increment>
bool cmmAttributes<InitSize, Increment>::SetAttributeItem(
	ULONG nuIndex,				// ���������������ַ�����ʽ�ṩ���磺'Att1'
	unsigned char* npBuffer,	// ����Ϊһ��Buffer��ֵ
	int niValueSize,
	bool nbOverwrite=false		// ��������Ƿ��д������Ϊtrue��������ͬ���������Ծ͸�д����Ϊfalse��������ͬ�����ͷ���ʧ��
	)	
{

#define ATTRIBUTE_ZERO_LEN	sizeof(ULONG)

	unsigned char* lpDataBuffer = NULL;		// ��ô洢���Ա�Ļ�����
	ULONG	luAttrNum = 0;					// ���Ը���
	USHORT	lsValueLen = 0;					// �Ѵ��ڵ�����ֵ���ܳ���
	USHORT	lsAttrValueLen = 0;
	int		luDataSize = 0;					// �����б��ܵĻ���������
	bool	lbIsHit = false;
	int		liBase;
	char	lcData;


	lpDataBuffer = cmmVector<unsigned char,InitSize,Increment>::GetBuffer();
	luDataSize = cmmVector<unsigned char,InitSize,Increment>::Size();


	// ������������б�
	for (int liLoop = 0; liLoop < luDataSize; )
	{
		if ('0000' == *(ULONG*)&lpDataBuffer[liLoop])
			break;

		if (nuIndex == *(ULONG*)&lpDataBuffer[liLoop])
			lbIsHit = true;

		luAttrNum++;
		liLoop += 4;
	}

	// �Ѿ�����ָ�����ԣ�����и���
	if (lbIsHit)
	{
		// ��ɾ����Ȼ���½����൱�ڸ���
		DeleteAttribute(nuIndex);
		luAttrNum--;
	}


	liBase = luAttrNum*sizeof(ULONG) +ATTRIBUTE_ZERO_LEN;
	// ��������Ѵ��ڵ����Զ�Ӧ���������ݵĳ���		
	for (int liLoop = 0; liLoop< (int)luAttrNum; liLoop++)
	{
		USHORT lsTmpLen = 0;
		// ��2�ֽ�ƴ��һ��USHORT,������Ҫע��little-endian����
		lsTmpLen |= cmmVector<unsigned char,InitSize,Increment>::GetEntry(liBase + liLoop*sizeof(USHORT)+1);
		lsTmpLen = lsTmpLen<<8;
		lsTmpLen |= cmmVector<unsigned char,InitSize,Increment>::GetEntry(liBase + liLoop*sizeof(USHORT)+0);

		lsValueLen += lsTmpLen;
	}


	liBase = luAttrNum*sizeof(ULONG);
	// �鿴�Ƿ��ǵ�һ�����ԣ�����ǣ����������β��0
	if (liBase == 0)	
	{
		liBase += 4;
		for (int liLoop = 0; liLoop < sizeof(ULONG); liLoop++)
		{
			lcData= '0';
			cmmVector<unsigned char,InitSize,Increment>::Insert(liBase, lcData);
			liBase++;
		}
	}
	// �����µ�����ֵ	
	liBase = luAttrNum*sizeof(ULONG);	
	for (int liLoop = 0; liLoop < sizeof(ULONG); liLoop++)
	{
		lcData= '0';
		lcData= (UCHAR)(nuIndex>>(liLoop*sizeof(char)*8));
		cmmVector<unsigned char,InitSize,Increment>::Insert(liBase, lcData);
		liBase++;
	}
	luAttrNum++;

	liBase = luAttrNum*sizeof(ULONG) + (luAttrNum-1)*sizeof(USHORT) + ATTRIBUTE_ZERO_LEN;
	// ���������Եĳ���ֵ
	for (int liLoop = 0; liLoop < sizeof(USHORT); liLoop++)
	{
		lcData= '0';
		lcData= niValueSize>>(liLoop*sizeof(char)*8);
		cmmVector<unsigned char,InitSize,Increment>::Insert(liBase, lcData);
		liBase++;
	}


	liBase = luAttrNum*sizeof(ULONG) + luAttrNum*sizeof(USHORT) + ATTRIBUTE_ZERO_LEN + lsValueLen;
	// ������ֵ������
	for (int liLoop = 0; liLoop < niValueSize; liLoop++)
	{
		cmmVector<unsigned char,InitSize,Increment>::Insert(liBase, npBuffer[liLoop]);
		liBase++;
	}

	return true;

}



template<int InitSize,int Increment>
int cmmAttributes<InitSize, Increment>::GetAttributeItem(
	ULONG nuIndex,				// ���������������ַ�����ʽ�ṩ���磺'Att1'
	unsigned char* npBuffer,
	int niBufSize				// �������Ĵ�С
	)
{

#define ATTRIBUTE_ZERO_LEN	sizeof(ULONG)

	unsigned char* lpDataBuffer = NULL;		// ��ô洢���Ա�Ļ�����
	ULONG	luAttrNum = 0;					// ���Ը���
	USHORT	lsValueLen = 0;					// �Ѵ��ڵ�����ֵ���ܳ���
	USHORT	lsAttrValueLen = 0;
	int		liHitIndex = 0;					// ��������
	int		luDataSize = 0;					// �����б��ܵĻ���������
	bool	lbIsHit = false;
	int		liBase;
	char	lcData;

	lpDataBuffer = cmmVector<unsigned char,InitSize,Increment>::GetBuffer();
	luDataSize = cmmVector<unsigned char,InitSize,Increment>::Size();

	// ������������б�
	for (int liLoop = 0; liLoop < luDataSize; )
	{
		if ('0000' == *(ULONG*)&lpDataBuffer[liLoop])
			break;

		if (nuIndex == *(ULONG*)&lpDataBuffer[liLoop])
		{	
			liHitIndex = luAttrNum;
			lbIsHit = true;
		}
		luAttrNum++;
		liLoop += 4;
	}

	// ������ָ�����ԣ�ֱ�ӷ���
	if (!lbIsHit)
		return false;

	// ��ȡָ�����Ե�ֵ����,���Ȼ�ȡ�����Ǹ�λ�������Բ��õ���ȡֵ�ķ���
	liBase = luAttrNum*sizeof(ULONG) + (liHitIndex+1)*sizeof(USHORT) + ATTRIBUTE_ZERO_LEN;
	for (int liLoop = 0; liLoop < sizeof(USHORT); liLoop++)
	{
		liBase--; lcData= '0';
		
		lcData = cmmVector<unsigned char,InitSize,Increment>::GetEntry(liBase);
		lsAttrValueLen = lsAttrValueLen<<(liLoop*8);
		lsAttrValueLen |= lcData;
	}
	if (lsAttrValueLen <= 0)
		return 0;


	// ����liHitIndex����֮ǰ���������ݵĳ���		
	liBase = luAttrNum*sizeof(ULONG) + ATTRIBUTE_ZERO_LEN;
	for (int liLoop = 0; liLoop< liHitIndex; liLoop++)
	{
		USHORT lsTmpLen = 0;
		// ��2�ֽ�ƴ��һ��USHORT,������Ҫע��little-endian����
		lsTmpLen |= cmmVector<unsigned char,InitSize,Increment>::GetEntry(liBase + liLoop*sizeof(USHORT)+1);
		lsTmpLen = lsTmpLen<<8;
		lsTmpLen |= cmmVector<unsigned char,InitSize,Increment>::GetEntry(liBase + liLoop*sizeof(USHORT)+0);

		lsValueLen += lsTmpLen;
	}

	// ��ȡָ�����Ե�ֵ
	liBase = luAttrNum*sizeof(ULONG) + luAttrNum*sizeof(USHORT) + lsValueLen + ATTRIBUTE_ZERO_LEN;
	for (int liLoop = 0; liLoop < lsAttrValueLen; liLoop++)
	{
		npBuffer[liLoop] = cmmVector<unsigned char,InitSize,Increment>::GetEntry(liBase);
		liBase++;
	}

	return lsAttrValueLen;

}

#endif//_CMMSTRUCT_H_
