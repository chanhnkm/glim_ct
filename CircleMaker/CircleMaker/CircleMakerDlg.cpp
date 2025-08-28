
// CircleMakerDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "CircleMaker.h"
#include "CircleMakerDlg.h"
#include "afxdialogex.h"

#include "CircleGroup.h"
#include "Circle.h"

#include <random>

#define BORDER 3

#ifdef _DEBUG
#define new DEBUG_NEW
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
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


// CCircleMakerDlg 대화 상자



CCircleMakerDlg::CCircleMakerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CIRCLEMAKER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	circleGroup = NULL;
	circumCircle = NULL;
	isMoving = false;
	m_stopWorker = false;
}

void CCircleMakerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CCircleMakerDlg, CDialogEx)
	ON_WM_CLOSE()
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_QUERYDRAGICON()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_BN_CLICKED(IDC_BTN_INIT, &CCircleMakerDlg::OnBnClickedBtnInit)
	ON_BN_CLICKED(IDC_BTN_MOVE, &CCircleMakerDlg::OnBnClickedBtnMove)
	ON_MESSAGE(WM_MOVE_RANDOM, &CCircleMakerDlg::OnMoveRandomMessage)
	ON_MESSAGE(WM_STOP_RANDOM, &CCircleMakerDlg::OnStopRandomMessage)
END_MESSAGE_MAP()


// CCircleMakerDlg 메시지 처리기

BOOL CCircleMakerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
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

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	SetDlgItemInt(IDC_EDIT_RADIUS, 10);
	InitDrawingArea();
	circleGroup = new CircleGroup;

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CCircleMakerDlg::OnClose()
{
	StopRandomMoveThread();

	if (circumCircle != NULL) {
		delete circumCircle;
	}
	if (circleGroup != NULL) {
		delete circleGroup;
	}

	CDialogEx::OnClose();
}

void CCircleMakerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CCircleMakerDlg::OnPaint()
{
	CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.
	if (IsIconic())
	{
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();

		ClearDrawingArea();

		unsigned char* fm = (unsigned char*)m_drawingArea.GetBits();

		int count = circleGroup->GetCount();
		if (count >= 3 && circumCircle != NULL) {
			DrawBlankCircle(fm, circumCircle->GetX(), circumCircle->GetY(), circumCircle->GetRadius(), 0);
		}

		Circle* circle;
		int color;
		int i = 0;
		while (i < count) {
			circle = circleGroup->GetAt(i);
			color = 0;
			if (circleGroup->GetSelected() == i) {
				color = 128;
			}
			DrawCircle(fm, circle->GetX(), circle->GetY(), circle->GetRadius(), color);
			i++;
		}

		m_drawingArea.Draw(dc.m_hDC, m_drawingAreaRect.left, m_drawingAreaRect.top);
		CBrush border(RGB(180, 180, 180));
		dc.FrameRect(&m_drawingAreaRect, &border);
	}
}

BOOL CCircleMakerDlg::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CCircleMakerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CCircleMakerDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (PtInRect(&m_drawingAreaRect, point)) {
		int x = point.x - m_drawingAreaRect.left;
		int y = point.y - m_drawingAreaRect.top;

		// 원 개수가 3개 미만이면 원을 만든다.
		int count = circleGroup->GetCount();
		if (count < 3) {
			int nRadius = GetDlgItemInt(IDC_EDIT_RADIUS);

			count = circleGroup->Add(x, y, nRadius);
			printf("x:%d | y:%d | radius:%d\n",
				circleGroup->GetAt(count - 1)->GetX(), circleGroup->GetAt(count - 1)->GetY(), circleGroup->GetAt(count - 1)->GetRadius());

			// 큰 원을 생성한다.
			circumCircle = circleGroup->MakeCircumCircle();
			if (circumCircle != NULL) {
				printf("x:%d | y:%d | radius:%d\n", circumCircle->GetX(), circumCircle->GetY(), circumCircle->GetRadius());
			}
			else {
				printf("외접원을 생성할 수 없습니다.\n");
			}
		}
		else {
			// 원 개수가 3개이고 마우스 좌표가 원 안에 있으면
			int i = 0;
			while (i < count &&
				IsInCircle(x, y,
					circleGroup->GetAt(i)->GetX(), circleGroup->GetAt(i)->GetY(), circleGroup->GetAt(i)->GetRadius()) == false) {
				i++;
			}
			if (i < count) {
				// 이동 중으로 설정한다.
				isMoving = true;
				// 원을 선택한다.
				circleGroup->Select(i);
			}
		}
		Invalidate();
	}
}

void CCircleMakerDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	if (isMoving == true && circleGroup->GetSelected() != -1) {
		int x = point.x - m_drawingAreaRect.left;
		int y = point.y - m_drawingAreaRect.top;

		Circle* circle = circleGroup->GetAt(circleGroup->GetSelected());
		circle->Move(x, y);

		if (circumCircle != NULL) {
			delete circumCircle;
		}

		circumCircle = circleGroup->MakeCircumCircle();
		if (circumCircle != NULL) {
			printf("x:%d | y:%d | radius:%d\n", circumCircle->GetX(), circumCircle->GetY(), circumCircle->GetRadius());
		}
		else {
			printf("외접원을 생성할 수 없습니다.\n");
		}

		Invalidate();
	}
}

void CCircleMakerDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	isMoving = false;
	circleGroup->Select(-1);
	Invalidate();
}

void CCircleMakerDlg::OnBnClickedBtnInit()
{
	StopRandomMoveThread();
	printf("랜덤 이동 끝\n");
	
	if (circumCircle != NULL) {
		delete circumCircle;
		circumCircle = NULL;
	}

	circleGroup->Clear();

	ClearDrawingArea();

	Invalidate();
}

void CCircleMakerDlg::OnBnClickedBtnMove()
{
	if (circumCircle != NULL) {
		BOOL isNotStarted = StartRandomMoveThread();
		if (isNotStarted == FALSE) {
			printf("랜덤 이동 시작\n");
		}
		else {
			AfxMessageBox(_T("이미 이동 중입니다."));
		}		
	}
}

LRESULT CCircleMakerDlg::OnMoveRandomMessage(WPARAM wParam, LPARAM lParam)
{
	Circle* circle;
	int x;
	int y;
	int i = 0;
	while (i < circleGroup->GetCount()) {
		circle = circleGroup->GetAt(i);
		GetRandomPoint(&x, &y);
		x = x - m_drawingAreaRect.left;
		y = y - m_drawingAreaRect.top;
		circle->Move(x, y);
		i++;
	}

	delete circumCircle;

	circumCircle = circleGroup->MakeCircumCircle();
	
	if (circumCircle != NULL) {
		printf("x:%d | y:%d | radius:%d\n", circumCircle->GetX(), circumCircle->GetY(), circumCircle->GetRadius());
	}
	else {
		printf("외접원을 생성할 수 없습니다.\n");
	}

	Invalidate();

	return 0;
}

LRESULT CCircleMakerDlg::OnStopRandomMessage(WPARAM wParam, LPARAM lParam)
{
	StopRandomMoveThread();
	printf("랜덤 이동 끝\n");
	return 0;
}

void CCircleMakerDlg::InitDrawingArea()
{
	GetClientRect(&m_drawingAreaRect);
	CRect staticRect;
	GetDlgItem(IDC_STATIC_RADIUS)->GetWindowRect(&staticRect);
	ScreenToClient(&staticRect);
	CRect editRect;
	GetDlgItem(IDC_EDIT_RADIUS)->GetWindowRect(&editRect);
	ScreenToClient(&editRect);

	m_drawingAreaRect.left = staticRect.left;
	m_drawingAreaRect.top = editRect.bottom + 10;
	m_drawingAreaRect.right -= 10;
	m_drawingAreaRect.bottom -= 10;

	int nWidth = m_drawingAreaRect.Width();
	int nHeight = m_drawingAreaRect.Height();
	int nBpp = 8;

	m_drawingArea.Create(nWidth, nHeight, nBpp);
	if (nBpp == 8) {
		static RGBQUAD rgb[256];
		for (int i = 0; i < 256; i++) {
			rgb[i].rgbRed = rgb[i].rgbGreen = rgb[i].rgbBlue = i;
		}
		m_drawingArea.SetColorTable(0, 256, rgb);
	}

	int nPitch = m_drawingArea.GetPitch();
	unsigned char* fm = (unsigned char*)m_drawingArea.GetBits();

	for (int j = 0; j < nHeight; j++) {
		for (int i = 0; i < nWidth; i++) {
			fm[j * nPitch + i] = 255;
		}
	}
}

void CCircleMakerDlg::ClearDrawingArea()
{
	int nPitch = m_drawingArea.GetPitch();
	int nWidth = m_drawingArea.GetWidth();
	int nHeight = m_drawingArea.GetHeight();

	unsigned char* fm = (unsigned char*)m_drawingArea.GetBits();

	for (int j = 0; j < nHeight; j++) {
		for (int i = 0; i < nWidth; i++) {
			fm[j * nPitch + i] = 255;
		}
	}
}

void CCircleMakerDlg::DrawCircle(unsigned char* fm, int x, int y, int radius, int color)
{
	int nPitch = m_drawingArea.GetPitch();
	int nWidth = m_drawingArea.GetWidth();
	int nHeight = m_drawingArea.GetHeight();

	int nStartX = (x - radius >= 0) ? (x - radius) : (0);
	int nStartY = (y - radius >= 0) ? (y - radius) : (0);
	int nEndX = (x + radius < nWidth) ? (x + radius) : (nWidth - 1);
	int nEndY = (y + radius < nHeight) ? (y + radius) : (nHeight - 1);

	for (int j = nStartY; j <= nEndY; j++) {
		for (int i = nStartX; i <= nEndX; i++) {
			if (IsInCircle(i, j, x, y, radius)) {
				fm[j * nPitch + i] = color;
			}
		}
	}
}

void CCircleMakerDlg::DrawBlankCircle(unsigned char* fm, int x, int y, int radius, int borderColor)
{
	int nPitch = m_drawingArea.GetPitch();
	int nWidth = m_drawingArea.GetWidth();
	int nHeight = m_drawingArea.GetHeight();

	int nStartX = (x - radius - BORDER >= 0) ? (x - radius - BORDER) : (0);
	int nStartY = (y - radius - BORDER >= 0) ? (y - radius - BORDER) : (0);
	int nEndX = (x + radius + BORDER < nWidth) ? (x + radius + BORDER) : (nWidth - 1);
	int nEndY = (y + radius + BORDER < nHeight) ? (y + radius + BORDER) : (nHeight - 1);

	for (int j = nStartY; j <= nEndY; j++) {
		for (int i = nStartX; i <= nEndX; i++) {
			if (IsInCircleBorder(i, j, x, y, radius)) {
				fm[j * nPitch + i] = borderColor;
			}
		}
	}
}

BOOL CCircleMakerDlg::IsInCircle(int x, int y, int nCenterX, int nCenterY, int radius)
{
	BOOL bRet = FALSE;

	double dX = x - nCenterX;
	double dY = y - nCenterY;
	double dDist = dX * dX + dY * dY;

	if (dDist < radius * radius) {
		bRet = TRUE;
	}

	return bRet;
}

BOOL CCircleMakerDlg::IsInCircleBorder(int x, int y, int nCenterX, int nCenterY, int radius)
{
	BOOL bRet = FALSE;

	double dX = x - nCenterX;
	double dY = y - nCenterY;
	double dDist = dX * dX + dY * dY;

	if (dDist >= (radius - BORDER) * (radius - BORDER) && dDist <= (radius + BORDER) * (radius + BORDER)) {
		bRet = TRUE;
	}

	return bRet;
}

void CCircleMakerDlg::GetRandomPoint(int* x, int* y)
{	
	static std::random_device random;
	static std::mt19937 generate(random());

	std::uniform_int_distribution<int> distX(0, m_drawingAreaRect.Width() - 1);
	std::uniform_int_distribution<int> distY(0, m_drawingAreaRect.Height() - 1);

	*x = m_drawingAreaRect.left + distX(generate);
	*y = m_drawingAreaRect.top + distY(generate);
}

void threadProcess(CWnd* pParent) {
	CCircleMakerDlg* pWnd = (CCircleMakerDlg*)pParent;
	// 초당 2회, 10번 반복
	int i = 1;
	while (i <= 10 && pWnd->m_stopWorker == false) {
		::PostMessage(pWnd->m_hWnd, WM_MOVE_RANDOM, 0, 0);
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		i++;
	}
	::PostMessage(pWnd->m_hWnd, WM_STOP_RANDOM, 0, 0);
}

BOOL CCircleMakerDlg::StartRandomMoveThread() {
	BOOL isNotStarted = m_worker.joinable();
	if (isNotStarted == FALSE) {
		m_stopWorker = false;
		m_worker = std::thread(threadProcess, this);
	}

	return isNotStarted;
}

void CCircleMakerDlg::StopRandomMoveThread()
{
	m_stopWorker = true;
	if (m_worker.joinable()) {
		m_worker.join();
	}
}