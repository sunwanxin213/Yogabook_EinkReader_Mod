/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */

#pragma once
#ifndef _EINKINTERNAL_H_
#define _EINKINTERNAL_H_
/*
	EinkIteApi dll ���ڲ�����
*/



#define GIHW_KEYBOARD	1
#define GIHW_DB_CLICK	4



//����֪ͨӦ���¼�
#define WM_EI_SERVICE_EVENT WM_USER + 0x501
//WParam: ULONG 
#define EI_POWER_ENTER_CONNECTED_STANDBY 0	//����sleep
#define EI_POWER_RETURN_CONNECTED_STANDBY 1	//����
#define EI_POWER_ENTER_SHUTDOWN 2			//�ػ�
#define EI_B_COVER_DB_CLICK 3				//B��˫��
#define EI_C_COVER_DB_CLICK 4				//C��˫��
#define EI_C_ENTER_KB 5						//�������ģʽ
#define EI_C_SHOW_KB_DISABLE 6				//��ʾ���̲�����
#define EI_C_SHOW_DB_WAKEUP 7				//��ʾ˫�����Ի���
#define EI_SET_HOMEBAR_CREATE_PAD 8			//Homebar�л���createpad
#define EI_SET_HOMEBAR_PEN_MOUSE 9			//Homebar�л���Pen mouse
#define EI_SET_HOMEBAR_EXPAND 10			//Homebar����չ��״̬
#define EI_SET_HOMEBAR_COLLAPSE 11			//Homebar��������״̬
#define EI_SET_HOMEBAR_UP_SHOW 12			//Homebar��clone����ʾ����
#define EI_SET_HOMEBAR_KEYBOARD 13			//Homebar�л�������
#define EI_SET_HOMEBAR_CLOSE_B_COVER 14		//Homebar�ر�B��
#define EI_SET_HOMEBAR_SHOW 15				//Homebar��ʾ������ˢ��һ��
#define EI_POWER_ENTER_CONNECTED_STANDBY_NONE 16
#define EI_POWER_RETURN_CONNECTED_STANDBY_NONE 17
#define EI_OOBE_COMPLETE 18	//C��OOBE�����˳�

//LParam: NA

#define EI_PLAY_KEYBOARD_SOUND_DOWN 1	//���Ű�����
#define EI_PLAY_KEYBOARD_SOUND_UP 2		//�����ͷ���

#ifdef __cplusplus
extern "C" {
#endif

	// �ڲ�����������һ��APP
	// ֪ͨϵͳ����һ��ָ����APP
	// ���أ� ���ط��㣬��ʾ�ɹ��������㣬��ʾʧ�ܣ�
	DWORD EiRunAPP(
		const wchar_t* npszFilePath,	// Ŀ���ļ���
		int niSession=-1					//����ָ���������ĸ�session,0��ʾsystem�û�
	);

	// ����Switcher�Ĵ�С
	void SetSwitcherLocation(
		const EI_RECT* npLocation // ����x��ŵ���������Ҳ�ľ���
	);

	// ����Switcher�Ŀ�������
	void SetSwitcherShowArea(
		const EI_RECT* npArea // ����x��ŵ���������Ҳ�ľ���
	);

	// Fuction Switcherʹ�õĻ��ƽӿ�
	// ��ͬ��EiDraw�������˴���pstBuffer������������������Ļ�����������ǽ�������Switcher��������
	//    ���ң��˴���Buffer��ÿһ���ֽڵ����λ��ʾ�������Ƿ���ʾ����
	DWORD EiSwitcherDraw(
		EI_BUFFER* pstBuffer		// image buffer 
	);

	// ֪ͨϵͳ�ػ�����ͼƬ��·��
	// ���أ� ���ط��㣬��ʾ�ɹ��������㣬��ʾʧ�ܣ�
	DWORD EiSetShutdownCover(
		const wchar_t* npszFilePath	// Ŀ���ļ���
	);

	// ֪ͨϵͳ����ͼƬ��·��
	// ���أ� ���ط��㣬��ʾ�ɹ��������㣬��ʾʧ�ܣ�
	DWORD EiSetKeyboardImg(
		const wchar_t* npszFilePath	// Ŀ���ļ���
	);

	// ֪ͨϵͳ��ʾ״̬Ҫ�����仯
	// ���أ� ���ط��㣬��ʾ�ɹ��������㣬��ʾʧ�ܣ�
	// 0������ʾAPP 1��ʾ���� 2��ʾ����A 3��ʾ����B 
	DWORD EiSetShowStatus(
		ULONG nulStatus	// ״̬
	);

	// ֪ͨϵͳ������̬�����仯
	// ���أ� ���ط��㣬��ʾ�ɹ��������㣬��ʾʧ�ܣ�
	// 2�ʼǱ���̬ 3tent��̬
	DWORD EiSetLaptopMode(
		ULONG nulMode	// ģʽ
	);

	// ֪ͨϵͳ���̰���������״̬
	// ���أ� ���ط��㣬��ʾ�ɹ��������㣬��ʾʧ�ܣ�
	// flase�ر� true����
	DWORD EiSetKeyboardDownSounds(
		bool nbIsSet
	);

	// ֪ͨϵͳ���̰���������״̬
	// ���أ� ���ط��㣬��ʾ�ɹ��������㣬��ʾʧ�ܣ�
	// flase�ر� true����
	DWORD EiSetKeyboardUpSounds(
		bool nbIsSet
	);

	// OOBE cstart
	//
	// ���أ� �����㣬��ʾ�ɹ������ط��㣬��ʾ�����룻
	// return
	//		zero: success
	//		non-zero: error code
	DWORD EiOOBEStart();
	
	// ��ȡϵͳ������������С
	// ���أ�������С(0-100)
	DWORD EiGetKeySoundsVolume(void);

	// ����ϵͳ������������С
	// ���أ���
	void EiSetKeySoundsVolume(LONG nlVolume);//(0-100)

	// ֪ͨϵͳhomebar��̬�����仯
	// ���أ� ���ط��㣬��ʾ�ɹ��������㣬��ʾʧ�ܣ�
	DWORD EiHomebarChanged(
		ULONG nulMode	// ģʽ
	);

	// ֪ͨϵͳ��Ļ����
	// ���أ� ���ط��㣬��ʾ�ɹ��������㣬��ʾʧ�ܣ�
	DWORD EiSetOrientation(
		DWORD ndwOrientation	// ģʽ
	);

	// ֪ͨϵͳ
	// ���أ� ���ط��㣬��ʾ�ɹ��������㣬��ʾʧ�ܣ�
	void EiSetBCover(
		char* npszPath	// ģʽ
	);

	// ֪ͨϵͳB�������˳�˫��ģʽ
	// ���أ� ���ط��㣬��ʾ�ɹ��������㣬��ʾʧ�ܣ�
	void EiBCoverEnterDbTap(
		bool nbIsEnter
	);

	// ��������������
	// ���أ� ���ط��㣬��ʾ�ɹ��������㣬��ʾʧ�ܣ�
	DWORD EiSetKeyboardSensitivity(
		LONG nlLevel
	);

	// ֪ͨhomebar��B��������Ϊ�˽���ϸ���Ļ�Զ������bug
	// ���أ� ���ط��㣬��ʾ�ɹ��������㣬��ʾʧ�ܣ�
	DWORD EiCloseBCover(
		BOOL nbClose
	);

	// ֪ͨ���񲥷ż���������Ϊ������
	// ���أ� ���ط��㣬��ʾ�ɹ��������㣬��ʾʧ�ܣ�
	DWORD EiPlayKeyboardSound(
		ULONG nlType
	);

	// ֪ͨ����ѻ�����̬�л���ƽ��ģʽ
	// ���أ� ���ط��㣬��ʾ�ɹ��������㣬��ʾʧ�ܣ�
	DWORD EiChangeTabletMode(
		bool nbIsTablet  //Ϊ���ʾ�л���ƽ�壬�����л����ʼǱ�
	);

	// ֪ͨ������һ�µ�ǰ�л���Ӧ�������л�����û
	// ���أ� ���ط��㣬��ʾ�ɹ��������㣬��ʾʧ�ܣ�
	DWORD EiCheckAppStatus();


	// ����/�ر�knock knock����
	// ���أ� ���ط��㣬��ʾ�ɹ��������㣬��ʾʧ�ܣ�
	DWORD EiOpenKnockKnock(
		bool nbIsOpen  //Ϊ���ʾ��
	);


	// ֪ͨ����֪ͨ��ǰӦ����������tp area
	// ���أ� ���ط��㣬��ʾ�ɹ��������㣬��ʾʧ�ܣ�
	DWORD EiResetTpArea();
	
	// ֪ͨϵͳ����һ����ǰ�û��µ�Ӧ�ã������ĳ���ļ��л�ĳ����ҳ
	// ���أ� ���ط��㣬��ʾ�ɹ��������㣬��ʾʧ�ܣ�
	DWORD EiRunExeForCurrentUset(
		const wchar_t* npszFilePath,	// Ŀ���ļ���
		const wchar_t* npszCommandLine
	);

	// ֪ͨ����C��������仯
	// ���أ� ���ط��㣬��ʾ�ɹ��������㣬��ʾʧ�ܣ�
	DWORD EiSetCCoverOri(
		ULONG nulOri
	);

	// ֪ͨ�����Դ״̬�仯
	// ���أ� ���ط��㣬��ʾ�ɹ��������㣬��ʾʧ�ܣ�
	DWORD EiSetPowerStatus(
		DWORD ndwStatus
	);

	// ѯ���Ƿ���Ҫ��ʾoobe,ͬһ�û�ֻ��ʾһ��
	// ���أ� ���ط��㣬��ʾ�ɹ��������㣬��ʾʧ�ܣ�
	ULONG EiIsShowOOBE(
		bool& rbFlag
	);


	// ֪ͨ����8951״̬�仯
	// ���أ� ���ط��㣬��ʾ�ɹ��������㣬��ʾʧ�ܣ�
	DWORD Ei8951StatusChanged(
		bool nbIsConnect
	);

	// ѯ��8951�Ƿ���sleep״̬
	// ���أ� ���ط��㣬��ʾ�ɹ��������㣬��ʾʧ�ܣ�
	ULONG Ei8951IsSleep(
		bool& rbSleep
	);

	// ֪ͨHomebar�л�Ӧ��
	// ���أ� ���ط��㣬��ʾ�ɹ��������㣬��ʾʧ�ܣ�
	ULONG EiChangeApp(
		DWORD ldwID
	);
	// ֪ͨ����5182״̬�仯
	// ���أ� ���ط��㣬��ʾ�ɹ��������㣬��ʾʧ�ܣ�
	DWORD Ei5182StatusChanged(
		bool nbIsConnect
	);

	// ��ȡ��ǰ�û������б�
	// ���أ� ���ط��㣬��ʾ�ɹ��������㣬��ʾʧ�ܣ�
	DWORD EiGetUserDiskList(
		void
	);

#define EI_LAPTOP_MODE 2
#define EI_TENT_MODE 3


#ifdef __cplusplus
}
#endif



#endif//_EINKINTERNAL_H_
