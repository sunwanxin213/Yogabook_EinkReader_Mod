#include "stdafx.h"

#include "CommonHeader.h"
#include "ElementImp.h"




#define LOSHORT(_X) ((LONG)(SHORT)LOWORD(_X))
#define HISHORT(_X) ((LONG)(SHORT)HIWORD(_X))

DEFINE_BUILTIN_NAME(CXelManager)




CXelManager::CXelManager()
{
	moIteratorRoot.gpElementManager = this;
	moIteratorRoot.mpElement = &moElementRoot;
	moIteratorRoot.mpParent = &moIteratorRoot;	// ��Ԫ�ص�������

	moIteratorVerification.Insert(&moIteratorRoot);

	mpDesktopIterator = NULL;
	mhMessageInserted = NULL;
	mpMouseFocus = NULL;
	mpActivatedElement = NULL;
	mpMouseMoveOn = NULL;
	mbDragging = false;
	mbHoverTest = false;
	mpKbFocus = NULL;
	mbXuiDragDrop = false;
	muTickOnTipShow = 0;
	mpTipOwner = NULL;
	mbTipHide = true;
	mbExitXui = false;
	mlCleanHumanInput = 0;

	mpDraggingElement = NULL;
	mpDragMsgReceiver = NULL;
	mpRecentAskedDrop = NULL;
	mpLastAccepter = NULL;
	mlMsgAllocated = 0;
	mbTrackMouse = false;

	mlProbeMode = -1;
	mlProbePass = 0;
	mdTopLeftInPanel = { 0.0f,0.0f };
}


CXelManager::~CXelManager()
{
	// ??? �ͷŵ�������
	if (mpDesktopIterator != NULL)
		mpDesktopIterator->KRelease();

	while (moFreeMessagePool.Size() > 0)
	{
		IEinkuiMessage* lpMsgIntf = moFreeMessagePool.Top();
		moFreeMessagePool.Pop();

		CMM_SAFE_RELEASE(lpMsgIntf);
	}
}

// ���ø�Ԫ�ص�Widget���ԣ���System Widget
void CXelManager::SetRootWidget(IXsWidget* npWidget)
{
	moIteratorLock.Enter();
	moIteratorRoot.mpWidget = npWidget;
	moIteratorLock.Leave();
}


ULONG CXelManager::InitOnCreate(void)
{
	mhMessageInserted = CreateSemaphore(NULL, 0, 0x7FFFFFFF, NULL);
	if (mhMessageInserted == NULL)
		return ERESULT_UNSUCCESSFUL;

	// ��������������Ϣ
	moIteratorRoot.SetFlags(EITR_FLAG_INIT);

	return ERESULT_SUCCESS;
}

// �ȴ���Ϣ����ڲ�������WaitForSingleObject�ȴ��������ֵͬWaitForSingleObjectһ��
ULONG CXelManager::WaitMessageReach(ULONG nuMilliseconds)
{
	return WaitForSingleObject(mhMessageInserted, nuMilliseconds);
}


// ��ϵͳ������ע��һ��Element�����ص���������ʧ�ܷ���NULL
// �ɹ����صĽӿڶ��󣬲�ʹ��ʱ��Ҫ�ͷ�
IEinkuiIterator* __stdcall CXelManager::RegisterElement(
	IN IEinkuiIterator* npParent,	// ��Ԫ�صĵ�����
	IN IXsElement* npElement,	// ��ע�����Ԫ��
	IN ULONG nuEID	// ��Ԫ�ص�EID����ͬһ����Ԫ���£�����Ԫ�ص�EID����Ψһ�����������EID��������=0��ϵͳ�Զ�����
)
{
	CXuiIterator* lpNewItr = NULL;
	IEinkuiIterator* lpReturn = NULL;
	ERESULT luResult = ERESULT_UNSUCCESSFUL;
	IXsWidget* lpCrtWidget;

	LockIterators();

	do
	{
		// ��鸸�������Ƿ�Ϸ�
		if (moIteratorVerification.Find(npParent) == moIteratorVerification.End())
			break;

		CXuiIterator* lpNewItr = CXuiIterator::CreateInstance();
		if (lpNewItr == NULL)
			break;

		lpCrtWidget = CEinkuiSystem::gpXuiSystem->GetCurrentWidget();
		lpNewItr->mpParent = dynamic_cast<CXuiIterator*>(npParent);
		lpNewItr->mpElement = npElement;
		lpNewItr->mpWidget = lpCrtWidget;
		lpNewItr->muEID = nuEID;
		lpNewItr->mlTabOrder = npElement->GetTabOrder();
		lpNewItr->mlZOrder = npElement->GetZOrder();

		CXuiIterator* lpFarther = dynamic_cast<CXuiIterator*>(npParent);

		// �Ȳ���β��
		if (ERESULT_FAILED(lpFarther->AddSubElement(lpNewItr)))
			break;

		// ������֤��
		moIteratorVerification.Insert(lpNewItr);

		// ����ǲ���Widget��homepage
		if (lpCrtWidget->GetHomePage() == NULL)
		{	// �ǵ�ǰWidgetע��ĵ�һ��Element����һ����homepage
			CXsWidget* lpWidgetObj = dynamic_cast<CXsWidget*>(lpCrtWidget);
			lpWidgetObj->SetHomePage(lpNewItr);
			lpNewItr->SetWidgetHomeFlag(true);
		}

		UnlockIterators();

		lpReturn = dynamic_cast<IEinkuiIterator*>(lpNewItr);

		// ���ͱ�������Ϣ
		luResult = CExMessage::PostMessage(lpNewItr, NULL, EMSG_CREATE, lpReturn, EMSG_POSTTYPE_REVERSE);

		LockIterators();

		// ���ɹ�
		if (luResult != ERESULT_SUCCESS)
			lpReturn = NULL;
		else
		{
			// ����ǲ��ǵ�һ���ͻ�Element������Ϊ����
			if (mpDesktopIterator == NULL)
			{
				//Trace_Point(27373);//����DeskTop Page
				mpDesktopIterator = dynamic_cast<CXuiIterator*>(lpReturn);
				mpDesktopIterator->KAddRefer();
			}
		}
	} while (false);

	UnlockIterators();

	CMM_SAFE_RELEASE(lpNewItr);

	return lpReturn;
}

// ��ϵͳ������ע��һ��Element���˹��ܽ�Ӧ�ñ���ע��Element�������XUIϵͳ���ã���������Ѿ�����
ERESULT __stdcall CXelManager::UnregisterElement(
	IN IEinkuiIterator* npElementIterator	// ��Ԫ�ض�Ӧ�ĵ�����
)
{
	return  ERESULT_UNSUCCESSFUL;
}

// ����Ԫ��
ERESULT CXelManager::DestroyElement(
	IN IEinkuiIterator* npElementIterator	// ��Ԫ�ض�Ӧ�ĵ�����
)
{
	CXuiIterator* lpFartherObj;
	CXuiIterator* lpToDestroy;

	if (npElementIterator == NULL || npElementIterator->IsKindOf(GET_BUILTIN_NAME(CXuiIterator)) == false)
		return ERESULT_ITERATOR_INVALID;

	// ���Ȼ�ȡ���ڵ�
	lpFartherObj = dynamic_cast<CXuiIterator*>(npElementIterator->GetParent());
	lpToDestroy = dynamic_cast<CXuiIterator*>(npElementIterator);

	// �����ڵ�����Ӹ��������Ƴ�
	if (lpFartherObj != NULL)
		lpFartherObj->RemoveSubElement((CXuiIterator*)npElementIterator);

	// ���ٱ�Ԫ�غ���Ԫ��
	DestroyElementSubTree(npElementIterator);

	//  ��DestroyElementSubTree�ڲ��ͷŵ�ǰ��npElementIterator

	return ERESULT_SUCCESS;
}


// �ݹ�����ĳ��Ԫ�أ��������ٸ�Ԫ�أ���������Ԫ��
void CXelManager::DestroyElementSubTree(IEinkuiIterator* npToDestroy)
{
	ERESULT luResult = ERESULT_SUCCESS;
	IEinkuiIterator* lpSubElement;
	CXuiIterator* lpToDestroy;

	lpToDestroy = dynamic_cast<CXuiIterator*>(npToDestroy);
	if (lpToDestroy == NULL)
		return;

	// ��Ŀ�귢��������Ϣ
	if (CExMessage::SendMessage(lpToDestroy, NULL, EMSG_DESTROY, CExMessage::DataInvalid) == ERESULT_ITERATOR_INVALID)
		return;

	// Iterator�Ƿ���Ч
	if (luResult == ERESULT_SUCCESS)
	{
		//// �ݹ����,ע��������Ԫ��

		// Modified by Archims Jul.8,2012 ԭ�����㷨����������
		// ���µķ����ݴ�����Ȼ���ߣ���������ͷŵĶ����ǵ�ǰ���㣬��Ȼ�ᵼ��UIϵͳ����
		// fixed by ax �п����Ѿ������������⣬���ڻ�û�����������֤����
		do
		{
			LockIterators();
			try
			{
				lpSubElement = lpToDestroy->GetSubElementByZOder(0);
			}
			catch (...)
			{
				lpSubElement = NULL;
				lpToDestroy = NULL;	//��֪��������ʲô���Ͳ�Ҫ����ʹ�����������
			}
			UnlockIterators();

			try
			{
				if (lpSubElement != NULL)
					lpSubElement->Close();
			}
			catch (...)
			{
				lpSubElement = NULL;
			}
		} while (lpSubElement != NULL);

	}

	//////////////////////////////////////////////////////////////////////////
	// ����Ĵ����������޸ģ�ֱ�Ӵ���֤����ɾ����Դ����������ش�������ٴν������ò�û��ɾ�����������û�л������֤����ɾ����
	//int EleRef = 0;
	//// ɾ������
	//if(lpToDestroy != NULL)
	//{
	//	lpToDestroy->SetAsDeleted();
	//	EleRef = lpToDestroy->KRelease();	// ��Iterator���ͷ�ʱ�������ͷ�Element����Ҳ���ͷŶԸ��ڵ������
	//}

	//if (EleRef == 0)
	//{
	//	LockIterators();
	//	// ����֤�����Ƴ�
	//	moIteratorVerification.Remove(lpToDestroy);
	//	UnlockIterators();
	//}
	// ɾ������
	if (lpToDestroy != NULL)
	{
		LockIterators();
		// ����֤�����Ƴ�
		moIteratorVerification.Remove(lpToDestroy);
		UnlockIterators();

		lpToDestroy->SetAsDeleted();
		lpToDestroy->KRelease();	// ��Iterator���ͷ�ʱ�������ͷ�Element����Ҳ���ͷŶԸ��ڵ������
	}

}


// ����һ��Iterator����Ϣ����
void CXelManager::StartMessageReceiver(IEinkuiIterator* npIterator)
{
	// �ҵ��Լ���Create����ȥִ�У���ôΪʲô���ȵ����ڲŷ���Create����Ҫ�ڴ�֮ǰ�ͷ���һ���в�ִ�е���Ϣ�أ���Ϊ�˷�ֹ�������������ö�©��Create��Ϣ
	IEinkuiMessage* lpMsg;

	lpMsg = moNormalMessages.GetMessage(EMSG_CREATE, npIterator);
	if (lpMsg != NULL)
	{
		ProcessNextMessage(lpMsg);
	}
}


// ��֤һ��Iterator�Ƿ���Ч������ERESULT_SUCCESS��Ч������ERESULT_ITERATOR_INVALID��Ч
ERESULT __stdcall CXelManager::VerifyIterator(
	IN IEinkuiIterator* npIterator	// ������
)
{
	ERESULT luResult;

	moIteratorLock.Enter();

	luResult = (moIteratorVerification.Find(npIterator) == moIteratorVerification.End()) ? ERESULT_ITERATOR_INVALID : ERESULT_SUCCESS;

	moIteratorLock.Leave();

	return luResult;
}


// �ڶ���������в���һ��Element�����ظ�Element�ĵ���������ʧ�ܷ���NULL
// �ɹ����صĽӿڶ��󣬲�ʹ��ʱ��Ҫ�ͷţ� ���������Ҫͨ��Ԫ�ػ������ע����������뱣���������ָ�룬��Ϊ���ñ�����ʹ��ȫ���������һ�ȡ���������󣬺�ʱ�ϴ�
IEinkuiIterator* __stdcall CXelManager::FindElement(
	IN IXsElement* npElement
)
{
	LockIterators();

	CXuiIterator* lpItrObj = moIteratorRoot.SeekIteratorInChild(npElement);

	UnlockIterators();

	return lpItrObj;
}

// ��ø�Ԫ�أ������Ϊ�˸����������������Ϣ��Ҳ����ֱ��ʹ��NULLָ���ʾ���������
// �ɹ����صĽӿڶ��󣬲�ʹ��ʱ��Ҫ�ͷ�
IEinkuiIterator* __stdcall CXelManager::GetRootElement(void)
{
	return &moIteratorRoot;
}

// ��ò˵�ҳ�����еĲ˵������������ҳ
IEinkuiIterator* __stdcall CXelManager::GetMenuPage(void)
{
	return NULL;
}

// ���ToolTipƽ�棬�����ߵ�ҳ
IEinkuiIterator* __stdcall CXelManager::GetToolTipPage(void)
{
	return NULL;
}


// �������Ԫ�أ�����Ԫ��������XUI����ʱָ��������Ԫ��ʵ�־���Ӧ�õ�ȫ�ֹ��ܣ��磺Idealife��ȫ��Ӧ����"Idealife"Ԫ�����ʵ���ṩ��
// ͨ��������Idealife��Ӧ����Ϣִ��Idealife��ϵͳ����
// �ɹ����صĽӿڶ��󣬲�ʹ��ʱ��Ҫ�ͷ�
IEinkuiIterator* __stdcall CXelManager::GetDesktop(void)
{
	return mpDesktopIterator;
}

// �����趨��Ԫ�أ�nbZTop==true������Zoder���㣬false�����ڵײ�
ERESULT __stdcall CXelManager::SetParentElement(IEinkuiIterator* npParent, IEinkuiIterator* npChild, bool nbZTop)
{
	CXuiIterator* lpMoveTo;
	CXuiIterator* lpMoveFrom;
	CXuiIterator* lpChild;
	ERESULT luResult = ERESULT_UNSUCCESSFUL;

	// ����Ƿ��ڲ����߳�
	if (CEinkuiSystem::gpXuiSystem->IsRunningInOperationThread() == false)
		return ERESULT_WRONG_THREAD;

	if (npParent->IsKindOf(GET_BUILTIN_NAME(CXuiIterator)) == false || npChild->IsKindOf(GET_BUILTIN_NAME(CXuiIterator)) == false)
		return ERESULT_ITERATOR_INVALID;

	lpMoveTo = dynamic_cast<CXuiIterator*>(npParent);
	lpMoveFrom = dynamic_cast<CXuiIterator*>(npChild->GetParent());
	lpChild = dynamic_cast<CXuiIterator*>(npChild);

	if (lpMoveFrom == NULL)
		return ERESULT_ELEMENT_INVALID;

	LockIterators();

	do
	{
		// ����ܷ�ӵ�ǰ�������Ƴ�
		// ����Ӷ����Ǹ��������Ч���������ƶ�
		if (lpMoveFrom->GetEnhancer() == npChild)
		{
			luResult = ERESULT_ACCESS_CONFLICT;
			break;
		}

		// �Ӹ������Ƴ�
		lpMoveFrom->RemoveSubElement(lpChild);

		// �����µĸ���������Ϊ����
		lpMoveTo->AddSubElement(lpChild);

		// ���
		luResult = ERESULT_SUCCESS;
	} while (false);

	UnlockIterators();

	if (ERESULT_SUCCEEDED(luResult))
		CEinkuiSystem::gpXuiSystem->UpdateView();

	return luResult;
}

// ����һ����Ϣ
// ���ص���Ϣ�����������Ҫ�ͷ�
IEinkuiMessage* __stdcall CXelManager::AllocateMessage(void)
{
	CXuiMessage* lpMsg = NULL;

	moFreeMessageLock.Enter();

	try
	{
		if (moFreeMessagePool.Size() > 0)
		{
			//if(Trace_Count(2,5)<5)
				//Trace_Point(29417);// �ӿ��гط�����Ϣ

			IEinkuiMessage* lpMsgIntf = moFreeMessagePool.Top();
			moFreeMessagePool.Pop();

			if (lpMsgIntf != NULL && (lpMsg = dynamic_cast<CXuiMessage*>(lpMsgIntf)))
			{
				lpMsg->AddRefer();
			}
		}
	}
	catch (...)
	{
	}

	moFreeMessageLock.Leave();

	if (lpMsg == NULL)
	{
		//if(Trace_Count(3,5)<5)
			//Trace_Point(24675);// �·�����Ϣ
		lpMsg = CXuiMessage::CreateInstance();
		lpMsg->AddRefer();

		InterlockedIncrement(&mlMsgAllocated);
		//if(mlMsgAllocated > 1000)
		//{
		//	IEinkuiMessage* lpNext;
		//	wchar_t lswText[MAX_PATH];
		//	int liIndex = moNormalMessages.Front();

		//	__asm int 3;

		//	for(;liIndex > 0;liIndex = moNormalMessages.Next(liIndex))
		//	{
		//		lpNext = moNormalMessages.GetEntry(liIndex);
		//		StringCchPrintf(lswText,MAX_PATH,L"Target=%s,Send=%s,Type=%8d,Maj=%02d,Min=%02d\n",
		//			lpNext->GetDestination()!=NULL?lpNext->GetDestination()->GetElementObject()->GetType():L"NULL",
		//			lpNext->GetMessageSender()!=NULL?lpNext->GetMessageSender()->GetElementObject()->GetType():L"NULL",
		//			LMSG_GET_TYPE(lpNext->GetMessageID()),
		//			LMSG_GET_MJNUM(lpNext->GetMessageID()),
		//			LMSG_GET_MNNUM(lpNext->GetMessageID()));

		//		OutputDebugString(lswText);
		//	}

		//}
	}

	return lpMsg;
}

// ����һ����Ϣ����ʼ����ز���
// ���ص���Ϣ�����������Ҫ�ͷ�
IEinkuiMessage* __stdcall CXelManager::AllocateMessage(
	IN ULONG nuMsgID,	// ��Ϣ����
	IN const void* npInputBuffer,	// �������ݵĻ�����
	IN int niInputSize,	// �������ݵĴ�С
	OUT void* npOutputBuffer,	// ���������(���ػ�����)
	IN int niOutputSize,	// �����������С
	IN bool nbInputVolatile	// ���뻺�����Ƿ�����ʧ�ģ��μ�IXuiMessage::SetInputData��ø�����Ϣ
)
{
	IEinkuiMessage* lpMsgIntf = AllocateMessage();
	CXuiMessage* lpMsg;

	if (lpMsgIntf == NULL || (lpMsg = dynamic_cast<CXuiMessage*>(lpMsgIntf)) == NULL)
		return NULL;

	lpMsg->SetMessageID(nuMsgID);
	if (npInputBuffer != NULL)
		lpMsg->SetInputData(npInputBuffer, niInputSize, nbInputVolatile);

	if (npOutputBuffer != NULL)
		lpMsg->SetOutputBuffer(npOutputBuffer, niOutputSize);

	return lpMsg;
}

// ����Ϣ��������ã��ͷſ���
void CXelManager::ReleaseMessage(IEinkuiMessage* npMsg)
{
	moFreeMessageLock.Enter();

	try
	{
		if (moFreeMessagePool.Size() < ELMGR_MAX_FREE_MESSAGE)
		{
			//if(Trace_Count(4,5)<5)
				//Trace_Point(31609);// ���浽���г�

			CXuiMessage* lpMsg = dynamic_cast<CXuiMessage*>(npMsg);

			lpMsg->Reuse();
			moFreeMessagePool.Push(npMsg);

			npMsg = NULL;
		}
	}
	catch (...)
	{
	}
	moFreeMessageLock.Leave();

	if (npMsg != NULL)
	{
		//if(Trace_Count(5,5)<5)
			//Trace_Point(31092);// �ͷ���Ϣ

		npMsg->Release();
		InterlockedDecrement(&mlMsgAllocated);
	}
}


// ��ָ��Ԫ�ط���һ����Ϣ������ģʽ��Send
ERESULT __stdcall CXelManager::SendMessage(
	IEinkuiIterator* npDestElement,	// ������Ϣ��Ŀ��Ԫ��
	IEinkuiMessage* npMsg
)
{
	ERESULT luResult = ERESULT_UNSUCCESSFUL;

	//if(mbExitXui != false && npMsg->GetMessageID() != EMSG_QUIT_XUI) discarded by AX 2013.1.21 check it only if is running in non-ui thread
	//	return ERESULT_UNEXPECTED_CALL;

	if (npMsg == NULL || npMsg->IsKindOf(GET_BUILTIN_NAME(CXuiMessage)) == false)
		return ERESULT_ELEMENT_INVALID;

	CXuiMessage* lpMsgObj = dynamic_cast<CXuiMessage*>(npMsg);

	if (npDestElement == NULL)
		npDestElement = &moIteratorRoot;

	// ����ǲ���ͬһ���̣߳��������ͬһ���߳�����Ҫ�߳�ͬ��
	if (CEinkuiSystem::gpXuiSystem->IsRunningInOperationThread() == false)
	{
		// �ж�XUIϵͳ�Ƿ��Ѿ�׼���˳����������ٹ�����ʱ�����ڽ��������̵߳���Ϣ
		if (mbExitXui != false && npMsg->GetMessageID() != EMSG_QUIT_XUI)
			return ERESULT_UNEXPECTED_CALL;

		lpMsgObj->mhCompleteEvent = CreateEvent(NULL, true, false, NULL);
		if (lpMsgObj->mhCompleteEvent == NULL)
			return ERESULT_INSUFFICIENT_RESOURCES;
	}

	lpMsgObj->SetFlags(ELMSG_FLAG_SEND);
	lpMsgObj->SetDestination(npDestElement);
	lpMsgObj->AddRefer();

	if (lpMsgObj->mhCompleteEvent != NULL)
	{
		// ��Ϣѹ�����ͷ��

		if (moFastMessages.Push_Front(npMsg) != false)
			luResult = ERESULT_SUCCESS;
		else
			luResult = ERESULT_INSUFFICIENT_RESOURCES;

		if (luResult == ERESULT_SUCCESS)
		{
			ReleaseSemaphore(mhMessageInserted, 1, NULL);

			while (WaitForSingleObject(lpMsgObj->mhCompleteEvent, 10) == WAIT_TIMEOUT)
			{
				// Ϊ��ֹwindows UI�߳��е��ñ�������Xui������Ϣ��ʱ��ͬʱ����Xui Operation Thread�е���WinUiCallBack���Ӷ�����������
				// �ٴ�ִ��Windows UI Callback���Ӵ���������
				if (CEinkuiSystem::gpXuiSystem->RunWindowsUICallback() == false)	// must quit
				{
					luResult = ERESULT_TIMEOUT;
					break;
				}
			}
			CloseHandle(lpMsgObj->mhCompleteEvent);

			luResult = lpMsgObj->GetResult();
		}
	}
	else
	{
		luResult = ProcessNextMessage(npMsg);
	}

	return luResult;
}


// ��ָ��Ԫ�ط���һ����Ϣ������ģʽ��Post
ERESULT __stdcall CXelManager::PostMessage(
	IEinkuiIterator* npDestElement,	// ������Ϣ��Ŀ��Ԫ��
	IEinkuiMessage* npMsg,
	IN ULONG nuPostType	// ��Ϣ�����ȼ���EMSG_POST_FAST,EMSG_POST_REVERSE
)
{
	ERESULT luResult = ERESULT_UNSUCCESSFUL;

	//if(mbExitXui != false && npMsg->GetMessageID() != EMSG_QUIT_XUI) discarded by AX 2013.1.21 check it only if is running in non-ui thread
	//	return ERESULT_UNEXPECTED_CALL;

	if (npMsg == NULL || npMsg->IsKindOf(GET_BUILTIN_NAME(CXuiMessage)) == false)
		return ERESULT_ELEMENT_INVALID;

	CXuiMessage* lpMsgObj = dynamic_cast<CXuiMessage*>(npMsg);

	if (npDestElement == NULL)
		npDestElement = &moIteratorRoot;

	if (CEinkuiSystem::gpXuiSystem->IsRunningInOperationThread() == false)
	{
		// �ж�XUIϵͳ�Ƿ��Ѿ�׼���˳����������ٹ�����ʱ�����ڽ��������̵߳���Ϣ
		if (mbExitXui != false && npMsg->GetMessageID() != EMSG_QUIT_XUI)
			return ERESULT_UNEXPECTED_CALL;
	}

	lpMsgObj->SetDestination(npDestElement);
	lpMsgObj->AddRefer();
	lpMsgObj->SetFlags(nuPostType);

	switch (nuPostType)
	{
	case EMSG_POSTTYPE_FAST:
		// ѹ�����ٶ���β��
		luResult = moFastMessages.Push_Back(npMsg) ? ERESULT_SUCCESS : ERESULT_INSUFFICIENT_RESOURCES;
		break;
	case EMSG_POSTTYPE_REVERSE:
		// ѹ����ͨ�������ͷ��
		luResult = moNormalMessages.Push_Front(npMsg) ? ERESULT_SUCCESS : ERESULT_INSUFFICIENT_RESOURCES;
		break;
	case EMSG_POSTTYPE_NORMAL:
	case EMSG_POSTTYPE_REDUCE:
		// ѹ����ͨ�������β��
		luResult = moNormalMessages.Push_Back(npMsg) ? ERESULT_SUCCESS : ERESULT_INSUFFICIENT_RESOURCES;
		break;
	case EMSG_POSTTYPE_QUIT:
		// ѹ����ٶ���ͷ��
		luResult = moFastMessages.Push_Front(npMsg) ? ERESULT_SUCCESS : ERESULT_INSUFFICIENT_RESOURCES;
		break;
		//case EMSG_POSTTYPE_REDUCE:
		//	// ѹ���Լ�������β����������ظ�����Ϣ
		//	{
		//		luResult = /*moReduceMessages*/moNormalMessages.Push_Back(npMsg)?ERESULT_SUCCESS:ERESULT_INSUFFICIENT_RESOURCES;
		//	}
		//	break;
	default:
		luResult = ERESULT_WRONG_PARAMETERS;
	}

	if (luResult == ERESULT_SUCCESS)
		ReleaseSemaphore(mhMessageInserted, 1, NULL);

	return luResult;
}

// �򵥵ظ�ָ��Ԫ�ط���һ����Ϣ������ģʽ��Send���˺����ķ��سɹ�������Ϣ����ķ���ֵ�������ԭ��Ͳ�һ������Ϣ����ķ���ֵ����������Ϣ����ʧ��
ERESULT __stdcall CXelManager::SimpleSendMessage(
	IEinkuiIterator* npDestElement,	// ������Ϣ��Ŀ��Ԫ��
	IN ULONG nuMsgID,	// ��Ϣ����
	IN const void* npInputBuffer,	// �������ݵĻ�����
	IN int niInputSize,	// �������ݵĴ�С
	OUT void* npOutputBuffer,	// ���������(���ػ�����)
	IN int niOutputSize	// �����������С
)
{
	IEinkuiMessage* lpMsgIntf;
	bool lbVolatile;

	// ��������������㹻�󣬲��һ���������ͻ���Ͳ��������뻺������
	if (niInputSize < 64 || npInputBuffer == NULL || (npOutputBuffer != NULL &&
		((UCHAR*)npInputBuffer + niInputSize) >= (UCHAR*)npOutputBuffer &&
		(UCHAR*)npInputBuffer < ((UCHAR*)npOutputBuffer + niOutputSize)))
		lbVolatile = true;
	else
		lbVolatile = false;


	lpMsgIntf = AllocateMessage(nuMsgID, npInputBuffer, niInputSize, npOutputBuffer, niOutputSize, false);

	if (lpMsgIntf == NULL)
		return ERESULT_UNSUCCESSFUL;

	ERESULT luResult = SendMessage(npDestElement, lpMsgIntf);

	lpMsgIntf->Release();

	return luResult;
}

// �򵥵ظ�ָ��Ԫ�ط���һ����Ϣ������ģʽ��Post���޷������Ϣ����ķ���ֵ���˺����ķ���ֵ����ʾ��Ϣ�Ƿ񱻳ɹ�������Ϣ����
ERESULT __stdcall CXelManager::SimplePostMessage(
	IEinkuiIterator* npDestElement,	// ������Ϣ��Ŀ��Ԫ��
	IN ULONG nuMsgID,	// ��Ϣ����
	IN const void* npInputBuffer,	// �������ݵĻ�����
	IN int niInputSize,	// �������ݵĴ�С
	IN ULONG nuPostType	// ��Ϣ�����ȼ���EMSG_POST_FAST,EMSG_POST_REVERSE
)
{
	IEinkuiMessage* lpMsgIntf = AllocateMessage(nuMsgID, npInputBuffer, niInputSize, NULL, 0);

	if (lpMsgIntf == NULL)
		return ERESULT_UNSUCCESSFUL;

	ERESULT luResult = PostMessage(npDestElement, lpMsgIntf, nuPostType);

	lpMsgIntf->Release();

	return luResult;
}



// ����Ϣ������ȡһ����Ϣ�����ҷַ�����
ERESULT CXelManager::ProcessNextMessage(
	IEinkuiMessage* npMsg	//��Ϊ�գ���ʾֱ�Ӵ���������Ϣ����������Ϣ���ж�ȡ
)
{
	IEinkuiMessage* lpMsg;
	CXuiMessage* lpMsgObj;

	// ȡһ����Ϣ
	if (npMsg == NULL)
	{
		lpMsg = moFastMessages.GetMessage();

		if (lpMsg == NULL)
			lpMsg = moNormalMessages.GetMessage();

		//if(lpMsg == NULL)
		//	lpMsg = moReduceMessages.GetMessage();

		if (lpMsg == NULL)
			return ERESULT_NO_MESSAGE;

	}
	else
		lpMsg = npMsg;

	lpMsgObj = dynamic_cast<CXuiMessage*>(lpMsg);
	if (lpMsgObj == NULL)
		return ERESULT_MESSAGE_EXCEPTION;

	// ����Ŀ��
	IEinkuiIterator* lpIterator = lpMsgObj->GetDestination();

	ERESULT luResult = VerifyIterator(lpIterator);

	if (luResult != ERESULT_SUCCESS)
		return luResult;

	CXuiIterator* lpItrObj = dynamic_cast<CXuiIterator*>(lpIterator);
	if (lpItrObj == NULL)
		return ERESULT_ITERATOR_INVALID;

	if (lpItrObj->IsDeleted() != false)
		return ERESULT_ITERATOR_INVALID;

	// ���Ŀ���Ƿ�Hook��ע�⣬Hook��ת����Ϣ�����ܱ�Hookת��
	IEinkuiIterator* lpHooker = lpItrObj->GetHooker();
	if (lpHooker != NULL)
	{
		STEMS_HOOKED_MSG ldHookMsg;

		ldHookMsg.OriginalElement = lpIterator;
		ldHookMsg.OriginalMessage = lpMsg;

		luResult = CExMessage::SendMessage(lpHooker, lpIterator, EMSG_HOOKED_MESSAGE, ldHookMsg, NULL, 0);
		if (luResult != ERESULT_MSG_SENDTO_NEXT && luResult != ERESULT_ITERATOR_INVALID)
			return luResult;
	}

	// ���͵�Ŀ�����
	CEinkuiSystem::gpXuiSystem->PushWidget(lpItrObj->mpWidget);

	ULONG luMsgID = lpMsg->GetMessageID();

	lpItrObj->KAddRefer();

	try {
		CThreadAbort::Dummy();
		luResult = lpItrObj->mpElement->MessageReceiver(lpMsg);
	}
	catch (CThreadAbort())
	{
		// �����ˣ���ô�ͽ������Widget
		luResult = ERESULT_STALLED_THREAD;
	}
	catch (...)
	{
		luResult = ERESULT_MESSAGE_EXCEPTION;
	}

	lpItrObj->KRelease();

	//if(luMsgID == EMSG_CREATE && luResult == ERESULT_SUCCESS)
	//{
	//	//Trace_StringW(6350,lpItrObj->mpElement->GetType());//�յ�Create��Ϣ
	//}

	CEinkuiSystem::gpXuiSystem->PopWidget();

	// �ж��Ƿ��еȴ��¼�
	if (lpMsgObj->mhCompleteEvent != NULL)
		SetEvent(lpMsgObj->mhCompleteEvent);

	// ��Ϣ������ϣ����ͷ���
	lpMsg->Release();

	return luResult;
}

// ����XUIԪ����
void CXelManager::LockIterators(void)
{
	moIteratorLock.Enter();
}

// �������XUIԪ����
void CXelManager::UnlockIterators(void)
{
	moIteratorLock.Leave();
}

// ö��ȫ��Ԫ�أ�ÿ������һ��Elementʱ����ö���������ṩ��ElementEnter��������һ��Ԫ��û����Ԫ��ʱ���������ṩ��ElementLeave
ERESULT __stdcall CXelManager::EnumAllElement(
	bool nbReverse,				// ����ָ����ö���ӽڵ�ʱ������Zorder��˳��ö�٣����߰���Zorder������ö��
	IBaseObject* npApplicant,	// �������
	ERESULT(__stdcall IBaseObject::*ElementEnter)(IEinkuiIterator* npRecipient),//�������ERESULT_ENUM_CHILD����ö�٣�����ERESULT_STOP_ENUM_CHILD����������ֵ��ֹͣö�ٴ�Ԫ�صĴ�Ԫ�ص���Ԫ��
	ERESULT(__stdcall IBaseObject::*ElementLeave)(IEinkuiIterator* npRecipient) //����ֵ������
)
{
	ERESULT luResult = ERESULT_SUCCESS;
	TElEnumIndexStack loIndexStack;
	int liIndex;
	int liDirection = (nbReverse != false) ? -1 : 1;
	CXuiIterator* lpCrtItr;
	CXuiIterator* lpParents;

	lpParents = &moIteratorRoot;

	if (ElementEnter == NULL || ElementLeave == NULL || npApplicant == NULL)
		return ERESULT_WRONG_PARAMETERS;

	//if(Trace_Count(/*EnumEle*/1,3)<3) Trace_Point(27454);//��ʼö������Ԫ��
	liIndex = (nbReverse != false) ? lpParents->GetSubElementCount() : -1;

	while (ERESULT_SUCCEEDED(luResult))
	{
		liIndex += liDirection;
		lpCrtItr = dynamic_cast<CXuiIterator*>(lpParents->GetSubElementByZOder(liIndex));

		if (lpCrtItr != NULL)
		{
			//if(Trace_CountRead(/*EnumEle*/1)<3) Trace_PVOID(23278,lpCrtItr);// �ص�Enter����
			luResult = (npApplicant->*ElementEnter)(lpCrtItr);

			loIndexStack.Push(liIndex);
			lpParents = lpCrtItr;

			if (luResult != ERESULT_ENUM_CHILD)
			{
				liIndex = -2; // ��ʹ������Ԫ��ʱ�Զ���ֹ
				//Trace_CountClear(/*EnumEle*/1);
			}
			else
				liIndex = (nbReverse != false) ? lpParents->GetSubElementCount() : -1;

		}
		else
		{
			// ������һ��
			if (loIndexStack.Size() <= 0)
				break;		// �Ѿ��ڸ�����û����һ����

			liIndex = loIndexStack.Top();
			loIndexStack.Pop();

			//if(Trace_CountRead(/*EnumEle*/1)<3) Trace_PVOID(26454,lpParents);// �ص��˳�����
			luResult = (npApplicant->*ElementLeave)(lpParents);
			if (ERESULT_SUCCEEDED(luResult) == false || luResult == ERESULT_EXIT_ENUM)
				break;

			// ���Բ�����Ϊ�գ����lpParents�Ǹ��ڵ�ĵ�һ���ӽڵ㣬��ô��Ӧ����ǰ��if(loIndexStack.Size() <= 0)�˳���
			CMMASSERT(lpParents->mpParent != NULL);

			// ���ղŵĸ��ڵ�λ���Ƿ�Ư��, ???��Ҫ����һ��
			lpCrtItr = dynamic_cast<CXuiIterator*>(lpParents->mpParent->GetSubElementByZOder(liIndex));
			if (lpCrtItr != lpParents)	// �ղŵĸ��ڵ�ı���λ��
			{
				int i = -1;
				do
				{
					i++;
					lpCrtItr = dynamic_cast<CXuiIterator*>(lpParents->mpParent->GetSubElementByZOder(i));
				} while (lpCrtItr != NULL && lpCrtItr != lpParents);

				if (lpCrtItr == NULL)
					luResult = ERESULT_UNSUCCESSFUL;
				else
					liIndex = i;
			}
			lpParents = lpParents->mpParent;

			if (luResult == ERESULT_REDO_ENUM)
				liIndex -= liDirection;	//���Ŀ��Ҫ���ٴ�ö�٣���ô��Ԥ�Ƚ���ǰ������λ���ص�ѭ��ǰ��Ĵ���ͻ�ָ�Ϊ��ǰĿ��

		}
	}

	return luResult;
}





// �����������ת��
ERESULT CXelManager::OnMsgMouseForward(const PSTELEMGR_WINMSG_FORWARD npMouseInput)
{
	CElMouseFootPrint loMouseFoot;
	ULONG luWsgID;
	bool lbStopDrag = false;
	CXuiIterator* lpCrtMouseFocus;
	IEinkuiIterator* lpModal;

	if (mlCleanHumanInput != 0)
		return ERESULT_UNSUCCESSFUL;

	//////////////////////////////////////////////////////////////////////////
	// ׼�����λ��
	loMouseFoot.TickCount = GetTickCount();
	if (npMouseInput != NULL)
	{
		loMouseFoot.Point.x = (FLOAT)LOSHORT(npMouseInput->lParam) - mdTopLeftInPanel.x;
		loMouseFoot.Point.y = (FLOAT)HISHORT(npMouseInput->lParam) - mdTopLeftInPanel.y;
		luWsgID = npMouseInput->WinMsgID;

		// �������ۼ�
		loMouseFoot.KeyFlag = LOWORD(npMouseInput->wParam);
		if (moMouseTrace.Back().Point.x != loMouseFoot.Point.x || moMouseTrace.Back().Point.y != loMouseFoot.Point.y)
			moMouseTrace.Push_Back(loMouseFoot);

		if (moMouseTrace.Size() > ELMSG_MAX_MOUSE_TRACK)	// ����¼�����󣬾��Ƴ�
			moMouseTrace.Pop_Front();
	}
	else
	{
		// ���û�����ۼ���ֱ���˳�
		if (moMouseTrace.Size() == 0)
			return ERESULT_SUCCESS;

		loMouseFoot = moMouseTrace.Back();
		luWsgID = MAXULONG32;
		loMouseFoot.TickCount = GetTickCount();
	}


	// �����꽹�㣬����ע��Ҫ�ͷ�
	lpCrtMouseFocus = InnerGetMouseFocus();

	//////////////////////////////////////////////////////////////////////////
	// �Ƿ�����ק״̬
	if (mbDragging != false && lpCrtMouseFocus != NULL)
	{
		bool lbLBReleased;

		// ��Ⱦ����ʰ���λ�ã����ô�����ק״̬
		if (npMouseInput == NULL)
		{
			if (lpCrtMouseFocus != NULL)
				lpCrtMouseFocus->KRelease();
			return ERESULT_SUCCESS;
		}

		switch (muKeyWithDragging)
		{
		case MK_LBUTTON:
			lbLBReleased = ((npMouseInput->wParam&MK_LBUTTON) == 0 || luWsgID == WM_LBUTTONUP);
			break;
		case MK_MBUTTON:
			lbLBReleased = ((npMouseInput->wParam&MK_MBUTTON) == 0 || luWsgID == WM_MBUTTONUP);
			break;
		case MK_RBUTTON:
			lbLBReleased = ((npMouseInput->wParam&MK_RBUTTON) == 0 || luWsgID == WM_RBUTTONUP);
			break;
		default:
			lbLBReleased = false;
		}


		// ����ק��Ϊ��
		if (luWsgID == WM_MOUSEMOVE || lbLBReleased != false/*luWsgID == WM_LBUTTONUP*/)
		{
			// ������ק����
			STMS_DRAGGING_ELE ldDragged;

			ldDragged.DragOn = mpDragMsgReceiver;
			CalculateMouseMoving(mpDragMsgReceiver, loMouseFoot.Point, mdDragFrom, ldDragged.Offset);
			ldDragged.ActKey = muKeyWithDragging;
			ldDragged.KeyFlag = LOWORD(npMouseInput->wParam);

			if (ldDragged.Offset.x >= 1.0f || ldDragged.Offset.x <= -1.0f || ldDragged.Offset.y >= 1.0f || ldDragged.Offset.y <= -1.0f)
			{
				// ����ֲ�����
				if (mpDragMsgReceiver->WorldToLocal(loMouseFoot.Point, ldDragged.CurrentPos) != false)
				{
					CExMessage::SendMessage(mpDragMsgReceiver, NULL, EMSG_DRAGGING_ELEMENT, ldDragged);
				}
				// else �����޷�����ֲ����꣬�򲻷�����ק��Ϣ
			}
		}

		// �Ƿ������ק״̬
		if (lbLBReleased != false)
		{
			STMS_DRAGGING_ELE ldDragged;

			ldDragged.DragOn = lpCrtMouseFocus;
			CalculateMouseMoving(mpDragMsgReceiver, loMouseFoot.Point, mdDragFrom, ldDragged.Offset);
			ldDragged.ActKey = muKeyWithDragging;
			ldDragged.KeyFlag = LOWORD(npMouseInput->wParam);

			// ����ֲ�����
			mpDragMsgReceiver->WorldToLocal(loMouseFoot.Point, ldDragged.CurrentPos);

			mbDragging = false;

			//Trace_ULONG(10252,muKeyWithDragging);//EMSG_DRAG_END
			CExMessage::SendMessage(mpDragMsgReceiver, NULL, EMSG_DRAG_END, ldDragged);

			// ������Ҫ���������꣬��ʶ�ղ�����ק��Ϊ�����⽫Button Up��Ϣ�����µ�������;
			// ͬʱ���������������ж��Ƿ�ִ��Drag&Drop��Drop down��Ϊ
			lbStopDrag = true;
		}
		else
			if (mbXuiDragDrop == false && luWsgID != WM_MOUSEWHEEL)	// ���û�н�����ק,������Drag&Drop״̬���Ҳ���Mouse Wheel��Ϣ
			{
				// ��ô�Ͳ��ټ����������
				if (lpCrtMouseFocus != NULL)
					lpCrtMouseFocus->KRelease();
				return ERESULT_SUCCESS;
			}
	}

	//////////////////////////////////////////////////////////////////////////
	//Trace_float(20125,loMouseFoot.Point.x);// ���������
	//Trace_float(22462,loMouseFoot.Point.y);
	if (luWsgID != WM_MOUSELEAVE)
	{
		if (mpMouseMoveOn != NULL)
			mpMouseMoveOn->KRelease();
		mpMouseMoveOn = NULL;
		mlMouseMoveOnLevel = -1;

		// �����ʼλ��
		moPointToTest.Clear();
		CElMouseTestState loState;
		loState.Point = loMouseFoot.Point;
		loState.mpElement = NULL;
		loState.mbSeekInLevels = false;
		loState.mlCrtLevel = 0;
		moPointToTest.Push(loState);

		// ���ȫ������Ŀǰ�����û��֧��Ƕ�׵Ļ��Ʋ�
		mbTopDrawTest = false;
		EnumAllElement(true, this,
			(ERESULT(__stdcall IBaseObject::*)(IEinkuiIterator* npRecipient))&CXelManager::EnterForMouseTest,
			(ERESULT(__stdcall IBaseObject::*)(IEinkuiIterator* npRecipient))&CXelManager::LeaveForMouseTest
		);	// �����жϷ���ֵ
	}
	else
	{
		mpMouseMoveOn = NULL;
		mbTrackMouse = false;
	}


	//////////////////////////////////////////////////////////////////////////
	// ���˵���������
	lpModal = CEinkuiSystem::gpXuiSystem->GetTopModalElement();
	if (mpMouseMoveOn != NULL && lpModal != NULL && lpModal->IsVisible() != false)
	{
		if (mpMouseMoveOn->FindAncestor(lpModal) == false)
		{
			//// �������갴����Ϣ���⼤��ģ̬������˸
			//if(luWsgID == WM_LBUTTONDOWN || luWsgID == WM_RBUTTONDOWN || luWsgID == WM_MBUTTONDOWN)
			//{
			//	CEinkuiSystem::gpXuiSystem->FlashModalElement(lpModal);
			//}
			mpMouseMoveOn = NULL;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// �Ƿ���Drag&drop״̬
	if (mbXuiDragDrop != false)
	{
		if (lbStopDrag == false)
		{
			// ѯ���Ƿ�֧��Drop
			DropDetect(mpMouseMoveOn);

			// �������¼���ִ����
			return ERESULT_SUCCESS;
		}
		else
		{
			// �ж��Ƿ�ִ��Drop down����
			TryDropDown(mpMouseMoveOn);
		}
	}

	if (lbStopDrag != false)
	{
		if (mpDragMsgReceiver != NULL)
			mpDragMsgReceiver->KRelease();
		mpDragMsgReceiver = NULL;
	}

	//////////////////////////////////////////////////////////////////////////
	// �Ƿ�任��꽹��
	if (mpMouseMoveOn != lpCrtMouseFocus)
	{
		if (lpCrtMouseFocus != NULL)
		{
			STEMS_STATE_CHANGE ldChange;
			ldChange.State = 0;//lose
			ldChange.Related = mpMouseMoveOn;

			CExMessage::SendMessage(lpCrtMouseFocus, NULL, EMSG_MOUSE_FOCUS, ldChange);

			ChangeMouseFocus(NULL);
		}

		if (mpMouseMoveOn != NULL)
		{
			STEMS_STATE_CHANGE ldChange;
			ldChange.State = 1;//got
			ldChange.Related = lpCrtMouseFocus;

			CExMessage::SendMessage(mpMouseMoveOn, NULL, EMSG_MOUSE_FOCUS, ldChange);

			ChangeMouseFocus(mpMouseMoveOn);

			// ���л�����Ԫ�أ��´ν��뽫̽������
			mbHoverTest = true;
		}

		// �����ǰ������ǣ��ͷ��������קͬʱ�任�˽��㣬��ô�Ͳ�Ӧ�ð���갴ţ̌����Ϣ���͵��µĽ���
		if (lbStopDrag != false)
		{
			if (lpCrtMouseFocus != NULL)
				lpCrtMouseFocus->KRelease();
			return ERESULT_SUCCESS;
		}

		if (mpTipOwner != NULL)
		{
			//��ʹTip��ʾ��ʱ
			muTickOnTipShow = 0;
			mpTipOwner->KRelease();
			mpTipOwner = NULL;
		}
	}
	else
		if (mbHoverTest != false && luWsgID == MAXULONG32 && lpCrtMouseFocus != NULL)	// ����������ϵͳ�ػ�����ʱ������Ҫ̽������
		{
			// ̽������
			ULONG luTotal = 0;

			if (moMouseTrace.Back().Point.x >= loMouseFoot.Point.x - 1.0f && moMouseTrace.Back().Point.x <= loMouseFoot.Point.x + 1.0f &&
				moMouseTrace.Back().Point.y >= loMouseFoot.Point.y - 1.0f && moMouseTrace.Back().Point.y <= loMouseFoot.Point.y + 1.0f &&
				(loMouseFoot.TickCount - moMouseTrace.Back().TickCount) >= ELMGR_HOVER_DURATION)
			{
				CExMessage::SendMessage(lpCrtMouseFocus, NULL, EMSG_MOUSE_HOVER, CExMessage::DataInvalid);

				// ���һ��Tip��ʾ�߲��Ǵ�Ԫ��
				if (mpTipOwner != lpCrtMouseFocus)
				{
					// �ж��Ƿ���Tip
					const wchar_t* lswTipText = lpCrtMouseFocus->GetToolTip();
					if (lswTipText != NULL)
					{
						STEMS_SWGT_SHOWTIP ldShow;

						muTickOnTipShow = loMouseFoot.TickCount;
						if (mpTipOwner != NULL)
							mpTipOwner->KRelease();
						mpTipOwner = lpCrtMouseFocus;
						mpTipOwner->KAddRefer();
						mbTipHide = false;

						ldShow.Text = lswTipText;
						ldShow.Position.x = loMouseFoot.Point.x + 25.0f;
						ldShow.Position.y = loMouseFoot.Point.y + 20.0f;
						CExMessage::SendMessage(GetDesktop(), NULL, EMSG_SWGT_SHOW_TIP, ldShow);
					}
				}
			}
		}

	// �ͷ���ǰ����꽹��
	if (lpCrtMouseFocus != NULL)
		lpCrtMouseFocus->KRelease();

	// ���Tip��ʾ�Ƿ���
	if (mbTipHide == false && ((loMouseFoot.TickCount - muTickOnTipShow) >= ELMGR_TIP_DURATION || npMouseInput != NULL))
	{
		mbTipHide = true;
		CExMessage::SendMessage(GetDesktop(), NULL, EMSG_SWGT_HIDE_TIP, CExMessage::DataInvalid);
	}

	//  ��������ǻ��ƺ������λ�ã��ǾͿ����˳���
	if (npMouseInput == NULL)
		return ERESULT_SUCCESS;

	// ִ��Windows������

	// ���»�����
	lpCrtMouseFocus = InnerGetMouseFocus();

	// �����ǰû����꽹�㣬��ô�ͼ��һ�»�����Ƿ�ı䣬���˳���
	if (lpCrtMouseFocus == NULL)
	{
		if (luWsgID == WM_LBUTTONDOWN || luWsgID == WM_RBUTTONDOWN || luWsgID == WM_MBUTTONDOWN)
			ChangeActiveElement(NULL);

		return ERESULT_SUCCESS;
	}

	// �������Ϊ����Ϊ�Լ�����Ϣ��ʽ
	switch (luWsgID)
	{
	case WM_LBUTTONDOWN:
	{
		// �п��ܻ�Ҫ�ı䴰�ڲ������Ҫ�ı���̽���
		BringFocusedPopupToTop(lpCrtMouseFocus);

		// �жϵ�ǰĿ���ǲ���һ������ק�Ĵ��ڣ������Ƿ������ק״̬
		DetectMouseDragBegin(lpCrtMouseFocus, MK_LBUTTON, LOWORD(npMouseInput->wParam), loMouseFoot.Point);

		SendMouseButtonPressed(lpCrtMouseFocus, true, MK_LBUTTON, LOWORD(npMouseInput->wParam), loMouseFoot.TickCount, mdPointRelative);

		// ���ü��̽���
		DetectKeyboardFocus(lpCrtMouseFocus);

		// ���û�Ծ����
		ChangeActiveElement(lpCrtMouseFocus);
	}
	break;
	case WM_LBUTTONUP:
	{
		SendMouseButtonPressed(lpCrtMouseFocus, false, MK_LBUTTON, LOWORD(npMouseInput->wParam), loMouseFoot.TickCount, mdPointRelative);
	}
	break;
	case WM_LBUTTONDBLCLK:
	{
		SendMouseButtonDbClick(lpCrtMouseFocus, MK_LBUTTON, LOWORD(npMouseInput->wParam), loMouseFoot.TickCount, mdPointRelative);
	}
	break;
	case WM_RBUTTONDOWN:
	{
		// �п��ܻ�Ҫ�ı䴰�ڲ������Ҫ�ı���̽���
		BringFocusedPopupToTop(lpCrtMouseFocus);

		// �жϵ�ǰĿ���ǲ���һ������ק�Ĵ��ڣ������Ƿ������ק״̬
		DetectMouseDragBegin(lpCrtMouseFocus, MK_RBUTTON, LOWORD(npMouseInput->wParam), loMouseFoot.Point);

		SendMouseButtonPressed(lpCrtMouseFocus, true, MK_RBUTTON, LOWORD(npMouseInput->wParam), loMouseFoot.TickCount, mdPointRelative);

		// ���ü��̽���
		DetectKeyboardFocus(lpCrtMouseFocus);

		// ���û�Ծ����
		ChangeActiveElement(lpCrtMouseFocus);
	}
	break;
	case WM_RBUTTONUP:
	{
		SendMouseButtonPressed(lpCrtMouseFocus, false, MK_RBUTTON, LOWORD(npMouseInput->wParam), loMouseFoot.TickCount, mdPointRelative);
	}
	break;
	case WM_RBUTTONDBLCLK:
	{
		SendMouseButtonDbClick(lpCrtMouseFocus, MK_RBUTTON, LOWORD(npMouseInput->wParam), loMouseFoot.TickCount, mdPointRelative);
	}
	break;
	case WM_MBUTTONDOWN:
	{
		// �п��ܻ�Ҫ�ı䴰�ڲ������Ҫ�ı���̽���
		BringFocusedPopupToTop(lpCrtMouseFocus);

		// �жϵ�ǰĿ���ǲ���һ������ק�Ĵ��ڣ������Ƿ������ק״̬
		DetectMouseDragBegin(lpCrtMouseFocus, MK_MBUTTON, LOWORD(npMouseInput->wParam), loMouseFoot.Point);

		SendMouseButtonPressed(lpCrtMouseFocus, true, MK_MBUTTON, LOWORD(npMouseInput->wParam), loMouseFoot.TickCount, mdPointRelative);

		// ���ü��̽���
		DetectKeyboardFocus(lpCrtMouseFocus);

		// ���û�Ծ����
		ChangeActiveElement(lpCrtMouseFocus);
	}
	break;
	case WM_MBUTTONUP:
	{
		SendMouseButtonPressed(lpCrtMouseFocus, false, MK_MBUTTON, LOWORD(npMouseInput->wParam), loMouseFoot.TickCount, mdPointRelative);
	}
	break;
	case WM_MBUTTONDBLCLK:
	{
		SendMouseButtonDbClick(lpCrtMouseFocus, MK_MBUTTON, LOWORD(npMouseInput->wParam), loMouseFoot.TickCount, mdPointRelative);
	}
	break;
	case WM_XBUTTONDOWN:
	{
		ULONG luActKey;

		if (HIWORD(npMouseInput->wParam) == XBUTTON1)
			luActKey = MK_XBUTTON1;
		else
			if (HIWORD(npMouseInput->wParam) == XBUTTON2)
				luActKey = MK_XBUTTON2;
			else
				break;

		SendMouseButtonPressed(lpCrtMouseFocus, true, luActKey, LOWORD(npMouseInput->wParam), loMouseFoot.TickCount, mdPointRelative);

	}
	break;
	case WM_XBUTTONUP:
	{
		ULONG luActKey;

		if (HIWORD(npMouseInput->wParam) == XBUTTON1)
			luActKey = MK_XBUTTON1;
		else
			if (HIWORD(npMouseInput->wParam) == XBUTTON2)
				luActKey = MK_XBUTTON2;
			else
				break;

		SendMouseButtonPressed(lpCrtMouseFocus, false, luActKey, LOWORD(npMouseInput->wParam), loMouseFoot.TickCount, mdPointRelative);

	}
	break;
	case WM_XBUTTONDBLCLK:
	{
		ULONG luActKey;

		if (HIWORD(npMouseInput->wParam) == XBUTTON1)
			luActKey = MK_XBUTTON1;
		else
			if (HIWORD(npMouseInput->wParam) == XBUTTON2)
				luActKey = MK_XBUTTON2;
			else
				break;

		SendMouseButtonDbClick(lpCrtMouseFocus, luActKey, LOWORD(npMouseInput->wParam), loMouseFoot.TickCount, mdPointRelative);
	}
	break;
	case WM_MOUSEMOVE:
	{
		STEMS_MOUSE_MOVING ldMouse;

		ldMouse.KeyFlag = LOWORD(npMouseInput->wParam);
		ldMouse.TickCount = loMouseFoot.TickCount;
		ldMouse.Position = mdPointRelative;

		CExMessage::SendMessage(lpCrtMouseFocus, NULL, EMSG_MOUSE_MOVING, ldMouse);
	}
	break;
	case WM_MOUSEWHEEL:	// Mouse wheel ��ϢЯ����pointλ������Ļ���꣬����ȫ�����ڳ����ʱ����Ҫ���⴦��Ŀǰ������⻹û�д����뿪���°��ʱ��ע��??? Ax Nov.7,2012
	{
		STEMS_MOUSE_WHEEL ldMouse;

		ldMouse.Delta = HIWORD(npMouseInput->wParam);
		ldMouse.KeyFlag = LOWORD(npMouseInput->wParam);
		ldMouse.TickCount = loMouseFoot.TickCount;
		ldMouse.MouseFocus = lpCrtMouseFocus;
		//ldMouse.Position = mdPointRelative;

		TransferMouseWheel(lpCrtMouseFocus, ldMouse);
	}
	break;
	default:
		break;
	}


	if (lpCrtMouseFocus != NULL)
	{
		// ������windows�����٣����û�п�ʼ���٣������windows�����
		if (mbTrackMouse == false)
		{
			//�ر���Track mouse �Ĳ��� ax
			//TRACKMOUSEEVENT TrackMouse;
			//TrackMouse.cbSize = sizeof(TrackMouse);
			//TrackMouse.dwFlags = TME_LEAVE;
			//TrackMouse.hwndTrack = EinkuiGetSystem()->GetMainWindow();

			//TrackMouseEvent(&TrackMouse);
			mbTrackMouse = true;
		}

		lpCrtMouseFocus->KRelease();
	}


	return ERESULT_SUCCESS;
}



// �޸Ĵ˴��룬��������Ϣ���������Ϣת���������߼�δ������Ax.2017.08.16
// �����������ת��
// ERESULT CXelManager::OnMsgEinkTouchForward(const PSTEMS_TOUCH npMouseInput)
// {
// 	return ERESULT_SUCCESS;
// }

// ѯ���Ƿ�֧��Drop
void CXelManager::DropDetect(CXuiIterator* npToDetect)
{
	CElMouseFootPrint loMouseFoot;
	if (npToDetect == NULL || npToDetect == mpDraggingElement/* || npToDetect == mpRecentAskedDrop*/)
		return;

	if (mpRecentAskedDrop != NULL && mpRecentAskedDrop->CheckStyle(EITR_STYLE_XUIDRAGDROP) == false && npToDetect == mpRecentAskedDrop)
		return;	// ������������Եģ������Ѿ���¼��

	if (mpRecentAskedDrop != NULL && mpRecentAskedDrop->CheckStyle(EITR_STYLE_XUIDRAGDROP) != false && npToDetect != mpRecentAskedDrop)
	{
		// ���뿪�Ķ�����Drop leave��Ϣ
		CExMessage::SendMessage(mpRecentAskedDrop, NULL, EMSG_DRAGDROP_LEAVE, CExMessage::DataInvalid);

	}

	// ����������ʼ�¼
	if (npToDetect != mpRecentAskedDrop)
	{
		if (mpRecentAskedDrop != NULL)
			mpRecentAskedDrop->KRelease();
		mpRecentAskedDrop = npToDetect;
		mpRecentAskedDrop->KAddRefer();

		// ���¶����ͽ�����Ϣ
		if (npToDetect->CheckStyle(EITR_STYLE_XUIDRAGDROP) != false)
		{
			CExMessage::SendMessage(mpRecentAskedDrop, NULL, EMSG_DRAGDROP_ENTER, CExMessage::DataInvalid);
		}
	}

	if (npToDetect->CheckStyle(EITR_STYLE_XUIDRAGDROP) != false)
	{
		// ����ֲ�����
		loMouseFoot = moMouseTrace.Back();
		npToDetect->WorldToLocal(loMouseFoot.Point, mdDrgdrpRequest.CurrentPos);
		mdDrgdrpRequest.KeyFlag = loMouseFoot.KeyFlag;

		// ѯ���Ƿ����Drop
		ERESULT luResult = CExMessage::SendMessage(npToDetect, NULL, EMSG_XUI_DROP_TEST, mdDrgdrpRequest);
		if (luResult == ERESULT_EDRAGDROP_ACCEPT)
		{
			if (mpLastAccepter != NULL)
				mpLastAccepter->KRelease();
			mpLastAccepter = npToDetect;
			mpLastAccepter->KAddRefer();
		}
	}
}

// ִ��Drop down
void CXelManager::TryDropDown(CXuiIterator* npToTry)
{
	if (npToTry != NULL && npToTry == mpLastAccepter && mpDraggingElement != NULL)
	{
		CElMouseFootPrint loMouseFoot;
		IEinkuiMessage* lpMsg = NULL;
		ERESULT luResult;

		do
		{
			// ����ֲ�����
			loMouseFoot = moMouseTrace.Back();
			npToTry->WorldToLocal(loMouseFoot.Point, mdDrgdrpRequest.CurrentPos);
			mdDrgdrpRequest.KeyFlag = loMouseFoot.KeyFlag;

			// send 'drag off' to Drag&Drop host
			lpMsg = AllocateMessage(EMSG_XUI_DRAG_OFF, &mpLastAccepter, sizeof(mpLastAccepter), &mdDrgdrpRequest, sizeof(mdDrgdrpRequest));
			BREAK_ON_NULL(lpMsg);

			luResult = SendMessage(mpDragMsgReceiver, lpMsg);
			if (luResult != ERESULT_SUCCESS)
				break;

			if (lpMsg->GetOutputDataSize() == sizeof(mdDrgdrpRequest))
			{
				//��ֹ�ò������޸�
				mdDrgdrpRequest.Host = mpDraggingElement;
			}
			CMM_SAFE_RELEASE(lpMsg);

			// send 'drop down' to Drag&Drop acceptor
			lpMsg = AllocateMessage(EMSG_XUI_DROP_DOWN, &mdDrgdrpRequest, sizeof(mdDrgdrpRequest), NULL, 0);
			BREAK_ON_NULL(lpMsg);

			luResult = SendMessage(mpLastAccepter, lpMsg);

		} while (false);

		CMM_SAFE_RELEASE(lpMsg);
	}

	mbXuiDragDrop = false;

	if (mpDraggingElement != NULL)
		mpDraggingElement->KRelease();
	mpDraggingElement = NULL;

	if (mpLastAccepter != NULL)
		mpLastAccepter->KRelease();
	mpLastAccepter = NULL;

	if (mpRecentAskedDrop != NULL)
		mpRecentAskedDrop->KRelease();
	mpRecentAskedDrop = NULL;

}


// ������갴����Ϣ
void CXelManager::SendMouseButtonPressed(IEinkuiIterator* npFocus, bool nbPressed, ULONG nuActKey, ULONG nuKeyFlag, ULONG nuTickCount, const D2D1_POINT_2F& rPosition)
{
	STEMS_MOUSE_BUTTON ldMouse;

	ldMouse.Presssed = nbPressed;
	ldMouse.ActKey = nuActKey;
	ldMouse.KeyFlag = nuKeyFlag;
	ldMouse.TickCount = nuTickCount;
	ldMouse.Position = rPosition;

	SimpleSendMessage(npFocus, EMSG_MOUSE_BUTTON, &ldMouse, sizeof(ldMouse), NULL, 0);
}

// ������갴��˫����Ϣ
void CXelManager::SendMouseButtonDbClick(IEinkuiIterator* npFocus, ULONG nuActKey, ULONG nuKeyFlag, ULONG nuTickCount, const D2D1_POINT_2F& rPosition)
{
	STEMS_MOUSE_BUTTON ldMouse;

	ldMouse.Presssed = false;
	ldMouse.ActKey = nuActKey;
	ldMouse.KeyFlag = nuKeyFlag;
	ldMouse.TickCount = nuTickCount;
	ldMouse.Position = rPosition;

	SimpleSendMessage(npFocus, EMSG_MOUSE_DBCLICK, &ldMouse, sizeof(ldMouse), NULL, 0);
}


// ����Ƿ�������ק���ҷ��Ϳ�ʼ��ק��Ϣ
void CXelManager::DetectMouseDragBegin(CXuiIterator* npFocus, ULONG nuActKey, ULONG nuKeyFlag, const D2D1_POINT_2F& rPosition)
{
	if (mbDragging != false)
		return;

	// ��ǰ�����Ƿ�֧����ק
	if (npFocus->CheckStyle(EITR_STYLE_DRAG) == false)
	{
		// ��֧�֣���ô�ж��Ƿ��и�����������ȫ��ק
		CXuiIterator* lpItrObj = (CXuiIterator*)npFocus->GetParent();

		if (mpDragMsgReceiver != NULL)
			mpDragMsgReceiver->KRelease();
		mpDragMsgReceiver = NULL;

		// û�е�����ڵ�
		while (lpItrObj != (CXuiIterator*)lpItrObj->GetParent())
		{
			if (lpItrObj->CheckStyle(EITR_STYLE_ALL_DRAG) != false)
			{
				mpDragMsgReceiver = lpItrObj;
				mpDragMsgReceiver->KAddRefer();
				break;
			}
			lpItrObj = (CXuiIterator*)lpItrObj->GetParent();
		}
		if (mpDragMsgReceiver == NULL)
			return;
	}
	else
	{
		if (mpDragMsgReceiver != npFocus)
		{
			if (mpDragMsgReceiver != NULL)
				mpDragMsgReceiver->KRelease();
			mpDragMsgReceiver = npFocus;
			mpDragMsgReceiver->KAddRefer();
		}
	}

	STMS_DRAGGING_ELE ldDragged;
	IEinkuiMessage* lpMsg;
	ERESULT luResult;

	// ������ק��ʼ��Ϣ
	ldDragged.Offset.x = 0.0f;
	ldDragged.Offset.y = 0.0f;
	ldDragged.ActKey = nuActKey;
	ldDragged.KeyFlag = nuKeyFlag;
	ldDragged.DragOn = npFocus;

	// ����ֲ����꣬Ĭ������£�����ת���������Ѿ�ת����
	if (npFocus->WorldToLocal(rPosition, ldDragged.CurrentPos) == false) //�����޷�����ֲ����꣬�򲻷�����ק��Ϣ
	{
		if (mpDragMsgReceiver != NULL)
			mpDragMsgReceiver->KRelease();
		mpDragMsgReceiver = NULL;
		return;
	}


	lpMsg = AllocateMessage(EMSG_DRAG_BEGIN, &ldDragged, sizeof(ldDragged), &mdDrgdrpRequest, sizeof(mdDrgdrpRequest));

	luResult = SendMessage(mpDragMsgReceiver, lpMsg);
	if (ERESULT_SUCCEEDED(luResult))	// ERESULT_WDRAGDROP_BEGIN for windows drag&drop
	{

		mbDragging = true;
		muKeyWithDragging = nuActKey;
		mdDragFrom = rPosition;

		if (lpMsg->GetResult() == ERESULT_EDRAGDROP_BEGIN && lpMsg->GetOutputDataSize() == sizeof(mdDrgdrpRequest))
		{
			// ��ʼִ���ϷŲ���
			mdDrgdrpRequest.Host = mpDragMsgReceiver;
			mbXuiDragDrop = true;

			if (mpDraggingElement != NULL)
				mpDraggingElement->KRelease();
			mpDraggingElement = npFocus;
			mpDraggingElement->KAddRefer();

			if (mpRecentAskedDrop != NULL)
				mpRecentAskedDrop->KRelease();
			mpRecentAskedDrop = NULL;

			if (mpLastAccepter != NULL)
				mpLastAccepter->KRelease();
			mpLastAccepter = NULL;
		}
	}
	else
	{
		if (mpDragMsgReceiver != NULL)
			mpDragMsgReceiver->KRelease();
		mpDragMsgReceiver = NULL;
	}

	lpMsg->Release();
}

// ��Ŀ��Ԫ�ش����ﵽ����ȫ��������Zorder��߲�
void CXelManager::BringFocusedPopupToTop(CXuiIterator* npFocus)
{
	CXuiIterator* lpItrObj = (CXuiIterator*)npFocus->GetParent();

	// ������ڵ�
	while (npFocus != lpItrObj)
	{
		if (npFocus->CheckStyle(EITR_STYLE_POPUP) != false)
		{
			lpItrObj->BringSubElementToTop(npFocus);
		}

		npFocus = lpItrObj;
		lpItrObj = (CXuiIterator*)npFocus->GetParent();
	}

}

// ����Ƿ���ü��̽��㣬���Ŀ��ǡ���ǵ�ǰ���̽�����ֱ�ӷ��أ������ͷŵ�ǰ���̽���
void CXelManager::DetectKeyboardFocus(CXuiIterator* npToDetect)
{
	// ��֧�֣���ô�ж��Ƿ��и�����������EITR_STYLE_ALL_KEY
	if (npToDetect->CheckStyle(EITR_STYLE_KEYBOARD) == false)
	{
		CXuiIterator* lpItrObj = (CXuiIterator*)npToDetect->GetParent();
		npToDetect = NULL;

		// û�е�����ڵ�
		while (lpItrObj != (CXuiIterator*)lpItrObj->GetParent())
		{
			if (lpItrObj->CheckStyle(EITR_STYLE_ALL_KEY) != false)
			{
				npToDetect = lpItrObj;
				break;
			}
			if (lpItrObj->CheckStyle(EITR_STYLE_POPUP) != false)
				break;
			lpItrObj = (CXuiIterator*)lpItrObj->GetParent();
		}
	}

	ChangeKeyFocus(npToDetect);
}


// ��������Ԥ����
ERESULT __stdcall CXelManager::EnterForMouseTest(IEinkuiIterator* npRecipient)
{
	ERESULT luResult;
	D2D1_RECT_F ldVisibleRegion;
	//IEinkuiIterator* lpEnhancer;
	CElMouseTestState loState;
	CXuiIterator* lpRecipient = dynamic_cast<CXuiIterator*>(npRecipient);

	do
	{
		loState = moPointToTest.Top();

		moPointToTest.Push(loState);
		// ������أ��˳�
		if (npRecipient->IsVisible() == false)
		{
			luResult = ERESULT_STOP_ENUM_CHILD;
			break;
		}

		if (moPointToTest.Top().mbSeekInLevels == false)
		{
			if (lpRecipient->GetPaintLevelCount() > 0)
			{
				moPointToTest.Top().mbSeekInLevels = true;
			}
		}

		if (lpRecipient->GetPaintLevel() >= 0)
			moPointToTest.Top().mlCrtLevel = lpRecipient->GetPaintLevel();

		// ��������˿����������ж��Ƿ���������
		if (lpRecipient->GetVisibleRegion(ldVisibleRegion) != false)
		{
			D2D1_POINT_2F ldPosition;
			// �����������������������
			if (lpRecipient->WorldToLocal(moPointToTest.Top().Point, ldPosition) == false ||
				ldPosition.x < ldVisibleRegion.left || ldPosition.x >= ldVisibleRegion.right ||
				ldPosition.y < ldVisibleRegion.top || ldPosition.y >= ldVisibleRegion.bottom)
			{
				return ERESULT_STOP_ENUM_CHILD;	// ����������
			}
		}

		luResult = ERESULT_ENUM_CHILD;

	} while (false);

	//// �ж��Ƿ�����Ч������Ч��״̬�µ���괦��û�кܺõ�ʵ��???������û�е���
	//lpEnhancer = lpRecipient->GetEnhancer();
	//if(lpEnhancer != NULL)
	//{
	//	CElMouseTestState loState;
	//	// ������Ч�������Ԥ����
	//	luResult = SimpleSendMessage(lpEnhancer,EMSG_ENHANCER_PRE_MOUSE_TEST,&moPointToTest.Top().Point,sizeof(loState.Point),&loState.Point,sizeof(loState.Point));
	//	if(luResult != ERESULT_SUCCESS)
	//		return ERESULT_STOP_ENUM_CHILD;

	//	// ���µ����λ��ѹ������ö�ջ
	//	loState.mpElement = npRecipient;
	//	moPointToTest.Push(loState);
	//}

	return luResult;
}

// �����������
ERESULT __stdcall CXelManager::LeaveForMouseTest(IEinkuiIterator* npRecipient)
{
	D2D1_RECT_F ldVisibleRegion;
	ERESULT luResult = ERESULT_SUCCESS;
	D2D1_POINT_2F ldLocalPos;
	CXuiIterator* lpRecipient = dynamic_cast<CXuiIterator*>(npRecipient);

	do
	{
		if ((mbXuiDragDrop != false && npRecipient == mpDraggingElement))
		{
			luResult = ERESULT_NOT_POINTOWNER;
			break;
		}
		if (npRecipient->IsVisible() == false)
			break;

		if (lpRecipient->CheckStyle(EITR_STYLE_MOUSE) == false)
		{
			luResult = ERESULT_NOT_POINTOWNER;
			break;
		}

		// ����ֲ�����
		if (lpRecipient->WorldToLocal(moPointToTest.Top().Point, ldLocalPos) != false)
		{
			// ��������˿����������ж��Ƿ���������
			if (lpRecipient->GetVisibleRegion(ldVisibleRegion) != false && (ldLocalPos.x < ldVisibleRegion.left || ldLocalPos.x >= ldVisibleRegion.right ||
				ldLocalPos.y < ldVisibleRegion.top || ldLocalPos.y >= ldVisibleRegion.bottom))
			{
				luResult = ERESULT_NOT_POINTOWNER;
			}
			else
			{
				// ��Ŀ��Ԫ�ط�����Ϣ
				luResult = SimpleSendMessage(lpRecipient, EMSG_MOUSE_OWNER_TEST, &ldLocalPos, sizeof(ldLocalPos), NULL, 0);
				if (luResult == ERESULT_MOUSE_OWNERSHIP)
				{
					// ��ͼ������������ӵ����
					if (mpMouseMoveOn != NULL)
					{
						if (moPointToTest.Top().mbSeekInLevels == false)
						{
							luResult = ERESULT_EXIT_ENUM;
							break;
						}

						// �Ƚ�һ�£��µĶ����Ƿ����еĲ�θ�
						if (mlMouseMoveOnLevel >= moPointToTest.Top().mlCrtLevel)
						{
							// ���ڻ��ߵ��ڣ����л�
							luResult = ERESULT_NOT_POINTOWNER;
							break;
						}
						// �ͷŵ�ǰ
						mpMouseMoveOn->KRelease();
					}
					mpMouseMoveOn = lpRecipient;
					mpMouseMoveOn->KAddRefer();
					mlMouseMoveOnLevel = moPointToTest.Top().mlCrtLevel;
					mdPointRelative = ldLocalPos;
					if (moPointToTest.Top().mbSeekInLevels == false)
						luResult = ERESULT_EXIT_ENUM;
					else
						luResult = ERESULT_ENUM_CHILD;//ERESULT_EXIT_ENUM;
				}
			}
		}
		else
		{
			//Trace_Point(11900);// ���󲻿��棬�޷�����
			luResult = ERESULT_UNSUCCESSFUL;
		}

	} while (false);

	//  ��ջ
	moPointToTest.Pop();

	return luResult;
}

// ��ݼ�������false��ʾ���ǿ�ݼ�
bool CXelManager::HotKeyProcessor(CXuiIterator* npHost, const PSTELEMGR_WINMSG_FORWARD npKeyStrike)
{
	CXuiHotkeyEntry loFind;
	//int liIndex;
	STEMS_HOTKEY ldHotKey;

	// ȡ�ð�����ȫ����Ϣ
	loFind.msuVkCode = (USHORT)npKeyStrike->wParam;
	loFind.mcuExKey = 0;
	if ((GetKeyState(VK_CONTROL) & 0x8000) != 0)
		loFind.mcuExKey |= loFind.eControl;
	if ((GetKeyState(VK_SHIFT) & 0x8000) != 0)
		loFind.mcuExKey |= loFind.eShit;
	if ((GetKeyState(VK_MENU) & 0x8000) != 0)
		loFind.mcuExKey |= loFind.eAlt;
	loFind.mpApplicant = NULL;

	// ���ȫ�ֵĿ�ݼ�
	if (DetectHotKey(&moIteratorRoot, loFind) == false)
	{
		// ȫ��û���ҵ�
		if (npHost != NULL)
			DetectHotKey(npHost, loFind);
	}

	if (loFind.mpApplicant != NULL)
	{
		// ���ֿ�ݼ���
		ldHotKey.HotKeyID = loFind.muHotKeyID;
		ldHotKey.Focus = npHost;
		ldHotKey.VkCode = loFind.msuVkCode;
		ldHotKey.Control = (loFind.mcuExKey&loFind.eControl) != 0;
		ldHotKey.Shift = (loFind.mcuExKey&loFind.eShit) != 0;
		ldHotKey.Alt = (loFind.mcuExKey&loFind.eAlt) != 0;

		if (CExMessage::SendMessage(loFind.mpApplicant, NULL, EMSG_HOTKEY_PRESSED, ldHotKey, NULL, 0) == ERESULT_KEY_UNEXPECTED)
			return false;

		return true;
	}

	return false;
}

// ����ݼ�������false��ʾ���ǿ�ݼ�
bool CXelManager::DetectHotKey(CXuiIterator* npHost, CXuiHotkeyEntry& rToFind)
{
	bool lbFound = false;

	// ���ȫ�ֵĿ�ݼ�
	//moIteratorLock.Enter();

	try
	{
		do
		{
			lbFound = npHost->DetectHotKey(rToFind);
			if (lbFound != false)
				break;

			npHost = (CXuiIterator*)npHost->GetParent();
		} while (npHost != (CXuiIterator*)npHost->GetParent());	// һֱ�ҵ����ڵ���һ��
	}
	catch (...)
	{
		lbFound = false;
	}

	//moIteratorLock.Leave();

	return lbFound;
}


// �����������ת��
ERESULT CXelManager::OnMsgKeyboardForward(const PSTELEMGR_WINMSG_FORWARD npKeyStrike)
{
	ERESULT luResult = ERESULT_UNSUCCESSFUL;
	CXuiIterator* lpKeyFocus = NULL;

	do
	{
		if (mlCleanHumanInput != 0)
			break;

		lpKeyFocus = InnerGetKeyFocus();

		// �����̽���Ϊ��ʱ����ͨ������Ϣ�����ߵ����ۼ��̽���״����ϵͳ������Ϣ	<- �޸ĺ�һ�����жϿ�ݼ�
		if (npKeyStrike->WinMsgID == WM_KEYDOWN /*&& lpKeyFocus==NULL*/ || npKeyStrike->WinMsgID == WM_SYSKEYDOWN)
		{
			CXuiIterator* lpHotkeyFrom;
			bool lbFound = false;

			if (lpKeyFocus != NULL)
			{
				lpHotkeyFrom = lpKeyFocus;
				lpHotkeyFrom->KAddRefer();
			}
			else
			{
				lpHotkeyFrom = GetActiveElement();
			}

			if (lpHotkeyFrom != NULL)
			{
				lbFound = HotKeyProcessor(lpHotkeyFrom, npKeyStrike);

				lpHotkeyFrom->KRelease();
			}

			// �ҵ��˿�ݼ�����ϵͳ������Ϣ������Ԫ�ط���
			if (lbFound != false || npKeyStrike->WinMsgID == WM_SYSKEYDOWN)
			{
				luResult = ERESULT_SUCCESS;
				break;
			}
		}

		if (lpKeyFocus == NULL)
		{
			if (npKeyStrike->WinMsgID == WM_KEYUP)
			{
				STEMS_KEY_PRESSED ldStrike;

				ldStrike.IsPressDown = false;
				ldStrike.VirtualKeyCode = (USHORT)npKeyStrike->wParam;
				ldStrike.ExtendedKeyFlag = (ULONG)npKeyStrike->lParam;

				if (ldStrike.VirtualKeyCode == VK_TAB)// �����Tab��Ϣ����ת�Ƽ��̽���
				{
					ShiftKeyBoardFocus(NULL);
				}
				//else
				//{
				//	KeyToCommand(&ldStrike);
				//}

				luResult = ERESULT_SUCCESS;
				break;
			}

			if (mlProbePass == 0)	// �ж��Ƿ����̽��ģʽ
			{
				if (npKeyStrike->WinMsgID == WM_KEYDOWN)
				{
					if (LOSHORT(npKeyStrike->wParam) == VK_F9)
						mlProbePass = 1;
					else
						if (mlProbeMode > 0)
						{
							// ���벻ͬ̽��ģʽ
							switch (LOSHORT(npKeyStrike->wParam))
							{
							case VK_F5:
								mlProbeMode = 1;
								break;
							case VK_F6:
								mlProbeMode = 2;
								break;
							case VK_F7:
								mlProbeMode = 3;
								break;
							case VK_F8:
								mlProbeMode = 4;
								break;
							default:
								mlProbePass = 0;
							}
						}
						else
							mlProbePass = 0;
				}
			}
			else
				if (npKeyStrike->WinMsgID == WM_CHAR)
				{
					if (mlProbePass == 1 && LOSHORT(npKeyStrike->wParam) == L'e' ||
						mlProbePass == 2 && LOSHORT(npKeyStrike->wParam) == L's' ||
						mlProbePass == 3 && LOSHORT(npKeyStrike->wParam) == L'i')
					{
						if (mlProbePass++ == 3)
						{
							if (mlProbeMode > 0)
								mlProbeMode = -1;
							else
								mlProbeMode = 1;

							mlProbePass = 0;
						}
					}
					else
						mlProbePass = 0;
				}

			luResult = ERESULT_SUCCESS;
			break;
		}

		if (npKeyStrike->WinMsgID == WM_CHAR)
		{
			STEMS_CHAR_INPUT ldChar;

			ldChar.CharIn = (USHORT)npKeyStrike->wParam;
			ldChar.Flags = (ULONG)npKeyStrike->lParam;

			luResult = SimpleSendMessage(lpKeyFocus, EMSG_CHAR_INPUT, &ldChar, sizeof(ldChar), NULL, 0);
		}
		else
			if (npKeyStrike->WinMsgID == WM_KEYDOWN || npKeyStrike->WinMsgID == WM_KEYUP)
			{
				STEMS_KEY_PRESSED ldStrike;

				ldStrike.IsPressDown = (npKeyStrike->WinMsgID == WM_KEYDOWN);
				ldStrike.VirtualKeyCode = (USHORT)npKeyStrike->wParam;
				ldStrike.ExtendedKeyFlag = (ULONG)npKeyStrike->lParam;

				luResult = KeyStrike(lpKeyFocus, &ldStrike);
				if (luResult == ERESULT_KEY_UNEXPECTED && npKeyStrike->WinMsgID == WM_KEYDOWN)
				{
					// ���̽��㲻ʶ��İ�����ת��Ϊ��ݼ�
					//HotKeyProcessor(lpKeyFocus,npKeyStrike);

					luResult = ERESULT_SUCCESS;
				}

			}
			else
				luResult = ERESULT_WRONG_PARAMETERS;

	} while (false);

	if (lpKeyFocus != NULL)
		lpKeyFocus->KRelease();

	return luResult;
}

ERESULT CXelManager::OnMsgEnalbeInput(void)
{
	InterlockedExchange(&mlCleanHumanInput, 0);
	return ERESULT_SUCCESS;
}

// ���ͼ�����Ϣ��Ŀ�꣬���Ŀ�귴��ERESULT_UNEXPECTED_KEY�������ϴ��ݲ�֧�ֵļ��̰�����Ϣ����PopupԪ��Ϊֹ
ERESULT CXelManager::KeyStrike(CXuiIterator* npKeyFocus, const PSTEMS_KEY_PRESSED npStrike)
{
	ERESULT luResult;

	luResult = SimpleSendMessage(npKeyFocus, EMSG_KEY_PRESSED, npStrike, sizeof(STEMS_KEY_PRESSED), NULL, 0);

	if (luResult == ERESULT_KEY_UNEXPECTED && npStrike->IsPressDown == false && npStrike->VirtualKeyCode == VK_TAB)// �����Tab��Ϣ����ת�Ƽ��̽���
	{
		ShiftKeyBoardFocus(npKeyFocus);
	}

	return luResult;
}

// ����Tab���������ļ��̽����л�
bool CXelManager::ShiftKeyBoardFocus(CXuiIterator* npKeyFocus)
{

	CXuiIterator* lpHost;
	CXuiIterator* lpMoveTo = NULL;
	bool lbOK = false;

	LockIterators();

	try
	{
		if (npKeyFocus == NULL)
		{
			lpHost = GetActiveElement();
			if (lpHost == NULL)
				THROW_FALSE;

			lpHost->KRelease();	// ֱ�Ӽ���һ�����ü�������Ϊ�ڻ������У������ڱ��ͷŵķ���
		}
		else
		{
			lpHost = (CXuiIterator*)npKeyFocus->GetParent();
		}

		while (lpHost != (CXuiIterator*)lpHost->GetParent())
		{
			lpMoveTo = lpHost->GetNextKeyBoardAccepter(npKeyFocus);
			if (lpMoveTo != NULL)	// �ҵ��µĽ��յ�
				break;
			if (lpHost->CheckStyle(EITR_STYLE_POPUP) != false)	// ��ǰ��Host�Ѿ���Popup�����ˣ��Ͳ��ܼ������Ϸ���
				break;
			if (npKeyFocus != NULL)	// �л�Host�����ˣ������ǰ�����û������
				npKeyFocus = NULL;
			lpHost = (CXuiIterator*)lpHost->GetParent();
		}

		lbOK = true;
	}
	catch (...)
	{
	}

	UnlockIterators();

	if (lpMoveTo != NULL)
	{
		// �л����̽���
		ChangeKeyFocus(lpMoveTo);
	}

	return lbOK;
}


//// ��Key��Ϣת��ΪCommand
//bool CXelManager::KeyToCommand(const PSTEMS_KEY_PRESSED npStrike)
//{
//	//ULONG luCmd;
//	bool lbDone = false;
//	CXuiIterator* lpAct = NULL;
//
//	//do
//	//{
//	//	lpAct = GetActiveElement();
//	//	if(lpAct == NULL || lpAct->CheckStyle(EITR_STYLE_KEY_COMMAND)==false)
//	//		break;
//
//	//	// ����Ƿ����CommandҪ��
//	//	switch(npStrike->VirtualKeyCode)
//	//	{
//	//	case VK_F1:
//	//		luCmd = (ULONG)nse_command::eInfo;
//	//		break;
//	//	case VK_ESCAPE:
//	//		luCmd = (ULONG)nse_command::eDeny;
//	//		break;
//	//	case VK_RETURN:
//	//		luCmd = (ULONG)nse_command::eConfirm;
//	//		break;
//	//	default:
//	//		luCmd = (ULONG)nse_command::eInvalid;
//	//	}
//	//	if(luCmd == (ULONG)nse_command::eInvalid && npStrike->VirtualKeyCode != VK_CONTROL && (GetKeyState(VK_CONTROL)&0x8000)!=0)
//	//	{
//	//		switch(npStrike->VirtualKeyCode)
//	//		{
//	//		case VK_LEFT:
//	//			luCmd = (ULONG)nse_command::eMoveLeft;
//	//			break;
//	//		case VK_RIGHT:
//	//			luCmd = (ULONG)nse_command::eMoveRight;
//	//			break;
//	//		case VK_UP:
//	//			luCmd = (ULONG)nse_command::eMoveUp;
//	//			break;
//	//		case VK_DOWN:
//	//			luCmd = (ULONG)nse_command::eMoveDown;
//	//			break;
//	//		case VK_NEXT:
//	//			luCmd = (ULONG)nse_command::eTurnRound;
//	//			break;
//	//		default:;
//	//		}
//	//	}
//	//	if(luCmd == (ULONG)nse_command::eInvalid)
//	//		break;
//
//	//	CExMessage::SendMessage(lpAct,NULL,EMSG_COMMAND,luCmd);
//
//	//	lbDone = true;
//
//	//}while(false);
//
//	//if(lpAct != NULL)
//	//	lpAct->KRelease();
//
//	return lbDone;
//}

// ����Iterator�����ã�����XUI�Ŀͻ�������ܻ���©��Iterator���ͷź����ò���������Ĭ�ϵ�Iterator->AddRef()��Iterator->Release()�����Ǽٵģ����������ʵ�ʵĵ��ã���Element��Close�󣬶�Ӧ��Iteratorһ��
// �ᱻ�ͷţ��ڱ��ӿ����ṩ����ʵ�����ú��ͷŵķ�������Iterator�����м�Ҫ����������������ͷŽ��ᵼ��XUI������
// ����Iterator����
int __stdcall CXelManager::AddRefIterator(IEinkuiIterator* npIterator)
{
	CXuiIterator* lpItrObj;

	if (npIterator == NULL || npIterator->IsKindOf(GET_BUILTIN_NAME(CXuiIterator)) == false)
		return ERESULT_ITERATOR_INVALID;

	lpItrObj = dynamic_cast<CXuiIterator*>(npIterator);
	if (lpItrObj == NULL)
		return ERESULT_ITERATOR_INVALID;

	return lpItrObj->KAddRefer();
}

// �ͷ�Iterator
int __stdcall CXelManager::ReleaseIterator(IEinkuiIterator* npIterator)
{
	CXuiIterator* lpItrObj;

	if (npIterator == NULL || npIterator->IsKindOf(GET_BUILTIN_NAME(CXuiIterator)) == false)
		return ERESULT_ITERATOR_INVALID;

	lpItrObj = dynamic_cast<CXuiIterator*>(npIterator);
	if (lpItrObj == NULL)
		return ERESULT_ITERATOR_INVALID;

	return lpItrObj->KRelease();
}

// �����꽹�㣬!!!ע��!!!�����صĶ���һ��Ҫ����ReleaseIterator�ͷţ�
// ��Ϊ��꽹����ʱ���ܸı䣬���У����صĶ���һ������ȫ��ʵ�ķ�Ӧ��ǰ�������
IEinkuiIterator* __stdcall CXelManager::GetMouseFocus(void)
{
	return InnerGetMouseFocus();
}

// ��ü��̽��㣬!!!ע��!!!�����صĶ���һ��Ҫ����ReleaseIterator�ͷţ�
// ��Ϊ���̽�����ʱ���ܸı䣬���У����صĶ���һ������ȫ��ʵ�ķ�Ӧ��ǰ�������
IEinkuiIterator* __stdcall CXelManager::GetKeyboardFocus(void)
{
	return InnerGetKeyFocus();
}

// ������ק��㣬����ϵͳ��������ק��Ϊ��ʱ�����Խ���תת�Ƹ�����
// �����ͼת�Ƶ���Ŀ��Ԫ�ز���֧����ק��Ϊ����ǰ����ק��ΪҲ����ֹ����ǰ��ק��Ŀ��Ԫ�ػ��յ�Drag_end��Ϣ
ERESULT __stdcall CXelManager::ResetDragBegin(IEinkuiIterator* npToDrag)
{
	CElMouseFootPrint loMouseFoot;
	STMS_DRAGGING_ELE ldDragged;
	CXuiIterator* lpToDrag;

	// ���봦����ק״̬�����ܴ���Drag-drop״̬
	if (mbDragging == false || mbXuiDragDrop != false || npToDrag == NULL)
		return ERESULT_UNEXPECTED_CALL;

	lpToDrag = dynamic_cast<CXuiIterator*>(npToDrag);

	// Ŀ�꽹���Ƿ�֧����ק
	if (lpToDrag->CheckStyle(EITR_STYLE_DRAG) == false)
		return ERESULT_DRAGGING_UNSUPPORT;

	if (moMouseTrace.Size() <= 0)
		return ERESULT_UNEXPECTED_CALL;

	loMouseFoot = moMouseTrace.Back();

	if (mpDragMsgReceiver != NULL)
	{
		// ���������ק������Ϣ
		ldDragged.DragOn = InnerGetMouseFocus();
		CalculateMouseMoving(mpDragMsgReceiver, loMouseFoot.Point, mdDragFrom, ldDragged.Offset);
		ldDragged.ActKey = muKeyWithDragging;
		ldDragged.KeyFlag = moMouseTrace.Back().KeyFlag;

		// ����ֲ�����
		mpDragMsgReceiver->WorldToLocal(loMouseFoot.Point, ldDragged.CurrentPos);

		CExMessage::SendMessage(mpDragMsgReceiver, NULL, EMSG_DRAG_END, ldDragged);
		// ??? ��Ҫ���ldDragged.DragOn�Ƿ���ȷ�ͷ����ã�Ax Jul.8
	}

	mbDragging = false;
	if (mpDragMsgReceiver != NULL)
		mpDragMsgReceiver->KRelease();
	mpDragMsgReceiver = NULL;
	//if(mpMouseFocus != NULL)
	//	mpMouseFocus->KRelease();	???
	//mpMouseFocus = NULL;//lpToDrag;

	DetectMouseDragBegin(lpToDrag, muKeyWithDragging, moMouseTrace.Back().KeyFlag, loMouseFoot.Point);

	return mbDragging != false ? ERESULT_SUCCESS : ERESULT_UNSUCCESSFUL;
}

// ����������ϢLMSG_GET_TYPE(MsgId) == LMSG_TP_WIN_INPUT
void __stdcall CXelManager::CleanHumanInput(bool nbStallInput)
{
	InterlockedExchange(&mlCleanHumanInput, 1);

	if (nbStallInput == false)
		SimplePostMessage(NULL, EMSG_INPUT_ENABLE, NULL, 0, EMSG_POSTTYPE_FAST);
}


// ��ü��̽��㣬��Ҫ�ͷ�
CXuiIterator* CXelManager::InnerGetKeyFocus(void)
{
	CXuiIterator* lpFocus;

	moMFocusLock.Enter();

	lpFocus = mpKbFocus;

	moMFocusLock.Leave();

	if (lpFocus != NULL)
		lpFocus->KAddRefer();

	return lpFocus;
}

// �ı���̽���
void CXelManager::ChangeKeyFocus(CXuiIterator* npNewFocus)
{
	STEMS_STATE_CHANGE ldChange;
	CXuiIterator* lpOld = NULL;
	CXuiIterator* lpNew = NULL;
	HWND lhWnd = FindWindow(L"OskMainClass", NULL);

	if (npNewFocus == NULL && lhWnd != NULL)
		::SendMessage(lhWnd, WM_SYSCOMMAND, SC_CLOSE, NULL);

	moMFocusLock.Enter();

	if (mpKbFocus != npNewFocus)
	{
		lpOld = mpKbFocus;

		mpKbFocus = npNewFocus;
		lpNew = npNewFocus;
		if (mpKbFocus != NULL)
		{
			mpKbFocus->KAddRefer();
			lpNew->KAddRefer();
		}
	}
	else if (npNewFocus != NULL)
	{
		if (EinkuiGetSystem()->IsTouch() != false && lhWnd == NULL)
		{
			SHELLEXECUTEINFO ShExecInfo = { 0 };
			ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
			ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
			ShExecInfo.hwnd = NULL;
			ShExecInfo.lpVerb = NULL;
			ShExecInfo.lpFile = L"Osk";
			ShExecInfo.nShow = SW_SHOW;
			ShExecInfo.hInstApp = NULL;
			ShellExecuteEx(&ShExecInfo);

			CExMessage::PostMessage(npNewFocus, NULL, EMSG_KEYBOARD_SCREEN_KEYBOARD, CExMessage::DataInvalid);
		}
	}

	moMFocusLock.Leave();

	if (lpOld != lpNew)
	{
		if (lpOld != NULL)
		{
			ldChange.State = 0;	// lose
			ldChange.Related = lpNew;
			CExMessage::PostMessage(lpOld, NULL, EMSG_KEYBOARD_FOCUS, ldChange, EMSG_POSTTYPE_FAST);
		}

		if (lpNew != NULL)
		{
			ldChange.State = 1;	// got
			ldChange.Related = lpOld;

			if (EinkuiGetSystem()->IsTouch() != false && lhWnd == NULL)
			{
				SHELLEXECUTEINFO ShExecInfo = { 0 };
				ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
				ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
				ShExecInfo.hwnd = NULL;
				ShExecInfo.lpVerb = NULL;
				ShExecInfo.lpFile = L"Osk";
				ShExecInfo.nShow = SW_SHOW;
				ShExecInfo.hInstApp = NULL;
				ShellExecuteEx(&ShExecInfo);

				CExMessage::PostMessage(npNewFocus, NULL, EMSG_KEYBOARD_SCREEN_KEYBOARD, CExMessage::DataInvalid);
			}

			CExMessage::PostMessage(lpNew, NULL, EMSG_KEYBOARD_FOCUS, ldChange, EMSG_POSTTYPE_FAST);

			// 			if(lpAccProxyObject != NULL)
			// 				lpAccProxyObject->SetEditable(TRUE);

		}

		if (lpNew != NULL && lpNew->CheckStyle(EITR_STYLE_DISABLE_IME) != false)
		{
			CEinkuiSystem::gpXuiSystem->GetImeContext()->DisableIme();
		}
		else
		{
			CEinkuiSystem::gpXuiSystem->GetImeContext()->EnableIme();
		}


	}


	if (lpOld != NULL)
		lpOld->KRelease();
	if (lpNew != NULL)
		lpNew->KRelease();
}

// �����꽹�㣬��Ҫ�ͷ�
CXuiIterator* CXelManager::InnerGetMouseFocus(void)
{
	CXuiIterator* lpFocus;

	moMFocusLock.Enter();

	lpFocus = mpMouseFocus;

	moMFocusLock.Leave();

	if (lpFocus != NULL)
		lpFocus->KAddRefer();

	return lpFocus;
}

// ��û�ԾԪ��
CXuiIterator* CXelManager::GetActiveElement(void)
{
	CXuiIterator* lpAct;

	moMFocusLock.Enter();

	lpAct = mpActivatedElement;

	moMFocusLock.Leave();

	if (lpAct != NULL)
		lpAct->KAddRefer();

	return lpAct;
}

// �ı���꽹��
void CXelManager::ChangeMouseFocus(CXuiIterator* npNewFocus)
{
	CXuiIterator* lpOld;

	moMFocusLock.Enter();

	lpOld = mpMouseFocus;
	mpMouseFocus = npNewFocus;
	if (mpMouseFocus != NULL)
		mpMouseFocus->KAddRefer();

	moMFocusLock.Leave();

	if (lpOld != NULL)
		lpOld->KRelease();

}

// ����Active PopupԪ��
ERESULT CXelManager::AssignActivation(IEinkuiIterator* npToSet)
{
	if (VerifyIterator(npToSet) != ERESULT_SUCCESS)
		return ERESULT_ITERATOR_INVALID;

	CXuiIterator* lpItr = dynamic_cast<CXuiIterator*>(npToSet);
	if (lpItr->CheckStyle(EITR_STYLE_ACTIVABLE) == false)
		return ERETULT_WRONG_STYLE;

	DetectKeyboardFocus(lpItr);

	ChangeActiveElement(lpItr);

	return ERESULT_SUCCESS;
}

// ����Active PopupԪ��
void CXelManager::ChangeActiveElement(CXuiIterator* npSeekFrom)
{
	STEMS_ELEMENT_ACTIVATION ldActivation;
	CXuiIterator* lpOld = NULL;
	CXuiIterator* lpNew = NULL;

	moMFocusLock.Enter();

	lpOld = mpActivatedElement;
	//if(lpOld!=NULL)
	//	lpOld->KAddRefer(); ���������ˣ�����Ҫ�ͷ���

	if (npSeekFrom != NULL)
	{
		while (npSeekFrom != (CXuiIterator*)npSeekFrom->GetParent())
		{
			if (npSeekFrom->CheckStyle(EITR_STYLE_ACTIVABLE) != false)
			{
				mpActivatedElement = npSeekFrom;
				mpActivatedElement->KAddRefer();
				break;
			}
			npSeekFrom = (CXuiIterator*)npSeekFrom->GetParent();
		}
		// ������ڵ㣬û��Ԫ�ػ�ü���ⲻ���ܷ���
		if (npSeekFrom == (CXuiIterator*)npSeekFrom->GetParent())
		{
			mpActivatedElement = NULL;
			lpNew = NULL;

		}
		else
		{
			lpNew = mpActivatedElement;
			lpNew->KAddRefer();		// twice
		}
	}
	else
	{
		mpActivatedElement = NULL;
		lpNew = NULL;
	}


	moMFocusLock.Leave();

	if (lpOld != lpNew)
	{
		ldActivation.Activated = lpNew;
		ldActivation.InActivated = lpOld;

		if (lpOld != NULL && lpOld->IsDeleted() == false)
		{
			ldActivation.State = 0;	// lose
//			ldChange.Related = lpNew;
			CXuiIterator* lpUp = lpOld;

			do
			{
				// ��ǰһ����ԾԪ�أ���������ͬʱʧȥ��Ծ�ĸ�Ԫ�ط���ʧȥ��Ծ��Ϣ
				SimpleSendMessage(lpUp, EMSG_ELEMENT_ACTIVATED, &ldActivation, sizeof(ldActivation), NULL, 0);
				lpUp = (CXuiIterator*)lpUp->GetParent();
				if (lpUp == (CXuiIterator*)lpUp->GetParent() || lpUp->IsDeleted() != false)
					break;
			} while (lpNew == NULL || lpNew->FindAncestor(lpUp) == false); // û���µļ���Ԫ�أ������µļ���Ԫ�ز���Ŀǰ���Ԫ�ص���Ԫ��
		}

		if (lpNew != NULL)
		{
			ldActivation.State = 1;	// got
			//ldChange.Related = lpOld;
			CXuiIterator* lpUp = lpNew;

			do
			{
				// ��ǰһ����ԾԪ�أ��������м��Ԫ�ط��ͼ�����Ϣ
				SimpleSendMessage(lpUp, EMSG_ELEMENT_ACTIVATED, &ldActivation, sizeof(ldActivation), NULL, 0);
				lpUp = (CXuiIterator*)lpUp->GetParent();
				if (lpUp == (CXuiIterator*)lpUp->GetParent() || lpUp->IsDeleted() != false)
					break;
			} while (lpOld == NULL || lpOld->FindAncestor(lpUp) == false); // �ı�״̬ǰû�м���Ԫ�أ�������ǰ�ļ���Ԫ�ز���Ŀǰ���Ԫ�ص���Ԫ��
		}
	}

	if (lpOld != NULL)
		lpOld->KRelease();
	if (lpNew != NULL)
		lpNew->KRelease();
}

// ����ģ̬�Ի�״̬
void CXelManager::EnterModal(void)
{
	// ������ܴ��ڵ�Drag&drop״̬
	TryDropDown(NULL);
}

// ������̽��㣬�����Ԫ�ؾ���popup���ԣ�Ҳ�������������ʵ��ϲ�
ERESULT CXelManager::ApplyKeyBoard(IEinkuiIterator* npIterator)
{
	CXuiIterator* lpAppEle;

	if (npIterator == NULL || npIterator->IsKindOf(GET_BUILTIN_NAME(CXuiIterator)) == false)
		return ERESULT_ITERATOR_INVALID;

	lpAppEle = dynamic_cast<CXuiIterator*>(npIterator);

	if (lpAppEle->CheckStyle(EITR_STYLE_KEYBOARD) == false)
		return ERESULT_ITERATOR_INVALID;

	ChangeKeyFocus(lpAppEle);

	return ERESULT_SUCCESS;
}

// �ͷż��̽��㣬�⽫����Tab Order����һ�����̽����߻�ý���
void CXelManager::ReleaseKeyBoard(PST_RELEASE_KEYFOCUS npRelease)
{
	CXuiIterator* lpKeyFocus;

	do
	{
		lpKeyFocus = InnerGetKeyFocus();
		BREAK_ON_NULL(lpKeyFocus);

		if (lpKeyFocus == npRelease->CrtFocus)
		{
			if (npRelease->ShiftTab != false)
				ShiftKeyBoardFocus(lpKeyFocus);
			else
				ChangeKeyFocus(NULL);

		}

		lpKeyFocus->KRelease();

	} while (false);

}

// ����Mouse wheel��Ϣ�������ǰ��꽹�㲻����������Ϣ����ô���ж��ڵ����һ��Popup(������һ��popup)֮ǰ�Ƿ���EITR_STYLE_ALL_MWHEEL��Ԫ�أ���Ҫ����Mouse Wheel��Ϣ
void CXelManager::TransferMouseWheel(CXuiIterator* npMouseFocus, STEMS_MOUSE_WHEEL& rInfor)
{
	ERESULT luResult = CExMessage::SendMessage(npMouseFocus, NULL, EMSG_MOUSE_WHEEL, rInfor);

	if (luResult != ERESULT_UNEXPECTED_MESSAGE)
		return;

	while (npMouseFocus->CheckStyle(EITR_STYLE_POPUP) == false)
	{
		npMouseFocus = (CXuiIterator*)npMouseFocus->GetParent();

		if (npMouseFocus == (CXuiIterator*)npMouseFocus->GetParent())
			break;

		if (npMouseFocus->CheckStyle(EITR_STYLE_ALL_MWHEEL) != false)
		{
			CExMessage::SendMessage(npMouseFocus, NULL, EMSG_MOUSE_WHEEL, rInfor);
			break;
		}
	}
}

void CXelManager::CalculateMouseMoving(IEinkuiIterator* npOwner, const D2D1_POINT_2F& rCrtPos, const D2D1_POINT_2F& rLastPos, D2D1_POINT_2F& rResult)
{
	D2D1_POINT_2F ldCrt, ldLast;
	rResult.x = rResult.y = 0.0f;	//Ҫ��ʼ������������Чֵ
	if (npOwner->WorldToLocal(rLastPos, ldLast) != false && npOwner->WorldToLocal(rCrtPos, ldCrt) != false)
	{
		rResult.x = ldCrt.x - ldLast.x;
		rResult.y = ldCrt.y - ldLast.y;
	}
}

// ע���ݼ�������ݼ���������ע���ݼ���Element�����ܵ���
// �����ͨ������ϣ�������Alt��)���µĵ�ʱ�����ڼ��̽��㣬������Ϣ�����ȷ��͸����̽��㣬������㷵��ERESULT_KEY_UNEXPECTED�Ż��ж��Ƿ���ڿ�ݼ���Ϊ
bool __stdcall CXelManager::RegisterHotKey(
	IN IEinkuiIterator* npApplicant,	// ע���Ԫ�أ��������յ�ע���ǿ�ݼ���Ϣ
	IN ULONG nuHotKeyID,	// ���ȶ���õĳ�������������Hotkey�����ܳ�����ͬ��ID����ͼע�����е�Hotkey����ʧ��
	IN ULONG nuVkNumber,	// �������
	IN bool nbControlKey,	// �Ƿ���ҪControl���
	IN bool nbShiftKey,		// �Ƿ���ҪShift���
	IN bool nbAltKey,		// �Ƿ���ҪAlt���
	IN IEinkuiIterator* npFocus	// ָ�����㷶Χ��������Ԫ�ؼ�����Ԫ�ػ�ü��̽���ʱ���Żᴥ������ע��Ŀ�ݼ�;
	// ʹ��NULL��Ϊ��������ָ�����㷶Χ�������ۼ��̽����ںδ����ܹ��յ�ע��Ŀ�ݼ�����Ϣ��
)
{
	bool lbOK;

	if (npFocus == NULL)
	{
		moIteratorLock.Enter();
		// ���ӵ�root
		lbOK = moIteratorRoot.RegisterHotKey(npApplicant, nuHotKeyID, nuVkNumber, nbControlKey, nbShiftKey, nbAltKey);
		moIteratorLock.Leave();
	}
	else
	{
		// ���ӵ�������
		CXuiIterator* lpItrObj;
		if (npFocus->IsKindOf(GET_BUILTIN_NAME(CXuiIterator)) == false)
			return false;

		lpItrObj = dynamic_cast<CXuiIterator*>(npFocus);

		moIteratorLock.Enter();
		lbOK = lpItrObj->RegisterHotKey(npApplicant, nuHotKeyID, nuVkNumber, nbControlKey, nbShiftKey, nbAltKey);
		moIteratorLock.Leave();

	}

	return lbOK;
}

// ע����ݼ�
bool __stdcall CXelManager::UnregisterHotKey(
	IN IEinkuiIterator* npApplicant,	// ע����
	IN ULONG UnuKeyNumber
)
{
	// �ݲ�֧��
	return false;
}

// ����������ʵ�Ԫ��
ERESULT CXelManager::SendCommand(nes_command::ESCOMMAND neCmd)
{
	CXuiIterator* lpFocus;
	CXuiIterator* lpFrom;

	lpFocus = InnerGetKeyFocus();
	if (lpFocus == NULL)
	{
		lpFocus = GetActiveElement();
		if (lpFocus == NULL)
		{
			lpFocus = (CXuiIterator*)GetDesktop();
			lpFocus->KAddRefer();
		}
	}

	lpFrom = lpFocus;
	do
	{
		moIteratorLock.Enter();

		do
		{
			if (moIteratorVerification.Find(lpFrom) == moIteratorVerification.End())
			{	// ����Ϊ������ֹ����ִ��
				lpFrom = &moIteratorRoot;
				break;
			}

			if (lpFrom->CheckStyle(EITR_STYLE_COMMAND) != false)
				break;

			lpFrom = (CXuiIterator*)lpFrom->GetParent();
		} while (lpFrom != (CXuiIterator*)lpFrom->GetParent());	// һֱ�ҵ����ڵ���һ��

		moIteratorLock.Leave();

		// �����ǰĿ�겻�����������������Ѱ��Сһ��
		if (CExMessage::SendMessage(lpFrom, NULL, EMSG_COMMAND, neCmd) == ERESULT_SUCCESS)
			break;

		// ����Ѿ��Ǹ��������Ƴ�
		if (lpFrom == (CXuiIterator*)lpFrom->GetParent())
			break;

		lpFrom = (CXuiIterator*)lpFrom->GetParent();

	} while (lpFrom != NULL);

	if (lpFocus != NULL)
		lpFocus->KRelease();

	return ERESULT_SUCCESS;
}
