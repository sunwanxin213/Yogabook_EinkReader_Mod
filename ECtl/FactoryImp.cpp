// ECtl.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"

#include "CfgIface.h"
#include "cmmstruct.h"
#include "cmmBaseObj.h"
#include "cmmPath.h"
#include "mapDefine.h"
#include "mapList.h"


#include "Einkui.h"
#include "XCtl.h"
#include "ElementImp.h"

#include "EvButtonImp.h"
#include "EvPictureFrameImp.h"
#include "EvStaticTextImp.h"
#include "EvEditImp.h"
#include "EvScrollBarImp.h"
#include "EvListImp.h"
#include "EvSliderBarImp.h"
#include "FactoryImp.h"
#include "EvSliderButtonImp.h"
#include "EvPpButtonImp.h"
#include "EvLabelImp.h"
#include "EvBiSliderBarImp.h"
#include "EvAnimatorImp.h"

#include "EvPopupMenuImp.h"
#include "EvMenuButtonImp.h"

#include "EvMenuBarImp.h"

//#include "EvToolPane.h"
//#include "EvToolBar.h"
//#include "EvImageButtonImp.h"
#include "EvComboBoxImp.h"
//#include "EvSpinButtonImp.h"
#include "EvWhirlAngle.h"
#include "xSelectPoint.h"
#include "xSelectFrame.h"
#include "EvCheckButtonImp.h"
#include "EvRadioButtonGroupImp.h"
//#include "EvTimeSpinButtonImp.h"

CFactoryImp* CFactoryImp::gpFactoryInstance = NULL;
DEFINE_BUILTIN_NAME(CFactoryImp)

CFactoryImp::CFactoryImp()
{

	mpConfig = NULL;
}
CFactoryImp::~CFactoryImp()
{

}

// ��ʼ������������ʵ�ָ��ֲ�ͬ�����ĳ�ʼ��������ע�⣬����������InitOnCreate������һ��Ҫ���û����InitOnCreate����
// ����0��ʾ�ɹ�������ֵ���λΪ1��ʾ�������ش���Ӧ����ֹ��ʼ�����̣����صľ��Ǵ����룻��������ֵ��ʾ�����Ǵ��󷵻���
ULONG CFactoryImp::InitOnCreate(void)
{
	if (gpFactoryInstance != NULL)
		return ERESULT_OBJECT_EXISTED;

	gpFactoryInstance = this;

	// ���������ӳ���Ԫ��
	bool lbStatus = false;
	do
	{
		lbStatus = moMapList.AddList(L"Button", CMapCallFP<CEvButton>::Custom2Normal(&CEvButton::CreateInstance));
		BREAK_ON_FALSE(lbStatus);
		lbStatus = moMapList.AddList(L"CheckButton", CMapCallFP<CEvCheckButton>::Custom2Normal(&CEvCheckButton::CreateInstance));
		BREAK_ON_FALSE(lbStatus);
		lbStatus = moMapList.AddList(L"RadioButtonGroup", CMapCallFP<CEvRadioButtonGroup>::Custom2Normal(&CEvRadioButtonGroup::CreateInstance));
		BREAK_ON_FALSE(lbStatus);
		lbStatus = moMapList.AddList(L"PictureFrame", CMapCallFP<CEvPictureFrame>::Custom2Normal(&CEvPictureFrame::CreateInstance));
		BREAK_ON_FALSE(lbStatus);
		lbStatus = moMapList.AddList(L"StaticText", CMapCallFP<CEvStaticText>::Custom2Normal(&CEvStaticText::CreateInstance));
		BREAK_ON_FALSE(lbStatus);
		lbStatus = moMapList.AddList(L"Edit", CMapCallFP<CEvEditImp>::Custom2Normal(&CEvEditImp::CreateInstance));
		BREAK_ON_FALSE(lbStatus);
		lbStatus = moMapList.AddList(L"ScrollBar", CMapCallFP<CEvScrollBar>::Custom2Normal(&CEvScrollBar::CreateInstance));
		BREAK_ON_FALSE(lbStatus);
		lbStatus = moMapList.AddList(L"List", CMapCallFP<CEvList>::Custom2Normal(&CEvList::CreateInstance));
		BREAK_ON_FALSE(lbStatus);
		lbStatus = moMapList.AddList(L"SliderBar", CMapCallFP<CEvSliderBar>::Custom2Normal(&CEvSliderBar::CreateInstance));
		BREAK_ON_FALSE(lbStatus);
		lbStatus = moMapList.AddList(L"BiSliderBar", CMapCallFP<CEvBiSliderBar>::Custom2Normal(&CEvBiSliderBar::CreateInstance));
		BREAK_ON_FALSE(lbStatus);
		lbStatus = moMapList.AddList(L"SliderButton", CMapCallFP<CEvSliderButton>::Custom2Normal(&CEvSliderButton::CreateInstance));
		BREAK_ON_FALSE(lbStatus);
		lbStatus = moMapList.AddList(L"PingpongButton", CMapCallFP<CEvPingpongButton>::Custom2Normal(&CEvPingpongButton::CreateInstance));
		BREAK_ON_FALSE(lbStatus);
		lbStatus = moMapList.AddList(L"Label", CMapCallFP<CEvLabelImp>::Custom2Normal(&CEvLabelImp::CreateInstance));
		BREAK_ON_FALSE(lbStatus);
		lbStatus = moMapList.AddList(L"Animator", CMapCallFP<CEvAnimatorImp>::Custom2Normal(&CEvAnimatorImp::CreateInstance));
		BREAK_ON_FALSE(lbStatus);
		//lbStatus = moMapList.AddList(L"MenuButton", CMapCallFP<CEvMenuButton>::Custom2Normal(&CEvMenuButton::CreateInstance));
		//BREAK_ON_FALSE(lbStatus);
		//lbStatus = moMapList.AddList(L"MenuBar", CMapCallFP<CEvMenuBar>::Custom2Normal(&CEvMenuBar::CreateInstance));
		//BREAK_ON_FALSE(lbStatus);
		lbStatus = moMapList.AddList(L"PopupMenu", CMapCallFP<CEvPopupMenu>::Custom2Normal(&CEvPopupMenu::CreateInstance));
		BREAK_ON_FALSE(lbStatus);
		// 		lbStatus = moMapList.AddList(L"ToolPane", CMapCallFP<CEvToolPane>::Custom2Normal(&CEvToolPane::CreateInstance));
		// 		BREAK_ON_FALSE(lbStatus);
				//lbStatus = moMapList.AddList(L"ToolBar", CMapCallFP<CEvToolBar>::Custom2Normal(&CEvToolBar::CreateInstance));
				//BREAK_ON_FALSE(lbStatus);
		// 		lbStatus = moMapList.AddList(L"ImageButton", CMapCallFP<CEvImageButton>::Custom2Normal(&CEvImageButton::CreateInstance));
		// 		BREAK_ON_FALSE(lbStatus);
		lbStatus = moMapList.AddList(L"ComboBox", CMapCallFP<CEvComboBox>::Custom2Normal(&CEvComboBox::CreateInstance));
		BREAK_ON_FALSE(lbStatus);
		lbStatus = moMapList.AddList(L"WhirlAngle", CMapCallFP<CEvWhirlAngleImp>::Custom2Normal(&CEvWhirlAngleImp::CreateInstance));
		BREAK_ON_FALSE(lbStatus);
		//lbStatus = moMapList.AddList(L"SpinButton", CMapCallFP<CEvSpinButton>::Custom2Normal(&CEvSpinButton::CreateInstance));
		//BREAK_ON_FALSE(lbStatus);
		lbStatus = moMapList.AddList(L"SelectFrame", CMapCallFP<CSelectFrame>::Custom2Normal(&CSelectFrame::CreateInstance));
		BREAK_ON_FALSE(lbStatus);
		//lbStatus = moMapList.AddList(L"TimeSpinButton", CMapCallFP<CEvTimeSpinButton>::Custom2Normal(&CEvTimeSpinButton::CreateInstance));
		//BREAK_ON_FALSE(lbStatus);
	} while (false);

	if (lbStatus == false)
		return ERESULT_UNSUCCESSFUL;

	return ERESULT_SUCCESS;
}

// �������ļ��д�������
IEinkuiIterator* __stdcall CFactoryImp::CreateElement(
	IN IEinkuiIterator* npParent,		// ������ָ��
	IN ICfKey* npTemplete,			// npTemplete��Key ID����EID��ֵ��������EType
	IN ULONG nuEID					// �����Ϊ0��MAXULONG32����ָ����Ԫ�ص�EID; ����ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��XUIϵͳ�Զ�����
)
{
	IXsElement* lpoElement = NULL;
	IEinkuiIterator* lpXIterator = NULL;
	wchar_t lswClsName[MAX_PATH] = { 0 };

	do
	{
		BREAK_ON_NULL(npTemplete);

		// ��ȡҪ�����Ķ�������
		if (npTemplete->GetValue(lswClsName, MAX_PATH * sizeof(wchar_t)) <= 0)
			break;

		// ��MAPLIST���в��Ҷ�Ӧ�Ĵ�������
		AFX_MAPCALL lpfnCreateFunction = moMapList.GetUserData(lswClsName, NULL);
		BREAK_ON_NULL(lpfnCreateFunction);

		// �÷��صĺ���ָ�봴������
		lpoElement = lpfnCreateFunction(npParent, npTemplete, nuEID);

	} while (false);

	if (lpoElement != NULL)
		lpXIterator = lpoElement->GetIterator();	//��ȡIteratorָ��

	return lpXIterator;
}


// ͨ����������������
IEinkuiIterator* __stdcall CFactoryImp::CreateElement(
	IN IEinkuiIterator* npParent,		// ������ָ��
	IN const wchar_t*		nswClassName,	// ����
	IN ULONG nuEID					// �����Ϊ0��MAXULONG32����ָ����Ԫ�ص�EID; ����ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��XUIϵͳ�Զ�����
)
{
	IXsElement* lpoElement = NULL;
	IEinkuiIterator* lpXIterator = NULL;

	do
	{
		BREAK_ON_NULL(nswClassName);

		// ��MAPLIST���в��Ҷ�Ӧ�Ĵ�������
		AFX_MAPCALL lpfnCreateFunction = moMapList.GetUserData(nswClassName, NULL);
		BREAK_ON_NULL(lpfnCreateFunction);

		// �÷��صĺ���ָ�봴������
		lpoElement = lpfnCreateFunction(npParent, NULL, nuEID);

	} while (false);

	if (lpoElement != NULL)
		lpXIterator = lpoElement->GetIterator();	//��ȡIteratorָ��

	return lpXIterator;
}


// ������Module���׵�Profile�ļ��ӿڣ����صĽӿڵ�����ʹ��ʱ����ҪRelease
IConfigFile* __stdcall CFactoryImp::GetTempleteFile(void)
{
	const wchar_t* lpszWidgetPath = NULL;
	const wchar_t* lpszLanguage = NULL;
	wchar_t lpszConfigFileName[CONFIG_FILE_NAME_MAX_LEN] = { 0 };

	IConfigFile* lpIConfigFile = NULL;

	do
	{
		if (mpConfig == NULL)
		{
			lpszWidgetPath = EinkuiGetSystem()->GetCurrentWidget()->GetWidgetDefaultPath();		//��ȡWidget�İ�װ·��
			BREAK_ON_NULL(lpszWidgetPath);

			CFilePathName loConfigFilePath(lpszWidgetPath);
			loConfigFilePath.AssurePath();	//����ΪĿ¼��Ҳ�������������"\"

			BREAK_ON_FALSE(loConfigFilePath.Transform(L"Profile\\"));

			lpszLanguage = EinkuiGetSystem()->GetCurrentLanguage();		//��ȡ��ǰϵͳ���Զ�Ӧ���ַ���,���磺���ļ����Ӧ��chn
			BREAK_ON_NULL(lpszLanguage);

			wcscpy_s(lpszConfigFileName, CONFIG_FILE_NAME_MAX_LEN, L"Widget_");		//ƴ���ļ��� ʾ����System_chn.set
			wcscat_s(lpszConfigFileName, CONFIG_FILE_NAME_MAX_LEN, lpszLanguage);
			wcscat_s(lpszConfigFileName, CONFIG_FILE_NAME_MAX_LEN, L".set");

			BREAK_ON_FALSE(loConfigFilePath.Transform(lpszConfigFileName));	//ƴ��ȫ·��

			lpIConfigFile = EinkuiGetSystem()->OpenConfigFile(loConfigFilePath.GetPathName(), OPEN_EXISTING);	//�򿪸������ļ�

			BREAK_ON_NULL(lpIConfigFile);

			mpConfig = lpIConfigFile;
		}
		else
			lpIConfigFile = mpConfig;

		lpIConfigFile->AddRefer();	//�������ü���

	} while (false);

	return lpIConfigFile;
}

// ��ñ�DLLΨһ�Ĺ�������
CFactoryImp* CFactoryImp::GetUniqueObject(void)
{
	if (gpFactoryInstance == NULL)
		CFactoryImp::CreateInstance();

	CMMASSERT(gpFactoryInstance != NULL);

	return gpFactoryInstance;
}

