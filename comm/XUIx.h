/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#pragma once
/*	FileName:	XuiX.h
	Comment:	���ļ��ṩ��Xuiϵͳ���õļ���װ
	History:
			Ax Created Nov.16,2011
			Pangxie modified to support Eink UI
*/
#include "math.h"
#include "Einkui.h"



// ������������ṩ��ݵķ���XUI��Ϣϵͳ�ķ���������������Ϣ���ʹ���Ϣ��ȡ����
class CExMessage{
private:
	static IXelManager* gpElementManager;
	static __inline void GetElementManager(void);

	static __inline IEinkuiMessage* MakeUpMessage(IEinkuiIterator* npSender,bool nbSend,ULONG nuMsgID,const void* npInBuffer,int niInBytes,void* npOutBuffer,int niOutBytes);

public:
	static void* DataInvalid;


	// Postģʽ������Ϣ������ֱ�ӷ���Я���Ĳ�����ע�⣬���������һ��ָ�룬��ֻ�Ḵ�ƴ���ָ�루һ����ַ�����������Ḵ�ƴ���ָ��ָ������ݣ�Post������Ϣ��Ҫע��������⣬��ֹ��ַ����Ϣ�����߳��޷����ʡ�
	// ���û�����ݷ��ͣ�rDataToPost��дCExMessage::DataInvalid;		nuFast��Ϣ�����ȼ���EMSG_POST_NORMAL,EMSG_POST_FAST,EMSG_POST_REVERSE
	template<typename DataType>
	static ERESULT PostMessage(IEinkuiIterator* npReceiver,IEinkuiIterator* npSender,ULONG nuMsgID,const DataType& rDataToPost,ULONG nuFast=EMSG_POSTTYPE_NORMAL);

	// Sendģʽ������Ϣ������ֱ�ӷ���Я���Ĳ�����ע�⣬���������һ��ָ�룬��ֻ�Ḵ�ƴ���ָ�루һ����ַ�����������Ḵ�ƴ���ָ��ָ������ݣ�Sendģʽ������Ϣʱ���ⲻ�ᵼ����������
	// ���û�����ݷ��ͣ�rDataToPost��дCExMessage::DataInvalid
	template<typename DataType>
	static ERESULT SendMessage(IEinkuiIterator* npReceiver,IEinkuiIterator* npSender,ULONG nuMsgID,const DataType& rDateForSend,void* npBufferForReceive=NULL,int niBytesOfBuffer=0);

	// Postģʽ������Ϣ������Я��һ���ַ�����ע�⣬�ַ���һ��Ҫ����\0��β
	// ���û�����ݷ��ͣ�rDataToPost��дCExMessage::DataInvalid;		nuFast��Ϣ�����ȼ���EMSG_POST_NORMAL,EMSG_POST_FAST,EMSG_POST_REVERSE
	static ERESULT PostMessageWithText(IEinkuiIterator* npReceiver,IEinkuiIterator* npSender,ULONG nuMsgID,const wchar_t* nswText,ULONG nuFast=EMSG_POSTTYPE_NORMAL);

	// Sendģʽ������Ϣ������Я��һ���ַ�����ע�⣬�ַ���һ��Ҫ����\0��β
	// ���û�����ݷ��ͣ�rDataToPost��дCExMessage::DataInvalid
	static ERESULT SendMessageWithText(IEinkuiIterator* npReceiver,IEinkuiIterator* npSender,ULONG nuMsgID,const wchar_t* nswText,void* npBufferForReceive=NULL,int niBytesOfBuffer=0);

	// Postģʽ������Ϣ������Я������������
	// ���û�����ݷ��ͣ�rDataToPost��дCExMessage::DataInvalid;		nuFast��Ϣ�����ȼ���EMSG_POST_NORMAL,EMSG_POST_FAST,EMSG_POST_REVERSE
	static ERESULT PostMessageWithBuffer(IEinkuiIterator* npReceiver,IEinkuiIterator* npSender,ULONG nuMsgID,const void* npBuffer,int niBytes,ULONG nuFast=EMSG_POSTTYPE_NORMAL);

	// û�б�Ҫ�ṩSendģʽ���ͻ��������ݵķ�����ֱ�ӷ��͵�ַ��ȥ�ͺ��ˡ���Ϊ������Ϣ����ʱ���ڴ治��ʧЧ
	//ERESULT SendMessageWithBuffer(...);

	// ��ȡ����������ɹ���ý�����ERESULT_SUCCESS
	template<typename DataType>
	static ERESULT GetInputData(IEinkuiMessage* npMsg,DataType& rVariable);

	// ��ȡ����������ɹ���ý�����ERESULT_SUCCESS������õ���ֵ��rInvalidValue��ͬ��������ERESULT_WRONG_PARAMETERS
	template<typename DataType>
	static ERESULT GetInputData(IEinkuiMessage* npMsg,DataType& rVariable,const DataType& rInvalidValue);

	// ��ȡ��������������������һ���ṹ��ʱ��Ҳ����ֱ�ӻ�ȡ��������ָ�룻ʧ�ܷ���NULL
	template<typename DataType>
	static ERESULT GetInputDataBuffer(IEinkuiMessage* npMsg,DataType*& rBufferPointer);
};


void CExMessage::GetElementManager(void){
	if(gpElementManager == NULL)
	{
		gpElementManager = EinkuiGetSystem()->GetElementManager();
	}
}

IEinkuiMessage* CExMessage::MakeUpMessage(IEinkuiIterator* npSender,bool nbSend,ULONG nuMsgID,const void* npInBuffer,int niInBytes,void* npOutBuffer,int niOutBytes){

	GetElementManager();

	IEinkuiMessage* lpMsgIntf = gpElementManager->AllocateMessage();
	if(lpMsgIntf == NULL)
		return NULL;

	lpMsgIntf->SetMessageID(nuMsgID);
	if(npSender != NULL)
		lpMsgIntf->SetMessageSender(npSender);

	if(npInBuffer != NULL)
	{
		bool lbVolatile;

		// ��������������㹻�󣬲��һ���������ͻ���Ͳ��������뻺������
		if(nbSend == false || niInBytes < 64 || npInBuffer == NULL || ( npOutBuffer != NULL &&
			((UCHAR*)npInBuffer+niInBytes) >= (UCHAR*)npOutBuffer &&
			(UCHAR*)npInBuffer			   < ((UCHAR*)npOutBuffer+niOutBytes)) )
			lbVolatile = true;
		else
			lbVolatile = false;

		lpMsgIntf->SetInputData(npInBuffer,niInBytes);
	}
	if(npOutBuffer != NULL)
		lpMsgIntf->SetOutputBuffer(npOutBuffer,niOutBytes);

	return lpMsgIntf;
}


// Postģʽ������Ϣ������ֱ�ӷ���Я���Ĳ�����ע�⣬���������һ��ָ�룬��ֻ�Ḵ�ƴ���ָ�루һ����ַ�����������Ḵ�ƴ���ָ��ָ������ݣ�Post������Ϣ��Ҫע��������⣬��ֹ��ַ����Ϣ�����߳��޷����ʡ�
// ���û�����ݷ��ͣ�rDataToPost��дCExMessage::DataInvalid;		nuFast��Ϣ�����ȼ���EMSG_POST_NORMAL,EMSG_POST_FAST,EMSG_POST_REVERSE
template<typename DataType>
ERESULT CExMessage::PostMessage(IEinkuiIterator* npReceiver,IEinkuiIterator* npSender,ULONG nuMsgID,const DataType& rDataToPost,ULONG nuFast)
{
	ERESULT luResult;
	IEinkuiMessage* lpMsgIntf;

	if((void*)&rDataToPost != (void*)&DataInvalid)
		lpMsgIntf = MakeUpMessage(npSender,false,nuMsgID,&rDataToPost,sizeof(rDataToPost),NULL,0);
	else
		lpMsgIntf = MakeUpMessage(npSender,false,nuMsgID,NULL,0,NULL,0);

	if(lpMsgIntf == NULL)
		return ERESULT_INSUFFICIENT_RESOURCES;

	luResult = gpElementManager->PostMessage(npReceiver,lpMsgIntf,nuFast);

	lpMsgIntf->Release();

	return luResult;
}

// Sendģʽ������Ϣ������ֱ�ӷ���Я���Ĳ�����ע�⣬���������һ��ָ�룬��ֻ�Ḵ�ƴ���ָ�루һ����ַ�����������Ḵ�ƴ���ָ��ָ������ݣ�Sendģʽ������Ϣʱ���ⲻ�ᵼ����������
// ���û�����ݷ��ͣ�rDataToPost��дCExMessage::DataInvalid
template<typename DataType>
ERESULT CExMessage::SendMessage(IEinkuiIterator* npReceiver,IEinkuiIterator* npSender,ULONG nuMsgID,const DataType& rDateForSend,void* npBufferForReceive,int niBytesOfBuffer)
{
	ERESULT luResult;
	IEinkuiMessage* lpMsgIntf;

	if((void*)&rDateForSend != (void*)&DataInvalid)
		lpMsgIntf = MakeUpMessage(npSender,true,nuMsgID,&rDateForSend,sizeof(rDateForSend),npBufferForReceive,niBytesOfBuffer);
	else
		lpMsgIntf = MakeUpMessage(npSender,true,nuMsgID,NULL,0,npBufferForReceive,niBytesOfBuffer);

	if(lpMsgIntf == NULL)
		return ERESULT_INSUFFICIENT_RESOURCES;

	luResult = gpElementManager->SendMessage(npReceiver,lpMsgIntf);

	lpMsgIntf->Release();

	return luResult;
}

// ��ȡ����������ɹ���ý�����ERESULT_SUCCESS
template<typename DataType>
ERESULT CExMessage::GetInputData(IEinkuiMessage* npMsg,DataType& rVariable)
{
	if(npMsg->GetInputDataSize() != sizeof(rVariable) || npMsg->GetInputData()==NULL)
		return ERESULT_WRONG_PARAMETERS;

	rVariable = *(DataType*)npMsg->GetInputData();

	return ERESULT_SUCCESS;
}

// ��ȡ����������ɹ���ý�����ERESULT_SUCCESS������õ���ֵ��rInvalidValue��ͬ��������ERESULT_WRONG_PARAMETERS
template<typename DataType>
ERESULT CExMessage::GetInputData(IEinkuiMessage* npMsg,DataType& rVariable,const DataType& rInvalidValue) 
{
	ERESULT luResult = CExMessage::GetInputData(npMsg,rVariable);

	if(luResult == ERESULT_SUCCESS && rVariable == rInvalidValue)
		return ERESULT_WRONG_PARAMETERS;

	return ERESULT_SUCCESS;
}

// ��ȡ��������������������һ���ṹ��ʱ��Ҳ����ֱ�ӻ�ȡ��������ָ�룻ʧ�ܷ���NULL
template<typename DataType>
ERESULT CExMessage::GetInputDataBuffer(IEinkuiMessage* npMsg,DataType*& rBufferPointer){
	if(npMsg->GetInputDataSize() != sizeof(*rBufferPointer) || npMsg->GetInputData()==NULL)
		return ERESULT_WRONG_PARAMETERS;

	rBufferPointer = (DataType*)npMsg->GetInputData();

	return ERESULT_SUCCESS;
}



//////////////////////////////////////////////////////////////////////////
// ������ʾWindows����Ϣ�Ի���
DECLARE_BUILTIN_NAME(CExWinPromptBox)
class CExWinPromptBox : public cmmBaseObject<CExWinPromptBox,IBaseObject,GET_BUILTIN_NAME(CExWinPromptBox)>
{
public:
	//CExWinPromptBox(){}
	//~CExWinPromptBox(){}

	// ��ʾWindows��MessageBox������ͬWindows����ȫһ�£��÷��磺CExWinPromptBox::MessageBox(L"������",L"Ax",MB_OK);
	static int MessageBox(const wchar_t* nswText,const wchar_t* nswTitle,UINT nuType);

	// ��ʾWindows��MessageBoxEx������ͬWindows����ȫһ�£�
	static int MessageBoxEx(const wchar_t* nswText,const wchar_t* nswTitle,UINT nuType,WORD nsuLanguage);

	// ���������
	static HCURSOR SetCursor(HCURSOR nhCursor);

protected:
	struct ST_MESSAGEBOX{
		const wchar_t* Text;
		const wchar_t* Title;
		UINT Type;
		WORD Language;
		int Result;
	};

	ERESULT __stdcall MessageBoxCallBack(ULONG nuFlag,LPVOID npContext);
	ERESULT __stdcall MessageBoxExCallBack(ULONG nuFlag,LPVOID npContext);

	ERESULT __stdcall SetCursorCallBack(ULONG nuFlag,LPVOID npContext);


};


//////////////////////////////////////////////////////////////////////////
// ���ڸ�������������
class CExFloat
{
public:
	// ȥ����������С��λ������4��5��
	static __inline FLOAT Round(FLOAT Org){
		return (FLOAT)floor(Org+0.49999f);
	}

	// ����������ȡֵΪ��ӽ��ġ�.5��ֵ����1.3 -> 1.5 , 2.9 -> 2.5
	static __inline FLOAT HalfPixel(FLOAT Org){
		return (FLOAT)floor(Org)+0.5f;
	}

	// ��������ȡֵΪ��С������ȥ0.4999����1.3 -> 0.5 , 2.9 -> 1.5
	static __inline FLOAT UnderHalf(FLOAT Org){
		return (FLOAT)floor(Org)-0.49999f;
	}

	// �ж������������Ƿ����
	static __inline bool Equal(FLOAT X1,FLOAT X2,FLOAT Allow=0.0001f){
		if(fabs(X1 - X2)<Allow)
			return true;
		return false;
	}

	// ȡ��С�Ĵ��ڵ������븡����������ֵ
	static __inline FLOAT Ceil(FLOAT Org){
		return (FLOAT)ceil(Org);
	}

	// ȡ����С�ڵ������븡����������ֵ
	static __inline FLOAT Floor(FLOAT Org){
		return (FLOAT)floor(Org);
	}

	// ��������ת��Ϊ����������ִ����������
	static __inline LONG ToLong(FLOAT Org){
		return static_cast<LONG>(Round(Org));
	}


};


#define LW_REAL_EPSILON        1.192092896e-07F        /* FLT_EPSILON */

//////////////////////////////////////////////////////////////////////////
// ��������
class CExRect
{
public:

	static __inline bool IsEmptyArea(float nfWidth, float nfHeight)
	{
//		return (nfWidth <= LW_REAL_EPSILON) || (nfHeight <= LW_REAL_EPSILON);
		return (nfWidth >= -LW_REAL_EPSILON && nfWidth <= LW_REAL_EPSILON) || (nfHeight >= -LW_REAL_EPSILON && nfHeight <= LW_REAL_EPSILON);
	}

	// �󽻼� Sets CRect equal to the intersection of two rectangles.
	static __inline bool  Intersect(IN const D2D1_RECT_F& ndRectOne, IN const D2D1_RECT_F& ndRectTwo, OUT D2D1_RECT_F& ndIntersectRect)
	{
		
		float lfRight = min(ndRectOne.right, ndRectTwo.right);
		float lfBottom  = min(ndRectOne.bottom, ndRectTwo.bottom);
		float lfLeft = max(ndRectOne.left, ndRectTwo.left);
		float lfTop = max(ndRectOne.top, ndRectTwo.top);

		ndIntersectRect.left = lfLeft;
		ndIntersectRect.top = lfTop;
		ndIntersectRect.right = lfRight;
		ndIntersectRect.bottom = lfBottom;

		return !CExRect::IsEmptyArea(ndIntersectRect.right - ndIntersectRect.left, ndIntersectRect.top - ndIntersectRect.bottom);

	}

	// �󲢼� Sets CRect equal to the union of two rectangles.
	static __inline bool Union(IN const D2D1_RECT_F& ndRectOne, IN const D2D1_RECT_F& ndRectTwo, OUT D2D1_RECT_F& ndUnionRect)
	{
		float lfRight = max(ndRectOne.right, ndRectTwo.right);
		float lfBottom  = max(ndRectOne.bottom, ndRectTwo.bottom);
		float lfLeft = min(ndRectOne.left, ndRectTwo.left);
		float lfTop = min(ndRectOne.top, ndRectTwo.top);

		ndUnionRect.left = lfLeft;
		ndUnionRect.top = lfTop;
		ndUnionRect.right = lfRight;
		ndUnionRect.bottom = lfBottom;
		
		return !CExRect::IsEmptyArea(ndUnionRect.right - ndUnionRect.left, ndUnionRect.top - ndUnionRect.bottom);
	}

	// �������Σ�ʹ�䶥�����л�
	static __inline D2D1_RECT_F GetNormalizedRectangle(float x1, float y1, float x2, float y2)
	{
		if (x2 < x1)
		{
			float tmp = x2;
			x2 = x1;
			x1 = tmp;
		}

		if (y2 < y1)
		{
			float tmp = y2;
			y2 = y1;
			y1 = tmp;
		}
		return D2D1::RectF(x1, y1, x2, y2);
	}

	// �������Σ�ʹ�䶥�����л�
	static __inline D2D1_RECT_F GetNormalizedRectangle(const D2D1_RECT_F& ndRect)
	{
		return CExRect::GetNormalizedRectangle(ndRect.left, ndRect.top, ndRect.right, ndRect.bottom);
	}

	// ��⵱ǰ���Ƿ����ھ�������	
	static __inline bool PtInRect(IN const D2D1_POINT_2F& ndPoint, IN const D2D1_RECT_F& ndRect)
	{
		D2D1_RECT_F ldRect = CExRect::GetNormalizedRectangle(ndRect);

		return ndPoint.x >= ldRect.left && ndPoint.x < ldRect.right && ndPoint.y >= ldRect.top && ndPoint.y < ldRect.bottom;
	}

};


//////////////////////////////////////////////////////////////////////////
// ���ڶ��������㷨��صĺ���
class CExFill{

public:

	// �жϵ��Ƿ��ڶ������
	static __inline bool PtInPolygon (
		D2D1_POINT_2F ndfPoint,			// �����ĵ�
		D2D1_POINT_2F* npPolygonPoint,	// ����εĸ����������꣨��β����Բ�һ�£�
		int niCount						// ����ζ������
		)
	{
		// ���������ͨ���õ��ˮƽ�������θ��ߵĽ���

		int liCross = 0;

		for (int liLoop = 0; liLoop < niCount; liLoop++)
		{
			D2D1_POINT_2F p1 = npPolygonPoint[liLoop];
			D2D1_POINT_2F p2 = npPolygonPoint[(liLoop + 1) % niCount];

			// ��� y=ndfPoint.y �� p1p2 �Ľ���

			// ���ƽ�������
			if ( p1.y == p2.y )		
				continue;

			// �����Ƿ���p1p2�ӳ�����
			if ( ndfPoint.y < min(p1.y, p2.y) )		
				continue;

			// �����Ƿ���p1p2�ӳ�����
			if ( ndfPoint.y >= max(p1.y, p2.y) )	
				continue;

			// �󽻵�� X ���� 
			// ����˼���ǣ����������Σ����ñߵı�����ϵ�����X�߳��ȡ��̶����X����

			float lfCrossCoordX = (float)(ndfPoint.y - p1.y) * (float)(p2.x - p1.x) / (float)(p2.y - p1.y) + p1.x;
			if ( lfCrossCoordX > ndfPoint.x )
				liCross++; // ֻͳ�Ƶ��߽���

		}

		// ���߽���Ϊż��ʱ����˵�����ڶ����֮��
		return (liCross % 2 == 1);
	}


	// �жϵ��Ƿ�����Բ��
	static __inline bool PtInEllipse(
		D2D1_POINT_2F ndfPoint,			// �����ĵ�
		D2D1_RECT_F	ndfRect				// ��Բ��Χ�ɵ���С����
		)
	{
		// ������
		// ������Բf(x,y)= x^2/a^2 + y^2/b^2=1, �����f(x1,y1) <1��˵��x1,y1����Բ�ڲ�

		// ������Բ������
		float lfShortAxis = (ndfRect.right-ndfRect.left)/2;
		float lfLongAxis = (ndfRect.bottom - ndfRect.top)/2;

		// ��ԲԲ��
		D2D1_POINT_2F ldfCenterPoint = D2D1::Point2F(lfShortAxis + ndfRect.left, lfLongAxis + ndfRect.top); 

		// ��������ĵ��������Բ���ĵ�����
		D2D1_POINT_2F ldfCheckPoint = D2D1::Point2F(ldfCenterPoint.x - ndfPoint.x, ldfCenterPoint.y - ndfPoint.y);

		// ������Բ����ֵ
		float lfResult = (ldfCheckPoint.x * ldfCheckPoint.x)/(lfShortAxis*lfShortAxis)+
			(ldfCheckPoint.y*ldfCheckPoint.y)/(lfLongAxis*lfLongAxis);

		return lfResult<=1.0f;

	}


	// �жϵ��Ƿ����߶���
	static __inline bool PtOnLine(
		D2D1_POINT_2F ndfPoint,			// �����ĵ�
		D2D1_POINT_2F ndfStartPoint,	// ֱ�����
		D2D1_POINT_2F ndfEndPoint		// ֱ���յ�
		)
	{
		// ������
		// 1���ò�˼��㣬��������ܴ��ƫ�
		// 2�����ڲ�˷���ƫ��̫������ò����ķ���

		// �����߶γ���
		float lfLineSegDistance = (FLOAT)sqrt((ndfEndPoint.x-ndfStartPoint.x)*(ndfEndPoint.x-ndfStartPoint.x) +
			(ndfEndPoint.y-ndfStartPoint.y)*(ndfEndPoint.y-ndfStartPoint.y)
			);

		// ����㵽�߶����˵ľ���
		float lfDistToStart = (FLOAT)sqrt((ndfPoint.x-ndfStartPoint.x)*(ndfPoint.x-ndfStartPoint.x) +
			(ndfPoint.y-ndfStartPoint.y)*(ndfPoint.y-ndfStartPoint.y)
			);

		float lfDistToEnd = (FLOAT)sqrt((ndfPoint.x-ndfEndPoint.x)*(ndfPoint.x-ndfEndPoint.x) +
			(ndfPoint.y-ndfEndPoint.y)*(ndfPoint.y-ndfEndPoint.y)
			);

		// ���ֵ
		float lfDist = lfDistToStart + lfDistToEnd - lfLineSegDistance;
			
		// ������Ϊ0.1
		lfDist = lfDist - 0.1f;

		return lfDist < LW_REAL_EPSILON;

		// ��˷��������������ĵ�϶����߶���Χ�ɵľ����ڣ�
		//{
		//	// ��������ʸ���߶�
		//	D2D1_POINT_2F ldfVector1 = D2D1::Point2F(ndfEndPoint.x - ndfStartPoint.x, ndfEndPoint.y - ndfStartPoint.y);
		//	D2D1_POINT_2F ldfVector2 = D2D1::Point2F(ndfPoint.x - ndfStartPoint.x, ndfPoint.y - ndfStartPoint.y);
		//
		//	// �����ˣ��жϵ��Ƿ���ֱ�߶����ڵ�ֱ����
		//	// ��ʽ�� V1(x1, y1) X V2(x2, y2) = x1y2 �C y1x2
		//	float lfCrossProduct = ldfVector1.x * ldfVector2.y - ldfVector1.y * ldfVector2.x;

		//	return (lfCrossProduct >= -LW_REAL_EPSILON && lfCrossProduct <= LW_REAL_EPSILON);
		//}

	}


};



//////////////////////////////////////////////////////////////////////////
//���������ת��
class CExPoint
{
public:
	//���ڼ��㱻�Ŵ�ͼ�ϵ������Ӧ��ԭͼ������
	//rdSrcSizeԭͼ��С
	//rdDestSize��ǰ����ʾ��С
	//rdPointҪת��������
	//nuMethodʹ�õķŴ�ʽ��1��ESPB_DRAWBMP_EXTEND����չ�߷�ʽ�� 2������ֵ��ʾֱ������ 
	//rdExtendLine���ʹ�õ���ESPB_DRAWBMP_EXTEND�Ŵ�ʽ����Ҫ������չ������
	static D2D1_POINT_2F BigToOldPoint(D2D1_SIZE_F& rdSrcSize,D2D1_SIZE_F& rdDestSize,D2D1_POINT_2F& rdPoint,ULONG nuMethod,D2D1_POINT_2F& rdExtendLine);
};


//////////////////////////////////////////////////////////////////////////
// ����ע��ϵͳĬ�Ͽ�ݼ�
class CExHotkey
{
public:
	static bool RegisterHotKey(
		IEinkuiIterator* npFocusOn,	// ��ע�˶���������Ӷ���NULL��ʾ������
		IEinkuiIterator* npReceiver,	// �������տ�ݼ��Ķ���
		ULONG nuSysHotkeyID		// ϵͳĬ�ϵ�ID����lwUI�ļ�IXelManager����ĩβ��ϵͳĬ�Ͽ�ݼ�ID��
		);
};