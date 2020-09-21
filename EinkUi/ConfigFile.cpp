#ifdef KERNEL_CODE
#include "ntddk.h"
#else
#include "Windows.h"
#include "time.h"
#endif


#include "ConfigFile.h"


#ifndef KERNEL_CODE
#define COPYSTR_S(_D,_L,_S) wcscpy_s(_D,_L,_S)
#else
#define COPYSTR_S(_D,_L,_S) wcscpy(_D,_S)
#endif

DEFINE_BUILTIN_NAME(CConfigFile)
DEFINE_BUILTIN_NAME(CCfKeyInterface)
DEFINE_BUILTIN_NAME(CStableConfigFile)

#ifdef __cplusplus
extern "C" {
#endif


// �������ߴ�һ�������ļ������������ļ�������߷���NULL��ʾʧ�ܣ�ʧ�ܵ�ԭ��������ļ������ڻ����ļ���ʽ����
IConfigFile* __stdcall CfCreateConfig(
		IN const wchar_t* nszPathName,				// �ļ�������·����
		IN ULONG nuCreationDisposition			// ͬCreateFile API���ƣ���CfgIface.h�ļ��е���ض���
		)
{
	return  CConfigFile::CreateInstance(nszPathName,nuCreationDisposition);
}

// ͬ����ĺ����������ǣ�StableConfig���Զ�Ϊ�ļ�����һ�����ݸ�������ֹ�����ļ���������ԭ���µ�һ�ļ��ƻ�
// �����ļ�������ǰ�����δ򿪵Ĳ�ͬ�����������ǽ���ʹ�ã���Ϊ���ݣ��Ӷ���ֹĳ�ε�ʹ�ù������ƻ�������һ���ļ���
// ��Ҫע�⣬���ݵ��Ǵ�ǰ�����ݣ���һ���򿪵����ñ���θ�д�������һ��д����ļ�����𻵣��������Զ��ظ��Ĳ���ǰһ��д������ݣ�����ǰ�ζ����ǰ��󱣴�����ݡ�
// �磺��������ļ���Ϊabc.set����ô��ͬһ��Ŀ¼�½����ܴ���һ��abc_dup.set���ļ��������Ǹ�����
IConfigFile* __stdcall CfCreateStableConfig(
	IN const wchar_t* nszPathName,				// �ļ�������·����������ΪNULL����ʾ����һ�����ļ���ʱ��ָ���ļ���
	IN ULONG nuCreationDisposition	// ͬCreateFile API���ƣ���������CREATE_ALWAYS��CREATE_NEW��OPEN_ALWAYS��OPEN_EXISTING�����������
	)
{
	return CStableConfigFile::CreateInstance(nszPathName,nuCreationDisposition);
}
#ifdef __cplusplus
}
#endif

CConfigFile::CConfigFile()
{
	miReferenceCount = 1;	// �൱�ڼ�¼��һ������
	mpRootInterface = NULL;
	mpRoot = NULL;
}

CConfigFile::~CConfigFile()
{
	CMMASSERT(miReferenceCount<=0);
	while(moUnusedKeyInterface.Size()>0)
	{
		CCfKeyInterface* lpKey = moUnusedKeyInterface.Top();
		moUnusedKeyInterface.Pop();

		if(lpKey!=NULL)
			delete lpKey;
	}
	if(mpRootInterface != NULL)
		mpRootInterface->Release();
	// �ͷ��ڴ�
	if(mpRoot != NULL)
		RemoveKey((PCFKEY_NODE)mpRoot);
}

ULONG CConfigFile::InitOnCreate(
		IN const wchar_t* nszPathName,				// �ļ�������·����
		IN ULONG nuCreationDisposition			// ͬCreateFile API���ƣ���CfgIface.h�ļ��е���ض���
	)
{
	bool lbReval = false;
#ifdef KERNEL_CODE
	OBJECT_ATTRIBUTES ObjectAttributes;
	UNICODE_STRING FileName;
	IO_STATUS_BLOCK IoStatus;
	NTSTATUS Status = STATUS_UNSUCCESSFUL;
#endif
	PCF_FILE_HEAD lpHead = NULL;
	PVOID lpBuffer = NULL;
	PCF_KEY_ENTRY lpEntry = NULL;
	PCF_KEY_ENTRY lpEndEntry = NULL;
	HANDLE lhFile = NULL;
	bool lbResetConfig = false;

	do{
		// ���nszPathNameΪNULL����ʾ����һ���µ��ļ�
		if(nszPathName != NULL)
		{
			COPYSTR_S(mszFileName,256,nszPathName);
			// ���ļ�
	#ifndef KERNEL_CODE
			lhFile = ::CreateFileW(mszFileName,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,nuCreationDisposition,NULL,NULL);
			if(lhFile == INVALID_HANDLE_VALUE)
			{
				lhFile = NULL;
				break;
			}
			{
				LARGE_INTEGER lxFileSize;
				if(GetFileSizeEx(lhFile,&lxFileSize)==FALSE)
					break;
				if(lxFileSize.QuadPart == 0)
					lbResetConfig = true;
			}
	#else
			RtlInitUnicodeString(&FileName,mszFileName);
			InitializeObjectAttributes(
				&ObjectAttributes
				,&FileName
				,OBJ_CASE_INSENSITIVE|OBJ_KERNEL_HANDLE
				,NULL 
				,NULL );

			Status = ZwCreateFile(&lhFile,FILE_READ_DATA|SYNCHRONIZE,&ObjectAttributes,&IoStatus,NULL,FILE_ATTRIBUTE_NORMAL,FILE_SHARE_READ|FILE_SHARE_WRITE,nuCreationDisposition,FILE_SYNCHRONOUS_IO_NONALERT,NULL,0);
			if(!NT_SUCCESS(Status))
				break;

			if(IoStatus.Information == FILE_CREATED || IoStatus.Information == FILE_DOES_NOT_EXIST)
				lbResetConfig = true;

	#endif

			// �ж��ļ��Ƿ����½����ģ�������½���lbResetConfig������true
			if(lbResetConfig == false)
			{
				bool lbLoadOk = false;
				do 
				{
					// �����ļ�ͷ
					lpHead = new CF_FILE_HEAD;
					if(lpHead == NULL)
						break;

			#ifndef KERNEL_CODE
					ULONG luRead;
					if(ReadFile(lhFile,lpHead,sizeof(CF_FILE_HEAD),&luRead,NULL)==FALSE)
						break;
			#else
					if(STATUS_SUCCESS != ZwReadFile(lhFile,NULL,NULL,NULL,&IoStatus,lpHead,sizeof(CF_FILE_HEAD),NULL,NULL))
						break;
			#endif

					// ���汾
					if(lpHead->ShortHead.Signature != CF_SIGNATURE || lpHead->ShortHead.Version > CF_VERSION)
						break;

					// ����ļ��Ƿ���ʧ�ܵ��޸�
					if(lpHead->ShortHead.SequenceA != lpHead->Tail.SequenceB)
						break;	// �ϴε��޸�û����ȷ�����棬��ǰ�ļ�������

					if (lpHead->ShortHead.SizeOfKeys > 0)
					{
						// һ���ж���ȫ������
						lpBuffer = new UCHAR[lpHead->ShortHead.SizeOfKeys];
		#ifndef KERNEL_CODE
						if(ReadFile(lhFile,lpBuffer,lpHead->ShortHead.SizeOfKeys,&luRead,NULL)==FALSE)
							break;
		#else
						if(STATUS_SUCCESS != ZwReadFile(lhFile,NULL,NULL,NULL,&IoStatus,lpBuffer,lpHead->ShortHead.SizeOfKeys,NULL,NULL))
							break;
		#endif
						// �ж��Ƿ�����ȷ��У���
						if(lpHead->ShortHead.Version >= CF_VERSION)
						{
							ULONG luCheckSum = 0;
							for (ULONG i=0;i< lpHead->ShortHead.SizeOfKeys;i++)
							{
								luCheckSum += *((UCHAR*)lpBuffer+i);
							}
							if(luCheckSum != lpHead->ShortHead.CheckSum)
								break;	// У��Ͳ���
						}

						// �ݹ����ִ�м�װ��
						lpEntry = (PCF_KEY_ENTRY)lpBuffer;
						lpEndEntry = (PCF_KEY_ENTRY)((UCHAR*)lpBuffer + lpHead->ShortHead.SizeOfKeys);

						mpRoot = (PCFKEY_BRANCH)LoadKey(lpEntry,lpEndEntry);
						if(mpRoot == NULL)
							break;

						// ���Ƶ�ǰ�ļ�ͷ
						RtlCopyMemory(&mdFileHead,&lpHead->ShortHead,sizeof(mdFileHead));

						lbLoadOk = true;
					}
				} while(false);
				if(lbLoadOk == false)
				{
					if (mpRoot!=NULL)
						RemoveKey((PCFKEY_NODE)mpRoot);
					lbResetConfig = true;
				}
			}
		}
		else
		{
			mszFileName[0] = UNICODE_NULL;
			lbResetConfig = true;
		}


		if(lbResetConfig != false)
		{
			// ׼��Ĭ���ļ�ͷ
			mdFileHead.Signature = CF_SIGNATURE;
			mdFileHead.Version = CF_VERSION;
			mdFileHead.SizeOfHead = sizeof(CF_FILE_HEAD);
			mdFileHead.SizeOfReserved = 0;
			mdFileHead.SequenceA = 1;

			// ��һ���½����ļ�������һ���յ�Root�ڵ�ͺ���
			mpRoot = (PCFKEY_BRANCH)new UCHAR[sizeof(CFKEY_BRANCH)+sizeof(wchar_t)*4];
			if(mpRoot == NULL)
				break;
			mpRoot->Flag = IConfigFile::Binary|CFKEY_INDEX_AVAILABLE;
			CFKEY_SET_VALUETYPE(mpRoot->Flag,IConfigFile::Invalid);

			mpRoot->NameLength = 4;
			COPYSTR_S(mpRoot->Name,5,L"Root");
			mpRoot->mpSubKeys = new TCFKEYSEQUENCE;
			if(mpRoot->mpSubKeys == NULL)
				break;
			mpRoot->ValueLength = 0;
		}

		// ��ȡһ��RootKey
		{
			ICfKey* lpKey = GetRootKey();
			if(lpKey == NULL)
				break;
			lpKey->Release();
		}

		lbReval = true;

	}while(false);


	if(lpHead != NULL)
		delete lpHead;

	if(lpBuffer != NULL)
		delete lpBuffer;

	if(lhFile != NULL)
	{
#ifndef KERNEL_CODE
		CloseHandle(lhFile);
#else
		ZwClose(lhFile);
#endif
	}

	return (lbReval?0:(ULONG)-1);
}

// �����޸ĵ��ļ�
bool CConfigFile::SaveFile(
		IN const wchar_t* nszOtherFile	// ����ָ������Ϊ�������ļ������ΪNULL���Ᵽ�浽�ղŴ򿪻����½����ļ�
	)
{
	bool lbReval = false;
#ifdef KERNEL_CODE
	OBJECT_ATTRIBUTES ObjectAttributes;
	UNICODE_STRING FileName;
	IO_STATUS_BLOCK IoStatus;
	NTSTATUS Status = STATUS_UNSUCCESSFUL;
#endif
	PCF_FILE_HEAD lpHead = NULL;
	PVOID lpBuffer = NULL;
	PCF_KEY_ENTRY lpEntry = NULL;
	HANDLE lhFile = NULL;


	do{
		if(nszOtherFile != NULL)
		{
			COPYSTR_S(mszFileName,256,nszOtherFile);
		}

		if(mszFileName[0]==UNICODE_NULL)
			break;

		// ���ļ�
#ifndef KERNEL_CODE
		lhFile = ::CreateFileW(mszFileName,GENERIC_READ|GENERIC_WRITE,NULL,NULL,OPEN_ALWAYS,NULL,NULL);
		if(lhFile == INVALID_HANDLE_VALUE)
		{
			lhFile = NULL;
			break;
		}
#else
		RtlInitUnicodeString(&FileName,mszFileName);
		InitializeObjectAttributes(
			&ObjectAttributes
			,&FileName
			,OBJ_CASE_INSENSITIVE|OBJ_KERNEL_HANDLE
			,NULL 
			,NULL );

		Status = ZwCreateFile(&lhFile,FILE_WRITE_DATA|SYNCHRONIZE,&ObjectAttributes,&IoStatus,NULL,FILE_ATTRIBUTE_NORMAL,0,FILE_OPEN_IF,FILE_SYNCHRONOUS_IO_NONALERT,NULL,0);
		if(!NT_SUCCESS(Status))
			break;

#endif

		// ׼���ļ�ͷ
		lpHead = new CF_FILE_HEAD;
		if(lpHead == NULL)
			break;

		RtlZeroMemory(lpHead,sizeof(CF_FILE_HEAD));
		RtlCopyMemory(&lpHead->ShortHead,&mdFileHead,sizeof(mdFileHead));
		lpHead->ShortHead.Version = CF_VERSION;

		// �趨һ�������˳��ֵ����Ϊ�����޸��ļ��ı�־
		lpHead->ShortHead.SequenceA = lpHead->Tail.SequenceB + 1;

		LARGE_INTEGER lxFilePos;
		lxFilePos.QuadPart = 0;

#ifndef KERNEL_CODE
		ULONG luWritten;
		if(SetFilePointerEx(lhFile,lxFilePos,NULL,FILE_BEGIN)==FALSE)
			break;
		if(WriteFile(lhFile,lpHead,sizeof(CF_FILE_HEAD),&luWritten,NULL)==FALSE)
			break;
#else
		Status = ZwWriteFile(lhFile,NULL,NULL,NULL,&IoStatus,lpHead,sizeof(CF_FILE_HEAD),&lxFilePos,NULL);
		if(STATUS_SUCCESS != Status)
			break;
#endif

		// ����ȷ������Key��ռ�õĳߴ�
		moExclusive.Enter();
		lpHead->ShortHead.SizeOfKeys = (LONG)SaveKey((PCFKEY_NODE)mpRoot,lpEntry);
		if(lpHead->ShortHead.SizeOfKeys == 0)
		{
			moExclusive.Leave();
			break;
		}

		// ׼��ȫ���洢�ռ�
		// ׼���洢��
		lpBuffer = new UCHAR[lpHead->ShortHead.SizeOfKeys];

		// �ݹ��������
		lpEntry = (PCF_KEY_ENTRY)lpBuffer;
		//moExclusive.Enter();
		LONG liSize = (LONG)SaveKey((PCFKEY_NODE)mpRoot,lpEntry);
		moExclusive.Leave();
		CMMASSERT(((UCHAR*)lpEntry - (UCHAR*)lpBuffer) <= liSize);
		if(liSize != lpHead->ShortHead.SizeOfKeys)
			break;

		// ����У���
		lpHead->ShortHead.CheckSum = 0;
		for(ULONG i=0;i< lpHead->ShortHead.SizeOfKeys;i++)
		{
			lpHead->ShortHead.CheckSum += *((UCHAR*)lpBuffer+i);
		}

		// д���ļ�
		lxFilePos.QuadPart = sizeof(CF_FILE_HEAD);
#ifndef KERNEL_CODE
		if(WriteFile(lhFile,lpBuffer,lpHead->ShortHead.SizeOfKeys,&luWritten,NULL)==FALSE)
			break;
#else
		Status = ZwWriteFile(lhFile,NULL,NULL,NULL,&IoStatus,lpBuffer,lpHead->ShortHead.SizeOfKeys,&lxFilePos,NULL);
		if(STATUS_SUCCESS != Status)
			break;
#endif

		// �����ļ�ͷ������д������
		lpHead->ShortHead.SequenceA++;
		lpHead->Tail.SequenceB = lpHead->ShortHead.SequenceA;
		lxFilePos.QuadPart = 0;
#ifndef KERNEL_CODE
		if(SetFilePointerEx(lhFile,lxFilePos,NULL,FILE_BEGIN)==FALSE)
			break;
		if(WriteFile(lhFile,lpHead,sizeof(CF_FILE_HEAD),&luWritten,NULL)==FALSE)
			break;
#else
		Status = ZwWriteFile(lhFile,NULL,NULL,NULL,&IoStatus,lpHead,sizeof(CF_FILE_HEAD),&lxFilePos,NULL);
		if(STATUS_SUCCESS != Status)
			break;
#endif

		lbReval = true;

	}while(false);


	if(lpHead != NULL)
		delete lpHead;

	if(lpBuffer != NULL)
		delete lpBuffer;

	if( lhFile != NULL)
	{
#ifndef KERNEL_CODE
		CloseHandle(lhFile);
#else
		ZwClose(lhFile);
#endif
	}

	return lbReval;
}


// ��ø�������õĶ��󵱲��ٷ���ʱ��ҪRelease
ICfKey* CConfigFile::GetRootKey(void)
{

	moExclusive.Enter();
	if(mpRootInterface == NULL)
	{
		mpRootInterface = AllocateKeyInterface((PCFKEY_NODE)mpRoot);
		if(mpRootInterface == NULL)
		{
			moExclusive.Leave();
			return NULL;
		}

		mpRootInterface->mpParentsKey = NULL;
		mpRootInterface->miPositionInBrothers = -1;
		// ����Ҫ��ʼ���������ݺ����ƵĿ��
	}

	mpRootInterface->miReferenceCount++;
	

	moExclusive.Leave();

	return dynamic_cast<ICfKey*>(mpRootInterface);
}

// ����һ���µ�Node
PCFKEY_NODE CConfigFile::AllocateNode(
	IN const wchar_t* nszName,	// �Ӽ�������
	IN int niNameLen,		// count in wchar, if be -1 indicate that the nszName is terminated by '\0' or '\\' or '/'
	IN UCHAR nchFlag,	// �Ӽ���Ĭ��ֵ���ͣ����ΪInvalid������Ժ���������
	IN const void* npValueBuf,
	IN int  niValuelen,
	IN int  niSubKeyCount
	)
{
	int liSize;
	PCFKEY_NODE lpRetrun = NULL;

	if(nszName == NULL)
		niNameLen = 0;
	else
	{
		if(niNameLen < 0)
			niNameLen = 255;

		// the max length is 255
		for(liSize = 0;liSize<niNameLen;liSize++)
		{
			if(/*nszName[liSize] == L'\\' || nszName[liSize] == L'/' || */nszName[liSize] == L'<' || nszName[liSize] == L'>')
				return NULL;	// �����г��ַǷ��ַ�

			if(nszName[liSize]==L'\0' || nszName[liSize] == L'\\' || nszName[liSize] == L'/' )
				break;
		}

		niNameLen = liSize;
	}

	if(niValuelen == -1)
		niValuelen = GetValueLengthOfTypeString(CFKEY_VALUE_TYPE(nchFlag),npValueBuf);

	// ȥ���Ƿ��Ѵ򿪱�־���������������
	nchFlag &= ~CFKEY_NODE_OPENED;

	// �����Ƿ�����ӽڵ�
	if(niSubKeyCount > 0)
	{	// �����ӽڵ�
		PCFKEY_BRANCH lpBranch = NULL;
		ULONG* lpHash = NULL;

		do 
		{
			// ���㵱ǰ�ڵ���Ҫ�Ĵ洢��С
			liSize = sizeof(CFKEY_BRANCH) + niNameLen*sizeof(wchar_t) + niValuelen;

			// ���䵱ǰ�ڵ�
			lpBranch = (PCFKEY_BRANCH)new UCHAR[liSize];
			if(lpBranch == NULL)
				break;

			lpBranch->Flag = (nchFlag|CFKEY_INDEX_AVAILABLE);
			lpBranch->mpSubKeys = new TCFKEYSEQUENCE;
			if(lpBranch->mpSubKeys == NULL)
				break;

			// ��������
			lpBranch->NameLength = (UCHAR)niNameLen;
			if(niNameLen > 0 )
			{
				RtlCopyMemory(lpBranch->Name,nszName,niNameLen*sizeof(wchar_t));
				lpBranch->Name[niNameLen] = UNICODE_NULL;
			}

			// ��������
			lpBranch->ValueLength = (USHORT)niValuelen;
			if(lpBranch->ValueLength > 0)
			{
				RtlCopyMemory(&lpBranch->Name[lpBranch->NameLength]+1,npValueBuf,lpBranch->ValueLength);
			}

			// �ɹ�����
			lpRetrun = (PCFKEY_NODE)lpBranch;
			lpBranch = NULL;

		} while(false);

		if(lpBranch != NULL)
		{
			if(lpBranch->mpSubKeys != NULL)
				delete lpBranch->mpSubKeys;
			delete lpBranch;
		}
	}
	else
	{	// û���ӽڵ�
		PCFKEY_LEAF lpLeaf = NULL;

		do 
		{
			// ���㵱ǰ�ڵ���Ҫ�Ĵ洢��С
			liSize = sizeof(CFKEY_LEAF) + niNameLen*sizeof(wchar_t) + niValuelen;

			// ���䵱ǰ�ڵ�
			lpLeaf = (PCFKEY_LEAF)new UCHAR[liSize];
			if(lpLeaf == NULL)
				break;

			lpLeaf->Flag = nchFlag;

			// ��������
			lpLeaf->NameLength = (UCHAR)niNameLen;
			if(niNameLen > 0 )
			{
				RtlCopyMemory(lpLeaf->Name,nszName,niNameLen*sizeof(wchar_t));
				lpLeaf->Name[niNameLen] = UNICODE_NULL;
			}

			// ��������
			lpLeaf->ValueLength = (USHORT)niValuelen;
			if(lpLeaf->ValueLength > 0)
			{
				RtlCopyMemory(&lpLeaf->Name[lpLeaf->NameLength]+1,npValueBuf,niValuelen);
			}

			// �ɹ�����
			lpRetrun = (PCFKEY_NODE)lpLeaf;
			lpLeaf = NULL;

		} while(false);

		if(lpLeaf != NULL)
			delete lpLeaf;
	}

	return lpRetrun;
}


// ��ָ���ļ�����õĶ��󵱲��ٷ���ʱ��ҪRelease
ICfKey* CConfigFile::OpenKey(
		IN const wchar_t* nszKeyName,		// ��'/'�ָ�Ӽ�������������ָ�����磺xx/yy/zz
		IN bool nbCreateIf			// �������������ڣ�������
	)
{
	ICfKey* lpRoot = GetRootKey();
	ICfKey* lpSub = lpRoot->OpenKey(nszKeyName,nbCreateIf);
	lpRoot->Release();
	return lpSub;
}


// �ݹ���ã�װ��ָ�����������Ӽ�������ʱnrKeyEntry���޸�ָ����һ��Entry
PCFKEY_NODE CConfigFile::LoadKey(PCF_KEY_ENTRY& nrKeyEntry,const PCF_KEY_ENTRY npEndEntry)
{
	PCFKEY_NODE lpRetrun = NULL;

	if(nrKeyEntry >= npEndEntry)	// ����β���ˣ��϶����д�����
		return NULL;

	if(nrKeyEntry->NameLength > 255 )	// �д����ⲻ������汾�Ϸ��ı�־ֵ
		return NULL;


	// �����Ƿ�����ӽڵ�
	if((nrKeyEntry->Flag&CFKEY_INDEX_AVAILABLE) != 0)
	{	// �����ӽڵ�
		PCFKEY_BRANCH lpBranch = NULL;
		ULONG* lpHash = NULL;

		do 
		{
			// ���䵱ǰ�ڵ�
			lpBranch = (PCFKEY_BRANCH)AllocateNode((wchar_t*)(nrKeyEntry+1),nrKeyEntry->NameLength,nrKeyEntry->Flag,(UCHAR*)((wchar_t*)(nrKeyEntry+1)+nrKeyEntry->NameLength),nrKeyEntry->ValueSize,nrKeyEntry->SubEntryCount);
			if(lpBranch == NULL)
				break;

			lpBranch->mpSubKeys = new TCFKEYSEQUENCE;
			if(lpBranch->mpSubKeys == NULL)
				 break;

			// ѭ������ÿһ���ӽڵ㣬�����ӽڵ��Hashֵ���ӽڵ�һͬ������������
			int liSubCount = nrKeyEntry->SubEntryCount;
			lpHash = (ULONG*)((UCHAR*)nrKeyEntry + sizeof(CF_KEY_ENTRY) + nrKeyEntry->NameLength*sizeof(wchar_t) + nrKeyEntry->ValueSize);	// ָ��Hash��
			// �������ݺ��ƣ�ָ����һ����¼
			nrKeyEntry = (PCF_KEY_ENTRY)(lpHash + nrKeyEntry->SubEntryCount);	// ָ����һ����¼
			while(--liSubCount >=  0)
			{
				CCfKeyHash loNewSubKey;

				loNewSubKey.HashValue = *lpHash;
				loNewSubKey.KeyObj = LoadKey(nrKeyEntry,npEndEntry);

				if(loNewSubKey.KeyObj == NULL)
				{
					break;
				}
				lpBranch->mpSubKeys->Push_Back(loNewSubKey);
				lpHash++;
			}
			//////////////////////////////////////////////////////////////////////////
			// discarded by AX. Do not terminate the loading on the event that a bad key is found for fastboot version.
			//if(liSubCount >= 0)
			//	break;	// ������

			// �ɹ�����
			lpRetrun = (PCFKEY_NODE)lpBranch;
			lpBranch = NULL;

		} while(false);

		if(lpBranch != NULL)
			delete lpBranch;
	}
	else
	{	// û���ӽڵ�
		PCFKEY_LEAF lpLeaf = NULL;

		do 
		{
			// ���䵱ǰ�ڵ�
			lpLeaf = (PCFKEY_LEAF)AllocateNode((wchar_t*)(nrKeyEntry+1),nrKeyEntry->NameLength,nrKeyEntry->Flag,(UCHAR*)((wchar_t*)(nrKeyEntry+1)+nrKeyEntry->NameLength),nrKeyEntry->ValueSize);
			if(lpLeaf == NULL)
				break;

			// �������ݺ��ƣ�ָ����һ����¼
			nrKeyEntry = (PCF_KEY_ENTRY)((UCHAR*)nrKeyEntry + sizeof(CF_KEY_ENTRY) + nrKeyEntry->NameLength*sizeof(wchar_t) + nrKeyEntry->ValueSize);

			// �ɹ�����
			lpRetrun = (PCFKEY_NODE)lpLeaf;
			lpLeaf = NULL;

		} while(false);

		if(lpLeaf != NULL)
			delete lpLeaf;
	}

	return lpRetrun;
}

// �ݹ���ã����汾�����Ӽ���ֵ������0��ʾʧ�ܡ�����ʽnrKeyEntry��Խ���Ѿ���ŵ����ݡ��������ʱnrKeyEntry==NULL����ֻ������Ҫ�Ĵ洢����С
int CConfigFile::SaveKey(PCFKEY_NODE npKey,PCF_KEY_ENTRY& nrKeyEntry)
{
	int liSize;
	int liReval = 0;

	// �����Ƿ�����ӽڵ�
	if((npKey->Flag&CFKEY_INDEX_AVAILABLE) != 0)
	{	// �����ӽڵ�
		PCFKEY_BRANCH lpBranch = (PCFKEY_BRANCH)npKey;
		ULONG* lpHash = NULL;

		do 
		{
			// ���㵱ǰ�ڵ���Ҫ�Ĵ洢��С
			liSize = sizeof(CF_KEY_ENTRY) + lpBranch->NameLength*sizeof(wchar_t) + lpBranch->ValueLength + lpBranch->mpSubKeys->Size()*sizeof(ULONG);

			// ���nrKeyEntry==NULL��ֻ��Ҫ����洢������������Ӽ���Ҫ�Ŀռ�Ϳ�����
			if(nrKeyEntry != NULL)
			{
				nrKeyEntry->Flag = lpBranch->Flag;
				if(lpBranch->mpSubKeys->Size() == 0)
					nrKeyEntry->Flag &= (~CFKEY_INDEX_AVAILABLE);	// ˵�����Ѿ�û�����Ӽ������Բ������Ӽ���־

				// ��������
				nrKeyEntry->NameLength = lpBranch->NameLength;
				RtlCopyMemory((wchar_t*)(nrKeyEntry+1),lpBranch->Name,lpBranch->NameLength*sizeof(wchar_t));

				// ��������
				nrKeyEntry->ValueSize = lpBranch->ValueLength;
				if(lpBranch->ValueLength > 0)
				{
					RtlCopyMemory((UCHAR*)((wchar_t*)(nrKeyEntry+1)+nrKeyEntry->NameLength),&lpBranch->Name[lpBranch->NameLength]+1,lpBranch->ValueLength);
				}

				// ����Hash��
				lpHash = (ULONG*)((UCHAR*)nrKeyEntry + sizeof(CF_KEY_ENTRY) + nrKeyEntry->NameLength*sizeof(wchar_t) + nrKeyEntry->ValueSize);
				nrKeyEntry->SubEntryCount = 0;
				for(int i=0;i<lpBranch->mpSubKeys->Size();i++)
				{
					lpHash[i] = (*lpBranch->mpSubKeys)[i].HashValue;
					nrKeyEntry->SubEntryCount++;
				}

				// �������ݺ��ƣ�ָ����һ����¼
				nrKeyEntry = (PCF_KEY_ENTRY)(lpHash + nrKeyEntry->SubEntryCount);	// ָ����һ����¼
			}

			// ѭ������ÿһ���ӽڵ�
			int i;
			for(i=0;i<lpBranch->mpSubKeys->Size();i++)
			{
				int liSubSize = SaveKey((*lpBranch->mpSubKeys)[i].KeyObj,nrKeyEntry);
				if(liSubSize == 0)
					break;
				liSize += liSubSize;
			}
			if(i < lpBranch->mpSubKeys->Size())
				break;

			// �ɹ�����
			liReval = liSize;

		} while(false);
	}
	else
	{	// û���ӽڵ�
		PCFKEY_LEAF lpLeaf = (PCFKEY_LEAF)npKey;

		do 
		{
			// ���㵱ǰ�ڵ���Ҫ�Ĵ洢��С
			liSize = sizeof(CF_KEY_ENTRY) + lpLeaf->NameLength*sizeof(wchar_t) + lpLeaf->ValueLength;

			// ���nrKeyEntry==NULL��ֻ��Ҫ����洢������������Ӽ���Ҫ�Ŀռ�Ϳ�����
			if(nrKeyEntry != NULL)
			{
				nrKeyEntry->Flag = lpLeaf->Flag;

				// ��������
				nrKeyEntry->NameLength = lpLeaf->NameLength;
				RtlCopyMemory((wchar_t*)(nrKeyEntry+1),lpLeaf->Name,lpLeaf->NameLength*sizeof(wchar_t));

				// ��������
				nrKeyEntry->ValueSize = lpLeaf->ValueLength;
				if(lpLeaf->ValueLength > 0)
				{
					RtlCopyMemory((UCHAR*)((wchar_t*)(nrKeyEntry+1)+nrKeyEntry->NameLength),&lpLeaf->Name[lpLeaf->NameLength]+1,lpLeaf->ValueLength);
				}

				// �������ݺ��ƣ�ָ����һ����¼
				nrKeyEntry = (PCF_KEY_ENTRY)((UCHAR*)nrKeyEntry + sizeof(CF_KEY_ENTRY) + nrKeyEntry->NameLength*sizeof(wchar_t) + nrKeyEntry->ValueSize);
			}

			// �ɹ�����
			liReval = liSize;
		} while(false);

	}

	return liReval;
}

// ɾ����ֵ�������Ӽ�ֵ
bool CConfigFile::RemoveKey(PCFKEY_NODE npKey)
{
	if((npKey->Flag&CFKEY_NODE_OPENED)!=0)
	{
		CCfKeyInterface* lpInterface = FindValidKeyInterface(npKey);
		// ʹ����Ч
		lpInterface->mbDeleted = true;
	}

	// �Ƿ�����ӽڵ�
	if(CFKEY_HAS_CHILD(npKey))
	{
		PCFKEY_BRANCH lpBranch = (PCFKEY_BRANCH)npKey;
		for (int i=0;i<lpBranch->mpSubKeys->Size();i++)
		{
			if(RemoveKey((*lpBranch->mpSubKeys)[i].KeyObj)==false)
				return false;
		}
		delete lpBranch->mpSubKeys;
	}

	// �ͷ��ڴ�
	delete npKey;
	
	return true;
}

// ѯ���Ƿ����ɾ��һ���Ӽ�ֵ�����������²�ĳ����ֵ
bool CConfigFile::QueryRemove(PCFKEY_NODE npKey)
{
	// �Ƿ�����ӽڵ�
	if(CFKEY_HAS_CHILD(npKey))
	{
		PCFKEY_BRANCH lpBranch = (PCFKEY_BRANCH)npKey;
		for (int i=0;i<lpBranch->mpSubKeys->Size();i++)
		{
			if(QueryRemove((*lpBranch->mpSubKeys)[i].KeyObj)==false)
				return false;	// ���ĳ���Ӽ����ܱ�ɾ����������Ҳ���ܱ�ɾ��
		}
	}
	// ��������������ܱ�ɾ��
	if((npKey->Flag&CDKEY_FLAG_LOCKED)!=0)
		return false;

	return true;
}

// ��һ��Key
CCfKeyInterface* CConfigFile::GetKeyInterface(CCfKeyInterface* npParents,int niPosition)
{
	CCfKeyInterface* lpInterface;

	if (niPosition < 0 || npParents->mbDeleted != false ||  CFKEY_HAS_CHILD(npParents->BaseNode)==false || niPosition >= npParents->Branch->mpSubKeys->Size())
		return NULL;

	// ����Ƿ��Ѿ���
	if(((*npParents->Branch->mpSubKeys)[niPosition].KeyObj->Flag&CFKEY_NODE_OPENED)!=0)
	{
		lpInterface = FindValidKeyInterface((*npParents->Branch->mpSubKeys)[niPosition].KeyObj);
		CMMASSERT(lpInterface->miPositionInBrothers == niPosition);
		if(lpInterface != NULL)
		{
			// �������ü�¼
			lpInterface->miReferenceCount++;
			return lpInterface;
		}
	}
	lpInterface = AllocateKeyInterface((*npParents->Branch->mpSubKeys)[niPosition].KeyObj);
	if(lpInterface != NULL)
	{
		lpInterface->miPositionInBrothers = niPosition;
		lpInterface->mpParentsKey = npParents;
		npParents->miReferenceCount++;	// ���ӶԸ��ڵ�����ü�¼

		// ��ʶ�ýڵ㱻��
		lpInterface->BaseNode->Flag |= CFKEY_NODE_OPENED;
	}

	return lpInterface;
}

// ����һ����Key�Ĵ򿪶���
CCfKeyInterface* CConfigFile::AllocateKeyInterface(PCFKEY_NODE npKeyNode)
{
	CCfKeyInterface* lpInterface;

	if(moUnusedKeyInterface.Size() > 0 )
	{
		lpInterface = moUnusedKeyInterface.Top();
		moUnusedKeyInterface.Pop();

		lpInterface->Reuse(this);
	}
	else
	{
		lpInterface = CCfKeyInterface::CreateInstance(this);
	}

	if(lpInterface == NULL)
		return NULL;

	lpInterface->BaseNode = npKeyNode;

	CCfKeyOpened loAdd;

	loAdd.KeyNode = npKeyNode;
	loAdd.Interface = lpInterface;

	moKeyOpened.Insert(loAdd);

	return lpInterface;
}

// ���Ӷ�һ��Key�����ã�ע�⣬���ڽ���SpinLock�������������ֻ��CCfKeyInterface���ã�������봦��SpinLock��ʱ��ֱ�ӶԸ�ֵ��1
int CConfigFile::AddReferToKeyInterface(CCfKeyInterface* npKeyInterface)
{
	int liValue;

	moExclusive.Enter();

	liValue = npKeyInterface->miReferenceCount++;	

	moExclusive.Leave();

	return liValue;
}

// ���ٶ�һ��Key�����ã��������Ϊ0���ͷ�����ע�⣬�������ֻ��CCfKeyInterface����
int CConfigFile::ReleaseKeyInterface(CCfKeyInterface* npKeyInterface)
{
	TCFKEYINTERFACESTACK loKeysToDelete;
	int liIndex;
	int liReval = -1;

	moExclusive.Enter();

	// ������Ƿ�������Ѵ򿪶�����
	while(npKeyInterface != NULL && (liIndex = FindKeyInterface(npKeyInterface)) >= 0)
	{
		// �������ü���
		npKeyInterface->miReferenceCount--;
		if(liReval == -1)
			liReval = npKeyInterface->miReferenceCount;

		if(npKeyInterface->miReferenceCount <= 0)	// ��Ҫɾ����
		{
			// �Ӵ򿪶������õ�
			moKeyOpened.RemoveByIndex(liIndex);

			// �����Node�ϵĴ򿪱�־
			if(npKeyInterface->mbDeleted == false)
				npKeyInterface->BaseNode->Flag &= ~CFKEY_NODE_OPENED;

			// ���뵽�ͷ�ջ��
			loKeysToDelete.Push(npKeyInterface);

			// ��ȥ�Ը��ڵ������
			npKeyInterface = npKeyInterface->mpParentsKey;
		}
		else
			break;
	}

	moExclusive.Leave();

	// �ͷ����е�KeyInterface
	while(loKeysToDelete.Size() != 0)
	{
		npKeyInterface = loKeysToDelete.Top();
		loKeysToDelete.Pop();
		if(npKeyInterface!=NULL)
		{
			moExclusive.Enter();

			if(moUnusedKeyInterface.Size() < CF_LOOKASIDE_UNUSEDKEY_COUNT)
				moUnusedKeyInterface.Push(npKeyInterface);
			else
				delete npKeyInterface;

			moExclusive.Leave();
		}
	}
	return liReval;
}

//// ����һ���򿪵�KEY
//CCfKeyInterface* CConfigFile::FindKeyInterface(PCFKEY_NODE npKeyNode,bool nbTakeOff)
//{
//	CCfKeyOpened loToFind;
//
//	loToFind.Interface = NULL;
//	loToFind.KeyNode = npKeyNode;
//
//	int liIndex = moKeyOpened.Find(loToFind);
//	if(liIndex >= 0)
//	{
//		CCfKeyInterface* lpReval = moKeyOpened[liIndex].Interface;
//		if(nbTakeOff != false)
//			moKeyOpened.RemoveByIndex(liIndex);
//		return lpReval;
//	}
//
//	return NULL;
//}

// ����һ���򿪲�����Ч��KEY����Ϊ���ܴ���ָ��ͬһ��npKeyNode�Ĳ�ͬnpKeyInterface������ֻ��һ������Ч�ģ�������ָ��Ķ��������Ѿ�ɾ����������Keyֵ
CCfKeyInterface* CConfigFile::FindValidKeyInterface(PCFKEY_NODE npKeyNode)
{
	CCfKeyOpened loToFind;

	loToFind.Interface = NULL;
	loToFind.KeyNode = npKeyNode;

	int liIndex = moKeyOpened.Find(loToFind);
	if(liIndex < 0)
		return NULL;

	for(;liIndex < moKeyOpened.Size() && moKeyOpened[liIndex].KeyNode == npKeyNode ;liIndex++)
	{
		// ֻ����һ��δ��ɾ������Ч��KeyInterface����
		if(moKeyOpened[liIndex].Interface->mbDeleted == false)
			return moKeyOpened[liIndex].Interface;
	}

	return NULL;
}

//// ����һ����һ������������KeyInterface��ע�⣬�����Ƕ�Ӧ��Key�Ѿ���ɾ����Interface��û�б��ͷţ����������ڵ�λ�ã�С��0��ʾû�ҵ�
//int CConfigFile::FindKeyInterface(PCFKEY_NODE npKeyNode)
//{
//	CCfKeyOpened loToFind;
//
//	loToFind.Interface = NULL;
//	loToFind.KeyNode = npKeyNode;
//
//	return moKeyOpened.Find(loToFind);
//}

// ����һ��KeyInterface�Ƿ��ڱ��������д��ڣ����������ڵ�λ�ã�С��0��ʾû�ҵ�
int CConfigFile::FindKeyInterface(CCfKeyInterface* npKeyInterface)
{
	CCfKeyOpened loToFind;

	loToFind.Interface = NULL;
	loToFind.KeyNode = npKeyInterface->BaseNode;

	int liIndex = moKeyOpened.Find(loToFind);
	if(liIndex >= 0)
	{
		for(;liIndex < moKeyOpened.Size();liIndex++)
		{
			// npKeyInterface �ǲ����ظ����ֵĵ�ַ
			if(moKeyOpened[liIndex].Interface == npKeyInterface)
				break;
		}
	}
	return liIndex;
}

// ����һ���򿪵ļ���Nodeָ��
bool CConfigFile::UpdateInterface(CCfKeyInterface* npInterface,PCFKEY_NODE npKeyNode)
{
	CCfKeyOpened loToFind;

	int liIndex = FindKeyInterface(npInterface);
	CMMASSERT(liIndex >= 0);

	if(moKeyOpened.RemoveByIndex(liIndex)==false)
		return false;

	if((PCFKEY_BRANCH)npInterface->BaseNode == mpRoot)
		mpRoot = (PCFKEY_BRANCH)npKeyNode;

	delete npInterface->BaseNode;

	npKeyNode->Flag |= CFKEY_NODE_OPENED;
	npInterface->BaseNode = npKeyNode;
	loToFind.Interface = npInterface;
	loToFind.KeyNode = npKeyNode;

	return moKeyOpened.Insert(loToFind)>=0;

}

// ����һ���򿪵ļ���SubNodes���飬��Ϊ���鷢���˿��ܵ�������Interface������ָ��ĳ��SubNode)ʧЧ��������Ҫ����
void CConfigFile::UpdateSubNodes(CCfKeyInterface* npInterface)
{
	CCfKeyInterface* lpSubIntf;
	for (int i=0;i< npInterface->Branch->mpSubKeys->Size();i++)
	{
		lpSubIntf = FindValidKeyInterface(npInterface->GetSubNode(i));
		if(lpSubIntf != NULL)
			lpSubIntf->miPositionInBrothers = i;
	}
}

// �ж��ַ���ֵ�ĳ���
int CConfigFile::GetValueLengthOfTypeString(IConfigFile::VALUETYPE nuValueType,const void* npValueBuf)
{
	int liValuelen = 0;
	if(nuValueType == IConfigFile::AnsiString)
		while(((char*)npValueBuf)[liValuelen++]!=0);
	else
		if(nuValueType == IConfigFile::UnicodeString)
		{
			while(((wchar_t*)npValueBuf)[liValuelen++]!=0);
			liValuelen *= 2;
		}

	return liValuelen;
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

// ��������
int CCfKeyInterface::AddRefer(void)
{
	return mpConfigFile->AddReferToKeyInterface(this);
}

//�ͷż����½����򿪣������κ�һ�ַ����õ��ı�����󶼱����ͷ�
int CCfKeyInterface::Release(void)
{
	return mpConfigFile->ReleaseKeyInterface(this);
}


//ɾ���������ע�⣬����ɾ����Ľӿ���Ȼ��ҪRelease�����ù�Delete������ӿ�Ҳ��û�����ˣ���Release�⣬�������ܶ���֧����
bool CCfKeyInterface::Delete(void)
{
	bool lbResult = false;
	mpConfigFile->moExclusive.Enter();

	do 
	{
		if(mbDeleted != false)
			break ;

		// ȷ�ϸ�����Ч
		if(mpParentsKey == NULL || mpParentsKey->mbDeleted != false || CFKEY_HAS_CHILD(mpParentsKey->BaseNode)==false)
			break;
		
		CMMASSERT(miPositionInBrothers >= 0 && miPositionInBrothers < mpParentsKey->Branch->mpSubKeys->Size());


		PCFKEY_NODE lpKeyNode = mpParentsKey->GetSubNode(miPositionInBrothers);

		// ȷ���Ƿ��ܹ���ɾ��
		if(mpConfigFile->QueryRemove(lpKeyNode)==false)
			break;

		// ���ȴӸ������Ӽ��б����õ�����
		if(mpParentsKey->RemoveSubNode(miPositionInBrothers)==false)
		{
			break;
		}

		// ����ͬ�������д򿪵ļ�
		mpConfigFile->UpdateSubNodes(mpParentsKey);

		// ɾ����Ӧ��Node��ȫ��������Node
		lbResult = mpConfigFile->RemoveKey(lpKeyNode);

	} while(false);

	mpConfigFile->moExclusive.Leave();

	return lbResult;
}
	

// �½�һ����������NULL��ʾʧ�ܣ�ʧ�ܵ�ԭ��������Ѿ�������ͬ�ļ�ֵ�����߷����ڴ�ʧ��
ICfKey* CCfKeyInterface::NewSubKey(
	IN const wchar_t* nszName,	// �Ӽ������֣�����Ϊ��
	IN IConfigFile::VALUETYPE nuValueType,	// �Ӽ���Ĭ��ֵ���ͣ����ΪInvalid������Ժ���������
	IN const void* npValueBuf,
	IN LONG  niValuelen	// �ַ�������ʱ������ͨ����β��\0��������ֵ�ĳ��ȣ���ʱ����Ϊ-1
	)
{
	PCFKEY_LEAF lpLeaf;
	ICfKey* lpReval = NULL;

	mpConfigFile->moExclusive.Enter();
	do 
	{
		if(mbDeleted != false)
			break;

		// �ж��Ƿ��Ѿ�����
		if(CFKEY_HAS_CHILD(BaseNode)!=false && FindSubKey(nszName,-1) >= 0)
			break;

		// ����ڵ�
		lpLeaf = (PCFKEY_LEAF)mpConfigFile->AllocateNode(nszName,-1,nuValueType,npValueBuf,niValuelen);
		if(lpLeaf == NULL)
			break;

		// ����
		int liPos = InsertSubNode((PCFKEY_NODE)lpLeaf);
		if(liPos < 0)
		{
			CMMASSERT(0);
			break;	// ���ڲ�֪��������ʲô��������ԣ������ͷ������������Ƭ���ڴ�
		}

		// ����ͬ�������д򿪵ļ�
		mpConfigFile->UpdateSubNodes(this);

		// ������Ӽ�
		lpReval = mpConfigFile->GetKeyInterface(this,liPos);

	} while(false);

	mpConfigFile->moExclusive.Leave();

	return lpReval;
}

// �½�һ��������������NULL��ʾʧ�ܣ����ͬʱ����������ֵ�����п��ܴ���������ֵ�����������м�����
ICfKey* CCfKeyInterface::NewSubKey(
	IN ULONG nuID,		// ָ���������ı�ʶID�������Բ�Ҫʹ����ͬ��ʶ
	IN bool nbAhead,	// ��������ͬID���Ӽ�ʱ������ͬID�Ӽ�֮ǰ�����¼�����������ͬID�Ӽ�֮�����¼�
	IN IConfigFile::VALUETYPE nuValueType,	// �Ӽ���Ĭ��ֵ���ͣ����ΪInvalid������Ժ���������
	IN const void* npValueBuf,
	IN LONG  niValuelen	// �ַ�������ʱ������ͨ����β��\0��������ֵ�ĳ��ȣ���ʱ����Ϊ-1
	)
{
	PCFKEY_LEAF lpLeaf;
	ICfKey* lpReval = NULL;

	if(nuID == 0)
		return NULL;

	mpConfigFile->moExclusive.Enter();
	do 
	{
		if(mbDeleted != false)
			break;

		//// �ж��Ƿ��Ѿ�����
		//if(CFKEY_HAS_CHILD(BaseNode)!=false && FindSubKey(nuID) >= 0)
		//	break;

		// ����ڵ�
		lpLeaf = (PCFKEY_LEAF)mpConfigFile->AllocateNode(NULL,0,nuValueType,npValueBuf,niValuelen);
		if(lpLeaf == NULL)
			break;

		// ����
		int liPos = InsertSubNode((PCFKEY_NODE)lpLeaf,nuID,nbAhead);
		if(liPos < 0)
		{
			CMMASSERT(0);
			break;	// ���ڲ�֪��������ʲô��������ԣ������ͷ������������Ƭ���ڴ�
		}

		// ����ͬ�������д򿪵ļ�
		mpConfigFile->UpdateSubNodes(this);

		// ������Ӽ�
		lpReval = mpConfigFile->GetKeyInterface(this,liPos);

	} while(false);

	mpConfigFile->moExclusive.Leave();

	return lpReval;
}

// ��ñ����ĸ�������������Ǹ������򷵻�NULL
ICfKey* CCfKeyInterface::GetParentsKey(void)
{
	if(mpParentsKey!=NULL)
		mpParentsKey->AddRefer();
	return mpParentsKey;
}

// �������ø��ڵ㣬�����ڵ�ӵ�ǰ���ڵ��Ƴ��������µĸ��ڵ���
bool __stdcall CCfKeyInterface::SetParentKey(
	IN ICfKey* npNewParent,
	IN bool nbAhead
	)
{
	wchar_t lswName[MAX_PATH];
	bool lbHasName = false;
	bool lbResult = false;
	CCfKeyInterface* lpNewParent;
	ULONG luID;

	if(GetName(lswName,MAX_PATH)>0)
	{
		lbHasName = true;
		ICfKey* lpExist = npNewParent->GetSubKey(lswName);
		if(lpExist != NULL)
		{	// ���������ܴ�����ͬ����
			lpExist->Release();
			return false;
		}
	}
	else
		luID = GetID(NULL);

	lpNewParent = dynamic_cast<CCfKeyInterface*>(npNewParent);

	mpConfigFile->moExclusive.Enter();
	do 
	{
		lbResult = false;

		if(mbDeleted != false)
			break;

		// �Ӹ��ڵ��Ƴ�
		if(mpParentsKey->RemoveSubNode(miPositionInBrothers)==false)
			break;

		// ���¸��ڵ�
		mpConfigFile->UpdateSubNodes(mpParentsKey);

		// ���µĸ��ڵ��ϲ����ӽڵ�
		if(lbHasName!=false)
		{
			if(lpNewParent->InsertSubNode(BaseNode)<0)
				break;
		}
		else
		{
			if(lpNewParent->InsertSubNode(BaseNode,luID,nbAhead)<0)
				break;
		}

		// �����¸��ڵ�
		mpConfigFile->UpdateSubNodes(lpNewParent);

		lbResult = true;

	} while(false);

	mpConfigFile->moExclusive.Leave();

	return lbResult;
}

// �����һ��������õĶ��󵱲��ٷ���ʱ��ҪRelease,���ջ�ȡ��ǰKey�õ�ͬһ�������
ICfKey* CCfKeyInterface::GetNextKey(void)
{
	ICfKey* lpReval = NULL;

	mpConfigFile->moExclusive.Enter();

	if(mbDeleted == false)
		lpReval =  mpConfigFile->GetKeyInterface(mpParentsKey,miPositionInBrothers+1);

	mpConfigFile->moExclusive.Leave();

	return lpReval;
}

// �����һ�����������ͷŵ�ǰ������õĶ��󵱲��ٷ���ʱ��ҪRelease
ICfKey* CCfKeyInterface::MoveToParentsKey(void)
{
	ICfKey* lpNext = GetParentsKey();

	mpConfigFile->ReleaseKeyInterface(this);

	return lpNext;
}

// �����һ�����������ͷŵ�ǰ������õĶ��󵱲��ٷ���ʱ��ҪRelease
ICfKey* CCfKeyInterface::MoveToNextKey(void)
{
	ICfKey* lpNext = GetNextKey();

	mpConfigFile->ReleaseKeyInterface(this);

	return lpNext;
}

// �����һ������ɾ�����ͷŵ�ǰ��!!!����õĶ��󵱲��ٷ���ʱ��ҪRelease
ICfKey* CCfKeyInterface::MoveToNextKey(bool nbDelete)
{
	ICfKey* lpNext = GetNextKey();

	if(nbDelete != false)
		Delete();

	mpConfigFile->ReleaseKeyInterface(this);

	return lpNext;
}

// ��õ�һ���Ӽ��������ͷŵ�ǰ��!!!����õĶ��󵱲��ٷ���ʱ��ҪRelease
ICfKey* CCfKeyInterface::MoveToSubKey(void)
{
	ICfKey* lpNext = GetSubKey();

	mpConfigFile->ReleaseKeyInterface(this);

	return lpNext;
}

// ���ָ���Ӽ��������ͷŵ�ǰ��!!!����õĶ��󵱲��ٷ���ʱ��ҪRelease
ICfKey* CCfKeyInterface::MoveToSubKey(
	IN ULONG nuID		// �Ӽ��ı�ʶID
	)
{
	ICfKey* lpNext = GetSubKey(nuID);

	mpConfigFile->ReleaseKeyInterface(this);

	return lpNext;
}

// ���ָ���Ӽ��������ͷŵ�ǰ��!!!����õĶ��󵱲��ٷ���ʱ��ҪRelease
ICfKey* CCfKeyInterface::MoveToSubKey(
	IN const wchar_t* nszName,	// ��������ȥ��ȡ�Ӽ�
	IN int niNameLen	// -1 indicate nszName is terminated by '\0' or '\\' or '/', >=0 is the charactar count of nszName
	)
{
	ICfKey* lpNext = GetSubKey(nszName,niNameLen);

	mpConfigFile->ReleaseKeyInterface(this);

	return lpNext;
}



// ��õ�һ���Ӽ�����õĶ��󵱲��ٷ���ʱ��ҪRelease
ICfKey* CCfKeyInterface::GetSubKey(void)
{
	ICfKey* lpReval = NULL;

	mpConfigFile->moExclusive.Enter();

	if(mbDeleted == false)
		lpReval =  mpConfigFile->GetKeyInterface(this,0);

	mpConfigFile->moExclusive.Leave();

	return lpReval;
}

// �򿪶��·��ָ�����Ӽ�����õĶ��󵱲��ٷ���ʱ��ҪRelease��������ͬ�����GetSubKey�����ǣ����������Դ�һ��·���磺xx/yy/zzָ�������²��zz�Ӽ�����GetSubKeyֻ��򿪵�һ���xx�Ӽ�
ICfKey* CCfKeyInterface::OpenKey(
	IN const wchar_t* nszKeyName,		// ��'/'�ָ�Ӽ�������������ָ�����磺xx/yy/zz
	IN bool nbCreateIf			// �������������ڣ�������
	)
{
	ICfKey* lpParents;
	ICfKey* lpCurrent;
	const wchar_t* lpName;
	int liLength;

	if(nszKeyName==NULL)
		return NULL;

	lpParents = this;
	lpParents->AddRefer();

	if(nszKeyName[0] == UNICODE_NULL)
		return lpParents;

	lpName = nszKeyName;
	lpCurrent = NULL;

	do 
	{
		// �����Ŀ¼
		lpCurrent = lpParents->GetSubKey(lpName,-1,&liLength,nbCreateIf);

		if(liLength < 0 || lpCurrent == NULL)
		{
			lpParents->Release();
			break;
		}

		// ��������һ��
		lpParents->Release();
		lpParents = lpCurrent;

		if(lpName[liLength]==UNICODE_NULL)
			break;

		lpName += liLength+1;

	} while(*lpName != UNICODE_NULL);

	return lpCurrent;
}


// ����Ӽ�����õĶ��󵱲��ٷ���ʱ��ҪRelease��������ͬ�����OpenKey�����ǣ�OpenKey���Դ�һ��·���磺xx/yy/zzָ�������²��zz�Ӽ�����GetSubKeyֻ��򿪵�һ���xx�Ӽ�
ICfKey* CCfKeyInterface::GetSubKey(
		IN const wchar_t* nszName,	// ��������ȥ��ȡ�Ӽ�
		IN int niNameLen,	// -1 indicate nszName is terminated by '\0' or '\\' or '/', >=0 is the charactar count of nszName
		OUT int* npNameLen,	// �������ֵ���Ч����
		IN bool nbCreateIf		// ����ü������ڣ�������
	)
{
	int liPos;
	ICfKey* lpReval=NULL;

	mpConfigFile->moExclusive.Enter();

	if(mbDeleted == false && CFKEY_HAS_CHILD(BaseNode)!=false)
	{
		liPos = FindSubKey(nszName,niNameLen,npNameLen);
		if(liPos >= 0)
			lpReval =  mpConfigFile->GetKeyInterface(this,liPos);
	}

	mpConfigFile->moExclusive.Leave();

	if(lpReval == NULL && nbCreateIf)
	{
		lpReval = NewSubKey(nszName);
		if(lpReval != NULL && npNameLen != NULL)
			*npNameLen = lpReval->GetName(NULL,0);
	}

	return lpReval;
}

// ����Ӽ�����õĶ��󵱲��ٷ���ʱ��ҪRelease��������ͬ�����GetSubKey�����ǣ�����İ����ִ򿪼�ֵ����������ֱ�Ӱ�ID�򿪣�ֻ��ͨ�����������ܴ�û�����ֵļ�ֵ
ICfKey* CCfKeyInterface::GetSubKey(
	IN ULONG nuID,		// �Ӽ��ı�ʶID
	IN int niPos		// ����ͬ��ID�ļ�ֵ�е��Ⱥ�λ�ã�< 0��ʾȡ���һ��
	)
{
	int liPos;
	ICfKey* lpReval=NULL;

	mpConfigFile->moExclusive.Enter();

	if(mbDeleted == false && CFKEY_HAS_CHILD(BaseNode)!=false)
	{
		liPos = FindSubKey(nuID,niPos);
		if(liPos >= 0)
			lpReval =  mpConfigFile->GetKeyInterface(this,liPos);
	}

	mpConfigFile->moExclusive.Leave();

	return lpReval;
}


// ��ñ�����Ĭ��ֵ����
IConfigFile::VALUETYPE CCfKeyInterface::GetValueType(void)
{
	IConfigFile::VALUETYPE luValue = IConfigFile::Invalid;

	mpConfigFile->moExclusive.Enter();

	if(mbDeleted == false)
		luValue =  CFKEY_VALUE_TYPE(BaseNode->Flag);

	mpConfigFile->moExclusive.Leave();

	return luValue;
}

// ��ñ�������չ��־������������
UCHAR CCfKeyInterface::GetExtFlag(void)
{
	UCHAR lcFlag = 0;

	mpConfigFile->moExclusive.Enter();

	if(mbDeleted == false)
		lcFlag =  CFKEY_GET_EXTFLAG(BaseNode->Flag);

	mpConfigFile->moExclusive.Leave();

	return lcFlag;
}


// ��ñ�����Ĭ��ֵ���ȣ��ֽڵ�λ
LONG CCfKeyInterface::GetValueLength(void)
{
	LONG liLength = 0;

	mpConfigFile->moExclusive.Enter();

	if(mbDeleted == false)
		liLength =  BaseNode->ValueLength;

	mpConfigFile->moExclusive.Leave();

	return liLength;
}

// ��ñ������ڵ����
int CCfKeyInterface::GetPosition(void)
{
	if(mbDeleted != false)
		return -1;
	return miPositionInBrothers;
}

// ���ID������������ֵ���Ի�õ���Hashֵ
ULONG CCfKeyInterface::GetID(
	OUT int* npPos		// ����ͬ��ID�ļ�ֵ�е��Ⱥ�λ��
	)
{
	ULONG luID;

	mpConfigFile->moExclusive.Enter();

	if(mbDeleted == false)
	{
		luID = (*mpParentsKey->Branch->mpSubKeys)[miPositionInBrothers].HashValue;
		if(npPos!=NULL)
		{
			*npPos = 0;
			for(int i=miPositionInBrothers-1 ; i >= 0;i--,*npPos += 1)
			{
				if((*mpParentsKey->Branch->mpSubKeys)[i].HashValue != luID)
					break;
			}
		}
	}

	mpConfigFile->moExclusive.Leave();

	return luID;
}


// ��ñ���������
int CCfKeyInterface::GetName(wchar_t* npNameBuff,int niBufLenByWchar)
{

	wchar_t* lszName;
	int liReval = -1;

	mpConfigFile->moExclusive.Enter();

	do 
	{
		if(mbDeleted != false)
			break;

		if(npNameBuff == NULL)
		{
			liReval = Branch->NameLength;
			break;
		}

		if(niBufLenByWchar < Branch->NameLength+1)
		{
			liReval = -1;
			break;
		}

		if(Branch->NameLength == 0)
		{
			liReval = 0;
			break;
		}

		if(CFKEY_HAS_CHILD(BaseNode))
			lszName = Branch->Name;
		else
			lszName = Leaf->Name;

		COPYSTR_S(npNameBuff,niBufLenByWchar,lszName);

		liReval = Branch->NameLength;

	} while(false);

	mpConfigFile->moExclusive.Leave();

	return liReval;
}


// ��������������ᵼ��ͬһ�ڵ��³���ͬ���ڵ㣬��ʧ��
bool __stdcall CCfKeyInterface::Rename(
	IN const wchar_t* nszName,
	IN bool FailIfExist	//	FailIfExist==true��������ᵼ��ͬһ�ڵ��³���ͬ���ڵ㣬��ʧ��;  ==false ���Զ����Ӹ����ַ�
	)
{
	wchar_t lswName[MAX_PATH];
	PCFKEY_NODE lpNewNode;
	bool lbReval = false;
	int i;

	if(nszName == NULL || nszName[0] == UNICODE_NULL)
		return false;	// ���ֲ���Ϊ��

	mpConfigFile->moExclusive.Enter();
	do 
	{
		if(mbDeleted != false|| mpParentsKey == NULL)
			break;

		// �����������޸�����
		if((BaseNode->Flag&CDKEY_FLAG_LOCKED)!=0)
			break;

		//if(BaseNode->NameLength == 0)	// û�����ֵļ�ֵ�����趨����		modified by AX 2011.10.13
		//	break;

		// ȷ������������Ƿ���ֳ�ͻ
		if(mpParentsKey->FindSubKey(nszName,-1) >= 0)
		{
			if(FailIfExist != false)
				break;

			for (i=0;i< 1000;i++)
			{
				wsprintf(lswName,L"%s%d",nszName,i);
				if(mpParentsKey->FindSubKey(lswName,-1) < 0)
				{
					nszName = lswName;
					break;
				}
			}
			if(i >= 1000)
				break;
		}

		// to allocate a new node
		if(CFKEY_HAS_CHILD(BaseNode)!=false)
		{
			lpNewNode = mpConfigFile->AllocateNode(nszName,-1,CFKEY_VALUE_TYPE(BaseNode->Flag),&Branch->Name[Branch->NameLength]+1,BaseNode->ValueLength,Branch->mpSubKeys->Size());
			if(lpNewNode != NULL)
			{
				// ���ӽڵ��¼�����ƹ���
				for (int i=0;i<Branch->mpSubKeys->Size();i++)
				{
					((PCFKEY_BRANCH)lpNewNode)->mpSubKeys->Push_Back((*Branch->mpSubKeys)[i]);
				}
			}
		}
		else
		{
			lpNewNode = mpConfigFile->AllocateNode(nszName,-1,CFKEY_VALUE_TYPE(BaseNode->Flag),&Leaf->Name[Leaf->NameLength]+1,BaseNode->ValueLength);
		}
		if(lpNewNode == NULL)
			break;

		// �滻�Լ��ڸ��ڵ��е�ָ���¼
		mpParentsKey->RemoveSubNode(miPositionInBrothers);
		mpParentsKey->InsertSubNode(lpNewNode);

		mpConfigFile->UpdateInterface(this,lpNewNode);

		// ����ͬ�������д򿪵ļ�
		mpConfigFile->UpdateSubNodes(mpParentsKey);

		lbReval = true;

	} while(false);

	mpConfigFile->moExclusive.Leave();

	
	return lbReval;
}

// ���û�ı䵱ǰ��Ĭ��ֵ
bool CCfKeyInterface::SetValue(
	IN IConfigFile::VALUETYPE nuValueType,	// �Ӽ���Ĭ��ֵ����
	IN const void* npValueBuf,
	IN LONG  niValuelen
	)
{
	PCFKEY_NODE lpNewNode;
	bool lbReval = false;

	if(niValuelen == -1)
	{
		niValuelen = CConfigFile::GetValueLengthOfTypeString(nuValueType,npValueBuf);
	}

	mpConfigFile->moExclusive.Enter();
	do 
	{
		if(mbDeleted != false || mpParentsKey == NULL || mpParentsKey->mbDeleted != false)
			break;

		// �����������޸�����
		if((BaseNode->Flag&CDKEY_FLAG_LOCKED)!=0)
			break;

		if(niValuelen <= BaseNode->ValueLength)
		{
			if(CFKEY_HAS_CHILD(BaseNode)!=false)
			{
				RtlCopyMemory(&Branch->Name[Branch->NameLength]+1,npValueBuf,niValuelen);
				Branch->ValueLength = (USHORT)niValuelen;
			}
			else
			{
				RtlCopyMemory(&Leaf->Name[Leaf->NameLength]+1,npValueBuf,niValuelen);
				Leaf->ValueLength = (USHORT)niValuelen;
			}
			CFKEY_SET_VALUETYPE(BaseNode->Flag,nuValueType);
			lbReval = true;
			break;
		}

		// to allocate a new node
		if(CFKEY_HAS_CHILD(BaseNode)!=false)
		{
			lpNewNode = mpConfigFile->AllocateNode(Branch->Name,Branch->NameLength,nuValueType,npValueBuf,niValuelen,Branch->mpSubKeys->Size());
			if(lpNewNode != NULL)
			{
				// ���ӽڵ��¼�����ƹ���
				for (int i=0;i<Branch->mpSubKeys->Size();i++)
				{
					((PCFKEY_BRANCH)lpNewNode)->mpSubKeys->Push_Back((*Branch->mpSubKeys)[i]);
				}
			}
		}
		else
		{
			lpNewNode = mpConfigFile->AllocateNode(Leaf->Name,Leaf->NameLength,nuValueType,npValueBuf,niValuelen);
		}
		if(lpNewNode == NULL)
			break;

		// �滻�Լ��ڸ��ڵ��е�ָ���¼
		mpParentsKey->UpdateSubNode(lpNewNode,miPositionInBrothers);

		mpConfigFile->UpdateInterface(this,lpNewNode);

		lbReval = true;

	} while(false);

	mpConfigFile->moExclusive.Leave();


	return lbReval;
}

// �޸���չ��־
bool CCfKeyInterface::SetExtFlag(
	IN UCHAR nchFlag
	)
{

	mpConfigFile->moExclusive.Enter();

	if(mbDeleted == false)
	{
		CFKEY_SET_EXTFLAG(BaseNode->Flag,nchFlag);
	}

	mpConfigFile->moExclusive.Leave();

	return true;
}

// ��õ�ǰĬ��ֵ�����ػ�õ�ֵ���ֽ���
int CCfKeyInterface::GetValue(
	OUT PVOID npValueBuf,
	IN  LONG  niBufLen
	)
{
	int liSize = -1;

	mpConfigFile->moExclusive.Enter();

	if(mbDeleted ==false && npValueBuf != NULL)
	{
		if(niBufLen >= BaseNode->ValueLength)
		{
			if(CFKEY_HAS_CHILD(BaseNode)!=false)
			{
				RtlCopyMemory(npValueBuf,&Branch->Name[Branch->NameLength]+1,Branch->ValueLength);
			}
			else
			{
				RtlCopyMemory(npValueBuf,&Leaf->Name[Leaf->NameLength]+1,Leaf->ValueLength);
			}
			liSize = BaseNode->ValueLength;
		}
		else
		{
			liSize = -1;	// overflow
		}
	}

	mpConfigFile->moExclusive.Leave();

	return liSize;
}

CCfKeyInterface::~CCfKeyInterface()
{
	CMMASSERT(miReferenceCount<=0);
}

// �����ӽڵ㣬�����ӽڵ���Hash�����е���ţ�����-1��ʾû���ҵ�
int CCfKeyInterface::FindSubKey(
	IN const wchar_t* nszName,
	IN int niNameLen,	// -1 indicate nszName is terminated by '\0' or '\\' or '/', >=0 is the charactar count of nszName
	OUT int* npNameLen	// �������ֵ���Ч����
	)
{
	CCfKeyHash loFind;
	
	niNameLen = loFind.GenerateHashCode(nszName,niNameLen);


	int liPos = Branch->mpSubKeys->Find(loFind);
	if(liPos >= 0)
	{
		for(;liPos < Branch->mpSubKeys->Size() && (*Branch->mpSubKeys)[liPos].HashValue == loFind.HashValue;liPos++)
		{
			if(niNameLen != (*Branch->mpSubKeys)[liPos].KeyObj->NameLength)
				continue;
			wchar_t* lpNodeName;
			if(CFKEY_HAS_CHILD((*Branch->mpSubKeys)[liPos].KeyObj))
				lpNodeName = ((PCFKEY_BRANCH)((*Branch->mpSubKeys)[liPos].KeyObj))->Name;
			else
				lpNodeName = ((PCFKEY_LEAF)((*Branch->mpSubKeys)[liPos].KeyObj))->Name;

			int i;
			for (i=0;i<niNameLen;i++)
			{
				if(nszName[i] != lpNodeName[i] && ((nszName[i]|0x20) != (lpNodeName[i]|0x20) || (lpNodeName[i]|0x20) < L'a' || (lpNodeName[i]|0x20) > L'z'))
					break;
			}
			if(i >= niNameLen)
				break;
		}
		if(liPos >= Branch->mpSubKeys->Size() || (*Branch->mpSubKeys)[liPos].HashValue != loFind.HashValue)
			liPos = -1;
	}

	if(npNameLen != NULL)
		*npNameLen = niNameLen;

	return liPos;
}

// �����ӽڵ㣬�����ӽڵ���Hash�����е���ţ�����-1��ʾû���ҵ�
int CCfKeyInterface::FindSubKey(
	ULONG nuHashCode,
	IN int niPos	// ����ͬ��ID�ļ�ֵ�е��Ⱥ�λ�ã�< 0��ʾȡ���һ��
	)
{
	CCfKeyHash loFind;
	int liEnd;

	loFind.HashValue = nuHashCode;

	int liPos = Branch->mpSubKeys->Find(loFind);
	if(liPos >= 0 && niPos != 0)	// niPos == 0 ��ʾ��һ��������
	{
		if(niPos < 0)
		{
			// �����һ��
			liEnd = Branch->mpSubKeys->Size();
		}
		else
		{
			liEnd = liPos+niPos+1;
		}

		while(liPos < Branch->mpSubKeys->Size() && liPos < liEnd && (*Branch->mpSubKeys)[liPos].HashValue == loFind.HashValue)
			liPos++;

		liPos--;	// �˻ص��ղ���Ч��λ��
	}

	return liPos;
}



// ����һ���µ��ӽڵ㣬���ز����λ��
int CCfKeyInterface::InsertSubNode(
	PCFKEY_NODE npNode,
	ULONG nuHashCode,
	bool nbAhead
	)
{
	if(mbDeleted != false)
		return 0;
	// ��鵱ǰ�ڵ������������Branch���ͣ����Ǿ���Ҫ���¹���Node�ڵ�
	if(CFKEY_HAS_CHILD(BaseNode)==false)
	{
		PCFKEY_NODE lpNewNode;
		lpNewNode = mpConfigFile->AllocateNode(Leaf->Name,Leaf->NameLength,CFKEY_VALUE_TYPE(BaseNode->Flag),&Leaf->Name[Leaf->NameLength]+1,BaseNode->ValueLength,1);
		if(lpNewNode == NULL)
			return -1;

		// �滻�Լ��ڸ��ڵ��е�ָ���¼
		if(mpParentsKey != NULL)
			(*mpParentsKey->Branch->mpSubKeys)[miPositionInBrothers].KeyObj = lpNewNode;

		mpConfigFile->UpdateInterface(this,lpNewNode);
	}

	CCfKeyHash loNew;
	if(nuHashCode == 0 )
	{
		// �жϲ���Ķ�������
		if(CFKEY_HAS_CHILD(npNode)!=false)
			loNew.GenerateHashCode(((PCFKEY_BRANCH)npNode)->Name,((PCFKEY_BRANCH)npNode)->NameLength);
		else
			loNew.GenerateHashCode(((PCFKEY_LEAF)npNode)->Name,((PCFKEY_LEAF)npNode)->NameLength);
	}
	else
		loNew.HashValue = nuHashCode;

	loNew.KeyObj = (PCFKEY_NODE)npNode;

	return Branch->mpSubKeys->Insert(loNew,nbAhead);
}


// ����ӽڵ�
PCFKEY_NODE CCfKeyInterface::GetSubNode(
	IN int niPosition
	)
{
	return (*Branch->mpSubKeys)[niPosition].KeyObj;
}

// �Ƴ��ӽڵ�
bool CCfKeyInterface::RemoveSubNode(
	IN int niPosition
	)
{
	return Branch->mpSubKeys->RemoveByIndex(niPosition);
}


// ���ַ�������Hash��,���ص������ֵ���Ч����
int CCfKeyHash::GenerateHashCode(
	IN const wchar_t* nszName,
	IN int niNameLen	// -1 indicate that the name was terminated by '\0' or '\\' or '/'
	)
{
	LONG liIndex = 0;
	HashValue = 0;

	if(niNameLen < 0)
	{
		niNameLen = 255;
	}

	wchar_t lcCrt;
	while (nszName[liIndex] != 0 && nszName[liIndex] != L'\\' && nszName[liIndex] != L'/' && liIndex < niNameLen)
	{
		lcCrt = nszName[liIndex++];
		if((lcCrt|0x20) >= L'a' && (lcCrt|0x20) <= L'z')
			lcCrt |= 0x20;
		HashValue += lcCrt;
		HashValue = (HashValue<<1) | ((HashValue&0x010000)>>16);
	}

	return liIndex;
}

// �����ӽڵ㣬�ⲻ��ı��������
bool CCfKeyInterface::UpdateSubNode(
	PCFKEY_NODE npNode,
	IN int niPosition
	)
{
	(*Branch->mpSubKeys)[niPosition].KeyObj = npNode;
	return true;
}



ULONG CStableConfigFile::InitOnCreate(
	IN const wchar_t* nszPathName,				// �ļ�������·����������ΪNULL����ʾ����һ�����ļ���ʱ��ָ���ļ���
	IN ULONG nuCreationDisposition			// ͬCreateFile API���ƣ���CfgIface.h�ļ��е���ض���
	)
{
	ULONG luResult = (ULONG)-1;
	ULONG luDuplicateNumberArr[2]={0};
	wchar_t* lpFileNameArr[2];
	int i;
	bool lbReval = false;
#ifdef KERNEL_CODE
	OBJECT_ATTRIBUTES ObjectAttributes;
	UNICODE_STRING FileName;
	IO_STATUS_BLOCK IoStatus;
	NTSTATUS Status = STATUS_UNSUCCESSFUL;
#endif
	PCF_FILE_HEAD lpHead = NULL;
	HANDLE lhFile = NULL;
	ULONG luNewDupNumber;

	if(nszPathName == NULL)
		return CConfigFile::InitOnCreate(NULL,nuCreationDisposition);

	do 
	{
		// ׼���ļ���
		for (i=0;i<2;i++)
		{
			lpFileNameArr[i] = new wchar_t[256];
			if(lpFileNameArr[i]==NULL)
				break;
			COPYSTR_S(lpFileNameArr[i],256,nszPathName);
		}
		if(lpFileNameArr[0] == NULL || lpFileNameArr[1] == NULL)
			break;

		// �޸ĵڶ��ļ������ں�׺��ǰ�����'_dup'
		{
			wchar_t* lpExtName = NULL;
			for(i=0;i<256;i++)
			{
				if(lpFileNameArr[1][i] == L'.')
					lpExtName = lpFileNameArr[1]+i;
				if(lpFileNameArr[1][i] == UNICODE_NULL)
				{
					if(lpExtName == NULL)
						lpExtName = lpFileNameArr[1]+i;
					break;
				}
			}
			// �Ƿ��ܹ�������������
			if(lpExtName - lpFileNameArr[1] >= 256 - 8)
				break;
			*(lpExtName+4) = *lpExtName;
			*(lpExtName+5) = *(lpExtName+1);
			*(lpExtName+6) = *(lpExtName+2);
			*(lpExtName+7) = *(lpExtName+3);
			*(lpExtName+8) = UNICODE_NULL;
			*(lpExtName) = L'_';
			*(lpExtName+1) = L'd';
			*(lpExtName+2) = L'u';
			*(lpExtName+3) = L'p';
		}

		lpHead = new CF_FILE_HEAD;
		if(lpHead == NULL)
			break;

		for(int i=0;i<2;i++)
		{
			// ���ļ��������ļ�ͷ
			do 
			{
	#ifndef KERNEL_CODE
				lhFile = ::CreateFileW(lpFileNameArr[i],GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,nuCreationDisposition,NULL,NULL);
				if(lhFile == INVALID_HANDLE_VALUE)
				{
					lhFile = NULL;
					break;
				}
	#else
				RtlInitUnicodeString(&FileName,lpFileNameArr[i]);
				InitializeObjectAttributes(
					&ObjectAttributes
					,&FileName
					,OBJ_CASE_INSENSITIVE|OBJ_KERNEL_HANDLE
					,NULL 
					,NULL );

				Status = ZwCreateFile(&lhFile,FILE_READ_DATA|SYNCHRONIZE,&ObjectAttributes,&IoStatus,NULL,FILE_ATTRIBUTE_NORMAL,FILE_SHARE_READ|FILE_SHARE_WRITE,nuCreationDisposition,FILE_SYNCHRONOUS_IO_NONALERT,NULL,0);
				if(!NT_SUCCESS(Status))
				{
					lhFile = NULL;
					break;
				}
	#endif

				// �����ļ�ͷ
	#ifndef KERNEL_CODE
				ULONG luRead;
				if(ReadFile(lhFile,lpHead,sizeof(CF_FILE_HEAD),&luRead,NULL)==FALSE)
					break;
	#else
				if(STATUS_SUCCESS != ZwReadFile(lhFile,NULL,NULL,NULL,&IoStatus,lpHead,sizeof(CF_FILE_HEAD),NULL,NULL))
					break;
	#endif

				// ���������
				if(lpHead->ShortHead.Signature != CF_SIGNATURE || lpHead->ShortHead.Version > CF_VERSION || lpHead->ShortHead.SequenceA != lpHead->Tail.SequenceB)
					break;

				// ������¼
				luDuplicateNumberArr[i] = lpHead->ShortHead.Duplicate;

			} while (false);

			if(lhFile != NULL)
			{
#ifndef KERNEL_CODE
				CloseHandle(lhFile);
#else
				ZwClose(lhFile);
#endif
			}
		}

		// �ж��ĸ��ļ�����󱣴�ģ����ļ����Ĵ��˳�����
		if( luDuplicateNumberArr[0] == (ULONG)-1 && luDuplicateNumberArr[1]== 1 ||
			luDuplicateNumberArr[0] < luDuplicateNumberArr[1] && (luDuplicateNumberArr[0]!=1 || luDuplicateNumberArr[1]!=(ULONG)-1)
			) 
		{
			wchar_t* lpTemp = lpFileNameArr[0];
			lpFileNameArr[0] = lpFileNameArr[1];
			lpFileNameArr[1] = lpTemp;
			luNewDupNumber = luDuplicateNumberArr[1]+1;
		}
		else
			luNewDupNumber = luDuplicateNumberArr[0]+1;

		if(luNewDupNumber == 0)
			luNewDupNumber = 1;

		// ���Դ���󱣴���ļ�
		luResult = CConfigFile::InitOnCreate(lpFileNameArr[0],nuCreationDisposition);
		if(luResult == 0)
		{
			// �޸Ļ�����ļ���Ϊ����һ���ļ����Ӷ�ʹ�ñ���ʱ����¼��������һ���ļ�
			COPYSTR_S(mszFileName,256,lpFileNameArr[1]);
		}
		else// ʧ�ܣ����Դ�����һ���ļ�
		{
			luResult = CConfigFile::InitOnCreate(lpFileNameArr[1],nuCreationDisposition);
			if(luResult == 0)
			{
				// �޸Ļ�����ļ���Ϊ����һ���ļ����Ӷ�ʹ�ñ���ʱ����¼��������һ���ļ�
				COPYSTR_S(mszFileName,256,lpFileNameArr[0]);
			}
		}
		mdFileHead.Duplicate = luNewDupNumber;

	} while (false);

	if(lpHead != NULL)
		delete lpHead;

	if(lpFileNameArr[0]!=NULL)
		delete lpFileNameArr[0];
	if(lpFileNameArr[1]!=NULL)
		delete lpFileNameArr[1];

	return luResult;
}

// ֱ�ӽ�ֵ����ΪLONG��ȡ�����ֵ�����Ͳ���4�ֽڣ�����������Ĭ��ֵ
LONG __stdcall CCfKeyInterface::QueryValueAsLONG(
	IN LONG niDefault
	)
{
	LONG liValue;

	mpConfigFile->moExclusive.Enter();

	if(mbDeleted == false && BaseNode->ValueLength == sizeof(LONG))
	{
		if(CFKEY_HAS_CHILD(BaseNode)!=false)
			liValue = *(LONG*)(&Branch->Name + Branch->NameLength + 1);
		else
			liValue = *(LONG*)(&Leaf->Name + Leaf->NameLength + 1);
	}
	else
		liValue = niDefault;

	mpConfigFile->moExclusive.Leave();

	return liValue;
}

// ֱ�ӽ�ֵ����ΪFLOAT��ȡ�����ֵ�����Ͳ���sizeof(FLOAT)�ֽڣ�����������Ĭ��ֵ
FLOAT __stdcall CCfKeyInterface::QueryValueAsFLOAT(
	IN FLOAT nfDefault
	)
{
	FLOAT lfValue;

	mpConfigFile->moExclusive.Enter();

	if(mbDeleted == false && BaseNode->ValueLength == sizeof(FLOAT))
	{
		if(CFKEY_HAS_CHILD(BaseNode)!=false)
			lfValue = *(FLOAT*)(&Branch->Name + Branch->NameLength + 1);
		else
			lfValue = *(FLOAT*)(&Leaf->Name + Leaf->NameLength + 1);
	}
	else
		lfValue = nfDefault;

	mpConfigFile->moExclusive.Leave();

	return lfValue;
}

// �����ֲ����Ӽ���ֱ�ӽ��Ӽ���ֵ����ΪLONG��ȡ�����ֵ�����Ͳ���4�ֽڣ�����������Ĭ��ֵ
LONG __stdcall CCfKeyInterface::QuerySubKeyValueAsLONG(
	IN const wchar_t* nszSubKeyName,	// ָ��Ҫ��ȡ��ֵ�ļ���������ΪNULL����nszSubKeyName[0]==UNICODE_NULL
	IN LONG niDefault
	)
{
	LONG liValue;
	ICfKey* lpKey = OpenKey(nszSubKeyName);

	if(lpKey != NULL)
	{
		liValue = lpKey->QueryValueAsLONG(niDefault);
		lpKey->Release();
	}
	else
		liValue = niDefault;

	return liValue;
}

// ��ID�����Ӽ���ֱ�ӽ��Ӽ���ֵ����ΪLONG��ȡ�����ֵ�����Ͳ���4�ֽڣ�����������Ĭ��ֵ
LONG __stdcall CCfKeyInterface::QuerySubKeyValueAsLONG(
	IN ULONG nuID,		// �Ӽ��ı�ʶID
	IN LONG niDefault
	)
{
	LONG liValue;
	ICfKey* lpKey = GetSubKey(nuID);

	if(lpKey != NULL)
	{
		liValue = lpKey->QueryValueAsLONG(niDefault);
		lpKey->Release();
	}
	else
		liValue = niDefault;

	return liValue;
}

// �����ֲ����Ӽ���ֱ�ӽ��Ӽ���ֵ����ΪFLOAT��ȡ�����ֵ�����Ͳ���sizeof(FLOAT)�ֽڣ�����������Ĭ��ֵ
FLOAT __stdcall CCfKeyInterface::QuerySubKeyValueAsFLOAT(
	IN const wchar_t* nszSubKeyName,	// ָ��Ҫ��ȡ��ֵ�ļ���������ΪNULL����nszSubKeyName[0]==UNICODE_NULL
	IN FLOAT nfDefault
	)
{
	FLOAT lfValue;
	ICfKey* lpKey = OpenKey(nszSubKeyName);

	if(lpKey != NULL)
	{
		lfValue = lpKey->QueryValueAsFLOAT(nfDefault);
		lpKey->Release();
	}
	else
		lfValue = nfDefault;

	return lfValue;
}

// ��ID�����Ӽ���ֱ�ӽ��Ӽ���ֵ����ΪFLOAT��ȡ�����ֵ�����Ͳ���sizeof(FLOAT)�ֽڣ�����������Ĭ��ֵ
FLOAT __stdcall CCfKeyInterface::QuerySubKeyValueAsFLOAT(
	IN ULONG nuID,		// �Ӽ��ı�ʶID
	IN FLOAT nfDefault
	)
{
	FLOAT lfValue;
	ICfKey* lpKey = GetSubKey(nuID);

	if(lpKey != NULL)
	{
		lfValue = lpKey->QueryValueAsFLOAT(nfDefault);
		lpKey->Release();
	}
	else
		lfValue = nfDefault;

	return lfValue;
}

// �����ֲ����Ӽ���ֱ�ӻ�ȡ�Ӽ���ֵ������С�����ʾĿ��������ڣ�����0��ʾĿ���ֵΪ�գ�����0��ʾ���ص�Value����
int __stdcall CCfKeyInterface::QuerySubKeyValue(
	IN const wchar_t* nszSubKeyName,	// ָ��Ҫ��ȡ��ֵ�ļ���������ΪNULL����nszSubKeyName[0]==UNICODE_NULL
	OUT PVOID npValueBuf,
	IN  LONG  niBufLen
	)
{
	LONG liSize;
	ICfKey* lpKey = OpenKey(nszSubKeyName);

	if(lpKey != NULL)
	{
		liSize = lpKey->GetValue(npValueBuf,niBufLen);

		lpKey->Release();
	}
	else
		liSize = -1;

	return liSize;
}


// ֱ�ӽ�ֵ����ΪPVOID��ȡ�����ֵ�����Ͳ���sizeof(PVOID)�ֽڣ�����������Ĭ��ֵ
PVOID __stdcall CCfKeyInterface::QueryValueAsPVOID(
	IN PVOID npDefault
	)
{
	PVOID lpValue;

	mpConfigFile->moExclusive.Enter();

	if(mbDeleted == false && BaseNode->ValueLength == sizeof(PVOID))
	{
		if(CFKEY_HAS_CHILD(BaseNode)!=false)
			lpValue = *(PVOID*)(&Branch->Name + Branch->NameLength + 1);
		else
			lpValue = *(PVOID*)(&Leaf->Name + Leaf->NameLength + 1);
	}
	else
		lpValue = npDefault;

	mpConfigFile->moExclusive.Leave();

	return lpValue;
}

// �����ֲ����Ӽ���ֱ�ӽ��Ӽ���ֵ����ΪPVOID��ȡ�����ֵ�����Ͳ���sizeof(PVOID)�ֽڣ�����������Ĭ��ֵ
PVOID __stdcall CCfKeyInterface::QuerySubKeyValueAsPVOID(
	IN const wchar_t* nszSubKeyName,	// ָ��Ҫ��ȡ��ֵ�ļ���������ΪNULL����nszSubKeyName[0]==UNICODE_NULL
	IN PVOID npDefault
	)
{
	PVOID lpValue;
	ICfKey* lpKey = OpenKey(nszSubKeyName);

	if(lpKey != NULL)
	{
		lpValue = lpKey->QueryValueAsPVOID(npDefault);
		lpKey->Release();
	}
	else
		lpValue = npDefault;

	return lpValue;
}

// ��ID�����Ӽ���ֱ�ӽ��Ӽ���ֵ����ΪPVOID��ȡ�����ֵ�����Ͳ���sizeof(PVOID)�ֽڣ�����������Ĭ��ֵ
PVOID __stdcall CCfKeyInterface::QuerySubKeyValueAsPVOID(
	IN ULONG nuID,		// �Ӽ��ı�ʶID
	IN PVOID npDefault
	)
{
	PVOID lpValue;
	ICfKey* lpKey = GetSubKey(nuID);

	if(lpKey != NULL)
	{
		lpValue = lpKey->QueryValueAsPVOID(npDefault);
		lpKey->Release();
	}
	else
		lpValue = npDefault;

	return lpValue;
}

//// ��ȡָ����ֵ�洢New�������ڴ��з�������ʧ�ܷ���NULL��ע��Ҫ�ͷŷ��ص�ָ��, ����Colin �ӡ�
//PVOID __stdcall CCfKeyInterface::QueryValueAsBuffer()
//{
//	PVOID lpRetValue = NULL;
//	mpConfigFile->moExclusive.Enter();
//	do 
//	{
//		if(this->GetValueType() == IConfigFile::Invalid)
//			break;
//
//		int liValueLength = this->GetValueLength();
//		lpRetValue = new BYTE[liValueLength];
//		memset(lpRetValue, 0, liValueLength);
//
//		if(this->GetValue(lpRetValue, liValueLength) <= 0)
//			break;
//
//	} while (false);
//
//	mpConfigFile->moExclusive.Leave();
//	return lpRetValue;
//}

//// ��ȡָ�����ӽ���ֵ�洢��New�������ڴ��з�������ʧ�ܷ���NULL��ע��Ҫ�ͷŷ��ص�ָ��, ����Colin �ӡ�
//PVOID __stdcall CCfKeyInterface::QuerySubKeyValueAsBuffer(
//	IN const wchar_t* nswSubKeyName
//	)
//{
//	PVOID lpRetValue = NULL;
//	mpConfigFile->moExclusive.Enter();
//	
//	ICfKey* lpSubKey = this->OpenKey(nswSubKeyName);
//	do 
//	{
//		if(NULL == lpSubKey || mbDeleted != false)
//			break;
//
//		lpRetValue = lpSubKey->QueryValueAsBuffer();
//
//	} while (false);
//	CMM_SAFE_RELEASE(lpSubKey);
//
//	mpConfigFile->moExclusive.Leave();
//	return lpRetValue;
//}
//
//// ��ȡָ�����ӽ���ֵ�洢��New�������ڴ��з�������ʧ�ܷ���NULL��ע��Ҫ�ͷŷ��ص�ָ��, ����Colin �ӡ�
//PVOID __stdcall CCfKeyInterface::QuerySubKeyValueAsBuffer(
//	IN ULONG nuID									// �Ӽ��ı�ʶID
//	)
//{
//	PVOID lpRetValue = NULL;
//	mpConfigFile->moExclusive.Enter();
//
//	ICfKey* lpSubKey = this->GetSubKey(nuID);
//	do 
//	{
//		if(NULL == lpSubKey || mbDeleted != false)
//			break;
//
//		lpRetValue = lpSubKey->QueryValueAsBuffer();
//
//	} while (false);
//	CMM_SAFE_RELEASE(lpSubKey);
//
//	mpConfigFile->moExclusive.Leave();
//	return lpRetValue;
//} 