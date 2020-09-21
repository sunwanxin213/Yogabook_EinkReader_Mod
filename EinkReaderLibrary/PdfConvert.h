#ifndef PDF_CONVERT_H_
#define PDF_CONVERT_H_

#include <string>

using std::wstring;

namespace PDFConvert
{
	enum class ConvertResult
	{
		OK,
		OfficeComponentNotInstalled,
		UnsupportedFileType,
		FileLocked,
		ComError,
		NeedPassword,
		InvalidFile,
		ConvertFailed,
		PDFFileLocked
	};

	ConvertResult GetConvertResult();   //3.ʵ�ʷ���code
	bool IsConvertingCompleted();  //2.����ͨ�����������ȡ������ture����������
	void SubmitConvertTask(const wstring& sourceFileName); //1.�ļ�·��ͨ���ýӿڴ���ȥ
	wstring GetPDFFileFullPath();  //4.����ת�����pdf�ĵ�
}


#endif