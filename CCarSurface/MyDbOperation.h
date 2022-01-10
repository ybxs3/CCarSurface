#pragma once
#include "ado2.h"
#include "MymfcTool.h"
#include <map>
class CMyDbOperation
{
private:
	CMymfcTool								m_tool;
	CADODatabase							m_pSecurityDb;
	CADORecordset							m_pSecurityRs;
	CString									m_sSecurityConStr;
	CRITICAL_SECTION						m_cSecurityDBLock;
public:
	CMyDbOperation(void);
	~CMyDbOperation(void);
	void SetSecurityConStr(CString str);
	CString	GetConstr();
	bool CloseConnect();
	bool ReConncet();
	bool OpenSecurityDB();
	bool WriteData(CString& str);
	bool GetMaxStartTime(ULONGLONG& nMaxStartTime,int ntemp=0);
	int GetCardRecord(ULONGLONG& nMaxTime, CString& sCardid, int& nDevice, CString& sDate);
	int GetCardInfo(CString& sCardid, CString& sName, int& ntype);
	int GetPersonInfo(CString& sCardid, CString& sName, CString& image, int& ntype);
	int GetPeopleInfo(CString& sCardid, CString& sName, CString& sPic);
	int GetLastCarInfo(CString& sCarNo, CString& sPic);
};
