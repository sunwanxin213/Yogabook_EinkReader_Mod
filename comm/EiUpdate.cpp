/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#include "Compile.h"
#include "eiupdate.h"


CEiLineDiffVector::CEiLineDiffVector()
{
	mlLock = 0;
}


CEiUpdate::CEiUpdate()
{
	muTotalPixels = 0;
}


CEiUpdate::~CEiUpdate()
{
}

int CEiUpdate::GetDiffBlocks(CEiBlocks& nrBlocks)
{
	CEiBlocks loBlocksFound;

	FoundDiffBlocks(loBlocksFound);

	if (loBlocksFound.Size() == 0)
		return 0;

	CombineBlocks(loBlocksFound,nrBlocks);

	return nrBlocks.Size();
}

void CEiUpdate::FoundDiffBlocks(CEiBlocks& nrBlocks)
{
	CEiRowBlock loBlock;
	bool lbFound = false;

	for (int i=0;i < moDiffLines.Size();i++)
	{
		if (moDiffLines[i].Begin >= moDiffLines[i].End)
		{
			// ���ֿ��У�����ǰһ������
			if (lbFound != false)// ���ǵ�һ�У���ǰ�ÿ�
			{
				lbFound = false;
				nrBlocks.Insert(-1, loBlock);
			}

			continue;	
		}

		if (lbFound == false)
		{
			loBlock.x1 = moDiffLines[i].Begin;
			loBlock.x2 = moDiffLines[i].End;
			loBlock.y1 = i;
			loBlock.y2 = i + 1;
			loBlock.Lines = 0;

			lbFound = true;
		}
		else
			loBlock.Add(moDiffLines[i]);

	}

	if (lbFound != false)
		nrBlocks.Insert(-1, loBlock);
}

// ���������С�ķֿ��ںϷ�����Ϊ�˼򵥣����ֻ�ϲ�Ϊ����
void CEiUpdate::CombineBlocks(CEiBlocks& nrDiffBlocks, CEiBlocks& nrComBlocks)
{
	CEiRowBlock loCombinedBlock;
	CEiBlocks loTopDown;
	CEiBlocks loBottomUp;
	LONG liMaxPixels = MAXLONG;
	int liUpBlock = -1;
	int liBottomBlock = -1;

	if (nrDiffBlocks.Size() == 1)
	{	// ֻ��һ��
		nrComBlocks.Insert(-1, nrDiffBlocks.Front());
		return;
	}

#ifdef BIG_UPDATE_AREA
	loCombinedBlock = nrDiffBlocks.Front();
	for (int i = 0; i < nrDiffBlocks.Size(); i++)
	{
		loCombinedBlock.Add(nrDiffBlocks[i]);
	}
	nrComBlocks.Insert(-1, loCombinedBlock);
#else

	loCombinedBlock = nrDiffBlocks.Front();
	for (int i=0;i<nrDiffBlocks.Size();i++)
	{
		loCombinedBlock.Add(nrDiffBlocks[i]);
		loTopDown.Insert(-1, loCombinedBlock);
	}

	loCombinedBlock = nrDiffBlocks.Back();
	for (int i = nrDiffBlocks.Size()-1; i>=0 ;i--)
	{
		loCombinedBlock.Add(nrDiffBlocks[i]);
		loBottomUp.Insert(-1, loCombinedBlock);
	}

	// ����������ϣ��ҵ������С�ķ���
	for (int i = 0,j = loTopDown.Size()- 1 - 1; i < loTopDown.Size() && j>=0; i++,j--)
	{
		LONG liCrtPixels = (loTopDown[i].x2 - loTopDown[i].x1)*(loTopDown[i].y2 - loTopDown[i].y1) + \
			(loBottomUp[j].x2 - loBottomUp[j].x1)*(loBottomUp[j].y2 - loBottomUp[j].y1);
		if (liCrtPixels < liMaxPixels)
		{
			liMaxPixels = liCrtPixels;
			liUpBlock = i;
			liBottomBlock = j;
		}
	}

	// �жϷֿ��Ƿ�������
	if ((liMaxPixels * 12) / 10 < (loTopDown.Back().x2 - loTopDown.Back().x1)*(loTopDown.Back().y2 - loTopDown.Back().y1) && \
			(ULONG)(liMaxPixels*10) < muTotalPixels)
	{
		// �ֿ������20%�����ݣ����������С����Ļ��1/10(��ͼ���Ϊ���λ���Ч�ʸ��ͣ�
		nrComBlocks.Insert(-1, loTopDown[liUpBlock]);
		nrComBlocks.Insert(-1, loBottomUp[liBottomBlock]);
	}
	else
	{
		nrComBlocks.Insert(-1, loTopDown.Back());
	}
#endif

}

