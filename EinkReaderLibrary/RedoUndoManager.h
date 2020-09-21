/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#pragma once

/*
	audo redo����ģ��
*/

#include "EdDoc.h"
#include "cmmstruct.h"


#define RUM_TYPE_INK 1
#define RUM_TYPE_HIGHLIGHT 2
#define RUM_TYPE_DELETE_LINE 3
#define RUM_TYPE_UNDER_LINE 4


class CRedoUndoItem {

public:
	bool IsDelete; //true��ʾ��������ӣ���֮��ɾ��
	int PageNumber; //��������ҳ��
	ULONGLONG PenItemSignature; //�ʼ�����ǩ��
	buf_ptr PenByteData; //���к�ıʼ�
	uint32Eink PenByteDataSize; //���к�ıʼ���С
	int AnnotType; //����

	CRedoUndoItem() {
		PageNumber = -1;
		PenItemSignature = 0;
		PenByteData = NULL;
		PenByteDataSize = 0;
		AnnotType = 0;
	}

	CRedoUndoItem(const CRedoUndoItem& src) {
		*this = src;
	}

	void operator = (const CRedoUndoItem& src) {
		IsDelete = src.IsDelete;
		PageNumber = src.PageNumber;
		PenItemSignature = src.PenItemSignature;
		PenByteData = src.PenByteData;
		PenByteDataSize = src.PenByteDataSize;
		AnnotType = src.AnnotType;
	}

};

typedef cmmVector<CRedoUndoItem, 0, 128> CRedoUndoStrokeList;

class CRedoUndoManager
{

public:

	CRedoUndoManager(void);
	~CRedoUndoManager(void);

	//���redo����
	void AddRedo(CRedoUndoStrokeList* npdRedo);
	//���undo����
	void AddUndo(CRedoUndoStrokeList* npdUndo);
	//��ȡredo����
	CRedoUndoStrokeList* GetRedo(int& rSize);
	//��ȡundo����
	CRedoUndoStrokeList* GetUndo(int& rSize);
	//���redo����
	void ClearRedo(void);
	//���undo����
	void ClearUndo(void);
	//��ȡredo�����С
	int GetRedoCount(void);
	//��ȡuedo�����С
	int GetUndoCount(void);

private:
	cmmStack<CRedoUndoStrokeList*> mdRedoStack;
	cmmStack<CRedoUndoStrokeList*> mdUndoStack;


};

