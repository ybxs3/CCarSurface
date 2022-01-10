#pragma once


// CCarDlg 对话框

class CCarDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CCarDlg)

public:
	CCarDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CCarDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_CAR };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
