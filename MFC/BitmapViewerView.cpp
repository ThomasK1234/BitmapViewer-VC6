// BitmapViewerView.cpp : implementation of the CBitmapViewerView class
//

#include "stdafx.h"
#include "BitmapViewer.h"
#include "BitmapViewerDoc.h"
#include "BitmapViewerView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBitmapViewerView

IMPLEMENT_DYNCREATE(CBitmapViewerView, CView)

BEGIN_MESSAGE_MAP(CBitmapViewerView, CView)
	//{{AFX_MSG_MAP(CBitmapViewerView)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEWHEEL()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBitmapViewerView construction/destruction

CBitmapViewerView::CBitmapViewerView()
{
	m_hBitmap = NULL;
	m_nBitmapWidth = 0;
	m_nBitmapHeight = 0;
	m_fOffsetX = 0.0f;
	m_fOffsetY = 0.0f;
	m_fScale = 1.0f;
	m_nNumHitRects = 0;
	m_bDragging = FALSE;
	m_nLastMouseX = 0;
	m_nLastMouseY = 0;
}

CBitmapViewerView::~CBitmapViewerView()
{
	if (m_hBitmap) {
		DeleteObject(m_hBitmap);
	}
}

BOOL CBitmapViewerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CBitmapViewerView drawing

void CBitmapViewerView::OnDraw(CDC* pDC)
{
	CBitmapViewerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// Weiß-Hintergrund füllen
	CRect rect;
	GetClientRect(&rect);
	pDC->FillSolidRect(&rect, RGB(255, 255, 255));

	// Bitmap zeichnen, falls vorhanden
	if (m_hBitmap && m_nBitmapWidth > 0 && m_nBitmapHeight > 0) {
		CDC memDC;
		memDC.CreateCompatibleDC(pDC);
		HBITMAP hOldBitmap = (HBITMAP)::SelectObject(memDC.m_hDC, m_hBitmap);

		// Skalierte Bitmap zeichnen
		int scaledWidth = (int)(m_nBitmapWidth * m_fScale);
		int scaledHeight = (int)(m_nBitmapHeight * m_fScale);

		::StretchBlt(pDC->m_hDC,
					 (int)m_fOffsetX, (int)m_fOffsetY,
					 scaledWidth, scaledHeight,
					 memDC.m_hDC, 0, 0,
					 m_nBitmapWidth, m_nBitmapHeight,
					 SRCCOPY);

		::SelectObject(memDC.m_hDC, hOldBitmap);
	}

	// Hit-Rects zeichnen (rote gestrichelte Linien)
	CPen penRed(PS_DASH, 1, RGB(255, 0, 0));
	CPen* pOldPen = pDC->SelectObject(&penRed);

	for (int i = 0; i < m_nNumHitRects; i++) {
		// Transformation: Bildkoordinaten zu Fensterkoordinaten
		int x1 = (int)(m_hitRects[i].x1 * m_fScale + m_fOffsetX);
		int y1 = (int)(m_hitRects[i].y1 * m_fScale + m_fOffsetY);
		int x2 = (int)(m_hitRects[i].x2 * m_fScale + m_fOffsetX);
		int y2 = (int)(m_hitRects[i].y2 * m_fScale + m_fOffsetY);

		pDC->Rectangle(x1, y1, x2, y2);
	}

	pDC->SelectObject(pOldPen);
}

/////////////////////////////////////////////////////////////////////////////
// CBitmapViewerView printing

BOOL CBitmapViewerView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CBitmapViewerView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CBitmapViewerView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CBitmapViewerView diagnostics

#ifdef _DEBUG
void CBitmapViewerView::AssertValid() const
{
	CView::AssertValid();
}

void CBitmapViewerView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CBitmapViewerDoc* CBitmapViewerView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CBitmapViewerDoc)));
	return (CBitmapViewerDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CBitmapViewerView message handlers

HBITMAP CBitmapViewerView::LoadBitmapFromFile(const char* filename)
{
	HANDLE hFile = ::CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ,
								 NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE) {
		return NULL;
	}

	BITMAPFILEHEADER bmfh;
	BITMAPINFOHEADER bmih;
	DWORD bytesRead;

	// BITMAPFILEHEADER lesen
	if (!ReadFile(hFile, &bmfh, sizeof(BITMAPFILEHEADER), &bytesRead, NULL)) {
		CloseHandle(hFile);
		return NULL;
	}

	// BITMAPINFOHEADER lesen
	if (!ReadFile(hFile, &bmih, sizeof(BITMAPINFOHEADER), &bytesRead, NULL)) {
		CloseHandle(hFile);
		return NULL;
	}

	// Prüfe ob es eine gültige BMP-Datei ist
	if (bmfh.bfType != 0x4D42) { // 'BM'
		CloseHandle(hFile);
		return NULL;
	}

	// Berechne die Größe der Pixel-Daten
	int imageSize = bmfh.bfSize - bmfh.bfOffBits;
	unsigned char* pBits = new unsigned char[imageSize];

	// Setze Dateiposition auf die Pixeldaten
	::SetFilePointer(hFile, bmfh.bfOffBits, NULL, FILE_BEGIN);

	// Pixeldaten lesen
	if (!ReadFile(hFile, pBits, imageSize, &bytesRead, NULL)) {
		delete[] pBits;
		CloseHandle(hFile);
		return NULL;
	}

	CloseHandle(hFile);

	// DIB (Device Independent Bitmap) erstellen
	HDC hDC = ::GetDC(NULL);
	HBITMAP hBmp = ::CreateDIBitmap(hDC, &bmih, CBM_INIT, pBits,
									  (BITMAPINFO*)&bmih, DIB_RGB_COLORS);
	::ReleaseDC(NULL, hDC);

	delete[] pBits;

	return hBmp;
}

void CBitmapViewerView::AddHitRect(int x1, int y1, int x2, int y2, int id, const char* name)
{
	if (m_nNumHitRects < MAX_HIT_RECTS) {
		m_hitRects[m_nNumHitRects].x1 = min(x1, x2);
		m_hitRects[m_nNumHitRects].y1 = min(y1, y2);
		m_hitRects[m_nNumHitRects].x2 = max(x1, x2);
		m_hitRects[m_nNumHitRects].y2 = max(y1, y2);
		m_hitRects[m_nNumHitRects].id = id;
		strcpy(m_hitRects[m_nNumHitRects].name, name);
		m_nNumHitRects++;
	}
}

int CBitmapViewerView::CheckHitRect(float absX, float absY)
{
	for (int i = 0; i < m_nNumHitRects; i++) {
		if (absX >= m_hitRects[i].x1 && absX <= m_hitRects[i].x2 &&
			absY >= m_hitRects[i].y1 && absY <= m_hitRects[i].y2) {
			return m_hitRects[i].id;
		}
	}
	return -1; // Kein Rechteck getroffen
}

void CBitmapViewerView::OnClickCallback(int absX, int absY, float relX, float relY, int hitRectId)
{
	CString strMessage;

	if (hitRectId != -1) {
		// Finde den Namen des getroffenen Rechtecks
		const char* rectName = "";
		for (int i = 0; i < m_nNumHitRects; i++) {
			if (m_hitRects[i].id == hitRectId) {
				rectName = m_hitRects[i].name;
				break;
			}
		}

		strMessage.Format(
			"Klick auf: %s\n"
			"Absolute Koordinaten: (%d, %d)\n"
			"Relative Koordinaten: (%.1f, %.1f)\n"
			"Skalierung: %.2f\n"
			"Hit-Rect ID: %d",
			rectName, absX, absY, relX, relY, m_fScale, hitRectId);
	} else {
		strMessage.Format(
			"Klick auf freie Fläche\n"
			"Absolute Koordinaten: (%d, %d)\n"
			"Relative Koordinaten: (%.1f, %.1f)\n"
			"Skalierung: %.2f\n"
			"Kein Hit-Rect getroffen",
			absX, absY, relX, relY, m_fScale);
	}

	// Debug-Ausgabe
	printf("%s\n", (const char*)(LPCTSTR)strMessage);
	
	// In Statusleiste anzeigen
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	if (pMainFrame) {
		pMainFrame->m_wndStatusBar.SetPaneText(0, strMessage);
	}
}

void CBitmapViewerView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// Fensterkoordinaten in Bildkoordinaten umwandeln
	float imageX = (point.x - m_fOffsetX) / m_fScale;
	float imageY = (point.y - m_fOffsetY) / m_fScale;

	// Prüfe Hit-Rects
	int hitId = CheckHitRect(imageX, imageY);

	// Callback aufrufen
	OnClickCallback((int)imageX, (int)imageY, imageX, imageY, hitId);

	CView::OnLButtonDown(nFlags, point);
}

void CBitmapViewerView::OnMouseMove(UINT nFlags, CPoint point)
{
	// Verschiebung mit rechter Maustaste
	if (nFlags & MK_RBUTTON) {
		if (m_bDragging) {
			int deltaX = point.x - m_nLastMouseX;
			int deltaY = point.y - m_nLastMouseY;

			m_fOffsetX += deltaX;
			m_fOffsetY += deltaY;

			InvalidateRect(NULL, FALSE);
		}
		m_nLastMouseX = point.x;
		m_nLastMouseY = point.y;
	}

	CView::OnMouseMove(nFlags, point);
}

void CBitmapViewerView::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bDragging = FALSE;
	CView::OnLButtonUp(nFlags, point);
}

BOOL CBitmapViewerView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// Fensterkoordinaten in Bildkoordinaten umwandeln (vor Skalierung)
	float oldScale = m_fScale;
	float imageX = (pt.x - m_fOffsetX) / oldScale;
	float imageY = (pt.y - m_fOffsetY) / oldScale;

	// Skalierungsfaktor berechnen (delta ist normalerweise ±120)
	float scaleChange = 1.0f + (zDelta / 1200.0f);
	m_fScale = m_fScale * scaleChange;

	// Skalierung begrenzen (0.1 bis 5.0)
	if (m_fScale < 0.1f) m_fScale = 0.1f;
	if (m_fScale > 5.0f) m_fScale = 5.0f;

	// Offset anpassen, so dass der Punkt unter der Maus gleich bleibt
	m_fOffsetX = pt.x - imageX * m_fScale;
	m_fOffsetY = pt.y - imageY * m_fScale;

	InvalidateRect(NULL, FALSE);

	return CView::OnMouseWheel(nFlags, zDelta, pt);
}

void CBitmapViewerView::OnRButtonDown(UINT nFlags, CPoint point)
{
	m_bDragging = TRUE;
	m_nLastMouseX = point.x;
	m_nLastMouseY = point.y;
	CView::OnRButtonDown(nFlags, point);
}
