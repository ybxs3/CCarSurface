
// CCarSurfaceDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "CCarSurface.h"
#include "CCarSurfaceDlg.h"
#include "afxdialogex.h"
#include "PlayApi.h"
#include "MyDbOperation.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#define MYWM_NOTIFYICON WM_USER+1
#pragma comment(lib,"dhconfigsdk.lib")
#pragma comment(lib,"dhnetsdk.lib")
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框


CPlayAPI g_PlayAPI;
LONG m_lLoginID;
LONG m_lPlayID;
int g_connected = 0;
class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CCCarSurfaceDlg 对话框


ULONG_PTR m_uGdiplusToken;
CCCarSurfaceDlg::CCCarSurfaceDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_CCARSURFACE_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	GdiplusStartupInput gdiplusStartupInput;//初始化结构体
	GdiplusStartup(&m_uGdiplusToken, &gdiplusStartupInput, NULL);
}

void CCCarSurfaceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CCCarSurfaceDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_COMMAND(ID_TP_SHOW, &CCCarSurfaceDlg::OnTpShow)
	ON_COMMAND(ID_TP_EXIT, &CCCarSurfaceDlg::OnTpExit)
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_OK, &CCCarSurfaceDlg::OnBnClickedButtonOk)
	ON_COMMAND(ID_MENU_CAR, &CCCarSurfaceDlg::OnMenuCar)
	ON_COMMAND(ID_MENU_PEOPLE, &CCCarSurfaceDlg::OnMenuPeople)
END_MESSAGE_MAP()


// CCCarSurfaceDlg 消息处理程序

BOOL CCCarSurfaceDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	m_tnid.cbSize = sizeof(NOTIFYICONDATA);
	m_tnid.hWnd = this->m_hWnd;
	m_tnid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	m_tnid.uCallbackMessage = MYWM_NOTIFYICON;
	//用户定义的回调消息
	CString szToolTip;
	szToolTip = _T("停车场管理系统客户端");
	_tcscpy_s(m_tnid.szTip,128,szToolTip);
	m_tnid.uID = IDR_MAINFRAME;
	HICON hIcon;
	hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_tnid.hIcon = hIcon;
	::Shell_NotifyIcon(NIM_ADD, &m_tnid);
	if (hIcon)::DestroyIcon(hIcon);

	InitDlg();
	m_bexit = false;
	g_PlayAPI.LoadPlayDll();
	InitSdk();
	play();
	Start();
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CCCarSurfaceDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CCCarSurfaceDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CCCarSurfaceDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CCCarSurfaceDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	Gdiplus::GdiplusShutdown(m_uGdiplusToken);
}
void CCCarSurfaceDlg::InitDlg()
{

	RECT rc = { 0 };
	GetClientRect(&rc);
	RECT rc_old(rc);
	rc.bottom = rc.bottom - 50;

	int npos_x = (rc.right*2 / 3);
	int npos_y = rc.bottom / 2 - 32;

	int nwidth = rc.right - npos_x - 2;
	int nheight = npos_y;
	
	m_playdlg[0].SetMode(0);
	m_playdlg[0].Create(IDD_DIALOG_PLAY, this);
	m_playdlg[0].MoveWindow(0, 0, npos_x-1, rc.bottom);
	m_playdlg[0].ShowWindow(SW_SHOW);
	m_playdlg[0].SetMyParent(this);
	
	m_playdlg[1].Create(IDD_DIALOG_PLAY, this);
	m_playdlg[1].SetMode(1);
	m_playdlg[1].MoveWindow(npos_x+1, 0, nwidth, nheight);
	m_playdlg[1].ShowWindow(SW_SHOW);
	m_playdlg[1].SetMyParent(this);
	GetDlgItem(IDC_STATIC_CARNUM)->MoveWindow(npos_x+1,npos_y+2, nwidth,30);
	GetDlgItem(IDC_STATIC_CARNUM)->SetWindowTextW(_T("车牌："));

	m_playdlg[2].Create(IDD_DIALOG_PLAY, this);
	m_playdlg[2].SetMode(2);
	m_playdlg[2].MoveWindow(npos_x + 1, npos_y + 32, nwidth, npos_y);
	m_playdlg[2].ShowWindow(SW_SHOW);
	m_playdlg[2].SetMyParent(this);

	GetDlgItem(IDC_STATIC_PEOPLE)->MoveWindow(npos_x + 1, rc.bottom - 32, nwidth, 30);
	GetDlgItem(IDC_STATIC_PEOPLE)->SetWindowTextW(_T("驾驶员："));

	int npos_direct = rc.right /2;
	GetDlgItem(IDC_STATIC_DIRECT)->MoveWindow(npos_direct, rc.bottom+6, 200, 30);
	GetDlgItem(IDC_STATIC_DIRECT)->SetWindowTextW(_T("警戒门："));

	GetDlgItem(IDC_BUTTON_OK)->MoveWindow(npos_direct+210, rc.bottom + 2, 120, 40);
	
}
void CCCarSurfaceDlg::ResetPos()
{
	RECT rc = { 0 };
	GetClientRect(&rc);
	int npos_x = (rc.right * 2 / 3);
	int npos_y = rc.bottom / 2 - 32;

	int nwidth = rc.right - npos_x;

	
	m_playdlg[0].MoveWindow(0, 0, npos_x - 1, rc.bottom);
	m_playdlg[0].ShowWindow(SW_SHOW);

	m_playdlg[1].MoveWindow(npos_x + 1, 0, nwidth, npos_y);
	m_playdlg[1].ShowWindow(SW_SHOW);

	GetDlgItem(IDC_STATIC_CARNUM)->MoveWindow(npos_x + 2, npos_y+2, nwidth, 30);
	GetDlgItem(IDC_STATIC_CARNUM)->SetWindowTextW(_T("车牌："));

	m_playdlg[2].MoveWindow(npos_x + 1, npos_y + 32, nwidth, npos_y);
	m_playdlg[2].ShowWindow(SW_SHOW);

	GetDlgItem(IDC_STATIC_PEOPLE)->MoveWindow(npos_x + 1, rc.bottom-32, nwidth, 30);
	GetDlgItem(IDC_STATIC_PEOPLE)->SetWindowTextW(_T("驾驶员："));
	
}
int g_times = 0;
LRESULT CCCarSurfaceDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: 在此添加专用代码和/或调用基类
	switch (message)
	{
	case MYWM_NOTIFYICON:
		//如果是用户定义的消息
		if (lParam == WM_LBUTTONDBLCLK)
		{
			//鼠标双击时主窗口出现
			AfxGetApp()->m_pMainWnd->ShowWindow(SW_SHOW);
		}
		else if (lParam == WM_RBUTTONDOWN) {
			//鼠标右键单击弹出菜单
			CMenu menu;
			menu.LoadMenu(IDR_MENU_RIGHT);
			//载入事先定义的菜单
			CMenu* pMenu = menu.GetSubMenu(0);
			CPoint pos;
			GetCursorPos(&pos);
			pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pos.x, pos.y, AfxGetMainWnd());
		}
		break;

	case WM_SHOWMSG_DLG:
		//SetTimer(2, 600, NULL);
		PostData* data1 = (PostData*)wParam;
		if (data1->sCarNum == _T("first"))
		{
			GetDlgItem(IDC_STATIC_CARNUM)->SetWindowTextW(_T("车牌："));
			GetDlgItem(IDC_STATIC_PEOPLE)->SetWindowTextW(_T("驾驶员："));
			GetDlgItem(IDC_STATIC_DIRECT)->SetWindowTextW(_T("警戒门："));

			m_playdlg[1].SetShowPicStr(_T(""));
			m_playdlg[2].SetShowPicStr(_T(""));
		}
		if (data1->ntype == 1)
		{
			CString spicpath;
			spicpath.Format(_T("http://192.168.88.101:8081/carpassrecord/%s.jpg"), data1->sPic);
			m_playdlg[1].SetShowPicStr(spicpath);
			m_playdlg[1].SetShowStr(data1->sName);
			CString str1;
			str1.Format(_T("车牌：%s"),data1->sName);
			GetDlgItem(IDC_STATIC_CARNUM)->SetWindowTextW(str1);
			
		}
		else if(data1->ntype==2)
		{
			CString spicpath;
			if (data1->sPic.GetLength() < 1)
			{
				spicpath.Format(_T("http://192.168.88.101:8081/driver/%s.jpg"), data1->sName);

			}
			else
			{
				spicpath.Format(_T("http://192.168.88.101:8081/driver/%s.jpg"), data1->sPic);
			}
			m_playdlg[2].SetShowPicStr(spicpath);
			m_playdlg[2].SetShowStr(data1->sName);
			CString str1;
			str1.Format(_T("驾驶员：%s"), data1->sName);
			GetDlgItem(IDC_STATIC_PEOPLE)->SetWindowTextW(str1);
		}
		else if (data1->ntype == 3)
		{
			GetDlgItem(IDC_STATIC_CARNUM)->SetWindowTextW(_T("车牌："));
			m_playdlg[1].SetShowPicStr(_T(""));

			CString spicpath;
			spicpath.Format(_T("http://192.168.88.101:8081/%s.jpg"), data1->sName);
			m_playdlg[2].SetShowPicStr(spicpath);
			m_playdlg[2].SetShowStr(data1->sName);
			CString str1;
			str1.Format(_T("人员姓名：%s"), data1->sName);
			GetDlgItem(IDC_STATIC_PEOPLE)->SetWindowTextW(str1);
		}
		if (data1->ndirect == 2)
		{//in
			GetDlgItem(IDC_STATIC_DIRECT)->SetWindowTextW(_T("警戒门：进"));
		}
		else if (data1->ndirect == 1)
		{// out
			GetDlgItem(IDC_STATIC_DIRECT)->SetWindowTextW(_T("警戒门：出"));
		}
		ShowWindow(SW_SHOW);
		SetTimer(1, 1000, NULL);
		g_times = 0;
		delete data1;
		data1 = NULL;
		break;
	}
	return __super::WindowProc(message, wParam, lParam);
}
BOOL CCCarSurfaceDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (pMsg->message == WM_KEYDOWN)
	{
		switch (pMsg->wParam)
		{
		case VK_RETURN:
			return TRUE;
		case VK_ESCAPE:
			return TRUE;
		default:
			break;
		}
	}
	return __super::PreTranslateMessage(pMsg);
}
void CCCarSurfaceDlg::OnTpShow()
{
	// TODO: 在此添加命令处理程序代码
	ShowWindow(SW_SHOW);
}


void CCCarSurfaceDlg::OnTpExit()
{
	// TODO: 在此添加命令处理程序代码
	if (MessageBox(_T("客户端退出后会将无法显示车辆信息，是否确定退出？"), _T("警告"), MB_ICONWARNING | MB_YESNO) != IDYES)
	{
		return;
	}
	::Shell_NotifyIcon(NIM_DELETE, &m_tnid);

	m_bexit = true;
	OnCancel();
}

void CCCarSurfaceDlg::OnDisConnect(LLONG lLoginID, char *pchDVRIP, LONG nDVRPort, LDWORD dwUser)
{
	printf("disconnect \r\n");
	g_connected = 0;
	return;
}
BOOL CCCarSurfaceDlg::InitSdk()
{

	CLIENT_Init(CCCarSurfaceDlg::OnDisConnect, (LDWORD)this);
	if (!login())
		return false;



	return true;
}
bool CCCarSurfaceDlg::login()
{
	NET_DEVICEINFO_Ex netDevInfo = { 0 };
	int error = 0;
	LONG lLoginID = CLIENT_LoginEx2("192.168.88.204", (WORD)37777, "admin",
		"admin", EM_LOGIN_SPEC_CAP_TCP,
		NULL, &netDevInfo, &error);
	if (lLoginID == 0)
	{
		m_tools.AddFileLog(_T("Login failed\r\n"));
		return false;
	}

	//Login ok
	m_lLoginID = lLoginID;

	////////////////////////////////////////////////////////////play video



	//////////////////////////////////////////////////////////////////////////准备订阅
	NET_RESERVED_COMMON stReservedCommon = { 0 };
	stReservedCommon.dwStructSize = sizeof(NET_RESERVED_COMMON);
	stReservedCommon.dwSnapFlagMask = 32;
	ReservedPara stReserved;
	stReserved.dwType = RESERVED_TYPE_FOR_COMMON;
	stReserved.pData = &stReservedCommon;
	LONG lRet = 0;// CLIENT_RealLoadPictureEx(m_lLoginID, 0, EVENT_IVS_ALL, true, CCCarSurfaceDlg::RealLoadPicCallback, (LDWORD)1, (void*)&stReserved);
	if (lRet == 0)
	{
		//MessageBox(ConvertString("Subscribe picture failed!"), ConvertString("Prompt"));
		m_tools.AddFileLog(_T("Subscribe picture failed!\r\n"));

		return false;
	}
	m_tools.AddFileLog(_T("login success!\r\n"));
	g_connected = 1;
	return true;
}

void CALLBACK RealDataCallBackEx(LLONG lRealHandle, DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize, LONG lParam, LDWORD dwUser)
{
	if (dwUser == 0)
	{
		return;
	}

	CCCarSurfaceDlg *dlg = (CCCarSurfaceDlg *)dwUser;
	dlg->ReceiveRealData(lRealHandle, dwDataType, pBuffer, dwBufSize);
}
//Process after receiving real-time data 
void CCCarSurfaceDlg::ReceiveRealData(LLONG lRealHandle, DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize)
{
	//Stream port number according to the real-time handle.
	long lRealPort = 450;
	//Input the stream data getting from the card
	BOOL bInput = FALSE;
	if (0 != lRealPort)
	{
		switch (dwDataType) {
		case 0:
			//Original data 
			bInput = g_PlayAPI.PLAY_InputData(lRealPort, pBuffer, dwBufSize);
			break;
		case 1:
			//Standard video data 

			break;
		case 2:
			//yuv data 

			break;
		case 3:
			//pcm audio data 

			break;
		case 4:
			//Original audio data 

			break;
		default:
			break;
		}
	}
}
void CCCarSurfaceDlg::play()
{
	//Open the preview
	int nChannelID = 0;
	int nPort = 450;
	//Enable stream
	BOOL bOpenRet = g_PlayAPI.PLAY_OpenStream(nPort,0,0,1024 * 900);
	if (bOpenRet)
	{
	
		//Begin play 
		BOOL bPlayRet = g_PlayAPI.PLAY_Play(nPort,m_playdlg[0].GetSafeHwnd());
		if (bPlayRet)
		{
			//Real-time play 
			m_lPlayID = CLIENT_RealPlayEx(m_lLoginID,nChannelID,0);
			if (0 != m_lPlayID)
			{
				//Callback monitor data and then save 
				CLIENT_SetRealDataCallBackEx(m_lPlayID, RealDataCallBackEx, (LDWORD)this, 0x1f);
			}
			else
			{
				//MessageBox(ConvertString("Fail to play!"), ConvertString("Prompt"));
			}
		}
	}
}
void CCCarSurfaceDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (!m_bexit)
	{
		ShowWindow(SW_HIDE);
		return;
	}
	
	CDialogEx::OnClose();
}
UINT CCCarSurfaceDlg::Worker(void)
{
	UINT nresult = 0;
	try
	{
		nresult = Work_();
	}
	catch (...)
	{
		nresult = -1;
		AfxMessageBox(_T("work thread error"));
	}
	return nresult;
}

UINT CCCarSurfaceDlg::Work_(void)
{

	CMyDbOperation dbfun;
	dbfun.SetSecurityConStr(_T("Driver=MySQL ODBC 5.1 Driver;SERVER=192.168.88.101;UID=root;PWD=123456;DATABASE=xws"));
	//dbfun.SetSecurityConStr(_T("Driver=MySQL ODBC 5.1 Driver;SERVER=127.0.0.1;UID=root;PWD=root;DATABASE=xws"));
	CString strOldCarNum = _T("");
	CMymfcTool tool;
	int nOldCamId = 0;
	ULONGLONG nMaxid = 0;
	dbfun.GetMaxStartTime(nMaxid);
	ULONGLONG uTempMaxid = 0;
	dbfun.GetMaxStartTime(uTempMaxid,1);

	

	CString sLastTime = _T("");
	int bFirstCar = false;
	int bfirstpeople = false;
	CString sDriverName = _T("");
	CString sCarName = _T("");
	CString sPic = _T("");

	map<CString, CString> mCarsMap;

	while (!IsExitSet())
	{

		

		
		Sleep(50);
		CString sCardId;
		int ndevice = 0;
		CString sDate;
		int nresult1 = dbfun.GetCardRecord(uTempMaxid, sCardId, ndevice, sDate);
		ULONGLONG ntemp1 = uTempMaxid;

		if (nresult1 > 0)
		{
			if (ntemp1 > 0)
			{
				CString upsql;
				upsql.Format(_T("UPDATE oa_id_car_record SET state=1 WHERE id=%I64u"), ntemp1);
				dbfun.WriteData(upsql);
			}
			CString sName = _T("");
			CString sImage = _T("");
			int ntype = 0;
			int nresult = dbfun.GetCardInfo(sCardId, sName, ntype);

			if (nresult > 0)
			{//deal car or driver
				if (ndevice == 2)
				{//deal in
					if (sLastTime.GetLength() > 0)
					{
						COleDateTime dTime_start;
						dTime_start.ParseDateTime(sLastTime, LOCALE_NOUSEROVERRIDE);//字符串转日期  
						COleDateTime dTime_end;
						dTime_end.ParseDateTime(sDate, LOCALE_NOUSEROVERRIDE);//字符串转日期  

						COleDateTimeSpan dTimeSpan = dTime_end - dTime_start;
						int nSecnonSpan = dTimeSpan.GetTotalSeconds();

						if (nSecnonSpan >60)
						{
							bfirstpeople = false;
							bFirstCar = false;
							sDriverName = _T("");
							sCarName = _T("");
						}
					}
					sLastTime = sDate;

					if (ntype == 2)
					{//deal people
						if (bFirstCar)
						{
							if (uTempMaxid > nMaxid)
							{
								CString sCarno;
								dbfun.GetLastCarInfo(sCarno, sPic);
							}
							bool bwrite = true;
							if (mCarsMap.find(sCarName) != mCarsMap.end())
							{
								
								COleDateTime dCarTime_start;
								dCarTime_start.ParseDateTime(mCarsMap[sCarName], LOCALE_NOUSEROVERRIDE);//字符串转日期  
								COleDateTime dCarTime_end;
								dCarTime_end.ParseDateTime(sDate, LOCALE_NOUSEROVERRIDE);//字符串转日期  

								COleDateTimeSpan dCarTimeSpan = dCarTime_end - dCarTime_start;
								int nSecnonSpan = dCarTimeSpan.GetTotalSeconds();

								if (nSecnonSpan <600)
								{
									bwrite = false;
								}
							}
							mCarsMap[sCarName] = sDate;
							if (bwrite)
							{
								CString strsql;
								strsql.Format(_T("INSERT INTO oa_id_carrecord(carnum,driver,date,state,picname) VALUES('%s','%s','%s',1,'%s')"), sCarName, sName, sDate, sPic);
								dbfun.WriteData(strsql);
							}
							
							bfirstpeople = false;
							bFirstCar = false;
							sDriverName = _T("");
							sCarName = _T("");
							if (uTempMaxid > nMaxid)
							{
								PostData* data1 = new PostData(2, ntype, sCardId, sName, _T("second"), _T(""));
								PostMessage(WM_SHOWMSG_DLG, (WPARAM)data1, 0);
							}
						}
						else
						{
							sDriverName = sName;
							bfirstpeople = true;
							
							if (uTempMaxid > nMaxid)
							{
								MANUAL_SNAP_PARAMETER stuSanpParam = { 0 };
								stuSanpParam.nChannel = 0;
								// 字符串长度判断，超过124会造成栈溢出
								CString m_strSnapSeq = _T("");
								int nCpLen = m_strSnapSeq.GetLength() > 64 ? 64 : m_strSnapSeq.GetLength();
								memcpy(stuSanpParam.bySequence, m_strSnapSeq, nCpLen);
								BOOL bRet = CLIENT_ControlDevice(m_lLoginID, DH_MANUAL_SNAP, &stuSanpParam);
							}

							if (uTempMaxid > nMaxid)
							{
								PostData* data1 = new PostData(2, ntype, sCardId, sName, _T("first"), _T(""));
								PostMessage(WM_SHOWMSG_DLG, (WPARAM)data1, 0);
							}
						}

						
					}
					else if(ntype==1)
					{//deal car
						if (bfirstpeople)
						{
							if (uTempMaxid > nMaxid)
							{
								CString sCarno;
								dbfun.GetLastCarInfo(sCarno, sPic);
							}
							bool bwrite = true;
							if (mCarsMap.find(sCarName) != mCarsMap.end())
							{

								COleDateTime dCarTime_start;
								dCarTime_start.ParseDateTime(mCarsMap[sCarName], LOCALE_NOUSEROVERRIDE);//字符串转日期  
								COleDateTime dCarTime_end;
								dCarTime_end.ParseDateTime(sDate, LOCALE_NOUSEROVERRIDE);//字符串转日期  

								COleDateTimeSpan dCarTimeSpan = dCarTime_end - dCarTime_start;
								int nSecnonSpan = dCarTimeSpan.GetTotalSeconds();

								if (nSecnonSpan < 600)
								{
									bwrite = false;
								}
							}
							mCarsMap[sName] = sDate;
							if (bwrite)
							{
								CString strsql;
								strsql.Format(_T("INSERT INTO oa_id_carrecord(carnum,driver,date,state,picname) VALUES('%s','%s','%s',1,'%s')"), sName, sDriverName, sDate, sPic);
								dbfun.WriteData(strsql);
							}
							
							bfirstpeople = false;
							bFirstCar = false;
							sDriverName = _T("");
							sCarName = _T("");

							if (uTempMaxid > nMaxid)
							{
								PostData* data1 = new PostData(2, ntype, sCardId, sName, _T("second"), _T(""));
								PostMessage(WM_SHOWMSG_DLG, (WPARAM)data1, 0);
							}
						}
						else
						{
							
							sCarName = sName;
							bFirstCar = true;

							if (uTempMaxid > nMaxid)
							{
								MANUAL_SNAP_PARAMETER stuSanpParam = { 0 };
								stuSanpParam.nChannel = 0;
								// 字符串长度判断，超过124会造成栈溢出
								CString m_strSnapSeq = _T("");
								int nCpLen = m_strSnapSeq.GetLength() > 64 ? 64 : m_strSnapSeq.GetLength();
								memcpy(stuSanpParam.bySequence, m_strSnapSeq, nCpLen);
								BOOL bRet = CLIENT_ControlDevice(m_lLoginID, DH_MANUAL_SNAP, &stuSanpParam);

							
									CString sCarno;
									dbfun.GetLastCarInfo(sCarno, sPic);
								
							}
							if (uTempMaxid > nMaxid)
							{
								PostData* data1 = new PostData(2, ntype, sCardId, sName, _T("first"), _T(""));
								PostMessage(WM_SHOWMSG_DLG, (WPARAM)data1, 0);
							}
						}

						
					}
				}
				else if (ndevice == 1)
				{//deal out 

				
					if (ntype == 1)
					{
						CString strsql;
						strsql.Format(_T("UPDATE oa_id_carrecord SET leavedate= '%s' WHERE carnum='%s' and date>DATE_SUB('%s',INTERVAL 1 day)"),sDate, sName,sDate);
						dbfun.WriteData(strsql);
						
						map<CString, CString>::iterator itr = mCarsMap.find(sName);
						if (itr != mCarsMap.end())
						{
							mCarsMap.erase(itr);
						}

					}
					if (uTempMaxid > nMaxid)
					{
						if (!bFirstCar && !bfirstpeople)
						{
							PostData* data1 = new PostData(1, ntype, sCardId, sName, _T("second"), _T(""));
							PostMessage(WM_SHOWMSG_DLG, (WPARAM)data1, 0);
						}
					}
									
				}
			}
			else
			{//判断卡是否提解员或者其他内部人员
				CString sPic;
				int nresult2 = dbfun.GetPeopleInfo(sCardId, sName, sPic);
				/*if (nresult2 > 0)
				{
					ntype = 3;
					PostData* data1 = new PostData(ndevice, ntype, sCardId, sName, sCardId, sPic);
					PostMessage(WM_SHOWMSG_DLG, (WPARAM)data1, 0);
				}*/
			}
			
		}
	}
	return 1;
}

void CCCarSurfaceDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nIDEvent == 1)
	{
		g_times++;
		if(g_times<3)
		{
			ShowWindow(SW_SHOW);
			::SetWindowPos(this->GetSafeHwnd(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
		}
		else 
		{
			KillTimer(1);
		}
	}

	__super::OnTimer(nIDEvent);
}


void CCCarSurfaceDlg::OnBnClickedButtonOk()
{
	// TODO: 在此添加控件通知处理程序代码
	KillTimer(1);
	GetDlgItem(IDC_STATIC_CARNUM)->SetWindowTextW(_T("车牌："));
	GetDlgItem(IDC_STATIC_PEOPLE)->SetWindowTextW(_T("驾驶员："));
	GetDlgItem(IDC_STATIC_DIRECT)->SetWindowTextW(_T("警戒门："));

	m_playdlg[1].SetShowPicStr(_T(""));
	m_playdlg[2].SetShowPicStr(_T(""));
	//ShowWindow(SW_HIDE);
	::SetWindowPos(this->GetSafeHwnd(), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	ShowWindow(SW_HIDE);
	
}


void CCCarSurfaceDlg::OnMenuCar()
{
	// TODO: 在此添加命令处理程序代码
	AfxMessageBox(_T("click car"));
}


void CCCarSurfaceDlg::OnMenuPeople()
{
	// TODO: 在此添加命令处理程序代码
	AfxMessageBox(_T("click people"));
}
