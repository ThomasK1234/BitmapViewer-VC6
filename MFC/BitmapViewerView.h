// BitmapViewerView.h : interface of the CBitmapViewerView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_BITMAPVIEWERVIEW_H__22334455_6677_8899_AABB_CCDDEEEFF112__INCLUDED_)
#define AFX_BITMAPVIEWERVIEW_H__22334455_6677_8899_AABB_CCDDEEEFF112__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HitRect.h"

class CBitmapViewerView : public CView
{
protected: // create from serialization only
	CBitmapViewerView();
	DECLARE_DYNCREATE(CBitmapViewerView)

// Attributes
public:
	CBitmapViewerDoc* GetDocument();

protected:
	HBITMAP m_hBitmap;
	int m_nBitmapWidth;
	int m_nBitmapHeight;
	
	// Verschiebung und Skalierung
	float m_fOffsetX;
	float m_fOffsetY;
	float m_fScale;
	
	// Hit-Test Rechtecke
	HitRect m_hitRects[MAX_HIT_RECTS];
	int m_nNumHitRects;
	
	// Drag-Unterstützung
	BOOL m_bDragging;
	int m_nLastMouseX;
	int m_nLastMouseY;

// Operations
public:
	HBITMAP LoadBitmapFromFile(const char* filename);
	void AddHitRect(int x1, int y1, int x2, int y2, int id, const char* name);
	int CheckHitRect(float absX, float absY);
	void OnClickCallback(int absX, int absY, float relX, float relY, int hitRectId);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBitmapViewerView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CBitmapViewerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CBitmapViewerView)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in BitmapViewerView.cpp
inline CBitmapViewerDoc* CBitmapViewerView::GetDocument()
   { return (CBitmapViewerDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BITMAPVIEWERVIEW_H__22334455_6677_8899_AABB_CCDDEEEFF112__INCLUDED_)
