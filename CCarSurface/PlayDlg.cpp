// PlayDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "CCarSurface.h"
#include "PlayDlg.h"
#include "afxdialogex.h"
#include "CCarSurfaceDlg.h"

// CPlayDlg 对话框


IMPLEMENT_DYNAMIC(CPlayDlg, CDialogEx)

CPlayDlg::CPlayDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_PLAY, pParent)
{
	
	m_nmode = 1;
	m_bIsFullScreen = false;
	m_hbr = NULL;
	m_sPicPath = _T("");
}

CPlayDlg::~CPlayDlg()
{
	
}

void CPlayDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPlayDlg, CDialogEx)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_CTLCOLOR()
	ON_WM_SIZE()
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CPlayDlg 消息处理程序
HRESULT CPlayDlg::ShowNetPic(TCHAR *lpstrImgUrl, HWND hWnd)
{
	CDC* hDC_Temp = GetDC();

	DWORD dwFileSize, dwByteRead;
	HINTERNET hOpen = InternetOpen(NULL, PRE_CONFIG_INTERNET_ACCESS, NULL, NULL, 0);
	HINTERNET hRequest = InternetOpenUrl(hOpen, lpstrImgUrl, NULL, 0, INTERNET_FLAG_TRANSFER_BINARY, 0);
	if (hOpen)
	{
		InternetCloseHandle(hOpen);
	}

	if (!hRequest)return FALSE;

	InternetCloseHandle(hRequest);

	CInternetSession session;
	CInternetFile* remoteFile = (CInternetFile*)session.OpenURL(
		lpstrImgUrl, 1,
		INTERNET_FLAG_TRANSFER_BINARY |
		INTERNET_FLAG_DONT_CACHE);
	if (!remoteFile)
	{
		remoteFile->Close();
		delete remoteFile;
		session.Close();
		return FALSE;
	}
	DWORD statusCode;
	((CHttpFile *)remoteFile)->QueryInfoStatusCode(statusCode);
	if (statusCode == 404)
		return FALSE;


	CFile localFile;//MZ?
	CString sfilename;
	sfilename.Format(_T("F://test%d.jpg"), m_nmode);
	if (localFile.Open(sfilename, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary, NULL))
	{
		int readlen = -1;
		char buf[1024];
		while (1)
		{
			readlen = remoteFile->Read(buf, 1024);
			if (readlen == 0)
			{
				break;
			}
			localFile.Write(buf, readlen);
			ZeroMemory(buf, 1024);
		}
		localFile.Close();
	}


	CRect  rect;
	GetClientRect(&rect);   		//获取客户区大小     
	Graphics graphics(hDC_Temp->m_hDC);
	Image image(sfilename, FALSE);
	graphics.DrawImage(&image, 0, 0, rect.right, rect.bottom);  //绘制背景


	remoteFile->Close();
	session.Close();
	delete remoteFile;

	return S_OK;

}

HRESULT CPlayDlg::ShowPic(TCHAR *filename)
{
	CDC* hDC_Temp = GetDC();

	CRect  rect;
	GetClientRect(&rect);   		//获取客户区大小     
	Graphics graphics(hDC_Temp->m_hDC);

	Image image(filename, FALSE);
	graphics.DrawImage(&image, 0, 0, rect.right, rect.bottom);  //绘制背景


	return S_OK;

}

void CPlayDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	
	if (m_bIsFullScreen)
	{
		SetParent(m_parent);
		MoveWindow(m_rect);
		//m_parentdlg->ResetPos();
		m_bIsFullScreen = false;
	}
	else
	{
		m_parent = GetParent();
		GetWindowRect(m_rect);
		m_parent->ScreenToClient(m_rect);
		CWnd* pDeskWnd = GetDesktopWindow();
		CRect orect;
		SetParent(pDeskWnd);
		pDeskWnd->GetClientRect(orect);
		MoveWindow(orect);
		m_bIsFullScreen = true;
	}
	CDialogEx::OnLButtonDblClk(nFlags, point);
}
void CPlayDlg::SetMode(int nMode)
{
	m_nmode = nMode;
}

HBRUSH CPlayDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	//HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何特性

	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	if (m_hbr == NULL)
	{
		if (m_nmode == 0)
			m_hbr = CreateSolidBrush(RGB(255, 0, 0));
		else if (m_nmode == 1)
			m_hbr = CreateSolidBrush(RGB(0, 255, 0));
		else if (m_nmode == 2)
			m_hbr = CreateSolidBrush(RGB(0, 0, 255));
	}
	
	return m_hbr;
}


void CPlayDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
}


void CPlayDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 在此处添加消息处理程序代码
					   // 不为绘图消息调用 CDialogEx::OnPaint()
	//ShowPic(_T("F://test.jpg"));
	if (m_sPicPath.GetLength() > 0)
	{
		ShowNetPic(m_sPicPath.GetBuffer(0),m_hWnd);
	}
}
void CPlayDlg::SetMyParent(CCCarSurfaceDlg *parent)
{
	m_parentdlg = parent;
}
void CPlayDlg::SetShowStr(CString name)
{
	
}

void CPlayDlg::SetShowPicStr(CString name)
{
	m_sPicPath = name;
	RedrawWindow();
}