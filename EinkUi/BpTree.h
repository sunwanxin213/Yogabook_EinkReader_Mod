/* Copyright 2019 - present Lenovo */
/* License: COPYING.GPLv3 */

/*++

Module Name:

    BpTree.h

Abstract:

    This module implements the bplustree struct and operations.

--*/

#ifndef _BPTREE_H_
#define _BPTREE_H_

#ifdef KERNEL_CODE

#include "ntddk.h"

#else

#include "Windows.h"

#endif//KERNEL_CODE


/*/////////////////////////////////////////////////////////////////////////
// B+��ģ��ʹ��˵��
///////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
���ģ������������ģ�
	��ģ�������ʹ���߽����ݴ洢��B+���У��������ݹ������Ҵ������߲�ѯ
Ч�ʡ� B+�����ô����ݽڵ㣬���Լ�����ɢ�ڴ���䡣


//////////////////////////////////////////////////////////////////////////
����ʲô��B+���У�
	���Ա����û��Լ������C++����󣬻�����C++������ָ�롣

1. ���������
	�����ַ�ʽ�£�B+���Ľڵ��н��������������û�����ͨ�����������صġ�=���������
������B+���Ĳ����㷨���ṩ���ⲿ����Ķ���ֵ��B+���ڲ�����Ķ���
	�������ַ����ĺô��ܶ࣬���ڴ�����С�����ݣ����Լ���С�����ݱ�����ڴ���䡣

2. ��������ָ��
	B+���ڵ��б����ֻ�Ƕ����ָ�룬�����΢���Ӳ�ѯ�����ʱ�䡣��������
�洢λ�ñ������ⲿ�Ķ�������Ψһ�ķ���������ö�����ϵͳGDI���ݶ���ȡ�

//////////////////////////////////////////////////////////////////////////
�Ƚϱ�׼��
	ʹ�ñ�B+��ģ�壬�����ṩһ���Ƚϱ�׼�࣬����ʵ�ֶ�����������͵ıȽϡ���ο�
���ļ��е�CBpTreeCriterion��

//////////////////////////////////////////////////////////////////////////
���������
	���B+��������Ƕ������ö���������Ҫ���ء�=��������������Ҫ�Զ��ͷŶ�����
���ⲿ��Դ����Ҫ���һЩ�����ü���֮��ķ��������Բο�demo�������ز���
	
//////////////////////////////////////////////////////////////////////////
// B+����ʹ��
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
// ���ø߼�ƪ
//////////////////////////////////////////////////////////////////////////
1. �ṩ��򻯹��캯�������Ч��
2. Ϊ�����ṩһ�������Ĺؼ�ֵ
3. �������ùؼ�ֵ��Ψһʱ����ν���B+��




/////////////////////////////////////////////////////////////////////////*/



class CBpTreeCriterion	// Ĭ�ϵ��ж�׼��
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

class bplustree_node_base{
public:
	bplustree_node_base(){
		miEntryCount = 0;
	}
	~bplustree_node_base(){}
	bool mbIsLeaf;	// ��ʾ����Ҷ�ӽڵ㼴��ײ�Ľڵ�
	int	miEntryCount;	// ������
	bplustree_node_base* mpParents;	// ָ�򸸽ڵ�
	void* mpLowestObject;	// ָ����С���������ܴ��ڱ����У�Ҳ���ܴ���ĳ�������ӿ���
};
typedef bplustree_node_base* LPBPNODE;

#pragma pack(4)

template<class CBPlusTreeObjectClass,int ObjsInNode>
class bplustree_node : public bplustree_node_base
{
public:
	bplustree_node(){}
	~bplustree_node(){}
	CBPlusTreeObjectClass& operator [](int niIndex)
	{	// Ϊ�����Ч�ʣ��������֤�±��Ƿ�Խ��
		return mObjectsArray[niIndex];
	}
	CBPlusTreeObjectClass&  GetEntry(int niIndex)
	{
		return mObjectsArray[niIndex];
	}
	void Insert(CBPlusTreeObjectClass& nrObject,int niIndex);	//������һ������ָ��λ��
	void Remove(int niIndex);	// ɾ��ָ��λ�õĶ���
	bplustree_node<CBPlusTreeObjectClass,ObjsInNode>* Split(int niCount);		// �����ڵ��Ϊ���������ƶ�niCount�����ݵ�ǰһ���ڵ㣬���صĶ�������½ڵ㣬�½ڵ�λ�ڱ��ڵ�ǰ����ͬ���ڵ㽫ǰ�����Ӻã������������������ϼ��ڵ�
	void Combine(bplustree_node<CBPlusTreeObjectClass,ObjsInNode>* npWith);		// ��Ŀ��ڵ�����ݺϲ������ڵ㣬�����޸�ǰ������ָ�룬�����������Ŀ������ǰ����

	bplustree_node<CBPlusTreeObjectClass,ObjsInNode>* mpLastSibling;
	bplustree_node<CBPlusTreeObjectClass,ObjsInNode>* mpNextSibling;
protected:
	//bplustree_node
	CBPlusTreeObjectClass mObjectsArray[ObjsInNode];	// ��Ŷ��������
};

#pragma pack()

template<class CBPlusTreeObjectClass,class Criterion,int ObjsInNode>
class bplustree_iterator;


//////////////////////////////////////////////////////////////////////////
// ��һ�������������Ƕ�������ͣ��ڶ����������������ж�׼�򣬵�����������������B+����ÿһ���ڵ��п������ɵ��������������ֵ���Բ���С��3
template<class CBPlusTreeObjectClass,class Criterion=CBpTreeCriterion,int ObjsInNode=30>
class bplustree 
{
protected:
	LPBPNODE mpRoot;	// ע�⣬������Ҷ�ڵ㣬Ҳ���ܲ���
	bplustree_node<CBPlusTreeObjectClass,ObjsInNode>* mpFirstLeaf;
	bplustree_node<CBPlusTreeObjectClass,ObjsInNode>* mpLastLeaf;
	int miTotalLeaves;
	int miTotalNodes;
	int miTotalObject;

#ifdef KERNEL_CODE
	KSPIN_LOCK SpinLock;
	KIRQL SavedIrql;
#else
	CRITICAL_SECTION CriticalSection;
#endif//KERNEL_CODE


public:
	bplustree();
	~bplustree();
	typedef bplustree_iterator<CBPlusTreeObjectClass,Criterion,ObjsInNode> iterator;

	//////////////////////////////////////////////////////////////////////////
	// ��׼�ӿ�																//
	//////////////////////////////////////////////////////////////////////////

	// ��ȡ����Ȩ��
	void EnterExclusiveAccess(void)const;
	// �ͷŷ���Ȩ��
	void LeaveExclusiveAccess(void)const;

	// ȡ�����е����ݶ�����Ŀ
	int GetCount(void)const{ return miTotalObject;}

	// ���룬���سɹ����ʧ�ܵ�ԭ������Ǵ�����ͬ��ֵ�����ڴ����ʧ��
	bool Insert(
		CBPlusTreeObjectClass& nrObject		// ������Ķ���ע�⣬�ö�������ݿ��ܻ�Ӧ��Ӧ���� = ���������Ϊ���޸�
		);
	// ���룬���سɹ����ʧ�ܵ�ԭ������Ǵ�����ͬ��ֵ�����ڴ����ʧ�ܣ�ͬ����������ǣ������ó�������������Ȼ���������Ӧ���ܹ�֧��'='������������޷�����ͨ��
	bool Insert(
		const CBPlusTreeObjectClass& nrObject		// ������Ķ���ע�⣬�ö�������ݿ��ܻ�Ӧ��Ӧ���� = ���������Ϊ���޸�
		);

	// ɾ��
	bool Remove(
		const CBPlusTreeObjectClass& nrObject // ���ҵ��ؼ�ֵ��������ͬ�Ķ��󣬰���ɾ����
		);

	// ɾ��
	bool Remove(
		bplustree_iterator<CBPlusTreeObjectClass,Criterion,ObjsInNode>&  nrIterator // ���ҵ��ؼ�ֵ��������ͬ�Ķ��󣬰���ɾ����
		);

	// ��ѯ���Բ�ѯ����ķ��ʣ���һ�������ƣ������ý�����ٴε��ù������ɾ�����ܣ������ܵ��¸ղŻ�õĽ������ʧЧ�����ԣ�������ڲ�ѯ��ͷ��ʲ�ѯ�������ǰ������B+���Ĳ����ɾ���㷨
	iterator Find(
		const CBPlusTreeObjectClass& nrToFind, // ���ҵ��ؼ�ֵ��������ͬ�Ķ����ṩ�����������Ķ��󣬿���ֻ���ùؼ�ֵ�Ƚ�����Ҫ�ĳ�Ա
		iterator* npEqualOrBelow=NULL	// ������ӽ����Ҷ���ģ����ڻ��߽��Ȳ��Ҷ���С�Ķ���
		)const;

	// ȡ�ؼ�ֵ��С�Ķ���
	iterator Begin(void)const;
	// ȡ�ؼ�ֵ���Ķ���
	iterator ReverseBegin(void)const;
	// ��ʾ��Ч�Ķ���
	inline iterator End(void)const;
	// ֱ��ȡĳ��ֵ��ע�⣬���niIndex����ȫ����ֵ��Χ����������Ч�����ݣ����⣬�������ʵ���ϲ���������ֱ�Ӷ�ȡ��������ֵ����һ��Ҷ�ڵ��������ʱ�����������Ծ��Ч����Ҫ���ǣ��ʣ������ϣ��˳�����һ���������ݣ���ֱ��ʹ��iterator��++��--������
	inline CBPlusTreeObjectClass& operator[] (int niIndex);

	// ���ȫ������
	inline void Clear(void);

	void VerifyTree(LPBPNODE npNode)const;
	ULONG GetSizeOfLeaf(void)const;
	ULONG GetSizeOfBranch(void)const;

private:
	iterator mEndItr;
	Criterion mCriterion;

	// �ͷ�һ����֧�ϵ������¼��ڵ�
	void ReleaseEntriesInNode(
		LPBPNODE npNode
		);
	bplustree_node<CBPlusTreeObjectClass,ObjsInNode>* FindInternal(
		const CBPlusTreeObjectClass& nrToFind,	// ���ҵĲ��ն���
		int& Index	// ���ض��������ֵ
		)const;
	int FindInBranch(	// ��ĳ��֦�ڵ���Ѱ������С�ڻ����Ŀ��Ľڵ�
		bplustree_node<LPBPNODE,ObjsInNode>* npBranch,
		const CBPlusTreeObjectClass& nrToFind	// ���ҵĲ��ն���
		)const;
	int FindInLeaf(	// ��ĳ��Ҷ�ڵ���Ѱ������С�ڻ����Ŀ��Ľڵ�
		bplustree_node<CBPlusTreeObjectClass,ObjsInNode>* npLeaf,
		const CBPlusTreeObjectClass& nrToFind	// ���ҵĲ��ն���
		)const;
	bool InsertToLeaf(
		bplustree_node<CBPlusTreeObjectClass,ObjsInNode>* npLeaf,
		CBPlusTreeObjectClass& nrObject
		);
	bool InsertToBranch(
		bplustree_node<LPBPNODE,ObjsInNode>* npBranch,
		LPBPNODE npNewNode
		);
	inline void UpdateLowestValue(	// �����ɸýڵ�������ؽڵ����Сֵ��������
		bplustree_node<LPBPNODE,ObjsInNode>* npBranch
		);
	bool NewLevel(	// �ڸ��ڵ�λ�ã�������һ�㣻ʵ����Ϊ����һ��������Ϊ��������������һ���µĸ��ڵ�
		LPBPNODE npNodeAhead,
		LPBPNODE npNodeBehind
		);
	bool DeleteFromLeaf(
		bplustree_node<CBPlusTreeObjectClass,ObjsInNode>* npLeaf,
		const int niIndex
		);
	bool DeleteFromBranch(
		bplustree_node<LPBPNODE,ObjsInNode>* npBranch,
		LPBPNODE npValue	// ֵ����npValue�����ݶ���
		);

};

template<class CBPlusTreeObjectClass,class Criterion,int ObjsInNode>
class bplustree_iterator
{
	friend class bplustree<CBPlusTreeObjectClass,Criterion,ObjsInNode>;
public:
	bplustree_iterator(){
		mpCrtLeaf = NULL;
		miCrtIndex = 0;
	}
	bplustree_iterator(const bplustree_iterator<CBPlusTreeObjectClass,Criterion,ObjsInNode>& src){
		mpCrtLeaf = src.mpCrtLeaf;
		miCrtIndex = src.miCrtIndex;
	}
	CBPlusTreeObjectClass& operator*() {	// ���صĶ��󣬲�Ҫ����ʹ�á�=���������ֵ���𴦣�����п��ܵ�������������ݷ���ת�ƣ���ȷ���ܹ���ȫʹ�á�=�������
		return mpCrtLeaf->GetEntry(miCrtIndex);
	}

	CBPlusTreeObjectClass* operator->() {	// return pointer to class object
		return &mpCrtLeaf->GetEntry(miCrtIndex);
	}
	void operator++() {
		if(miCrtIndex >= mpCrtLeaf->miEntryCount-1)
		{
			mpCrtLeaf = mpCrtLeaf->mpNextSibling;
			miCrtIndex = 0;
		}
		else
			miCrtIndex++;
	}
	void operator++(int) { // postfix
		if(miCrtIndex >= mpCrtLeaf->miEntryCount-1)
		{
			mpCrtLeaf = mpCrtLeaf->mpNextSibling;
			miCrtIndex = 0;
		}
		else
			miCrtIndex++;
	}

	void operator--(){
		if(miCrtIndex <= 0)
		{
			mpCrtLeaf = mpCrtLeaf->mpLastSibling;
			if(mpCrtLeaf != NULL)
				miCrtIndex = mpCrtLeaf->miEntryCount - 1;
		}
		else
			miCrtIndex--;
	}
	void operator--(int){ //postfix
		if(miCrtIndex <= 0)
		{
			mpCrtLeaf = mpCrtLeaf->mpLastSibling;
			if(mpCrtLeaf != NULL)
				miCrtIndex = mpCrtLeaf->miEntryCount - 1;
		}
		else
			miCrtIndex--;
	}
	void operator+= (int niOff) {

		if(mpCrtLeaf == NULL)
			return;

		miCrtIndex+= niOff;

		while(miCrtIndex >= mpCrtLeaf->miEntryCount)
		{
			miCrtIndex -= mpCrtLeaf->miEntryCount;
			mpCrtLeaf = mpCrtLeaf->mpNextSibling;
			if(mpCrtLeaf == NULL)
			{
				miCrtIndex = 0;
				return;
			}
		}
	}

	void operator-= (int niOff) {

		if(mpCrtLeaf == NULL)
			return;

		miCrtIndex -= niOff;

		while(miCrtIndex < 0)
		{
			mpCrtLeaf = mpCrtLeaf->mpLastSibling;
			if(mpCrtLeaf == NULL)
			{
				miCrtIndex = 0;
				return;
			}
			miCrtIndex += mpCrtLeaf->miEntryCount;
		}
	}

	bplustree_iterator<CBPlusTreeObjectClass,Criterion,ObjsInNode> operator+ (int niOff)const {
		bplustree_iterator<CBPlusTreeObjectClass,Criterion,ObjsInNode> loReturn = *this;
		loReturn += niOff;
		return loReturn;
	}

	bplustree_iterator<CBPlusTreeObjectClass,Criterion,ObjsInNode> operator- (int niOff)const {
		bplustree_iterator<CBPlusTreeObjectClass,Criterion,ObjsInNode> loReturn = *this;
		loReturn -= niOff;
		return loReturn;
	}

	void operator =(const bplustree_iterator<CBPlusTreeObjectClass,Criterion,ObjsInNode>& src){
		mpCrtLeaf = src.mpCrtLeaf;
		miCrtIndex = src.miCrtIndex;
	}

	bool operator==(const bplustree_iterator<CBPlusTreeObjectClass,Criterion,ObjsInNode>& cmp) const
	{
		if(mpCrtLeaf == cmp.mpCrtLeaf && miCrtIndex == cmp.miCrtIndex)
			return true;
		return false;
	}
	bool operator!=(const bplustree_iterator<CBPlusTreeObjectClass,Criterion,ObjsInNode>& cmp) const
	{
		if(mpCrtLeaf != cmp.mpCrtLeaf || miCrtIndex != cmp.miCrtIndex)
			return true;
		return false;
	}
		
protected:
	bplustree_node<CBPlusTreeObjectClass,ObjsInNode>* mpCrtLeaf;
	int miCrtIndex;
	//bplustree<CBPlusTreeObjectClass,Criterion,ObjsInNode>* mpOwnerOfMe;
};



#if defined(DBG)||defined(_DEBUG)	// >>>>>>>>>>>>>>>>>>>

#ifdef KERNEL_CODE
#define BPASSERT(_X) {if(!(_X))DbgBreakPoint();}
#else
#define BPASSERT(_X) {if(!(_X))RaiseException(0x888888,0,0,0);}
#endif

#else
#define BPASSERT


#endif//  define(DBG) || define(_DEBUG) <<<<<<<<<<<<<<

#include "BpTree.cpp"





#endif//_BPTREE_H_
