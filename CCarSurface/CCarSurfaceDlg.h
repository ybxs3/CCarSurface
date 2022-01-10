
// CCarSurfaceDlg.h : 头文件
//

#pragma once
#include "PlayDlg.h"
#include "Utility.h"
#include "MymfcTool.h"
#include "XThread.h"
// CCCarSurfaceDlg 对话框

#define WM_SHOWMSG_DLG WM_USER+101
class PostData
{
public:
	int ntype;
	int ndirect;
	CString sCardNo;
	CString sName;
	CString sCarNum;
	CString sPic;
	PostData(int direct,int type, CString cardno, CString name, CString carno,CString pic)
	{
		ndirect = direct;
		ntype = type;
		sCardNo = cardno;
		sName = name;
		sCarNum = carno;
		sPic = pic;
	}
};
class CCCarSurfaceDlg : public CDialogEx,public XThread
{
// 构造
public:
	CCCarSurfaceDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CCARSURFACE_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


public:
	CPlayDlg  m_playdlg[3];
	NOTIFYICONDATA					m_tnid;
	bool m_bexit;

	CMymfcTool   m_tools;
// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:

	UINT	Work_(void);
	UINT	Worker(void);
	bool	TermViolent(void) { return true; }

	afx_msg void OnDestroy();
	void InitDlg();
	void ResetPos();
	afx_msg void OnTpShow();
	afx_msg void OnTpExit();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnClose();
	bool login();
	void play();
	static int  __stdcall RealLoadPicCallback(LLONG lAnalyzerHandle, DWORD dwAlarmType, void* pAlarmInfo, BYTE *pBuffer, DWORD dwBufSize, LDWORD dwUser, int nSequence, void *userdata);
	//断线回调
	static void __stdcall OnDisConnect(LLONG lLoginID, char *pchDVRIP, LONG nDVRPort, LDWORD dwUser);
	void ReceiveRealData(LLONG lRealHandle, DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize);
	BOOL CCCarSurfaceDlg::InitSdk();

	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnMenuCar();
	afx_msg void OnMenuPeople();
};
