#include "stdafx.h"

#include "CommonHeader.h"
#include "XsMsgQueue.h"




//// ������Ϣ����������������Ϣ���η��뵽һ���µ���Ϣ������
//void CXuiMessageQueue::DispartMessage(
//	IN ULONG nuMsgID,
//	IN IEinkuiIterator* npItr,	// ��Ϣ�ķ���Ŀ���Ǵ˶������Ӷ���
//	OUT CXuiMessageQueue& rSaveTo
//	)
//{
//	CXuiMessage* lpMsgObj;
//	int liIndex;
//	int liPick;
//
//	rSaveTo.Clear();
//
//	moLock.Enter();
//
//	do 
//	{
//		if(Size()<=0)
//			break;
//		liIndex = CEsMessageFastList::Front();
//		while(liIndex > 0)
//		{
//			liPick = liIndex;
//			liIndex = CEsMessageFastList::Next(liIndex);
//
//			lpMsgObj = CEsMessageFastList::GetEntry(liPick);
//			if(lpMsgObj->muMsgID == nuMsgID &&( lpMsgObj->mpMsgDestItr == npItr || lpMsgObj->mpMsgDestItr->FindAncestor(npItr)!=false))
//			{
//				rSaveTo.Push_Back(lpMsgObj);
//				CEsMessageFastList::Remove(liPick);
//			}
//		}
//	} while (false);
//
//	moLock.Leave();
//}

// ��ȡ�ض���Ϣ
IEinkuiMessage* CXuiMessageQueue::GetMessage(
	IN ULONG nuMsgID,
	IN IEinkuiIterator* npItr	// ��Ϣ�ķ���Ŀ���Ǵ˶������Ӷ���
)
{
	CXuiMessage* lpMsgObj;
	int liIndex;
	int liPick;
	IEinkuiMessage* lpGot = NULL;


	moLock.Enter();

	__try
	{
		if (Size() > 0)
		{
			liIndex = CEsMessageFastList::Front();
			while (liIndex > 0)
			{
				liPick = liIndex;

				lpMsgObj = CEsMessageFastList::GetEntry(liPick);
				if (lpMsgObj->muMsgID == nuMsgID && lpMsgObj->mpMsgDestItr == npItr)
				{
					lpGot = lpMsgObj;
					CEsMessageFastList::Remove(liPick);
					break;
				}

				liIndex = CEsMessageFastList::Next(liIndex);
			}
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
	}

	moLock.Leave();

	return lpGot;
}


// ���ȫ����Ϣ
void CXuiMessageQueue::Clear(void)
{
	IEinkuiMessage* lpMsg;
	int liIndex;

	moLock.Enter();

	__try
	{
		while (CEsMessageFastList::Size() > 0)
		{
			liIndex = CEsMessageFastList::Front();
			if (liIndex >= 0)
			{
				lpMsg = CEsMessageFastList::GetEntry(liIndex);
				CEsMessageFastList::Remove(liIndex);
				lpMsg->Release();
			}
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
	}
	moLock.Leave();
}

bool CXuiMessageQueue::Push_Front(IEinkuiMessage* npMsg) {
	bool lbReval = false;

	moLock.Enter();

	__try
	{
		lbReval = Insert(CEsMessageFastList::Front(), dynamic_cast<CXuiMessage*>(npMsg), true) >= 0;

		if (miHistroy < Size())
		{
			miHistroy = Size();
			////Trace_int(27513,miHistroy);//��Ϣ��������
		}

	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
	}

	moLock.Leave();

	return lbReval;
}

bool CXuiMessageQueue::Push_Back(IEinkuiMessage* npMsg) {
	bool lbReval = false;

	moLock.Enter();

	__try
	{
		lbReval = Insert(CEsMessageFastList::Back(), dynamic_cast<CXuiMessage*>(npMsg)) >= 0;

		if (miHistroy < Size())
		{
			miHistroy = Size();
			////Trace_int(13738,miHistroy);//��Ϣ��������
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
	}

	moLock.Leave();

	return lbReval;
}

//bool CXuiMessageQueue::Push_Back_Reduce(IEinkuiMessage* npMsg){
//	bool lbReval;
//	IEinkuiIterator* lpSender;
//	IEinkuiIterator* lpReceiver;
//	PElMessage lpNextMsg;
//	int liIndex;
//	bool lbDel = false;
//	ULONG luMsgID;
//
//	moLock.Enter();
//
//	lpSender = npMsg->GetMessageSender();
//	lpReceiver = npMsg->GetDestination();
//	luMsgID = npMsg->GetMessageID();
//
//
//	// ��ǰ�����Ѱͬ������Ϣ�����ߺͷ����ߣ�Ȼ��ɾ�������ĵڶ�����Ϣ
//	for(liIndex = CEsMessageFastList::Front();liIndex > 0;liIndex = CEsMessageFastList::Next(liIndex))
//	{
//		lpNextMsg = CEsMessageFastList::GetEntry(liIndex);
//		if(lpNextMsg->GetMessageID() == luMsgID && lpNextMsg->GetMessageSender()==lpSender && lpNextMsg->GetDestination()==lpReceiver)
//		{
//			if(lbDel != false)
//			{
//				// ɾ����ǰ��Ϣ
//				CEsMessageFastList::RemoveByIndex(liIndex);
//
//				// ����������ˣ���������ԭ���������Ѿ�����3����3�����ϵ���ͬ��Ϣ
//				break;
//			}
//			else
//				lbDel = true;
//		}
//	}
//
//	lbReval = Insert(CEsMessageFastList::Back(),dynamic_cast<CXuiMessage*>(npMsg))>=0;
//
//	moLock.Leave();
//
//	if(miHistroy < Size())
//	{
//		miHistroy = Size();
//	}
//
//	return lbReval;
//}
//

IEinkuiMessage* CXuiMessageQueue::GetMessage(void) {
	IEinkuiMessage* lpMsg = NULL;
	int liIndex;

	moLock.Enter();
	__try
	{
		liIndex = CEsMessageFastList::Front();
		if (liIndex > 0)
		{
			lpMsg = CEsMessageFastList::GetEntry(liIndex);
			CEsMessageFastList::Remove(liIndex);
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
	}

	moLock.Leave();

	return lpMsg;
}

// ��ȡ������
int CXuiMessageQueue::Size(void) {
	int liSize;

	moLock.Enter();
	liSize = CEsMessageFastList::Size();
	moLock.Leave();

	return liSize;
}


//// ���ȫ��ԭʼ������Ϣ
//int CXuiMessageQueue::RemoveMessages(
//	unsigned short nusType,
//	unsigned short nusMajNum,
//	unsigned short nusMinNum,
//	unsigned long nusMask 	// combination of the values that they are defined below
//	)
//{
//	CXuiMessage* lpMsgObj;
//	int liIndex;
//	int liPick;
//	int liRemoved = 0;
//
//	if (nusMask == 0)
//		return liRemoved;
//
//	moLock.Enter();
//
//	__try
//	{
//		if (Size() <= 0)
//			__leave;
//		liIndex = CEsMessageFastList::Front();
//		while(liIndex > 0)
//		{
//			liPick = liIndex;
//			liIndex = CEsMessageFastList::Next(liIndex);
//
//			lpMsgObj = CEsMessageFastList::GetEntry(liPick);
//			if(lpMsgObj == NULL)
//				continue;
//
//			if((nusMask&XSMSG_REMOVE_TYPE)!=0 && LMSG_GET_TYPE(lpMsgObj->muMsgID)!=nusType)
//				continue;
//
//			if((nusMask&XSMSG_REMOVE_MAJ)!=0 && LMSG_GET_MJNUM(lpMsgObj->muMsgID)!=nusMajNum)
//				continue;
//
//			if((nusMask&XSMSG_REMOVE_MIN)!=0 && LMSG_GET_MNNUM(lpMsgObj->muMsgID)!=nusMinNum)
//				continue;
//
//			CEsMessageFastList::Remove(liPick);
//			lpMsgObj->Release();
//			liRemoved++;
//		}
//	}
//	__except (EXCEPTION_EXECUTE_HANDLER)
//	{
//		liRemoved = 0;
//	}
//
//	moLock.Leave();
//
//	return liRemoved;
//}

// �����ָ���Ķ�ʱ����Ϣ
int CXuiMessageQueue::RemoveTimerMessage(
	const IEinkuiIterator* npTarget,	// null for all targets
	unsigned long nuTimerID		// 0 for all timer-message sending to the target
)
{
	CXuiMessage* lpMsgObj;
	PSTEMS_TIMER lpTimer;
	int liIndex;
	int liPick;
	int liRemoved = 0;

	moLock.Enter();

	__try
	{
		if (Size() <= 0)
			__leave;
		liIndex = CEsMessageFastList::Front();
		while (liIndex > 0)
		{
			liPick = liIndex;
			liIndex = CEsMessageFastList::Next(liIndex);

			lpMsgObj = CEsMessageFastList::GetEntry(liPick);

			if (lpMsgObj->muMsgID != EMSG_TIMER)
				continue;

			if (npTarget != NULL && lpMsgObj->mpMsgDestItr != npTarget)
				continue;

			if (lpMsgObj->GetInputDataSize() == sizeof(STEMS_TIMER) || nuTimerID != 0)	// ���������������������һ���ж��ˣ�����ɾ������
			{
				lpTimer = (PSTEMS_TIMER)lpMsgObj->GetInputData();
				if (lpTimer != NULL)
					if (lpTimer->TimerID != nuTimerID)
						continue;
			}

			CEsMessageFastList::Remove(liPick);
			lpMsgObj->Release();
			liRemoved++;
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
	}


	moLock.Leave();

	return liRemoved;
}
