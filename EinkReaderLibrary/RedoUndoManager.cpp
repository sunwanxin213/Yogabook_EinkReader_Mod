/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#include "StdAfx.h"
#include "RedoUndoManager.h"
#include "stdio.h"
#include "cmmStrHandle.h"
#include "cmmPath.h"
#include "MsgDefine.h"
#include <Shlobj.h>


CRedoUndoManager::CRedoUndoManager(void)
{

}


CRedoUndoManager::~CRedoUndoManager(void)
{
	//���Redo����
	ClearRedo();

	//���Redo����
	ClearUndo();

}

//���redo����
void CRedoUndoManager::AddRedo(CRedoUndoStrokeList* npdRedo)
{
	mdRedoStack.Push(npdRedo);
}

//���undo����
void CRedoUndoManager::AddUndo(CRedoUndoStrokeList* npdUndo)
{
	mdUndoStack.Push(npdUndo);
}

//��ȡredo�����С
int CRedoUndoManager::GetRedoCount(void)
{
	return mdRedoStack.Size();
}

//��ȡuedo�����С
int CRedoUndoManager::GetUndoCount(void)
{
	return mdUndoStack.Size();
}

//���redo����
void CRedoUndoManager::ClearRedo(void)
{
	for (int j = 0; j < mdRedoStack.Size(); j++)
	{
		CRedoUndoStrokeList* lpList = mdRedoStack.GetEntry(j);
		for (int i = 0; i < lpList->Size(); i++)
		{
			if (lpList->GetEntry(i).PenByteData != NULL)
				delete lpList->GetEntry(i).PenByteData;
		}
		lpList->Clear();
	}
	mdRedoStack.Clear();
}

//���undo����
void CRedoUndoManager::ClearUndo(void)
{
	for (int j = 0; j < mdUndoStack.Size(); j++)
	{
		CRedoUndoStrokeList* lpList = mdUndoStack.GetEntry(j);
		for (int i = 0; i < lpList->Size(); i++)
		{
			if (lpList->GetEntry(i).PenByteData != NULL)
				delete lpList->GetEntry(i).PenByteData;
		}
		lpList->Clear();
	}
	mdUndoStack.Clear();
}

//��ȡredo����
CRedoUndoStrokeList* CRedoUndoManager::GetRedo(int& rSize)
{
	CRedoUndoStrokeList* lpRet = NULL;
	rSize = mdRedoStack.Size();
	if (rSize > 0)
	{
		lpRet = mdRedoStack.Top();
		mdRedoStack.Pop();
	}

	return lpRet;
}

//��ȡundo����
CRedoUndoStrokeList* CRedoUndoManager::GetUndo(int& rSize)
{
	CRedoUndoStrokeList* lpRet = NULL;
	rSize = mdUndoStack.Size();
	if (rSize > 0)
	{
		lpRet = mdUndoStack.Top();
		mdUndoStack.Pop();
	}

	return lpRet;
}