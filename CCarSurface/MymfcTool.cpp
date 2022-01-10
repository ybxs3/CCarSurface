#include "StdAfx.h"
#include "MymfcTool.h"
#include "easylogging++.h"
INITIALIZE_EASYLOGGINGPP
CMymfcTool::CMymfcTool(void)
{
}

CMymfcTool::~CMymfcTool(void)
{
}

CString CMymfcTool::GetFullPath(void)
{
	CString sPath;
	GetModuleFileName(NULL,sPath.GetBufferSetLength(MAX_PATH+1),MAX_PATH);
	sPath.ReleaseBuffer();
	int nPos;
	nPos=sPath.ReverseFind('\\');   
	sPath=sPath.Left(nPos);
	return sPath;
}

string CMymfcTool::CStringTostring(CString str)
{
	USES_CONVERSION;
	string str1=T2A(str);
	return str1;
}

CString CMymfcTool::stringToCString(string str)
{
	USES_CONVERSION;
	CString str1=A2T(str.c_str());
	return str1;
}

void CMymfcTool::WriteLog(string filename,string str,bool btime)
{
	ofstream of;
	of.open(filename.c_str(),ios_base::app|ios_base::out);
	string strcontent=str;
	if (btime)
	{
		strcontent=GetCurrentstrtime()+"\t"+str;
	}
	of<<strcontent<<_T("\n");
	of.close();
}

string CMymfcTool::GetstringFullpath(void)
{
	CString sPath;   
	GetModuleFileName(NULL,sPath.GetBufferSetLength(MAX_PATH+1),MAX_PATH);   
	sPath.ReleaseBuffer();
	int nPos;   
	nPos=sPath.ReverseFind('\\');   
	sPath=sPath.Left(nPos);   
	string str=CStringTostring(sPath);
	return str;
}

CString CMymfcTool::uchartohexCStr(unsigned char* data,int nlen,bool bspace)
{
	CString str;
	for (int i=0;i<nlen;i++)
	{
		if(bspace)
			str+=" ";
		CString str1;
		str1.Format(_T("%X"),data[i]);
		if (str1.GetLength()<2)
		{
			str1=_T("0")+str1;
		}
		str+=str1;
	}
	return str;
}
string CMymfcTool::uchartohexstr(unsigned char* data,int nlen,bool bspace)
{
	string str;
	for (int i=0;i<nlen;i++)
	{
		if (bspace)
		{
			str+=" ";
		}
		string str1,str2;
		char ss;
		ss=data[i]&0x0F;
		int num=ss;
		if (num>9)
			ss='A'+(num-10);
		else
			ss=ss+0x30;
		str2=ss;
		ss=(data[i]&0xF0)>>4;
		num=ss;
		if (num>9)
			ss='A'+(num-10);
		else
			ss=ss+0x30;
		str1=ss;
		str+=str1+str2;
	}
	return str;
}
WORD CMymfcTool::GetCheckCode(UCHAR * pSendBuf, int nEnd)//获得校验码 
{ 
	WORD wCrc = WORD(0xFFFF);
	for(int i=0; i<nEnd; i++)
	{
		wCrc ^= WORD(BYTE(pSendBuf[i]));
		for(int j=0; j<8; j++)
		{
			if(wCrc & 1)
			{
				wCrc >>= 1; 
				wCrc ^= 0xA001; 
			}
			else
			{
				wCrc >>= 1; 
			}
		}
	}
	return wCrc;
}
bool CMymfcTool::CheckCRC(UCHAR *data,int nlen)
{
	UCHAR ncrc[2];
	ncrc[1]=*(data+nlen-2);
	ncrc[0]=*(data+nlen-1);
	long snum=(ncrc[0]<<8)+ncrc[1];
	DWORD ss=GetCheckCode(data,nlen-2);
	if (ss==snum)
		return true;
	else
		return false;
}

string CMymfcTool::GetCurrentstrtime(void)
{
	struct tm ltime;
	time_t local_tv_sec;
	char ctime[16];
	time(&local_tv_sec);
	localtime_s(&ltime,&local_tv_sec);
	strftime( ctime, sizeof ctime, "%Y%m%d%H%M%S", &ltime);
	string strttime(ctime);
	return string(strttime);
}
CString CMymfcTool::GetCurTimes(void)
{
	return stringToCString(GetCurrentstrtime());
}
bool CMymfcTool::WaitTimer(int second, int msecond)
{
	HANDLE hEvent = CreateEvent(NULL, true, false, NULL);
	WaitForSingleObjectEx(hEvent, second*1000+msecond, true);//INFINITE 
	CloseHandle(hEvent);
	return true;
}

string CMymfcTool::ToUTF8(const wchar_t* buffer, int len)
{
	int size = ::WideCharToMultiByte(CP_UTF8, 0, buffer, len, NULL, 0, NULL,
		NULL);
	if (size == 0)
		return "";

	std::string newbuffer;
	newbuffer.resize(size);
	::WideCharToMultiByte(CP_UTF8, 0, buffer, len,
		const_cast<char*>(newbuffer.c_str()), size, NULL, NULL);

	return newbuffer;
}

string CMymfcTool::ReadCmdValueConfigFile(const TCHAR* pTypeName, string nodeName)
{
	return "";
}
CString CMymfcTool::GetTime1(void)
{
	struct tm ltime;
	time_t local_tv_sec;
	char ctime[16];
	time(&local_tv_sec);
	localtime_s(&ltime,&local_tv_sec);
	strftime(ctime, sizeof ctime, "%Y-%m-%d", &ltime);
	string strttime(ctime);
	return stringToCString(strttime);
}
CString CMymfcTool::GetTime2(void)
{
	struct tm ltime;
	time_t local_tv_sec;
	char ctime[16];
	time(&local_tv_sec);
	localtime_s(&ltime,&local_tv_sec);
	strftime(ctime, sizeof ctime, "%H:%M:%S", &ltime);
	string strttime(ctime);
	return stringToCString(strttime);
}

int CMymfcTool::GetDayOfWeek()
{
	SYSTEMTIME sys;
	GetLocalTime(&sys);
	return sys.wDayOfWeek;
}
int CMymfcTool::GetCurrentHour()
{
	SYSTEMTIME sys;
	GetLocalTime(&sys);
	return sys.wHour;
}
static unsigned int idx=0;
void rolloutHandler(const char* filename, std::size_t size) {
	// SHOULD NOT LOG ANYTHING HERE BECAUSE LOG FILE IS CLOSED!
	std::cout << "************** Rolling out [" << filename << "] because it reached [" << size << " bytes]" << std::endl;

	// BACK IT UP

	std::stringstream ss;
	string strfilename = filename;
	strfilename = strfilename.substr(strfilename.length() - 31, 27);
	ss << "ren " << filename << " " << strfilename << "_" << ++idx << ".log";
	string command = ss.str().c_str();
	system(command.c_str());
}
void CMymfcTool::InitLogFile()
{
	el::Loggers::addFlag(el::LoggingFlag::StrictLogFileSizeCheck);
	string strfilename = CStringTostring(GetFullPath());
	strfilename += "\\log.conf";

	el::Configurations conf(strfilename.c_str());
	el::Loggers::reconfigureAllLoggers(conf);
	el::Loggers::addFlag(el::LoggingFlag::DisableApplicationAbortOnFatalLog);
	el::Helpers::installPreRollOutCallback(rolloutHandler);
}
void CMymfcTool::AddFileLog(CString str)
{
	string str1 = CStringTostring(str);
	LOG(INFO) << str1.c_str();
}