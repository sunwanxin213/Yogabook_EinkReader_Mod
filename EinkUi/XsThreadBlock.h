/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#pragma once

#include "cmmIDSet.h"


class CXuiThreadBlock{
public:
	CXuiThreadBlock(){
	};
	~CXuiThreadBlock(){};

	//����һ����㣬����Ψһ�����ID
	ULONG AddBlock();

	//�������״��������ERESULT_BLOCK��ʾ�������δ��ɣ�����ֵ��ʾ�Ѿ����;
	ERESULT ReadBlockState(ULONG nuID);

	//�趨����״��
	void SetBlockState(ULONG nuID,ERESULT nuState);

	//ɾ��һ�����
	void RemoveBlock(ULONG nuID);

private:
	cmmIDSet<ERESULT> moBlocks;
	CExclusiveAccess moAccLock;
};


class CXuiModalState
{
public:
	IEinkuiIterator* mpTarget;
	ULONG muBlock;
	void operator=(const CXuiModalState& src){
		mpTarget = src.mpTarget;
		muBlock = src.muBlock;
	}
};

class CXuiModalStack
{
public:
	CXuiModalStack(){}
	~CXuiModalStack(){}

	void AddModal(const CXuiModalState& rModalState);

	bool GetTopModel(CXuiModalState& rState);

	void RemoveModal(IEinkuiIterator* npModelElement);

	// ���ص��Ǵ����BlockID
	ULONG GetBlockIDOfModal(IEinkuiIterator* npModelElement);

private:
	cmmVector<CXuiModalState,16,16> moStack;
	CExclusiveAccess moAccLock;

	int FindModal(IEinkuiIterator* npModelElement);
};