#include "stdafx.h"

#include "CommonHeader.h"
#include "XsWgtContext.h"




void CXsWgtContext::PushWidget(IXsWidget* npWidget)
{
	CLsWgtcNode ldNode;

	ldNode.SetTickCount();
	ldNode.mpWidget = dynamic_cast<CXsWidget*>(npWidget);

	moLock.Enter();
	moStack.Push(ldNode);

#ifdef _DEBUG
	if (miMaxDepth < moStack.Size())
		miMaxDepth = moStack.Size();
#endif//_DEBUG

	moLock.Leave();
}

IXsWidget* CXsWgtContext::GetTopWidget(void)
{
	CXsWidget* lpWidget;

	moLock.Enter();
	lpWidget = moStack.Top().mpWidget;
	moLock.Leave();

	return lpWidget;
}

void CXsWgtContext::PopWidget(void)
{
	moLock.Enter();

	if (moStack.Size() > 1)	// �������һ������
	{
		moStack.Pop();
	}
	moStack.Top().SetTickCount();	// ��������Tick��

	moLock.Leave();
}

ULONG CXsWgtContext::CheckElapsedTick(void)
{
	ULONG luElapsed = GetTickCount();
	ULONG luFrom;


	moLock.Enter();

	if (mbDetectionTick != false)
	{
		luElapsed = GetTickCount();
		luFrom = moStack.Top().muTickReached;
		if (luElapsed < luFrom)
		{
			moStack.Top().muTickReached = luFrom = luElapsed;
		}

		luElapsed -= luFrom;
	}
	else
		luElapsed = 0;

	moLock.Leave();

	return luElapsed;
}

void CXsWgtContext::EnableTickDetection(bool nbEnalbe)
{
	int liIndex;

	if (nbEnalbe == mbDetectionTick)
		return;

	moLock.Enter();

	if (nbEnalbe == false)
	{
		// �����еļ�¼��TickReached����Ϊ���ֵ������ϵͳ�ָ����к󣬵õ����������ʱ��
		for (liIndex = 0; liIndex < moStack.Size(); liIndex++)
		{
			moStack.GetEntry(liIndex).muTickReached = (ULONG)-1;
		}
	}

	mbDetectionTick = nbEnalbe;

	moLock.Leave();
}


int CXsWgtContext::GetStackDepth(void)
{
	return moStack.Size();
}

bool CXsWgtContext::HasTriedPulling(void)
{
	return moStack.Top().mbTriedPullOut;
}

void CXsWgtContext::SetTriedPulling(void)
{
	moStack.Top().mbTriedPullOut = true;
}

