/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#pragma once

#include "time.h"


template<typename DataType>
class CIdsPaireNode{
public:
	ULONG ID;
	DataType mdUserData;

	void operator=(const class CIdsPaireNode& src) {
		ID = src.ID;
		mdUserData = src.mdUserData;
	}
};

template<typename DataType>
class CIdsPaireNodeCriterion	// Ĭ�ϵ��ж�׼��
{
public:
	bool operator () (const CIdsPaireNode<DataType>& Obj1,const CIdsPaireNode<DataType>& Obj2)const // һ��Ҫ����������
	{
		// ������Ob1С�ڶ���Obj2ʱ������True�����򷵻�false
		return (Obj1.ID < Obj2.ID);
	}
};


// ID�������㷨����������ɢ��ֵ�������ǳ�ͻ
class CIdsIDGenerator	// Ĭ�ϵ��ж�׼��
{
public:
	CIdsIDGenerator(){
		srand((unsigned int)time(0));
	}
	ULONG operator () (ULONG nuSeed)const
	{
		int liRand = rand();
		do 
		{
			nuSeed = ((ULONG)LOWORD(nuSeed)<<(liRand%24))|((ULONG)HIWORD(nuSeed)^(liRand&0xFFFF))+1;
		} while (nuSeed == 0);

		return nuSeed;
	}
};

//////////////////////////////////////////////////////////////////////////
// ID���������Զ�����ΨһID����������֮��Ӧ�����ݣ�����ָ���Լ���ID�����㷨��Ҳ����ʹ��Ĭ�ϵ�
template<typename DataType,class IDGenerator=CIdsIDGenerator,int InitSize=16,int Increment=16>
class cmmIDSet
{
public:
	// ����һ�����ݣ������·����ID������0��ʾʧ��
	ULONG SaveItem(const DataType& UserData){
		int liIndex;
		int i;
		CIdsPaireNode<DataType> loNode;
		union {
			const void* p;
			ULONG d;
		}ldValue;
		ldValue.p = &UserData;

		loNode.mdUserData = UserData;
		loNode.ID = loIDGenerator(ldValue.d);

		i = 0;
		liIndex = -1;
		do 
		{
			liIndex = moSet.Insert(loNode);
			if(liIndex >= 0)
				break;
			loNode.ID = loIDGenerator(loNode.ID);
		} while (++i < 10000);
		
		if(liIndex >=0)
			return moSet[liIndex].ID;

		return 0;
	}

	// ����һ�����ݣ�ָ��ʹ�õ�ID�������ID�Ѿ����ڣ�����0ʧ�ܣ����򷵻ش����ID
	ULONG SaveItem(const DataType& UserData,ULONG nuID){
		CIdsPaireNode<DataType> loNode;

		if(nuID == 0)
			return 0;

		loNode.mdUserData = UserData;
		loNode.ID = nuID;

		if(moSet.Insert(loNode)>=0)
			return nuID;
		return false;
	}

	// �޸�һ������
	bool UpdateItem(ULONG nuID,const DataType& UserData){
		int liIndex;
		CIdsPaireNode<DataType> loNode;

		loNode.ID = nuID;
		
		liIndex = moSet.Find(loNode);

		if(liIndex < 0)
			return false;

		moSet[liIndex].mdUserData = UserData;

		return true;
	}

	// ɾ��һ������
	bool DeleteItem(ULONG nuID){
		CIdsPaireNode<DataType> loNode;

		loNode.ID = nuID;

		return moSet.Remove(loNode);
	};

	// ���һ������
	DataType& GetItem(ULONG nuID,DataType& Default)	{
		int liIndex;
		CIdsPaireNode<DataType> loNode;

		loNode.ID = nuID;

		liIndex = moSet.Find(loNode);

		if(liIndex < 0)
			return Default;

		return moSet[liIndex].mdUserData;
	}

	// ���һ������
	const DataType& GetItem(ULONG nuID,const DataType& Default)	{
		DataType ldBridge = Default;
		return GetItem(nuID,ldBridge);
	}

	void Clear(void) {
		moSet.Clear();
	}

	//��ȡ��������
	int Size(void){
		return moSet.Size();
	}

	//�����������ض���
	DataType& GetItemByIndex(LONG nlIndex,DataType& Default){
		if(nlIndex < 0 || nlIndex > Size())
			return Default;

		return moSet[nlIndex].mdUserData;
	}

	//�����������ض���
	const DataType& GetItemByIndex(LONG nlIndex,const DataType& Default){
		DataType ldBridge = Default;

		return GetItemByIndex(nlIndex,ldBridge);
	}

	//�������ݻ�ȡ�����ݶ�Ӧ��ID
	ULONG GetIdByItem(DataType UserData,ULONG Default){
		int liIndex=0;
		for (liIndex=0;liIndex<Size();liIndex++)
		{
			if(moSet[liIndex].mdUserData == UserData)
				break;	//�ҵ���
		}

		if(liIndex > 0 && liIndex < Size())
			return moSet[liIndex].ID;
		else
			return Default;
	}

protected:
	cmmSequence<CIdsPaireNode<DataType>,CIdsPaireNodeCriterion<DataType>,InitSize,Increment> moSet;
	IDGenerator loIDGenerator;
};


