/* Copyright 2019 - present Lenovo */
/* License: COPYING.GPLv3 */
#pragma once
//����APP��ʾ�ڴ�
#include "cmmstruct.h"
#include "EinkIteAPI.h"

class CBufferMgr
{
public:
	CBufferMgr();
	~CBufferMgr();

	typedef struct _BUFFER_BLOCK
	{
		volatile bool IsFree;
		EI_BUFFER* EiBuffer;
		ULONG BufferOffset;
	}BUFFER_BLOCK,*PBUFFER_BLOCK;

	//�����ڴ���ʼ��ַ��������С
	//nulBufferSizeΪBuffer��С
	//nulWidthΪÿ��Ҫ����Ĵ�С�Ŀ��
	//nulHeightΪÿ��Ҫ����Ĵ�С�ĸ߶ȶ�
	void Initialize(const BYTE* npBuffer,ULONG nulBufferSize, ULONG nulWidth,ULONG nulHeight);
	//��ȡBuffer,������Ҫʱ��Ҫ����FreeBuffer
	//��������Buffer��ַ�����Buffer�Ѿ������򷵻�NULL
	EI_BUFFER* GetBuffer();
	//�ͷ�Buffer
	bool FreeBuffer(const EI_BUFFER* npBuffer);
	//��ȡBuffer��offset
	ULONG GetBufferOffset(const EI_BUFFER* npBuffer);

private:
	//������Լ����ڴ���ʼ��ַ
	const BYTE* mpBufferBase;
	//BUFFER�ܴ�С
	ULONG mulSize;
	//������С
	ULONG mulBlockSize;

	//Buffer��
	cmmVector<BUFFER_BLOCK*> mdBufferList;
};

