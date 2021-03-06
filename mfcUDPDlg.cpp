
// mfcUDPDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "mfcUDP.h"
#include "mfcUDPDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#define UM_SOCK WM_USER+100

// CmfcUDPDlg 对话框



CmfcUDPDlg::CmfcUDPDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_MFCUDP_DIALOG, pParent)
	, m_MyPort(12345)
	, m_DestPort(12346)
	, m_DestAddress(_T("127.0.0.1"))
	, m_RecvData(_T(""))
	, m_SendData(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CmfcUDPDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_RecvData);
	DDX_Text(pDX, IDC_EDIT2, m_SendData);
	DDX_Text(pDX, IDC_EDIT3, m_DestAddress);
	DDX_Text(pDX, IDC_EDIT4, m_DestPort);
	DDX_Text(pDX, IDC_EDIT5, m_MyPort);
}

BEGIN_MESSAGE_MAP(CmfcUDPDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CmfcUDPDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CmfcUDPDlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// CmfcUDPDlg 消息处理程序

BOOL CmfcUDPDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CmfcUDPDlg::OnPaint()
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
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CmfcUDPDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CmfcUDPDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	WSAData wsa;
	struct sockaddr_in local;
	int len;
	WSAStartup(0x101, &wsa);
	UpdateData(TRUE);
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock == INVALID_SOCKET) {
		MessageBox(_T("socket() failed\n"), _T("初始化"), MB_OK);
		return;
	}
	local.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	local.sin_family = AF_INET;
	local.sin_port = htons(m_MyPort);
	len = sizeof(local);
	if (bind(sock, (sockaddr*)&local, len) < 0) {
		MessageBox(_T("bind() failed\n"), _T("初始化"), MB_OK);
		return;
	}
	UpdateData(FALSE);
	WSAAsyncSelect(sock, m_hWnd, UM_SOCK, FD_CLOSE | FD_WRITE | FD_READ);
}


void CmfcUDPDlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	struct sockaddr_in remote;
	int len,remote_len;
	CString recvbuf;
	vector<CString>::iterator it;
	UpdateData(TRUE);
	USES_CONVERSION;
	char* destAddr = T2A(m_DestAddress.GetBuffer(0));
	remote.sin_addr.S_un.S_addr = inet_addr(destAddr);//htonl(INADDR_ANY);
	remote.sin_family = AF_INET;
	remote.sin_port = htons(m_DestPort);
	remote_len = sizeof(remote);

	len = m_SendData.GetLength();
	len = sendto(sock, (char*)m_SendData.GetBuffer(len * 2), len * 2 + 2, 0, (sockaddr*)&remote, remote_len);
	if (len == SOCKET_ERROR) {
		MessageBox(_T(""), _T("Sending Error"), MB_OK);
		return;
	}
	recvbuf = _T("send:") + m_SendData + _T("\r\n");
	que.push_back(recvbuf);
	if (que.size() > 20)
		que.erase(que.begin());
	recvbuf = "";
	for (it = que.begin(); it != que.end(); it++)
		recvbuf += *it;
	m_RecvData = recvbuf;
	UpdateData(FALSE);
}
LRESULT CmfcUDPDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	SOCKET s;
	int wmEvent;
	int len, remote_len;
	struct sockaddr_in remote;
	TCHAR buf[48];
	TCHAR buffer[1024];
	unsigned long retval;
	CString recvbuf;
	vector<CString>::iterator it;
	switch (message) {
	case UM_SOCK:
		s = (SOCKET)wParam;
		wmEvent = LOWORD(lParam);
		switch (wmEvent) {
		case FD_READ:
			//wsprintf(buf, _T("recvfrom()  %d\n"), s);
			//MessageBox(buf, _T(""), MB_OK);
			len = sizeof(buffer);
			remote_len = sizeof(remote);
			len = recvfrom(s, (char*)buffer, len, 0, (sockaddr*)&remote, &remote_len);
			if (len <= 0) {
				retval = WSAGetLastError();
				wsprintf(buf, _T("recvfrom() failed %u\n"), retval);
				MessageBox(buf, _T("Receiving Error"), MB_OK);
				break;
			}
			recvbuf = buffer;
			recvbuf = _T("recv:") + recvbuf + _T("\r\n");
			que.push_back(recvbuf);
			if (que.size() > 20)
				que.erase(que.begin());
			recvbuf = "";
			for (it = que.begin(); it != que.end(); it++)
				recvbuf += *it;

			UpdateData(TRUE);
			m_RecvData = recvbuf;
			UpdateData(FALSE);
			break;
		case FD_CLOSE:
			MessageBox(_T("socket closed\n"), NULL, MB_OK);
			closesocket(s);
			WSACleanup();
			break;
		default:
			break;
		}
		break;
	default:
		return CDialog::WindowProc(message, wParam, lParam);
	}
	return 0;
}