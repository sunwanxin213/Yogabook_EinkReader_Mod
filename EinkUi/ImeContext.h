/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#pragma once







DECLARE_BUILTIN_NAME(CXsImeContext)
class CXsImeContext:public cmmBaseObject<CXsImeContext,IBaseObject,GET_BUILTIN_NAME(CXsImeContext)>
{
	friend class cmmBaseObject<CXsImeContext,IBaseObject,GET_BUILTIN_NAME(CXsImeContext)>;
public:
	LRESULT OnImeMessage(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

	void EnableIme(void);

	void DisableIme(void);

	void SetImeCompositionWnd(D2D1_POINT_2F ndPosition);

	ERESULT InitOnCreate();

	void OnWsgStartComposition(void);
protected:
	HIMC mhPreviousImc;
	HIMC mhImcToHaltIme;
	bool mbAssociated;
	COMPOSITIONFORM mdComInfo;

	CXsImeContext();
	~CXsImeContext();

	// ����IME��Composition Windows��λ��
	ERESULT __stdcall SetImeCompositionWindowCallback(ULONG nuFlag,LPVOID npContext);

	// ����IME��Composition Windows��λ��
	ERESULT __stdcall ChangeImeContextCallback(ULONG nuFlag,LPVOID npContext);

};







