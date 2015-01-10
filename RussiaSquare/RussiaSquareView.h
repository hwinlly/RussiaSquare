#pragma once

#define GAMEVIEW_BACKGROUND RGB(200,220,200)

class CRussiaSquareView : public CView
{
protected: // �������л�����
	CRussiaSquareView();
	DECLARE_DYNCREATE(CRussiaSquareView)

// ����
public:
	CRussiaSquareDoc* GetDocument() const;

// ����
protected:
	int m_nGridWidth;
	int m_nGridHeight;
	int m_nGridHSpace;
	int m_nGridVSpace;
	int m_nViewWidth;
	int m_nViewHeight;
	int m_nDeleteIndex;
	CDC *m_pMemDC;
	CBitmap *m_pBitmap;

	UINT_PTR m_nDELTimer;
	CWinThread* m_pGameThread;

public:
	void FallDownProcess();

// ��д
public:
	virtual ~CRussiaSquareView();
	virtual void OnDraw(CDC* pDC);  // ��д�Ի��Ƹ���ͼ
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnUpdate(CView* pSender,LPARAM lHint,CObject* pHint);

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif


// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnGameNew();
	afx_msg void OnGamePause();
	afx_msg void OnGameStop();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
};

#ifndef _DEBUG  // RussiaSquareView.cpp �ĵ��԰汾
inline CRussiaSquareDoc* CRussiaSquareView::GetDocument() const
   { return reinterpret_cast<CRussiaSquareDoc*>(m_pDocument); }
#endif

