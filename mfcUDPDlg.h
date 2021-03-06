
// mfcUDPDlg.h: 头文件
//
#include <vector>
#pragma once
using namespace std;

// CmfcUDPDlg 对话框
class CmfcUDPDlg : public CDialog
{
// 构造
public:
	CmfcUDPDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCUDP_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	int m_MyPort;
	int m_DestPort;
	CString m_DestAddress;
	CString m_RecvData;
	CString m_SendData;
	vector<CString> que;
	SOCKET sock;
};
