
// BMA_StabilizationDlg.h : ��� ����
//

#pragma once


// CBMA_StabilizationDlg ��ȭ ����
class CBMA_StabilizationDlg : public CDialogEx
{
// �����Դϴ�.
public:
	CBMA_StabilizationDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_BMA_STABILIZATION_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.


// �����Դϴ�.
protected:
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedBUTTON_OPEN();
	afx_msg void OnBnClickedBUTTON_RUN();

};
