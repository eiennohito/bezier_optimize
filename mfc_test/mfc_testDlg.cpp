
// mfc_testDlg.cpp : implementation file
//

#include "stdafx.h"
#include "mfc_test.h"
#include "mfc_testDlg.h"
#include "afxdialogex.h"
#include <vector>
#include <memory>
#include <tuple>
#include "WorkObjects.h"
#include "parser.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
  ON_WM_PAINT()
  ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()


// Cmfc_testDlg dialog

static int selected_curve = 0;
static chain_storage curves[2];
static std::vector<CPoint> points[2];
static CWnd* textarea;

static struct {
  int cur;
  Point2 pts[10];
} ptHolder;

static struct {
  bool moving;
  CPoint pos;
  size_t idx;
  size_t pt_id;
} moveObj;

static struct {
  float xmin, xmax, ymin, ymax;
  float tform[6];
} bbox;

void calc_bbox() {
  auto v = curves[0].chain();
  bbox.xmin = 1e10f;
  bbox.xmax = -1e10f;
  bbox.ymin = 1e10f;
  bbox.ymax = -1e10f;
  v.for_each_fragment([](const BezierFragment& f) {
    for (int i = 0; i < 3; ++i) {
      bbox.xmin = min(f[i].x, bbox.xmin);
      bbox.xmax = max(f[i].x, bbox.xmax);
      bbox.ymin = min(f[i].y, bbox.ymin);
      bbox.ymax = max(f[i].y, bbox.ymax);
    }
  });
  
  float cx = (bbox.xmin + bbox.xmax) / 2;
  float cy = (bbox.ymin + bbox.ymax) / 2;

  CRect rect;
  textarea->GetClientRect(rect);
  auto center = rect.CenterPoint();
  float h = abs(rect.top - center.y);
  float w = abs(rect.right - center.x);

  float a = -cx;
  float b = -cy;
  float c = rect.Width() / (bbox.xmax - bbox.xmin);
  float d = rect.Height() / (bbox.ymax - bbox.ymin);
  float s = min(c, d) * 0.50;
  float f[6] = {
    s, 0, w + s * a,
    0, s, h + s * b
  };
  std::copy(f, f+6, bbox.tform);
}

Point2 tfed(const Point2& pt) {
  auto& f = bbox.tform;
  float x = pt.x * f[0] + pt.y * f[1] + f[2];
  float y = pt.x * f[3] + pt.y * f[4] + f[5];
  return Point2(x, y);
}

CPoint convert(const Point2& pt) {
  return CPoint((int)pt.x, (int)pt.y);
}

void applyMove() {

}

const float margin = 2.f;
void generate_points(chain_storage& st, std::vector<CPoint>& pts) {
  auto chain = st.chain();
  Point2 last = tfed(chain.head().p1);
  pts.push_back(convert(last));
  chain.for_each_fragment([&](const BezierFragment& fr){
    float len = fr.length() * bbox.tform[0];
    if (len < margin) {
      Point2 p = tfed(fr.p3);
      float lastdist = fabs(sqdist(p, last));
      if (lastdist > margin) {
        pts.push_back(convert(fr.p3));
      }
      return;
    }
    int n = static_cast<int>(len / 5);
    float dist = 1.0f / n;
    for (int i = 1; i <= n; ++i) {
      Point2 pt = fr.interpolate(dist * i);
      pts.push_back(convert(tfed(pt)));
    }
    //Point2 p1 = fr.p1 + (fr.p2 - fr.p1) * (2/3.0f);
    //Point2 p2 = fr.p2 + (fr.p2 - fr.p3) * (2/3.0f);
    //Point2 p3 = fr.p2;
    //pts.push_back(convert(tfed(p1)));
    //pts.push_back(convert(tfed(p2)));
    //pts.push_back(convert(tfed(p3)));
  });
}

void drawCurve(const std::vector<CPoint>& pts, CPaintDC& dc) {
  if (pts.size() != 0) {
    auto obj = dc.SelectStockObject(BLACK_PEN);
    //dc.PolyBezier(pts.data(), pts.size());
    dc.Polyline(pts.data(), pts.size());
    dc.SelectObject(obj);
  }
}

chain_storage& current_storage() {
  return curves[selected_curve];
}

void addPoint(CPoint pt) {
  ptHolder.pts[ptHolder.cur] = Point2(pt.x, pt.y);
  ptHolder.cur += 1;
  Point2 *p = ptHolder.pts;
  chain_storage& st = current_storage();
  if (st.size() == 0 && ptHolder.cur >= 3) {    
    st.push(BezierFragment(p[0], p[1], p[2]));
    ptHolder.cur -= 3;
    points[selected_curve].clear();
  } else if (st.size() != 0 && ptHolder.cur >= 2) {
    st.push(BezierFragment(st.tail(), p[0], p[1]));
    ptHolder.cur -= 2;
    points[selected_curve].clear();
  }
}

void drawPt(CPaintDC& dc, const Point2& pt, bool active) {
  const int radi = 2;
  auto p = convert(tfed(pt));
  if (active) {    
    dc.Rectangle(p.x - radi, p.y - radi, p.x + radi, p.y + radi);
  } else {
    dc.Ellipse(p.x - radi, p.y - radi, p.x + radi, p.y + radi);
  }
}

void drawPoints(CPaintDC& dc, chain_storage& st, bool active) {
  if (st.size() != 0) {
    auto chain = st.chain();
    drawPt(dc, chain.tip(), active);
    for (auto t = chain.first(); t < chain.last(); ++t) {
      auto crv = chain.at(t);
      drawPt(dc, crv.p2, active);
      drawPt(dc, crv.p3, active);
    }
  }
  
  for (int i = 0; i < ptHolder.cur; ++i) {
    drawPt(dc, ptHolder.pts[i], active);
  }
}

void calculateDistance() 
{
  if (curves[0].size() != 0 && curves[1].size() != 0) {
    std::vector<length_at_crd> pts;
    pts.reserve(100);
    auto chn = curves[0].chain();
    float len = chn.length_at_crds(5, pts);
    const BezierFragment& bf = curves[1].at(0);
    float area = bf.difference(len, pts);
    float len2 = bf.length();
    float dist = 2 * area / (len + len2);
    std::vector<float> v1, v2;
    float sqdist = curves[0].chain().sqdist(curves[1].chain(), v1, v2);
    float area2 = curves[0].chain().area(curves[1].chain(), v1, v2);
    CString s;
    s.Format(L"Area: %f\nDist: %f\nl1:%f; l2:%f\nsqdist: %f\narea2: %f\n%d-%d", 
      area, dist, len, len2, sqdist, area2, curves[0].size(), curves[1].size());
    if (textarea != 0) {
      textarea->SetDlgItemTextW(IDC_TEXTAREA, s);
    }
  }
  
}

void drawAll(CPaintDC& dc, CRect rect) {
  for (int i = 0; i < 2; ++i) {
    std::vector<CPoint>& pts = points[i];
    chain_storage& st = curves[i];
    if (pts.empty() && st.size() != 0) {
      generate_points(st, pts);
      calculateDistance();
    }
    drawCurve(pts, dc);
    bool active = i == selected_curve;
    CPen pen(PS_COSMETIC, 1, active ? RGB(255, 0, 0) : RGB(0, 0, 255));
    auto obj = dc.SelectObject(pen);
    drawPoints(dc, st, active);
    dc.SelectObject(obj);
  }

  if (moveObj.moving) {
    CPen pen(PS_COSMETIC, 1, RGB(0, 255, 0));
    auto obj = dc.SelectObject(pen);
    auto &pt = moveObj.pos;
    dc.Ellipse(pt.x - 3, pt.y - 3, pt.x + 3, pt.y + 3);
    dc.SelectObject(obj);
  }
}

std::pair<int, int> findNearestPt(const chain_storage& stor, const CPoint& pt) {  
  auto sz = stor.size();
  int idx = -1, pos = 0;
  Point2 p1(pt.x, pt.y);
  float minv = 1e10;
  for (size_t i = 0; i < sz; ++i) {
    const auto& frag = stor.at(i);
    for (int ps = 0; ps < 3; ++ps) {
      const Point2 &p2 = frag[ps];
      float dist = sqdist(tfed(p1), tfed(p2));
      if (dist < minv) {
        idx = i;
        pos = ps;
        minv = dist;
      }
    }
  }
  return std::pair<int, int>(idx, pos);
}

void handleStartDrag(CPoint st) {
  int idx, pos;
  const auto& curve = curves[selected_curve];
  std::tie(idx, pos) = findNearestPt(curve, st);
  if (idx == -1) { return; }
  const Point2& pt = curve.at(idx)[pos];
  Point2 o(st.x, st.y);
  auto dist = sqdist(o, pt);
  if (dist < 30) {
    moveObj.moving = true;
    moveObj.pos = st;
    moveObj.idx = idx;
    moveObj.pt_id = pos;
  }
}

void handleEndDrag(CPoint pt) {
  if (moveObj.moving) {
    auto& curve = curves[selected_curve];
    moveObj.moving = false;
    Point2 p(pt.x, pt.y);
    switch(moveObj.pt_id) {
    case 0:
      if (moveObj.idx != 0) {
        curve.at(moveObj.idx - 1).p3 = p;        
      }
      break;
    case 2:
      if (moveObj.idx != curve.size() - 1) {
        curve.at(moveObj.idx + 1).p1 = p;
      }
      break;
    }
    curve.at(moveObj.idx)[moveObj.pt_id] = p;
    points[selected_curve].clear();
  }
}


Cmfc_testDlg::Cmfc_testDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(Cmfc_testDlg::IDD, pParent)
{
  m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
  checkedIdx = 0;
  input = _T("");
  simplPer = 5.0;
}

void Cmfc_testDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialogEx::DoDataExchange(pDX);
  DDX_Radio(pDX, IDC_RADIO1, checkedIdx);
  DDX_Text(pDX, IDC_EDIT1, input);
  DDX_Text(pDX, IDC_EDIT2, simplPer);
}

BEGIN_MESSAGE_MAP(Cmfc_testDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
  ON_BN_CLICKED(IDC_RADIO2, &Cmfc_testDlg::OnBnClickedRadio2)
  ON_BN_CLICKED(IDC_RADIO1, &Cmfc_testDlg::OnBnClickedRadio1)
  ON_WM_MOUSEMOVE()
  ON_WM_LBUTTONDBLCLK()
  ON_WM_LBUTTONDOWN()
  ON_WM_LBUTTONUP()
  ON_BN_CLICKED(IDC_CRAPPBTN, &Cmfc_testDlg::OnBnClickedCrappbtn)
  ON_BN_CLICKED(IDC_GDAPPXBTN, &Cmfc_testDlg::OnBnClickedGdappxbtn)
  ON_BN_CLICKED(IDC_BUTTON1, &Cmfc_testDlg::LoadClicked)
  ON_BN_CLICKED(IDC_SIMPLIFY1, &Cmfc_testDlg::OnBnClickedSimplify1)
END_MESSAGE_MAP()


// Cmfc_testDlg message handlers

BOOL Cmfc_testDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
  bbox.tform[0] = 1;
  bbox.tform[4] = 1;


	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
  textarea = this;

  ptHolder.cur = 0;
    

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void Cmfc_testDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void Cmfc_testDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
    CPaintDC dc(this);
    CRect rect;
    GetDlgItem(IDC_DRAWING)->GetClientRect(rect);
    drawAll(dc, rect);
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR Cmfc_testDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void Cmfc_testDlg::OnBnClickedRadio2()
{ 
  selected_curve = 1;
  redrawDrawing();
}


void Cmfc_testDlg::OnBnClickedRadio1()
{
  selected_curve = 0;
  redrawDrawing();
}


void Cmfc_testDlg::OnMouseMove(UINT nFlags, CPoint point)
{
  if (moveObj.moving) {
    moveObj.pos = point;
    redrawDrawing();
  }  

  CDialogEx::OnMouseMove(nFlags, point);
}


void Cmfc_testDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
  CRect rect;
  
  ::GetClientRect(GetDlgItem(IDC_DRAWING)->GetSafeHwnd(), rect);
  if (rect.PtInRect(point)) {
    addPoint(point);
  }

  InvalidateRect(rect);

  CDialogEx::OnLButtonDblClk(nFlags, point);
}


void Cmfc_testDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
  handleStartDrag(point);

  CDialogEx::OnLButtonDown(nFlags, point);
}


void Cmfc_testDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
  handleEndDrag(point);
  redrawDrawing();

  CDialogEx::OnLButtonUp(nFlags, point);
}


void Cmfc_testDlg::redrawDrawing(void) const
{
  CRect rect;
  GetDlgItem(IDC_DRAWING)->GetClientRect(rect);
  ::InvalidateRect(this->GetSafeHwnd(), rect, true);
}


void Cmfc_testDlg::OnBnClickedCrappbtn()
{
  if (curves[selected_curve].size() >= 2) {
    auto bf = curves[selected_curve].chain().crude_appx();
    auto & o = curves[selected_curve == 0? 1 : 0];
    o.clear();
    o.push(bf);    
    redrawDrawing();
  }
}


void Cmfc_testDlg::OnBnClickedGdappxbtn()
{
  std::vector<float> v1, v2;
  if (curves[0].size() >= 2) {
    curves[0].chain().gd_appx(curves[1], v1, v2);
    points[1].clear();
    redrawDrawing();
  }
}


void Cmfc_testDlg::LoadClicked()
{
  this->UpdateData(TRUE);
  std::string inp;  
  curves[0].clear();
  std::copy(input.GetString(), input.GetString() + input.GetLength(), std::back_inserter(inp));
  parse_string(inp, curves[0].data());
  points[0].clear();
  calc_bbox();
  redrawDrawing();
}


void Cmfc_testDlg::OnBnClickedSimplify1()
{
  if (curves[0].size() >= 2) {
    UpdateData(TRUE);
    curves[1].clear();
    points[1].clear();
    std::vector<float> v1, v2;
    curves[0].chain().simplify_gd(curves[1], (float)simplPer, v1, v2);
    redrawDrawing();
  }
}
