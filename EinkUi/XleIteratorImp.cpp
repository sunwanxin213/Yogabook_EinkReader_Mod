#include "stdafx.h"

#include "CommonHeader.h"

#include "XleIteratorImp.h"




DEFINE_BUILTIN_NAME(CXuiIterator)
CXelManager* CXuiIterator::gpElementManager = NULL;
CExclusiveAccess CXuiIterator::goAttributesLock;


CXuiIterator::CXuiIterator()
{
	mpExtension = NULL;
	muEID = EID_INVALID;
	muStyle = EITR_STYLE_NONE;
	mpElement = NULL;
	mpParent = NULL;
	mdPosition.x = mdPosition.y = 0;
	mdSize.width = mdSize.height = 0;
	mlTabOrder = -1;
	mlZOrder = -1;

	mbInverted = false;
	// ������Զ���ƶ��ľ���δ����ʱ����жϳ���
	mdWorldMatrix._11 = 1.0f; mdWorldMatrix._12 = 0.0f;
	mdWorldMatrix._21 = 0.0f; mdWorldMatrix._22 = 1.0f;
	mdWorldMatrix._31 = 1000000.0f; mdWorldMatrix._32 = 1000000.0f;
	mdWMInverted._11 = 1.0f;
	mdWMInverted._12 = 0.0f;
	mdWMInverted._21 = 0.0f;
	mdWMInverted._22 = 1.0f;
	mdWMInverted._31 = 0.0f;
	mdWMInverted._32 = 0.0f;

	cmmBaseObject::SetFlags(EITR_FLAG_VISIBLE, true);
	cmmBaseObject::SetFlags(EITR_FLAG_DIRTY, true);
}

CXuiIterator::~CXuiIterator()
{
	if (mpExtension != NULL)
		delete mpExtension;

	if (cmmBaseObject::TestFlag(EITR_FLAG_HOTKEY) != false)
	{
		THotKeyTable* lpHtKyTable = NULL;
		moAtts.GetAttribute('htky', lpHtKyTable);
		CMM_SAFE_DELETE(lpHtKyTable);

	}

	CMM_SAFE_RELEASE(mpElement);
	//if(mpParent != NULL)	// ������Ӹ������Ƴ�ʱ���ã��������Ӷ����ͷ�ʱ����
	//{
	//	mpParent->KRelease();
	//	mpParent = NULL;
	//}
}

// ����Ԫ�أ�����֪ͨһ��Ԫ�����٣�ע�⣺��Ԫ�������յ�����Ϣ��Ӧ�ü�ʱ����Ԫ�ع�������UnregisterElement������
// �Ӷ�����Ԫ�ع�������������һ��Ԫ�ط���������Ϣ�������ٽ��Լ���Ԫ�ع�����ע���������ͷ��������
ERESULT CXuiIterator::OnElementDestroy()
{
	ClearTip();

	return ERESULT_SUCCESS;
}

// ����Iterator��׼��������Ϣ���������������Element���Ȼ��յ�EMSG_CREATE��Ϣ���������ͨ����Element��ʵ���������˳�ǰ����
void __stdcall CXuiIterator::Start(void)
{
	ITR_CHECK();
	if (cmmBaseObject::TestFlag(EITR_FLAG_INIT) != false)
		return;	// �Ѿ���ʼ����

	// ���ó�ʼ����ɱ�־�����ҵ���Ԫ�ع������ķ���
	cmmBaseObject::SetFlags(EITR_FLAG_INIT);
	gpElementManager->StartMessageReceiver(this);
}

// ��ñ�Element��EID
ULONG __stdcall CXuiIterator::GetID(void)
{
	ITR_CHECK();
	return muEID;
}

// ��ñ�Element������Etype
const wchar_t* __stdcall CXuiIterator::GetType(void)
{
	ITR_CHECK();
	return mpElement->GetType();
}

// ��ø�����
IEinkuiIterator* __stdcall CXuiIterator::GetParent(void)
{
	ITR_CHECK();

	CMMASSERT(cmmBaseObject::TestFlag(EITR_FLAG_DELETED) == false);

	return mpParent;
}


// ��ñ���������Ӧ��Element����
IXsElement* __stdcall CXuiIterator::GetElementObject(void)
{
	ITR_CHECK();
	return mpElement;
}

// �����һ����Ӷ��������
int __stdcall CXuiIterator::GetSubElementCount(void)
{
	int liCount;

	ITR_CHECK();
	gpElementManager->LockIterators();

	if (mpExtension != NULL)
		liCount = mpExtension->moZOrder.Size();
	else
		liCount = 0;

	gpElementManager->UnlockIterators();

	return liCount;
}

// ѯ��ĳ��Iterator�ڲ�νṹ���ǲ��ǵ�ǰIterator������
bool __stdcall CXuiIterator::FindAncestor(const IEinkuiIterator* npIsAncestor)
{
	bool lbIsAncestor = false;
	CXuiIterator* lpItrObj;

	ITR_CHECK();

	if (npIsAncestor == this)
		return true;

	lpItrObj = this->mpParent;
	gpElementManager->LockIterators();

	// ������ڵ�
	while (lpItrObj != lpItrObj->mpParent)
	{
		if (lpItrObj == npIsAncestor)
		{
			lbIsAncestor = true;
			break;
		}
		lpItrObj = lpItrObj->mpParent;
	}

	gpElementManager->UnlockIterators();

	return lbIsAncestor;
}

// ͨ��ZOder�����д������ӽڵ㣬���صĽӿ���Ҫ�ͷ�
IEinkuiIterator* __stdcall CXuiIterator::GetSubElementByZOder(
	int niPos	// zero base index value to indicate the position in z-order array
)
{
	IEinkuiIterator* lpReturn = NULL;

	ITR_CHECK();
	gpElementManager->LockIterators();

	if (mpExtension != NULL && mpExtension->moZOrder.Size() > 0 && niPos >= 0 && niPos < mpExtension->moZOrder.Size())
	{
		mpExtension->moZOrder[niPos]->AddRefer();
		lpReturn = mpExtension->moZOrder[niPos];
	}

	gpElementManager->UnlockIterators();

	return lpReturn;
}

// ͨ��ID����ӽڵ㣬���صĽӿ���Ҫ�ͷ�
IEinkuiIterator* __stdcall CXuiIterator::GetSubElementByID(ULONG nuEid)
{
	IEinkuiIterator* lpFound = NULL;

	ITR_CHECK();
	gpElementManager->LockIterators();

	if (mpExtension != NULL && mpExtension->moIDOrder.Size() > 0)
	{
		CEoSubItrNode loToFind;
		loToFind.muID = nuEid;
		int liIndex = mpExtension->moIDOrder.Find(loToFind);
		if (liIndex >= 0)
		{
			lpFound = mpExtension->moIDOrder[liIndex].mpIterator;
			lpFound->AddRefer();
		}
	}

	gpElementManager->UnlockIterators();

	return lpFound;
}

// ����Ԫ�ط���һ����Ϣ������ģʽ��Send
// ��Ϣ���ͺ󣬷�������Ȼ��Ҫ�ͷţ����ϣ���Ը��Ӽ򵥵ķ�ʽ������Ϣ���ο�IXelManager��SimplePostMessage����
ERESULT __stdcall CXuiIterator::SendMessage(IEinkuiMessage* npMsg)
{
	ITR_CHECK();
	return gpElementManager->SendMessage(this, npMsg);
}

// ����Ԫ�ط���һ����Ϣ������ģʽ��Post
// ��Ϣ���ͺ󣬷�������Ȼ��Ҫ�ͷţ����ϣ���Ը��Ӽ򵥵ķ�ʽ������Ϣ���ο�IXelManager��SimplePostMessage����
ERESULT __stdcall CXuiIterator::PostMessage(IEinkuiMessage* npMsg)
{
	ITR_CHECK();
	return gpElementManager->PostMessage(this, npMsg);
}

// ����Ԫ�صĸ�Ԫ�ط���һ����Ϣ�����͵�ģʽ��FastPost
// ��Ϣ���ͺ󣬷�������Ȼ��Ҫ�ͷ�
// !!!ע��!!! �����ڷ������Ǳ���������Ӧ��Ԫ��֮���
ERESULT __stdcall CXuiIterator::PostMessageToParent(IEinkuiMessage* npMsg)
{
	IEinkuiIterator* lpFarther;

	ITR_CHECK();
	lpFarther = GetParent();
	if (lpFarther == NULL || lpFarther == this)	// �������󣬻��ߵִ������û�и��ڵ���
		return ERESULT_ITERATOR_INVALID;

	npMsg->SetMessageSender(this);

	return gpElementManager->PostMessage(lpFarther, npMsg, EMSG_POSTTYPE_FAST);
}

// ���붨ʱ�����������ô����Ķ�ʱ������Ҫע��
ERESULT __stdcall CXuiIterator::SetTimer(
	IN ULONG nuID,	  // ��ʱ��ID
	IN ULONG nuRepeat,// ��Ҫ�ظ������Ĵ�����MAXULONG32��ʾ��Զ�ظ�
	IN ULONG nuDuration,	// ��������
	IN void* npContext//�����ģ������Ŷ�ʱ����Ϣ���͸�������
)
{
	ITR_CHECK();
	return CEinkuiSystem::gpXuiSystem->SetTimer(this, nuID, nuRepeat, nuDuration, npContext);
}

// ���ٶ�ʱ��
ERESULT __stdcall CXuiIterator::KillTimer(
	IN ULONG nuID	  // ��ʱ��ID
)
{
	ITR_CHECK();
	return CEinkuiSystem::gpXuiSystem->KillTimer(this, nuID);
}

// ������Ⱦ��Ч������Ч�����ڸ�ĳ��Ԫ�غ�������Ԫ���ṩ�ض�����Ⱦ����Ч������ѡ��Direct2D��Direct3D��������XUIϵͳ����Ⱦ
// ͬһ��Ԫ����ͬһʱ��ֻ����һ����Ч���ڹ��������ң�ͨ����Ч�����Ƕ��丸Ԫ�ط�������
// ����ERESULT_ACCESS_CONFLICT��ʾ�����Ч��������ͻ����Ч�����ã����ڽ��յ�EMSG_PREPARE_PAINTʱ���������ط������ã��п��ܵ������ش���
ERESULT __stdcall CXuiIterator::SetEnhancer(
	IN IEinkuiIterator* npEnhancer,
	IN bool nbEnable		// true ���ã�false ȡ��
)
{
	ERESULT luResult = ERESULT_SUCCESS;
	IEinkuiIterator* lpCrtEnhancer;

	ITR_CHECK();
	goAttributesLock.Enter();

	if (nbEnable != false)
	{
		if (moAtts.SetAttribute('ehcr', npEnhancer) != false)
			cmmBaseObject::SetFlags(EITR_FLAG_ENHANCER, true);
		else
			luResult = ERESULT_UNSUCCESSFUL;
	}
	else
	{
		if (moAtts.GetAttribute('ehcr', lpCrtEnhancer) != false)
		{
			if (lpCrtEnhancer != npEnhancer)
				luResult = ERESULT_ACCESS_CONFLICT;
			else
			{
				moAtts.DeleteAttribute('ehcr');
			}
		}
		cmmBaseObject::SetFlags(EITR_FLAG_ENHANCER, false);
	}

	goAttributesLock.Leave();

	return luResult;

}

// �����Ч��
IEinkuiIterator* __stdcall CXuiIterator::GetEnhancer(void)
{
	ITR_CHECK();
	if (cmmBaseObject::TestFlag(EITR_FLAG_ENHANCER) == false)
		return NULL;

	IEinkuiIterator* lpCrtEnhancer;

	goAttributesLock.Enter();

	if (moAtts.GetAttribute('ehcr', lpCrtEnhancer) == false)
		lpCrtEnhancer = NULL;
	goAttributesLock.Leave();

	return lpCrtEnhancer;
}

// ���һ���ӽڵ�
ERESULT CXuiIterator::AddSubElement(
	CXuiIterator* npSubElement
)
{
	ERESULT luResult = ERESULT_UNSUCCESSFUL;
	bool lbDirty = false;

	do
	{
		// ���û����չ�ṹ��������չ�ṹ
		if (mpExtension == NULL)
		{
			mpExtension = new CXuiIteratorExtension();
			if (mpExtension == NULL)
				break;
		}

		// ����ID����
		{
			CEoSubItrNode loAddIn;
			loAddIn.muID = npSubElement->muEID;//npSubElement->GetID();
			loAddIn.mpIterator = npSubElement;

			// �����ע��Ԫ��û��EID����ô��Ϊ������һ��
			if (loAddIn.muID == 0 || loAddIn.muID == MAXULONG32)
			{
				ULONG i = 0;
				const ULONG luMax = 0x10000;
				do
				{
					union {
						void* p;
						ULONG d;
					}ldValue;
					ldValue.p = npSubElement->mpElement;
					loAddIn.muID = ((ldValue.d + i++) | 0x80000000);	// ���λ��һ����Ϊ�˷�ֹͬ�������ܼ��������ָ��ID��Ԫ�س�ͻ

				} while (mpExtension->moIDOrder.Find(loAddIn) >= 0 && i < luMax);	// ѭ��16K�Σ���������оͷ���
				if (i >= luMax)
					break;	//ʧ���˳�

				npSubElement->muEID = loAddIn.muID;
			}

			if (mpExtension->moIDOrder.Insert(loAddIn) < 0)
				break;
		}
		lbDirty = true;

		// ���뵽Z-Order
//		BringSubElementToTop(npSubElement);
		InsertToZOder(npSubElement);

		// ���뵽Tab-Order
		if (mpExtension->moTabOder.Insert(-1, npSubElement) < 0)
			break;

		// ���ø�����
		npSubElement->mpParent = this;

		// ���ӱ������������
		KAddRefer();

		// ���سɹ�
		luResult = ERESULT_SUCCESS;

	} while (false);

	if (luResult != ERESULT_SUCCESS && lbDirty != false)
	{
		// �ͷ���Դ
		CEoSubItrNode loAddIn;
		loAddIn.muID = npSubElement->GetID();
		mpExtension->moIDOrder.Remove(loAddIn);

		for (int i = 0; i < mpExtension->moZOrder.Size(); i++)
		{
			if (mpExtension->moZOrder[i] == npSubElement)
			{
				mpExtension->moZOrder.RemoveByIndex(i);
				break;
			}
		}
		for (int i = 0; i < mpExtension->moTabOder.Size(); i++)
		{
			if (mpExtension->moTabOder[i] == npSubElement)
			{
				mpExtension->moTabOder.RemoveByIndex(i);
				break;
			}
		}
	}

	return luResult;
}


// ɾ��һ���ӽڵ�
void CXuiIterator::RemoveSubElement(
	CXuiIterator* npSubElement
)
{
	CEoSubItrNode loRemove;
	bool lbFound = false;
	loRemove.muID = npSubElement->GetID();
	mpExtension->moIDOrder.Remove(loRemove);

	for (int i = 0; i < mpExtension->moZOrder.Size(); i++)
	{
		if (mpExtension->moZOrder[i] == npSubElement)
		{
			mpExtension->moZOrder.RemoveByIndex(i);
			lbFound = true;
			break;
		}
	}

	for (int i = 0; i < mpExtension->moTabOder.Size(); i++)
	{
		if (mpExtension->moTabOder[i] == npSubElement)
		{
			mpExtension->moTabOder.RemoveByIndex(i);
			lbFound = true;
			break;
		}
	}

	// ָ��������ʾû�и��ڵ���
	npSubElement->mpParent = npSubElement;

	// ���ȷʵɾ����һ���ӽڵ㣬��ô�ͼ������������
	if (lbFound != false)
		KRelease();
}

// ���ӽڵ��в���Я��Ŀ��ĵ�����
CXuiIterator* CXuiIterator::SeekIteratorInChild(IXsElement* npElement)
{
	CXuiIterator* lpIteratorObj = NULL;

	for (int i = 0; i < mpExtension->moIDOrder.Size(); i++)
	{
		CMMASSERT(mpExtension->moIDOrder[i].mpIterator != NULL);

		if (mpExtension->moIDOrder[i].mpIterator->mpElement == npElement)
		{
			lpIteratorObj = mpExtension->moIDOrder[i].mpIterator;
			break;
		}

		// �ݹ���ã���������ӽڵ�
		if (mpExtension->moIDOrder[i].mpIterator->mpExtension != NULL)
		{
			lpIteratorObj = mpExtension->moIDOrder[i].mpIterator->SeekIteratorInChild(npElement);
			if (lpIteratorObj != NULL)
				break;
		}

	}

	return lpIteratorObj;
}

// �ر�һ��Ԫ��
void __stdcall CXuiIterator::Close(void)
{
	ITR_CHECK();
	// ע���ؼ�
	CExMessage::SendMessage(NULL, this, EMSG_APPLY_DESTROY, CExMessage::DataInvalid, NULL, 0);
}

// HookĿ�꣬��ǰ��֧�ֵ���ε�Hook������һ��Ԫ����ͬһʱ�̽���һ��Ԫ��Hook����ͼHookһ���Ѿ���Hook��Ԫ��ʱ�����᷵��ʧ��ERESULT_ACCESS_CONFLICT
ERESULT __stdcall CXuiIterator::SetHook(
	IN IEinkuiIterator* npHooker,	// Hook�����ߣ�һ��������Hook���������������Ϣ��EMSG_HOOKED_MESSAGE���ᱻת�����������ȷ��͸�Hooker����Hooker�����޸��������Ϣ��Ҳ������ֹ��Ϣ���͸�������
	IN bool nbSet		// true to set ,false to remove
)
{
	ERESULT luResult = ERESULT_UNSUCCESSFUL;

	ITR_CHECK();
	if (nbSet != FALSE)
	{
		if (cmmBaseObject::TestFlag(EITR_FLAG_HOOK) != false)
			return ERESULT_ACCESS_CONFLICT;
	}
	else
	{
		if (cmmBaseObject::TestFlag(EITR_FLAG_HOOK) == false)
			return ERESULT_SUCCESS;
	}

	goAttributesLock.Enter();

	if (nbSet != FALSE)
	{
		if (moAtts.SetAttribute('hook', npHooker) != false)
			luResult = ERESULT_SUCCESS;
	}
	else
	{
		if (moAtts.DeleteAttribute('hook') != false)
			luResult = ERESULT_SUCCESS;
	}

	if (luResult == ERESULT_SUCCESS)
		cmmBaseObject::SetFlags(EITR_FLAG_HOOK, nbSet);

	goAttributesLock.Leave();

	return luResult;
}

// ���Hooker����ȡ��Ԫ�ر�˭Hook
IEinkuiIterator* __stdcall CXuiIterator::GetHooker(void)
{
	IEinkuiIterator* lpHooker;

	ITR_CHECK();
	if (cmmBaseObject::TestFlag(EITR_FLAG_HOOK) == false)
		return NULL;
	goAttributesLock.Enter();

	if (moAtts.GetAttribute('hook', lpHooker) == false)
		lpHooker = NULL;
	goAttributesLock.Leave();

	return lpHooker;

}


//////////////////////////////////////////////////////////////////////////
// ��������������ʾ��λ����صķ���

// ������ʾ/����
void __stdcall CXuiIterator::SetVisible(bool nbVisible)
{
	ITR_CHECK();
	cmmBaseObject::SetFlags(EITR_FLAG_VISIBLE, nbVisible);
	cmmBaseObject::SetFlags(EITR_FLAG_DIRTY, true);

	CExMessage::SendMessage(this, NULL, EMSG_SHOW_HIDE, nbVisible);

	CEinkuiSystem::gpXuiSystem->UpdateView();
}

// ��ȡ��ʾ/����״̬
bool __stdcall CXuiIterator::IsVisible(void)
{
	ITR_CHECK();
	return cmmBaseObject::TestFlag(EITR_FLAG_VISIBLE) && cmmBaseObject::TestFlag(EITR_FLAG_INIT);
}

// �趨����͸����
void __stdcall CXuiIterator::SetAlpha(FLOAT nfAlpha)
{
	ITR_CHECK();
	mfAlpha = nfAlpha;
	cmmBaseObject::SetFlags(EITR_FLAG_ALPHA, nfAlpha > 0.99f ? false : true);	// �������0.99�൱������ȫ��͸��
	cmmBaseObject::SetFlags(EITR_FLAG_DIRTY, true);

	CEinkuiSystem::gpXuiSystem->UpdateView();
}

// ��ȡ����͸����
FLOAT __stdcall CXuiIterator::GetAlpha(void)
{
	ITR_CHECK();
	if (cmmBaseObject::TestFlag(EITR_FLAG_ALPHA) == false)
		return 1.0f;

	return mfAlpha;
}

// ����ƽ������
void __stdcall CXuiIterator::SetPosition(FLOAT nfX, FLOAT nfY)
{
	ITR_CHECK();
	mdPosition.x = CExFloat::Round(nfX);
	mdPosition.y = CExFloat::Round(nfY);
	cmmBaseObject::SetFlags(EITR_FLAG_DIRTY, true);

	CExMessage::SendMessage(this, NULL, EMSG_ELEMENT_MOVED, mdPosition);

	CEinkuiSystem::gpXuiSystem->UpdateView();
}
void __stdcall CXuiIterator::SetPosition(const D2D1_POINT_2F& rPosition)
{
	ITR_CHECK();
	SetPosition(rPosition.x, rPosition.y);
}


// ��ȡƽ������
FLOAT __stdcall CXuiIterator::GetPositionX(void)
{
	ITR_CHECK();
	return mdPosition.x;
}
FLOAT __stdcall CXuiIterator::GetPositionY(void)
{
	ITR_CHECK();
	return mdPosition.y;
}

// ��ȡƽ������
D2D1_POINT_2F __stdcall CXuiIterator::GetPosition(void)
{
	ITR_CHECK();
	return mdPosition;
}

void __stdcall CXuiIterator::GetRect(D2D1_RECT_F& rRect)
{
	ITR_CHECK();
	rRect.left = mdPosition.x;
	rRect.top = mdPosition.y;
	rRect.right = mdPosition.x + mdSize.width;
	rRect.bottom = mdPosition.y + mdSize.height;
}

// ���ÿ�������
void __stdcall CXuiIterator::SetVisibleRegion(
	IN const D2D1_RECT_F& rRegion		// �����������Ŀ������򣬴�����֮�ⲻ����ʾ��Ԫ�ؼ���Ԫ�ص����ݣ����rRegion.left > region.right ��ʾȡ������������
)
{
	ITR_CHECK();
	// �����ϴ���ĳ��Element�����Ǵ�����Ʋ���ǡ���ڻ�������ʱ�����ô˺������������ܻᵼ�����ع���
	if (CEinkuiSystem::gpXuiSystem->GetRenderStep() != CEinkuiSystem::eRenderStop)
		return;

	goAttributesLock.Enter();

	if (rRegion.left > rRegion.right || rRegion.top > rRegion.bottom)
	{
		moAtts.DeleteAttribute('vrgn');
		cmmBaseObject::SetFlags(EITR_FLAG_VREGION, false);
	}
	else
		if (moAtts.SetAttribute('vrgn', rRegion, true) != false)
			cmmBaseObject::SetFlags(EITR_FLAG_VREGION, true);

	goAttributesLock.Leave();

}

// ��ȡ�������򣬷���false��ʾû�����ÿ�������
bool __stdcall CXuiIterator::GetVisibleRegion(
	OUT D2D1_RECT_F& rRegion	// ���ؿ����������û�����ÿ��������򲻻��޸��������
)
{
	bool lbReval;

	ITR_CHECK();
	if (cmmBaseObject::TestFlag(EITR_FLAG_VREGION) == false)
		return false;

	goAttributesLock.Enter();

	lbReval = moAtts.GetAttribute('vrgn', rRegion);

	goAttributesLock.Leave();

	return lbReval;
}


// ����ƽ��ת��
void __stdcall CXuiIterator::SetRotation(FLOAT nfAngle, D2D1_POINT_2F ndCenter)
{
	// �����ϴ���ĳ��Element�����Ǵ�����Ʋ���ǡ���ڻ�������ʱ�����ô˺������������ܻᵼ�����ع���
	if (CEinkuiSystem::gpXuiSystem->GetRenderStep() != CEinkuiSystem::eRenderStop)
		return;

	ITR_CHECK();
	goAttributesLock.Enter();

	if (nfAngle > -1.0f && nfAngle < 1.0)// ���С��1���൱����û����ת
	{
		cmmBaseObject::SetFlags(EITR_FLAG_ROTATION, false);
		moAtts.DeleteAttribute('rtal');
		moAtts.DeleteAttribute('rtcn');
	}
	else
	{
		cmmBaseObject::SetFlags(EITR_FLAG_ROTATION, true);
		moAtts.SetAttribute('rtal', nfAngle, true);
		moAtts.SetAttribute('rtcn', ndCenter, true);
	}

	goAttributesLock.Leave();

	cmmBaseObject::SetFlags(EITR_FLAG_DIRTY, true);

	CEinkuiSystem::gpXuiSystem->UpdateView();
}

// ����ƽ��ת�ǣ���Ԫ������Ϊ��ת����
void __stdcall CXuiIterator::SetRotation(
	FLOAT nfAngle			// �Ƕȵ�λ -359 -> +359��
)
{
	D2D1_POINT_2F ldCenter;

	ITR_CHECK();
	ldCenter.x = mdSize.width / 2.0f;
	ldCenter.y = mdSize.height / 2.0f;

	goAttributesLock.Enter();

	if (nfAngle > -1.0f && nfAngle < 1.0)// ���С��1���൱����û����ת
	{
		cmmBaseObject::SetFlags(EITR_FLAG_ROTATION, false);

		moAtts.DeleteAttribute('rtal');
		moAtts.DeleteAttribute('rtcn');
	}
	else
	{
		cmmBaseObject::SetFlags(EITR_FLAG_ROTATION, true);
		moAtts.SetAttribute('rtal', nfAngle, true);
		moAtts.SetAttribute('rtcn', ldCenter, true);
	}

	goAttributesLock.Leave();

	cmmBaseObject::SetFlags(EITR_FLAG_DIRTY, true);

	CEinkuiSystem::gpXuiSystem->UpdateView();
}

// ��ȡƽ��ת��
FLOAT __stdcall CXuiIterator::GetRotationAngle(void)
{
	FLOAT lfAngle;

	ITR_CHECK();
	if (cmmBaseObject::TestFlag(EITR_FLAG_ROTATION) == false)
		return 0.0f;

	goAttributesLock.Enter();

	if (moAtts.GetAttribute('rtal', lfAngle) == false)
		lfAngle = 0.0f;

	goAttributesLock.Leave();

	return lfAngle;
}

D2D1_POINT_2F __stdcall CXuiIterator::GetRotationCenter(void)
{
	D2D1_POINT_2F ldCenter = { 0.0f,0.0f };

	ITR_CHECK();
	goAttributesLock.Enter();

	if (cmmBaseObject::TestFlag(EITR_FLAG_ROTATION) == false)
		moAtts.GetAttribute('rtcn', ldCenter);

	goAttributesLock.Leave();

	return ldCenter;
}

FLOAT __stdcall CXuiIterator::GetRotation(D2D1_POINT_2F& rCenter)
{
	FLOAT lfAngle;

	ITR_CHECK();
	if (cmmBaseObject::TestFlag(EITR_FLAG_ROTATION) == false)
		return 0.0f;

	goAttributesLock.Enter();

	if (moAtts.GetAttribute('rtal', lfAngle) == false)
		lfAngle = 0.0f;

	moAtts.GetAttribute('rtcn', rCenter);

	goAttributesLock.Leave();

	return lfAngle;
}

// ���òο��ߴ�
void __stdcall CXuiIterator::SetSize(FLOAT nfCx, FLOAT nfCy)
{
	ITR_CHECK();
	mdSize.width = nfCx;
	mdSize.height = nfCy;

	CExMessage::SendMessage(this, NULL, EMSG_ELEMENT_RESIZED, mdSize);

	cmmBaseObject::SetFlags(EITR_FLAG_DIRTY, true);

	CEinkuiSystem::gpXuiSystem->UpdateView();
}

void __stdcall CXuiIterator::SetSize(const D2D1_SIZE_F& rSize)
{
	ITR_CHECK();
	mdSize = rSize;

	CExMessage::SendMessage(this, NULL, EMSG_ELEMENT_RESIZED, mdSize);

	cmmBaseObject::SetFlags(EITR_FLAG_DIRTY, true);

	CEinkuiSystem::gpXuiSystem->UpdateView();
}

// ��ȡ�ο��ߴ�
FLOAT __stdcall CXuiIterator::GetSizeX(void)
{
	ITR_CHECK();
	return mdSize.width;
}
FLOAT __stdcall CXuiIterator::GetSizeY(void)
{
	ITR_CHECK();
	return mdSize.height;
}

// ��ȡ�ο��ߴ�
D2D1_SIZE_F __stdcall CXuiIterator::GetSize(void)
{
	ITR_CHECK();
	return mdSize;
}

// �����Ƿ���Ч
void __stdcall CXuiIterator::SetEnable(bool nbSet)
{
	ITR_CHECK();
	cmmBaseObject::SetFlags(EITR_FLAG_DISABLE, !nbSet);
	cmmBaseObject::SetFlags(EITR_FLAG_DIRTY, true);

	CExMessage::SendMessage(this, NULL, EMSG_ENALBE_DISABLE, nbSet);

	CEinkuiSystem::gpXuiSystem->UpdateView();
}

// ��ȡ�Ƿ���Ч
bool __stdcall CXuiIterator::IsEnable(void)
{
	return !cmmBaseObject::TestFlag(EITR_FLAG_DISABLE);
}

// ����Style
void __stdcall CXuiIterator::SetStyles(ULONG nuStyles)
{
	ITR_CHECK();
	// �����ϴ���ĳ��Element�����Ǵ�����Ʋ���ǡ���ڻ�������ʱ�����ô˺������������ܻᵼ�����ع���
	if (CEinkuiSystem::gpXuiSystem->GetRenderStep() != CEinkuiSystem::eRenderStop)
		return;

	muStyle = nuStyles;
	cmmBaseObject::SetFlags(EITR_FLAG_DIRTY, true);

	if ((nuStyles&EITR_STYLE_TOP) != 0)
		mpParent->BringSubElementToTop(this);

	CEinkuiSystem::gpXuiSystem->UpdateView();
}

// �޸�Style��ǰһ��������ϣ�����ӵ�Style����һ��������ϣ���Ƴ���Style����ǰ�����������а�����ͬStyleʱ����Style���ᱻ�Ƴ� 
void __stdcall CXuiIterator::ModifyStyles(ULONG nuSet, ULONG nuClear)
{
	ITR_CHECK();
	// �����ϴ���ĳ��Element�����Ǵ�����Ʋ���ǡ���ڻ�������ʱ�����ô˺������������ܻᵼ�����ع���
	if (CEinkuiSystem::gpXuiSystem->GetRenderStep() != CEinkuiSystem::eRenderStop)
		return;

	muStyle &= (~nuClear);
	muStyle |= nuSet;
	cmmBaseObject::SetFlags(EITR_FLAG_DIRTY, true);

	if ((nuSet&EITR_STYLE_TOP) != 0)
		mpParent->BringSubElementToTop(this);

	CEinkuiSystem::gpXuiSystem->UpdateView();
}

// ��ȡStyle
ULONG __stdcall CXuiIterator::GetStyles(void)
{
	ITR_CHECK();
	return muStyle;
}

// ������̽��㣬�����Ԫ�ؾ���popup���ԣ�Ҳ�������������ʵ��ϲ�
ERESULT __stdcall CXuiIterator::SetKeyBoardFocus(void)
{
	ITR_CHECK();
	return CExMessage::SendMessage(NULL, this, EMSG_SET_KEYBOARD_FOCUS, this, NULL, 0);
}

// �ͷż��̽��㣬�⽫����Tab Order����һ�����̽����߻�ý���
void __stdcall CXuiIterator::ReleaseKeyBoardFocus(bool nbShiftFocus)
{
	ST_RELEASE_KEYFOCUS loRelease;

	ITR_CHECK();
	loRelease.CrtFocus = this;
	loRelease.ShiftTab = nbShiftFocus;

	CExMessage::SendMessage(NULL, this, EMSG_RELEASE_KEYBOARD_FOCUS, loRelease, NULL, 0);
}

// ����Ԫ��Ϊ��Ծ�������Ԫ�ز�����EITR_STYLE_POPUP��EITR_STYLE_ACTIVE�����ô��͵�һ������EITR_STYLE_POPUP��EITR_STYLE_ACTIVE�����ϲ�Ԫ�ؽ�������
void __stdcall CXuiIterator::SetActive(void)
{
	ITR_CHECK();
	CExMessage::SendMessage(NULL, this, EMSG_SET_ACTIVE, this, NULL, 0);
}



// ����Ԫ�ص�����ͬ��������߲�
void __stdcall CXuiIterator::BringToTop(void)
{
	ITR_CHECK();
	dynamic_cast<CXuiIterator*>(mpParent)->BringSubElementToTop(this);
}

// ����Ԫ���ڸ�Ԫ�ص�Z Order�����У������ƶ�һλ
bool __stdcall CXuiIterator::MoveDown(void)
{
	bool lbReval;

	ITR_CHECK();
	gpElementManager->LockIterators();

	lbReval = mpParent->MoveOnZOrder(false, this);

	gpElementManager->UnlockIterators();

	return lbReval;
}

// ����Ԫ���ڸ�Ԫ�ص�Z Order�����У������ƶ�һλ
bool __stdcall CXuiIterator::MoveUp(void)
{
	bool lbReval;

	ITR_CHECK();
	gpElementManager->LockIterators();

	lbReval = mpParent->MoveOnZOrder(true, this);

	gpElementManager->UnlockIterators();

	return lbReval;
}

// ����������Ԫ�صĵ��Ŵ���Ϊԭʼ���ã��������ļ��ƶ��Ĵ���
bool __stdcall CXuiIterator::ResetChildrenByDefualtZOrder(void)
{
	ITR_CHECK();
	UpdateOrder();

	return true;
}

// �������ñ�Ԫ�ص�Z Order��ֵ����������ֵ�Element������ͬ��Z Order���޷�ȷ�����ǵ��Ⱥ�ѭ�򣬵�ϵͳ�������в������
void __stdcall CXuiIterator::SetDefaultZOrder(const LONG nlDefaultZOrder)
{
	ITR_CHECK();
	mlZOrder = nlDefaultZOrder;
}

// ���Ĭ�ϵ�ZOrderֵ
LONG __stdcall CXuiIterator::GetDefaultZOrder(void)
{
	ITR_CHECK();
	return mlZOrder;
}



// ������Ԫ����ZOrder��˳��
bool CXuiIterator::MoveOnZOrder(bool nbUp, CXuiIterator* npChild)
{
	int i;
	CXuiIterator* lpSwap;
	bool lbResult = false;

	for (i = 0; i < mpExtension->moZOrder.Size(); i++)
	{
		if (mpExtension->moZOrder[i] == npChild)
		{
			if (nbUp != false)
			{
				if (i + 1 < mpExtension->moZOrder.Size())
				{
					lpSwap = mpExtension->moZOrder[i];
					mpExtension->moZOrder[i] = mpExtension->moZOrder[i + 1];
					mpExtension->moZOrder[i + 1] = lpSwap;
					lbResult = true;
				}
			}
			else
			{
				if (i > 1)
				{
					lpSwap = mpExtension->moZOrder[i];
					mpExtension->moZOrder[i] = mpExtension->moZOrder[i - 1];
					mpExtension->moZOrder[i - 1] = lpSwap;
					lbResult = true;
				}
			}
			break;
		}
	}

	return lbResult;
}


// ��ĳ����Ԫ�ص�����ZOder��߲�
void CXuiIterator::BringSubElementToTop(
	CXuiIterator* npSubElement
)
{
	int i;

	gpElementManager->LockIterators();

	for (i = 0; i < mpExtension->moZOrder.Size(); i++)
	{
		if (mpExtension->moZOrder[i] == npSubElement)
		{
			mpExtension->moZOrder.RemoveByIndex(i);
			break;
		}
	}

	for (i = mpExtension->moZOrder.Size() - 1; i >= 0; i--)
	{
		// ����Ԫ�ز�����Top���ԣ����ߣ���Ԫ��Ҳ����Top����
		if (mpExtension->moZOrder[i]->CheckStyle(EITR_STYLE_TOP) == false || npSubElement->CheckStyle(EITR_STYLE_TOP) != false)
		{
			break;
		}
	}

	mpExtension->moZOrder.Insert(i + 1, npSubElement);

	gpElementManager->UnlockIterators();

}

// ��Ԫ�ز��뵽ZOrder��
void CXuiIterator::InsertToZOder(
	CXuiIterator* npSubElement
)
{
	int i;

	gpElementManager->LockIterators();

	for (i = mpExtension->moZOrder.Size() - 1; i >= 0; i--)
	{
		// ����Ԫ�ز�����Top���ԣ����ߣ���Ԫ��Ҳ����Top����
		if (mpExtension->moZOrder[i]->mlZOrder <= npSubElement->mlZOrder && (mpExtension->moZOrder[i]->CheckStyle(EITR_STYLE_TOP) == false || npSubElement->CheckStyle(EITR_STYLE_TOP) != false))
		{
			break;
		}
	}

	mpExtension->moZOrder.Insert(i + 1, npSubElement);

	gpElementManager->UnlockIterators();

}


// ����TabOrder��ZOrder����ʼ��֮����Element����������
void CXuiIterator::UpdateOrder(void)
{
	int liCount;
	int i, j;
	CXuiIterator* lpSwap;
	bool lbClean;

	if (mpExtension == NULL)
		return;

	gpElementManager->LockIterators();

	// ð������
	liCount = mpExtension->moTabOder.Size();
	for (i = 0; i < liCount - 1; i++)
	{
		lbClean = true;
		for (j = 0; j < liCount - i - 1; j++)
		{
			if (mpExtension->moTabOder[j]->mlTabOrder > mpExtension->moTabOder[j + 1]->mlTabOrder)
			{
				lpSwap = mpExtension->moTabOder[j];
				mpExtension->moTabOder[j] = mpExtension->moTabOder[j + 1];
				mpExtension->moTabOder[j + 1] = lpSwap;
				lbClean = false;
			}
		}
		if (lbClean != false)
			break;
	}

	// ð������
	liCount = mpExtension->moZOrder.Size();
	for (i = 0; i < liCount - 1; i++)
	{
		lbClean = true;
		for (j = 0; j < liCount - i - 1; j++)
		{
			if (mpExtension->moZOrder[j]->mlZOrder > mpExtension->moZOrder[j + 1]->mlZOrder && mpExtension->moZOrder[j]->CheckStyle(EITR_STYLE_TOP) == mpExtension->moZOrder[j + 1]->CheckStyle(EITR_STYLE_TOP) \
				|| mpExtension->moZOrder[j]->CheckStyle(EITR_STYLE_TOP) != false && mpExtension->moZOrder[j + 1]->CheckStyle(EITR_STYLE_TOP) == false)
			{
				lpSwap = mpExtension->moZOrder[j];
				mpExtension->moZOrder[j] = mpExtension->moZOrder[j + 1];
				mpExtension->moZOrder[j + 1] = lpSwap;
				lbClean = false;
			}
		}
		if (lbClean != false)
			break;
	}

	gpElementManager->UnlockIterators();
}

// �����һ��EITR_STYLE_KEYBOARD��Ԫ�أ��������Iterator�ٽ���
CXuiIterator* CXuiIterator::GetNextKeyBoardAccepter(CXuiIterator* npCurrent)
{
	int i;
	int liEnd = 0;//hy 20190723
	CXuiIterator* lpFound = NULL;

	if (mpExtension == NULL || mpExtension->moTabOder.Size() <= 0)
		return NULL;

	// ��λ����ǰԪ�أ����Ҽ��㻷�غ�Ľ�����ַ
	if (npCurrent != NULL)
	{
		for (i = 0; i < mpExtension->moTabOder.Size(); i++)
			if (mpExtension->moTabOder[i] == npCurrent)
			{
				i++;

				if (CheckStyle(EITR_STYLE_POPUP) != false)
					liEnd = i + mpExtension->moTabOder.Size();
				else
					liEnd = mpExtension->moTabOder.Size();

				break;
			}
	}
	else
	{
		i = 0;
		liEnd = mpExtension->moTabOder.Size();
	}


	for (; i < liEnd && lpFound == NULL; i++)
	{
		npCurrent = mpExtension->moTabOder[i%mpExtension->moTabOder.Size()];
		if (npCurrent->CheckStyle(EITR_STYLE_KEYBOARD) != false)
		{
			lpFound = npCurrent;
		}
		else
			if (npCurrent->mpExtension != NULL)
				lpFound = npCurrent->GetNextKeyBoardAccepter(NULL);	// �ݹ���ã���ʹ�ý������ĳ���ӿؼ��ڲ�
	}

	return lpFound;
}

//����ToolTip������ڱ���������ͣ�������Զ���ʾ�ĵ��м��������ʾ�����һ���ƿ���ʾ��ToolTip�����Զ���ʧ
void __stdcall CXuiIterator::SetToolTip(const wchar_t* nswTip)
{
	ERESULT luResult = ERESULT_SUCCESS;
	wchar_t* lswTipText;
	int liSize;

	ClearTip();

	if (nswTip == NULL)
		return;

	liSize = (int)wcslen(nswTip);
	if (liSize <= 0)
		return;

	lswTipText = new wchar_t[liSize + 1];
	wcscpy_s(lswTipText, liSize + 1, nswTip);

	goAttributesLock.Enter();

	if (moAtts.SetAttribute('tip', lswTipText) != false)
		cmmBaseObject::SetFlags(EITR_FLAG_TIP, true);

	goAttributesLock.Leave();
	delete[] lswTipText;//hy 20190723
	lswTipText = NULL;
}

void CXuiIterator::ClearTip()
{
	wchar_t* lswTipText;

	if (cmmBaseObject::TestFlag(EITR_FLAG_TIP) == false)
		return;

	goAttributesLock.Enter();

	lswTipText = NULL;
	moAtts.GetAttribute('tip', lswTipText);

	moAtts.DeleteAttribute('tip');
	cmmBaseObject::SetFlags(EITR_FLAG_TIP, false);

	goAttributesLock.Leave();

	CMM_SAFE_DELETE(lswTipText);

}

// ���Tip
const wchar_t* CXuiIterator::GetToolTip(void)
{
	wchar_t* lswTipText = NULL;

	if (cmmBaseObject::TestFlag(EITR_FLAG_TIP) == false)
		return NULL;

	goAttributesLock.Enter();

	moAtts.GetAttribute('tip', lswTipText);

	goAttributesLock.Leave();

	return lswTipText;
}

//����IME���봰��λ�ã�ndPosition�Ǳ�Ԫ�ؾֲ������е�λ��; ��ԭԪ�ؾ���EITR_STYLE_DISABLE_IME����ʱ��Ч
void __stdcall CXuiIterator::SetIMECompositionWindow(D2D1_POINT_2F ndPosition)
{
	ITR_CHECK();
	if (CheckStyle(EITR_STYLE_DISABLE_IME) == false)
	{
		D2D1_POINT_2F ldPoint;

		if (LocalToWorld(ndPosition, ldPoint) == false)
			return;

		CEinkuiSystem::gpXuiSystem->GetImeContext()->SetImeCompositionWnd(ldPoint);
	}
}


// �Ӿֲ���ַת��Ϊ�����ַ
bool __stdcall CXuiIterator::LocalToWorld(const D2D1_POINT_2F& crLocalPoint, D2D1_POINT_2F& rWorldPoint)
{
	ITR_CHECK();
	rWorldPoint = crLocalPoint * mdWorldMatrix;

	return true;
}

// �������ַת��Ϊ�ֲ���ַ
bool __stdcall CXuiIterator::WorldToLocal(const D2D1_POINT_2F& crWorldPoint, D2D1_POINT_2F& rLocalPoint)
{
	ITR_CHECK();
	// ����ֲ�����
	if (mbInverted == false)
	{
		mdWMInverted = mdWorldMatrix;
		mbInverted = mdWMInverted.Invert();
	}
	if (mbInverted != false)
	{
		rLocalPoint = crWorldPoint * mdWMInverted;

		return true;
	}

	return false;
}

// ע���ݼ�������ݼ���������ע���ݼ���Element�����ܵ���
// �����ͨ������ϣ�������Alt��)���µĵ�ʱ�����ڼ��̽��㣬������Ϣ�����ȷ��͸����̽��㣬������㷵��ERESULT_KEY_UNEXPECTED�Ż��ж��Ƿ���ڿ�ݼ���Ϊ
bool CXuiIterator::RegisterHotKey(
	IN IEinkuiIterator* npApplicant,	// ע���Ԫ�أ��������յ�ע���ǿ�ݼ���Ϣ
	IN ULONG nuHotKeyID,	// ���ȶ���õĳ�������������Hotkey�����ܳ�����ͬ��ID����ͼע�����е�Hotkey����ʧ��
	IN ULONG nuVkNumber,	// �������
	IN bool nbControlKey,	// �Ƿ���ҪControl���
	IN bool nbShiftKey,		// �Ƿ���ҪShift���
	IN bool nbAltKey		// �Ƿ���ҪAlt���
)
{
	bool lbOk = true;

	//CXuiHotkeyEntry loEntry;
	//THotKeyTable* lpHtKyTable = NULL;

	//do 
	//{
	//	// ���û����չ�ṹ��������չ�ṹ
	//	goAttributesLock.Enter();

	//	if(moAtts.GetAttribute('htky',lpHtKyTable)==false)
	//		lpHtKyTable = NULL;

	//	goAttributesLock.Leave();

	//	if(lpHtKyTable == NULL)
	//	{
	//		lpHtKyTable = new THotKeyTable;
	//		if(lpHtKyTable == NULL)
	//			break;

	//		goAttributesLock.Enter();

	//		lbOk = moAtts.SetAttribute('htky',lpHtKyTable);

	//		goAttributesLock.Leave();

	//		if(lbOk == false)
	//		{
	//			delete lpHtKyTable;
	//			break;
	//		}

	//		lbOk = false;
	//	}

	//	// ���ټ����ͬID������ʹ����ͬIDע��
	//	//for(i=0;i<lpHtKyTable->Size();i++)
	//	//{
	//	//	if((*lpHtKyTable)[i].muHotKeyID == nuHotKeyID)
	//	//		break;
	//	//}
	//	//if(i<mpExtension->moHotKey.Size())
	//	//	break;

	//	loEntry.mpApplicant = npApplicant;
	//	loEntry.mcuExKey = 0;

	//	if(nbControlKey != false)
	//		loEntry.mcuExKey |= CXuiHotkeyEntry::eControl;
	//	if(nbShiftKey != false)
	//		loEntry.mcuExKey |= CXuiHotkeyEntry::eShit;
	//	if(nbAltKey != false)
	//		loEntry.mcuExKey |= CXuiHotkeyEntry::eAlt;

	//	loEntry.msuVkCode = (USHORT)nuVkNumber;
	//	loEntry.muHotKeyID = nuHotKeyID;

	//	if(lpHtKyTable->Insert(loEntry)<0)
	//		break;

	//	cmmBaseObject::SetFlags(EITR_FLAG_HOTKEY,true);

	//	lbOk = true;
	//	
	//} while (false);

	return lbOk;
}

// ע����ݼ�
bool CXuiIterator::UnregisterHotKey(
	IN IEinkuiIterator* npApplicant,	// ע����
	IN ULONG UnuKeyNumber
)
{
	// �ݲ�֧��
	return false;
}

// ����Ƿ���з��ϵ�HotKey
bool CXuiIterator::DetectHotKey(
	CXuiHotkeyEntry& rToDetect
)
{
	int liIndex;
	THotKeyTable* lpHtKyTable;

	if (cmmBaseObject::TestFlag(EITR_FLAG_HOTKEY) == false)
		return false;

	goAttributesLock.Enter();

	if (moAtts.GetAttribute('htky', lpHtKyTable) == false)
		lpHtKyTable = NULL;

	goAttributesLock.Leave();

	// ���û����չ�ṹ��������չ�ṹ
	if (lpHtKyTable == NULL)
		return false;

	liIndex = lpHtKyTable->Find(rToDetect);
	if (liIndex >= 0)
	{
		rToDetect = (*lpHtKyTable)[liIndex];

		return true;
	}

	return false;

}

// �����һ���Ʋ�
LONG CXuiIterator::GetNextPaintLevel(LONG nlCrtLevel)
{
	if (cmmBaseObject::TestFlag(EITR_FLAG_PAINTLEVEL_HOST) == false)
	{
		if (nlCrtLevel == -1)
			return 0;
		return nlCrtLevel;
	}
	LONG llLevelCount;

	goAttributesLock.Enter();

	if (moAtts.GetAttribute('plvc', llLevelCount) == false)
		llLevelCount = 1;
	goAttributesLock.Leave();

	if (nlCrtLevel + 1 >= llLevelCount)
		return nlCrtLevel;
	return nlCrtLevel + 1;
}


// ����Ԫ�ؽ������Ʋ㣬���Ʋ���һ���ı���Ԫ�ػ���ʱ���Ŵ���ķ�����ͨ������Ԫ�ذ��մ�����ϵ����Ϊ���νṹ������ʱҲ�ǰ������νṹ�ȸ�����ִ�У�
//		������Ʋ㼼������Ԫ�ؽ��ڲ�ͬ������λ��ƣ�ͬһ�������Ԫ�أ���Ȼ���մ�����ϵ�����νṹ�ȸ�������
//		�����趨����Ļ��Ʋ㣬�����ܴ���max-ulong����������ж�Ҳ�յ����Ʋ��Ӱ�죬��θߵ�Ԫ�����ȱ��ж�������
//		�����һ���Ѿ��趨�˻��Ʋ�ε������е�ĳ��Ԫ���ٴν������Ʋ�Σ���ô���������е�������Ԫ�ؽ����ܵ���һ�����Ʋ�ε�Ӱ�죬�����µĲ�ι�����
//		Ϊ�˱�������뾡���ܱ���ʹ��Ƕ�׵Ļ��Ʋ��
ERESULT __stdcall CXuiIterator::CreatePaintLevel(LONG nlLevelCount)
{
	ERESULT luResult = ERESULT_UNSUCCESSFUL;

	if (cmmBaseObject::TestFlag(EITR_FLAG_PAINTLEVEL_HOST) != false)
		return luResult;	// �Ѿ������˻��Ʋ��

	goAttributesLock.Enter();

	if (moAtts.SetAttribute('plvc', nlLevelCount) != false)
		luResult = ERESULT_SUCCESS;

	goAttributesLock.Leave();

	if (luResult == ERESULT_SUCCESS)
		cmmBaseObject::SetFlags(EITR_FLAG_PAINTLEVEL_HOST, true);

	return luResult;
}

// �����Ԫ�ػ��Ʋ�����
LONG __stdcall CXuiIterator::GetPaintLevelCount(void)
{
	LONG llLevelCount = 0;

	if (cmmBaseObject::TestFlag(EITR_FLAG_PAINTLEVEL_HOST) == false)
		return 0;

	goAttributesLock.Enter();

	moAtts.GetAttribute('plvc', llLevelCount);

	goAttributesLock.Leave();

	return llLevelCount;
}

// ɾ�����Ʋ���趨
ERESULT __stdcall CXuiIterator::DeletePaintLevel(void
	//bool nbClearAll=true		// �����������Ԫ�صĻ��Ʋ���趨
)
{
	ERESULT luResult = ERESULT_UNSUCCESSFUL;

	if (cmmBaseObject::TestFlag(EITR_FLAG_PAINTLEVEL_HOST) == false)
		return true;

	goAttributesLock.Enter();

	if (moAtts.DeleteAttribute('plvc') != false)
		luResult = ERESULT_SUCCESS;

	goAttributesLock.Leave();

	cmmBaseObject::SetFlags(EITR_FLAG_PAINTLEVEL_HOST, false);

	return luResult;
}

// �趨����Ļ��Ʋ��
ERESULT __stdcall CXuiIterator::SetPaintLevel(LONG nlLevel)
{
	ERESULT luResult = ERESULT_UNSUCCESSFUL;

	goAttributesLock.Enter();

	if (moAtts.SetAttribute('plvn', nlLevel) != false)
		luResult = ERESULT_SUCCESS;

	goAttributesLock.Leave();

	if (luResult == ERESULT_SUCCESS)
		cmmBaseObject::SetFlags(EITR_FLAG_CRT_PAINTLEVEL, true);

	return luResult;
}

// �������Ļ��Ʋ��
LONG __stdcall CXuiIterator::GetPaintLevel(void)
{
	LONG llLevel = -1;

	if (cmmBaseObject::TestFlag(EITR_FLAG_CRT_PAINTLEVEL) != false)
	{
		goAttributesLock.Enter();

		moAtts.GetAttribute('plvn', llLevel);

		goAttributesLock.Leave();
	}

	return llLevel;
}
