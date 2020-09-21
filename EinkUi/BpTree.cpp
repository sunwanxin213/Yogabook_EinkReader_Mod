

template<class CBPlusTreeObjectClass,class Criterion,int ObjsInNode> bplustree<CBPlusTreeObjectClass,Criterion,ObjsInNode>::bplustree()
{
#ifdef KERNEL_CODE
	KeInitializeSpinLock(&SpinLock);
	SavedIrql = PASSIVE_LEVEL;
#else
	InitializeCriticalSection(&CriticalSection);
#endif//KERNEL_CODE

	mpRoot = NULL;
	mEndItr.mpCrtLeaf = NULL;
	mEndItr.miCrtIndex = 0;

	Clear();
}


template<class CBPlusTreeObjectClass,class Criterion,int ObjsInNode> bplustree<CBPlusTreeObjectClass,Criterion,ObjsInNode>::~bplustree()
{
	ReleaseEntriesInNode(mpRoot);
#ifndef KERNEL_CODE
	DeleteCriticalSection(&CriticalSection);
#endif
	mpRoot = NULL;
}


// ��ȡ����Ȩ��
template<class CBPlusTreeObjectClass,class Criterion,int ObjsInNode>
inline void bplustree<CBPlusTreeObjectClass,Criterion,ObjsInNode>::EnterExclusiveAccess(void)const{
#ifdef KERNEL_CODE
	KeAcquireSpinLock((PKSPIN_LOCK)&SpinLock,(PKIRQL)&SavedIrql);
#else
	EnterCriticalSection((LPCRITICAL_SECTION)&CriticalSection);
#endif//KERNEL_CODE

}
// �ͷŷ���Ȩ��
template<class CBPlusTreeObjectClass,class Criterion,int ObjsInNode>
inline void bplustree<CBPlusTreeObjectClass,Criterion,ObjsInNode>::LeaveExclusiveAccess(void)const{
#ifdef KERNEL_CODE
	KeReleaseSpinLock((PKSPIN_LOCK)&SpinLock,SavedIrql);
#else
	LeaveCriticalSection((LPCRITICAL_SECTION)&CriticalSection);
#endif//KERNEL_CODE

}


// �ͷ�һ����֧�ϵ������¼��ڵ�
template<class CBPlusTreeObjectClass,class Criterion,int ObjsInNode>
void bplustree<CBPlusTreeObjectClass,Criterion,ObjsInNode>::ReleaseEntriesInNode(LPBPNODE npNode)
{
	if(npNode->mbIsLeaf == false)	// ����Ҷ�ڵ�,��Ҫ�ͷ���������
	{
		bplustree_node<LPBPNODE,ObjsInNode>* lpBranch;
		lpBranch = (bplustree_node<LPBPNODE,ObjsInNode>*)npNode;

		for (int i=0;i<lpBranch->miEntryCount;i++)
		{
			ReleaseEntriesInNode(lpBranch->GetEntry(i));
		}
	}
	else
	{
		bplustree_node<CBPlusTreeObjectClass,ObjsInNode>* lpLeaf = (bplustree_node<CBPlusTreeObjectClass,ObjsInNode>*)npNode;
		for(int i=0;i<lpLeaf->miEntryCount;i++)
		{
			lpLeaf->GetEntry(i).~CBPlusTreeObjectClass();
		}
	}
	delete npNode;
}

// ���룬���سɹ����ʧ�ܵ�ԭ������Ǵ�����ͬ��ֵ�����ڴ����ʧ��
template<class CBPlusTreeObjectClass,class Criterion,int ObjsInNode>
bool bplustree<CBPlusTreeObjectClass,Criterion,ObjsInNode>::Insert(
	CBPlusTreeObjectClass& nrObject		// ������Ķ���ע�⣬�ö�������ݿ��ܻ�Ӧ��Ӧ���� = ���������Ϊ���޸�
	)
{
	int liIndex;
	bplustree_node<CBPlusTreeObjectClass,ObjsInNode>* lpLeaf;

	lpLeaf = FindInternal(nrObject,liIndex);
	if(lpLeaf ==NULL || liIndex < 0)
	{// Insert to the first leaf
		lpLeaf = mpFirstLeaf;
	}
	else
	if(mCriterion(nrObject,lpLeaf->GetEntry(liIndex))==false && mCriterion(lpLeaf->GetEntry(liIndex),nrObject)==false)
		return false;	// ����ֵ�Ѿ�����

	return InsertToLeaf(lpLeaf,nrObject);
}
// ���룬���سɹ����ʧ�ܵ�ԭ������Ǵ�����ͬ��ֵ�����ڴ����ʧ�ܣ�ͬ����������ǣ������ó�������������Ȼ���������Ӧ���ܹ�֧��'='������������޷�����ͨ��
template<class CBPlusTreeObjectClass,class Criterion,int ObjsInNode>
bool bplustree<CBPlusTreeObjectClass,Criterion,ObjsInNode>::Insert(
	const CBPlusTreeObjectClass& nrObject		// ������Ķ���ע�⣬�ö�������ݿ��ܻ�Ӧ��Ӧ���� = ���������Ϊ���޸�
	)
{
	CBPlusTreeObjectClass loConvert = nrObject;
	return Insert(loConvert);
}

// ɾ��
template<class CBPlusTreeObjectClass,class Criterion,int ObjsInNode>
bool bplustree<CBPlusTreeObjectClass,Criterion,ObjsInNode>::Remove(
	const CBPlusTreeObjectClass& nrObject // ���ҵ��ؼ�ֵ��������ͬ�Ķ��󣬰���ɾ����
	)
{
	bplustree_iterator<CBPlusTreeObjectClass,Criterion,ObjsInNode> itr;
	itr = Find(nrObject);
	return Remove(itr);
}
// ɾ��
template<class CBPlusTreeObjectClass,class Criterion,int ObjsInNode>
bool bplustree<CBPlusTreeObjectClass,Criterion,ObjsInNode>::Remove(
	bplustree_iterator<CBPlusTreeObjectClass,Criterion,ObjsInNode>&  nrIterator // ���ҵ��ؼ�ֵ��������ͬ�Ķ��󣬰���ɾ����
	)
{
	if(nrIterator != End())
		return DeleteFromLeaf(nrIterator.mpCrtLeaf,nrIterator.miCrtIndex);
	return false;
}

// ���ȫ������
template<class CBPlusTreeObjectClass,class Criterion,int ObjsInNode>
void bplustree<CBPlusTreeObjectClass,Criterion,ObjsInNode>::Clear(void)
{
	// ɾ��ȫ��
	if(mpRoot != NULL)
		ReleaseEntriesInNode(mpRoot);

	// ���³�ʼ��
	bplustree_node<CBPlusTreeObjectClass,ObjsInNode>* lpFirstLeaf;

	lpFirstLeaf = new bplustree_node<CBPlusTreeObjectClass,ObjsInNode>;
	if(lpFirstLeaf == NULL)
		return;

	lpFirstLeaf->mpLastSibling = NULL;
	lpFirstLeaf->mpNextSibling = NULL;
	lpFirstLeaf->mpParents = NULL;
	lpFirstLeaf->mpLowestObject = NULL;
	lpFirstLeaf->mbIsLeaf = true;

	mpRoot = lpFirstLeaf;
	mpFirstLeaf = mpLastLeaf = lpFirstLeaf;
	miTotalLeaves = 1;
	miTotalNodes = 0;
	miTotalObject = 0;
}

// ��ѯ���Բ�ѯ����ķ��ʣ���һ�������ƣ������ý�����ٴε��ù������ɾ�����ܣ������ܵ��¸ղŻ�õĽ������ʧЧ�����ԣ�������ڲ�ѯ��ͷ��ʲ�ѯ�������ǰ������B+���Ĳ����ɾ���㷨
template<class CBPlusTreeObjectClass,class Criterion,int ObjsInNode>
bplustree_iterator<CBPlusTreeObjectClass,Criterion,ObjsInNode> bplustree<CBPlusTreeObjectClass,Criterion,ObjsInNode>::Find(
	const CBPlusTreeObjectClass& nrToFind, // ���ҵ��ؼ�ֵ��������ͬ�Ķ����ṩ�����������Ķ��󣬿���ֻ���ùؼ�ֵ�Ƚ�����Ҫ�ĳ�Ա
	iterator* npEqualOrBelow=NULL	// ������ӽ����Ҷ���ģ����ڻ��߽��Ȳ��Ҷ���С�Ķ���
	)const
{
	int liIndex;
	iterator loFind;
	bplustree_node<CBPlusTreeObjectClass,ObjsInNode>* lpLeaf;

	lpLeaf = FindInternal(nrToFind,liIndex);
	if(lpLeaf !=NULL && liIndex >= 0 )
	{
		if(npEqualOrBelow != NULL)
		{
			npEqualOrBelow->mpCrtLeaf = lpLeaf;
			npEqualOrBelow->miCrtIndex = liIndex;
		}

		if(mCriterion(nrToFind,lpLeaf->GetEntry(liIndex))==false && mCriterion(lpLeaf->GetEntry(liIndex),nrToFind)==false)
		{
			loFind.mpCrtLeaf = lpLeaf;
			loFind.miCrtIndex = liIndex;
			return loFind;
		}
	}
	else
	if(npEqualOrBelow != NULL)
		*npEqualOrBelow = mEndItr;

	return mEndItr;
}

// ȡ�ؼ�ֵ��С�Ķ���
template<class CBPlusTreeObjectClass,class Criterion,int ObjsInNode>
bplustree_iterator<CBPlusTreeObjectClass,Criterion,ObjsInNode> bplustree<CBPlusTreeObjectClass,Criterion,ObjsInNode>::Begin(void)const
{
	iterator loFind;
	if(mpFirstLeaf->miEntryCount > 0)
	{
		loFind.mpCrtLeaf = (bplustree_node<CBPlusTreeObjectClass,ObjsInNode>*)mpFirstLeaf;
		loFind.miCrtIndex = 0;
		return loFind;
	}
	return mEndItr;
}

// ȡ�ؼ�ֵ���Ķ���
template<class CBPlusTreeObjectClass,class Criterion,int ObjsInNode>
bplustree_iterator<CBPlusTreeObjectClass,Criterion,ObjsInNode> bplustree<CBPlusTreeObjectClass,Criterion,ObjsInNode>::ReverseBegin(void)const
{
	iterator loFind;
	if(mpLastLeaf->miEntryCount > 0)
	{
		loFind.mpCrtLeaf = (bplustree_node<CBPlusTreeObjectClass,ObjsInNode>*)mpLastLeaf;
		loFind.miCrtIndex = mpLastLeaf->miEntryCount - 1;
		return loFind;
	}
	return mEndItr;
}

// ��ʾ��Ч�Ķ���
template<class CBPlusTreeObjectClass,class Criterion,int ObjsInNode>
inline bplustree_iterator<CBPlusTreeObjectClass,Criterion,ObjsInNode> bplustree<CBPlusTreeObjectClass,Criterion,ObjsInNode>::End(void)const
{
	return mEndItr;
}

// ֱ��ȡĳ��ֵ��ע�⣬���niIndex����ȫ����ֵ��Χ����������Ч�����ݻ����쳣�����⣬�������ʵ���ϲ���������ֱ�Ӷ�ȡ��������ֵ����һ��Ҷ�ڵ��������ʱ�����������Ծ��Ч����Ҫ���ǣ��ʣ������ϣ��˳�����һ���������ݣ���ֱ��ʹ��iterator��++��--������
template<class CBPlusTreeObjectClass,class Criterion,int ObjsInNode>
inline CBPlusTreeObjectClass& bplustree<CBPlusTreeObjectClass,Criterion,ObjsInNode>::operator[] (int niIndex)
{
	bplustree_iterator<CBPlusTreeObjectClass,Criterion,ObjsInNode> itr = Begin();
	itr += niIndex;
	return *itr;
}

template<class CBPlusTreeObjectClass,class Criterion,int ObjsInNode>
void bplustree<CBPlusTreeObjectClass,Criterion,ObjsInNode>::VerifyTree(LPBPNODE npNode)const
{
	int liIndex;
	if(npNode == NULL)
		npNode = mpRoot;

	if(npNode->mbIsLeaf != false)
	{
		bplustree_node<CBPlusTreeObjectClass,ObjsInNode>* lpLeaf = (bplustree_node<CBPlusTreeObjectClass,ObjsInNode>*)npNode;
		for (liIndex=0;liIndex<lpLeaf->miEntryCount-1;liIndex++)
		{
			if(mCriterion(lpLeaf->GetEntry(liIndex),lpLeaf->GetEntry(liIndex+1))==false)
				break;
		}
		BPASSERT(liIndex >= lpLeaf->miEntryCount-1);
		BPASSERT(lpLeaf->mpLowestObject == &lpLeaf->GetEntry(0));
		BPASSERT(lpLeaf->mpLastSibling==NULL || lpLeaf->mpLastSibling->miEntryCount <= ObjsInNode);
		BPASSERT(lpLeaf->mpLastSibling==NULL || lpLeaf->mpLastSibling->mpNextSibling == lpLeaf);
		BPASSERT(lpLeaf->mpNextSibling == NULL || lpLeaf->mpNextSibling->mpLastSibling == lpLeaf);
		BPASSERT(lpLeaf->mpNextSibling == NULL || lpLeaf->mpNextSibling->miEntryCount<= ObjsInNode && mCriterion(lpLeaf->GetEntry(lpLeaf->miEntryCount-1),lpLeaf->mpNextSibling->GetEntry(0))!= false);
	}
	else
	{
		bplustree_node<LPBPNODE,ObjsInNode>* lpBranch = (bplustree_node<LPBPNODE,ObjsInNode>*)npNode;
		for (liIndex=0;liIndex<lpBranch->miEntryCount;liIndex++)
		{
			BPASSERT(lpBranch->GetEntry(liIndex)->mpParents == lpBranch);
		}

		for (liIndex=0;liIndex<lpBranch->miEntryCount-1;liIndex++)
		{
			if(mCriterion(*(CBPlusTreeObjectClass*)(lpBranch->GetEntry(liIndex)->mpLowestObject),*(CBPlusTreeObjectClass*)(lpBranch->GetEntry(liIndex+1)->mpLowestObject))==false)
				break;
		}
		BPASSERT(liIndex >= lpBranch->miEntryCount-1);
		BPASSERT(lpBranch->mpLowestObject == lpBranch->GetEntry(0)->mpLowestObject);

		for (liIndex=0;liIndex<lpBranch->miEntryCount;liIndex++)
			VerifyTree(lpBranch->GetEntry(liIndex));
	}
}

template<class CBPlusTreeObjectClass,class Criterion,int ObjsInNode>\
ULONG bplustree<CBPlusTreeObjectClass,Criterion,ObjsInNode>::GetSizeOfLeaf(void)const
{
	return sizeof(bplustree_node<CBPlusTreeObjectClass,ObjsInNode>);
}

template<class CBPlusTreeObjectClass,class Criterion,int ObjsInNode>
ULONG bplustree<CBPlusTreeObjectClass,Criterion,ObjsInNode>::GetSizeOfBranch(void)const
{
	return sizeof(bplustree_node<LPBPNODE,ObjsInNode>);
}



// internal procedues

template<class CBPlusTreeObjectClass,class Criterion,int ObjsInNode>
bplustree_node<CBPlusTreeObjectClass,ObjsInNode>* bplustree<CBPlusTreeObjectClass,Criterion,ObjsInNode>::FindInternal(
	const CBPlusTreeObjectClass& nrToFind,	// ���ҵĲ��ն���
	int& Index	// ���ض��������ֵ
	)const
{
	int liIndex = -1;
	LPBPNODE lpNode = mpRoot;
	BPASSERT(mpRoot!=NULL);

	while (lpNode != NULL)
	{
		if(lpNode->mbIsLeaf != false)
		{
			liIndex = FindInLeaf((bplustree_node<CBPlusTreeObjectClass,ObjsInNode>*)lpNode,nrToFind);
			break;
		}
		liIndex = FindInBranch((bplustree_node<LPBPNODE,ObjsInNode>*)lpNode,nrToFind);
		if(liIndex < 0)
			break;
		lpNode = ((bplustree_node<LPBPNODE,ObjsInNode>*)lpNode)->GetEntry(liIndex);
	}

	if(liIndex < 0)
		return NULL;

	BPASSERT(liIndex < lpNode->miEntryCount);
	Index = liIndex;
	return (bplustree_node<CBPlusTreeObjectClass,ObjsInNode>*) lpNode;
}

template<class CBPlusTreeObjectClass,class Criterion,int ObjsInNode>
int bplustree<CBPlusTreeObjectClass,Criterion,ObjsInNode>::FindInBranch(	// ��ĳ��֦�ڵ���Ѱ������С�ڻ����Ŀ��Ľڵ�
	bplustree_node<LPBPNODE,ObjsInNode>* npBranch,
	const CBPlusTreeObjectClass& nrToFind	// ���ҵĲ��ն���
	)const
{
	int liTop,liBottom,liCrt;
	CBPlusTreeObjectClass* lpLowest;
	liTop = npBranch->miEntryCount -1 ;
	liBottom = 0;

	BPASSERT(npBranch->miEntryCount >0 || mCriterion(nrToFind,*((CBPlusTreeObjectClass*)(npBranch->mpLowestObject)))==false);

	while(liBottom <= liTop)
	{
		// �۰����
		liCrt = (liTop + liBottom)/2;
		// ȡ��ǰλ�ö�Ӧ�ڵ����Сֵ
		lpLowest = (CBPlusTreeObjectClass*)((npBranch->GetEntry(liCrt))->mpLowestObject);

		// ��С�ڵ�ǰֵ
		if(mCriterion(nrToFind,*lpLowest) == false)
		{
			// �Ƿ����
			if(mCriterion(*lpLowest,nrToFind) == false)
				return liCrt;
			liBottom = liCrt+1;
		}
		else
		{
			liTop = liCrt-1;
		}
	}

	return liTop;
}

template<class CBPlusTreeObjectClass,class Criterion,int ObjsInNode>
int bplustree<CBPlusTreeObjectClass,Criterion,ObjsInNode>::FindInLeaf(	// ��ĳ��֦�ڵ���Ѱ������С�ڻ����Ŀ��Ľڵ�
	bplustree_node<CBPlusTreeObjectClass,ObjsInNode>* npLeaf,
	const CBPlusTreeObjectClass& nrToFind	// ���ҵĲ��ն���
	)const
{
	int liTop,liBottom,liCrt;
	liTop = npLeaf->miEntryCount -1 ;
	liBottom = 0;

	BPASSERT(npLeaf->mpLowestObject==NULL || mCriterion(nrToFind,*((CBPlusTreeObjectClass*)(npLeaf->mpLowestObject)))==false || npLeaf == mpFirstLeaf);

	while(liBottom <= liTop)
	{
		// �۰����
		liCrt = (liTop + liBottom)/2;

		// ��С�ڵ�ǰֵ
		if(mCriterion(nrToFind,npLeaf->GetEntry(liCrt)) == false)
		{
			// �Ƿ����
			if(mCriterion(npLeaf->GetEntry(liCrt),nrToFind) == false)
				return liCrt;
			liBottom = liCrt+1;
		}
		else
		{
			liTop = liCrt-1;
		}
	}

	return liTop;
}


template<class CBPlusTreeObjectClass,class Criterion,int ObjsInNode>
bool bplustree<CBPlusTreeObjectClass,Criterion,ObjsInNode>::InsertToLeaf(
	bplustree_node<CBPlusTreeObjectClass,ObjsInNode>* npLeaf,
	CBPlusTreeObjectClass& nrObject
	)
{
	int liIndex;
	bool lbReval = false;

	BPASSERT(npLeaf!=NULL);

	// ���ȼ������ᵲ���ĸ�λ��
	liIndex = FindInLeaf(npLeaf,nrObject)+1;	// �������С�Ķ���ҪС����᷵��-1

	// ��ǰ��Ҷ�Ƿ��Ѿ����Է����������
	if(npLeaf->miEntryCount < ObjsInNode)
	{
		npLeaf->Insert(nrObject,liIndex);
		if(liIndex == 0)	// ��Ҫ����ȫ������Сֵ����
		{
			npLeaf->mpLowestObject = &npLeaf->GetEntry(0);
			UpdateLowestValue((bplustree_node<LPBPNODE,ObjsInNode>*)npLeaf->mpParents);
		}
		lbReval = true;
	}
	else	// ǰһ���ڵ��Ƿ��ܹ�����
	if(npLeaf->mpLastSibling!=NULL && npLeaf->mpLastSibling->miEntryCount < ObjsInNode)
	{	
		// ������������ǲ���С�ڱ��ڵ�����ж���
		if(liIndex <= 0)
		{
			// �ǵģ�����ֱ�Ӳ��뵽ǰһ���ڵ�ȥ����Ϊ��������㷨ʱ��ͬ�϶��ǽ������ݲ��뵽����С�Ľڵ��ϣ����ԣ�����ط�ͨ�����ᵽ�Ҳ����˵���˵�Ŀǰ��Ч
			npLeaf->mpLastSibling->Insert(nrObject,npLeaf->mpLastSibling->miEntryCount);
		}
		else
		{
			// �ƶ���С��һ�����ݵ�ǰ��ȥ
			npLeaf->mpLastSibling->Insert(npLeaf->GetEntry(0),npLeaf->mpLastSibling->miEntryCount);
			// ɾ�������ߵĶ���
			npLeaf->Remove(0);
			// �������ݲ��뵽��ǰ�ڵ㣬��Ϊ������һ���������ԣ��ղż����λ����Ҫ��ȥ1
			liIndex--;
			npLeaf->Insert(nrObject,liIndex);
			
			// ��Ҫ����ȫ������Сֵ����
			npLeaf->mpLowestObject = &npLeaf->GetEntry(0);
			UpdateLowestValue((bplustree_node<LPBPNODE,ObjsInNode>*)npLeaf->mpParents);
		}
		lbReval = true;
	}
	else	// ��һ���ڵ��Ƿ���Է���
	if(npLeaf->mpNextSibling != NULL && npLeaf->mpNextSibling->miEntryCount < ObjsInNode)
	{
		// ������������ǲ��Ǵ��ڱ��ڵ�����ж���
		if(liIndex >= ObjsInNode)
		{
			// �ǵģ�����ֱ�Ӳ��뵽��һ���ڵ�ȥ
			npLeaf->mpNextSibling->Insert(nrObject,0);
			npLeaf->mpNextSibling->mpLowestObject = &npLeaf->mpNextSibling->GetEntry(0);
			UpdateLowestValue((bplustree_node<LPBPNODE,ObjsInNode>*)npLeaf->mpNextSibling->mpParents);
		}
		else
		{
			// ��������һ������
			npLeaf->mpNextSibling->Insert(npLeaf->GetEntry(ObjsInNode-1),0);
			npLeaf->mpNextSibling->mpLowestObject = &npLeaf->mpNextSibling->GetEntry(0);
			UpdateLowestValue((bplustree_node<LPBPNODE,ObjsInNode>*)npLeaf->mpNextSibling->mpParents);

			npLeaf->Remove(ObjsInNode-1);
			// �������ݲ���
			npLeaf->Insert(nrObject,liIndex);
			if(liIndex == 0)	// ��Ҫ����ȫ������Сֵ����
			{
				npLeaf->mpLowestObject = &npLeaf->GetEntry(0);
				UpdateLowestValue((bplustree_node<LPBPNODE,ObjsInNode>*)npLeaf->mpParents);
			}
		}
		lbReval = true;
	}
	else	// ֻ�ܷ���ѱ��ڵ���
	{
		// ����Ϊ����
		bplustree_node<CBPlusTreeObjectClass,ObjsInNode>* lpNewLeaf = npLeaf->Split(ObjsInNode/2);

		if(lpNewLeaf == NULL)
			return false;	// ʧ�ܷ���

		if(liIndex <= ObjsInNode/2)
		{
			lpNewLeaf->Insert(nrObject,liIndex);
		}
		else
		{
			liIndex -= ObjsInNode/2;
			npLeaf->Insert(nrObject,liIndex);
		}
		lpNewLeaf->mpLowestObject = &lpNewLeaf->GetEntry(0);
		npLeaf->mpLowestObject = & npLeaf->GetEntry(0);
		UpdateLowestValue((bplustree_node<LPBPNODE,ObjsInNode>*)npLeaf->mpParents);

		// ����Ҷ�ڵ���뵽�ϼ��ķ�֧�ڵ���
		if(npLeaf->mpParents == NULL)	//  ������ڵ�
			lbReval = NewLevel(lpNewLeaf,npLeaf);
		else
			lbReval = InsertToBranch((bplustree_node<LPBPNODE,ObjsInNode>*)npLeaf->mpParents,lpNewLeaf);
		//�������ǰ�Ľڵ�����СҶ�ڵ㣬�޸���СҶ�ڵ��¼
		if(mpFirstLeaf == npLeaf)
			mpFirstLeaf = lpNewLeaf;
		miTotalLeaves++;

	}

	if(lbReval != false)
		miTotalObject++;
	return lbReval;
}


template<class CBPlusTreeObjectClass,class Criterion,int ObjsInNode>
bool bplustree<CBPlusTreeObjectClass,Criterion,ObjsInNode>::InsertToBranch(
	bplustree_node<LPBPNODE,ObjsInNode>* npBranch,
	LPBPNODE npNewNode
	)
{
	int liIndex;
	bool lbReval = false;

	BPASSERT(npBranch!=NULL);

	// ���ȼ������ᵲ���ĸ�λ��
	liIndex = FindInBranch(npBranch,*((CBPlusTreeObjectClass*)npNewNode->mpLowestObject))+1;	// �������С�Ķ���ҪС����᷵��-1

	// ��ǰ��Ҷ�Ƿ��Ѿ����Է����������
	if(npBranch->miEntryCount < ObjsInNode)
	{
		npBranch->Insert(npNewNode,liIndex);
		npNewNode->mpParents = npBranch;	// ���¸��ڵ�
		if(liIndex == 0)	// ��Ҫ����ȫ������Сֵ����
		{
			UpdateLowestValue(npBranch);
		}
		lbReval = true;
	}
	else	// ǰһ���ڵ��Ƿ��ܹ�����
	if(npBranch->mpLastSibling!=NULL && npBranch->mpLastSibling->miEntryCount < ObjsInNode)
	{	
		// ������������ǲ���С�ڱ��ڵ�����ж���
		if(liIndex <= 0)
		{
			// �ǵģ�����ֱ�Ӳ��뵽ǰһ���ڵ�ȥ
			npBranch->mpLastSibling->Insert(npNewNode,npBranch->mpLastSibling->miEntryCount);
			npNewNode->mpParents = npBranch->mpLastSibling;
		}
		else
		{
			// �ƶ���С��һ�����ݵ�ǰ��ȥ
			npBranch->mpLastSibling->Insert(npBranch->GetEntry(0),npBranch->mpLastSibling->miEntryCount);
			npBranch->GetEntry(0)->mpParents = npBranch->mpLastSibling;
			// ɾ�������ߵĶ���
			npBranch->Remove(0);
			// �������ݲ��뵽��ǰ�ڵ㣬��Ϊ������һ���������ԣ��ղż����λ����Ҫ��ȥ1
			liIndex--;
			npBranch->Insert(npNewNode,liIndex);
			npNewNode->mpParents = npBranch;	// ���¸��ڵ�

			// ��Ҫ����ȫ������Сֵ����
			UpdateLowestValue(npBranch);
		}
		lbReval = true;
	}
	else	// ��һ���ڵ��Ƿ���Է���
	if(npBranch->mpNextSibling != NULL && npBranch->mpNextSibling->miEntryCount < ObjsInNode)
	{
		// ������������ǲ��Ǵ��ڱ��ڵ�����ж���
		if(liIndex >= ObjsInNode)
		{
			// �ǵģ�����ֱ�Ӳ��뵽��һ���ڵ�ȥ��ͬ������Ҷ�ĺ�������ǰ�ڵ��λ���ƣ�����ط�Ҳ���ᱻ���á���Ϊ���½ڵ㶼�ǲ����ھɽڵ���ǰ���ѣ������ϼ��ڵ�ʱ�����ǲ��뵽����С�Ľڵ���
			// ������ֲ�������ݴ���Ŀǰȫ�����ݵĿ��ܣ����Ŀǰ����
			npBranch->mpNextSibling->Insert(npNewNode,0);
			npNewNode->mpParents = npBranch->mpNextSibling;
			UpdateLowestValue(npBranch->mpNextSibling);
		}
		else
		{
			// ��������һ������
			npBranch->mpNextSibling->Insert(npBranch->GetEntry(ObjsInNode-1),0);
			npBranch->GetEntry(ObjsInNode-1)->mpParents = npBranch->mpNextSibling;
			UpdateLowestValue(npBranch->mpNextSibling);

			npBranch->Remove(ObjsInNode-1);
			// �������ݲ���
			npBranch->Insert(npNewNode,liIndex);
			npNewNode->mpParents = npBranch;
			if(liIndex == 0)	// ��Ҫ����ȫ������Сֵ����
			{
				UpdateLowestValue(npBranch);
			}
		}
		lbReval = true;
	}
	else	// ֻ�ܷ���ѱ��ڵ���
	{
		// ����Ϊ����
		bplustree_node<LPBPNODE,ObjsInNode>* lpNewBranch = npBranch->Split(ObjsInNode/2);

		if(lpNewBranch == NULL)
			return false;	// ʧ�ܷ���

		if(liIndex <= ObjsInNode/2)
		{
			lpNewBranch->Insert(npNewNode,liIndex);
			npNewNode->mpParents = lpNewBranch;
		}
		else
		{
			liIndex -= ObjsInNode/2;
			npBranch->Insert(npNewNode,liIndex);
			npNewNode->mpParents = npBranch;
		}
		for(liIndex=0;liIndex<lpNewBranch->miEntryCount;liIndex++)
			lpNewBranch->GetEntry(liIndex)->mpParents = lpNewBranch;

		UpdateLowestValue(lpNewBranch);
		UpdateLowestValue(npBranch);

		// ���½ڵ���뵽�ϼ��ķ�֧�ڵ���
		if(npBranch->mpParents == NULL)	//  ������ڵ�
			lbReval = NewLevel(lpNewBranch,npBranch);
		else
			lbReval = InsertToBranch((bplustree_node<LPBPNODE,ObjsInNode>*)npBranch->mpParents,lpNewBranch);
		miTotalNodes++;
	}

	return lbReval;
}

template<class CBPlusTreeObjectClass,class Criterion,int ObjsInNode>
bool bplustree<CBPlusTreeObjectClass,Criterion,ObjsInNode>::DeleteFromLeaf(
	bplustree_node<CBPlusTreeObjectClass,ObjsInNode>* npLeaf,
	const int niIndex
	)
{
	bool lbReval = true;
	// ����ɾ�����������
	npLeaf->GetEntry(niIndex).~CBPlusTreeObjectClass();
	npLeaf->Remove(niIndex);
	if(niIndex == 0 && npLeaf->miEntryCount > 0)
	{
		npLeaf->mpLowestObject = &npLeaf->GetEntry(0);
		UpdateLowestValue((bplustree_node<LPBPNODE,ObjsInNode>*)npLeaf->mpParents);
	}

	// ���ýڵ��Ƿ��������������һ�������
	if(npLeaf->miEntryCount < ObjsInNode/2 && npLeaf != mpRoot)
	{
		if(npLeaf->mpLastSibling != NULL && npLeaf->mpLastSibling->miEntryCount <= ObjsInNode/2) // ͬǰһ���ڵ�ϲ�
		{
			bplustree_node<CBPlusTreeObjectClass,ObjsInNode>* lpAhead = npLeaf->mpLastSibling;
			lpAhead->Combine(npLeaf);
			lbReval = DeleteFromBranch((bplustree_node<LPBPNODE,ObjsInNode>*)npLeaf->mpParents,npLeaf);
			if(npLeaf == mpLastLeaf)
				mpLastLeaf = lpAhead;

			delete npLeaf;
			miTotalLeaves--;
		}
		else
		if(npLeaf->mpNextSibling != NULL && npLeaf->mpNextSibling->miEntryCount <= ObjsInNode/2) // ͬ��һ���ڵ�ϲ�
		{
			bplustree_node<CBPlusTreeObjectClass,ObjsInNode>* lpBehind = npLeaf->mpNextSibling;
			npLeaf->Combine(lpBehind);
			lbReval = DeleteFromBranch((bplustree_node<LPBPNODE,ObjsInNode>*)lpBehind->mpParents,lpBehind);
			if(lpBehind == mpLastLeaf)
				mpLastLeaf = npLeaf;
			delete lpBehind;
			miTotalLeaves--;
		}
		else
		if(npLeaf->mpLastSibling != NULL && npLeaf->mpLastSibling->miEntryCount > ObjsInNode/2) // ��ǰ��ڵ��һ������
		{
			npLeaf->Insert(npLeaf->mpLastSibling->GetEntry(npLeaf->mpLastSibling->miEntryCount-1),0);
			npLeaf->mpLastSibling->Remove(npLeaf->mpLastSibling->miEntryCount-1);
			npLeaf->mpLowestObject = &npLeaf->GetEntry(0);
			UpdateLowestValue((bplustree_node<LPBPNODE,ObjsInNode>*)npLeaf->mpParents);
		}
		else
		if(npLeaf->mpNextSibling != NULL && npLeaf->mpNextSibling->miEntryCount > ObjsInNode/2) // �Ӻ���ڵ��һ������
		{
			npLeaf->Insert(npLeaf->mpNextSibling->GetEntry(0),npLeaf->miEntryCount);
			npLeaf->mpNextSibling->Remove(0);
			npLeaf->mpNextSibling->mpLowestObject = &npLeaf->mpNextSibling->GetEntry(0);
			UpdateLowestValue((bplustree_node<LPBPNODE,ObjsInNode>*)npLeaf->mpNextSibling->mpParents);
		}
		else
		{
			// �����ܵ���
			BPASSERT(0);
		}
	}

	if(lbReval != false)
		miTotalObject--;
	return lbReval;
}

template<class CBPlusTreeObjectClass,class Criterion,int ObjsInNode>
bool bplustree<CBPlusTreeObjectClass,Criterion,ObjsInNode>::DeleteFromBranch(
	bplustree_node<LPBPNODE,ObjsInNode>* npBranch,
	LPBPNODE npValue	// ֵ����npValue�����ݶ���
	)
{
	int liIndex;
	bool lbReval = true;

	// �����ҵ����ֵ���ڵ�λ��
	for (liIndex = 0;liIndex < npBranch->miEntryCount;liIndex++)
	{
		if(npBranch->GetEntry(liIndex) == npValue)
			break;
	}
	if(liIndex >= npBranch->miEntryCount)
		return false;	// ������

	// ɾ�����������
	npBranch->Remove(liIndex);
	if(liIndex == 0 && npBranch->miEntryCount > 0)
	{
		UpdateLowestValue(npBranch);
	}

	// ���ýڵ��Ƿ��������������һ�������
	if(npBranch->miEntryCount < ObjsInNode/2)
	{
		// �ж��Ƿ��Ǹ��ڵ�
		if(npBranch == mpRoot)
		{
			if(npBranch->miEntryCount <= 1)
			{
				// ȥ�����ڵ㣬ֱ�ӽ��²�ڵ�������
				LPBPNODE lpLower = npBranch->GetEntry(0);
				lpLower->mpParents = NULL;
				delete mpRoot;
				mpRoot = lpLower;
				miTotalNodes--;
			}
			// else �޸ı�
		}
		else
		if(npBranch->mpLastSibling != NULL && npBranch->mpLastSibling->miEntryCount <= ObjsInNode/2) // ͬǰһ���ڵ�ϲ�
		{
			bplustree_node<LPBPNODE,ObjsInNode>* lpAhead = npBranch->mpLastSibling;
			lpAhead->Combine(npBranch);
			for (liIndex = 0;liIndex< lpAhead->miEntryCount;liIndex++)
			{
				if(lpAhead->GetEntry(liIndex)->mpParents != lpAhead)
					lpAhead->GetEntry(liIndex)->mpParents = lpAhead;
			}
			lbReval = DeleteFromBranch((bplustree_node<LPBPNODE,ObjsInNode>*)npBranch->mpParents,npBranch);
			delete npBranch;
			miTotalNodes--;
		}
		else
		if(npBranch->mpNextSibling != NULL && npBranch->mpNextSibling->miEntryCount <= ObjsInNode/2) // ͬ��һ���ڵ�ϲ�
		{
			bplustree_node<LPBPNODE,ObjsInNode>* lpBehind = npBranch->mpNextSibling;
			npBranch->Combine(lpBehind);
			for (liIndex = 0;liIndex< npBranch->miEntryCount;liIndex++)
			{
				if(npBranch->GetEntry(liIndex)->mpParents != npBranch)
					npBranch->GetEntry(liIndex)->mpParents = npBranch;
			}
			lbReval = DeleteFromBranch((bplustree_node<LPBPNODE,ObjsInNode>*)lpBehind->mpParents,lpBehind);
			delete lpBehind;
			miTotalNodes--;
		}
		else
		if(npBranch->mpLastSibling != NULL && npBranch->mpLastSibling->miEntryCount > ObjsInNode/2) // ��ǰ��ڵ��һ������
		{
			npBranch->Insert(npBranch->mpLastSibling->GetEntry(npBranch->mpLastSibling->miEntryCount-1),0);
			npBranch->GetEntry(0)->mpParents = npBranch;
			npBranch->mpLastSibling->Remove(npBranch->mpLastSibling->miEntryCount-1);
			UpdateLowestValue(npBranch);
		}
		else
		if(npBranch->mpNextSibling != NULL && npBranch->mpNextSibling->miEntryCount > ObjsInNode/2) // �Ӻ���ڵ��һ������
		{
			npBranch->Insert(npBranch->mpNextSibling->GetEntry(0),npBranch->miEntryCount);
			npBranch->GetEntry(npBranch->miEntryCount-1)->mpParents = npBranch;
			npBranch->mpNextSibling->Remove(0);
			UpdateLowestValue(npBranch->mpNextSibling);
		}
		else
		{
			// �����ܵ���
			BPASSERT(0);
		}
	}

	return lbReval;
}


template<class CBPlusTreeObjectClass,class Criterion,int ObjsInNode>
inline void bplustree<CBPlusTreeObjectClass,Criterion,ObjsInNode>::UpdateLowestValue(	// �����ɸýڵ�������ؽڵ����Сֵ��������
	bplustree_node<LPBPNODE,ObjsInNode>* npBranch
	)
{
	while(npBranch != NULL)
	{
		if(npBranch->mpLowestObject == npBranch->GetEntry(0)->mpLowestObject)
			break;
		npBranch->mpLowestObject = npBranch->GetEntry(0)->mpLowestObject;
		npBranch = (bplustree_node<LPBPNODE,ObjsInNode>*)npBranch->mpParents;
	}
}


template<class CBPlusTreeObjectClass,int ObjsInNode>
void bplustree_node<CBPlusTreeObjectClass,ObjsInNode>::Insert(CBPlusTreeObjectClass& nrObject,int niIndex)	//������һ������ָ��λ��
{
	int liBehind = miEntryCount>=ObjsInNode?ObjsInNode-1:miEntryCount;
	// �����Ķ���ȫ������һ��
	while(liBehind > niIndex)
	{
		mObjectsArray[liBehind] = mObjectsArray[liBehind-1];
		liBehind--;
	}
	// ���������
	mObjectsArray[niIndex] = nrObject;
	miEntryCount++;
}

template<class CBPlusTreeObjectClass,int ObjsInNode>
void bplustree_node<CBPlusTreeObjectClass,ObjsInNode>::Remove(int niIndex)	// ɾ��ָ��λ�õĶ���
{
	int liBehind = niIndex+1;

	// �����Ķ���ȫ��ǰ��һ��
	while(liBehind < miEntryCount)
	{
		mObjectsArray[liBehind-1] = mObjectsArray[liBehind];
		liBehind++;
	}
	miEntryCount--;
}

template<class CBPlusTreeObjectClass,int ObjsInNode>
bplustree_node<CBPlusTreeObjectClass,ObjsInNode>* bplustree_node<CBPlusTreeObjectClass,ObjsInNode>::Split(int niCount)		// �����ڵ��Ϊ���������ƶ�niCount�����ݵ�ǰһ���ڵ㣬���صĶ�������½ڵ㣬�½ڵ�λ�ڱ��ڵ�ǰ����ͬ���ڵ㽫ǰ�����Ӻã������������������ϼ��ڵ�
{
	int liIndex;
	bplustree_node<CBPlusTreeObjectClass,ObjsInNode>* lpAhead = new bplustree_node<CBPlusTreeObjectClass,ObjsInNode>;
	if(lpAhead == NULL)
		return NULL;

	BPASSERT(niCount < miEntryCount);

	lpAhead->mpParents = NULL;
	lpAhead->mbIsLeaf = mbIsLeaf;
	lpAhead->mpLastSibling = mpLastSibling;
	lpAhead->mpNextSibling = this;
	mpLastSibling = lpAhead;
	if(lpAhead->mpLastSibling != NULL)
		lpAhead->mpLastSibling->mpNextSibling = lpAhead;

	// ��������
	for(liIndex=0;liIndex<niCount;liIndex++)
	{
		lpAhead->mObjectsArray[liIndex] = mObjectsArray[liIndex];
	}
	lpAhead->miEntryCount = liIndex;
	lpAhead->mpLowestObject = &lpAhead->mObjectsArray[0];

	for(;liIndex<miEntryCount;liIndex++)
	{
		mObjectsArray[liIndex-niCount] = mObjectsArray[liIndex];
	}
	miEntryCount -= niCount;

	return lpAhead;
}

template<class CBPlusTreeObjectClass,int ObjsInNode>
void bplustree_node<CBPlusTreeObjectClass,ObjsInNode>::Combine(bplustree_node<CBPlusTreeObjectClass,ObjsInNode>* npWith)		// ��Ŀ��ڵ�����ݺϲ������ڵ㣬�����޸�ǰ������ָ�룬�����������Ŀ������ǰ����
{
	int liIndex;
	BPASSERT(mpNextSibling == npWith && (miEntryCount+npWith->miEntryCount) <= ObjsInNode);

	// ��������
	for(liIndex=0;liIndex<npWith->miEntryCount;liIndex++)
	{
		mObjectsArray[liIndex+miEntryCount] = npWith->mObjectsArray[liIndex];
	}
	miEntryCount += npWith->miEntryCount;

	mpNextSibling = npWith->mpNextSibling;
	if(mpNextSibling != NULL)
		mpNextSibling->mpLastSibling = this;

}

template<class CBPlusTreeObjectClass,class Criterion,int ObjsInNode>
bool bplustree<CBPlusTreeObjectClass,Criterion,ObjsInNode>::NewLevel(	// �ڸ��ڵ�λ�ã�������һ�㣻ʵ����Ϊ����һ��������Ϊ��������������һ���µĸ��ڵ�
	LPBPNODE npNodeAhead,
	LPBPNODE npNodeBehind
	)
{
	bplustree_node<LPBPNODE,ObjsInNode>* lpNewRoot = new bplustree_node<LPBPNODE,ObjsInNode>;
	if(lpNewRoot == NULL)
		return false;
	lpNewRoot->Insert(npNodeAhead,0);
	lpNewRoot->Insert(npNodeBehind,1);
	lpNewRoot->mpLowestObject = npNodeAhead->mpLowestObject;
	lpNewRoot->mpLastSibling = NULL;
	lpNewRoot->mpNextSibling = NULL;
	lpNewRoot->mpParents = NULL;
	lpNewRoot->mbIsLeaf = false;
	mpRoot = lpNewRoot;
	npNodeAhead->mpParents = mpRoot;
	npNodeBehind->mpParents = mpRoot;

	miTotalNodes++;
	return true;
}
