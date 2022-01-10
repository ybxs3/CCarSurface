#pragma once
#include "string"
#include "iostream"
#include "fstream"
using namespace std;
class CMymfcTool
{
public:
	CMymfcTool(void);
	~CMymfcTool(void);
	CString GetFullPath(void);
	string CStringTostring(CString str);
	CString stringToCString(string str);
	void WriteLog(string filename,string str,bool btime=false);
	string GetstringFullpath(void);
	CString uchartohexCStr(unsigned char* data,int nlen,bool bspace=false);
	string uchartohexstr(unsigned char* data,int nlen,bool bspace=false);
	bool CheckCRC(UCHAR *data,int nlen);
	WORD GetCheckCode(UCHAR * pSendBuf, int nEnd);
	string GetCurrentstrtime(void);
	CString GetCurTimes(void);
	bool WaitTimer(int second, int msecond);//等待指定时间
	string ToUTF8(const wchar_t* buffer, int len);
	string ReadCmdValueConfigFile(const TCHAR* pTypeName, string nodeName);		// 读取config中某一节点的信息
	CString GetTime1(void);
	CString GetTime2(void);
	int GetDayOfWeek();
	int GetCurrentHour();
	void InitLogFile();
	void AddFileLog(CString str);
};
