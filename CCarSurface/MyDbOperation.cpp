#include "StdAfx.h"
#include "MyDbOperation.h"

CMyDbOperation::CMyDbOperation(void)
{
	m_pSecurityRs=CADORecordset(&m_pSecurityDb);
	InitializeCriticalSection(&m_cSecurityDBLock);
}

CMyDbOperation::~CMyDbOperation(void)
{
	DeleteCriticalSection(&m_cSecurityDBLock);
}
bool CMyDbOperation::CloseConnect()
{
	m_pSecurityDb.Close();
	
	return true;
}
bool CMyDbOperation::ReConncet()
{
	m_pSecurityDb.Close();
	bool bresult = m_pSecurityDb.Open(m_sSecurityConStr);

	if (!bresult)
	{
		//AfxMessageBox(_T("打开数据库失败!"));
	}
	return bresult;
}
bool CMyDbOperation::OpenSecurityDB()
{
	bool bresult=true;
	if (!m_pSecurityDb.IsOpen())
		bresult = m_pSecurityDb.Open(m_sSecurityConStr);

	if (!bresult)
	{
		//AfxMessageBox(_T("打开数据库失败!"));
	}
	return bresult;
}
bool CMyDbOperation::WriteData(CString& str)
{
	bool bresult =false;
	EnterCriticalSection(&m_cSecurityDBLock);
	if (OpenSecurityDB())
		bresult = m_pSecurityDb.Execute(str);
	LeaveCriticalSection(&m_cSecurityDBLock);
	return bresult;
}
void CMyDbOperation::SetSecurityConStr(CString str)
{
	m_sSecurityConStr=str;
}
CString	CMyDbOperation::GetConstr()
{
	return m_sSecurityConStr;
}
bool CMyDbOperation::GetMaxStartTime(ULONGLONG& nMaxId,int ntemp)
{
	bool bresult = false;
	EnterCriticalSection(&m_cSecurityDBLock);
	if (OpenSecurityDB())
	{
		CString strSQL;
		if (ntemp)
		{
			strSQL.Format(_T("select MAX(id) as maxid from oa_id_car_record where state=1"));

		}
		else
			strSQL.Format(_T("select MAX(id) as maxid from oa_id_car_record"));
		if (m_pSecurityRs.Open(strSQL, CADORecordset::openQuery))
		{
			int ncount = m_pSecurityRs.GetRecordCount();
			if (ncount>0)
			{
				m_pSecurityRs.MoveFirst();
				m_pSecurityRs.GetFieldValue(_T("maxid"), nMaxId);
				bresult = true;
			}
			m_pSecurityRs.Close();
		}
	}
	LeaveCriticalSection(&m_cSecurityDBLock);
	return bresult;
}
int CMyDbOperation::GetCardRecord(ULONGLONG& nMaxId, CString& sCardid,int& nDevice,CString& sDate)
{
	int nresult = -2;
	EnterCriticalSection(&m_cSecurityDBLock);
	if (OpenSecurityDB())
	{
		nresult = -1;
		CString strSQL;
		strSQL.Format(_T("select * from oa_id_car_record where id>%I64u"), nMaxId);
		if (m_pSecurityRs.Open(strSQL, CADORecordset::openQuery))
		{
			nresult = 0;
			
			int ncount = m_pSecurityRs.GetRecordCount();
			if (ncount>0)
			{
				m_pSecurityRs.MoveFirst();
				m_pSecurityRs.GetFieldValue(_T("id"), nMaxId);
				m_pSecurityRs.GetFieldValue(_T("cardno"), sCardid);
				m_pSecurityRs.GetFieldValue(_T("device"), nDevice);
				m_pSecurityRs.GetFieldValue(_T("date"), sDate);
				nresult = 1;
			}
			m_pSecurityRs.Close();
		}
		else
		{
			m_pSecurityRs.Close();
			m_pSecurityDb.Close();
		}
	}
	LeaveCriticalSection(&m_cSecurityDBLock);
	return nresult;
}

int CMyDbOperation::GetCardInfo(CString& sCardid,CString& sName,int& ntype)
{
	int nresult = -2;
	EnterCriticalSection(&m_cSecurityDBLock);
	if (OpenSecurityDB())
	{
		nresult = -1;
		CString strSQL;
		strSQL.Format(_T("select * from oa_id_car_info where cardid24='%s'"), sCardid);
		if (m_pSecurityRs.Open(strSQL, CADORecordset::openQuery))
		{
			nresult = 0;

			int ncount = m_pSecurityRs.GetRecordCount();
			if (ncount>0)
			{
				m_pSecurityRs.MoveFirst();
				m_pSecurityRs.GetFieldValue(_T("name"), sName);
				CString stype = _T("");
				m_pSecurityRs.GetFieldValue(_T("type"), stype);
				if (stype == _T("车辆"))
					ntype = 1;
				else
					ntype = 2;
				nresult = 1;
			}
			m_pSecurityRs.Close();
		}
	}
	LeaveCriticalSection(&m_cSecurityDBLock);
	return nresult;
}

int CMyDbOperation::GetPersonInfo(CString& sCardid, CString& sName,CString& image, int& ntype)
{
	int nresult = -2;
	EnterCriticalSection(&m_cSecurityDBLock);
	if (OpenSecurityDB())
	{
		nresult = -1;
		CString strSQL;
		strSQL.Format(_T("select * from oa_id_person where cardid24='%s' and ptype =''"), sCardid);
		if (m_pSecurityRs.Open(strSQL, CADORecordset::openQuery))
		{
			nresult = 0;

			int ncount = m_pSecurityRs.GetRecordCount();
			if (ncount > 0)
			{
				m_pSecurityRs.MoveFirst();
				m_pSecurityRs.GetFieldValue(_T("name"), sName);
				CString stype = _T("");
				m_pSecurityRs.GetFieldValue(_T("image"), stype);
				
				ntype = 2;
				nresult = 1;
			}
			m_pSecurityRs.Close();
		}
	}
	LeaveCriticalSection(&m_cSecurityDBLock);
	return nresult;
}
int CMyDbOperation::GetPeopleInfo(CString& sCardid, CString& sName, CString& sPic)
{
	int nresult = -2;
	EnterCriticalSection(&m_cSecurityDBLock);
	if (OpenSecurityDB())
	{
		nresult = -1;
		CString strSQL;
		strSQL.Format(_T("select * from oa_id_person where cardid24='%s'"), sCardid);
		if (m_pSecurityRs.Open(strSQL, CADORecordset::openQuery))
		{
			nresult = 0;

			int ncount = m_pSecurityRs.GetRecordCount();
			if (ncount>0)
			{
				m_pSecurityRs.MoveFirst();
				m_pSecurityRs.GetFieldValue(_T("name"), sName);
				m_pSecurityRs.GetFieldValue(_T("image"), sPic);
				nresult = 1;
			}
			m_pSecurityRs.Close();
		}
	}
	LeaveCriticalSection(&m_cSecurityDBLock);
	return nresult;
}
int CMyDbOperation::GetLastCarInfo(CString& sCarNo, CString& sPic)
{
	int nresult = -2;
	EnterCriticalSection(&m_cSecurityDBLock);
	if (OpenSecurityDB())
	{
		nresult = -1;
		CString strSQL;
		strSQL=(_T("SELECT * FROM oa_id_carrecord_old WHERE id=(SELECT max(id) FROM oa_id_carrecord_old)"));
		if (m_pSecurityRs.Open(strSQL, CADORecordset::openQuery))
		{
			nresult = 0;

			int ncount = m_pSecurityRs.GetRecordCount();
			if (ncount>0)
			{
				m_pSecurityRs.MoveFirst();
				m_pSecurityRs.GetFieldValue(_T("carnum"), sCarNo);
				CString stype = _T("");
				m_pSecurityRs.GetFieldValue(_T("picname"), sPic);
				
				nresult = 1;
			}
			m_pSecurityRs.Close();
		}
	}
	LeaveCriticalSection(&m_cSecurityDBLock);
	return nresult;
}