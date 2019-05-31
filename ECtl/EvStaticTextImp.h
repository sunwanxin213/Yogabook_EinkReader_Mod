/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#ifndef _EVSTATICTEXTIMP_H_
#define _EVSTATICTEXTIMP_H_



// Element�Ļ���ʵ�֣���������Elementʱ�����Դ�Ϊ������Ҫ���ԴӴ����ֽ��������࣬��Ϊ��������������ʵ�������Ľӿڣ�ֱ�����������CEvStaticText���������ṩIXsElement�ӿ�
// ���ʵ�ֵ�����ͬ�ӿڵ���𣬾Ϳ���ֱ�Ӵ�ĳ��ʵ�������������ࡣ
DECLARE_BUILTIN_NAME(StaticText)
class CEvStaticText :
	public CXuiElement<CEvStaticText ,GET_BUILTIN_NAME(StaticText)>
{
friend CXuiElement<CEvStaticText ,GET_BUILTIN_NAME(StaticText)>;
public:
	// �������༰����������ʱ�����ر�ע��!!! һ��Ҫ���ȵ��û���ķ���
	// �����������ڽ�����Ԫ�ض��󡢳�ʼ���������ݣ�λ�á���С������ͼ�ȣ�
	ULONG InitOnCreate(
		IN IEinkuiIterator* npParent,	// ������ָ��
		IN ICfKey* npTemplete,		// npTemplete��Key ID����EID��ֵ��������EType
		IN ULONG nuEID=MAXULONG32		// �����Ϊ0��MAXULONG32����ָ����Ԫ�ص�EID; ����ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��XUIϵͳ�Զ�����
		);

protected:
	CEvStaticText();
	virtual ~CEvStaticText();

	//װ��������Դ
	virtual ERESULT LoadResource();
	//����
	virtual ERESULT OnPaint(IEinkuiPaintBoard* npPaintBoard);
	// �ֽ���Ϣ���ṩ��Ϣ�ֽ����Ϣ��Ӧ�Ĺ��ܣ������ʵ���ǽ���Ϣ�ֽ�Ϊ��ͬ������󣬵�����Ӧ�Ĵ����麯�������ڲ���ʶ����Ϣ��һ�ɷ���ERESULT_UNEXPECTED_MESSAGE
	// �������ķ���ֵ���Զ�ͬ�����õ�npMsgָ�����Ϣ������
	virtual ERESULT ParseMessage(IEinkuiMessage* npMsg);
	// ����Ԫ�أ�����֪ͨһ��Ԫ�����٣�ע�⣺��Ԫ�������յ�����Ϣ��Ӧ�ü�ʱ����Ԫ�ع�������UnregisterElement������
	// �Ӷ�����Ԫ�ع�������������һ��Ԫ�ط���������Ϣ�������ٽ��Լ���Ԫ�ع�����ע���������ͷ��������
	virtual ERESULT OnElementDestroy();
	//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
	virtual ERESULT OnElementCreate(IEinkuiIterator* npIterator);

private:
	wchar_t* mpswText;		 //��ʾ����
	//XuiPosition mdTextPosition;		 //�����ڱ���ͼ�е�λ��
	//XuiPosition mdExtendPosition;	 //��չ������
	DWORD mdwColor;			//������ɫ
	DWORD mdwFontSize;		//�����С
	wchar_t* mpswFontName;	//��������
	STETXT_BMP_INIT::eSIZELIMIT mLimit;
	STETXT_BMP_INIT::eTEXTALIGN mTalign;
	STETXT_BMP_INIT::ePARAALIGN mPalign;
	STETXT_BMP_INIT::eFONTWEIGHT mFontWidget;
	D2D1_RECT_F mdDrawRect;	//���������С��Ŀ���Դһ��

	//������ʾ����
	bool SetText(wchar_t* npswText = NULL);
	//��������ͼƬ
	bool ReCreateBmp();

};


#define TF_ID_ST_TEXT L"Text"				//��ʾ����
#define TF_ID_ST_COLOR L"Color"				//������ɫ
#define TF_ID_ST_FONT L"FontName"			//����
#define TF_ID_ST_FONT_SIZE L"FontSize"		//�ֺ�
#define TF_ID_ST_SIZE_LIMIT L"SizeLimit"	//
#define TF_ID_ST_TALIGN L"Talign"			//
#define TF_ID_ST_PALIGN L"Palign"			//
#define TF_ID_ST_FONTWIDGET L"FontWidget"	//


#endif//_EVSTATICTEXTIMP_H_
