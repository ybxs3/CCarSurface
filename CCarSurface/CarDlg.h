#pragma once


// CCarDlg �Ի���

class CCarDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CCarDlg)

public:
	CCarDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CCarDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_CAR };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
};
