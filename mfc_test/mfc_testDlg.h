
// mfc_testDlg.h : header file
//

#pragma once


// Cmfc_testDlg dialog
class Cmfc_testDlg : public CDialogEx
{
// Construction
public:
	Cmfc_testDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_MFC_TEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnBnClickedRadio2();
  afx_msg void OnBnClickedRadio1();
  afx_msg void OnMouseMove(UINT nFlags, CPoint point);
  afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
  int checkedIdx;
  void redrawDrawing(void) const;
  afx_msg void OnBnClickedCrappbtn();
  afx_msg void OnBnClickedGdappxbtn();
  CString input;
  afx_msg void LoadClicked();
  afx_msg void OnBnClickedSimplify1();
  double simplPer;
};
