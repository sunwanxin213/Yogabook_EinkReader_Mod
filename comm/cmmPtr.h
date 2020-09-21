/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */

#ifndef _CMM_PTR_
#define _CMM_PTR_

template<class CCmmContent>
class cmmDeletePtr {
public:
	virtual ~cmmDeletePtr() {
		freePtr();
	}

	cmmDeletePtr() {
		mPtr = NULL;
	}

	// ��ָ�����͵�ָ�빹��
	cmmDeletePtr(CCmmContent* src) {
		mPtr = src;
	}

	// ������ָ�빹�죬����ת��
	cmmDeletePtr(cmmDeletePtr<CCmmContent>& src) {
		move(src);
	}

	// new ����һ�����
	bool allocate(int size) {
		freePtr();

		mPtr = new CCmmContent[size];

		return mPtr != NULL;
	}

	// �������ƺ������Զ��ͷ�ǰһ�α����ָ��
	void operator = (CCmmContent* src) {
		freePtr();

		mPtr = src;
	}

	// �����͸��ƣ���������ĳ��ϣ�����ͬ���͵�ָ��ǿ�Ʒ���
	void set_by_void(void* pt) {
		freePtr();

		mPtr = (CCmmContent*)src;
	}

	// ���ƺ������Զ��ͷ�ǰһ�α����ָ�룬��ȡ�¶�������Ȩ
	void operator ()(cmmDeletePtr<CCmmContent>& src) {
		take(src);
	}

	// ���ƺ������Զ��ͷ�ǰһ�α����ָ�룬��ȡ�¶�������Ȩ��ע��src�Ķ����������ȷ�Ļ�÷�����õģ�����Ӧ delete/release/HeapFree���ͷ�Ҫ��
	void take(CCmmContent* src) {
		freePtr();

		mPtr = src;
	}

	// ���ƺ������Զ��ͷ�ǰһ�α����ָ�룬��ȡ�¶�������Ȩ
	void take(cmmDeletePtr<CCmmContent>& src) {
		freePtr();

		mPtr = src.mPtr;
		src.mPtr = NULL;
	}

	// ��ȡ������ָ��, xxx()
	CCmmContent* operator ()(){
		return mPtr;
	}

	// ��ȡ�����ָ�룬xxx.ptr()
	CCmmContent* ptr() {
		return mPtr;
	}

	// ��ȡֻ�������ݻ�����
	CCmmContent* conPtr() const{
		return mPtr;
	}

	// ��ȡ�����ָ�룬xxx(false)����������(��ת������Ȩ�����󽫻����ű��������ɾ������xxx(true)��������ת�ƶ�������Ȩ�������汾����ɾ����
	CCmmContent* operator ()(bool move) {
		if (move != false)
			return notfree();
		return mPtr;
	}

	// ��ȡ�����ָ�룬xxx.notfree��������
	CCmmContent* notfree() {
		auto rev = mPtr;
		mPtr = NULL;
		return rev;
	}

	// ���ö����ͷ�֮ǰ�����ָ��
	void reset() {
		freePtr();
	}

	// �ж��Ƿ�ָ��ͬһ��Ŀ��
	bool operator ==(const CCmmContent* src) {
		return mPtr == src;
	}

	// �ж��Ƿ�ָ��ͬһ��Ŀ��
	bool operator !=(const CCmmContent* src) {
		return mPtr != src;
	}

	// �ж��Ƿ�ָ��ͬһ��Ŀ��
	bool  operator == (const cmmDeletePtr<CCmmContent>& src) {
		return mPtr == src.mPtr;
	}

	// ��ȡ�����ָ��ָ��Ķ���
	CCmmContent& operator *() {
		return *mPtr;
	}

	// ��ȡ�����ָ��ָ��Ķ���
	CCmmContent& object() {
		return *mPtr;
	}

	// ��ȡָ�뱾��ĵ�ַ�����ڽ����ⲿ�ĸ�ֵ
	CCmmContent**  addressOfPtr() {
		return &mPtr;
	}

	// ֱ�ӵ���ָ��ĳ�Ա����
	CCmmContent* operator ->() {
		return mPtr;
	}


protected:
	CCmmContent* mPtr;

	virtual void freePtr() {
		if (mPtr != NULL)
		{
			delete[] mPtr;
			mPtr = NULL;
		}
	}
};

//////////////////////////////////////////////////////////////////////////
// �ͷ�ʱ���Զ�����xxx->Release();
template<class CCmmContent>
class cmmReleasePtr :public cmmDeletePtr<CCmmContent> {
public:
	cmmReleasePtr() : cmmDeletePtr<CCmmContent>() {}
	virtual ~cmmReleasePtr() {	// �����Լ�ʵ�֣�����������������ܵ����麯�����޷�ͨ���麯��freePtr�ͷ�
		freePtr();
	}
	cmmReleasePtr(CCmmContent* src) : cmmDeletePtr<CCmmContent>(src) {	}

	virtual void freePtr() {
		if (mPtr != NULL)
		{
			mPtr->Release();
			mPtr = NULL;
		}
	}

	void operator = (CCmmContent* src) {
		freePtr();
		mPtr = src;
	}
};

//////////////////////////////////////////////////////////////////////////
// �ͷ�ʱ���Զ����ö��ͷ� heapfree(xxx);
template<class CCmmContent>
class cmmHeapPtr :public cmmDeletePtr<CCmmContent> {
public:
	cmmHeapPtr() : cmmDeletePtr<CCmmContent>() {}
	virtual ~cmmHeapPtr() {
		freePtr();
	}
	virtual void freePtr() {
		if (mPtr != NULL)
		{
			HeapFree(GetProcessHeap(), 0, mPtr);
			mPtr = NULL;
		}
	}
	void operator = (CCmmContent* src) {
		freePtr();
		mPtr = src;
	}

	// �Ӷ�������һ���ڴ�
	bool allocate(int size) {
		freePtr();
		mPtr = (CCmmContent*)HeapAlloc(GetProcessHeap(), 0, size);
		return mPtr != NULL;
	}
};


//cmmDeletePtr<classX> xx;
//xx = new classX;
//xx->XFun();
//
//return xx(true);	// return classX*��and discard xx;
//
//return xx();		// return classX*, and keep going of xx
//
//return *xx;		// return classX
//
//cmmDeletePtr<classX> bb;
//
//bb(xx);		// move classX* from xx to bb��and discard xx
//













#endif//_CMM_PTR_