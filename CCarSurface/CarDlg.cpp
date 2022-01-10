// CarDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "CCarSurface.h"
#include "CarDlg.h"
#include "afxdialogex.h"


// CCarDlg 对话框

IMPLEMENT_DYNAMIC(CCarDlg, CDialogEx)

CCarDlg::CCarDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_CAR, pParent)
{

}

CCarDlg::~CCarDlg()
{
}

void CCarDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCarDlg, CDialogEx)
END_MESSAGE_MAP()


// CCarDlg 消息处理程序
