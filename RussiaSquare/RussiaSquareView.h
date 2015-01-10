#pragma once

#define GAMEVIEW_BACKGROUND RGB(200,220,200)

class CRussiaSquareView : public CView
{
protected: // 仅从序列化创建
	CRussiaSquareView();
	DECLARE_DYNCREATE(CRussiaSquareView)

// 操作
public:
	CRussiaSquareDoc* GetDocument() const;

// 属性
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

// 重写
public:
	virtual ~CRussiaSquareView();
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnUpdate(CView* pSender,LPARAM lHint,CObject* pHint);

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif


// 生成的消息映射函数
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

#ifndef _DEBUG  // RussiaSquareView.cpp 的调试版本
inline CRussiaSquareDoc* CRussiaSquareView::GetDocument() const
   { return reinterpret_cast<CRussiaSquareDoc*>(m_pDocument); }
#endif

