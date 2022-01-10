#pragma once
#include <gdiplus.h>

#pragma comment(lib,"gdiplus.lib")
using namespace Gdiplus;

// CPlayDlg 对话框
class CCCarSurfaceDlg;
class CPlayDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CPlayDlg)

public:
	CPlayDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPlayDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_PLAY };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	HRESULT ShowNetPic(TCHAR *lpstrImgUrl, HWND hWnd);
	HRESULT ShowPic(TCHAR *filename);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	void SetMode(int nMode);

private:
	int m_nmode;
	HBRUSH m_hbr;
	bool m_bIsFullScreen;
	CWnd* m_parent;
	CRect m_rect;
	CCCarSurfaceDlg* m_parentdlg;
	CString m_sPicPath;
	
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	void SetMyParent(CCCarSurfaceDlg *parent);
	void SetShowStr(CString name);
	void SetShowPicStr(CString name);
};
