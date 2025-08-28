
// CircleMakerDlg.h: 헤더 파일
//

#pragma once

#include <thread>

#define WM_MOVE_RANDOM	(WM_USER + 1)
#define WM_STOP_RANDOM	(WM_USER + 2)

class CircleGroup;
class Circle;

// CCircleMakerDlg 대화 상자
class CCircleMakerDlg : public CDialogEx
{
// 생성입니다.
public:
	CCircleMakerDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CIRCLEMAKER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedBtnInit();
	afx_msg void OnBnClickedBtnMove();
	afx_msg LRESULT OnMoveRandomMessage(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnStopRandomMessage(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

private:
	void InitDrawingArea();
	void ClearDrawingArea();
	void DrawCircle(unsigned char* fm, int x, int y, int radius, int color);
	void DrawBlankCircle(unsigned char* fm, int x, int y, int radius, int borderColor);
	BOOL IsInCircle(int x, int y, int nCenterX, int nCenterY, int radius);
	BOOL IsInCircleBorder(int x, int y, int nCenterX, int nCenterY, int radius);
	void GetRandomPoint(int* x, int* y);
	
	BOOL StartRandomMoveThread();
	void StopRandomMoveThread();

private:
	CImage m_drawingArea;
	CRect m_drawingAreaRect;
	CircleGroup* circleGroup;
	Circle* circumCircle;
	bool isMoving;

public:
	std::thread m_worker;
	bool m_stopWorker;
};
