/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


//////////////////////////////////////////////////////////////////////////
// �����ļ���Ŀ¼ת�����ߺ���
#ifndef _CMMPATH_H_
#define _CMMPATH_H_

// ���ڱ���ʹ��MAX_PATH��Ϊ�����������ԣ������ȳ���MAX_PATH�ĳ��ϣ�������Ҫ�������·���ĳ��ϣ������ʺ�ʹ�ñ�����
class CFilePathName {
public:
	CFilePathName();
	CFilePathName(const class CFilePathName& src);
	CFilePathName(const wchar_t* nszPathName); // ����������Ŀ¼�����һ���ַ�һ��Ҫ��L'\\'
	~CFilePathName(){}

	// ��̬��������
	static CFilePathName* Create(const wchar_t* nszPathName); // ����������Ŀ¼�����һ���ַ�һ��Ҫ��L'\\'

	// ��̬��������
	CFilePathName* Duplicate(void);

	// ����Ŀ¼�����ļ���������falseʧ�ܣ����ܵ�ԭ���޷�ת��λ�ã����߽��������nbForceBePath����==true����ת���Ľ����Я��'\\'ʱ�����ӽ�β'\\'
	//	ʵ���÷���ȡͬ��Ŀ¼����L".\\"��ȡ�ϲ�Ŀ¼����"..\\'
	bool Transform(const wchar_t* nszTransTo,bool nbForceBePath=false);
	bool Transform(const CFilePathName& roPath);

	// ��õ�ǰĿ¼
	bool SetByCurrentPath(void);

	// ���ģ������Ŀ¼���ļ���
	bool SetByModulePathName(void);

	// ��õ�ǰ�û���AppData\Roaming Ŀ¼ ���磺C:\Users\UserName\AppData\Roaming
	bool SetByUserAppData(void);

	// ����ΪĿ¼�������βû��'\\'�ͼ���
	void AssurePath(void);

	// ͨ��������������������ֱ���޸����ݣ������������ɵ��ַ���ΪMAX_PATH
	wchar_t* LockBuffer(){	// �˺������Ա���ε����Ի�û�����ָ�룬����ı���Buf�����ݣ��������UnlockBuffer����ʹ����ظ���Ч
		msiLength = 0;
		return mszPathName;
	}
	void UnlockBuffer();

	// ���
	void Clean(void);

	// �������·����
	const wchar_t* GetPathName(void) const {
		return mszPathName;
	}

	// ȡ�ļ����������ǰ�������һ��Ŀ¼���򷵻�L"\0"
	const wchar_t* GetFileName(
		int* npLength=NULL	// �������ֵ��ַ�����������'\0'��β
		)const;

	// ȡ��չ���������ǰ����Ĳ���һ���ļ������߲�������չ�����򷵻�L"\0"
	const wchar_t* GetExtName(void)const;

	// ����
	__inline void operator=(const class CFilePathName& src);
	// ���ַ�����ֵ
	__inline void operator=(const wchar_t* nswSrc);

	// ֻ����ȫ��Ŀ¼������ᱣ��'\\'
	bool CopyPath(const class CFilePathName& src);

	// �����������ȣ�������'\0'��β
	int GetLength(void)const{
		return msiLength;
	}

	// �ж��ļ���չ���Ƿ����ָ�������ͣ�ʹ��\0�ָ�������������\0��ʾ���ͽ���������ֵ<0������ϣ����򷵻ص�������ϵ����ͱ�ţ���0��ʼ
	int ExtNameMatch(const wchar_t* nszFilter)const;

	// ֱ�Ӹ����ַ���
	void operator+=(const class CFilePathName& src) throw(...);
	void operator+=(const wchar_t* nszAppend)throw(...);

	// �Ƚϣ��Ƿ����
	bool operator==(const class CFilePathName& right);
	bool operator!=(const class CFilePathName& right);

private:
	short msiLength;
	wchar_t mszPathName[MAX_PATH];
};


#endif//_CMMPATH_H_