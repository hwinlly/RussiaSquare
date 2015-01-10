// RussiaSquareView.cpp : CRussiaSquareView 类的实现
//

#include "stdafx.h"
#include "RussiaSquare.h"
#include "RussiaSquareDoc.h"
#include "RussiaSquareView.h"
#include "afxmt.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

UINT GameThreadProc( LPVOID pParam);
// CRussiaSquareView

IMPLEMENT_DYNCREATE(CRussiaSquareView, CView)

BEGIN_MESSAGE_MAP(CRussiaSquareView, CView)
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_KEYDOWN()
	ON_COMMAND(ID_GAME_NEW, OnGameNew)
	ON_COMMAND(ID_GAME_PAUSE, OnGamePause)
	ON_COMMAND(ID_GAME_STOP, OnGameStop)
	ON_WM_KILLFOCUS()
END_MESSAGE_MAP()

// CRussiaSquareView 构造/析构

CRussiaSquareView::CRussiaSquareView()
{
	m_nGridWidth = 15;
	m_nGridHeight = 15;
	m_nGridHSpace = 0;
	m_nGridVSpace = 0;
	m_pGameThread = NULL;
	m_pMemDC = new CDC;
	m_pBitmap = new CBitmap;
}

CRussiaSquareView::~CRussiaSquareView()
{
	m_pBitmap->DeleteObject();
	m_pMemDC->DeleteDC();
	delete m_pBitmap;
	delete m_pMemDC;
}

BOOL CRussiaSquareView::PreCreateWindow(CREATESTRUCT& cs)
{
	return CView::PreCreateWindow(cs);
}

CCriticalSection g_criticalsection;
// CRussiaSquareView 绘制

void CRussiaSquareView::OnDraw(CDC* pDC)
{
	g_criticalsection.Lock();
	pDC->BitBlt(0, 0, m_nViewWidth, m_nViewHeight, m_pMemDC, 0, 0, SRCCOPY);
	g_criticalsection.Unlock();
}

void CRussiaSquareView::OnUpdate(CView* pSender,LPARAM lHint,CObject* pHint)
{
	CRussiaSquareDoc* pDoc = GetDocument();
	RECT rc;

	int matrixWidth  = pDoc->GetMatrixWidth();
	int matrixHeight = pDoc->GetMatrixHeight();

	g_criticalsection.Lock();

	m_pMemDC->FillSolidRect(0,0,m_nViewWidth,m_nViewHeight,GAMEVIEW_BACKGROUND);

	for(int i = 0; i < matrixWidth; i++) 
	{
		for(int j = 0; j < matrixHeight; j++) 
		{
			rc.left = i*(m_nGridWidth + m_nGridHSpace);
			rc.right = rc.left + m_nGridWidth;
			rc.top = j*(m_nGridHeight + m_nGridVSpace);
			rc.bottom = rc.top + m_nGridHeight;
			m_pMemDC->FillSolidRect(&rc,pDoc->GetGridColor(i,j));
		}
	}
	rc.left = rc.top = 0;
	m_pMemDC->DrawEdge(&rc,BDR_RAISEDINNER,BF_BOTTOMRIGHT);

	int rightline = rc.right;

	GameStatus  gs = pDoc->GetGameStatus();

	if(GAME_STOP != gs)
	{
		rightline += 5;
		m_pMemDC->SetBkColor(GAMEVIEW_BACKGROUND);
		if(GAME_PAUSE == gs)
		{
			COLORREF oldPencolor = m_pMemDC->SetTextColor(RGB(255,20,20));
			m_pMemDC->TextOut(rightline,10,"PAUSED");
			m_pMemDC->SetTextColor(oldPencolor);
		}
		CString str;
		str.AppendFormat("Score: %d",pDoc->GetGameScore());
		m_pMemDC->TextOut(rightline,40,str);
		str.Empty();
		int level = pDoc->GetGameLevel() + 1;
		str.AppendFormat("Level: %d", level);
		m_pMemDC->TextOut(rightline,60,str);

		const SquareBlock* pBlock = pDoc->GetNextSquareBlock();

		int centerx = pBlock->GetCenterX();
		int centery = pBlock->GetCenterY();
		COLORREF blockColor = pBlock->GetBlockColor();

		int x,y;
		for(int i = 0; i < 4; i++)
		{
			x = pBlock->GetX(i) - centerx;
			y = pBlock->GetY(i) - centery;

			rc.left   = 10 + (matrixWidth + 2 + x)*(m_nGridWidth + m_nGridHSpace);
			rc.right  = rc.left + m_nGridWidth;
			rc.top    = 100 +  (2 + y)*(m_nGridHeight + m_nGridVSpace);
			rc.bottom = rc.top + m_nGridHeight;
			m_pMemDC->FillSolidRect(&rc,blockColor);
		}
	}
	g_criticalsection.Unlock();
	return CView::OnUpdate(pSender,lHint,pHint);
}

// CRussiaSquareView 诊断

#ifdef _DEBUG
void CRussiaSquareView::AssertValid() const
{
	CView::AssertValid();
}

void CRussiaSquareView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CRussiaSquareDoc* CRussiaSquareView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CRussiaSquareDoc)));
	return (CRussiaSquareDoc*)m_pDocument;
}
#endif //_DEBUG

// CRussiaSquareView 消息处理程序

void CRussiaSquareView::FallDownProcess()
{
	CRussiaSquareDoc* pDoc = GetDocument();

	g_criticalsection.Lock();
	pDoc->ClearSquareBlock();
	int n = pDoc->FallSquareBlock();
	g_criticalsection.Unlock();
	if(n >= 0)
	{
		if(n > 0)
		{
			m_nDeleteIndex = 1;
			m_nDELTimer = SetTimer(321,60,NULL);
			g_criticalsection.Lock();
			pDoc->UpdateAllViews(NULL,0,NULL);
			g_criticalsection.Unlock();
			return;
		}
		pDoc->DeleteLines();
	}
	g_criticalsection.Lock();
	pDoc->UpdateAllViews(NULL,0,NULL);
	g_criticalsection.Unlock();
}

void CRussiaSquareView::OnTimer(UINT nIDEvent)
{
	CRussiaSquareDoc* pDoc = GetDocument();

	if(m_nDeleteIndex < (int)pDoc->GetMatrixWidth())
	{
		pDoc->MarkDeleteLines(m_nDeleteIndex);
		m_nDeleteIndex++;
	}
	else
	{
		KillTimer(m_nDELTimer);
		pDoc->DeleteLines();
	}
	pDoc->UpdateAllViews(NULL,0,NULL);
	CView::OnTimer(nIDEvent);
}

int CRussiaSquareView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRussiaSquareDoc* pDoc = GetDocument();

	m_nViewWidth = (pDoc->GetMatrixWidth() + 15)*(m_nGridWidth + m_nGridHSpace);
	m_nViewHeight = (pDoc->GetMatrixHeight() + 1)*(m_nGridHeight + m_nGridVSpace);

	CDC* pDC = this->GetDC();
	m_pMemDC->CreateCompatibleDC(NULL);
	m_pBitmap->CreateCompatibleBitmap(pDC,m_nViewWidth, m_nViewHeight);
	m_pMemDC->SelectObject(m_pBitmap);
	ReleaseDC(pDC);
	return 0;
}

BOOL CRussiaSquareView::OnEraseBkgnd(CDC* pDC)
{
	RECT rc;
	g_criticalsection.Lock();
	GetClientRect(&rc);
	rc.left += m_nViewWidth;
	pDC->FillSolidRect(&rc,GAMEVIEW_BACKGROUND);
	rc.left -= m_nViewWidth;
	rc.top += m_nViewHeight;
	rc.right = m_nViewWidth;
	pDC->FillSolidRect(&rc,GAMEVIEW_BACKGROUND);
	g_criticalsection.Unlock();
	return TRUE;
}

void CRussiaSquareView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CRussiaSquareDoc* pDoc = GetDocument();
	if(GAME_START == pDoc->GetGameStatus())
	{
		switch(nChar)
		{
		case VK_UP:
			g_criticalsection.Lock();
			pDoc->ClearSquareBlock();
			pDoc->TransferSquareBlock();
			pDoc->UpdateAllViews(NULL,0,NULL);
			g_criticalsection.Unlock();
			break;
		case VK_LEFT:
			g_criticalsection.Lock();
			pDoc->ClearSquareBlock();
			pDoc->LeftMoveSquareBlock();
			pDoc->UpdateAllViews(NULL,0,NULL);
			g_criticalsection.Unlock();
			break;
		case VK_RIGHT:
			g_criticalsection.Lock();
			pDoc->ClearSquareBlock();
			pDoc->RightMoveSquareBlock();
			pDoc->UpdateAllViews(NULL,0,NULL);
			g_criticalsection.Unlock();
			break;
		case VK_DOWN:
			FallDownProcess();
			break;
		default:
			break;
		}
	}
	
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

static BOOL isGameThreadSuspended = FALSE;

void CRussiaSquareView::OnGameNew()
{
	CRussiaSquareDoc* pDoc = GetDocument();
	pDoc->SetGameStatus(GAME_START);
	int level = pDoc->GetGameLevel() + 1;
	if(NULL == m_pGameThread)
	{
		m_pGameThread = AfxBeginThread(GameThreadProc, this);
	}
	else if(isGameThreadSuspended)
	{
		m_pGameThread->ResumeThread();
		isGameThreadSuspended = FALSE;
	}
}
	

void CRussiaSquareView::OnGamePause()
{
	CRussiaSquareDoc* pDoc = GetDocument();
	
	pDoc->SetGameStatus(GAME_PAUSE);
	GameStatus status = pDoc->GetGameStatus();
	if(GAME_PAUSE == status)
	{
		if(!isGameThreadSuspended)
		{
			m_pGameThread->SuspendThread();
			isGameThreadSuspended = TRUE;
		}
			
	}
	else if(GAME_START == status)
	{
		if(isGameThreadSuspended)
		{
			m_pGameThread->ResumeThread();
			isGameThreadSuspended = FALSE;
		}
	}
}

void CRussiaSquareView::OnGameStop()
{
	CRussiaSquareDoc* pDoc = GetDocument();
	pDoc->SetGameStatus(GAME_STOP);
	if(!isGameThreadSuspended)
	{
		m_pGameThread->SuspendThread();
		isGameThreadSuspended = TRUE;
	}
}

UINT GameThreadProc( LPVOID pParam)
{
	CRussiaSquareView* pView = (CRussiaSquareView*)pParam;
	CRussiaSquareDoc* pDoc = pView->GetDocument();

	int level = pDoc->GetGameLevel() + 1;
	GameStatus status = pDoc->GetGameStatus();

	while(TRUE)
	{
		level = pDoc->GetGameLevel() + 1;
		
		Sleep(1000/level);

		status = pDoc->GetGameStatus();
		if(GAME_START == status)
		{
			pView->FallDownProcess();
		}
	}
	return 0;
}

void CRussiaSquareView::OnKillFocus(CWnd* pNewWnd)
{
	CView::OnKillFocus(pNewWnd);

	CRussiaSquareDoc* pDoc = GetDocument();

	GameStatus status = pDoc->GetGameStatus();
	if(GAME_START == status)
	{
		OnGamePause();
	}
}
