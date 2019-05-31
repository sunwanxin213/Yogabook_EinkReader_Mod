/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#ifndef _EVPICTUREFRAMEIMP_H_
#define _EVPICTUREFRAMEIMP_H_



// Element�Ļ���ʵ�֣���������Elementʱ�����Դ�Ϊ������Ҫ���ԴӴ����ֽ��������࣬��Ϊ��������������ʵ�������Ľӿڣ�ֱ�����������CEvPictureFrame���������ṩIXsElement�ӿ�
// ���ʵ�ֵ�����ͬ�ӿڵ���𣬾Ϳ���ֱ�Ӵ�ĳ��ʵ�������������ࡣ
DECLARE_BUILTIN_NAME(PictureFrame)
class CEvPictureFrame :
	public CXuiElement<CEvPictureFrame ,GET_BUILTIN_NAME(PictureFrame)>
{
friend CXuiElement<CEvPictureFrame ,GET_BUILTIN_NAME(PictureFrame)>;
public:

	// �������༰����������ʱ�����ر�ע��!!! һ��Ҫ���ȵ��û���ķ���
	// �����������ڽ�����Ԫ�ض��󡢳�ʼ���������ݣ�λ�á���С������ͼ�ȣ�
	ULONG InitOnCreate(
		IN IEinkuiIterator* npParent,	// ������ָ��
		IN ICfKey* npTemplete,		// npTemplete��Key ID����EID��ֵ��������EType
		IN ULONG nuEID=MAXULONG32		// �����Ϊ0��MAXULONG32����ָ����Ԫ�ص�EID; ����ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��XUIϵͳ�Զ�����
		);

protected:
	CEvPictureFrame();
	virtual ~CEvPictureFrame();

	//װ��������Դ
	virtual ERESULT LoadResource();
	//����
	//virtual ERESULT OnPaint(IEinkuiPaintBoard* npPaintBoard);
	// �ֽ���Ϣ���ṩ��Ϣ�ֽ����Ϣ��Ӧ�Ĺ��ܣ������ʵ���ǽ���Ϣ�ֽ�Ϊ��ͬ������󣬵�����Ӧ�Ĵ����麯�������ڲ���ʶ����Ϣ��һ�ɷ���ERESULT_UNEXPECTED_MESSAGE
	// �������ķ���ֵ���Զ�ͬ�����õ�npMsgָ�����Ϣ������
	virtual ERESULT ParseMessage(IEinkuiMessage* npMsg);
	// ����Ԫ�أ�����֪ͨһ��Ԫ�����٣�ע�⣺��Ԫ�������յ�����Ϣ��Ӧ�ü�ʱ����Ԫ�ع�������UnregisterElement������
	// �Ӷ�����Ԫ�ع�������������һ��Ԫ�ط���������Ϣ�������ٽ��Լ���Ԫ�ع�����ע���������ͷ��������
	virtual ERESULT OnElementDestroy();
	//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
	virtual ERESULT OnElementCreate(IEinkuiIterator* npIterator);
	//������Ϣ
	virtual ERESULT OnPaint(IEinkuiPaintBoard* npPaintBoard);
	//�л���ʾ֡,��һ֡Ϊ1
	virtual ERESULT OnChangeIndex(LONG nlIndex = 0);
	//������ʾͼƬ
	virtual ERESULT OnChangePic(wchar_t* npswPicPath = NULL,bool nbIsFullPath = false);
	// ��������
	virtual ERESULT OnMouseOwnerTest(const D2D1_POINT_2F& rPoint);
	//��ʱ��
	virtual void OnTimer(
		PSTEMS_TIMER npStatus
	);
	//֪ͨԪ�ء���ʾ/���ء������ı�
	virtual ERESULT OnElementShow(bool nbIsShow);
private:
	LONG mlCurrentIndex;			 //��ǰ��ʾ�ڼ�֡
	LONG mlMaxFrame;				 //���֡��
	D2D1_SIZE_F mdFrameSize;		 //ÿ֡����ʵ�ߴ�
	ULONG mulMethod;				 //����ʲô���ŷ�ʽ
	FLOAT mfBeginPos;				 //����ʱ��X������ʼ��
	bool mbIsAutoPlay;				//�Ƿ��Զ�����
	bool mbIsPlayLoop;				//�Ƿ�ѭ������
	LONG mlPlayTimerElapse;			//���Ŷ����Ķ�ʱ��ʱ��

	//���¼���֡��С,nbResizeΪ����������ô�С
	bool Resize(bool nbResize = true);
};


#define TF_ID_PIC_FRAME_COUNT L"FrameCount"	//��ͼ�м�֡
#define TF_ID_PIC_AUTO_PLAY L"AutoPlay"	//1�Զ����ţ�0�ر�
#define TF_ID_PIC_PLAY_LOOP L"PlayLoop"	//1ѭ�����ţ�0�ر�
#define TF_ID_PIC_PLAY_TIMER_ELAPSE L"PlayElapse"	//�л�ÿ֡�ļ����ms��λ

#define TF_ID_PIC_TIMER_ID_PLAY 1 //��ʱ��ID�����Ŷ���

#endif//_EVPICTUREFRAMEIMP_H_
