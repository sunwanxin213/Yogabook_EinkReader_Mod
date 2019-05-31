/* Copyright 2019 - present Lenovo */
/* License: COPYING.GPLv3 */
#include "stdafx.h"
#include "BufferMgr.h"


CBufferMgr::CBufferMgr()
{
	mpBufferBase = NULL;
	mulSize = 0;
	mulBlockSize = 0;
}


CBufferMgr::~CBufferMgr()
{
	for (int i = 0; i < mdBufferList.Size(); i++)
		if (mdBufferList[i] != NULL)
			delete mdBufferList[i];
}

//�����ڴ���ʼ��ַ��������С
//nulBufferSizeΪBuffer��С
//nulBlockSizeΪÿ��Ҫ����Ĵ�С
void CBufferMgr::Initialize(const BYTE* npBuffer, ULONG nulBufferSize, ULONG nulWidth, ULONG nulHeight)
{
	do 
	{
		mpBufferBase = npBuffer;
		mulSize = nulBufferSize;
		mulBlockSize = nulWidth*nulHeight;

		//��ʼ�����ڴ��
		ULONG lulOffset = 0;

		while ((lulOffset+ mulBlockSize) < nulBufferSize)
		{
			//��ʼ��ַ
			BYTE* lpBufferBase = (BYTE*)npBuffer + lulOffset;

			//ת���ɶ�Ӧ�ṹ��
			EI_BUFFER* lpEiBuffer = (EI_BUFFER*)lpBufferBase;
			lpEiBuffer->ulBufferSize = mulBlockSize;
			lpEiBuffer->ulWidth = nulWidth;
			lpEiBuffer->ulHeight = nulHeight;

			BUFFER_BLOCK* lpdBufferBlock = new BUFFER_BLOCK();
			lpdBufferBlock->EiBuffer = lpEiBuffer;
			lpdBufferBlock->IsFree = true;
			lpdBufferBlock->BufferOffset = lulOffset;

			mdBufferList.Insert(-1, lpdBufferBlock);

			//ƫ��Ҫ���Ͻṹ���С
			lulOffset += mulBlockSize + sizeof(EI_BUFFER) + 1;
		}

	} while (false);
	
}

//��ȡBuffer,������Ҫʱ��Ҫ����FreeBuffer
//��������Buffer��ַ�����Buffer�Ѿ������򷵻�NULL
EI_BUFFER* CBufferMgr::GetBuffer()
{
	EI_BUFFER* lpRetBuffer = NULL;

	do 
	{
		for (int i=0;i<mdBufferList.Size();i++)
		{
			BUFFER_BLOCK* lpdBufferBlock = mdBufferList.GetEntry(i);
			if (lpdBufferBlock->IsFree != false)
			{
				lpdBufferBlock->IsFree = false;
				lpRetBuffer = (EI_BUFFER*)lpdBufferBlock->EiBuffer;
				break;
			}
		}

	} while (false);

	return lpRetBuffer;
}

//�ͷ�Buffer
bool CBufferMgr::FreeBuffer(const EI_BUFFER* npBuffer)
{
	bool lbRet = false;

	do
	{
		for (int i = 0; i < mdBufferList.Size(); i++)
		{
			BUFFER_BLOCK* lpdBufferBlock = mdBufferList.GetEntry(i);
			if (lpdBufferBlock->EiBuffer == npBuffer)
			{
				lpdBufferBlock->IsFree = true;

				break;
			}
		}

		lbRet = true;

	} while (false);

	return lbRet;
}

//��ȡBuffer��offset
ULONG CBufferMgr::GetBufferOffset(const EI_BUFFER* npBuffer)
{
	ULONG lbRet = 0;

	do
	{
		for (int i = 0; i < mdBufferList.Size(); i++)
		{
			BUFFER_BLOCK* lpdBufferBlock = mdBufferList.GetEntry(i);
			if (lpdBufferBlock->EiBuffer == npBuffer)
			{
				lbRet = lpdBufferBlock->BufferOffset;

				break;
			}
		}

	} while (false);

	return lbRet;
}